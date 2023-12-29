#include <ncurses.h>
#include "state.h"
#include "modes.h"

#define WHITE_TEXT 1
#define RED_TEXT 2

void initColors() {
    // TODO get colors into own file and enforce 256 colors
    init_pair(WHITE_TEXT, COLOR_WHITE, COLOR_BLACK);
    init_pair(RED_TEXT, COLOR_RED, COLOR_BLACK);
}

void renderStatusBar(State state) {
    if (state.status.length() > 0) {
        attron(COLOR_PAIR(RED_TEXT));
        printw("%s ", state.status.c_str());
        attroff(COLOR_PAIR(RED_TEXT));
    }
    if (state.mode == COMMANDLINE) {
        printw(":%s", state.commandLineQuery.c_str());
    }
    printw("\n");
}

void renderScreen(State state) {
    // TODO partial update
    clear();
    initColors();
    renderStatusBar(state);

    for (size_t i = 0; i < state.data.size(); i++) {
        printw("%4ld ", i);
        printw("%s\n", state.data[i].c_str());
    }
    move(state.row + 1, state.col + 5);
    refresh();
}
