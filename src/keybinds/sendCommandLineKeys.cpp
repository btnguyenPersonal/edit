#include "sendCommandLineKeys.h"
#include "../util/helper.h"
#include "../util/modes.h"
#include "../util/state.h"
#include "../util/clipboard.h"
#include <ncurses.h>
#include <sstream>
#include <string>
#include <vector>

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
    } else if (state->commandLineQuery.substr(0, 1) == "s") {
        std::istringstream iss(state->commandLineQuery);
        std::string s, first, second, g;
        if (std::getline(iss, s, '/') && std::getline(iss, first, '/') && std::getline(iss, second, '/')) {
            replaceCurrentLine(state, first, second);
        }
    } else if (state->commandLineQuery.substr(0, 2) == "gs") {
        std::istringstream iss(state->commandLineQuery);
        std::string s, first, second, g;
        if (std::getline(iss, s, '/') && std::getline(iss, first, '/') && std::getline(iss, second, '/')) {
            replaceAllGlobally(state, first, second);
        }
    } else if (state->commandLineQuery.substr(0, 2) == "%s") {
        std::istringstream iss(state->commandLineQuery);
        std::string s, first, second, g;
        if (std::getline(iss, s, '/') && std::getline(iss, first, '/') && std::getline(iss, second, '/')) {
            replaceAll(state, first, second);
        }
    } else if (is_number(state->commandLineQuery)) {
        unsigned int number = stoul(state->commandLineQuery);
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
    } else if (c == ctrl('g')) {
        if (state->commandLineQuery.length() > 0) {
            if (state->commandLineQuery[0] == 'g') {
                state->commandLineQuery = state->commandLineQuery.substr(1);
            } else {
                state->commandLineQuery = 'g' + state->commandLineQuery;
            }
        }
    } else if (c == 127) { // BACKSPACE
        state->commandLineQuery = state->commandLineQuery.substr(0, state->commandLineQuery.length() - 1);
    } else if (c == ctrl('v')) {
        state->commandLineQuery += getFromClipboard();
    } else if (c == 13) { // ENTER
        evaluateCommandLineQuery(state);
        state->commandLineQuery = std::string("");
        state->mode = SHORTCUTS;
    }
}
