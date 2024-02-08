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
#include "../util/indent.h"
#include "../util/comment.h"
#include "../util/insertLoggingCode.h"
#include "sendVisualKeys.h"
#include "sendKeys.h"
#include "sendShortcutKeys.h"

void sendShortcutKeys(State* state, char c) {
    if (c == 27) { // ESC
        state->prevKeys = "";
    } else if (handleMotion(state, c, "ge")) {
        if (state->motionComplete) {
            unCommentBlock(state);
        }
    } else if (handleMotion(state, c, "gt")) {
        if (state->motionComplete) {
            for (unsigned int i = 0; i < state->data.size(); i++) {
                rtrim(state->data[i]);
            }
        }
    } else if (handleMotion(state, c, "gg")) {
        if (state->motionComplete) {
            state->row = 0;
        }
    } else if (handleMotion(state, c, "ci`")) {
        if (state->motionComplete) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findQuoteBounds(state->data[state->row], '`', state->col, false));
            copyInVisual(state);
            setPosition(state, changeInVisual(state));
            state->mode = TYPING;
        }
    } else if (handleMotion(state, c, "ca`")) {
        if (state->motionComplete) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findQuoteBounds(state->data[state->row], '`', state->col, true));
            copyInVisual(state);
            setPosition(state, changeInVisual(state));
            state->mode = TYPING;
        }
    } else if (handleMotion(state, c, "ci\"")) {
        if (state->motionComplete) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findQuoteBounds(state->data[state->row], '"', state->col, false));
            copyInVisual(state);
            setPosition(state, changeInVisual(state));
            state->mode = TYPING;
        }
    } else if (handleMotion(state, c, "ca\"")) {
        if (state->motionComplete) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findQuoteBounds(state->data[state->row], '"', state->col, true));
            copyInVisual(state);
            setPosition(state, changeInVisual(state));
            state->mode = TYPING;
        }
    } else if (handleMotion(state, c, "ci'")) {
        if (state->motionComplete) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findQuoteBounds(state->data[state->row], '\'', state->col, false));
            copyInVisual(state);
            setPosition(state, changeInVisual(state));
            state->mode = TYPING;
        }
    } else if (handleMotion(state, c, "ca'")) {
        if (state->motionComplete) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findQuoteBounds(state->data[state->row], '\'', state->col, true));
            copyInVisual(state);
            setPosition(state, changeInVisual(state));
            state->mode = TYPING;
        }
    } else if (handleMotion(state, c, "caT")) {
        if (state->motionComplete) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '>', '<', state->col, true));
            copyInVisual(state);
            setPosition(state, changeInVisual(state));
            state->mode = TYPING;
        }
    } else if (handleMotion(state, c, "cat")) {
        if (state->motionComplete) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '<', '>', state->col, true));
            copyInVisual(state);
            setPosition(state, changeInVisual(state));
            state->mode = TYPING;
        }
    } else if (handleMotion(state, c, "cad")) {
        if (state->motionComplete) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '[', ']', state->col, true));
            copyInVisual(state);
            setPosition(state, changeInVisual(state));
            state->mode = TYPING;
        }
    } else if (handleMotion(state, c, "caB")) {
        if (state->motionComplete) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '{', '}', state->col, true));
            copyInVisual(state);
            setPosition(state, changeInVisual(state));
            state->mode = TYPING;
        }
    } else if (handleMotion(state, c, "cab")) {
        if (state->motionComplete) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '(', ')', state->col, true));
            copyInVisual(state);
            setPosition(state, changeInVisual(state));
            state->mode = TYPING;
        }
    } else if (handleMotion(state, c, "ciT")) {
        if (state->motionComplete) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '>', '<', state->col, false));
            copyInVisual(state);
            setPosition(state, changeInVisual(state));
            state->mode = TYPING;
        }
    } else if (handleMotion(state, c, "cit")) {
        if (state->motionComplete) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '<', '>', state->col, false));
            copyInVisual(state);
            setPosition(state, changeInVisual(state));
            state->mode = TYPING;
        }
    } else if (handleMotion(state, c, "cid")) {
        if (state->motionComplete) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '[', ']', state->col, false));
            copyInVisual(state);
            setPosition(state, changeInVisual(state));
            state->mode = TYPING;
        }
    } else if (handleMotion(state, c, "ciB")) {
        if (state->motionComplete) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '{', '}', state->col, false));
            copyInVisual(state);
            setPosition(state, changeInVisual(state));
            state->mode = TYPING;
        }
    } else if (handleMotion(state, c, "cib")) {
        if (state->motionComplete) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '(', ')', state->col, false));
            copyInVisual(state);
            setPosition(state, changeInVisual(state));
            state->mode = TYPING;
        }
    } else if (handleMotion(state, c, "ciw")) {
        if (state->motionComplete) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, getWordPosition(state->data[state->row], state->col));
            copyInVisual(state);
            setPosition(state, changeInVisual(state));
            state->mode = TYPING;
        }
    } else if (handleMotion(state, c, "cc")) {
        if (state->motionComplete) {
            initVisual(state, LINE);
            copyInVisual(state);
            changeInVisual(state);
            state->col = getIndexFirstNonSpace(state);
            state->mode = TYPING;
        }
    } else if (handleMotion(state, c, "ck")) {
        if (state->motionComplete) {
            initVisual(state, LINE);
            up(state);
            copyInVisual(state);
            setPosition(state, changeInVisual(state));
            state->col = getIndexFirstNonSpace(state);
            state->mode = TYPING;
        }
    } else if (handleMotion(state, c, "cj")) {
        if (state->motionComplete) {
            initVisual(state, LINE);
            down(state);
            copyInVisual(state);
            setPosition(state, changeInVisual(state));
            state->col = getIndexFirstNonSpace(state);
            state->mode = TYPING;
        }
    } else if (handleMotion(state, c, "di`")) {
        if (state->motionComplete) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findQuoteBounds(state->data[state->row], '`', state->col, false));
            copyInVisual(state);
            setPosition(state, deleteInVisual(state));
        }
    } else if (handleMotion(state, c, "da`")) {
        if (state->motionComplete) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findQuoteBounds(state->data[state->row], '`', state->col, true));
            copyInVisual(state);
            setPosition(state, deleteInVisual(state));
        }
    } else if (handleMotion(state, c, "di\"")) {
        if (state->motionComplete) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findQuoteBounds(state->data[state->row], '"', state->col, false));
            copyInVisual(state);
            setPosition(state, deleteInVisual(state));
        }
    } else if (handleMotion(state, c, "da\"")) {
        if (state->motionComplete) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findQuoteBounds(state->data[state->row], '"', state->col, true));
            copyInVisual(state);
            setPosition(state, deleteInVisual(state));
        }
    } else if (handleMotion(state, c, "di'")) {
        if (state->motionComplete) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findQuoteBounds(state->data[state->row], '\'', state->col, false));
            copyInVisual(state);
            setPosition(state, deleteInVisual(state));
        }
    } else if (handleMotion(state, c, "da'")) {
        if (state->motionComplete) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findQuoteBounds(state->data[state->row], '\'', state->col, true));
            copyInVisual(state);
            setPosition(state, deleteInVisual(state));
        }
    } else if (handleMotion(state, c, "daT")) {
        if (state->motionComplete) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '>', '<', state->col, true));
            copyInVisual(state);
            setPosition(state, deleteInVisual(state));
        }
    } else if (handleMotion(state, c, "dat")) {
        if (state->motionComplete) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '<', '>', state->col, true));
            copyInVisual(state);
            setPosition(state, deleteInVisual(state));
        }
    } else if (handleMotion(state, c, "dad")) {
        if (state->motionComplete) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '[', ']', state->col, true));
            copyInVisual(state);
            setPosition(state, deleteInVisual(state));
        }
    } else if (handleMotion(state, c, "daB")) {
        if (state->motionComplete) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '{', '}', state->col, true));
            copyInVisual(state);
            setPosition(state, deleteInVisual(state));
        }
    } else if (handleMotion(state, c, "dab")) {
        if (state->motionComplete) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '(', ')', state->col, true));
            copyInVisual(state);
            setPosition(state, deleteInVisual(state));
        }
    } else if (handleMotion(state, c, "diT")) {
        if (state->motionComplete) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '>', '<', state->col, false));
            copyInVisual(state);
            setPosition(state, deleteInVisual(state));
        }
    } else if (handleMotion(state, c, "dit")) {
        if (state->motionComplete) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '<', '>', state->col, false));
            copyInVisual(state);
            setPosition(state, deleteInVisual(state));
        }
    } else if (handleMotion(state, c, "did")) {
        if (state->motionComplete) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '[', ']', state->col, false));
            copyInVisual(state);
            setPosition(state, deleteInVisual(state));
        }
    } else if (handleMotion(state, c, "diB")) {
        if (state->motionComplete) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '{', '}', state->col, false));
            copyInVisual(state);
            setPosition(state, deleteInVisual(state));
        }
    } else if (handleMotion(state, c, "dib")) {
        if (state->motionComplete) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '(', ')', state->col, false));
            copyInVisual(state);
            setPosition(state, deleteInVisual(state));
        }
    } else if (handleMotion(state, c, "diw")) {
        if (state->motionComplete) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, getWordPosition(state->data[state->row], state->col));
            copyInVisual(state);
            setPosition(state, deleteInVisual(state));
        }
    } else if (handleMotion(state, c, "dd")) {
        if (state->motionComplete) {
            initVisual(state, LINE);
            copyInVisual(state);
            deleteInVisual(state);
            state->col = getIndexFirstNonSpace(state);
        }
    } else if (handleMotion(state, c, "dk")) {
        if (state->motionComplete) {
            initVisual(state, LINE);
            up(state);
            copyInVisual(state);
            setPosition(state, deleteInVisual(state));
            state->col = getIndexFirstNonSpace(state);
        }
    } else if (handleMotion(state, c, "dj")) {
        if (state->motionComplete) {
            initVisual(state, LINE);
            down(state);
            copyInVisual(state);
            setPosition(state, deleteInVisual(state));
            state->col = getIndexFirstNonSpace(state);
        }
    } else if (handleMotion(state, c, "yi`")) {
        if (state->motionComplete) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findQuoteBounds(state->data[state->row], '`', state->col, false));
            setPosition(state, copyInVisual(state));
        }
    } else if (handleMotion(state, c, "ya`")) {
        if (state->motionComplete) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findQuoteBounds(state->data[state->row], '`', state->col, true));
            setPosition(state, copyInVisual(state));
        }
    } else if (handleMotion(state, c, "yi\"")) {
        if (state->motionComplete) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findQuoteBounds(state->data[state->row], '"', state->col, false));
            setPosition(state, copyInVisual(state));
        }
    } else if (handleMotion(state, c, "ya\"")) {
        if (state->motionComplete) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findQuoteBounds(state->data[state->row], '"', state->col, true));
            setPosition(state, copyInVisual(state));
        }
    } else if (handleMotion(state, c, "yi'")) {
        if (state->motionComplete) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findQuoteBounds(state->data[state->row], '\'', state->col, false));
            setPosition(state, copyInVisual(state));
        }
    } else if (handleMotion(state, c, "ya'")) {
        if (state->motionComplete) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findQuoteBounds(state->data[state->row], '\'', state->col, true));
            setPosition(state, copyInVisual(state));
        }
    } else if (handleMotion(state, c, "yaT")) {
        if (state->motionComplete) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '>', '<', state->col, true));
            setPosition(state, copyInVisual(state));
        }
    } else if (handleMotion(state, c, "yat")) {
        if (state->motionComplete) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '<', '>', state->col, true));
            setPosition(state, copyInVisual(state));
        }
    } else if (handleMotion(state, c, "yad")) {
        if (state->motionComplete) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '[', ']', state->col, true));
            setPosition(state, copyInVisual(state));
        }
    } else if (handleMotion(state, c, "yaB")) {
        if (state->motionComplete) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '{', '}', state->col, true));
            setPosition(state, copyInVisual(state));
        }
    } else if (handleMotion(state, c, "yab")) {
        if (state->motionComplete) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '(', ')', state->col, true));
            setPosition(state, copyInVisual(state));
        }
    } else if (handleMotion(state, c, "yiT")) {
        if (state->motionComplete) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '>', '<', state->col, false));
            setPosition(state, copyInVisual(state));
        }
    } else if (handleMotion(state, c, "yit")) {
        if (state->motionComplete) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '<', '>', state->col, false));
            setPosition(state, copyInVisual(state));
        }
    } else if (handleMotion(state, c, "yid")) {
        if (state->motionComplete) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '[', ']', state->col, false));
            setPosition(state, copyInVisual(state));
        }
    } else if (handleMotion(state, c, "yiB")) {
        if (state->motionComplete) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '{', '}', state->col, false));
            setPosition(state, copyInVisual(state));
        }
    } else if (handleMotion(state, c, "yib")) {
        if (state->motionComplete) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, findParentheses(state->data[state->row], '(', ')', state->col, false));
            setPosition(state, copyInVisual(state));
        }
    } else if (handleMotion(state, c, "yiw")) {
        if (state->motionComplete) {
            initVisual(state, NORMAL);
            setStateFromWordPosition(state, getWordPosition(state->data[state->row], state->col));
            setPosition(state, copyInVisual(state));
        }
    } else if (handleMotion(state, c, "yy")) {
        if (state->motionComplete) {
            initVisual(state, LINE);
            copyInVisual(state);
            state->col = getIndexFirstNonSpace(state);
        }
    } else if (handleMotion(state, c, "yk")) {
        if (state->motionComplete) {
            initVisual(state, LINE);
            up(state);
            setPosition(state, copyInVisual(state));
            state->col = getIndexFirstNonSpace(state);
        }
    } else if (handleMotion(state, c, "yj")) {
        if (state->motionComplete) {
            initVisual(state, LINE);
            down(state);
            setPosition(state, copyInVisual(state));
            state->col = getIndexFirstNonSpace(state);
        }
    } else if (state->prevKeys == "r") {
        if (state->col < state->data[state->row].length()) {
            state->data[state->row] = state->data[state->row].substr(0, state->col) + c + state->data[state->row].substr(state->col + 1);
        }
        state->motion = "";
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
    } else if (c == ctrl('i')) {
        if (state->fileStackIndex + 1 < state->fileStack.size()) {
            state->fileStackIndex += 1;
        }
        if (std::filesystem::exists(state->fileStack[state->fileStackIndex].c_str())) {
            state->changeFile(state->fileStack[state->fileStackIndex]);
            state->showFileStack = true;
        } else {
            state->status = "file not found";
            state->fileStack.erase(state->fileStack.begin() + state->fileStackIndex);
        }
    } else if (c == ctrl('o')) {
        if (state->fileStackIndex > 0) {
            state->fileStackIndex -= 1;
        }
        if (std::filesystem::exists(state->fileStack[state->fileStackIndex].c_str())) {
            state->changeFile(state->fileStack[state->fileStackIndex]);
            state->showFileStack = true;
        } else {
            state->status = "file not found";
            state->fileStack.erase(state->fileStack.begin() + state->fileStackIndex);
        }
    } else if (c == 'r') {
        state->prevKeys = "r";
    } else if (c == 'h') {
        left(state);
    } else if (c == 'l') {
        right(state);
    } else if (c == 'k' || c == ctrl('k')) {
        up(state);
    } else if (c == 'j' || c == ctrl('j')) {
        down(state);
    } else if (c == '{') {
        state->row = getPrevEmptyLine(state);
    } else if (c == '}') {
        state->row = getNextEmptyLine(state);
    } else if (c == '[') {
        state->row = getPrevLineSameIndent(state);
    } else if (c == ']') {
        state->row = getNextLineSameIndent(state);
    } else if (c == ctrl('g')) {
        state->mode = GREP;
        generateGrepOutput(state);
    } else if (c == ctrl('p')) {
        state->mode = FINDFILE;
        state->selectAll = true;
    } else if (c == 'v') {
        state->mode = VISUAL;
        initVisual(state, NORMAL);
        state->motion = "v";
    } else if (c == 'V') {
        state->mode = VISUAL;
        initVisual(state, LINE);
        state->motion = "V";
    } else if (c == 'b') {
        state->col = b(state);
    } else if (c == 'w') {
        state->col = w(state);
    } else if (c == 'i') {
        state->mode = TYPING;
        state->motion = "i";
    } else if (c == 'a') {
        right(state);
        state->mode = TYPING;
        state->motion = "a";
    } else if (c == ctrl('u')) {
        upHalfScreen(state);
    } else if (c == ctrl('d')) {
        downHalfScreen(state);
    } else if (c == 'o') {
        insertEmptyLineBelow(state);
        down(state);
        state->mode = TYPING;
        state->motion = "o";
    } else if (c == 'O') {
        insertEmptyLine(state);
        state->mode = TYPING;
        state->motion = "O";
    } else if (c == 'Y') {
        fixColOverMax(state);
        copyToClipboard(state->data[state->row].substr(state->col));
    } else if (c == 'D') {
        fixColOverMax(state);
        copyToClipboard(state->data[state->row].substr(state->col));
        state->data[state->row] = state->data[state->row].substr(0, state->col);
    } else if (c == 'C') {
        fixColOverMax(state);
        copyToClipboard(state->data[state->row].substr(state->col));
        state->data[state->row] = state->data[state->row].substr(0, state->col);
        state->mode = TYPING;
        state->motion = "C";
    } else if (c == 'I') {
        state->col = getIndexFirstNonSpace(state);
        state->mode = TYPING;
        state->motion = "I";
    } else if (c == 'A') {
        state->col = state->data[state->row].length();
        state->mode = TYPING;
        state->motion = "A";
    } else if (c == 'N') {
        state->searching = true;
        setSearchResultReverse(state);
        centerScreen(state);
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
    } else if (c == '.') {
        for (unsigned int i = 0; i < state->dotCommand.length(); i++) {
            state->dontRecordKey = true;
            sendKeys(state, state->dotCommand[i]);
        }
        state->dontRecordKey = true;
    } else if (c == ',' && state->recording == false) {
        // TODO replay with clipboard
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
    } else if (c == '^') {
        state->col = getIndexFirstNonSpace(state);
    } else if (c == 's') {
        if (state->col < state->data[state->row].length()) {
            state->data[state->row] = state->data[state->row].substr(0, state->col) + state->data[state->row].substr(state->col + 1);
            state->mode = TYPING;
            state->motion = "s";
        }
    } else if (c == 'x') {
        if (state->col < state->data[state->row].length()) {
            copyToClipboard(state->data[state->row].substr(state->col, 1));
            state->data[state->row] = state->data[state->row].substr(0, state->col) + state->data[state->row].substr(state->col + 1);
        }
    } else if (c == ctrl('h')) {
        moveHarpoonLeft(state);
    } else if (c == ctrl('l')) {
        moveHarpoonRight(state);
    } else if (c == ctrl('y')) {
        state->col = 0;
        state->mode = BLAME;
    } else if (c == ctrl('f')) {
        state->mode = SEARCH;
        state->replacing = true;
    } else if (c == '0') {
        state->col = 0;
    } else if (c == '$') {
        if (state->data[state->row].length() != 0) {
            state->col = state->data[state->row].length() - 1;
        } else {
            state->col = 0;
        }
    } else if (c == 'z') {
        fixColOverMax(state);
        state->windowPosition.col = 0;
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
        } else if (state->harpoonIndex >= state->harpoonFiles.size()) {
            state->harpoonIndex = state->harpoonFiles.size() - 1;
        }
        state->resetState(state->harpoonFiles[state->harpoonIndex]);
    } else if (c == 'X') {
        state->harpoonIndex = 0;
        state->harpoonFiles.clear();
    } else if (c == 'e') {
        toggleComment(state);
        state->col = getIndexFirstNonSpace(state);
    } else if (c == '=') {
        indentLine(state);
        state->col = getIndexFirstNonSpace(state);
    } else if (c == 'Q') {
        removeAllLoggingCode(state);
    } else if (c == ctrl('q')) {
        toggleLoggingCode(state);
    } else if (c == ctrl('e')) {
        if (state->harpoonIndex + 1 < state->harpoonFiles.size()) {
            if (std::filesystem::exists(state->harpoonFiles[state->harpoonIndex + 1].c_str())) {
                state->harpoonIndex += 1;
                state->resetState(state->harpoonFiles[state->harpoonIndex]);
            } else {
                state->status = "file not found";
                state->harpoonFiles.erase(state->harpoonFiles.begin() + state->harpoonIndex + 1);
            }
        } else {
            if (std::filesystem::exists(state->harpoonFiles[state->harpoonIndex].c_str())) {
                state->resetState(state->harpoonFiles[state->harpoonIndex]);
            }
        }
    } else if (c == ctrl('w')) {
        if (state->harpoonIndex > 0) {
            if (std::filesystem::exists(state->harpoonFiles[state->harpoonIndex - 1].c_str())) {
                state->harpoonIndex -= 1;
                state->resetState(state->harpoonFiles[state->harpoonIndex]);
            } else {
                state->status = "file not found";
                state->harpoonFiles.erase(state->harpoonFiles.begin() + state->harpoonIndex - 1);
                state->harpoonIndex -= 1;
            }
        } else {
            if (std::filesystem::exists(state->harpoonFiles[state->harpoonIndex].c_str())) {
                state->resetState(state->harpoonFiles[state->harpoonIndex]);
            }
        }
    } else if (c == '\\') {
        state->changeFile(state->filename);
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
    }
}

