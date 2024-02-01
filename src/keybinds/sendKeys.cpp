#include <ncurses.h>
#include "sendKeys.h"
#include "sendShortcutKeys.h"
#include "sendTypingKeys.h"
#include "sendCommandLineKeys.h"
#include "sendVisualKeys.h"
#include "sendFindFileKeys.h"
#include "sendGrepKeys.h"
#include "sendSearchKeys.h"
#include "../util/state.h"
#include "../util/helper.h"
#include "../util/history.h"
#include "../util/modes.h"

void sendKeys(State* state, char c) {
    state->status = std::string("");
    state->showFileStack = false;
    state->dontRecordKey = false;
    state->searching = state->mode == SEARCH;
    state->motionComplete = false;
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
    } else if (state->mode == SEARCH) {
        sendSearchKeys(state, c);
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
    if (state->recording == true && state->dontRecordKey == false) {
        state->macroCommand += c;
    }
    if (state->historyPosition == -2) {
        state->historyPosition = -1;
    } else if (state->recording == false && state->mode == SHORTCUTS) {
        std::vector<diffLine> diff = generateDiff(state->previousState, state->data);
        if (diff.size() != 0) {
            saveFile(state->filename, state->data);
        }
        if (diff.size() != 0 && c != ctrl('r') && c != 'u') {
            if (state->dontRecordKey == false) {
                state->dotCommand = state->motion + c;
            }
            if (state->historyPosition < (int) state->history.size()) {
                state->history.erase(state->history.begin() + state->historyPosition + 1, state->history.end());
            }
            state->history.push_back(diff);
            state->historyPosition = (int) state->history.size() - 1;
        }
    }
    if (state->motionComplete) {
        state->prevKeys = "";
    }
}