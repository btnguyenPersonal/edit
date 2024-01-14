#include <iostream>
#include <ncurses.h>
#include "util/render.h"
#include "util/state.h"
#include "util/helper.h"
#include "keybinds/sendKeys.h"

int main(int argc, char* argv[]) {
    char c;
    if (argc < 2) {
        std::cerr << "usage: edit [file]" << std::endl;
        exit(1);
    }
    initTerminal();
    const char* filename = argv[1];
    State* state = new State(filename);
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