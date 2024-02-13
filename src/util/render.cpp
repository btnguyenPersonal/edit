#include <fstream>
#include <string>
#include <filesystem>
#include <vector>

#include <ncurses.h>
#include <iostream>
#include "state.h"
#include "modes.h"
#include "render.h"
#include "helper.h"
#include "insertLoggingCode.h"

#define _COLOR_BLACK 16
#define _COLOR_GREY 242
#define _COLOR_RED 196
#define _COLOR_GREEN 46
#define _COLOR_YELLOW 226
#define _COLOR_BLUE 21
#define _COLOR_MAGENTA 201
#define _COLOR_CYAN 51
#define _COLOR_WHITE 231

#define BLACK 1
#define GREY 2
#define RED 3
#define GREEN 4
#define YELLOW 5
#define BLUE 6
#define MAGENTA 7
#define CYAN 8
#define WHITE 9

int invertColor(int color) {
    return color + 9;
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

    init_pair(invertColor(BLACK), _COLOR_BLACK, _COLOR_BLACK);
    init_pair(invertColor(GREY), _COLOR_BLACK, _COLOR_GREY);
    init_pair(invertColor(RED), _COLOR_BLACK, _COLOR_RED);
    init_pair(invertColor(GREEN), _COLOR_BLACK, _COLOR_GREEN);
    init_pair(invertColor(YELLOW), _COLOR_BLACK, _COLOR_YELLOW);
    init_pair(invertColor(BLUE), _COLOR_BLACK, _COLOR_BLUE);
    init_pair(invertColor(MAGENTA), _COLOR_BLACK, _COLOR_MAGENTA);
    init_pair(invertColor(CYAN), _COLOR_BLACK, _COLOR_CYAN);
    init_pair(invertColor(WHITE), _COLOR_BLACK, _COLOR_WHITE);
}

