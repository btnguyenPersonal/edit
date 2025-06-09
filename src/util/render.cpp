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

struct Pixel {
	char c;
	int32_t color;
};

int32_t invertColor(int32_t color)
{
	return color + 11;
}

std::vector<Pixel> toPixels(State *state, std::string s, int32_t color, uint32_t size)
{
	std::vector<Pixel> pixels = std::vector<Pixel>();
	for (char c : s) {
		if (' ' <= c && c <= '~') {
			pixels.push_back({ c, color });
		} else if (c == '\t') {
			if (state->options.indent_style == "tab") {
				for (uint32_t i = (size + pixels.size()) % state->options.indent_size;
				     i < state->options.indent_size; i++) {
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

int32_t renderPixels(State *state, int32_t r, int32_t c, std::vector<Pixel> pixels)
{
	int32_t row = r;
	int32_t col = c;
	for (Pixel p : pixels) {
		attron(COLOR_PAIR(p.color));
		mvaddch(row, col, p.c);
		attroff(COLOR_PAIR(p.color));
		if (state->options.wordwrap && (uint32_t)col + 1 >= state->maxX) {
			row++;
			col = getLineNumberOffset(state);
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
}

void renderFileStack(State *state)
{
	std::vector<Pixel> pixels = std::vector<Pixel>();

	int32_t start = (int32_t)state->fileStack.size() - 1;
	if ((int32_t)state->fileStackIndex < start - (int32_t)state->maxY / 2) {
		start = (int32_t)state->fileStackIndex + (int32_t)state->maxY / 2;
	}
	uint32_t renderRow = 1;
	for (int32_t i = start; i >= 0; i--) {
		std::string filename = minimize_filename(state->fileStack[i]);
		if (state->maxX / 2 > filename.length()) {
			filename += std::string(state->maxX / 2 - filename.length(), ' ');
		}
		int32_t color = i == (int32_t)state->fileStackIndex ? invertColor(YELLOW) : GREY;
		insertPixels(state, &pixels, "|" + filename, color);
		renderPixels(state, renderRow++, state->maxX / 2, pixels);
		pixels.clear();
	}
	for (int32_t i = renderRow; i <= (int32_t)state->maxY; i++) {
		insertPixels(state, &pixels, "|" + std::string(state->maxX / 2, ' '), GREY);
		renderPixels(state, renderRow++, state->maxX / 2, pixels);
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
	std::string prefix;
	std::vector<Pixel> pixels = std::vector<Pixel>();

	if (state->status.length() > 0) {
		prefix = "";
		insertPixels(state, &pixels, prefix + state->status, RED);
		renderPixels(state, 0, 0, pixels);
	}

	if (state->mode == NAMING) {
		prefix = "name: ";
		insertPixels(state, &pixels, prefix + state->name.query, WHITE);
		renderPixels(state, 0, 0, pixels);
		return prefix.length() + state->name.cursor;
	} else if (state->mode == COMMANDLINE) {
		std::string prefix = ":";
		insertPixels(state, &pixels, prefix + state->commandLine.query, WHITE);
		renderPixels(state, 0, 0, pixels);
		return prefix.length() + state->commandLine.cursor;
	} else if (state->mode == GREP) {
		prefix = state->grepPath + "> ";
		insertPixels(state, &pixels, prefix + state->grep.query, state->showAllGrep ? DARKGREEN : GREEN);
		insertPixels(state, &pixels, "  ", WHITE);
		insertPixels(state, &pixels, std::to_string(state->grep.selection + 1), WHITE);
		insertPixels(state, &pixels, " of ", WHITE);
		insertPixels(state, &pixels, std::to_string(state->grepOutput.size()), WHITE);
		renderPixels(state, 0, 0, pixels);
		return prefix.length() + state->grep.cursor;
	} else if (state->mode == FINDFILE) {
		prefix = "> ";
		insertPixels(state, &pixels, prefix, YELLOW);
		insertPixels(state, &pixels, state->findFile.query, state->selectAll ? invertColor(YELLOW) : YELLOW);
		insertPixels(state, &pixels, "  ", WHITE);
		insertPixels(state, &pixels, std::to_string(state->findFile.selection + 1), WHITE);
		insertPixels(state, &pixels, " of ", WHITE);
		insertPixels(state, &pixels, std::to_string(state->findFileOutput.size()), WHITE);
		renderPixels(state, 0, 0, pixels);
		return prefix.length() + state->findFile.cursor;
	} else {
		uint32_t i = 0;
		uint32_t limit = 5;
		if (state->harpoonIndex >= 5) {
			i = state->harpoonIndex - 2;
			limit = state->harpoonIndex + 3;
		}
		for (; i < limit; i++) {
			std::string s;
			if (state->harpoonFiles.count(i) > 0) {
				bool current = state->harpoonFiles[i] == state->filename && state->harpoonIndex == i;
				insertPixels(state, &pixels, '[', current ? ORANGE : GREY);
				insertPixels(state, &pixels, std::to_string(i + 1), current ? YELLOW : WHITE);
				insertPixels(state, &pixels, ']', current ? ORANGE : GREY);
			} else {
				insertPixels(state, &pixels, "[ ]", GREY);
			}
		}
		insertPixels(state, &pixels, ' ', WHITE);
		insertPixels(state, &pixels, setStringToLength(state->keys, 15, false), state->recording ? RED : WHITE);
		insertPixels(state, &pixels, ' ', WHITE);
		prefix = "/";
		std::string displayQuery = state->search.query;
		for (size_t i = 0; i < displayQuery.length(); ++i) {
			if (displayQuery[i] == '\n') {
				displayQuery.replace(i, 1, "\\n");
				i++;
			}
		}
		insertPixels(state, &pixels, prefix + displayQuery, state->searchFail ? RED : GREEN);
		if (state->replacing) {
			insertPixels(state, &pixels, prefix + state->replace.query, MAGENTA);
			renderPixels(state, 0, 0, pixels);
		}
		if (state->mode == SEARCH) {
			renderPixels(state, 0, 0, pixels);
		}
	}

	auto displayFileName =
		"\"" + setStringToLength(state->filename, state->maxX - (pixels.size() + 2), true) + "\"";
	auto tmp = displayFileName.length() + pixels.size();
	auto len = state->maxX > tmp ? state->maxX - tmp : 0;
	prefix = std::string(len, ' ');
	insertPixels(state, &pixels, prefix + displayFileName, state->lastSave != state->historyPosition ? GREY : WHITE);

	renderPixels(state, 0, 0, pixels);
	return -1;
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
	if (state->row == (uint32_t)row && startOfSearch + query.length() >= state->col &&
	    startOfSearch <= state->col) {
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
	return ((int32_t)state->visual.row <= row && row <= (int32_t)state->row) ||
	       ((int32_t)state->row <= row && row <= (int32_t)state->visual.row);
}

void renderGrepOutput(State *state)
{
	uint32_t index;
	if ((int32_t)state->grep.selection - ((int32_t)state->maxY / 2) > 0) {
		index = state->grep.selection - state->maxY / 2;
	} else {
		index = 0;
	}
	uint32_t renderIndex = 1;
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
		renderPixels(state, renderIndex, 0, pixels);
		renderIndex++;
	}
}

void renderFindFileOutput(State *state)
{
	uint32_t index;
	if ((int32_t)state->findFile.selection - ((int32_t)state->maxY / 2) > 0) {
		index = state->findFile.selection - state->maxY / 2;
	} else {
		index = 0;
	}
	uint32_t renderIndex = 1;
	int32_t color;
	for (uint32_t i = index; i < state->findFileOutput.size() && i < index + state->maxY; i++) {
		std::vector<Pixel> pixels = std::vector<Pixel>();
		color = isTestFile(state->findFileOutput[i]) ? ORANGE : WHITE;
		if (i == state->findFile.selection) {
			color = invertColor(color);
		}
		std::string line = safeSubstring(state->findFileOutput[i], 0, state->maxX);
		insertPixels(state, &pixels, line, color);
		renderPixels(state, renderIndex, 0, pixels);
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
	int32_t currentRenderRow = 1;
	for (int32_t i = state->windowPosition.row;
	     i < (int32_t)state->data.size() && i < (int32_t)(state->maxY + state->windowPosition.row) - 1; i++) {
		renderLineNumber(state, i, currentRenderRow);
		currentRenderRow = renderLineContent(state, i, currentRenderRow, &cursor);
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
	bool endsWithSpace = state->data[row].back() == ' ';
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

	insertPixels(state, &pixels, padTo(std::to_string(row + 1), state->lineNumSize, ' '),
		     getLineNumberColor(state, row));

	insertPixels(state, &pixels, "|", getMarkColor(state, row));

	if (state->mode == BLAME) {
		insertPixels(state, &pixels, getBlame(state, row), getBlameColor(state, row));
	}

	int32_t border = state->fileExplorerOpen ? state->fileExplorerSize : 0;
	renderPixels(state, renderRow, border, pixels);
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

int32_t renderLineContent(State *state, int32_t row, int32_t renderRow, Cursor *cursor)
{
	std::vector<Pixel> pixels = std::vector<Pixel>();

	if (isRowColInVisual(state, row, 0) && state->data[row].length() == 0) {
		insertPixels(state, &pixels, " ", invertColor(WHITE));
		if (state->row == (uint32_t)row) {
			cursor->row = renderRow;
			cursor->col = 0;
		}
	} else {
		bool inString = false;
		bool skipNext = false;
		char stringType;
		uint32_t searchCounter = 0;
		uint32_t startOfSearch = 0;
		bool isComment = false;
		bool foundCursor = false;

		// TODO just search replace tab with spaces then don't need weird workarounds
		// will need to add to cursor as well
		for (uint32_t col = 0; col < state->data[row].length(); col++) {
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

			if (!inString &&
			    safeSubstring(state->data[row], col, state->commentSymbol.length()) ==
				    state->commentSymbol &&
			    state->commentSymbol != "") {
				isComment = true;
			}

			int32_t color = getColorFromChar(c);
			if (state->matching.row == (uint32_t)row && state->matching.col == col &&
			    (state->matching.row != state->row || state->matching.col != state->col)) {
				color = invertColor(GREY);
			} else {
				if (state->showGrep && searchCounter != 0) {
					color = getSearchColor(state, row, startOfSearch, state->grep.query, true);
				} else if (searchCounter != 0 && (state->searching || state->replacing)) {
					color = getSearchColor(state, row, startOfSearch, state->search.query, false);
				} else {
					if (isMergeConflict(state->data[row])) {
						color = RED;
					} else if (isComment) {
						color = GREEN;
					} else if (inString && getExtension(state->filename) != "md" &&
						   getExtension(state->filename) != "txt") {
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
						if (state->col + 1 >= state->data[state->row].length() ||
						    !isAlphanumeric(state->data[state->row][state->col])) {
							cursor->row = renderRow;
							cursor->col = pixels.size() > 0 ? pixels.size() : 0;
							foundCursor = true;
							insertPixels(state, &pixels,
								     autocomplete(state, getCurrentWord(state)), GREY);
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
	return renderRow + renderPixels(state, renderRow, getLineNumberOffset(state), pixels);
}

void moveCursor(State *state, int32_t cursorOnStatusBar, Cursor editorCursor, Cursor fileExplorerCursor)
{
	if (cursorOnStatusBar != -1) {
		move(0, cursorOnStatusBar);
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
		move(row, col + getLineNumberOffset(state));
	}
}

int32_t renderFileExplorerNode(State *state, FileExplorerNode *node, int32_t r, std::string startingSpaces,
			       Cursor &cursor)
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
		auto displayRow = row - state->fileExplorerWindowLine;
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
		renderPixels(state, displayRow, 0, pixels);
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
	clear();
	renderScreen(state);
}

void renderScreen(State *state)
{
	erase();
	Cursor editorCursor, fileExplorerCursor;
	if (state->mode == FINDFILE) {
		renderFindFileOutput(state);
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
	moveCursor(state, cursorOnStatusBar, editorCursor, fileExplorerCursor);
	wnoutrefresh(stdscr);
	doupdate();
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
