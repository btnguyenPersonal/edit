#include <ncurses.h>
#include <iostream>
#include "state.h"
#include "modes.h"

#define BLACK 232
#define GREY 242
#define RED 196
#define GREEN 46
#define YELLOW 226
#define BLUE 21
#define MAGENTA 201
#define CYAN 51
#define WHITE 231

#define LINE_NUM_OFFSET 6

void initColors() {
    init_pair(BLACK, BLACK, COLOR_BLACK);
    init_pair(GREY, GREY, COLOR_BLACK);
    init_pair(RED, RED, COLOR_BLACK);
    init_pair(GREEN, GREEN, COLOR_BLACK);
    init_pair(YELLOW, YELLOW, COLOR_BLACK);
    init_pair(BLUE, BLUE, COLOR_BLACK);
    init_pair(MAGENTA, MAGENTA, COLOR_BLACK);
    init_pair(CYAN, CYAN, COLOR_BLACK);
    init_pair(WHITE, WHITE, COLOR_BLACK);

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

void printChar(int windowPosition, int i, int j, char c, bool isInString) {
    int color;
    if (isInString == FALSE) {
        color = getColorFromChar(c);
    } else {
        color = CYAN;
    }
    attron(COLOR_PAIR(color));
    mvaddch(i - windowPosition + 1, j + LINE_NUM_OFFSET, c);
    attroff(COLOR_PAIR(color));
}

void printLineNumber(int i, int windowPosition) {
    attron(COLOR_PAIR(GREY));
    mvprintw(i - windowPosition + 1, 0, "%5d ", i + 1);
    attroff(COLOR_PAIR(GREY));
}

void printLine(std::string line, int i, int windowPosition) {
    printLineNumber(i, windowPosition);
    bool isInString = FALSE;
    bool skipNext = FALSE;
    char stringType;
    // TODO if in comment put in green
    // TODO visual mode reversed
    for (int j = 0; j < (int) line.length(); j++) {
        if (skipNext == FALSE) {
            char current = line[j];
            if (isInString && current == '\\') {
                skipNext = TRUE;
            } else {
                if (isInString == FALSE && (current == '"' || current == '`' || current == '\'')) {
                    isInString = TRUE;
                    stringType = current;
                } else if (isInString == TRUE && current == stringType) {
                    isInString = FALSE;
                }
            }
        } else {
            skipNext = FALSE;
        }
        printChar(windowPosition, i, j, line[j], isInString);
    }
}

void renderVisibleLines(State* state) {
    // TODO fix maxX as well
    for (int i = state->windowPosition; i < (int) state->data.size() && i < (int) (state->maxY + state->windowPosition) - 1; i++) {
        printLine(state->data[i], i, state->windowPosition);
    }
}

void moveCursor(State* state, int commandLineCursorPosition) {
    if (commandLineCursorPosition != 0) {
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
    // TODO partial update
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
    keypad(stdscr, TRUE);
    noecho();
    if (has_colors() == FALSE) {
        endwin();
        std::cout << "Your terminal does not support color" << std::endl;
        exit(1);
    }
    start_color();
}
