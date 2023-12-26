#include <ncurses.h>
#include "state.h"

void renderScreen(State state) {
    // TODO partial update
    clear();
    printw("%s\n", state.status.c_str());
    for (size_t i = 0; i < state.data.size(); i++) {
        printw("%4ld %s\n", i, state.data[i].c_str());
    }
    move(state.row + 1, state.col + 5);
    refresh();
}
