#include <filesystem>
#include <fstream>
#include <regex>
#include <string>
#include <vector>

#include "helper.h"
#include "insertLoggingCode.h"
#include "modes.h"
#include "render.h"
#include "state.h"
#include "autocomplete.h"
#include <iostream>
#include <ncurses.h>

#define _COLOR_BLACK 16
#define _COLOR_GREY 242
#define _COLOR_RED 196
#define _COLOR_GREEN 46
#define _COLOR_YELLOW 226
#define _COLOR_BLUE 21
#define _COLOR_MAGENTA 201
#define _COLOR_CYAN 51
#define _COLOR_WHITE 231
#define _COLOR_ORANGE 214
#define _COLOR_DARKGREEN 22
#define _COLOR_MANTISGREEN 113

#define STATUS_BAR_LENGTH 2

#define BLACK 1
#define GREY 2
#define RED 3
#define GREEN 4
#define YELLOW 5
#define BLUE 6
#define MAGENTA 7
#define CYAN 8
#define WHITE 9
#define ORANGE 10
#define DARKGREEN 11
#define MANTISGREEN 12

struct Pixel {
	char c;
	int32_t color;
};

int32_t invertColor(int32_t color)
{
	return color + 12;
}

std::vector<Pixel> toPixels(State *state, std::string s, int32_t color, uint32_t size)
{
	std::vector<Pixel> pixels = std::vector<Pixel>();
	for (char c : s) {
		if (' ' <= c && c <= '~') {
			pixels.push_back({ c, color });
		} else if (c == '\t') {
			if (state->options.indent_style == "tab") {
				for (uint32_t i = (size + pixels.size()) % state->options.indent_size; i < state->options.indent_size; i++) {
					pixels.push_back({ ' ', color == WHITE ? GREY : color });
				}
			} else {
				pixels.push_back({ ' ', color == WHITE ? GREY : color });
			}
		} else if (' ' <= unctrl(c) && unctrl(c) <= '~') {
			pixels.push_back({ unctrl(c), invertColor(MAGENTA) });
		} else {
			pixels.push_back({ ' ', invertColor(MAGENTA) });
		}
	}
	return pixels;
}

int32_t renderPixels(State *state, int32_t r, int32_t c, std::vector<Pixel> pixels, bool wrap)
{
	int32_t row = r;
	int32_t col = c;
	for (uint32_t i = 0; i + getLineNumberOffset(state) + 1 < state->maxX; i++) {
		if (i < pixels.size()) {
			attron(COLOR_PAIR(pixels[i].color));
			mvaddch(row, col, pixels[i].c);
			attroff(COLOR_PAIR(pixels[i].color));
		} else {
			attron(COLOR_PAIR(BLACK));
			mvaddch(row, col, ' ');
			attroff(COLOR_PAIR(BLACK));
		}
		if ((uint32_t)col + 1 >= state->maxX) {
			if (wrap && state->options.wordwrap) {
				row++;
				col = getLineNumberOffset(state);
			}
		} else {
			col++;
		}
	}
	return row - r + 1;
}

void insertPixels(State *state, std::vector<Pixel> *pixels, std::string s, int32_t color)
{
	std::vector<Pixel> tmp = toPixels(state, s, color, pixels->size());
	pixels->insert(std::end(*pixels), std::begin(tmp), std::end(tmp));
}

void insertPixels(State *state, std::vector<Pixel> *pixels, char c, int32_t color)
{
	insertPixels(state, pixels, std::string("") + c, color);
}

