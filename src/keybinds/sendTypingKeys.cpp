#include <string>
#include <vector>
#include "../util/helper.h"
#include "../util/state.h"
#include "../util/modes.h"
#include "../util/indent.h"
#include "sendTypingKeys.h"

void sendTypingKeys(State* state, char c) {
    if (c == 27) { // ESC
        left(state);
        state->mode = SHORTCUTS;
    } else if (c == 127) { // BACKSPACE
        if (state->col > 0) {
            std::string current = state->data[state->row];
            state->data[state->row] = current.substr(0, state->col - 1) + current.substr(state->col);
            state->col -= 1;
        }
    } else if (' ' <= c && c <= '~') {
        std::string current = state->data[state->row];
        state->data[state->row] = current.substr(0, state->col) + c + current.substr(state->col);
        if ((int) state->col == getIndexFirstNonSpace(state)) {
            indentLine(state);
            state->col = getIndexFirstNonSpace(state);
        }
        state->col += 1;
    } else if (c == ctrl('t')) {
        std::string current = state->data[state->row];
        state->data[state->row] = current.substr(0, state->col) + '\t' + current.substr(state->col);
    } else if (c == ctrl('i')) { // TAB
        std::string completion = autocomplete(state, getCurrentWord(state));
        std::string current = state->data[state->row];
        state->data[state->row] = current.substr(0, state->col) + completion + current.substr(state->col);
        state->col += completion.length();
    } else if (c == ctrl('m')) { // ENTER
        std::string current = state->data[state->row];
        state->data[state->row] = current.substr(0, state->col);
        state->data.insert(state->data.begin() + state->row + 1, current.substr(state->col));
        state->row += 1;
        state->col = 0;
    }
    state->motion += c;
}