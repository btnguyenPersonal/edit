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

#define LEFT_STATUS_BORDER 10
#define RIGHT_STATUS_BORDER 20

int invertColor(int color) { return color + 11; }

// is there a better way for arg passing?
void mvprintw_color(int r, int c, const char* formatString, const char* cstring1, int num, const char* cstring2, int color) {
    attron(COLOR_PAIR(color));
    mvprintw(r, c, formatString, cstring1, num, cstring2);
    attroff(COLOR_PAIR(color));
}

void mvprintw_color(int r, int c, const char* formatString, int num, int color) {
    attron(COLOR_PAIR(color));
    mvprintw(r, c, formatString, num);
    attroff(COLOR_PAIR(color));
}

void mvprintw_color(int r, int c, const char* formatString, const char* cstring, int color) {
    attron(COLOR_PAIR(color));
    mvprintw(r, c, formatString, cstring);
    attroff(COLOR_PAIR(color));
}

void mvaddch_color(int r, int c, char ch, int color) {
    attron(COLOR_PAIR(color));
    mvaddch(r, c, ch);
    attroff(COLOR_PAIR(color));
}

void initColors() {
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

void renderNumMatches(int offset, int selection, int total) {
    int index = offset;
    mvprintw_color(0, index, "%d", selection, WHITE);
    index += std::to_string(selection).length();
    mvprintw_color(0, index, " of ", "", WHITE);
    index += 4;
    mvprintw_color(0, index, "%d", total, WHITE);
    index += std::to_string(total).length();
}

int renderStatusBar(State* state) {
    int cursor = -1;
    int offset = 0;

    if (state->status.length() > 0) {
        mvprintw_color(0, (state->maxX / 2) - (state->status.length() / 2), "%s", state->status.c_str(), RED);
        return cursor;
    }
    if (state->mode == NAMING) {
        mvprintw_color(0, offset, "name: %s", state->name.query.c_str(), WHITE);
        offset += state->name.cursor + 6;
        return offset;
    } else if (state->mode == COMMANDLINE) {
        mvprintw_color(0, offset, ":%s", state->commandLine.query.c_str(), WHITE);
        offset += state->commandLine.cursor + 1;
        return offset;
    } else if (state->mode == GREP) {
        mvprintw_color(0, offset, "> %s", state->grep.query.c_str(), GREEN);
        offset += state->grep.cursor + 2;
        renderNumMatches(state->grep.query.length() + 4, state->grep.selection + 1, state->grepOutput.size());
        return offset;
    } else if (state->mode == FINDFILE) {
        mvprintw_color(0, offset, "> ", "", YELLOW);
        mvprintw_color(0, offset + 2, "%s", state->findFile.query.c_str(), state->selectAll ? invertColor(YELLOW) : YELLOW);
        offset += state->findFile.cursor + 2;
        renderNumMatches(state->findFile.query.length() + 4, state->findFile.selection + 1, state->findFileOutput.size());
        return offset;
    } else if (state->mode == SEARCH) {
        std::string displayQuery = state->search.query;
        for (size_t i = 0; i < displayQuery.length(); ++i) {
            if (displayQuery[i] == '\n') {
                displayQuery.replace(i, 1, "\\n");
                i++;
            }
        }
        mvprintw_color(0, offset, "/%s", displayQuery.c_str(), state->searchFail ? RED : GREEN);
        offset += displayQuery.length() + 1;
        if (state->replacing) {
            mvprintw_color(0, offset, "/%s", state->replace.query.c_str(), MAGENTA);
            offset += state->replace.query.length() + 1;
        }
        return offset;
    } else {
        if (state->harpoonIndex < state->harpoonFiles.size()) {
            auto min_name = minimize_filename(state->harpoonFiles[state->harpoonIndex]);
            int left = (state->maxX / 2) - (min_name.length() / 2);
            int right = left + min_name.length();
            mvprintw_color(0, left, "%s", min_name.c_str(), state->harpoonFiles[state->harpoonIndex] == state->filename ? YELLOW : GREY);
            std::string renderString = "";
            for (unsigned int i = state->harpoonIndex + 1; i < state->harpoonFiles.size(); i++) {
                min_name = minimize_filename(state->harpoonFiles[i]);
                renderString += "  " + min_name;
            }
            size_t rightBorder = (static_cast<size_t>(state->maxX - RIGHT_STATUS_BORDER) > static_cast<size_t>(right)) ? state->maxX - RIGHT_STATUS_BORDER - right : 0;
            if (renderString.length() > rightBorder && rightBorder > 0) {
                renderString = safeSubstring(renderString, 0, rightBorder);
            }
            mvprintw_color(0, right, "%s", renderString.c_str(), GREY);
            renderString = "";
            for (int i = state->harpoonIndex - 1; i >= 0; i--) {
                min_name = minimize_filename(state->harpoonFiles[i]);
                renderString = min_name + "  " + renderString;
            }
            size_t leftBorder = (left > LEFT_STATUS_BORDER) ? left - LEFT_STATUS_BORDER : 0;
            if (renderString.length() > leftBorder && leftBorder > 0) {
                renderString = safeSubstring(renderString, renderString.length() - leftBorder, leftBorder);
            }
            mvprintw_color(0, left - renderString.length(), "%s", renderString.c_str(), GREY);
        }

        std::string displayQuery = state->search.query;
        for (size_t i = 0; i < displayQuery.length(); ++i) {
            if (displayQuery[i] == '\n') {
                displayQuery.replace(i, 1, "\\n");
                i++;
            }
        }
        if (displayQuery.length() > LEFT_STATUS_BORDER) {
            displayQuery = safeSubstring(displayQuery, 0, LEFT_STATUS_BORDER);
            displayQuery += "...";
        }
        mvprintw_color(0, offset, "/%s", displayQuery.c_str(), state->searchFail ? RED : GREEN);
        offset += displayQuery.length() + 1;
        if (state->replacing) {
            mvprintw_color(0, offset, "/%s", state->replace.query.c_str(), MAGENTA);
            offset += state->replace.query.length() + 1;
        }
        if (state->replacing) {
            cursor = state->search.query.length() + state->replace.cursor + 2;
        } else if (state->mode == SEARCH) {
            cursor = state->search.cursor + 1;
        }
    }
    if (state->showFileStack == true) {
        for (int i = (int)(state->fileStack.size() - 1); i >= 0; i--) {
            mvprintw_color(
                state->fileStack.size() - i - 1,
                state->maxX - state->fileStack[i].length() - 2,
                "\"%s\"",
                state->fileStack[i].c_str(),
                i == (int)state->fileStackIndex ? RED : WHITE
            );
        }
    } else {
        auto displayFileName = setStringToLength(state->filename, RIGHT_STATUS_BORDER);
        mvprintw_color(0, state->maxX - (displayFileName.length() + 2), "\"%s\"", displayFileName.c_str(), WHITE);
    }
    return cursor;
}

std::string minimize_filename(const std::string& filename) {
    std::vector<std::string> parts;
    std::stringstream ss(filename);
    std::string part;
    std::string minimized;
    while (std::getline(ss, part, '/')) {
        parts.push_back(part);
    }
    for (size_t i = 0; i < parts.size() - 1; ++i) {
        if (!parts[i].empty()) {
            minimized += parts[i][0];
            minimized += '/';
        }
    }
    minimized += parts.back();
    return minimized;
}

int getColorFromChar(char c) {
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

int getSearchColor(State* state, int row, unsigned int startOfSearch, std::string query, bool grep) {
    if (state->row == (unsigned int)row && startOfSearch + query.length() >= state->col && startOfSearch <= state->col) {
        return invertColor(grep ? GREEN : MAGENTA);
    } else {
        return invertColor(grep ? DARKGREEN : CYAN);
    }
}

bool isMergeConflict(const std::string& str) {
    const std::vector<std::string> markers = {"<<<<<<<", "=======", ">>>>>>>", "|||||||"};
    for (const auto& marker : markers) {
        if (str.length() >= marker.length()) {
            if (str.substr(0, marker.length()) == marker) {
                return true;
            }
        }
    }
    return false;
}

void printChar(State* state, int row, int col, char c, int color) {
    if (' ' <= c && c <= '~') {
        mvaddch_color(row, col + getLineNumberOffset(state), c, color);
    } else if (c == '\t') {
        for (unsigned int i = 0; i < state->indent; i++) {
            mvaddch_color(row, col + getLineNumberOffset(state), ' ', color);
        }
    } else if (' ' <= unctrl(c) && unctrl(c) <= '~') {
        mvaddch_color(row, col + getLineNumberOffset(state), unctrl(c), invertColor(MAGENTA));
    } else {
        mvaddch_color(row, col + getLineNumberOffset(state), ' ', invertColor(MAGENTA));
    }
}

void printChar(State* state, int& row, int& col, char c, int color, bool advance) {
    if (' ' <= c && c <= '~') {
        mvaddch_color(row, col + getLineNumberOffset(state), c, color);
    } else if (c == '\t') {
        for (unsigned int i = 0; i < state->indent; i++) {
            mvaddch_color(row, col + getLineNumberOffset(state), ' ', color);
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

bool isRowColInVisual(State* state, unsigned int i, unsigned int j) {
    if (state->mode == VISUAL) {
        unsigned int minR;
        unsigned int minC;
        unsigned int maxR;
        unsigned int maxC;
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

bool isInQuery(State* state, unsigned int row, unsigned int col, std::string query) {
    if (query != "" && col + query.length() <= state->data[row].length()) {
        if (query == state->data[row].substr(col, query.length())) {
            return true;
        }
    }
    return false;
}

bool isRowInVisual(State* state, int row) { return ((int)state->visual.row <= row && row <= (int)state->row) || ((int)state->row <= row && row <= (int)state->visual.row); }

unsigned int renderAutoComplete(State* state, unsigned int renderRow, unsigned int renderCol) {
    if (state->mode == TYPING || state->mode == MULTICURSOR) {
        std::string completion = autocomplete(state, getCurrentWord(state));
        if (state->col + 1 >= state->data[state->row].length() || !isAlphanumeric(state->data[state->row][state->col])) {
            for (unsigned int i = 0; i < completion.length(); i++) {
                printChar(state, renderRow, renderCol + i, completion[i], GREY);
            }
            return completion.length();
        }
    }
    return 0;
}

void renderGrepOutput(State* state) {
    unsigned int index;
    if ((int)state->grep.selection - ((int)state->maxY / 2) > 0) {
        index = state->grep.selection - state->maxY / 2;
    } else {
        index = 0;
    }
    unsigned int renderIndex = 1;
    int color;
    for (unsigned int i = index; i < state->grepOutput.size() && i < index + state->maxY; i++) {
        color = isTestFile(state->grepOutput[i].path.string()) ? ORANGE : WHITE;
        mvprintw_color(
            renderIndex,
            0,
            "%s:%d %s\n",
            state->grepOutput[i].path.c_str(),
            state->grepOutput[i].lineNum,
            state->grepOutput[i].line.c_str(),
            i == state->grep.selection ? invertColor(color) : color
        );
        renderIndex++;
    }
}

void renderFindFileOutput(State* state) {
    unsigned int index;
    if ((int)state->findFile.selection - ((int)state->maxY / 2) > 0) {
        index = state->findFile.selection - state->maxY / 2;
    } else {
        index = 0;
    }
    unsigned int renderIndex = 1;
    int color;
    for (unsigned int i = index; i < state->findFileOutput.size() && i < index + state->maxY; i++) {
        color = isTestFile(state->findFileOutput[i]) ? ORANGE : WHITE;
        mvprintw_color(
            renderIndex,
            0,
            "%s\n",
            state->findFileOutput[i].c_str(),
            i == state->findFile.selection ? invertColor(color) : color
        );
        renderIndex++;
    }
}

std::string getRenderBlameString(State* state, int i) {
    if (state->mode == BLAME && state->blame.size() >= state->data.size() && i < (int)state->data.size()) {
        return state->blame[i];
    } else {
        return "";
    }
}

Cursor renderVisibleLines(State* state) {
    Cursor cursor{-1, -1};
    int currentRenderRow = 1;
    for (int i = state->windowPosition.row; i < (int)state->data.size() && i < (int)(state->maxY + state->windowPosition.row) - 1; i++) {
        printLineNumber(state, i, currentRenderRow);
        currentRenderRow = printLineContent(state, i, currentRenderRow, &cursor);
    }
    return cursor;
}

void printLineNumber(State* state, int row, int renderRow) {
    int border = state->fileExplorerOpen ? state->fileExplorerSize : 0;
    if (row == (int)state->row) {
        mvprintw_color(renderRow, border, "%5d", row + 1, WHITE);
    } else if (state->recording) {
        mvprintw_color(renderRow, border, "%5d", row + 1, RED);
    } else {
        mvprintw_color(renderRow, border, "%5d", row + 1, GREY);
    }
    bool isLogging = getLoggingRegex(state) != "" && std::regex_search(state->data[row], std::regex(getLoggingRegex(state)));
    bool endsWithSpace = state->data[row].back() == ' ';
    bool isOnMark = (int)state->mark.mark == row && state->mark.filename == state->filename;
    int color = BLACK;
    if (endsWithSpace && state->mode != TYPING) {
        color = RED;
    } else if (isLogging) {
        color = YELLOW;
    } else if (isOnMark) {
        color = CYAN;
    }
    mvprintw_color(renderRow, border + 5, "%c", '|', color);
    if (state->mode == BLAME) {
        mvprintw_color(renderRow, border + 6, "%-65s", getRenderBlameString(state, row).substr(0, 65).c_str(), row == (int)state->row ? invertColor(WHITE) : WHITE);
    }
}

void advancePosition(State* state, int& renderRow, int& renderCol) {
    if (state->wordwrap && (unsigned int)renderCol + 1 >= state->maxX - getLineNumberOffset(state)) {
        renderRow++;
        renderCol = 0;
    } else {
        renderCol++;
    }
}

int printLineContent(State* state, int row, int renderRow, Cursor* cursor) {
    if (isRowColInVisual(state, row, 0) == true && state->data[row].length() == 0) {
        printChar(state, renderRow, 0, ' ', invertColor(WHITE));
        if (state->row == (unsigned int)row) {
            cursor->row = renderRow - 1;
            cursor->col = 0;
        }
    } else {
        bool isInString = false;
        bool skipNext = false;
        unsigned int searchCounter = 0;
        int renderCol = 0;
        unsigned int startOfSearch = 0;
        bool isComment = false;
        char stringType;
        unsigned int col = 0;
        unsigned int completionLength = 0;
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
                    if (isInString == false && (current == '"' || current == '`' || current == '\'')) {
                        isInString = true;
                        stringType = current;
                    } else if (isInString == true && current == stringType) {
                        isInString = false;
                    }
                }
            } else {
                skipNext = false;
            }
            if (!isInString && state->data[row].substr(col, state->commentSymbol.length()) == state->commentSymbol && state->commentSymbol != "") {
                isComment = true;
            }
            if (col >= state->windowPosition.col) {
                if (state->replacing && searchCounter != 0) {
                    for (unsigned int i = 0; i < state->replace.query.length(); i++) {
                        printChar(state, renderRow, renderCol, state->replace.query[i], getSearchColor(state, row, startOfSearch, state->search.query, false), true);
                    }
                    col += state->search.query.length();
                    searchCounter = 0;
                } else {
                    // TODO make this a getColor function();, lots of logic in here that's messy
                    int color;
                    if (state->matching.row == (unsigned int)row && state->matching.col == col && (state->matching.row != state->row || state->matching.col != state->col)) {
                        color = invertColor(GREY);
                    } else {
                        if (state->showGrep && searchCounter != 0) {
                            color = getSearchColor(state, row, startOfSearch, state->grep.query, true);
                        } else if (searchCounter != 0 && state->searching == true) {
                            color = getSearchColor(state, row, startOfSearch, state->search.query, false);
                        } else {
                            if (isMergeConflict(state->data[row])) {
                                color = RED;
                            } else if (isComment) {
                                color = GREEN;
                            } else if (isInString == true && getExtension(state->filename) != "md" && getExtension(state->filename) != "txt") {
                                color = CYAN;
                            } else {
                                color = getColorFromChar(state->data[row][col]);
                            }
                            if (isRowColInVisual(state, row, col)) {
                                color = invertColor(color);
                            }
                        }
                    }
                    if (state->row == (unsigned int)row && state->col == col) {
                        cursor->row = renderRow - 1;
                        cursor->col = renderCol;
                        if (completionLength != 0) {
                            renderCol += completionLength;
                            completionLength = 0;
                        }
                    }
                    printChar(state, renderRow, renderCol, state->data[row][col], color, true);
                    col++;
                    if (state->row == (unsigned int)row && state->col == col) {
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
            if (state->row == (unsigned int)row) {
                cursor->row = renderRow - 1;
                cursor->col = renderCol;
            }
        }
    }
    return renderRow + 1;
}

void moveCursor(State* state, int cursorOnStatusBar, Cursor editorCursor, Cursor fileExplorerCursor) {
    if (cursorOnStatusBar != -1) {
        move(0, cursorOnStatusBar);
    } else if (state->mode == FILEEXPLORER) {
        move(fileExplorerCursor.row, fileExplorerCursor.col);
    } else {
        move(editorCursor.row + 1, editorCursor.col + getLineNumberOffset(state));
    }
}

int renderFileExplorerNode(FileExplorerNode* node, int r, int selectedRow, std::string startingSpaces, bool isFileExplorerOpen, int fileExplorerWindowLine, int fileExplorerSize, Cursor &cursor) {
    int color;
    if (shouldIgnoreFile(node->path)) {
        color = GREY;
    } else if (node->isFolder) {
        color = CYAN;
    } else {
        color = WHITE;
    }
    int row = r + 1;
    int offset = 0;
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
            displayRow,
            offset,
            (std::string("%-") + std::to_string(fileExplorerSize - offset - 1) + std::string("s")).c_str(),
            (int)node->name.length() >= fileExplorerSize - offset - 1
                ? (node->name.substr(0, fileExplorerSize - 3 - offset - 1) + "...").c_str()
                : node->name.substr(0, fileExplorerSize - offset - 1).c_str(),
            color
        );
        offset += node->name.substr(0, fileExplorerSize - offset - 1).length();
    }
    if (node->isOpen) {
        for(unsigned int i = 0; i < node->children.size(); i++) {
            row = renderFileExplorerNode(&node->children[i], row, selectedRow, startingSpaces + "  ", isFileExplorerOpen, fileExplorerWindowLine, fileExplorerSize, cursor);
        }
    }
    return row;
}

Cursor renderFileExplorer(State* state) {
    Cursor cursor{-1, -1};
    renderFileExplorerNode(state->fileExplorer, 0, state->fileExplorerIndex, std::string(""), state->mode == FILEEXPLORER, state->fileExplorerWindowLine, state->fileExplorerSize, cursor);
    return cursor;
}

void renderScreen(State* state, bool fullRedraw) {
    clear();
    renderScreen(state);
}

void renderScreen(State* state) {
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
    int cursorOnStatusBar = renderStatusBar(state);
    moveCursor(state, cursorOnStatusBar, editorCursor, fileExplorerCursor);
    wnoutrefresh(stdscr);
    doupdate();
}

void initTerminal() {
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
