#include "sendShortcutKeys.h"
#include "sendTypingKeys.h"
#include "sendCommandLineKeys.h"
#include "sendVisualKeys.h"
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
    }
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
    state->status = std::__cxx11::to_string(state->history.size());
}