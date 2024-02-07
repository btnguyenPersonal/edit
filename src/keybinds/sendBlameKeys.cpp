#include <string>
#include <vector>
#include "../util/state.h"
#include "../util/helper.h"
#include "../util/modes.h"
#include "../util/clipboard.h"
#include "sendBlameKeys.h"

void sendBlameKeys(State* state, char c) {
    if (c == 27) { // ESC
        state->mode = SHORTCUTS;
    } else if (c == 'k' || c == ctrl('k')) {
        up(state);
    } else if (c == 'j' || c == ctrl('j')) {
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
        copyToClipboard("git show " + gitHash);
        state->status = gitHash;
        state->mode = SHORTCUTS;
    }
}