void initColors()
{
	init_pair(BLACK, _COLOR_BLACK, _COLOR_BLACK);
	init_pair(GREY, _COLOR_GREY, _COLOR_BLACK);
	init_pair(RED, _COLOR_RED, _COLOR_BLACK);
	init_pair(GREEN, _COLOR_GREEN, _COLOR_BLACK);
	init_pair(YELLOW, _COLOR_YELLOW, _COLOR_BLACK);
	init_pair(BLUE, _COLOR_BLUE, _COLOR_BLACK);
	init_pair(MAGENTA, _COLOR_MAGENTA, _COLOR_BLACK);
	init_pair(CYAN, _COLOR_CYAN, _COLOR_BLACK);
	init_pair(WHITE, _COLOR_WHITE, _COLOR_BLACK);
	init_pair(ORANGE, _COLOR_ORANGE, _COLOR_BLACK);
	init_pair(DARKGREEN, _COLOR_DARKGREEN, _COLOR_BLACK);
	init_pair(MANTISGREEN, _COLOR_MANTISGREEN, _COLOR_BLACK);

	init_pair(invertColor(BLACK), _COLOR_BLACK, _COLOR_BLACK);
	init_pair(invertColor(GREY), _COLOR_BLACK, _COLOR_GREY);
	init_pair(invertColor(RED), _COLOR_BLACK, _COLOR_RED);
	init_pair(invertColor(GREEN), _COLOR_BLACK, _COLOR_GREEN);
	init_pair(invertColor(YELLOW), _COLOR_BLACK, _COLOR_YELLOW);
	init_pair(invertColor(BLUE), _COLOR_BLACK, _COLOR_BLUE);
	init_pair(invertColor(MAGENTA), _COLOR_BLACK, _COLOR_MAGENTA);
	init_pair(invertColor(CYAN), _COLOR_BLACK, _COLOR_CYAN);
	init_pair(invertColor(WHITE), _COLOR_BLACK, _COLOR_WHITE);
	init_pair(invertColor(ORANGE), _COLOR_BLACK, _COLOR_ORANGE);
	init_pair(invertColor(DARKGREEN), _COLOR_BLACK, _COLOR_DARKGREEN);
	init_pair(invertColor(MANTISGREEN), _COLOR_BLACK, _COLOR_MANTISGREEN);
}

void renderFileStack(State *state)
{
	std::vector<Pixel> pixels = std::vector<Pixel>();

	int32_t start = (int32_t)state->fileStack.size() - 1;
	if ((int32_t)state->fileStackIndex < start - (int32_t)state->maxY / 2) {
		start = (int32_t)state->fileStackIndex + (int32_t)state->maxY / 2;
	}
	uint32_t renderRow = STATUS_BAR_LENGTH;
	for (int32_t i = start; i >= 0; i--) {
		std::string filename = minimize_filename(state->fileStack[i]);
		if (state->maxX / 4 > filename.length()) {
			filename += std::string(state->maxX / 4 - filename.length(), ' ');
		}
		int32_t color = i == (int32_t)state->fileStackIndex ? invertColor(YELLOW) : GREY;
		insertPixels(state, &pixels, "|" + filename, color);
		renderPixels(state, renderRow++, (3 * state->maxX) / 4, pixels, false);
		pixels.clear();
	}
	for (int32_t i = renderRow; i <= (int32_t)state->maxY; i++) {
		insertPixels(state, &pixels, "|" + std::string(state->maxX / 4, ' '), GREY);
		renderPixels(state, renderRow++, (3 * state->maxX) / 4, pixels, false);
		pixels.clear();
	}
}

int32_t getModeColor(State *state)
{
	if (state->mode == TYPING) {
		return MAGENTA;
	} else if (state->mode == FILEEXPLORER) {
		return CYAN;
	} else if (state->mode == VISUAL) {
		return ORANGE;
	} else {
		return GREEN;
	}
}

int32_t getDisplayModeColor(Mode m)
{
	if (m == TYPING) {
		return GREEN;
	} else if (m == SHORTCUTS) {
		return YELLOW;
	} else if (m == VISUAL) {
		return RED;
	} else {
		return ORANGE;
	}
}
std::string getDisplayModeName(State *state)
{
	if (state->mode == VISUAL && state->visualType == NORMAL) {
		return "VISUAL";
	} else if (state->mode == VISUAL && state->visualType == BLOCK) {
		return "VISUALBLOCK";
	} else if (state->mode == VISUAL && state->visualType == LINE) {
		return "VISUALLINE";
	} else if (state->mode == COMMANDLINE) {
		return "COMMANDLINE";
	} else if (state->mode == TYPING) {
		return "TYPING";
	} else if (state->mode == SHORTCUTS) {
		return "SHORTCUT";
	} else if (state->mode == FINDFILE) {
		return "FINDFILE";
	} else if (state->mode == GREP) {
		return "GREP";
	} else if (state->mode == BLAME) {
		return "BLAME";
	} else if (state->mode == MULTICURSOR) {
		return "MULTICURSOR";
	} else if (state->mode == SEARCH) {
		return "SEARCH";
	} else if (state->mode == FILEEXPLORER) {
		return "FILEEXPLORER";
	} else if (state->mode == NAMING) {
		return "NAMING";
	}
	return "ERROR";
}

