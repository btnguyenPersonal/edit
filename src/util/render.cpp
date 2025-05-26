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

// is there a better way for arg passing?
void mvprintw_color(int32_t r, int32_t c, const char *formatString, const char *cstring1, int32_t num,
		    const char *cstring2, int32_t color)
{
	attron(COLOR_PAIR(color));
	mvprintw(r, c, formatString, cstring1, num, cstring2);
	attroff(COLOR_PAIR(color));
}

void mvprintw_color(int32_t r, int32_t c, const char *formatString, int32_t num, int32_t color)
{
	attron(COLOR_PAIR(color));
	mvprintw(r, c, formatString, num);
	attroff(COLOR_PAIR(color));
}

void mvprintw_color(int32_t r, int32_t c, const char *formatString, const char *cstring, int32_t color)
{
	attron(COLOR_PAIR(color));
	mvprintw(r, c, formatString, cstring);
	attroff(COLOR_PAIR(color));
}

void mvaddch_color(int32_t r, int32_t c, char ch, int32_t color)
{
	attron(COLOR_PAIR(color));
	mvaddch(r, c, ch);
	attroff(COLOR_PAIR(color));
}

std::vector<Pixel> toPixels(std::string s, int32_t color)
{
	std::vector<Pixel> pixels = std::vector<Pixel>();
	for (char c : s) {
		pixels.push_back({ c, color });
	}
	return pixels;
}

void renderPixels(int32_t r, int32_t c, std::vector<Pixel> pixels)
{
	int32_t i = c;
	for (Pixel p : pixels) {
		attron(COLOR_PAIR(p.color));
		mvaddch(r, i, p.c);
		attroff(COLOR_PAIR(p.color));
		i++;
	}
}

void insertPixels(std::vector<Pixel> *pixels, std::string s, int32_t color)
{
	std::vector<Pixel> tmp = toPixels(s, color);
	pixels->insert(std::end(*pixels), std::begin(tmp), std::end(tmp));
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
		insertPixels(&pixels, "|" + filename, color);
		renderPixels(renderRow++, state->maxX / 2, pixels);
		pixels.clear();
	}
	for (int32_t i = renderRow; i <= (int32_t)state->maxY; i++) {
		insertPixels(&pixels, "|" + std::string(state->maxX / 2, ' '), GREY);
		renderPixels(renderRow++, state->maxX / 2, pixels);
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
		insertPixels(&pixels, prefix + state->status, RED);
		renderPixels(0, 0, pixels);
	}

	if (state->mode == NAMING) {
		prefix = "name: ";
		insertPixels(&pixels, prefix + state->name.query, WHITE);
		renderPixels(0, 0, pixels);
		return prefix.length() + state->name.cursor;
	} else if (state->mode == COMMANDLINE) {
		std::string prefix = ":";
		insertPixels(&pixels, prefix + state->commandLine.query, WHITE);
		renderPixels(0, 0, pixels);
		return prefix.length() + state->commandLine.cursor;
	} else if (state->mode == GREP) {
		prefix = state->grepPath + "> ";
		insertPixels(&pixels, prefix + state->grep.query, state->showAllGrep ? DARKGREEN : GREEN);
		insertPixels(&pixels, "  ", WHITE);
		insertPixels(&pixels, std::to_string(state->grep.selection + 1), WHITE);
		insertPixels(&pixels, " of ", WHITE);
		insertPixels(&pixels, std::to_string(state->grepOutput.size()), WHITE);
		renderPixels(0, 0, pixels);
		return prefix.length() + state->grep.cursor;
	} else if (state->mode == FINDFILE) {
		prefix = "> ";
		insertPixels(&pixels, prefix, YELLOW);
		insertPixels(&pixels, state->findFile.query, state->selectAll ? invertColor(YELLOW) : YELLOW);
		insertPixels(&pixels, "  ", WHITE);
		insertPixels(&pixels, std::to_string(state->findFile.selection + 1), WHITE);
		insertPixels(&pixels, " of ", WHITE);
		insertPixels(&pixels, std::to_string(state->findFileOutput.size()), WHITE);
		renderPixels(0, 0, pixels);
		return prefix.length() + state->findFile.cursor;
	} else if (state->searching || state->mode == SEARCH) {
		prefix = "/";
		std::string displayQuery = state->search.query;
		for (size_t i = 0; i < displayQuery.length(); ++i) {
			if (displayQuery[i] == '\n') {
				displayQuery.replace(i, 1, "\\n");
				i++;
			}
		}
		insertPixels(&pixels, prefix + displayQuery, state->searchFail ? RED : GREEN);
		if (state->replacing) {
			insertPixels(&pixels, prefix + state->replace.query, MAGENTA);
			renderPixels(0, 0, pixels);
			return prefix.length() * 2 + displayQuery.length() + state->replace.cursor;
		}
		if (state->mode == SEARCH) {
			renderPixels(0, 0, pixels);
			return prefix.length() + state->search.cursor;
		}
	} else {
		for (uint32_t i = 0; i < 10; i++) {
			std::string s;
			if (state->harpoonFiles.count(i) > 0) {
				s = "[" + std::to_string(i + 1) + "]";
				bool currentSelected = state->harpoonFiles[i] == state->filename &&
						       state->harpoonIndex == i;
				insertPixels(&pixels, s, currentSelected ? YELLOW : GREY);
			} else {
				insertPixels(&pixels, "   ", GREY);
			}
		}
	}

	auto displayFileName = "\"" + setStringToLength(state->filename, state->maxX - 30) + "\"";
	auto tmp = displayFileName.length() + pixels.size();
	auto len = state->maxX > tmp ? state->maxX - tmp : 0;
	prefix = std::string(len, ' ');
	insertPixels(&pixels, prefix + displayFileName, state->unsavedFile ? GREY : WHITE);

	if (state->options.showmode) {
		std::string prefix = " --%s-- ";
		std::string modename = getDisplayModeName(state);
		insertPixels(&pixels, modename, getModeColor(state));
	}

	renderPixels(0, 0, pixels);
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

