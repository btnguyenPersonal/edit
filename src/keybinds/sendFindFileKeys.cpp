#include <string>
#include <vector>
#include "../util/state.h"
#include "../util/helper.h"
#include "../util/modes.h"
#include "../util/clipboard.h"
#include "sendFindFileKeys.h"

void sendFindFileKeys(State* state, char c) {
    if (c == 27) { // ESC
        state->selectAll = false;
        state->mode = SHORTCUTS;
    } else if (' ' <= c && c <= '~') {
        if (state->selectAll == true) {
            state->findFileQuery = std::string("");
            state->findFileSelection = 0;
            state->selectAll = false;
        }
        state->findFileQuery += c;
        state->findFileSelection = 0;
    } else if (c == 127) { // BACKSPACE
        if (state->selectAll == true) {
            state->findFileQuery = "";
            state->findFileSelection = 0;
            state->selectAll = false;
        } else {
            state->findFileQuery = state->findFileQuery.substr(0, state->findFileQuery.length() - 1);
            state->findFileSelection = 0;
        }
    } else if (c == ctrl('a')) {
        state->selectAll = !state->selectAll;
    } else if (c == ctrl('g')) {
        state->mode = GREP;
    } else if (c == ctrl('l')) {
        state->selectAll = false;
        state->findFileQuery = std::string("");
        state->findFileSelection = 0;
    } else if (c == ctrl('d')) {
        for (unsigned int i = 0; i < state->maxY; i++) {
            if (state->findFileSelection + 1 < state->findFileOutput.size()) {
                state->findFileSelection += 1;
            }
        }
    } else if (c == ctrl('u')) {
        for (unsigned int i = 0; i < state->maxY; i++) {
            if (state->findFileSelection > 0) {
                state->findFileSelection -= 1;
            }
        }
    } else if (c == ctrl('n')) {
        if (state->findFileSelection + 1 < state->findFileOutput.size()) {
            state->findFileSelection += 1;
        }
    } else if (c == ctrl('p')) {
        if (state->findFileSelection > 0) {
            state->findFileSelection -= 1;
        }
    } else if (c == ctrl('r')) {
        if (state->findFileSelection < state->findFileOutput.size()) {
            state->selectAll = false;
            auto selectedFile = state->findFileOutput[state->findFileSelection].string();
            std::filesystem::path currentDir = ((std::filesystem::path) state->filename).parent_path();
            std::filesystem::path relativePath = std::filesystem::relative(selectedFile, currentDir);
            copyToClipboard(relativePath.string());
            state->mode = SHORTCUTS;
        }
    } else if (c == ctrl('y')) {
        if (state->findFileSelection < state->findFileOutput.size()) {
            state->selectAll = false;
            auto selectedFile = state->findFileOutput[state->findFileSelection].string();
            copyToClipboard(selectedFile);
            state->mode = SHORTCUTS;
        }
    } else if (c == ctrl('m')) { // ENTER
        if (state->findFileSelection < state->findFileOutput.size()) {
            state->selectAll = false;
            auto selectedFile = state->findFileOutput[state->findFileSelection].string();
            state->resetState(selectedFile);
        }
    }
    if (state->mode == FINDFILE && c != ctrl('u') && c != ctrl('d') && c != ctrl('p') && c != ctrl('n')) {
        generateFindFileOutput(state);
    }
}
