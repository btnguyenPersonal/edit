#include "../global.h"

#include <fstream>
#include <string>
#include <filesystem>
#include <vector>

#include <ncurses.h>
#include <iostream>
#include "state.h"
#include "modes.h"
#include "render.h"

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

#define LINE_NUM_OFFSET 6

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
    mvprintw(0, state->maxX - state->filename.length() - 2, "\"%s\"", state->filename.c_str());
    if (state->status.length() > 0) {
        attron(COLOR_PAIR(RED));
        mvprintw(0, 0, "%s ", state->status.c_str());
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
        mvprintw(0, offset, "> %s", state->findFileQuery.c_str());
        attroff(COLOR_PAIR(YELLOW));
        offset += state->findFileQuery.length() + 2;
        return offset;
    } else {
        attron(COLOR_PAIR(GREEN));
        mvprintw(0, offset, "/%s", state->searchQuery.c_str());
        offset += state->searchQuery.length() + 1;
        attroff(COLOR_PAIR(GREEN));
        for (uint i = 0; i < state->harpoonFiles.size(); i++) {
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

void printChar(int row, int col, char c, bool isInString, bool isInverted) {
    int color;
    if (isInString == false) {
        color = getColorFromChar(c);
    } else {
        color = CYAN;
    }
    if (isInverted == false) {
        attron(COLOR_PAIR(color));
    } else {
        attron(COLOR_PAIR(invertColor(color)));
    }
    mvaddch(row, col + LINE_NUM_OFFSET, c);
    if (isInverted == false) {
        attroff(COLOR_PAIR(color));
    } else {
        attroff(COLOR_PAIR(invertColor(color)));
    }
}

void printLineNumber(int r, int i, bool isCurrentRow, bool recording) {
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
}

bool isRowColInVisual(State* state, uint i, uint j) {
    if (state->mode == VISUAL) {
        uint minR;
        uint minC;
        uint maxR;
        uint maxC;
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

void printLine(State* state, int row) {
    if (isRowColInVisual(state, row, 0) == true && state->data[row].length() == 0) {
        printChar(row - state->windowPosition + 1, 0, ' ', false, true);
    } else {
        bool isInString = false;
        bool skipNext = false;
        char stringType;
        // TODO if in comment put in green
        for (int col = 0; col < (int) state->data[row].length(); col++) {
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
            printChar(row - state->windowPosition + 1, col, state->data[row][col], isInString, isRowColInVisual(state, row, col));
        }
    }
}

void renderGrepOutput(State* state) {
    uint index;
    if ((int) state->grepSelection - ((int) state->maxY / 2) > 0) {
        index = state->grepSelection - state->maxY / 2;
    } else {
        index = 0;
    }
    uint renderIndex = 1;
    for (uint i = index; i < state->grepOutput.size() && i < index + state->maxY; i++) {
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
    uint index;
    if ((int) state->findFileSelection - ((int) state->maxY / 2) > 0) {
        index = state->findFileSelection - state->maxY / 2;
    } else {
        index = 0;
    }
    uint renderIndex = 1;
    for (uint i = index; i < state->findFileOutput.size() && i < index + state->maxY; i++) {
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
    // TODO fix maxX as well
    for (int i = state->windowPosition; i < (int) state->data.size() && i < (int) (state->maxY + state->windowPosition) - 1; i++) {
        printLineNumber(i - state->windowPosition + 1, i, i == (int) state->row, state->recording);
        printLine(state, i);
    }
}

void moveCursor(State* state, int cursorPosition) {
    if (cursorPosition != -1) {
        move(0, cursorPosition);
    } else {
        uint row = state->row + 1;
        if (row > state->windowPosition) {
            row -= state->windowPosition;
        } else {
            row = 1;
        }
        uint col = state->col + LINE_NUM_OFFSET;
        if (state->col > state->data[state->row].length()) {
            col = state->data[state->row].length() + LINE_NUM_OFFSET;
        }
        move(row, col);
    }
}

void renderScreen(State* state) {
    clear();
    if (state->mode == FINDFILE) {
        renderFindFileOutput(state);
    } else if (state->mode == GREP) {
        renderGrepOutput(state);
    } else {
        renderVisibleLines(state);
    }
    int cursorPosition = renderStatusBar(state);
    moveCursor(state, cursorPosition);
    refresh();
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
