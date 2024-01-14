#include <string>
#include <vector>
#include <ncurses.h>
#include "../util/state.h"
#include "../util/helper.h"
#include "../util/modes.h"
#include "../util/clipboard.h"

Position changeInVisual(State* state) {
    uint minR;
    uint maxR;
    uint minC;
    uint maxC;
    if (state->row < state->visual.row) {
        minR = state->row;
        minC = state->col;
        maxR = state->visual.row;
        maxC = state->visual.col;
    } else if (state->row > state->visual.row) {
        minR = state->visual.row;
        minC = state->visual.col;
        maxR = state->row;
        maxC = state->col;
    } else {
        minR = state->visual.row;
        maxR = state->row;
        minC = std::min(state->col, state->visual.col);
        maxC = std::max(state->col, state->visual.col);
    }
    if (state->visualType == LINE) {
        state->data.erase(state->data.begin() + minR, state->data.begin() + maxR);
        state->data[minR] = std::string("");
    } else if (state->visualType == NORMAL) {
        state->data[minR] = state->data[minR].substr(0, minC) + state->data[maxR].substr(maxC + 1);
        state->data.erase(state->data.begin() + minR + 1, state->data.begin() + maxR + 1);
    }
    Position pos = Position();
    pos.row = minR;
    pos.col = minC;
    return pos;
}

Position copyInVisual(State* state) {
    uint minR;
    uint maxR;
    uint minC;
    uint maxC;
    if (state->row < state->visual.row) {
        minR = state->row;
        minC = state->col;
        maxR = state->visual.row;
        maxC = state->visual.col;
    } else if (state->row > state->visual.row) {
        minR = state->visual.row;
        minC = state->visual.col;
        maxR = state->row;
        maxC = state->col;
    } else {
        minR = state->visual.row;
        maxR = state->row;
        minC = std::min(state->col, state->visual.col);
        maxC = std::max(state->col, state->visual.col);
    }
    if (state->visualType == LINE) {
        std::string clip = "";
        for (size_t i = minR; i <= maxR; i++) {
            clip += state->data[i] + "\n";
        }
        copyToClipboard(clip);
    } else if (state->visualType == NORMAL) {
        std::string clip = "";
        uint index = minC;
        for (size_t i = minR; i < maxR; i++) {
            while (index < state->data[i].size()) {
                clip += state->data[i][index];
                index += 1;
            }
            index = 0;
            clip += '\n';
        }
        clip += state->data[maxR].substr(index, maxC + 1);
        copyToClipboard(clip);
    }
    Position pos = Position();
    pos.row = minR;
    pos.col = minC;
    return pos;
}

Position deleteInVisual(State* state) {
    uint minR;
    uint maxR;
    uint minC;
    uint maxC;
    if (state->row < state->visual.row) {
        minR = state->row;
        minC = state->col;
        maxR = state->visual.row;
        maxC = state->visual.col;
    } else if (state->row > state->visual.row) {
        minR = state->visual.row;
        minC = state->visual.col;
        maxR = state->row;
        maxC = state->col;
    } else {
        minR = state->visual.row;
        maxR = state->row;
        minC = std::min(state->col, state->visual.col);
        maxC = std::max(state->col, state->visual.col);
    }
    if (state->visualType == LINE) {
        state->data.erase(state->data.begin() + minR, state->data.begin() + maxR + 1);
    } else if (state->visualType == NORMAL) {
        state->data[minR] = state->data[minR].substr(0, minC) + state->data[maxR].substr(maxC + 1);
        state->data.erase(state->data.begin() + minR + 1, state->data.begin() + maxR + 1);
    }
    Position pos = Position();
    pos.row = minR;
    pos.col = minC;
    return pos;
}

void sendVisualKeys(State* state, char c) {
    if (c == 27) { // ESC
        state->mode = SHORTCUTS;
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
    } else if (c == 'p') {
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
