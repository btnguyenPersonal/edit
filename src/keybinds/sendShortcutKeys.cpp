#include "sendShortcutKeys.h"
#include "../util/clipboard.h"
#include "../util/comment.h"
#include "../util/helper.h"
#include "../util/render.h"
#include "../util/history.h"
#include "../util/indent.h"
#include "../util/insertLoggingCode.h"
#include "../util/modes.h"
#include "../util/query.h"
#include "../util/state.h"
#include "../util/visualType.h"
#include "../util/expect.h"
#include "../util/cleanup.h"
#include "../util/keys.h"
#include "sendKeys.h"
#include "sendVisualKeys.h"
#include <algorithm>
#include <climits>
#include <ncurses.h>
#include <string>
#include <vector>

void sendShortcutKeys(State *state, int32_t c)
{
	if (c == KEY_MOUSE) {
		MEVENT event;
		if (getmouse(&event) == OK) {
			if (event.bstate & BUTTON4_PRESSED) {
				upScreen(state);
#ifdef __APPLE__
			} else if (event.bstate & 0x8000000) {
#else
			} else if (event.bstate & BUTTON5_PRESSED) {
#endif
				downScreen(state);
			}
		}
	} else if (c == 27) { // ESC
		state->prevKeys = "";
		state->motion.clear();
	} else if (state->prevKeys == "t") {
		state->col = toNextChar(state, c);
		state->prevKeys = "";
	} else if (state->prevKeys == "f") {
		state->col = findNextChar(state, c);
		state->prevKeys = "";
	} else if (state->prevKeys == "r") {
		if (state->col < state->data[state->row].length() && ' ' <= c && c <= '~') {
			state->data[state->row] = safeSubstring(state->data[state->row], 0, state->col) + (char)c +
						  safeSubstring(state->data[state->row], state->col + 1);
		}
		setDotCommand(state, { 'r', c });
		state->prevKeys = "";
	} else if ((state->prevKeys[0] == 'y' || state->prevKeys[0] == 'd' || state->prevKeys[0] == 'c') &&
		   state->prevKeys.length() == 2) {
		uint32_t tempRow = state->row;
		uint32_t tempCol = state->col;
		char command0 = state->prevKeys[0];
		char command1 = state->prevKeys[1];
		state->prevKeys = "";
		state->motion.clear();
		recordMotion(state, 'v');
		initVisual(state, NORMAL);
		sendVisualKeys(state, command1, true);
		sendVisualKeys(state, c, true);
		if (state->row != state->visual.row || state->col != state->visual.col) {
			sendVisualKeys(state, command0, false);
		} else {
			state->prevKeys = "";
			state->motion.clear();
			state->row = tempRow;
			state->col = tempCol;
			state->mode = SHORTCUTS;
		}
		return;
	} else if (state->prevKeys == "y" || state->prevKeys == "d" || state->prevKeys == "c") {
		if (c == 'i' || c == 'a' || c == 'f' || c == 't') {
			state->prevKeys += c;
		} else {
			bool specialWOrLOnlyCase = state->prevKeys.length() == 1 && (c == 'w' || c == 'l');
			uint32_t tempRow = state->row;
			uint32_t tempCol = state->col;
			char command = state->prevKeys[0];
			state->prevKeys = "";
			bool success = true;
			initVisual(state, NORMAL);
			if (c != command) {
				success = sendVisualKeys(state, c, true);
				if (state->row != state->visual.row) {
					state->visualType = LINE;
					state->motion.insert(state->motion.begin(), 1, 'V');
				} else {
					state->motion.insert(state->motion.begin(), 1, 'v');
				}
			} else {
				state->visualType = LINE;
				state->motion.insert(state->motion.begin(), 1, 'V');
			}
			if (success) {
				if (specialWOrLOnlyCase) {
					sendVisualKeys(state, 'h', true);
				}
				sendVisualKeys(state, command, false);
			} else {
				state->prevKeys = "";
				state->motion.clear();
				state->row = tempRow;
				state->col = tempCol;
				state->mode = SHORTCUTS;
			}
		}
		return;
	} else if (state->prevKeys == "g" && c == '/') {
		state->mode = SEARCH;
	} else if (state->prevKeys == "g" && c == 'r') {
		initVisual(state, NORMAL);
		setStateFromWordPosition(state, getWordPosition(state->data[state->row], state->col));
		setQuery(&state->grep, getInVisual(state));
		generateGrepOutput(state, true);
		findDefinitionFromGrepOutput(state, getInVisual(state));
		centerScreen(state);
		state->prevKeys = "";
	} else if (state->prevKeys == "g" && c == 'e') {
		unCommentBlock(state);
		state->prevKeys = "";
		setDotCommand(state, { 'g', c });
	} else if (state->prevKeys == "g" && c == 't') {
		trimTrailingWhitespace(state);
		state->prevKeys = "";
	} else if (state->prevKeys == "g" && c == 'y') {
		state->status = state->filename;
		copyToClipboard(state, state->filename);
		state->prevKeys = "";
	} else if (state->prevKeys == "g" && c == 'g') {
		state->row = 0;
		state->col = getNormalizedCol(state, state->hardCol);
		state->skipSetHardCol = true;
		state->prevKeys = "";
	} else if (state->prevKeys == "g" && c == ctrl('u')) {
		for (uint32_t i = 0; i < state->maxY / 2; i++) {
			upVisual(state);
		}
		state->prevKeys = "";
	} else if (state->prevKeys == "g" && c == ctrl('d')) {
		for (uint32_t i = 0; i < state->maxY / 2; i++) {
			downVisual(state);
		}
		state->prevKeys = "";
	} else if (state->prevKeys == "g" && (c == 'k' || c == KEY_UP)) {
		upVisual(state);
		state->prevKeys = "";
	} else if (state->prevKeys == "g" && (c == 'j' || c == KEY_DOWN)) {
		downVisual(state);
		state->prevKeys = "";
	} else if (state->prevKeys == "g" && c == ctrl('r')) {
		std::string path = getRelativeToLastAndRoute(state);
		state->status = path;
		copyToClipboard(state, path);
		state->prevKeys = "";
	} else if (c == ' ') {
		createNewestHarpoon(state);
	} else if (c == '1') {
		state->workspace = 0;
		focusHarpoon(state);
	} else if (c == '2') {
		state->workspace = 1;
		focusHarpoon(state);
	} else if (c == '3') {
		state->workspace = 2;
		focusHarpoon(state);
	} else if (c == '4') {
		state->workspace = 3;
		focusHarpoon(state);
	} else if (c == '5') {
		state->workspace = 4;
		focusHarpoon(state);
	} else if (c == '6') {
		state->workspace = 5;
		focusHarpoon(state);
	} else if (c == '7') {
		state->workspace = 6;
		focusHarpoon(state);
	} else if (c == '8') {
		state->workspace = 7;
		focusHarpoon(state);
	} else if (c == '9') {
		state->workspace = 8;
		focusHarpoon(state);
	} else if (state->prevKeys != "") {
		state->prevKeys = "";
	} else if (c == ':') {
		state->mode = COMMANDLINE;
	} else if (c == '<') {
		deindent(state);
		state->col = getIndexFirstNonSpace(state);
		setDotCommand(state, c);
	} else if (c == '>') {
		indent(state);
		state->col = getIndexFirstNonSpace(state);
		setDotCommand(state, c);
	} else if (c == 'u') {
		if (state->historyPosition >= 0) {
			state->row = applyDiff(state, state->history[state->historyPosition], true);
			state->historyPosition--;
		}
	} else if (c == ctrl('r')) {
		if (state->historyPosition < ((int32_t)state->history.size()) - 1) {
			state->row = applyDiff(state, state->history[state->historyPosition + 1], false);
			state->historyPosition++;
		}
	} else if (c == ctrl('i')) {
		forwardFileStack(state);
	} else if (c == ctrl('o')) {
		backwardFileStack(state);
	} else if (c == 'r' || c == 'g' || c == 'c' || c == 'd' || c == 'y' || c == 'f' || c == 't') {
		state->prevKeys = c;
	} else if (c == 'h' || c == KEY_LEFT) {
		left(state);
	} else if (c == 'l' || c == KEY_RIGHT) {
		right(state);
	} else if (c == 'k' || c == KEY_UP) {
		up(state);
	} else if (c == 'j' || c == KEY_DOWN) {
		down(state);
	} else if (c == '{') {
		state->row = getPrevEmptyLine(state);
	} else if (c == '}') {
		state->row = getNextEmptyLine(state);
	} else if (c == '[') {
		state->row = getPrevLineSameIndent(state);
	} else if (c == ']') {
		state->row = getNextLineSameIndent(state);
	} else if (c == '~') {
		swapCase(state, state->row, state->col);
		right(state);
	} else if (c == '#') {
		initVisual(state, NORMAL);
		setStateFromWordPosition(state, getWordPosition(state->data[state->row], state->col));
		setQuery(&state->grep, getInVisual(state));
		state->mode = GREP;
		state->showAllGrep = false;
		generateGrepOutput(state, true);
	} else if (c == '*') {
		initVisual(state, NORMAL);
		setStateFromWordPosition(state, getWordPosition(state->data[state->row], state->col));
		setQuery(&state->search, getInVisual(state));
		state->searching = true;
		state->col += 1;
		uint32_t temp_col = state->col;
		uint32_t temp_row = state->row;
		bool result = setSearchResult(state);
		if (result == false) {
			state->row = temp_row;
			state->col = temp_col - 1;
		}
		centerScreen(state);
	} else if (c == ctrl('g')) {
		state->showAllGrep = false;
		state->mode = GREP;
		generateGrepOutput(state, false);
	} else if (c == ctrl('p')) {
		state->mode = FINDFILE;
		state->selectAll = true;
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
		fixColOverMax(state);
		copyToClipboard(state, safeSubstring(state->data[state->row], state->col));
	} else if (c == 'D') {
		fixColOverMax(state);
		copyToClipboard(state, safeSubstring(state->data[state->row], state->col));
		state->data[state->row] = state->data[state->row].substr(0, state->col);
	} else if (c == 'C') {
		fixColOverMax(state);
		copyToClipboard(state, safeSubstring(state->data[state->row], state->col));
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
	} else if (c == ctrl('q')) {
		state->logLines = getLogLines(state);
		state->diffLines = getDiffLines(state);
		state->viewingDiff = true;
		state->mode = DIFF;
	} else if (c == 'M') {
		state->mark = { state->filename, state->row };
	} else if (c == 'N') {
		state->searching = true;
		bool result = setSearchResultReverse(state);
		if (result == false) {
			state->searchFail = true;
		}
		centerScreen(state);
	} else if (c == 'n') {
		state->searching = true;
		state->col += 1;
		uint32_t temp_col = state->col;
		uint32_t temp_row = state->row;
		bool result = setSearchResult(state);
		if (result == false) {
			state->searchFail = true;
			state->row = temp_row;
			state->col = temp_col - 1;
		}
		centerScreen(state);
	} else if (c == '.') {
		resetValidCursorState(state);
		for (uint32_t i = 0; i < state->dotCommand.size(); i++) {
			state->dontRecordKey = true;
			sendKeys(state, getUnEscapedChar(state->dotCommand[i]));
			cleanup(state, c);
		}
		state->dontRecordKey = true;
	} else if (c == ',') {
		resetValidCursorState(state);
		for (uint32_t i = 0; i < state->macroCommand.size(); i++) {
			state->dontRecordKey = true;
			sendKeys(state, getUnEscapedChar(state->macroCommand[i]));
			cleanup(state, c);
		}
		centerScreen(state);
		state->dontRecordKey = true;
	} else if (c == 'q') {
		if (!state->recording) {
			state->macroCommand.clear();
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
		setDotCommand(state, c);
	} else if (c == 'J') {
		state->col = state->data[state->row].length();
		if (state->row + 1 < state->data.size()) {
			ltrim(state->data[state->row + 1]);
			state->data[state->row] += state->data[state->row + 1];
			state->data.erase(state->data.begin() + state->row + 1);
		}
		setDotCommand(state, c);
	} else if (c == '/') {
		backspaceAll(&state->search);
		state->mode = SEARCH;
	} else if (c == '^') {
		state->col = getIndexFirstNonSpace(state);
	} else if (c == ctrl('t')) {
		if (!state->dontSave) {
			state->mode = FILEEXPLORER;
			if (!state->fileExplorerOpen) {
				state->fileExplorerIndex = state->fileExplorer->expand(state->filename);
				centerFileExplorer(state);
			}
			state->fileExplorerOpen = true;
		}
	} else if (c == ctrl('s')) {
		getAndAddNumber(state, state->row, state->col, -1);
	} else if (c == ctrl('a')) {
		getAndAddNumber(state, state->row, state->col, 1);
	} else if (c == 's') {
		if (state->col < state->data[state->row].length()) {
			copyToClipboard(state, state->data[state->row].substr(state->col, 1));
			state->data[state->row] = state->data[state->row].substr(0, state->col) +
						  state->data[state->row].substr(state->col + 1);
			state->mode = TYPING;
		}
	} else if (c == 'x') {
		if (state->col < state->data[state->row].length()) {
			copyToClipboard(state, state->data[state->row].substr(state->col, 1));
			state->data[state->row] = state->data[state->row].substr(0, state->col) +
						  state->data[state->row].substr(state->col + 1);
		}
		setDotCommand(state, c);
	} else if (c == ctrl('y')) {
		state->col = 0;
		state->mode = BLAME;
		try {
			state->blame = getGitBlame(state->filename);
		} catch (const std::exception &e) {
			state->status = "git blame error";
		}
	} else if (c == ctrl('f')) {
		state->mode = SEARCH;
		state->replacing = true;
		backspaceAll(&state->replace);
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
		renderScreen(state, true);
	} else if (c == 'P') {
		pasteFromClipboard(state);
		setDotCommand(state, c);
	} else if (c == 'p') {
		pasteFromClipboardAfter(state);
		setDotCommand(state, c);
	} else if (c == ctrl('l')) {
		moveHarpoonRight(state);
	} else if (c == KEY_BACKSPACE || c == ctrl('h')) {
		moveHarpoonLeft(state);
	} else if (c == ctrl('x')) {
		if (eraseHarpoon(state)) {
			jumpToPrevHarpoon(state);
		}
	} else if (c == 'X') {
		clearHarpoon(state);
	} else if (c == 'e') {
		toggleComment(state);
		state->col = getIndexFirstNonSpace(state);
	} else if (c == '=') {
		indentLine(state);
		state->col = getIndexFirstNonSpace(state);
	} else if (c == 'Q') {
		removeAllLoggingCode(state);
		setDotCommand(state, c);
	} else if (c == 'm') {
		initVisual(state, NORMAL);
		setStateFromWordPosition(state, getWordPosition(state->data[state->row], state->col));
		toggleLoggingCode(state, getInVisual(state), true);
		state->mode = SHORTCUTS;
		setDotCommand(state, c);
	} else if (c == ctrl('w')) {
		jumpToPrevHarpoon(state);
	} else if (c == ctrl('e')) {
		jumpToNextHarpoon(state);
	} else if (c == '%') {
		auto pos = matchIt(state);
		state->row = pos.row;
		state->col = pos.col;
	} else if (c == '\\') {
		state->changeFile(state->filename);
	} else if (c == 'G') {
		state->row = state->data.size() - 1;
		state->col = getNormalizedCol(state, state->hardCol);
		state->skipSetHardCol = true;
	} else if (c == ctrl('z')) {
		if (state->jumplist.index > 0) {
			state->jumplist.index--;
			state->row = state->jumplist.list[state->jumplist.index].row;
			state->col = state->jumplist.list[state->jumplist.index].col;
			state->jumplist.touched = true;
		}
	} else if (c == 'Z') {
		if (state->jumplist.index + 1 < state->jumplist.list.size()) {
			state->jumplist.index++;
			state->row = state->jumplist.list[state->jumplist.index].row;
			state->col = state->jumplist.list[state->jumplist.index].col;
			state->jumplist.touched = true;
		}
	}
	if (state->mode != SHORTCUTS) {
		state->motion.clear();
		recordMotion(state, c);
	}
}
