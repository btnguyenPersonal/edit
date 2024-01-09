#include <ncurses.h>
#include <iostream>
#include "state.h"
#include "modes.h"

#define _COLOR_BLACK 232
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
    if (state->status.length() > 0) {
        attron(COLOR_PAIR(RED));
        mvprintw(0, 0, "%s ", state->status.c_str());
        attroff(COLOR_PAIR(RED));
        offset += state->status.length() + 2;
    }
    if (state->mode == COMMANDLINE) {
        mvprintw(0, offset, ":%s", state->commandLineQuery.c_str());
        offset += state->commandLineQuery.length() + 1;
        return offset;
    }
    return 0;
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

void printLineNumber(int r, int i) {
    attron(COLOR_PAIR(GREY));
    mvprintw(r, 0, "%5d ", i + 1);
    attroff(COLOR_PAIR(GREY));
}

void printLine(std::string line, int row, bool isInverted) {
    if (isInverted == true && line.length() == 0) {
        printChar(row, 0, ' ', false, isInverted);
    } else {
        bool isInString = false;
        bool skipNext = false;
        char stringType;
        // TODO if in comment put in green
        for (int col = 0; col < (int) line.length(); col++) {
            if (skipNext == false) {
                char current = line[col];
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
            printChar(row, col, line[col], isInString, isInverted);
        }
    }
}

bool isRowInVisual(State* state, uint i) {
    if (state->mode == VISUAL) {
        if (state->visualType == LINE) {
            if ((state->row <= i && i <= state->visual.row) || (state->visual.row <= i && i <= state->row)) {
                return true;
            }
        }
    }
    return false;
}

void renderVisibleLines(State* state) {
    // TODO fix maxX as well
    for (int i = state->windowPosition; i < (int) state->data.size() && i < (int) (state->maxY + state->windowPosition) - 1; i++) {
        printLineNumber(i - state->windowPosition + 1, i);
        printLine(state->data[i], i - state->windowPosition + 1, isRowInVisual(state, (uint) i));
    }
}

void moveCursor(State* state, int commandLineCursorPosition) {
    if (state->mode == COMMANDLINE) {
        move(0, commandLineCursorPosition);
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
    initColors();
    renderVisibleLines(state);
    int commandLineCursorPosition = renderStatusBar(state);
    moveCursor(state, commandLineCursorPosition);
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
}
