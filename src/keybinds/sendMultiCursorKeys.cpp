#include <string>
#include <vector>
#include "../util/helper.h"
#include "../util/state.h"
#include "../util/modes.h"
#include "../util/bounds.h"
#include "sendMultiCursorKeys.h"
#include "sendVisualKeys.h"

void sendMultiCursorKeys(State* state, char c) {
    Bounds bounds = getBounds(state);
    if (c == 27) { // ESC
        left(state);
        state->mode = SHORTCUTS;
    } else if (c == 127) { // BACKSPACE
        if (state->col > 0) {
            for (unsigned int i = bounds.minR; i <= bounds.maxR; i++) {
                std::string current = state->data[i];
                state->data[i] = current.substr(0, state->col - 1) + current.substr(state->col);
            }
            state->col -= 1;
        }
    } else if (' ' <= c && c <= '~') {
        for (unsigned int i = bounds.minR; i <= bounds.maxR; i++) {
            std::string current = state->data[i];
            state->data[i] = current.substr(0, state->col) + c + current.substr(state->col);
        }
        state->col += 1;
    } else if (c == ctrl('t')) {
        for (unsigned int i = bounds.minR; i <= bounds.maxR; i++) {
            std::string current = state->data[i];
            state->data[i] = current.substr(0, state->col) + '\t' + current.substr(state->col);
        }
        state->col += 1;
    } else if (c == ctrl('i')) { // TAB
        std::string completion = autocomplete(state, getCurrentWord(state));
        for (unsigned int i = bounds.minR; i <= bounds.maxR; i++) {
            std::string current = state->data[i];
            state->data[i] = current.substr(0, state->col) + completion + current.substr(state->col);
        }
        state->col += completion.length();
    }
    if (!state->dontRecordKey) {
        state->motion += c;
    }
}