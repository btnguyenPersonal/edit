#include "sendFindFileKeys.h"
#include "../util/clipboard.h"
#include "../util/helper.h"
#include "../util/modes.h"
#include "../util/render.h"
#include "../util/query.h"
#include "../util/state.h"
#include <ncurses.h>
#include <string>
#include <vector>

void sendFindFileKeys(State* state, int c) {
    if (c == 27) { // ESC
        state->selectAll = false;
        state->mode = SHORTCUTS;
    } else if (' ' <= c && c <= '~') {
        if (state->selectAll == true) {
            backspaceAll(&state->findFile);
            state->findFile.selection = 0;
            state->selectAll = false;
        }
        add(&state->findFile, c);
        state->findFile.selection = 0;
    } else if (c == KEY_LEFT) {
        moveCursorLeft(&state->findFile);
    } else if (c == KEY_RIGHT) {
        moveCursorRight(&state->findFile);
    } else if (c == KEY_BACKSPACE || c == 127) {
        if (state->selectAll == true) {
            backspaceAll(&state->findFile);
            state->findFile.selection = 0;
            state->selectAll = false;
        } else {
            backspace(&state->findFile);
            state->findFile.selection = 0;
        }
    } else if (c == ctrl('a')) {
        state->selectAll = !state->selectAll;
    } else if (c == ctrl('g')) {
        state->mode = GREP;
    } else if (c == ctrl('l')) {
        state->selectAll = false;
        backspaceAll(&state->findFile);
        state->findFile.selection = 0;
    } else if (c == ctrl('d')) {
        for (unsigned int i = 0; i < state->maxY; i++) {
            if (state->findFile.selection + 1 < state->findFileOutput.size()) {
                state->findFile.selection += 1;
            }
        }
    } else if (c == ctrl('u')) {
        for (unsigned int i = 0; i < state->maxY; i++) {
            if (state->findFile.selection > 0) {
                state->findFile.selection -= 1;
            }
        }
    } else if (c == KEY_DOWN || c == ctrl('n')) {
        if (state->findFile.selection + 1 < state->findFileOutput.size()) {
            state->findFile.selection += 1;
        }
    } else if (c == KEY_UP || c == ctrl('p')) {
        if (state->findFile.selection > 0) {
            state->findFile.selection -= 1;
        }
    } else if (c == ctrl('r')) {
        if (state->findFile.selection < state->findFileOutput.size()) {
            state->selectAll = false;
            auto selectedFile = state->findFileOutput[state->findFile.selection].string();
            std::filesystem::path currentDir = ((std::filesystem::path)state->filename).parent_path();
            std::filesystem::path relativePath = std::filesystem::relative(selectedFile, currentDir);
            copyToClipboard(relativePath.string());
            state->mode = SHORTCUTS;
        }
    } else if (c == ctrl('y')) {
        if (state->findFile.selection < state->findFileOutput.size()) {
            state->selectAll = false;
            auto selectedFile = state->findFileOutput[state->findFile.selection].string();
            copyToClipboard(selectedFile);
            state->mode = SHORTCUTS;
        }
    } else if (c == ctrl('v')) {
        if (state->selectAll == true) {
            backspaceAll(&state->findFile);
            state->findFile.selection = 0;
            state->selectAll = false;
        }
        addFromClipboard(&state->findFile);
    } else if (c == ctrl('w')) {
        state->findFile.selection = 0;
    } else if (c == ctrl('e')) {
        for (unsigned int i = 0; i < state->findFileOutput.size(); i++) {
            if (state->findFile.selection + 1 < state->findFileOutput.size()) {
                state->findFile.selection += 1;
            }
            auto selectedFile = state->findFileOutput[state->findFile.selection].string();
            if (isTestFile(selectedFile)) {
                break;
            }
        }
    } else if (c == '\n') {
        if (state->findFile.selection < state->findFileOutput.size()) {
            state->selectAll = false;
            auto selectedFile = state->findFileOutput[state->findFile.selection].string();
            state->resetState(selectedFile);
        }
    }
    if (state->mode == FINDFILE && c != ctrl('u') && c != ctrl('d') && c != ctrl('p') && c != ctrl('n')) {
        renderScreen(state);
        generateFindFileOutput(state);
    }
}