int renderStatusBar(State* state) {
    int offset = 0;
    if (state->showFileStack == true) {
        // TODO make work for fileStack > maxY
        for (int i = (int) (state->fileStack.size() - 1); i >= 0; i--) {
            if (i == (int) state->fileStackIndex) {
                attron(COLOR_PAIR(RED));
            }
            mvprintw(state->fileStack.size() - i - 1, state->maxX - state->fileStack[i].length() - 2, "\"%s\"", state->fileStack[i].c_str());
            if (i == (int) state->fileStackIndex) {
                attroff(COLOR_PAIR(RED));
            }
        }
    } else {
        mvprintw(0, state->maxX - state->filename.length() - 2, "\"%s\"", state->filename.c_str());
    }
    if (state->status.length() > 0) {
        attron(COLOR_PAIR(RED));
        mvprintw(0, offset, "%s ", state->status.c_str());
        attroff(COLOR_PAIR(RED));
        offset += state->status.length() + 1;
    }
    if (state->mode == COMMANDLINE) {
        mvprintw(0, offset, ":%s", state->commandLineQuery.c_str());
        offset += state->commandLineQuery.length() + 1;
        return offset;
    } else if (state->mode == GREP) {
        attron(COLOR_PAIR(GREEN));
        mvprintw(0, offset, "> %s", state->grepQuery.c_str());
        attroff(COLOR_PAIR(GREEN));
        offset += state->grepQuery.length() + 2;
        return offset;
    } else if (state->mode == FINDFILE) {
        attron(COLOR_PAIR(YELLOW));
        mvprintw(0, offset, "> ");
        attroff(COLOR_PAIR(YELLOW));
        offset += 2;
        if (state->selectAll) {
            attron(COLOR_PAIR(invertColor(YELLOW)));
        }
        mvprintw(0, offset, "%s", state->findFileQuery.c_str());
        offset += state->findFileQuery.length();
        if (state->selectAll) {
            attroff(COLOR_PAIR(invertColor(YELLOW)));
        }
        return offset;
    } else {
        attron(COLOR_PAIR(GREEN));
        mvprintw(0, offset, "/%s", state->searchQuery.c_str());
        offset += state->searchQuery.length() + 1;
        attroff(COLOR_PAIR(GREEN));
        if (state->replacing) {
            attron(COLOR_PAIR(MAGENTA));
            mvprintw(0, offset, "/%s", state->replaceQuery.c_str());
            offset += state->replaceQuery.length() + 1;
            attroff(COLOR_PAIR(MAGENTA));
        }
        for (unsigned int i = 0; i < state->harpoonFiles.size(); i++) {
            if (state->harpoonFiles[i] == state->filename) {
                attron(COLOR_PAIR(YELLOW));
            } else {
                attron(COLOR_PAIR(GREY));
            }
            auto min_name = minimize_filename(state->harpoonFiles[i]);
            mvprintw(0, offset, " %s", min_name.c_str());
            offset += min_name.length() + 1;
            if (state->harpoonFiles[i] == state->filename) {
                attroff(COLOR_PAIR(YELLOW));
            } else {
                attroff(COLOR_PAIR(GREY));
            }
        }
    }
    return -1;
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

void printChar(State* state, int row, int col, char c, bool isInString, bool isInverted, bool isInSearchQuery, unsigned int startOfSearch, bool isComment, bool isAutoComplete) {
    int color;
    if (isAutoComplete) {
        color = GREY;
    } else if (isInSearchQuery == true && isInverted == false && state->searching == true) {
        if (state->row == (unsigned int) row && startOfSearch + state->searchQuery.length() >= state->col && startOfSearch <= state->col) {
            color = invertColor(MAGENTA);
        } else {
            color = invertColor(CYAN);
        }
    } else if (isComment) {
        color = GREEN;
    } else if (isInString == true) {
        color = CYAN;
    } else {
        color = getColorFromChar(c);
    }
    if (c == '\t') {
        attron(COLOR_PAIR(invertColor(RED)));
    } else if (isInverted == false) {
        attron(COLOR_PAIR(color));
    } else {
        attron(COLOR_PAIR(invertColor(color)));
    }
    mvaddch(row - state->windowPosition.row + 1, col + getLineNumberOffset(state), c);
    if (c == '\t') {
        attroff(COLOR_PAIR(invertColor(RED)));
    } else if (isInverted == false) {
        attroff(COLOR_PAIR(color));
    } else {
        attroff(COLOR_PAIR(invertColor(color)));
    }
}

void printLineNumber(State* state, int r, int i, bool isCurrentRow, bool recording, std::string blame) {
    if (isCurrentRow == true) {
        attron(COLOR_PAIR(WHITE));
        mvprintw(r, 0, "%5d ", i + 1);
        attroff(COLOR_PAIR(WHITE));
    } else if (recording) {
        attron(COLOR_PAIR(RED));
        mvprintw(r, 0, "%5d ", i + 1);
        attroff(COLOR_PAIR(RED));
    } else {
        attron(COLOR_PAIR(GREY));
        mvprintw(r, 0, "%5d ", i + 1);
        attroff(COLOR_PAIR(GREY));
    }
    if (state->mode == BLAME) {
        if (i == (int) state->row) {
            attron(COLOR_PAIR(invertColor(WHITE)));
        }
        mvprintw(r, 6, "%-65s", blame.substr(0, 65).c_str());
        if (i == (int) state->row) {
            attroff(COLOR_PAIR(invertColor(WHITE)));
        }
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

bool isInSearchQuery(State* state, unsigned int row, unsigned int col) {
    if (state->searchQuery != "" && col + state->searchQuery.length() <= state->data[row].length()) {
        if (state->searchQuery == state->data[row].substr(col, state->searchQuery.length())) {
            return true;
        }
    }
    return false;
}

void printLine(State* state, int row) {
    if (isRowColInVisual(state, row, 0) == true && state->data[row].length() == 0) {
        printChar(state, row, 0, ' ', false, true, false, 0, false, false);
    } else {
        bool isInString = false;
        bool skipNext = false;
        unsigned int searchCounter = 0;
        int renderCol = 0;
        unsigned int startOfSearch;
        bool isComment = false;
        char stringType;
        unsigned int col = 0;
        std::string loggingCode = getLoggingCode(state, row);
        if (state->data[row].substr(0, loggingCode.length()) == loggingCode) {
            attron(COLOR_PAIR(invertColor(BLUE)));
            mvprintw(row - state->windowPosition.row + 1, col + getLineNumberOffset(state), "%s", loggingCode.c_str());
            attroff(COLOR_PAIR(invertColor(BLUE)));
            renderCol += loggingCode.length();
            col += loggingCode.length();
        }
        while (col < state->data[row].length() && col < state->windowPosition.col + state->maxX - getLineNumberOffset(state)) {
            renderCol = renderAutoComplete(state, row, col, renderCol);
            if (searchCounter == 0 && isInSearchQuery(state, row, col)) {
                searchCounter = state->searchQuery.length();
                startOfSearch = col;
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
                    for (unsigned int i = 0; i < state->replaceQuery.length(); i++) {
                        printChar(state, row, renderCol, state->replaceQuery[i], false, false, true, startOfSearch, false, false);
                        renderCol++;
                    }
                    col += state->searchQuery.length();
                    searchCounter = 0;
                } else {
                    printChar(state, row, renderCol, state->data[row][col], isInString, isRowColInVisual(state, row, col), searchCounter != 0, startOfSearch, isComment, false);
                    renderCol++;
                    col++;
                }
            } else {
                col++;
            }
            if (searchCounter != 0) {
                searchCounter -= 1;
            }
        }
        renderCol = renderAutoComplete(state, row, col, renderCol);
    }
}

unsigned int renderAutoComplete(State* state, int row, unsigned int col, unsigned int renderCol) {
    if (state->mode == TYPING && row == (int) state->row && col == state->col) {
        std::string completion = autocomplete(state, getCurrentWord(state));
        for (unsigned int i = 0; i < completion.length(); i++) {
            printChar(state, row, col + i, completion[i], false, false, false, 0, false, true);
        }
        return renderCol + completion.length();
    }
    return renderCol;
}

void renderGrepOutput(State* state) {
    unsigned int index;
    if ((int) state->grepSelection - ((int) state->maxY / 2) > 0) {
        index = state->grepSelection - state->maxY / 2;
    } else {
        index = 0;
    }
    unsigned int renderIndex = 1;
    for (unsigned int i = index; i < state->grepOutput.size() && i < index + state->maxY; i++) {
        if (i == state->grepSelection) {
            attron(COLOR_PAIR(invertColor(WHITE)));
            mvprintw(renderIndex, 0, "%s:%d %s\n", state->grepOutput[i].path.c_str(), state->grepOutput[i].lineNum, state->grepOutput[i].line.c_str());
            attroff(COLOR_PAIR(invertColor(WHITE)));
        } else {
            mvprintw(renderIndex, 0, "%s:%d %s\n", state->grepOutput[i].path.c_str(), state->grepOutput[i].lineNum, state->grepOutput[i].line.c_str());
        }
        renderIndex++;
    }
}

void renderFindFileOutput(State* state) {
    unsigned int index;
    if ((int) state->findFileSelection - ((int) state->maxY / 2) > 0) {
        index = state->findFileSelection - state->maxY / 2;
    } else {
        index = 0;
    }
    unsigned int renderIndex = 1;
    for (unsigned int i = index; i < state->findFileOutput.size() && i < index + state->maxY; i++) {
        if (i == state->findFileSelection) {
            attron(COLOR_PAIR(invertColor(WHITE)));
            mvprintw(renderIndex, 0, "%s\n", state->findFileOutput[i].c_str());
            attroff(COLOR_PAIR(invertColor(WHITE)));
        } else {
            mvprintw(renderIndex, 0, "%s\n", state->findFileOutput[i].c_str());
        }
        renderIndex++;
    }
}

void renderVisibleLines(State* state) {
    std::vector<std::string> blame;
    bool blameError = false;
    if (state->mode == BLAME) {
        try {
            blame = getGitBlame(state->filename);
        } catch (const std::exception& e) {
            state->status = "git blame error";
            blameError = true;
        }
    }
    for (int i = state->windowPosition.row; i < (int) state->data.size() && i < (int) (state->maxY + state->windowPosition.row) - 1; i++) {
        printLineNumber(
            state,
            i - state->windowPosition.row + 1,
            i,
            i == (int) state->row,
            state->recording,
            state->mode == BLAME && !blameError && blame.size() >= state->data.size() && i < (int) state->data.size() ? blame[i] : ""
        );
        printLine(state, i);
    }
}

void moveCursor(State* state, int cursorPosition) {
    if (cursorPosition != -1) {
        move(0, cursorPosition);
    } else {
        unsigned int row = state->row + 1;
        if (row > state->windowPosition.row) {
            row -= state->windowPosition.row;
        } else {
            row = 1;
        }
        unsigned int col = state->col + getLineNumberOffset(state);
        if (state->col > state->data[state->row].length()) {
            col = state->data[state->row].length() + getLineNumberOffset(state);
            if (state->windowPosition.col > col) {
                col = getLineNumberOffset(state);
            } else {
                col -= state->windowPosition.col;
            }
        } else if (col > state->windowPosition.col) {
            col -= state->windowPosition.col;
        }
        move(row, col);
    }
}

void renderScreen(State* state) {
    erase();
    if (state->mode == FINDFILE) {
        renderFindFileOutput(state);
    } else if (state->mode == GREP) {
        renderGrepOutput(state);
    } else {
        renderVisibleLines(state);
    }
    int cursorPosition = renderStatusBar(state);
    moveCursor(state, cursorPosition);
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
    initColors();
}