int32_t renderStatusBar(State *state)
{
	int32_t cursor = -1;
	std::string prefix;
	std::vector<Pixel> pixels = std::vector<Pixel>();

	for (uint32_t i = 0; i < state->harpoon[state->workspace].list.size(); i++) {
		std::string file = state->harpoon[state->workspace].list[i];
		bool current = file == state->filename && state->harpoon[state->workspace].index == i;
		insertPixels(state, &pixels, minimize_filename(file), current ? invertColor(WHITE) : GREY);
		insertPixels(state, &pixels, ' ', WHITE);
		insertPixels(state, &pixels, ' ', WHITE);
	}
	renderPixels(state, 0, 0, pixels, false);

	pixels.clear();

	if (state->status.length() > 0) {
		prefix = "";
		insertPixels(state, &pixels, prefix + state->status, RED);
		renderPixels(state, 1, 0, pixels, false);
	}

	if (state->mode == NAMING) {
		prefix = state->prompt + " ";
		insertPixels(state, &pixels, prefix + state->name.query, WHITE);
		renderPixels(state, 1, 0, pixels, false);
		cursor = prefix.length() + state->name.cursor;
	} else if (state->mode == COMMANDLINE) {
		std::string prefix = ":";
		insertPixels(state, &pixels, prefix + state->commandLine.query, WHITE);
		renderPixels(state, 1, 0, pixels, false);
		cursor = prefix.length() + state->commandLine.cursor;
	} else if (state->mode == GREP) {
		prefix = state->grepPath + "> ";
		insertPixels(state, &pixels, prefix + state->grep.query, state->showAllGrep ? DARKGREEN : GREEN);
		insertPixels(state, &pixels, "  ", WHITE);
		insertPixels(state, &pixels, std::to_string(state->grep.selection + 1), WHITE);
		insertPixels(state, &pixels, " of ", WHITE);
		insertPixels(state, &pixels, std::to_string(state->grepOutput.size()), WHITE);
		renderPixels(state, 1, 0, pixels, false);
		cursor = prefix.length() + state->grep.cursor;
	} else if (state->mode == FINDFILE) {
		prefix = "> ";
		insertPixels(state, &pixels, prefix, YELLOW);
		insertPixels(state, &pixels, state->find.query, state->selectAll ? invertColor(YELLOW) : YELLOW);
		insertPixels(state, &pixels, "  ", WHITE);
		insertPixels(state, &pixels, std::to_string(state->find.selection + 1), WHITE);
		insertPixels(state, &pixels, " of ", WHITE);
		insertPixels(state, &pixels, std::to_string(state->findOutput.size()), WHITE);
		renderPixels(state, 1, 0, pixels, false);
		cursor = prefix.length() + state->find.cursor;
	} else {
		if (state->recording) {
			std::string s;
			for (uint32_t i = 0; i < state->macroCommand.size(); i++) {
				s += state->macroCommand[i];
			}
			insertPixels(state, &pixels, std::string("recording: ") + setStringToLength(s, 60, true), RED);
		}
		prefix = "/";
		std::string displayQuery = state->search.query;
		if (state->mode == SEARCH) {
			cursor = prefix.length() + pixels.size() + state->search.cursor;
		}
		for (size_t i = 0; i < displayQuery.length(); ++i) {
			if (displayQuery[i] == '\n') {
				displayQuery.replace(i, 1, "\\n");
				i++;
			}
		}
		insertPixels(state, &pixels, prefix + displayQuery, state->searchFail ? RED : GREEN);
		if (state->replacing) {
			cursor = prefix.length() + pixels.size() + state->replace.cursor;
			insertPixels(state, &pixels, prefix + state->replace.query, MAGENTA);
			renderPixels(state, 1, 0, pixels, false);
		}
		if (state->mode == SEARCH) {
			renderPixels(state, 1, 0, pixels, false);
		}
	}

	std::string rightSide = std::string(" --") + getMode(state->mode) + std::string("-- ");
	rightSide += "\"" + setStringToLength(state->filename, state->maxX - (pixels.size() + 2), true) + "\"";
	auto tmp = rightSide.length() + pixels.size();
	auto len = state->maxX > tmp ? state->maxX - tmp : 0;
	prefix = std::string(len, ' ');
	insertPixels(state, &pixels, prefix + rightSide, state->lastSave != state->historyPosition ? GREY : WHITE);

	renderPixels(state, 1, 0, pixels, false);
	return cursor;
}

