#include <string>
#include <vector>
#include "../util/state.h"
#include "../util/helper.h"
#include "../util/modes.h"

void sendFindFileKeys(State* state, char c) {
    if (c == 27) { // ESC
        state->mode = SHORTCUTS;
    } else if (' ' <= c && c <= '~') {
        state->findFileQuery += c;
        state->findFileSelection = 0;
    } else if (c == 127) { // BACKSPACE
        state->findFileQuery = state->findFileQuery.substr(0, state->findFileQuery.length() - 1);
        state->findFileSelection = 0;
    } else if (c == ctrl('l')) {
        state->findFileQuery = std::string("");
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
    } else {
        state->status = std::string(1, c) + " <" + std::to_string((int)c) + ">";
    }
    if (state->mode == FINDFILE && c != ctrl('p') && c != ctrl('n')) {
        generateFindFileOutput(state);
    }
}
