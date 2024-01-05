#include <string>
#include <iostream>
#include <vector>
#include <ncurses.h>
#include "util/helper.h"
#include "util/render.h"
#include "util/state.h"
#include "util/modes.h"
#include "keybinds/sendKeys.h"

int main(int argc, char* argv[]) {
    char c;
    if (argc < 2) {
        std::cerr << "usage: edit [file]" << std::endl;
        exit(1);
    }
    State state(argv[1]);
    initTerminal();
    calcWindowBounds();
    renderScreen(&state);
    while (true) {
        c = getchar();
        state.status = std::string("");
        calcWindowBounds();
        sendKeys(&state, c);
        sanityCheckRowColOutOfBounds(&state);
        if (isWindowPositionInvalid(&state)) {
            centerScreen(&state);
        }
        renderScreen(&state);
    }
    endwin();
    return 0;
}