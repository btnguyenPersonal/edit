#include "../global.h"
#include <string>
#include <vector>
#include "../util/state.h"
#include "../util/helper.h"
#include "../util/modes.h"
#include "sendFindFileKeys.h"

void sendFindFileKeys(State* state, char c) {
    if (c == 27) { // ESC
        state->mode = SHORTCUTS;
    } else if (' ' <= c && c <= '~') {
        state->findFileQuery += c;
        state->findFileSelection = 0;
    } else if (c == 127) { // BACKSPACE
        state->findFileQuery = state->findFileQuery.substr(0, state->findFileQuery.length() - 1);
        state->findFileSelection = 0;
    } else if (c == ctrl('g')) {
        state->mode = GREP;
    } else if (c == ctrl('l')) {
        state->findFileQuery = std::string("");
        state->findFileSelection = 0;
    } else if (c == ctrl('n')) {
        if (state->findFileSelection + 1 < state->findFileOutput.size()) {
            state->findFileSelection += 1;
        }
    } else if (c == ctrl('p')) {
        if (state->findFileSelection > 0) {
            state->findFileSelection -= 1;
        }
    } else if (c == ctrl('m')) { // ENTER
        auto selectedFile = state->findFileOutput[state->findFileSelection].string();
        state->resetState(selectedFile.c_str());
    }
    if (state->mode == FINDFILE && c != ctrl('p') && c != ctrl('n')) {
        generateFindFileOutput(state);
    }
}
