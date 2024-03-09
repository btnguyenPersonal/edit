#include "sendShortcutKeys.h"
#include "../util/clipboard.h"
#include "../util/comment.h"
#include "../util/helper.h"
#include "../util/history.h"
#include "../util/indent.h"
#include "../util/insertLoggingCode.h"
#include "../util/modes.h"
#include "../util/state.h"
#include "../util/visualType.h"
#include "sendKeys.h"
#include "sendVisualKeys.h"
#include <algorithm>
#include <climits>
#include <ncurses.h>
#include <string>
#include <vector>

void sendShortcutKeys(State* state, char c) {
    if (c == 27) { // ESC
        state->prevKeys = "";
        state->motion = "";
        return;
    } else if (state->prevKeys == "t") {
        state->col = toNextChar(state, c);
        state->prevKeys = "";
        return;
    } else if (state->prevKeys == "f") {
        state->col = findNextChar(state, c);
        state->prevKeys = "";
        return;
    } else if (state->prevKeys == "r") {
        state->motion = "r";
        if (state->col < state->data[state->row].length()) {
            state->data[state->row] = state->data[state->row].substr(0, state->col) + c + state->data[state->row].substr(state->col + 1);
        }
        state->prevKeys = "";
    } else if ((state->prevKeys[0] == 'y' || state->prevKeys[0] == 'd' || state->prevKeys[0] == 'c') && state->prevKeys.length() == 2) {
        unsigned int tempRow = state->row;
        unsigned int tempCol = state->col;
        char command0 = state->prevKeys[0];
        char command1 = state->prevKeys[1];
        state->prevKeys = "";
        state->motion = "v";
        bool success = true;
        initVisual(state, NORMAL);
        sendVisualKeys(state, command1);
        success = sendVisualKeys(state, c);
        if (success) {
            sendVisualKeys(state, command0);
        } else {
            state->prevKeys = "";
            state->motion = "";
            state->row = tempRow;
            state->col = tempCol;
            state->mode = SHORTCUTS;
        }
        return;
    } else if (state->prevKeys == "y" || state->prevKeys == "d" || state->prevKeys == "c") {
        if (c == 'i' || c == 'a' || c == 'f' || c == 't') {
            state->prevKeys += c;
        } else {
            unsigned int tempRow = state->row;
            unsigned int tempCol = state->col;
            char command = state->prevKeys[0];
            state->prevKeys = "";
            state->motion = "V";
            bool success = true;
            initVisual(state, LINE);
            if (c != command) {
                success = sendVisualKeys(state, c);
            }
            if (success) {
                sendVisualKeys(state, command);
            } else {
                state->prevKeys = "";
                state->motion = "";
                state->row = tempRow;
                state->col = tempCol;
                state->mode = SHORTCUTS;
            }
            return;
        }
    } else if (state->prevKeys + c == "gm") {
        toggleLoggingCode(state, state->lastLoggingVar);
        state->prevKeys = "";
        state->motion = "gm";
        return;
    } else if (state->prevKeys + c == "ge") {
        unCommentBlock(state);
        state->prevKeys = "";
        state->motion = "ge";
        return;
    } else if (state->prevKeys + c == "gt") {
        for (unsigned int i = 0; i < state->data.size(); i++) {
            rtrim(state->data[i]);
        }
        state->prevKeys = "";
    } else if (state->prevKeys + c == "gy") {
        state->status = state->filename;
        copyToClipboard(state->filename);
        state->prevKeys = "";
        return;
    } else if (state->prevKeys + c == "gg") {
        state->row = 0;
        state->prevKeys = "";
        return;
    } else if (state->prevKeys != "") {
        state->prevKeys = "";
    } else if (c == ':') {
        state->mode = COMMANDLINE;
        return;
    } else if (c == '<') {
        deindent(state);
        state->col = getIndexFirstNonSpace(state);
    } else if (c == '>') {
        indent(state);
        state->col = getIndexFirstNonSpace(state);
    } else if (c == 'u') {
        if (state->historyPosition >= 0) {
            state->row = applyDiff(state, state->history[state->historyPosition], false);
            state->historyPosition--;
        }
        return;
    } else if (c == ctrl('r')) {
        if (state->historyPosition < ((int)state->history.size()) - 1) {
            state->row = applyDiff(state, state->history[state->historyPosition + 1], true);
            state->historyPosition++;
        }
        return;
    } else if (c == ctrl('i')) {
        if (state->fileStackIndex + 1 < state->fileStack.size()) {
            state->fileStackIndex += 1;
        }
        if (std::filesystem::is_regular_file(state->fileStack[state->fileStackIndex].c_str())) {
            state->changeFile(state->fileStack[state->fileStackIndex]);
            state->showFileStack = true;
        } else {
            state->status = "file not found";
            state->fileStack.erase(state->fileStack.begin() + state->fileStackIndex);
        }
        return;
    } else if (c == ctrl('o')) {
        if (state->fileStackIndex > 0) {
            state->fileStackIndex -= 1;
        }
        if (std::filesystem::is_regular_file(state->fileStack[state->fileStackIndex].c_str())) {
            state->changeFile(state->fileStack[state->fileStackIndex]);
            state->showFileStack = true;
        } else {
            state->status = "file not found";
            state->fileStack.erase(state->fileStack.begin() + state->fileStackIndex);
        }
        return;
    } else if (c == 'r' || c == 'g' || c == 'c' || c == 'd' || c == 'y' || c == 'f' || c == 't') {
        state->prevKeys = c;
        return;
    } else if (c == 'h') {
        left(state);
        return;
    } else if (c == 'l') {
        right(state);
        return;
    } else if (c == 'k') {
        up(state);
        return;
    } else if (c == 'j') {
        down(state);
        return;
    } else if (c == '{') {
        state->row = getPrevEmptyLine(state);
        return;
    } else if (c == '}') {
        state->row = getNextEmptyLine(state);
        return;
    } else if (c == '[') {
        state->row = getPrevLineSameIndent(state);
        return;
    } else if (c == ']') {
        state->row = getNextLineSameIndent(state);
        return;
    } else if (c == '#') {
        initVisual(state, NORMAL);
        setStateFromWordPosition(state, getWordPosition(state->data[state->row], state->col));
        state->grepQuery = getInVisual(state);
        state->grepSelection = 0;
        state->mode = GREP;
        generateGrepOutput(state);
        return;
    } else if (c == '*') {
        initVisual(state, NORMAL);
        setStateFromWordPosition(state, getWordPosition(state->data[state->row], state->col));
        state->searchQuery = getInVisual(state);
        state->searching = true;
        state->col += 1;
        unsigned int temp_col = state->col;
        unsigned int temp_row = state->row;
        bool result = setSearchResult(state);
        if (result == false) {
            state->row = temp_row;
            state->col = temp_col - 1;
        }
        centerScreen(state);
        return;
    } else if (c == ctrl('g')) {
        state->mode = GREP;
        generateGrepOutput(state);
        return;
    } else if (c == ctrl('p')) {
        state->mode = FINDFILE;
        state->selectAll = true;
        return;
    } else if (c == ctrl('v')) {
        state->mode = VISUAL;
        initVisual(state, BLOCK);
    } else if (c == 'v') {
        state->mode = VISUAL;
        initVisual(state, NORMAL);
    } else if (c == 'V') {
        state->mode = VISUAL;
        initVisual(state, LINE);
    } else if (c == 'b') {
        state->col = b(state);
        return;
    } else if (c == 'w') {
        state->col = w(state);
        return;
    } else if (c == 'i') {
        state->mode = TYPING;
    } else if (c == 'a') {
        right(state);
        state->mode = TYPING;
    } else if (c == ctrl('u')) {
        upHalfScreen(state);
        return;
    } else if (c == ctrl('d')) {
        downHalfScreen(state);
        return;
    } else if (c == 'o') {
        insertEmptyLineBelow(state);
        down(state);
        state->mode = TYPING;
    } else if (c == 'O') {
        insertEmptyLine(state);
        state->mode = TYPING;
    } else if (c == 'Y') {
        fixColOverMax(state);
        copyToClipboard(state->data[state->row].substr(state->col));
        return;
    } else if (c == 'D') {
        fixColOverMax(state);
        copyToClipboard(state->data[state->row].substr(state->col));
        state->data[state->row] = state->data[state->row].substr(0, state->col);
    } else if (c == 'C') {
        fixColOverMax(state);
        copyToClipboard(state->data[state->row].substr(state->col));
        state->data[state->row] = state->data[state->row].substr(0, state->col);
        state->mode = TYPING;
    } else if (c == 'I') {
        state->col = getIndexFirstNonSpace(state);
        state->mode = TYPING;
    } else if (c == 'A') {
        state->col = state->data[state->row].length();
        state->mode = TYPING;
    } else if (c == '\'') {
        if (state->mark.filename != "") {
            state->resetState(state->mark.filename);
            state->row = state->mark.mark;
        }
    } else if (c == '"') {
        state->mark = {state->filename, state->row};
    } else if (c == '@') {
        state->searching = true;
        state->searchQuery = state->grepQuery;
        state->col += 1;
        unsigned int temp_col = state->col;
        unsigned int temp_row = state->row;
        bool result = setSearchResult(state);
        if (result == false) {
            state->row = temp_row;
            state->col = temp_col - 1;
        }
        centerScreen(state);
        return;
    } else if (c == 'N') {
        state->searching = true;
        setSearchResultReverse(state);
        centerScreen(state);
        return;
    } else if (c == 'n') {
        state->searching = true;
        state->col += 1;
        unsigned int temp_col = state->col;
        unsigned int temp_row = state->row;
        bool result = setSearchResult(state);
        if (result == false) {
            state->row = temp_row;
            state->col = temp_col - 1;
        }
        centerScreen(state);
        return;
    } else if (c == '.') {
        for (unsigned int i = 0; i < state->dotCommand.length(); i++) {
            state->dontRecordKey = true;
            sendKeys(state, state->dotCommand[i]);
        }
        state->dontRecordKey = true;
    } else if (c == ',' && state->recording == false) {
        for (unsigned int i = 0; i < state->macroCommand.length(); i++) {
            state->dontRecordKey = true;
            sendKeys(state, state->macroCommand[i]);
        }
        state->dontRecordKey = true;
    } else if (c == 'q') {
        if (state->recording == false) {
            state->macroCommand = "";
        }
        state->recording = !state->recording;
        state->dontRecordKey = true;
        return;
    } else if (c == 'K') {
        state->col = state->data[state->row].length();
        if (state->row + 1 < state->data.size()) {
            ltrim(state->data[state->row + 1]);
            state->data[state->row] += " " + state->data[state->row + 1];
            state->data.erase(state->data.begin() + state->row + 1);
        }
    } else if (c == 'J') {
        state->col = state->data[state->row].length();
        if (state->row + 1 < state->data.size()) {
            ltrim(state->data[state->row + 1]);
            state->data[state->row] += state->data[state->row + 1];
            state->data.erase(state->data.begin() + state->row + 1);
        }
    } else if (c == '/') {
        state->searchQuery = std::string("");
        state->mode = SEARCH;
        return;
    } else if (c == '^') {
        state->col = getIndexFirstNonSpace(state);
        return;
    } else if (c == 's') {
        if (state->col < state->data[state->row].length()) {
            state->data[state->row] = state->data[state->row].substr(0, state->col) + state->data[state->row].substr(state->col + 1);
            state->mode = TYPING;
        }
    } else if (c == 'x') {
        if (state->col < state->data[state->row].length()) {
            copyToClipboard(state->data[state->row].substr(state->col, 1));
            state->data[state->row] = state->data[state->row].substr(0, state->col) + state->data[state->row].substr(state->col + 1);
        }
    } else if (c == ctrl('h')) {
        moveHarpoonLeft(state);
        return;
    } else if (c == ctrl('l')) {
        moveHarpoonRight(state);
        return;
    } else if (c == ctrl('y')) {
        state->col = 0;
        state->mode = BLAME;
        return;
    } else if (c == ctrl('f')) {
        state->mode = SEARCH;
        state->replacing = true;
        return;
    } else if (c == '0') {
        state->col = 0;
        return;
    } else if (c == '$') {
        if (state->data[state->row].length() != 0) {
            state->col = state->data[state->row].length() - 1;
        } else {
            state->col = 0;
        }
        return;
    } else if (c == 'z') {
        fixColOverMax(state);
        state->windowPosition.col = 0;
        centerScreen(state);
        return;
    } else if (c == 'P') {
        pasteFromClipboard(state);
    } else if (c == 'p') {
        pasteFromClipboardAfter(state);
    } else if (c == ctrl('x')) {
        if (state->harpoonFiles.size() != 0) {
            state->harpoonFiles.erase(state->harpoonFiles.begin() + state->harpoonIndex);
        }
        if (state->harpoonFiles.size() == 0) {
            state->harpoonIndex = 0;
        } else if (state->harpoonIndex >= state->harpoonFiles.size()) {
            state->harpoonIndex = state->harpoonFiles.size() - 1;
        }
        state->resetState(state->harpoonFiles[state->harpoonIndex]);
        return;
    } else if (c == 'X') {
        state->harpoonIndex = 0;
        state->harpoonFiles.clear();
        return;
    } else if (c == 'e') {
        toggleComment(state);
        state->col = getIndexFirstNonSpace(state);
    } else if (c == '=') {
        indentLine(state);
        state->col = getIndexFirstNonSpace(state);
    } else if (c == 'Q') {
        removeAllLoggingCode(state);
    } else if (c == 'm') {
        initVisual(state, NORMAL);
        setStateFromWordPosition(state, getWordPosition(state->data[state->row], state->col));
        toggleLoggingCode(state, getInVisual(state));
        state->mode = SHORTCUTS;
    } else if (c == ctrl('e')) {
        if (state->harpoonIndex + 1 < state->harpoonFiles.size()) {
            if (std::filesystem::is_regular_file(state->harpoonFiles[state->harpoonIndex + 1].c_str())) {
                state->harpoonIndex += 1;
                state->resetState(state->harpoonFiles[state->harpoonIndex]);
            } else {
                state->status = "file not found";
                state->harpoonFiles.erase(state->harpoonFiles.begin() + state->harpoonIndex + 1);
            }
        } else {
            if (std::filesystem::is_regular_file(state->harpoonFiles[state->harpoonIndex].c_str())) {
                state->resetState(state->harpoonFiles[state->harpoonIndex]);
            }
        }
        return;
    } else if (c == ctrl('w')) {
        if (state->harpoonIndex > 0) {
            if (std::filesystem::is_regular_file(state->harpoonFiles[state->harpoonIndex - 1].c_str())) {
                state->harpoonIndex -= 1;
                state->resetState(state->harpoonFiles[state->harpoonIndex]);
            } else {
                state->status = "file not found";
                state->harpoonFiles.erase(state->harpoonFiles.begin() + state->harpoonIndex - 1);
                state->harpoonIndex -= 1;
            }
        } else {
            if (std::filesystem::is_regular_file(state->harpoonFiles[state->harpoonIndex].c_str())) {
                state->resetState(state->harpoonFiles[state->harpoonIndex]);
            }
        }
        return;
    } else if (c == '\\') {
        state->changeFile(state->filename);
    } else if (c == ' ') {
        bool found = false;
        for (auto it = state->harpoonFiles.begin(); it != state->harpoonFiles.end();) {
            if (*it == state->filename) {
                it = state->harpoonFiles.erase(it);
                found = true;
                break;
            } else {
                it++;
            }
        }
        if (!found) {
            state->harpoonFiles.push_back(state->filename);
        }
        state->harpoonIndex = state->harpoonFiles.size() - 1;
        return;
    } else if (c == 'G') {
        state->row = state->data.size() - 1;
        return;
    }
    if (!state->dontRecordKey) {
        state->motion += c;
    }
}
