#include <string>
#include <iostream>
#include <vector>
#include <ncurses.h>
#include "util/helper.h"
#include "util/render.h"
#include "util/state.h"
#include "keybinds/sendKeys.h"

int main(int argc, char* argv[]) {
    char c;
    int maxY, maxX;
    if (argc < 2) {
        std::cerr << "usage: edit [file]" << std::endl;
       exit(1);
    }
    initscr();
    raw();
    keypad(stdscr, TRUE);
    noecho();
    if (has_colors() == FALSE) {
        endwin();
        std::cout << "Your terminal does not support color" << std::endl;
        return 1;
    }
    start_color();
    State state(argv[1]);
    getmaxyx(stdscr, maxY, maxX);
    State::setMaxYX(maxY, maxX);
    renderScreen(state);
    while (true) {
        state.status = std::string("");
        c = getchar();
        // calc new window barriers
        getmaxyx(stdscr, maxY, maxX);
        State::setMaxYX(maxY, maxX);
        // send keys
        sendKeys(&state, c);
        // if offscreen, center screen
        if (isWindowPositionInvalid(state)) {
            centerScreen(&state);
        }
        renderScreen(state);
    }
    endwin();
    return 0;
}