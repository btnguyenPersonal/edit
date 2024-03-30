#include "keybinds/sendKeys.h"
#include "util/helper.h"
#include "util/render.h"
#include "util/state.h"
#include <cstring>
#include <iostream>
#include <ncurses.h>

int main(int argc, char* argv[]) {
    char c;
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
            char ch;
            for (size_t j = 0; j < sequence.size(); ++j) {
                if (sequence[j] == '<' && j + 2 < sequence.size() && sequence[j + 1] == 'c' && sequence[j + 2] == '-') {
                    char control_char = sequence[j + 3];
                    ch = ctrl(control_char);
                    j += 4;
                } else {
                    ch = sequence[j];
                }
                sendKeys(state, ch);
            }
        }
        return 0;
    }
    initTerminal();
    calcWindowBounds();
    renderScreen(state);
    while (true) {
        c = getchar();
        recordAction(state);
        sendKeys(state, c);
        renderScreen(state);
    }
    endwin();
    return 0;
}
