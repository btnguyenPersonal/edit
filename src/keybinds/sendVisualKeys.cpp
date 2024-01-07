#include <string>
#include <vector>
#include <ncurses.h>
#include "../util/state.h"
#include "../util/helper.h"
#include "../util/modes.h"
#include "../util/clipboard.h"

int changeInVisual(State* state) {
    int min = minimum(state->row, state->visual.row);
    int max = maximum(state->row, state->visual.row);
    if (state->visualType == LINE) {
        state->data.erase(state->data.begin() + min, state->data.begin() + max);
        state->data[min] = std::string("");
    }
    return min;
}

int copyInVisual(State* state) {
    int min = minimum(state->row, state->visual.row);
    int max = maximum(state->row, state->visual.row);
    if (state->visualType == LINE) {
        std::string clip = "";
        for (size_t i = min; i <= (uint) max; i++) {
            clip += state->data[i] + "\n";
        }
        copyToClipboard(clip);
    }
    return min;
}

int deleteInVisual(State* state) {
    int min = minimum(state->row, state->visual.row);
    int max = maximum(state->row, state->visual.row);
    if (state->visualType == LINE) {
        state->data.erase(state->data.begin() + min, state->data.begin() + max + 1);
    }
    return min;
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
    } else if (c == 'h') {
        left(state);
    } else if (c == 'l') {
        right(state);
    } else if (c == 'k') {
        up(state);
    } else if (c == 'j') {
        down(state);
    } else if (c == 'G') {
        state->row = state->data.size() - 1;
    } else if (c == 'd') {
        copyInVisual(state);
        state->row = deleteInVisual(state);
        state->mode = SHORTCUTS;
    } else if (c == 'y') {
        state->row = copyInVisual(state);
        state->mode = SHORTCUTS;
    } else if (c == 'c') {
        state->row = changeInVisual(state);
        state->mode = TYPING;
    } else {
        state->status = std::string(1, c) + " <" + std::to_string((int)c) + ">";
    }
}
