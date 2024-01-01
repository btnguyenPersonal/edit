#include <string>
#include <vector>
#include <ncurses.h>
#include "../util/state.h"
#include "../util/helper.h"
#include "../util/modes.h"

void sendShortcutKeys(State* state, char c) {
    if (c == 27) { // ESC
        state->prevKeys = "";
    } else if (handleMotion(state, c, "gg")) {
        if (isMotionCompleted(state)) {
            state->row = 0;
        }
    } else if (handleMotion(state, c, "dd")) {
        if (isMotionCompleted(state)) {
            copyToClipboard(state->data[state->row] + "\n");
            state->data.erase(state->data.begin()+state->row);
        }
    } else if (handleMotion(state, c, "yy")) {
        if (isMotionCompleted(state)) {
            copyToClipboard(state->data[state->row] + "\n");
        }
    } else if (handleMotion(state, c, "yk")) {
        if (isMotionCompleted(state)) {
            std::string clip = state->data[state->row];
            if (state->row - 1 >= 0) {
                clip = state->data[state->row - 1] + "\n" + clip;
            }
            copyToClipboard(clip + "\n");
        }
    } else if (handleMotion(state, c, "yj")) {
        if (isMotionCompleted(state)) {
            std::string clip = state->data[state->row];
            if (state->row + 1 < state->data.size()) {
                clip += "\n" + state->data[state->row + 1];
            }
            copyToClipboard(clip + "\n");
        }
    } else if (state->prevKeys != "") {
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
    } else if (c == 'I') {
        state->col = getIndexFirstNonSpace(state);
        state->mode = TYPING;
    } else if (c == 'A') {
        state->col = state->data[state->row].length();
        state->mode = TYPING;
    } else if (c == '^') {
        state->col = getIndexFirstNonSpace(state);
    } else if (c == '0') {
        state->col = 0;
    } else if (c == '$') {
        state->col = state->data[state->row].length();
    } else if (c == 'z') {
        centerScreen(state);
    } else if (c == 'p') {
        state->status = getFromClipboard();
    } else if (c == 'G') {
        state->row = state->data.size() - 1;
    } else {
        state->status = std::string(1, c) + " <" + std::to_string((int)c) + ">";
    }
}