int32_t getColorFromChar(char c)
{
	if (c == '[' || c == ']') {
		return GREEN;
	} else if (c == '(' || c == ')') {
		return YELLOW;
	} else if (c == '{' || c == '}') {
		return MAGENTA;
	} else if (c == '\'' || c == '"' || c == '`') {
		return CYAN;
	} else {
		return WHITE;
	}
}

int32_t getSearchColor(State *state, int32_t row, uint32_t startOfSearch, std::string query, bool grep)
{
	if (state->row == (uint32_t)row && startOfSearch + query.length() >= state->col && startOfSearch <= state->col) {
		return invertColor(grep ? GREEN : MAGENTA);
	} else {
		return invertColor(grep ? DARKGREEN : CYAN);
	}
}

bool isMergeConflict(const std::string &str)
{
	const std::vector<std::string> markers = { "<<<<<<<", "=======", ">>>>>>>", "|||||||" };
	for (const auto &marker : markers) {
		if (str.length() >= marker.length()) {
			if (str.substr(0, marker.length()) == marker) {
				return true;
			}
		}
	}
	return false;
}

bool isRowColInVisual(State *state, uint32_t i, uint32_t j)
{
	if (state->mode == VISUAL) {
		uint32_t minR;
		uint32_t minC;
		uint32_t maxR;
		uint32_t maxC;
		if (state->row < state->visual.row) {
			minR = state->row;
			minC = state->col;
			maxR = state->visual.row;
			maxC = state->visual.col;
		} else {
			minR = state->visual.row;
			minC = state->visual.col;
			maxR = state->row;
			maxC = state->col;
		}
		if (state->visualType == LINE) {
			if (minR <= i && i <= maxR) {
				return true;
			}
		} else if (state->visualType == BLOCK) {
			if (minR <= i && i <= maxR) {
				return (minC <= j && j <= maxC) || (maxC <= j && j <= minC);
			}
		} else if (state->visualType == NORMAL) {
			if (minR < i && i < maxR) {
				return true;
			} else if (minR == i && maxR == i) {
				return (minC <= j && j <= maxC) || (maxC <= j && j <= minC);
			} else if (minR == i) {
				return minC <= j;
			} else if (maxR == i) {
				return maxC >= j;
			}
		}
	}
	return false;
}

bool isInQuery(State *state, uint32_t row, uint32_t col, std::string query)
{
	if (query != "" && col + query.length() <= state->data[row].length()) {
		if (query == safeSubstring(state->data[row], col, query.length())) {
			return true;
		}
	}
	return false;
}

bool isRowInVisual(State *state, int32_t row)
{
	return ((int32_t)state->visual.row <= row && row <= (int32_t)state->row) || ((int32_t)state->row <= row && row <= (int32_t)state->visual.row);
}

void renderGrepOutput(State *state)
{
	uint32_t index;
	if ((int32_t)state->grep.selection - ((int32_t)state->maxY / 2) > 0) {
		index = state->grep.selection - state->maxY / 2;
	} else {
		index = 0;
	}
	uint32_t renderIndex = STATUS_BAR_LENGTH;
	int32_t color;
	for (uint32_t i = index; i < state->grepOutput.size() && i < index + state->maxY; i++) {
		std::vector<Pixel> pixels = std::vector<Pixel>();
		color = isTestFile(state->grepOutput[i].path.string()) ? ORANGE : WHITE;
		if (i == state->grep.selection) {
			color = invertColor(color);
		}
		std::string line = state->grepOutput[i].path.string();
		line += ":";
		line += std::to_string(state->grepOutput[i].lineNum);
		line += " ";
		line += state->grepOutput[i].line;
		line = safeSubstring(line, 0, state->maxX);
		insertPixels(state, &pixels, line, color);
		renderPixels(state, renderIndex, 0, pixels, false);
		renderIndex++;
	}
}

