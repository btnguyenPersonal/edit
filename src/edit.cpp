#include <string>
#include <iostream>
#include <vector>
#include <ncurses.h>
#include "util/helper.h"
#include "util/render.h"
#include "util/state.h"
#include "util/modes.h"
#include "util/history.h"
#include "keybinds/sendKeys.h"

int main(int argc, char* argv[]) {
    char c;
    if (argc < 2) {
        std::cerr << "usage: edit [file]" << std::endl;
        exit(1);
    }
    State state(argv[1]);
    // initTerminal();
    calcWindowBounds();
    // renderScreen(&state);
    // while (true) {
        // c = getchar();
        state.status = std::string("");
        calcWindowBounds();
        // TODO have two history copies and just alternate between them for generating diffs
        std::vector<std::string> historyCopy(state.data);
        // sendKeys(&state, c);
        sendKeys(&state, 'o');
        sendKeys(&state, 'h');
        sendKeys(&state, 'e');
        sendKeys(&state, 'l');
        sendKeys(&state, 'l');
        sendKeys(&state, 'o');
        sendKeys(&state, 27);
        sendKeys(&state, 'j');
        sendKeys(&state, 'd');
        sendKeys(&state, 'd');
        sendKeys(&state, 'd');
        sendKeys(&state, 'd');
        sendKeys(&state, 'A');
        sendKeys(&state, ctrl('m'));
        sendKeys(&state, 'h');
        sendKeys(&state, 'e');
        sendKeys(&state, 'l');
        sendKeys(&state, 'l');
        sendKeys(&state, 'o');
        if (state.data.size() == 0) {
            state.data.push_back("");
        }
        sanityCheckRowColOutOfBounds(&state);
        if (isWindowPositionInvalid(&state)) {
            centerScreen(&state);
        }
        std::vector<diffLine> diff = generateDiff(historyCopy, state.data);
        for (uint i = 0; i < diff.size(); i++) {
            std::cout << "Line " << diff[i].lineNum << ": " << (diff[i].add ? "Add: " : "Delete: ") << diff[i].line << std::endl;
        }
        std::cout << std::endl;
        for (uint i = 0; i < historyCopy.size(); i++) {
            std::cout << i << ": " << historyCopy[i] << std::endl;
        }
        std::cout << std::endl;
        for (uint i = 0; i < state.data.size(); i++) {
            std::cout << i << ": " << state.data[i] << std::endl;
        }
        std::cout << std::endl;
        // renderScreen(&state);
    // }
    // endwin();
    return 0;
}