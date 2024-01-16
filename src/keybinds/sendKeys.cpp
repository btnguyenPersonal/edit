#include <ncurses.h>
#include "sendKeys.h"
#include "sendShortcutKeys.h"
#include "sendTypingKeys.h"
#include "sendCommandLineKeys.h"
#include "sendVisualKeys.h"
#include "sendFindFileKeys.h"
#include "sendGrepKeys.h"
#include "../util/state.h"
#include "../util/helper.h"
#include "../util/history.h"
#include "../util/modes.h"

void sendKeys(State* state, char c) {
    state->status = std::string("");
    calcWindowBounds();
    if (state->mode == SHORTCUTS) {
        state->previousState = state->data;
        sendShortcutKeys(state, c);
    } else if (state->mode == TYPING) {
        sendTypingKeys(state, c);
    } else if (state->mode == VISUAL) {
        sendVisualKeys(state, c);
    } else if (state->mode == COMMANDLINE) {
        sendCommandLineKeys(state, c);
    } else if (state->mode == FINDFILE) {
        sendFindFileKeys(state, c);
    } else if (state->mode == GREP) {
        sendGrepKeys(state, c);
    }
    if (state->mode == SHORTCUTS && state->filename == "") {
        endwin();
        exit(0);
    }
    sanityCheckDocumentEmpty(state);
    sanityCheckRowColOutOfBounds(state);
    if (isWindowPositionInvalid(state)) {
        centerScreen(state);
    }
    if (state->mode == SHORTCUTS && c != ctrl('r') && c != 'u') {
        std::vector<diffLine> diff = generateDiff(state->previousState, state->data);
        if (diff.size() != 0) {
            if (state->historyPosition < (int) state->history.size()) {
                state->history.erase(state->history.begin() + state->historyPosition + 1, state->history.end());
            }
            state->history.push_back(diff);
            state->historyPosition = (int) state->history.size() - 1;
        }
    }
}