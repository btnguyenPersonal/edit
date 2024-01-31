#include <string>
#include <vector>
#include "../util/state.h"
#include "../util/helper.h"
#include "../util/modes.h"
#include "sendGrepKeys.h"

void sendGrepKeys(State* state, char c) {
    if (c == 27) { // ESC
        state->mode = SHORTCUTS;
    } else if (' ' <= c && c <= '~') {
        state->grepQuery += c;
        state->grepSelection = 0;
    } else if (c == 127) { // BACKSPACE
        state->grepQuery = state->grepQuery.substr(0, state->grepQuery.length() - 1);
        state->grepSelection = 0;
    } else if (c == ctrl('g')) {
        state->mode = FINDFILE;
    } else if (c == ctrl('l')) {
        state->grepQuery = std::string("");
        state->grepSelection = 0;
    } else if (c == ctrl('n')) {
        if (state->grepSelection + 1 < state->grepOutput.size()) {
            state->grepSelection += 1;
        }
    } else if (c == ctrl('p')) {
        if (state->grepSelection > 0) {
            state->grepSelection -= 1;
        }
    } else if (c == ctrl('m')) { // ENTER
        if (state->grepSelection < state->grepOutput.size()) {
            std::filesystem::path selectedFile = state->grepOutput[state->grepSelection].path;
            int lineNum = state->grepOutput[state->grepSelection].lineNum;
            state->resetState(selectedFile);
            state->row = lineNum - 1;
        }
    }
    if (state->mode == GREP && c != ctrl('p') && c != ctrl('n')) {
        generateGrepOutput(state);
    }
}
