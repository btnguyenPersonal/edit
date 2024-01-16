#include <iostream>
#include <ncurses.h>
#include "util/render.h"
#include "util/state.h"
#include "util/helper.h"
#include "keybinds/sendKeys.h"

int main(int argc, char* argv[]) {
    char c;
    State* state;
    if (argc < 2) {
        state = new State();
    } else {
        const char* filename = argv[1];
        state = new State(filename);
    }
    initTerminal();
    calcWindowBounds();
    renderScreen(state);
    while (true) {
        c = getchar();
        sendKeys(state, c);
        renderScreen(state);
    }
    endwin();
    return 0;
}