void renderFindOutput(State *state)
{
	uint32_t index;
	if ((int32_t)state->find.selection - ((int32_t)state->maxY / 2) > 0) {
		index = state->find.selection - state->maxY / 2;
	} else {
		index = 0;
	}
	uint32_t renderIndex = STATUS_BAR_LENGTH;
	int32_t color;
	for (uint32_t i = index; i < state->findOutput.size() && i < index + state->maxY; i++) {
		std::vector<Pixel> pixels = std::vector<Pixel>();
		color = isTestFile(state->findOutput[i]) ? ORANGE : WHITE;
		if (i == state->find.selection) {
			color = invertColor(color);
		}
		std::string line = safeSubstring(state->findOutput[i], 0, state->maxX);
		insertPixels(state, &pixels, line, color);
		renderPixels(state, renderIndex, 0, pixels, false);
		renderIndex++;
	}
}

std::string getBlame(State *state, int32_t i)
{
	if (state->mode == BLAME && state->blame.size() >= state->data.size() && i < (int32_t)state->data.size()) {
		return safeSubstring(state->blame[i], 0, state->blameSize);
	} else {
		return "";
	}
}

Cursor renderVisibleLines(State *state)
{
	Cursor cursor{ -1, -1 };
	int32_t currentRenderRow = STATUS_BAR_LENGTH;
	bool multiLineComment = false;
	bool checkAgain = false;
	for (int32_t i = 0; i < (int32_t)state->data.size() && i < (int32_t)(state->maxY + state->windowPosition.row) - 1; i++) {
		checkAgain = false;
		if (startsWithSymbol(state, i, "/*")) {
			multiLineComment = true;
			checkAgain = true;
		}
		if (i >= (int32_t)state->windowPosition.row) {
			renderLineNumber(state, i, currentRenderRow);
			currentRenderRow = renderLineContent(state, i, currentRenderRow, &cursor, multiLineComment);
		}
		if (startsWithSymbol(state, i, "*/")) {
			multiLineComment = false;
		}
		if (checkAgain && endsWithSymbol(state, i, "*/")) {
			multiLineComment = false;
		}
	}
	return cursor;
}

int32_t getLineNumberColor(State *state, int32_t row)
{
	if (state->runningAsRoot) {
		return RED;
	} else if (row == (int32_t)state->row) {
		return WHITE;
	} else {
		return GREY;
	}
}

int32_t getMarkColor(State *state, int32_t row)
{
	bool logging = std::regex_search(state->data[row], std::regex(getLoggingRegex(state)));
	bool endsWithSpace = isWhitespace(state->data[row].back());
	bool isOnMark = (int32_t)state->mark.mark == row && state->mark.filename == state->filename;
	if (endsWithSpace && state->mode != TYPING) {
		return RED;
	} else if (getLoggingRegex(state) != "" && logging) {
		return YELLOW;
	} else if (isOnMark) {
		return CYAN;
	} else {
		return BLACK;
	}
}

int32_t getBlameColor(State *state, int32_t row)
{
	if (row == (int32_t)state->row) {
		return invertColor(WHITE);
	} else {
		return WHITE;
	}
}

void renderLineNumber(State *state, int32_t row, int32_t renderRow)
{
	std::vector<Pixel> pixels = std::vector<Pixel>();

	insertPixels(state, &pixels, padTo(std::to_string(row + 1), state->lineNumSize, ' '), getLineNumberColor(state, row));

	insertPixels(state, &pixels, "|", getMarkColor(state, row));

	if (state->mode == BLAME) {
		insertPixels(state, &pixels, getBlame(state, row), getBlameColor(state, row));
	}

	int32_t border = state->fileExplorerOpen ? state->fileExplorerSize : 0;
	renderPixels(state, renderRow, border, pixels, false);
}

void advancePosition(State *state, int &renderRow, int &renderCol)
{
	if (state->options.wordwrap && (uint32_t)renderCol + 1 >= state->maxX - getLineNumberOffset(state)) {
		renderRow++;
		renderCol = 0;
	} else {
		renderCol++;
	}
}

