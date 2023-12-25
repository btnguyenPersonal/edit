#pragma once

#include <ncurses.h>
#include <string>
#include <vector>
#include "state.cpp"

void renderScreen(State state) {
    // TODO partial update
    clear();
    for (int i = 0; i < state.data.size(); i++) {
        printw("%s\n", state.data[i].c_str());
    }
    refresh();
}
