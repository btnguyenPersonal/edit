#include <string>
#include <iostream>
#include <vector>
#include <ncurses.h>
#include "util/helper.cpp"
#include "util/render.cpp"
#include "util/state.cpp"
#include "util/sendKeys.cpp"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "usage: edit [file]" << std::endl;
        exit(1);
    }
    State state(argv[1]);
    initscr();
    renderScreen(state);
    char c;
    while (c = getchar()) {
        sendKeys(&state, c);
        renderScreen(state);
    }
    endwin();
    return 0;
}