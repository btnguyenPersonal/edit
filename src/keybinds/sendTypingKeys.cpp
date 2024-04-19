#include "sendTypingKeys.h"
#include "../util/helper.h"
#include "../util/indent.h"
#include "../util/modes.h"
#include "../util/state.h"
#include <string>
#include <vector>
#include <ncurses.h>

void insertNewline(State* state) {
    std::string current = state->data[state->row];
    state->data[state->row] = current.substr(0, state->col);
    state->data.insert(state->data.begin() + state->row + 1, current.substr(state->col));
}

void sendTypingKeys(State* state, int c) {
    if (!state->dontRecordKey) {
        state->motion += c;
    }
    if (c == 27) { // ESC
        left(state);
        state->mode = SHORTCUTS;
        state->dotCommand = state->motion;
        state->motion = "";
        state->prevKeys = "";
        return;
    } else if (state->prevKeys == "v") {
        state->prevKeys = "";
        if (c == '\n') {
            insertNewline(state);
            state->row += 1;
            state->col = 0;
        } else {
            std::string current = state->data[state->row];
            state->data[state->row] = current.substr(0, state->col) + (char)c + current.substr(state->col);
            if ((int)state->col == getIndexFirstNonSpace(state)) {
                indentLine(state);
                state->col = getIndexFirstNonSpace(state);
            }
            state->col += 1;
        }
    } else if (c == '\n') {
        insertNewline(state);
        state->row += 1;
        state->col = 0;
    } else if (c == KEY_BACKSPACE || c == 127) {
        if (state->col > 0) {
            std::string current = state->data[state->row];
            state->data[state->row] = current.substr(0, state->col - 1) + current.substr(state->col);
            state->col -= 1;
        } else if (state->row > 0) {
            state->col = state->data[state->row - 1].length();
            state->data[state->row - 1] += state->data[state->row];
            state->data.erase(state->data.begin() + state->row);
            state->row -= 1;
        }
    } else if (c == ctrl('w')) {
        std::string current = state->data[state->row];
        unsigned int index = b(state);
        state->data[state->row] = current.substr(0, index) + current.substr(state->col);
        state->col = index;
    } else if (' ' <= c && c <= '~') {
        std::string current = state->data[state->row];
        state->data[state->row] = current.substr(0, state->col) + (char)c + current.substr(state->col);
        if ((int)state->col == getIndexFirstNonSpace(state)) {
            indentLine(state);
            state->col = getIndexFirstNonSpace(state);
        }
        state->col += 1;
    } else if (c == ctrl('t')) {
        std::string current = state->data[state->row];
        state->data[state->row] = current.substr(0, state->col) + '\t' + current.substr(state->col);
        state->col += 1;
    } else if (c == ctrl('v')) {
        state->prevKeys = 'v';
    } else if (c == ctrl('i')) { // TAB
        std::string completion = autocomplete(state, getCurrentWord(state));
        std::string current = state->data[state->row];
        state->data[state->row] = current.substr(0, state->col) + completion + current.substr(state->col);
        state->col += completion.length();
    } else if (c == KEY_LEFT) {
        left(state);
    } else if (c == KEY_RIGHT) {
        right(state);
    } else if (c == KEY_UP) {
        up(state);
    } else if (c == KEY_DOWN) {
        down(state);
    }
}