void printChar(State *state, int &row, int &col, char c, int32_t color, bool advance)
{
	if (' ' <= c && c <= '~') {
		mvaddch_color(row, col + getLineNumberOffset(state), c, color);
	} else if (c == '\t') {
		for (uint32_t i = col % state->options.indent_size; i < state->options.indent_size; i++) {
			mvaddch_color(row, col + getLineNumberOffset(state), ' ', color == WHITE ? GREY : color);
			advancePosition(state, row, col);
		}
		return;
	} else if (' ' <= unctrl(c) && unctrl(c) <= '~') {
		mvaddch_color(row, col + getLineNumberOffset(state), unctrl(c), invertColor(MAGENTA));
	} else {
		mvaddch_color(row, col + getLineNumberOffset(state), ' ', invertColor(MAGENTA));
	}
	if (advance) {
		advancePosition(state, row, col);
	}
}

void printChar(State *state, int32_t row, int32_t col, char c, int32_t color)
{
	printChar(state, row, col, c, color, false);
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
		if (query == state->data[row].substr(col, query.length())) {
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

uint32_t renderAutoComplete(State *state, uint32_t renderRow, uint32_t renderCol)
{
	if (state->mode == TYPING || state->mode == MULTICURSOR) {
		std::string completion = autocomplete(state, getCurrentWord(state));
		if (state->col + 1 >= state->data[state->row].length() ||
		    !isAlphanumeric(state->data[state->row][state->col])) {
			for (uint32_t i = 0; i < completion.length(); i++) {
				printChar(state, renderRow, renderCol + i, completion[i], GREY);
			}
			return completion.length();
		}
	}
	return 0;
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
		color = isTestFile(state->grepOutput[i].path.string()) ? ORANGE : WHITE;
		mvprintw_color(renderIndex, 0, "%s:%d %s\n", state->grepOutput[i].path.c_str(),
			       state->grepOutput[i].lineNum, state->grepOutput[i].line.c_str(),
			       i == state->grep.selection ? invertColor(color) : color);
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
		color = isTestFile(state->findFileOutput[i]) ? ORANGE : WHITE;
		mvprintw_color(renderIndex, 0, "%s\n", state->findFileOutput[i].c_str(),
			       i == state->findFile.selection ? invertColor(color) : color);
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
		printLineNumber(state, i, currentRenderRow);
		currentRenderRow = printLineContent(state, i, currentRenderRow, &cursor);
	}
	return cursor;
}

int32_t getLineNumberColor(State *state, int32_t row)
{
	if (row == (int32_t)state->row) {
		return WHITE;
	} else if (state->recording) {
		return RED;
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

void printLineNumber(State *state, int32_t row, int32_t renderRow)
{
	std::vector<Pixel> pixels = std::vector<Pixel>();

	insertPixels(&pixels, padTo(std::to_string(row + 1), state->lineNumSize, ' '), getLineNumberColor(state, row));

	insertPixels(&pixels, "|", getMarkColor(state, row));

	if (state->mode == BLAME) {
		insertPixels(&pixels, getBlame(state, row), getBlameColor(state, row));
	}

	int32_t border = state->fileExplorerOpen ? state->fileExplorerSize : 0;
	renderPixels(renderRow, border, pixels);
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

int32_t printLineContent(State *state, int32_t row, int32_t renderRow, Cursor *cursor)
{
	if (isRowColInVisual(state, row, 0) == true && state->data[row].length() == 0) {
		printChar(state, renderRow, 0, ' ', invertColor(WHITE));
		if (state->row == (uint32_t)row) {
			cursor->row = renderRow - 1;
			cursor->col = 0;
		}
	} else {
		bool isInString = false;
		bool skipNext = false;
		uint32_t searchCounter = 0;
		int32_t renderCol = 0;
		uint32_t startOfSearch = 0;
		bool isComment = false;
		char stringType;
		uint32_t col = 0;
		uint32_t completionLength = 0;
		while (col < state->data[row].length()) {
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
			if (skipNext == false) {
				char current = state->data[row][col];
				if (isInString && current == '\\') {
					skipNext = true;
				} else {
					if (isInString == false &&
					    (current == '"' || current == '`' || current == '\'')) {
						isInString = true;
						stringType = current;
					} else if (isInString == true && current == stringType) {
						isInString = false;
					}
				}
			} else {
				skipNext = false;
			}
			if (!isInString &&
			    state->data[row].substr(col, state->commentSymbol.length()) == state->commentSymbol &&
			    state->commentSymbol != "") {
				isComment = true;
			}
			if (col >= state->windowPosition.col) {
				if (state->replacing && searchCounter != 0) {
					for (uint32_t i = 0; i < state->replace.query.length(); i++) {
						printChar(state, renderRow, renderCol, state->replace.query[i],
							  getSearchColor(state, row, startOfSearch, state->search.query,
									 false),
							  true);
					}
					col += state->search.query.length();
					searchCounter = 0;
				} else {
					// TODO make this a getColor function();, lots of logic in here that's messy
					int32_t color;
					if (state->matching.row == (uint32_t)row && state->matching.col == col &&
					    (state->matching.row != state->row || state->matching.col != state->col)) {
						color = invertColor(GREY);
					} else {
						if (state->showGrep && searchCounter != 0) {
							color = getSearchColor(state, row, startOfSearch,
									       state->grep.query, true);
						} else if (searchCounter != 0 && state->searching == true) {
							color = getSearchColor(state, row, startOfSearch,
									       state->search.query, false);
						} else {
							if (isMergeConflict(state->data[row])) {
								color = RED;
							} else if (isComment) {
								color = GREEN;
							} else if (isInString == true &&
								   getExtension(state->filename) != "md" &&
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
					if (state->row == (uint32_t)row && state->col == col) {
						cursor->row = renderRow - 1;
						cursor->col = renderCol;
						if (completionLength != 0) {
							renderCol += completionLength;
							completionLength = 0;
						}
					}
					printChar(state, renderRow, renderCol, state->data[row][col], color, true);
					col++;
					if (state->row == (uint32_t)row && state->col == col) {
						completionLength = renderAutoComplete(state, renderRow, renderCol);
					}
				}
			} else {
				col++;
			}
			if (searchCounter != 0) {
				searchCounter -= 1;
			}
		}
		if (state->col >= state->data[row].length()) {
			if (state->row == (uint32_t)row) {
				cursor->row = renderRow - 1;
				cursor->col = renderCol;
			}
		}
	}
	return renderRow + 1;
}

void moveCursor(State *state, int32_t cursorOnStatusBar, Cursor editorCursor, Cursor fileExplorerCursor)
{
	if (cursorOnStatusBar != -1) {
		move(0, cursorOnStatusBar);
	} else if (state->mode == FILEEXPLORER) {
		move(fileExplorerCursor.row, fileExplorerCursor.col);
	} else {
		move(editorCursor.row + 1, editorCursor.col + getLineNumberOffset(state));
	}
}

int32_t renderFileExplorerNode(FileExplorerNode *node, int32_t r, int32_t selectedRow, std::string startingSpaces,
			       bool isFileExplorerOpen, int32_t fileExplorerWindowLine, int32_t fileExplorerSize,
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
	int32_t offset = 0;
	if (row - fileExplorerWindowLine > 0) {
		auto displayRow = row - fileExplorerWindowLine;
		mvprintw_color(displayRow, offset, "%s", startingSpaces.c_str(), WHITE);
		offset += startingSpaces.length();
		if (node->isFolder) {
			mvprintw_color(displayRow, offset, "%s", node->isOpen ? "v" : ">", GREY);
			offset += 1;
		}
		offset += 1;
		if (selectedRow == r) {
			cursor.row = displayRow;
			cursor.col = offset;
		}
		mvprintw_color(
			displayRow, offset,
			(std::string("%-") + std::to_string(fileExplorerSize - offset - 1) + std::string("s")).c_str(),
			(int32_t)node->name.length() >= fileExplorerSize - offset - 1 ?
				(node->name.substr(0, fileExplorerSize - 3 - offset - 1) + "...").c_str() :
				node->name.substr(0, fileExplorerSize - offset - 1).c_str(),
			color);
		offset += node->name.substr(0, fileExplorerSize - offset - 1).length();
	}
	if (node->isOpen) {
		for (uint32_t i = 0; i < node->children.size(); i++) {
			row = renderFileExplorerNode(&node->children[i], row, selectedRow, startingSpaces + "  ",
						     isFileExplorerOpen, fileExplorerWindowLine, fileExplorerSize,
						     cursor);
		}
	}
	return row;
}

Cursor renderFileExplorer(State *state)
{
	Cursor cursor{ -1, -1 };
	renderFileExplorerNode(state->fileExplorer, 0, state->fileExplorerIndex, std::string(""),
			       state->mode == FILEEXPLORER, state->fileExplorerWindowLine, state->fileExplorerSize,
			       cursor);
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
