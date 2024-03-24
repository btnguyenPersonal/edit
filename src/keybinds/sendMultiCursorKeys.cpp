#include "sendMultiCursorKeys.h"
#include "../util/bounds.h"
#include "../util/helper.h"
#include "../util/modes.h"
#include "../util/state.h"
#include "sendVisualKeys.h"
#include <string>
#include <vector>

void sendMultiCursorKeys(State* state, char c) {
    if (!state->dontRecordKey) {
        state->motion += c;
    }
    Bounds bounds = getBounds(state);
    if (c == 27) { // ESC
        left(state);
        state->mode = SHORTCUTS;
        state->dotCommand = state->motion;
        state->motion = "";
        return;
    } else if (c == 127) { // BACKSPACE
        if (state->col > 0) {
            for (unsigned int i = bounds.minR; i <= bounds.maxR; i++) {
                std::string current = state->data[i];
                state->data[i] = current.substr(0, state->col - 1) + safeSubstring(current, state->col);
            }
            state->col -= 1;
        }
    } else if (' ' <= c && c <= '~') {
        for (unsigned int i = bounds.minR; i <= bounds.maxR; i++) {
            std::string current = state->data[i];
            state->data[i] = current.substr(0, state->col) + c + safeSubstring(current, state->col);
        }
        state->col += 1;
    } else if (c == ctrl('t')) {
        for (unsigned int i = bounds.minR; i <= bounds.maxR; i++) {
            std::string current = state->data[i];
            state->data[i] = current.substr(0, state->col) + '\t' + safeSubstring(current, state->col);
        }
        state->col += 1;
    } else if (c == ctrl('i')) { // TAB
        std::string completion = autocomplete(state, getCurrentWord(state));
        for (unsigned int i = bounds.minR; i <= bounds.maxR; i++) {
            std::string current = state->data[i];
            state->data[i] = current.substr(0, state->col) + completion + safeSubstring(current, state->col);
        }
        state->col += completion.length();
    }
}