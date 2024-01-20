#include <climits>
#include "../util/state.h"
#include "../util/helper.h"
#include "../util/modes.h"
#include "sendSearchKeys.h"

void sendSearchKeys(State* state, char c) {
    if (c == 27) { // ESC
        state->searchQuery = std::string("");
        state->mode = SHORTCUTS;
    } else if (' ' <= c && c <= '~') {
        state->searchQuery += c;
    } else if (c == 127) { // BACKSPACE
        state->searchQuery = state->searchQuery.substr(0, state->searchQuery.length() - 1);
    } else if (c == ctrl('g')) {
        state->mode = FINDFILE;
    } else if (c == ctrl('l')) {
        state->searchQuery = std::string("");
    } else if (c == ctrl('m')) { // ENTER
        state->mode = SHORTCUTS;
    }
    setSearchResult(state);
}