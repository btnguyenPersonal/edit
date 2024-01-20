#include <string>
#include <vector>
#include <ncurses.h>
#include "../util/state.h"
#include "../util/helper.h"
#include "../util/modes.h"
#include "../util/clipboard.h"
#include "sendVisualKeys.h"

void setStateFromWordPosition(State* state, WordPosition pos) {
    if (pos.min != 0 || pos.max != 0) {
        state->visual.col = pos.min;
        state->col = pos.max;
        state->visual.row = state->row;
    }
}

std::string getInVisual(State* state) {
    Bounds bounds = getBounds(state);
    std::string clip = "";
    if (state->visualType == LINE) {
        for (size_t i = bounds.minR; i <= bounds.maxR; i++) {
            clip += state->data[i] + "\n";
        }
    } else if (state->visualType == NORMAL) {
        unsigned int index = bounds.minC;
        for (size_t i = bounds.minR; i < bounds.maxR; i++) {
            while (index < state->data[i].size()) {
                clip += state->data[i][index];
                index += 1;
            }
            index = 0;
            clip += '\n';
        }
        clip += state->data[bounds.maxR].substr(index, bounds.maxC - index + 1);
    }
    return clip;
}

Bounds getBounds(State* state) {
    Bounds bounds;
    if (state->row < state->visual.row) {
        bounds.minR = state->row;
        bounds.minC = state->col;
        bounds.maxR = state->visual.row;
        bounds.maxC = state->visual.col;
    } else if (state->row > state->visual.row) {
        bounds.minR = state->visual.row;
        bounds.minC = state->visual.col;
        bounds.maxR = state->row;
        bounds.maxC = state->col;
    } else {
        bounds.minR = state->visual.row;
        bounds.maxR = state->row;
        bounds.minC = std::min(state->col, state->visual.col);
        bounds.maxC = std::max(state->col, state->visual.col);
    }
    return bounds;
}

Position changeInVisual(State* state) {
    Bounds bounds = getBounds(state);
    if (state->visualType == LINE) {
        state->data.erase(state->data.begin() + bounds.minR, state->data.begin() + bounds.maxR);
        state->data[bounds.minR] = std::string("");
    } else if (state->visualType == NORMAL) {
        state->data[bounds.minR] = state->data[bounds.minR].substr(0, bounds.minC) + state->data[bounds.maxR].substr(bounds.maxC + 1);
        state->data.erase(state->data.begin() + bounds.minR + 1, state->data.begin() + bounds.maxR + 1);
    }
    Position pos = Position();
    pos.row = bounds.minR;
    pos.col = bounds.minC;
    return pos;
}

Position copyInVisual(State* state) {
    Bounds bounds = getBounds(state);
    copyToClipboard(getInVisual(state));
    Position pos = Position();
    pos.row = bounds.minR;
    pos.col = bounds.minC;
    return pos;
}

Position deleteInVisual(State* state) {
    Bounds bounds = getBounds(state);
    if (state->visualType == LINE) {
        state->data.erase(state->data.begin() + bounds.minR, state->data.begin() + bounds.maxR + 1);
    } else if (state->visualType == NORMAL) {
        state->data[bounds.minR] = state->data[bounds.minR].substr(0, bounds.minC) + state->data[bounds.maxR].substr(bounds.maxC + 1);
        state->data.erase(state->data.begin() + bounds.minR + 1, state->data.begin() + bounds.maxR + 1);
    }
    Position pos = Position();
    pos.row = bounds.minR;
    pos.col = bounds.minC;
    return pos;
}

