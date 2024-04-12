#include "sendSearchKeys.h"
#include "../util/clipboard.h"
#include "../util/helper.h"
#include "../util/modes.h"
#include "../util/state.h"
#include <climits>
#include <ncurses.h>

void sendSearchKeys(State* state, int c) {
    if (c == 27) { // ESC
        state->searchQuery = std::string("");
        state->replaceQuery = std::string("");
        state->replacing = false;
        state->mode = SHORTCUTS;
    } else if (' ' <= c && c <= '~') {
        if (state->replacing) {
            state->replaceQuery += c;
        } else {
            state->searchQuery += c;
        }
    } else if (c == 127) { // BACKSPACE
        if (state->replacing) {
            state->replaceQuery = state->replaceQuery.substr(0, state->replaceQuery.length() - 1);
        } else {
            state->searchQuery = state->searchQuery.substr(0, state->searchQuery.length() - 1);
        }
    } else if (c == ctrl('f')) {
        state->replacing = true;
    } else if (c == ctrl('g')) {
        state->mode = FINDFILE;
    } else if (c == ctrl('l')) {
        if (state->replacing) {
            state->replaceQuery = std::string("");
        } else {
            state->searchQuery = std::string("");
        }
    } else if (c == ctrl('v')) {
        if (state->replacing) {
            state->replaceQuery += getFromClipboard();
        } else {
            state->searchQuery += getFromClipboard();
        }
    } else if (c == 10) { // ENTER
        if (state->replacing) {
            replaceAll(state, state->searchQuery, state->replaceQuery);
        }
        state->replaceQuery = std::string("");
        state->replacing = false;
        state->mode = SHORTCUTS;
    }
    bool result = setSearchResult(state);
    state->searchFail = !result;
}