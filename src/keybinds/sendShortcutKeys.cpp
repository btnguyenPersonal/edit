#include <string>
#include <vector>
#include "../util/state.h"
#include "../util/modes.h"

void sendShortcutKeys(State* state, char c) {
    if (c == 27) { // ESC
        state->prevKeys = "";
    } else if (c == 'h') {
        if (state->col > 0) {
            state->col -= 1;
        }
    } else if (c == 'l') {
        if (state->col < state->data[state->row].length()) {
            state->col += 1;
        }
    } else if (c == 'j') {
        if (state->row < state->data.size() - 1) {
            state->row += 1;
        }
    } else if (c == 'k') {
        if (state->row > 0) {
            state->row -= 1;
        }
    } else if (c == 'i') {
        state->mode = TYPING;
    } else {
        state->status = std::string(1, c) + " <" + std::to_string((int)c) + ">";
    }
}
