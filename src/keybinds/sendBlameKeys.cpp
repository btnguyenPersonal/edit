#include "sendBlameKeys.h"
#include "../util/clipboard.h"
#include "../util/helper.h"
#include "../util/modes.h"
#include "../util/state.h"
#include <string>
#include <vector>

void sendBlameKeys(State* state, int c) {
    if (c == 27) { // ESC
        state->mode = SHORTCUTS;
    } else if (c == 'k') {
        up(state);
    } else if (c == 'j') {
        down(state);
    } else if (c == '[') {
        state->row = getPrevLineSameIndent(state);
    } else if (c == ']') {
        state->row = getNextLineSameIndent(state);
    } else if (c == ctrl('u')) {
        upHalfScreen(state);
    } else if (c == ctrl('d')) {
        downHalfScreen(state);
    } else if (c == 'y' || c == ctrl('y')) {
        std::string gitHash = getGitHash(state);
        copyToClipboard(gitHash);
        state->status = gitHash;
        state->mode = SHORTCUTS;
    }
}
