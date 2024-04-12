#include "sendGrepKeys.h"
#include "../util/clipboard.h"
#include "../util/helper.h"
#include "../util/modes.h"
#include "../util/state.h"
#include <string>
#include <vector>
#include <ncurses.h>

void sendGrepKeys(State* state, int c) {
    if (c == 27) { // ESC
        state->mode = SHORTCUTS;
    } else if (' ' <= c && c <= '~') {
        state->grepQuery += c;
        state->grepSelection = 0;
    } else if (c == KEY_BACKSPACE) {
        state->grepQuery = state->grepQuery.substr(0, state->grepQuery.length() - 1);
        state->grepSelection = 0;
    } else if (c == ctrl('g')) {
        state->mode = FINDFILE;
    } else if (c == ctrl('l')) {
        state->grepQuery = std::string("");
        state->grepSelection = 0;
    } else if (c == ctrl('d')) {
        for (unsigned int i = 0; i < state->maxY; i++) {
            if (state->grepSelection + 1 < state->grepOutput.size()) {
                state->grepSelection += 1;
            }
        }
    } else if (c == ctrl('u')) {
        for (unsigned int i = 0; i < state->maxY; i++) {
            if (state->grepSelection > 0) {
                state->grepSelection -= 1;
            }
        }
    } else if (c == ctrl('n')) {
        if (state->grepSelection + 1 < state->grepOutput.size()) {
            state->grepSelection += 1;
        }
    } else if (c == ctrl('p')) {
        if (state->grepSelection > 0) {
            state->grepSelection -= 1;
        }
    } else if (c == ctrl('r')) {
        if (state->grepSelection < state->grepOutput.size()) {
            std::filesystem::path selectedFile = state->grepOutput[state->grepSelection].path.string();
            std::filesystem::path currentDir = ((std::filesystem::path)state->filename).parent_path();
            std::filesystem::path relativePath = std::filesystem::relative(selectedFile, currentDir);
            copyToClipboard(relativePath.string());
            state->mode = SHORTCUTS;
        }
    } else if (c == ctrl('y')) {
        if (state->grepSelection < state->grepOutput.size()) {
            std::filesystem::path selectedFile = state->grepOutput[state->grepSelection].path.string();
            copyToClipboard(selectedFile);
            state->mode = SHORTCUTS;
        }
    } else if (c == ctrl('v')) {
        state->grepQuery += getFromClipboard();
    } else if (c == 10) { // ENTER
        if (state->grepSelection < state->grepOutput.size()) {
            std::filesystem::path selectedFile = state->grepOutput[state->grepSelection].path;
            int lineNum = state->grepOutput[state->grepSelection].lineNum;
            state->resetState(selectedFile);
            state->row = lineNum - 1;
            setSearchResultCurrentLine(state, state->grepQuery);
        }
    }
    if (state->mode == GREP && c != ctrl('u') && c != ctrl('d') && c != ctrl('p') && c != ctrl('n')) {
        generateGrepOutput(state);
    }
}