Cursor renderLogLines(State *state)
{
	Cursor cursor;
	uint32_t index;
	if ((int32_t)state->logIndex - ((int32_t)state->maxY / 2) > 0) {
		index = state->logIndex - state->maxY / 2;
	} else {
		index = 0;
	}
	std::vector<Pixel> pixels = std::vector<Pixel>();
	uint32_t renderRow = STATUS_BAR_LENGTH;
	for (uint32_t i = index; i < state->logLines.size() && i < index + state->maxY; i++) {
		insertPixels(state, &pixels, state->logLines[i], WHITE);
		if (i == state->logIndex) {
			cursor = { (int32_t)renderRow, 0 };
		}
		renderRow += renderPixels(state, renderRow, 0, pixels, true);
		pixels.clear();
	}
	return cursor;
}

Cursor renderDiff(State *state)
{
	Cursor cursor = { -1, -1 };
	uint32_t index;
	if ((int32_t)state->diffIndex - ((int32_t)state->maxY / 2) > 0) {
		index = state->diffIndex - state->maxY / 2;
	} else {
		index = 0;
	}
	std::vector<Pixel> pixels = std::vector<Pixel>();
	uint32_t renderRow = STATUS_BAR_LENGTH;
	for (uint32_t i = index; i < state->diffLines.size() && i < index + state->maxY; i++) {
		int color = WHITE;
		if (state->diffLines[i].length() > 2 && state->diffLines[i][0] == '+' && state->diffLines[i][1] == '+' && state->diffLines[i][2] == '+') {
			color = WHITE;
		} else if (state->diffLines[i].length() > 2 && state->diffLines[i][0] == '-' && state->diffLines[i][1] == '-' && state->diffLines[i][2] == '-') {
			color = WHITE;
		} else if (state->diffLines[i].length() > 0 && state->diffLines[i][0] == '+') {
			color = GREEN;
		} else if (state->diffLines[i].length() > 0 && state->diffLines[i][0] == '-') {
			color = RED;
		} else if (state->diffLines[i].length() > 0 && state->diffLines[i][0] == '@') {
			color = CYAN;
		}
		insertPixels(state, &pixels, state->diffLines[i], color);
		if (i == state->diffIndex) {
			cursor = { (int32_t)renderRow, 0 };
		}
		renderRow += renderPixels(state, renderRow, 0, pixels, true);
		pixels.clear();
	}
	return cursor;
}

bool endsWithSymbol(State *state, uint32_t row, std::string symbol)
{
	return safeSubstring(state->data[row], ((int32_t)state->data[row].length()) - symbol.length()) == symbol;
}

bool startsWithSymbol(State *state, uint32_t row, std::string symbol)
{
	for (uint32_t i = 0; i < state->data[row].length(); i++) {
		if (safeSubstring(state->data[row], i, symbol.length()) == symbol) {
			return true;
		} else if (!(state->data[row][i] == '\t' || state->data[row][i] == ' ')) {
			return false;
		}
	}
	return false;
}

