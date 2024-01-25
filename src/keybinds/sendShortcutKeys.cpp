#include "../global.h"
#include <string>
#include <vector>
#include <climits>
#include <algorithm>
#include <ncurses.h>
#include "../util/state.h"
#include "../util/helper.h"
#include "../util/history.h"
#include "../util/modes.h"
#include "../util/clipboard.h"
#include "../util/visualType.h"
#include "sendVisualKeys.h"
#include "sendKeys.h"
#include "sendShortcutKeys.h"

void sendShortcutKeys(State* state, char c) {
    if (c == 27) { // ESC
        state->prevKeys = "";
    } else if (handleMotion(state, c, "gg")) {
        if (isMotionCompleted(state)) {
            state->row = 0;
        }
    } else if (handleMotion(state, c, "ci`")) {
        if (isMotionCompleted(state)) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findQuoteBounds(state->data[state->row], '`', state->col, false));
            copyInVisual(state);
            setPosition(state, changeInVisual(state));
            state->mode = TYPING;
        }
    } else if (handleMotion(state, c, "ca`")) {
        if (isMotionCompleted(state)) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findQuoteBounds(state->data[state->row], '`', state->col, true));
            copyInVisual(state);
            setPosition(state, changeInVisual(state));
            state->mode = TYPING;
        }
    } else if (handleMotion(state, c, "ci\"")) {
        if (isMotionCompleted(state)) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findQuoteBounds(state->data[state->row], '"', state->col, false));
            copyInVisual(state);
            setPosition(state, changeInVisual(state));
            state->mode = TYPING;
        }
    } else if (handleMotion(state, c, "ca\"")) {
        if (isMotionCompleted(state)) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findQuoteBounds(state->data[state->row], '"', state->col, true));
            copyInVisual(state);
            setPosition(state, changeInVisual(state));
            state->mode = TYPING;
        }
    } else if (handleMotion(state, c, "ci'")) {
        if (isMotionCompleted(state)) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findQuoteBounds(state->data[state->row], '\'', state->col, false));
            copyInVisual(state);
            setPosition(state, changeInVisual(state));
            state->mode = TYPING;
        }
    } else if (handleMotion(state, c, "ca'")) {
        if (isMotionCompleted(state)) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findQuoteBounds(state->data[state->row], '\'', state->col, true));
            copyInVisual(state);
            setPosition(state, changeInVisual(state));
            state->mode = TYPING;
        }
    } else if (handleMotion(state, c, "caT")) {
        if (isMotionCompleted(state)) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '>', '<', state->col, true));
            copyInVisual(state);
            setPosition(state, changeInVisual(state));
            state->mode = TYPING;
        }
    } else if (handleMotion(state, c, "cat")) {
        if (isMotionCompleted(state)) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '<', '>', state->col, true));
            copyInVisual(state);
            setPosition(state, changeInVisual(state));
            state->mode = TYPING;
        }
    } else if (handleMotion(state, c, "cad")) {
        if (isMotionCompleted(state)) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '[', ']', state->col, true));
            copyInVisual(state);
            setPosition(state, changeInVisual(state));
            state->mode = TYPING;
        }
    } else if (handleMotion(state, c, "caB")) {
        if (isMotionCompleted(state)) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '{', '}', state->col, true));
            copyInVisual(state);
            setPosition(state, changeInVisual(state));
            state->mode = TYPING;
        }
    } else if (handleMotion(state, c, "cab")) {
        if (isMotionCompleted(state)) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '(', ')', state->col, true));
            copyInVisual(state);
            setPosition(state, changeInVisual(state));
            state->mode = TYPING;
        }
    } else if (handleMotion(state, c, "ciT")) {
        if (isMotionCompleted(state)) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '>', '<', state->col, false));
            copyInVisual(state);
            setPosition(state, changeInVisual(state));
            state->mode = TYPING;
        }
    } else if (handleMotion(state, c, "cit")) {
        if (isMotionCompleted(state)) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '<', '>', state->col, false));
            copyInVisual(state);
            setPosition(state, changeInVisual(state));
            state->mode = TYPING;
        }
    } else if (handleMotion(state, c, "cid")) {
        if (isMotionCompleted(state)) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '[', ']', state->col, false));
            copyInVisual(state);
            setPosition(state, changeInVisual(state));
            state->mode = TYPING;
        }
    } else if (handleMotion(state, c, "ciB")) {
        if (isMotionCompleted(state)) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '{', '}', state->col, false));
            copyInVisual(state);
            setPosition(state, changeInVisual(state));
            state->mode = TYPING;
        }
    } else if (handleMotion(state, c, "cib")) {
        if (isMotionCompleted(state)) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '(', ')', state->col, false));
            copyInVisual(state);
            setPosition(state, changeInVisual(state));
            state->mode = TYPING;
        }
    } else if (handleMotion(state, c, "ciw")) {
        if (isMotionCompleted(state)) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, getWordPosition(state->data[state->row], state->col));
            copyInVisual(state);
            setPosition(state, changeInVisual(state));
            state->mode = TYPING;
        }
    } else if (handleMotion(state, c, "cc")) {
        if (isMotionCompleted(state)) {
            initVisual(state, LINE);
            copyInVisual(state);
            changeInVisual(state);
            state->col = getIndexFirstNonSpace(state);
            state->mode = TYPING;
        }
    } else if (handleMotion(state, c, "ck")) {
        if (isMotionCompleted(state)) {
            initVisual(state, LINE);
            up(state);
            copyInVisual(state);
            setPosition(state, changeInVisual(state));
            state->col = getIndexFirstNonSpace(state);
            state->mode = TYPING;
        }
    } else if (handleMotion(state, c, "cj")) {
        if (isMotionCompleted(state)) {
            initVisual(state, LINE);
            down(state);
            copyInVisual(state);
            setPosition(state, changeInVisual(state));
            state->col = getIndexFirstNonSpace(state);
            state->mode = TYPING;
        }
    } else if (handleMotion(state, c, "di`")) {
        if (isMotionCompleted(state)) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findQuoteBounds(state->data[state->row], '`', state->col, false));
            copyInVisual(state);
            setPosition(state, deleteInVisual(state));
        }
    } else if (handleMotion(state, c, "da`")) {
        if (isMotionCompleted(state)) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findQuoteBounds(state->data[state->row], '`', state->col, true));
            copyInVisual(state);
            setPosition(state, deleteInVisual(state));
        }
    } else if (handleMotion(state, c, "di\"")) {
        if (isMotionCompleted(state)) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findQuoteBounds(state->data[state->row], '"', state->col, false));
            copyInVisual(state);
            setPosition(state, deleteInVisual(state));
        }
    } else if (handleMotion(state, c, "da\"")) {
        if (isMotionCompleted(state)) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findQuoteBounds(state->data[state->row], '"', state->col, true));
            copyInVisual(state);
            setPosition(state, deleteInVisual(state));
        }
    } else if (handleMotion(state, c, "di'")) {
        if (isMotionCompleted(state)) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findQuoteBounds(state->data[state->row], '\'', state->col, false));
            copyInVisual(state);
            setPosition(state, deleteInVisual(state));
        }
    } else if (handleMotion(state, c, "da'")) {
        if (isMotionCompleted(state)) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findQuoteBounds(state->data[state->row], '\'', state->col, true));
            copyInVisual(state);
            setPosition(state, deleteInVisual(state));
        }
    } else if (handleMotion(state, c, "daT")) {
        if (isMotionCompleted(state)) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '>', '<', state->col, true));
            copyInVisual(state);
            setPosition(state, deleteInVisual(state));
        }
    } else if (handleMotion(state, c, "dat")) {
        if (isMotionCompleted(state)) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '<', '>', state->col, true));
            copyInVisual(state);
            setPosition(state, deleteInVisual(state));
        }
    } else if (handleMotion(state, c, "dad")) {
        if (isMotionCompleted(state)) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '[', ']', state->col, true));
            copyInVisual(state);
            setPosition(state, deleteInVisual(state));
        }
    } else if (handleMotion(state, c, "daB")) {
        if (isMotionCompleted(state)) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '{', '}', state->col, true));
            copyInVisual(state);
            setPosition(state, deleteInVisual(state));
        }
    } else if (handleMotion(state, c, "dab")) {
        if (isMotionCompleted(state)) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '(', ')', state->col, true));
            copyInVisual(state);
            setPosition(state, deleteInVisual(state));
        }
    } else if (handleMotion(state, c, "diT")) {
        if (isMotionCompleted(state)) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '>', '<', state->col, false));
            copyInVisual(state);
            setPosition(state, deleteInVisual(state));
        }
    } else if (handleMotion(state, c, "dit")) {
        if (isMotionCompleted(state)) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '<', '>', state->col, false));
            copyInVisual(state);
            setPosition(state, deleteInVisual(state));
        }
    } else if (handleMotion(state, c, "did")) {
        if (isMotionCompleted(state)) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '[', ']', state->col, false));
            copyInVisual(state);
            setPosition(state, deleteInVisual(state));
        }
    } else if (handleMotion(state, c, "diB")) {
        if (isMotionCompleted(state)) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '{', '}', state->col, false));
            copyInVisual(state);
            setPosition(state, deleteInVisual(state));
        }
    } else if (handleMotion(state, c, "dib")) {
        if (isMotionCompleted(state)) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '(', ')', state->col, false));
            copyInVisual(state);
            setPosition(state, deleteInVisual(state));
        }
    } else if (handleMotion(state, c, "diw")) {
        if (isMotionCompleted(state)) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, getWordPosition(state->data[state->row], state->col));
            copyInVisual(state);
            setPosition(state, deleteInVisual(state));
        }
    } else if (handleMotion(state, c, "dd")) {
        if (isMotionCompleted(state)) {
            initVisual(state, LINE);
            copyInVisual(state);
            deleteInVisual(state);
            state->col = getIndexFirstNonSpace(state);
        }
    } else if (handleMotion(state, c, "dk")) {
        if (isMotionCompleted(state)) {
            initVisual(state, LINE);
            up(state);
            copyInVisual(state);
            setPosition(state, deleteInVisual(state));
            state->col = getIndexFirstNonSpace(state);
        }
    } else if (handleMotion(state, c, "dj")) {
        if (isMotionCompleted(state)) {
            initVisual(state, LINE);
            down(state);
            copyInVisual(state);
            setPosition(state, deleteInVisual(state));
            state->col = getIndexFirstNonSpace(state);
        }
    } else if (handleMotion(state, c, "yi`")) {
        if (isMotionCompleted(state)) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findQuoteBounds(state->data[state->row], '`', state->col, false));
            setPosition(state, copyInVisual(state));
        }
    } else if (handleMotion(state, c, "ya`")) {
        if (isMotionCompleted(state)) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findQuoteBounds(state->data[state->row], '`', state->col, true));
            setPosition(state, copyInVisual(state));
        }
    } else if (handleMotion(state, c, "yi\"")) {
        if (isMotionCompleted(state)) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findQuoteBounds(state->data[state->row], '"', state->col, false));
            setPosition(state, copyInVisual(state));
        }
    } else if (handleMotion(state, c, "ya\"")) {
        if (isMotionCompleted(state)) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findQuoteBounds(state->data[state->row], '"', state->col, true));
            setPosition(state, copyInVisual(state));
        }
    } else if (handleMotion(state, c, "yi'")) {
        if (isMotionCompleted(state)) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findQuoteBounds(state->data[state->row], '\'', state->col, false));
            setPosition(state, copyInVisual(state));
        }
    } else if (handleMotion(state, c, "ya'")) {
        if (isMotionCompleted(state)) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findQuoteBounds(state->data[state->row], '\'', state->col, true));
            setPosition(state, copyInVisual(state));
        }
    } else if (handleMotion(state, c, "yaT")) {
        if (isMotionCompleted(state)) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '>', '<', state->col, true));
            setPosition(state, copyInVisual(state));
        }
    } else if (handleMotion(state, c, "yat")) {
        if (isMotionCompleted(state)) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '<', '>', state->col, true));
            setPosition(state, copyInVisual(state));
        }
    } else if (handleMotion(state, c, "yad")) {
        if (isMotionCompleted(state)) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '[', ']', state->col, true));
            setPosition(state, copyInVisual(state));
        }
    } else if (handleMotion(state, c, "yaB")) {
        if (isMotionCompleted(state)) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '{', '}', state->col, true));
            setPosition(state, copyInVisual(state));
        }
    } else if (handleMotion(state, c, "yab")) {
        if (isMotionCompleted(state)) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '(', ')', state->col, true));
            setPosition(state, copyInVisual(state));
        }
    } else if (handleMotion(state, c, "yiT")) {
        if (isMotionCompleted(state)) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '>', '<', state->col, false));
            setPosition(state, copyInVisual(state));
        }
    } else if (handleMotion(state, c, "yit")) {
        if (isMotionCompleted(state)) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '<', '>', state->col, false));
            setPosition(state, copyInVisual(state));
        }
    } else if (handleMotion(state, c, "yid")) {
        if (isMotionCompleted(state)) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '[', ']', state->col, false));
            setPosition(state, copyInVisual(state));
        }
    } else if (handleMotion(state, c, "yiB")) {
        if (isMotionCompleted(state)) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '{', '}', state->col, false));
            setPosition(state, copyInVisual(state));
        }
    } else if (handleMotion(state, c, "yib")) {
        if (isMotionCompleted(state)) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '(', ')', state->col, false));
            setPosition(state, copyInVisual(state));
        }
    } else if (handleMotion(state, c, "yiw")) {
        if (isMotionCompleted(state)) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, getWordPosition(state->data[state->row], state->col));
            setPosition(state, copyInVisual(state));
        }
    } else if (handleMotion(state, c, "yy")) {
        if (isMotionCompleted(state)) {
            initVisual(state, LINE);
            copyInVisual(state);
            state->col = getIndexFirstNonSpace(state);
        }
    } else if (handleMotion(state, c, "yk")) {
        if (isMotionCompleted(state)) {
            initVisual(state, LINE);
            up(state);
            setPosition(state, copyInVisual(state));
            state->col = getIndexFirstNonSpace(state);
        }
    } else if (handleMotion(state, c, "yj")) {
        if (isMotionCompleted(state)) {
            initVisual(state, LINE);
            down(state);
            setPosition(state, copyInVisual(state));
            state->col = getIndexFirstNonSpace(state);
        }
    } else if (state->prevKeys == "r") {
        if (state->col < state->data[state->row].length()) {
            state->data[state->row] = state->data[state->row].substr(0, state->col) + c + state->data[state->row].substr(state->col + 1);
        }
        state->prevKeys = "";
    } else if (state->prevKeys != "") {
        state->prevKeys = "";
    } else if (c == ':') {
        state->mode = COMMANDLINE;
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
    } else if (c == ctrl('r')) {
        if (state->historyPosition < ((int) state->history.size()) - 1) {
            state->row = applyDiff(state, state->history[state->historyPosition + 1], true);
            state->historyPosition++;
        }
    } else if (c == 'r') {
        state->prevKeys = "r";
    } else if (c == 'h') {
        left(state);
    } else if (c == 'l') {
        right(state);
    } else if (c == 'k') {
        up(state);
    } else if (c == 'j') {
        down(state);
    } else if (c == '[') {
        state->row = getPrevLineSameIndent(state);
    } else if (c == ']') {
        state->row = getNextLineSameIndent(state);
    } else if (c == ctrl('g')) {
        state->mode = GREP;
        generateGrepOutput(state);
    } else if (c == ctrl('p')) {
        state->mode = FINDFILE;
        generateFindFileOutput(state);
    } else if (c == 'v') {
        state->mode = VISUAL;
        initVisual(state, NORMAL);
    } else if (c == 'V') {
        state->mode = VISUAL;
        initVisual(state, LINE);
    } else if (c == 'b') {
        state->col = b(state);
    } else if (c == 'w') {
        state->col = w(state);
    } else if (c == 'i') {
        state->mode = TYPING;
    } else if (c == 'a') {
        right(state);
        state->mode = TYPING;
    } else if (c == ctrl('u')) {
        upHalfScreen(state);
    } else if (c == ctrl('d')) {
        downHalfScreen(state);
    } else if (c == 'o') {
        insertEmptyLineBelow(state);
        down(state);
        state->mode = TYPING;
    } else if (c == 'O') {
        insertEmptyLine(state);
        state->mode = TYPING;
    } else if (c == 'Y') {
        copyToClipboard(state->data[state->row].substr(state->col));
    } else if (c == 'D') {
        copyToClipboard(state->data[state->row].substr(state->col));
        state->data[state->row] = state->data[state->row].substr(0, state->col);
    } else if (c == 'C') {
        copyToClipboard(state->data[state->row].substr(state->col));
        state->data[state->row] = state->data[state->row].substr(0, state->col);
        state->mode = TYPING;
    } else if (c == 'I') {
        state->col = getIndexFirstNonSpace(state);
        state->mode = TYPING;
    } else if (c == 'A') {
        state->col = state->data[state->row].length();
        state->mode = TYPING;
    } else if (c == 'n') {
        state->col += 1;
        uint temp_col = state->col;
        uint temp_row = state->row;
        bool result = setSearchResult(state);
        if (result == false) {
            state->row = temp_row;
            state->col = temp_col - 1;
        }
    } else if (c == ',' && state->recording == false) {
        // TODO replay with clipboard
        for (uint i = 0; i < state->macroCommand.length(); i++) {
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
    } else if (c == '/') {
        state->searchQuery = std::string("");
        state->mode = SEARCH;
    } else if (c == '^') {
        state->col = getIndexFirstNonSpace(state);
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
    } else if (c == '0') {
        state->col = 0;
    } else if (c == '$') {
        if (state->data[state->row].length() != 0) {
            state->col = state->data[state->row].length() - 1;
        } else {
            state->col = 0;
        }
    } else if (c == 'z') {
        centerScreen(state);
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
            state->resetState(state->harpoonFiles[state->harpoonIndex]);
        } else if (state->harpoonIndex >= state->harpoonFiles.size()) {
            state->harpoonIndex = state->harpoonFiles.size() - 1;
            state->resetState(state->harpoonFiles[state->harpoonIndex]);
        }
    } else if (c == 'X') {
        state->harpoonIndex = 0;
        state->harpoonFiles.clear();
    } else if (c == ctrl('e')) {
        if (state->harpoonIndex + 1 < state->harpoonFiles.size()) {
            state->harpoonIndex += 1;
            state->resetState(state->harpoonFiles[state->harpoonIndex]);
        }
    } else if (c == ctrl('w')) {
        if (state->harpoonIndex > 0) {
            state->harpoonIndex -= 1;
            state->resetState(state->harpoonFiles[state->harpoonIndex]);
        }
    } else if (c == ' ') {
        bool found = false;
        for (auto it = state->harpoonFiles.begin(); it != state->harpoonFiles.end(); ) {
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
    } else if (c == 'G') {
        state->row = state->data.size() - 1;
    } else {
        state->status = std::string(1, c) + " <" + std::to_string((int)c) + ">";
    }
}

