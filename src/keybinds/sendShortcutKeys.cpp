#include <string>
#include <vector>
#include <ncurses.h>
#include "../util/state.h"
#include "../util/helper.h"
#include "../util/history.h"
#include "../util/modes.h"
#include "../util/clipboard.h"
#include "../util/visualType.h"

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
    } else if (handleMotion(state, c, "dk")) {
        if (isMotionCompleted(state)) {
            std::string clip = state->data[state->row] + "\n";
            if (state->row - 1 >= 0) {
                clip = state->data[state->row - 1] + "\n" + clip;
            }
            state->data.erase(state->data.begin()+state->row);
            if (state->row - 1 >= 0) {
                state->data.erase(state->data.begin()+state->row - 1);
                state->row -= 1;
            }
            state->col = getIndexFirstNonSpace(state);
            copyToClipboard(clip);
        }
    } else if (handleMotion(state, c, "dj")) {
        if (isMotionCompleted(state)) {
            std::string clip = state->data[state->row] + "\n";
            if (state->row + 1 < state->data.size()) {
                clip += state->data[state->row + 1] + "\n";
            }
            state->data.erase(state->data.begin()+state->row);
            if (state->row >= 0) {
                state->data.erase(state->data.begin()+state->row);
            }
            state->col = getIndexFirstNonSpace(state);
            copyToClipboard(clip);
        }
    } else if (handleMotion(state, c, "yy")) {
        if (isMotionCompleted(state)) {
            copyToClipboard(state->data[state->row] + "\n");
            state->col = getIndexFirstNonSpace(state);
        }
    } else if (handleMotion(state, c, "yk")) {
        if (isMotionCompleted(state)) {
            std::string clip = state->data[state->row] + "\n";
            if (state->row - 1 >= 0) {
                clip = state->data[state->row - 1] + "\n" + clip;
                state->row -= 1;
            }
            state->col = getIndexFirstNonSpace(state);
            copyToClipboard(clip);
        }
    } else if (handleMotion(state, c, "yj")) {
        if (isMotionCompleted(state)) {
            std::string clip = state->data[state->row] + "\n";
            if (state->row + 1 < state->data.size()) {
                clip += state->data[state->row + 1] + "\n";
            }
            state->col = getIndexFirstNonSpace(state);
            copyToClipboard(clip);
        }
    } else if (state->prevKeys != "") {
        state->prevKeys = "";
    } else if (c == ':') {
        state->mode = COMMANDLINE;
    } else if (c == '<') {
        deindent(state);
        state->col = getIndexFirstNonSpace(state);
    } else if (c == '>') {
        indent(state);
        state->col = getIndexFirstNonSpace(state);
    } else if (c == 'u') {
        if (state->historyPosition >= 0) {
            state->row = applyDiff(state, state->history[state->historyPosition], false);
            state->historyPosition--;
        }
    } else if (c == ctrl('r')) {
        if (state->historyPosition < ((int) state->history.size()) - 1) {
            state->row = applyDiff(state, state->history[state->historyPosition + 1], true);
            state->historyPosition++;
        }
    } else if (c == 'h') {
        left(state);
    } else if (c == 'l') {
        right(state);
    } else if (c == 'k') {
        up(state);
    } else if (c == 'j') {
        down(state);
    } else if (c == 'v') {
        state->mode = VISUAL;
        state->visualType = NORMAL;
        state->visual.row = state->row;
        state->visual.col = state->col;
    } else if (c == 'V') {
        state->mode = VISUAL;
        state->visualType = LINE;
        state->visual.row = state->row;
        state->visual.col = state->col;
    } else if (c == 'b') {
        state->col = b(state);
    } else if (c == 'w') {
        state->col = w(state);
    } else if (c == 'i') {
        state->mode = TYPING;
    } else if (c == 'a') {
        right(state);
        state->mode = TYPING;
    } else if (c == ctrl('u')) {
        upHalfScreen(state);
    } else if (c == ctrl('d')) {
        downHalfScreen(state);
    } else if (c == 'o') {
        insertEmptyLineBelow(state);
        down(state);
        state->mode = TYPING;
    } else if (c == 'O') {
        insertEmptyLine(state);
        state->mode = TYPING;
    } else if (c == 'Y') {
        copyToClipboard(state->data[state->row].substr(state->col));
    } else if (c == 'D') {
        copyToClipboard(state->data[state->row].substr(state->col));
        state->data[state->row] = state->data[state->row].substr(0, state->col);
    } else if (c == 'C') {
        copyToClipboard(state->data[state->row].substr(state->col));
        state->data[state->row] = state->data[state->row].substr(0, state->col);
        state->mode = TYPING;
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
    } else if (c == 'P') {
        pasteFromClipboard(state);
    } else if (c == 'p') {
        pasteFromClipboardAfter(state);
    } else if (c == 'G') {
        state->row = state->data.size() - 1;
    } else {
        state->status = std::string(1, c) + " <" + std::to_string((int)c) + ">";
    }
}
