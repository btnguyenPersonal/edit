#include <string>
#include <vector>
#include "../util/state.h"
#include "../util/modes.h"

void sendTypingKeys(State* state, char c) {
    if (c == 27) { // ESC
        state->mode = SHORTCUTS;
    } else if (c == 127) { // BACKSPACE
        // TODO make delete new lines
        if (state->col > 0) {
            std::string current = state->data[state->row];
            state->data[state->row] = current.substr(0, state->col - 1) + current.substr(state->col);
            state->col -= 1;
        }
    } else if (' ' <= c && c <= '~') {
        std::string current = state->data[state->row];
        state->data[state->row] = current.substr(0, state->col) + c + current.substr(state->col);
        state->col += 1;
    }
}