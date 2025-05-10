#include "sendKeys.h"
#include "../util/helper.h"
#include "../util/expect.h"
#include "../util/history.h"
#include "../util/modes.h"
#include "../util/state.h"
#include "sendBlameKeys.h"
#include "sendFileExplorerKeys.h"
#include "sendCommandLineKeys.h"
#include "sendFindFileKeys.h"
#include "sendGrepKeys.h"
#include "sendMultiCursorKeys.h"
#include "sendSearchKeys.h"
#include "sendShortcutKeys.h"
#include "sendTypingKeys.h"
#include "sendVisualKeys.h"
#include <ncurses.h>

void sendKeys(State* state, int32_t c) {
    state->showGrep = false;
    state->status = std::string("");
    state->searchFail = false;
    state->showFileStack = false;
    state->searching = state->mode == SEARCH;
    calcWindowBounds();
    if (state->mode == SHORTCUTS) {
        sendShortcutKeys(state, c);
    } else if (state->mode == TYPING) {
        sendTypingKeys(state, c);
    } else if (state->mode == VISUAL) {
        sendVisualKeys(state, c, false);
    } else if (state->mode == COMMANDLINE) {
        sendCommandLineKeys(state, c);
    } else if (state->mode == FINDFILE) {
        sendFindFileKeys(state, c);
    } else if (state->mode == GREP) {
        sendGrepKeys(state, c);
    } else if (state->mode == SEARCH) {
        sendSearchKeys(state, c);
    } else if (state->mode == FILEEXPLORER) {
        sendFileExplorerKeys(state, c);
    } else if (state->mode == BLAME) {
        sendBlameKeys(state, c);
    } else if (state->mode == MULTICURSOR) {
        sendMultiCursorKeys(state, c);
    }
    if (state->mode == SHORTCUTS && state->filename == "") {
        endwin();
        exit(0);
    } else if (state->filename != "") {
        sanityCheckDocumentEmpty(state);
        sanityCheckRowColOutOfBounds(state);
        expect(state->data[state->row].length() >= 0);
        if (isWindowPositionInvalid(state)) {
            centerScreen(state);
        }
        expect(state->windowPosition.row <= state->row);
        expect(state->windowPosition.row + state->maxY > state->row);
        if (state->recording == true && state->dontRecordKey == false) {
            if (!(c == ',' && state->mode == SHORTCUTS)) {
                state->macroCommand += c;
            }
        }
        if (state->recording == false && state->mode == SHORTCUTS) {
            std::vector<diffLine> diff = generateDiff(state->previousState, state->data);
            if (diff.size() != 0) {
                // TODO helper function for recording jumplist
                if (state->mode == SHORTCUTS && c != ctrl('z') && c != ctrl('q')) {
                    if (state->jumplist.list.size() > 0) {
                        auto pos = state->jumplist.list.back();
                        if (pos.row != state->row || pos.col != state->col) {
                            state->jumplist.list.erase(state->jumplist.list.begin() + state->jumplist.index + 1, state->jumplist.list.end());
                            state->jumplist.list.push_back({state->row, state->col});
                            state->jumplist.index = state->jumplist.list.size() - 1;
                        }
                    } else {
                        state->jumplist.list.push_back({state->row, state->col});
                        state->jumplist.index = state->jumplist.list.size() - 1;
                    }
                }
                if (state->options.autosave && !state->unsavedFile) {
                    saveFile(state);
                }
                state->previousState = state->data;
                if (c != ctrl('r') && c != 'u') {
                    if (state->historyPosition < (int32_t)state->history.size()) {
                        state->history.erase(state->history.begin() + state->historyPosition + 1, state->history.end());
                    }
                    state->history.push_back(diff);
                    state->historyPosition = (int32_t)state->history.size() - 1;
                    expect(state->historyPosition >= 0);
                }
            }
        }
        state->matching = matchIt(state);
        for (uint32_t i = 0; i < 10; i++) {
            if (state->harpoonFiles.count(i) > 0 && state->harpoonFiles[i] == state->filename) {
                state->harpoonIndex = i;
            }
        }
    }
    state->dontRecordKey = false;
}