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
    if (argc >= 2 && strcmp(argv[1], "-c") == 0) {
        if (argc == 2) {
            std::cerr << "Usage: editor -c '<sequence_of_letters>'\n";
            return 1;
        }
        state = new State();
        for (int i = 2; i < argc; ++i) {
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
        endwin();
        return 0;
    } else if (argc > 1) {
        state = new State(argv[1]);
    } else {
        state = new State();
        generateFindFileOutput(state);
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