void sendVisualKeys(State* state, char c) {
    if (c == 27) { // ESC
        state->mode = SHORTCUTS;
    } else if (handleMotion(state, c, "i`")) {
        if (isMotionCompleted(state)) {
            setStateFromWordPosition(state, findQuoteBounds(state->data[state->row], '`', state->col, false));
        }
    } else if (handleMotion(state, c, "a`")) {
        if (isMotionCompleted(state)) {
            setStateFromWordPosition(state, findQuoteBounds(state->data[state->row], '`', state->col, true));
        }
    } else if (handleMotion(state, c, "i\"")) {
        if (isMotionCompleted(state)) {
            setStateFromWordPosition(state, findQuoteBounds(state->data[state->row], '"', state->col, false));
        }
    } else if (handleMotion(state, c, "a\"")) {
        if (isMotionCompleted(state)) {
            setStateFromWordPosition(state, findQuoteBounds(state->data[state->row], '"', state->col, true));
        }
    } else if (handleMotion(state, c, "i'")) {
        if (isMotionCompleted(state)) {
            setStateFromWordPosition(state, findQuoteBounds(state->data[state->row], '\'', state->col, false));
        }
    } else if (handleMotion(state, c, "a'")) {
        if (isMotionCompleted(state)) {
            setStateFromWordPosition(state, findQuoteBounds(state->data[state->row], '\'', state->col, true));
        }
    } else if (handleMotion(state, c, "aT")) {
        if (isMotionCompleted(state)) {
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '>', '<', state->col, true));
        }
    } else if (handleMotion(state, c, "at")) {
        if (isMotionCompleted(state)) {
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '<', '>', state->col, true));
        }
    } else if (handleMotion(state, c, "ad")) {
        if (isMotionCompleted(state)) {
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '[', ']', state->col, true));
        }
    } else if (handleMotion(state, c, "aB")) {
        if (isMotionCompleted(state)) {
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '{', '}', state->col, true));
        }
    } else if (handleMotion(state, c, "ab")) {
        if (isMotionCompleted(state)) {
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '(', ')', state->col, true));
        }
    } else if (handleMotion(state, c, "iT")) {
        if (isMotionCompleted(state)) {
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '>', '<', state->col, false));
        }
    } else if (handleMotion(state, c, "it")) {
        if (isMotionCompleted(state)) {
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '<', '>', state->col, false));
        }
    } else if (handleMotion(state, c, "id")) {
        if (isMotionCompleted(state)) {
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '[', ']', state->col, false));
        }
    } else if (handleMotion(state, c, "iB")) {
        if (isMotionCompleted(state)) {
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '{', '}', state->col, false));
        }
    } else if (handleMotion(state, c, "ib")) {
        if (isMotionCompleted(state)) {
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '(', ')', state->col, false));
        }
    } else if (handleMotion(state, c, "iw")) {
        if (isMotionCompleted(state)) {
            setStateFromWordPosition(state, getWordPosition(state->data[state->row], state->col));
        }
    } else if (handleMotion(state, c, "gg")) {
        if (isMotionCompleted(state)) {
            state->row = 0;
        }
    } else if (c == '^') {
        state->col = getIndexFirstNonSpace(state);
    } else if (c == '0') {
        state->col = 0;
    } else if (c == '$') {
        state->col = state->data[state->row].length();
    } else if (c == 'b') {
        state->col = b(state);
    } else if (c == 'w') {
        state->col = w(state);
    } else if (c == 'h') {
        left(state);
    } else if (c == '[') {
        state->row = getPrevLineSameIndent(state);
    } else if (c == ']') {
        state->row = getNextLineSameIndent(state);
    } else if (c == '#') {
        state->grepQuery = getInVisual(state);
        state->mode = GREP;
        generateGrepOutput(state);
    } else if (c == 'l') {
        right(state);
    } else if (c == 'k') {
        up(state);
    } else if (c == 'j') {
        down(state);
    } else if (c == 'V') {
        state->visualType = LINE;
    } else if (c == 'v') {
        state->visualType = NORMAL;
    } else if (c == 'G') {
        state->row = state->data.size() - 1;
    } else if (c == 'p' || c == 'P') {
        auto pos = deleteInVisual(state);
        state->row = pos.row;
        state->col = pos.col;
        pasteFromClipboard(state);
        state->mode = SHORTCUTS;
    } else if (c == 'd') {
        copyInVisual(state);
        auto pos = deleteInVisual(state);
        state->row = pos.row;
        state->col = pos.col;
        state->mode = SHORTCUTS;
    } else if (c == 'y') {
        auto pos = copyInVisual(state);
        state->row = pos.row;
        state->col = pos.col;
        state->mode = SHORTCUTS;
    } else if (c == 'c') {
        auto pos = changeInVisual(state);
        state->row = pos.row;
        state->col = pos.col;
        state->mode = TYPING;
    } else {
        state->status = std::string(1, c) + " <" + std::to_string((int)c) + ">";
    }
}
