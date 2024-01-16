#include <string>
#include <vector>
#include <ncurses.h>
#include "../util/state.h"
#include "../util/helper.h"
#include "../util/history.h"
#include "../util/modes.h"
#include "../util/clipboard.h"
#include "../util/visualType.h"
#include "sendVisualKeys.h"
#include "sendShortcutKeys.h"

void sendShortcutKeys(State* state, char c) {
    if (c == 27) { // ESC
        state->prevKeys = "";
    } else if (handleMotion(state, c, "gg")) {
        if (isMotionCompleted(state)) {
            state->row = 0;
        }
    } else if (handleMotion(state, c, "caT")) {
        if (isMotionCompleted(state)) {
            state->visualType = NORMAL;
            state->visual.row = state->row;
            state->visual.col = state->col;
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '>', '<', state->col, true));
            copyInVisual(state);
            auto pos = changeInVisual(state);
            state->row = pos.row;
            state->col = pos.col;
            state->mode = TYPING;
        }
    } else if (handleMotion(state, c, "cat")) {
        if (isMotionCompleted(state)) {
            state->visualType = NORMAL;
            state->visual.row = state->row;
            state->visual.col = state->col;
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '<', '>', state->col, true));
            copyInVisual(state);
            auto pos = changeInVisual(state);
            state->row = pos.row;
            state->col = pos.col;
            state->mode = TYPING;
        }
    } else if (handleMotion(state, c, "cad")) {
        if (isMotionCompleted(state)) {
            state->visualType = NORMAL;
            state->visual.row = state->row;
            state->visual.col = state->col;
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '[', ']', state->col, true));
            copyInVisual(state);
            auto pos = changeInVisual(state);
            state->row = pos.row;
            state->col = pos.col;
            state->mode = TYPING;
        }
    } else if (handleMotion(state, c, "caB")) {
        if (isMotionCompleted(state)) {
            state->visualType = NORMAL;
            state->visual.row = state->row;
            state->visual.col = state->col;
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '{', '}', state->col, true));
            copyInVisual(state);
            auto pos = changeInVisual(state);
            state->row = pos.row;
            state->col = pos.col;
            state->mode = TYPING;
        }
    } else if (handleMotion(state, c, "cab")) {
        if (isMotionCompleted(state)) {
            state->visualType = NORMAL;
            state->visual.row = state->row;
            state->visual.col = state->col;
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '(', ')', state->col, true));
            copyInVisual(state);
            auto pos = changeInVisual(state);
            state->row = pos.row;
            state->col = pos.col;
            state->mode = TYPING;
        }
    } else if (handleMotion(state, c, "ciT")) {
        if (isMotionCompleted(state)) {
            state->visualType = NORMAL;
            state->visual.row = state->row;
            state->visual.col = state->col;
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '>', '<', state->col, false));
            copyInVisual(state);
            auto pos = changeInVisual(state);
            state->row = pos.row;
            state->col = pos.col;
            state->mode = TYPING;
        }
    } else if (handleMotion(state, c, "cit")) {
        if (isMotionCompleted(state)) {
            state->visualType = NORMAL;
            state->visual.row = state->row;
            state->visual.col = state->col;
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '<', '>', state->col, false));
            copyInVisual(state);
            auto pos = changeInVisual(state);
            state->row = pos.row;
            state->col = pos.col;
            state->mode = TYPING;
        }
    } else if (handleMotion(state, c, "cid")) {
        if (isMotionCompleted(state)) {
            state->visualType = NORMAL;
            state->visual.row = state->row;
            state->visual.col = state->col;
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '[', ']', state->col, false));
            copyInVisual(state);
            auto pos = changeInVisual(state);
            state->row = pos.row;
            state->col = pos.col;
            state->mode = TYPING;
        }
    } else if (handleMotion(state, c, "ciB")) {
        if (isMotionCompleted(state)) {
            state->visualType = NORMAL;
            state->visual.row = state->row;
            state->visual.col = state->col;
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '{', '}', state->col, false));
            copyInVisual(state);
            auto pos = changeInVisual(state);
            state->row = pos.row;
            state->col = pos.col;
            state->mode = TYPING;
        }
    } else if (handleMotion(state, c, "cib")) {
        if (isMotionCompleted(state)) {
            state->visualType = NORMAL;
            state->visual.row = state->row;
            state->visual.col = state->col;
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '(', ')', state->col, false));
            copyInVisual(state);
            auto pos = changeInVisual(state);
            state->row = pos.row;
            state->col = pos.col;
            state->mode = TYPING;
        }
    } else if (handleMotion(state, c, "ciw")) {
        if (isMotionCompleted(state)) {
            state->visualType = NORMAL;
            state->visual.row = state->row;
            state->visual.col = state->col;
            setStateFromWordPosition(state, getWordPosition(state->data[state->row], state->col));
            copyInVisual(state);
            auto pos = changeInVisual(state);
            state->row = pos.row;
            state->col = pos.col;
            state->mode = TYPING;
        }
    } else if (handleMotion(state, c, "cc")) {
        if (isMotionCompleted(state)) {
            state->visualType = LINE;
            state->visual.row = state->row;
            state->visual.col = state->col;
            copyInVisual(state);
            changeInVisual(state);
            state->col = getIndexFirstNonSpace(state);
            state->mode = TYPING;
        }
    } else if (handleMotion(state, c, "ck")) {
        if (isMotionCompleted(state)) {
            state->visualType = LINE;
            state->visual.row = state->row;
            state->visual.col = state->col;
            up(state);
            copyInVisual(state);
            auto pos = changeInVisual(state);
            state->row = pos.row;
            state->col = getIndexFirstNonSpace(state);
            state->mode = TYPING;
        }
    } else if (handleMotion(state, c, "cj")) {
        if (isMotionCompleted(state)) {
            state->visualType = LINE;
            state->visual.row = state->row;
            state->visual.col = state->col;
            down(state);
            copyInVisual(state);
            auto pos = changeInVisual(state);
            state->row = pos.row;
            state->col = getIndexFirstNonSpace(state);
            state->mode = TYPING;
        }
    } else if (handleMotion(state, c, "daT")) {
        if (isMotionCompleted(state)) {
            state->visualType = NORMAL;
            state->visual.row = state->row;
            state->visual.col = state->col;
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '>', '<', state->col, true));
            copyInVisual(state);
            auto pos = deleteInVisual(state);
            state->row = pos.row;
            state->col = pos.col;
        }
    } else if (handleMotion(state, c, "dat")) {
        if (isMotionCompleted(state)) {
            state->visualType = NORMAL;
            state->visual.row = state->row;
            state->visual.col = state->col;
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '<', '>', state->col, true));
            copyInVisual(state);
            auto pos = deleteInVisual(state);
            state->row = pos.row;
            state->col = pos.col;
        }
    } else if (handleMotion(state, c, "dad")) {
        if (isMotionCompleted(state)) {
            state->visualType = NORMAL;
            state->visual.row = state->row;
            state->visual.col = state->col;
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '[', ']', state->col, true));
            copyInVisual(state);
            auto pos = deleteInVisual(state);
            state->row = pos.row;
            state->col = pos.col;
        }
    } else if (handleMotion(state, c, "daB")) {
        if (isMotionCompleted(state)) {
            state->visualType = NORMAL;
            state->visual.row = state->row;
            state->visual.col = state->col;
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '{', '}', state->col, true));
            copyInVisual(state);
            auto pos = deleteInVisual(state);
            state->row = pos.row;
            state->col = pos.col;
        }
    } else if (handleMotion(state, c, "dab")) {
        if (isMotionCompleted(state)) {
            state->visualType = NORMAL;
            state->visual.row = state->row;
            state->visual.col = state->col;
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '(', ')', state->col, true));
            copyInVisual(state);
            auto pos = deleteInVisual(state);
            state->row = pos.row;
            state->col = pos.col;
        }
    } else if (handleMotion(state, c, "diT")) {
        if (isMotionCompleted(state)) {
            state->visualType = NORMAL;
            state->visual.row = state->row;
            state->visual.col = state->col;
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '>', '<', state->col, false));
            copyInVisual(state);
            auto pos = deleteInVisual(state);
            state->row = pos.row;
            state->col = pos.col;
        }
    } else if (handleMotion(state, c, "dit")) {
        if (isMotionCompleted(state)) {
            state->visualType = NORMAL;
            state->visual.row = state->row;
            state->visual.col = state->col;
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '<', '>', state->col, false));
            copyInVisual(state);
            auto pos = deleteInVisual(state);
            state->row = pos.row;
            state->col = pos.col;
        }
    } else if (handleMotion(state, c, "did")) {
        if (isMotionCompleted(state)) {
            state->visualType = NORMAL;
            state->visual.row = state->row;
            state->visual.col = state->col;
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '[', ']', state->col, false));
            copyInVisual(state);
            auto pos = deleteInVisual(state);
            state->row = pos.row;
            state->col = pos.col;
        }
    } else if (handleMotion(state, c, "diB")) {
        if (isMotionCompleted(state)) {
            state->visualType = NORMAL;
            state->visual.row = state->row;
            state->visual.col = state->col;
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '{', '}', state->col, false));
            copyInVisual(state);
            auto pos = deleteInVisual(state);
            state->row = pos.row;
            state->col = pos.col;
        }
    } else if (handleMotion(state, c, "dib")) {
        if (isMotionCompleted(state)) {
            state->visualType = NORMAL;
            state->visual.row = state->row;
            state->visual.col = state->col;
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '(', ')', state->col, false));
            copyInVisual(state);
            auto pos = deleteInVisual(state);
            state->row = pos.row;
            state->col = pos.col;
        }
    } else if (handleMotion(state, c, "diw")) {
        if (isMotionCompleted(state)) {
            state->visualType = NORMAL;
            state->visual.row = state->row;
            state->visual.col = state->col;
            setStateFromWordPosition(state, getWordPosition(state->data[state->row], state->col));
            copyInVisual(state);
            auto pos = deleteInVisual(state);
            state->row = pos.row;
            state->col = pos.col;
        }
    } else if (handleMotion(state, c, "dd")) {
        if (isMotionCompleted(state)) {
            state->visualType = LINE;
            state->visual.row = state->row;
            state->visual.col = state->col;
            copyInVisual(state);
            deleteInVisual(state);
            state->col = getIndexFirstNonSpace(state);
        }
    } else if (handleMotion(state, c, "dk")) {
        if (isMotionCompleted(state)) {
            state->visualType = LINE;
            state->visual.row = state->row;
            state->visual.col = state->col;
            up(state);
            copyInVisual(state);
            auto pos = deleteInVisual(state);
            state->row = pos.row;
            state->col = getIndexFirstNonSpace(state);
        }
    } else if (handleMotion(state, c, "dj")) {
        if (isMotionCompleted(state)) {
            state->visualType = LINE;
            state->visual.row = state->row;
            state->visual.col = state->col;
            down(state);
            copyInVisual(state);
            auto pos = deleteInVisual(state);
            state->row = pos.row;
            state->col = getIndexFirstNonSpace(state);
        }
    } else if (handleMotion(state, c, "yaT")) {
        if (isMotionCompleted(state)) {
            state->visualType = NORMAL;
            state->visual.row = state->row;
            state->visual.col = state->col;
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '>', '<', state->col, true));
            copyInVisual(state);
            auto pos = deleteInVisual(state);
            state->row = pos.row;
            state->col = pos.col;
        }
    } else if (handleMotion(state, c, "yat")) {
        if (isMotionCompleted(state)) {
            state->visualType = NORMAL;
            state->visual.row = state->row;
            state->visual.col = state->col;
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '<', '>', state->col, true));
            copyInVisual(state);
            auto pos = deleteInVisual(state);
            state->row = pos.row;
            state->col = pos.col;
        }
    } else if (handleMotion(state, c, "yad")) {
        if (isMotionCompleted(state)) {
            state->visualType = NORMAL;
            state->visual.row = state->row;
            state->visual.col = state->col;
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '[', ']', state->col, true));
            auto pos = copyInVisual(state);
            state->row = pos.row;
            state->col = pos.col;
        }
    } else if (handleMotion(state, c, "yaB")) {
        if (isMotionCompleted(state)) {
            state->visualType = NORMAL;
            state->visual.row = state->row;
            state->visual.col = state->col;
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '{', '}', state->col, true));
            auto pos = copyInVisual(state);
            state->row = pos.row;
            state->col = pos.col;
        }
    } else if (handleMotion(state, c, "yab")) {
        if (isMotionCompleted(state)) {
            state->visualType = NORMAL;
            state->visual.row = state->row;
            state->visual.col = state->col;
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '(', ')', state->col, true));
            auto pos = copyInVisual(state);
            state->row = pos.row;
            state->col = pos.col;
        }
    } else if (handleMotion(state, c, "yiT")) {
        if (isMotionCompleted(state)) {
            state->visualType = NORMAL;
            state->visual.row = state->row;
            state->visual.col = state->col;
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '>', '<', state->col, false));
            auto pos = copyInVisual(state);
            state->row = pos.row;
            state->col = pos.col;
        }
    } else if (handleMotion(state, c, "yit")) {
        if (isMotionCompleted(state)) {
            state->visualType = NORMAL;
            state->visual.row = state->row;
            state->visual.col = state->col;
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '<', '>', state->col, false));
            auto pos = copyInVisual(state);
            state->row = pos.row;
            state->col = pos.col;
        }
    } else if (handleMotion(state, c, "yid")) {
        if (isMotionCompleted(state)) {
            state->visualType = NORMAL;
            state->visual.row = state->row;
            state->visual.col = state->col;
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '[', ']', state->col, false));
            auto pos = copyInVisual(state);
            state->row = pos.row;
            state->col = pos.col;
        }
    } else if (handleMotion(state, c, "yiB")) {
        if (isMotionCompleted(state)) {
            state->visualType = NORMAL;
            state->visual.row = state->row;
            state->visual.col = state->col;
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '{', '}', state->col, false));
            auto pos = copyInVisual(state);
            state->row = pos.row;
            state->col = pos.col;
        }
    } else if (handleMotion(state, c, "yib")) {
        if (isMotionCompleted(state)) {
            state->visualType = NORMAL;
            state->visual.row = state->row;
            state->visual.col = state->col;
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '(', ')', state->col, false));
            auto pos = copyInVisual(state);
            state->row = pos.row;
            state->col = pos.col;
        }
    } else if (handleMotion(state, c, "yiw")) {
        if (isMotionCompleted(state)) {
            state->visualType = NORMAL;
            state->visual.row = state->row;
            state->visual.col = state->col;
            setStateFromWordPosition(state, getWordPosition(state->data[state->row], state->col));
            auto pos = copyInVisual(state);
            state->row = pos.row;
            state->col = pos.col;
        }
    } else if (handleMotion(state, c, "yy")) {
        if (isMotionCompleted(state)) {
            state->visualType = LINE;
            state->visual.row = state->row;
            state->visual.col = state->col;
            copyInVisual(state);
            state->col = getIndexFirstNonSpace(state);
        }
    } else if (handleMotion(state, c, "yk")) {
        if (isMotionCompleted(state)) {
            state->visualType = LINE;
            state->visual.row = state->row;
            state->visual.col = state->col;
            up(state);
            auto pos = copyInVisual(state);
            state->row = pos.row;
            state->col = getIndexFirstNonSpace(state);
        }
    } else if (handleMotion(state, c, "yj")) {
        if (isMotionCompleted(state)) {
            state->visualType = LINE;
            state->visual.row = state->row;
            state->visual.col = state->col;
            down(state);
            auto pos = copyInVisual(state);
            state->row = pos.row;
            state->col = getIndexFirstNonSpace(state);
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
    } else if (c == '[') {
        state->row = getPrevLineSameIndent(state);
    } else if (c == ']') {
        state->row = getNextLineSameIndent(state);
    } else if (c == ctrl('g')) {
        state->mode = GREP;
    } else if (c == ctrl('p')) {
        state->mode = FINDFILE;
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

