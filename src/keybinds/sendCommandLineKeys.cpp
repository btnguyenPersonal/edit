#include "../global.h"
#include <string>
#include <vector>
#include <ncurses.h>
#include "../util/state.h"
#include "../util/helper.h"
#include "../util/modes.h"
#include "sendCommandLineKeys.h"

void evaluateCommandLineQuery(State* state) {
    if (state->commandLineQuery == "q") {
        endwin();
        exit(0);
    } else if (state->commandLineQuery == "w") {
        saveFile(state->filename, state->data);
    } else if (state->commandLineQuery == "wq" || state->commandLineQuery == "x") {
        saveFile(state->filename, state->data);
        endwin();
        exit(0);
    } else if (is_number(state->commandLineQuery)) {
        uint number = stoul(state->commandLineQuery);
        if (number > 0) {
            state->row = number - 1;
        } else {
            state->row = 0;
        }
    }
}

void sendCommandLineKeys(State* state, char c) {
    if (c == 27) { // ESC
        state->commandLineQuery = std::string("");
        state->mode = SHORTCUTS;
    } else if (' ' <= c && c <= '~') {
        state->commandLineQuery += c;
    } else if (c == 127) { // BACKSPACE
        state->commandLineQuery = state->commandLineQuery.substr(0, state->commandLineQuery.length() - 1);
    } else if (c == 13) { // ENTER
        evaluateCommandLineQuery(state);
        state->commandLineQuery = std::string("");
        state->mode = SHORTCUTS;
    }
}