int32_t renderLineContent(State *state, int32_t row, int32_t renderRow, Cursor *cursor, bool multiLineComment)
{
	std::vector<Pixel> pixels = std::vector<Pixel>();
	std::vector<Pixel> replacePixels = std::vector<Pixel>();

	if (isRowColInVisual(state, row, 0) && state->data[row].length() == 0) {
		insertPixels(state, &pixels, " ", invertColor(WHITE));
		if (state->row == (uint32_t)row) {
			cursor->row = renderRow;
			cursor->col = 0;
		}
	} else {
		bool inString = false;
		bool skipNext = false;
		char stringType = 'E';
		uint32_t searchCounter = 0;
		uint32_t startOfSearch = 0;
		bool isComment = false;
		if (multiLineComment) {
			isComment = true;
		}
		bool foundCursor = false;

		uint32_t col = 0;
		if (getDisplayRows(state, row) > state->maxY) {
			int32_t width = ((int32_t)state->maxX - getLineNumberOffset(state));
			int32_t offset = ((((int32_t)state->maxY / 2) - 1) * width) + (width / 2);
			if (state->col > (uint32_t)offset) {
				col = state->col - offset;
				cursor->row = (state->maxY / 2) + 1;
				cursor->col = width / 2;
				foundCursor = true;
				insertPixels(state, &replacePixels, '.', GREY);
				insertPixels(state, &replacePixels, '.', GREY);
				insertPixels(state, &replacePixels, '.', GREY);
				insertPixels(state, &replacePixels, '>', GREY);
			}
		}
		for (; col < state->data[row].length(); col++) {
			char c = state->data[row][col];

			if (skipNext) {
				skipNext = false;
			} else if (inString && c == '\\') {
				skipNext = true;
			} else if (!inString && (c == '"' || c == '`' || c == '\'')) {
				inString = true;
				stringType = c;
			} else if (inString && c == stringType) {
				inString = false;
			}

			if (state->showGrep) {
				if (searchCounter == 0 && isInQuery(state, row, col, state->grep.query)) {
					searchCounter = state->grep.query.length();
					startOfSearch = col;
				}
			} else {
				if (searchCounter == 0 && isInQuery(state, row, col, state->search.query)) {
					searchCounter = state->search.query.length();
					startOfSearch = col;
				}
			}

			if (!inString && safeSubstring(state->data[row], col, state->commentSymbol.length()) == state->commentSymbol && state->commentSymbol != "") {
				isComment = true;
			}

			int32_t color = getColorFromChar(c);
			if (state->matching.row == (uint32_t)row && state->matching.col == col && (state->matching.row != state->row || state->matching.col != state->col)) {
				color = invertColor(GREY);
			} else {
				if (state->showGrep && searchCounter != 0) {
					color = getSearchColor(state, row, startOfSearch, state->grep.query, true);
				} else if (searchCounter != 0 && (state->searching || state->replacing)) {
					color = getSearchColor(state, row, startOfSearch, state->search.query, false);
				} else if ((int32_t)state->row == row && state->col == col && col < state->data[row].length() && state->data[row][col] == '\t') {
					color = invertColor(WHITE);
				} else {
					if (isMergeConflict(state->data[row])) {
						color = RED;
					} else if (isComment) {
						color = MANTISGREEN;
					} else if (inString && getExtension(state->filename) != "md" && getExtension(state->filename) != "txt") {
						color = CYAN;
					} else {
						color = getColorFromChar(state->data[row][col]);
					}
					if (isRowColInVisual(state, row, col)) {
						color = invertColor(color);
					}
				}
			}

			if (col >= state->windowPosition.col) {
				if (state->replacing && searchCounter != 0) {
					insertPixels(state, &pixels, state->replace.query, color);
					col += state->search.query.length() - 1;
					searchCounter = 0;
				} else {
					insertPixels(state, &pixels, c, color);
				}
				if (!foundCursor && state->row == (uint32_t)row && state->col == col) {
					cursor->row = renderRow;
					cursor->col = pixels.size() > 0 ? pixels.size() - 1 : 0;
					foundCursor = true;
				}
				if (state->row == (uint32_t)row && state->col == col + 1) {
					if (state->mode == TYPING || state->mode == MULTICURSOR) {
						if (state->col + 1 >= state->data[state->row].length() || !isAlphanumeric(state->data[state->row][state->col])) {
							cursor->row = renderRow;
							cursor->col = pixels.size() > 0 ? pixels.size() : 0;
							foundCursor = true;
							insertPixels(state, &pixels, autocomplete(state, getCurrentWord(state)), GREY);
						}
					}
				}
			}

			if (searchCounter != 0) {
				searchCounter -= 1;
			}
		}
		if (state->row == (uint32_t)row) {
			if (!foundCursor && state->col >= state->data[row].length()) {
				cursor->row = renderRow;
				cursor->col = getDisplayLength(state, state->data[row]);
				foundCursor = true;
			}
		}
	}
	for (uint32_t i = 0; i < replacePixels.size(); i++) {
		if (i < pixels.size()) {
			pixels[i] = replacePixels[i];
		}
	}
	return renderRow + renderPixels(state, renderRow, getLineNumberOffset(state), pixels, true);
}

