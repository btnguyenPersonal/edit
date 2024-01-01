#include <string>
#include <vector>
#include <ncurses.h>
#include "../util/state.h"
#include "../util/helper.h"
#include "../util/modes.h"

void sendShortcutKeys(State* state, char c) {
    if (c == 27) { // ESC
        state->prevKeys = "";
    } else if (c == ':') {
        state->mode = COMMANDLINE;
    } else if (c == 'h') {
        if (state->col > 0) {
            state->col -= 1;
        }
    } else if (c == 'l') {
        if (state->col < state->data[state->row].length()) {
            state->col += 1;
        }
    } else if (c == 'j') {
        down(state);
    } else if (c == 'k') {
        up(state);
    } else if (c == 'i') {
        state->mode = TYPING;
    } else {
        state->status = std::string(1, c) + " <" + std::to_string((int)c) + ">";
    }
}
