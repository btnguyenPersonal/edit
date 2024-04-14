#include "keybinds/sendKeys.h"
#include "util/helper.h"
#include "util/render.h"
#include "util/state.h"
#include <cstring>
#include <iostream>
#include <ncurses.h>

int main(int argc, char* argv[]) {
    int c;
    State* state;
    int commandFlag = 0;
    std::string filename;
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-c") == 0) {
            commandFlag = i;
            break;
        } else {
            filename = argv[i];
        }
    }
    if (!filename.empty()) {
        state = new State(filename);
    } else {
        state = new State();
        generateFindFileOutput(state);
    }
    if (commandFlag != 0) {
        state->dontSave = true;
        for (int i = commandFlag + 1; i < argc; ++i) {
            const std::string& sequence = argv[i];
            for (size_t j = 0; j < sequence.size(); ++j) {
                sendKeys(state, sequence[j]);
            }
        }
        return 0;
    }
    initTerminal();
    calcWindowBounds();
    renderScreen(state);
    while (true) {
        c = getch();
        if (c != ERR) {
            recordAction(state);
            sendKeys(state, c);
            renderScreen(state);
        }
    }
    endwin();
    return 0;
}