void moveCursor(State *state, int32_t cursorOnStatusBar, Cursor editorCursor, Cursor fileExplorerCursor, bool noLineNum)
{
	if (cursorOnStatusBar != -1) {
		move(1, cursorOnStatusBar);
	} else if (state->mode == FILEEXPLORER) {
		move(fileExplorerCursor.row, fileExplorerCursor.col);
	} else {
		auto row = editorCursor.row;
		auto col = editorCursor.col;
		uint32_t len = state->maxX - getLineNumberOffset(state);
		if (state->options.wordwrap && (uint32_t)col + 1 >= len) {
			row += col / len;
			col %= len;
		}
		if (noLineNum) {
			move(row, col);
		} else {
			move(row, col + getLineNumberOffset(state));
		}
	}
}

int32_t renderFileExplorerNode(State *state, FileExplorerNode *node, int32_t r, std::string startingSpaces, Cursor &cursor)
{
	int32_t color;
	if (shouldIgnoreFile(node->path)) {
		color = GREY;
	} else if (node->isFolder) {
		color = CYAN;
	} else {
		color = WHITE;
	}
	int32_t row = r + 1;
	std::vector<Pixel> pixels = std::vector<Pixel>();
	if (row - state->fileExplorerWindowLine > 0) {
		auto displayRow = row - state->fileExplorerWindowLine + 1;
		insertPixels(state, &pixels, startingSpaces, GREY);
		if (node->isFolder) {
			insertPixels(state, &pixels, node->isOpen ? 'v' : '>', GREY);
			insertPixels(state, &pixels, ' ', GREY);
		}
		if (state->fileExplorerIndex == r) {
			cursor.row = displayRow;
			cursor.col = pixels.size();
		}
		insertPixels(state, &pixels, node->name, color);
		if (pixels.size() > state->fileExplorerSize) {
			auto size = pixels.size();
			for (uint32_t i = state->fileExplorerSize; i < size + 1; i++) {
				pixels.pop_back();
			}
			insertPixels(state, &pixels, "~", color);
		}
		renderPixels(state, displayRow, 0, pixels, false);
	}
	if (node->isOpen) {
		for (uint32_t i = 0; i < node->children.size(); i++) {
			row = renderFileExplorerNode(state, &node->children[i], row, startingSpaces + "  ", cursor);
		}
	}
	return row;
}

Cursor renderFileExplorer(State *state)
{
	Cursor cursor{ -1, -1 };
	renderFileExplorerNode(state, state->fileExplorer, 0, std::string(""), cursor);
	return cursor;
}

void renderScreen(State *state, bool fullRedraw)
{
	if (fullRedraw) {
		clear();
	}
	renderScreen(state);
}

void renderScreen(State *state)
{
	try {
		state->renderMutex.lock();
		erase();
		bool noLineNum = false;
		Cursor editorCursor, fileExplorerCursor;
		if (state->mode == FINDFILE) {
			renderFindOutput(state);
		} else if (state->mode == DIFF) {
			if (state->viewingDiff) {
				editorCursor = renderDiff(state);
			} else {
				editorCursor = renderLogLines(state);
			}
			noLineNum = true;
		} else if (state->mode == GREP) {
			renderGrepOutput(state);
		} else {
			editorCursor = renderVisibleLines(state);
			if (state->fileExplorerOpen) {
				fileExplorerCursor = renderFileExplorer(state);
			}
		}
		if (state->showFileStack) {
			renderFileStack(state);
		}
		int32_t cursorOnStatusBar = renderStatusBar(state);
		moveCursor(state, cursorOnStatusBar, editorCursor, fileExplorerCursor, noLineNum);
		wnoutrefresh(stdscr);
		doupdate();
		state->renderMutex.unlock();
	} catch (const std::exception &e) {
		state->renderMutex.unlock();
		state->status = std::string("something went wrong while rendering") + e.what();
	}
}

void initTerminal()
{
	initscr();
	raw();
	keypad(stdscr, true);
	noecho();
	if (has_colors() == false) {
		endwin();
		std::cout << "Your terminal does not support color" << std::endl;
		exit(1);
	}
	start_color();
	if (COLORS < 256) {
		endwin();
		std::cout << "Your terminal does not support 256 colors" << std::endl;
		exit(1);
	}
	initColors();
	mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL);
}
