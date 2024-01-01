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
        left(state);
    } else if (c == 'l') {
        right(state);
    } else if (c == 'k') {
        up(state);
    } else if (c == 'j') {
        down(state);
    } else if (c == 'i') {
        state->mode = TYPING;
    } else if (c == 'a') {
        right(state);
        state->mode = TYPING;
    } else if (c == ctrl('u')) {
        upHalfScreen(state);
    } else if (c == ctrl('d')) {
        downHalfScreen(state);
    } else if (c == 'z') {
        centerScreen(state);
    } else {
        state->status = std::string(1, c) + " <" + std::to_string((int)c) + ">";
    }
}
