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
#include "../util/assert.h"
#include "../util/cleanup.h"
#include "../util/keys.h"
#include "../util/grep.h"
#include "../util/sanity.h"
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
	} else if (state->prevKeys == "T") {
		state->file->col = toPrevChar(state, c);
		state->prevKeys = "";
	} else if (state->prevKeys == "F") {
		state->file->col = findPrevChar(state, c);
		state->prevKeys = "";
	} else if (state->prevKeys == "t") {
		state->file->col = toNextChar(state, c);
		state->prevKeys = "";
	} else if (state->prevKeys == "f") {
		state->file->col = findNextChar(state, c);
		state->prevKeys = "";
	} else if (state->prevKeys == "@") {
		state->prevKeys = "";
		char macro;
		if (c == '@' && state->lastMacro != 'E') {
			macro = state->lastMacro;
		} else {
			macro = std::tolower((char)c);
			state->lastMacro = macro;
		}
		state->lastMacro = macro;
		try {
			resetValidCursorState(state);
			for (uint32_t i = 0; i < state->macroCommand[macro].size(); i++) {
				state->dontRecordKey = true;
				sendKeys(state, getUnEscapedChar(state->macroCommand[macro][i]));
				cleanup(state, c);
			}
			centerScreen(state);
			state->dontRecordKey = true;
		} catch (const std::exception &e) {
		}
	} else if (state->prevKeys == "q") {
		if (!state->recording.on) {
			state->recording.c = std::tolower((char)c);
			state->macroCommand[state->recording.c].clear();
		}
		state->recording.on = !state->recording.on;
		state->dontRecordKey = true;
		state->prevKeys = "";
	} else if (state->prevKeys == "r") {
		if (state->file->col < state->file->data[state->file->row].length() && ' ' <= c && c <= '~') {
			state->file->data[state->file->row] = safeSubstring(state->file->data[state->file->row], 0, state->file->col) + (char)c + safeSubstring(state->file->data[state->file->row], state->file->col + 1);
		}
		setDotCommand(state, { 'r', c });
		state->prevKeys = "";
	} else if ((state->prevKeys[0] == 'y' || state->prevKeys[0] == 'd' || state->prevKeys[0] == 'c') && state->prevKeys.length() == 2) {
		state->dontRecordKey = true;
		uint32_t tempRow = state->file->row;
		uint32_t tempCol = state->file->col;
		char command0 = state->prevKeys[0];
		char command1 = state->prevKeys[1];
		state->dotCommand.clear();
		for (uint32_t i = 0; i < state->prevKeys.size(); i++) {
			state->dotCommand.push_back(getEscapedChar(state->prevKeys[i]));
		}
		state->prevKeys = "";
		state->motion.clear();
		recordMotion(state, 'v');
		initVisual(state, SELECT);
		sendVisualKeys(state, command1, true);
		sendVisualKeys(state, c, true);
		if (state->file->row != state->visual.row || state->file->col != state->visual.col) {
			sendVisualKeys(state, command0, false);
		} else {
			state->prevKeys = "";
			state->motion.clear();
			state->file->row = tempRow;
			state->file->col = tempCol;
			state->mode = NORMAL;
		}
		state->dotCommand.push_back(getEscapedChar(c));
		state->dontRecordKey = false;
		return;
	} else if (state->prevKeys == "y" || state->prevKeys == "d" || state->prevKeys == "c") {
		state->dontRecordKey = true;
		if (c == 'i' || c == 'a' || c == 'f' || c == 't' || c == 'F' || c == 'T') {
			state->prevKeys += c;
		} else {
			state->dotCommand.clear();
			for (uint32_t i = 0; i < state->prevKeys.size(); i++) {
				state->dotCommand.push_back(getEscapedChar(state->prevKeys[i]));
			}

			bool specialWOrLOnlyCase = state->prevKeys.length() == 1 && (c == 'w' || c == 'l');
			uint32_t tempRow = state->file->row;
			uint32_t tempCol = state->file->col;
			char command = state->prevKeys[0];
			state->prevKeys = "";
			bool success = true;
			initVisual(state, SELECT);
			if (c != command) {
				success = sendVisualKeys(state, c, true);
				if (state->file->row != state->visual.row) {
					state->visualType = LINE;
				} else {
					if (state->file->col < state->visual.col && state->visual.col > 0) {
						state->visual.col -= 1;
					}
				}
			} else {
				state->visualType = LINE;
			}
			if (success) {
				if (specialWOrLOnlyCase) {
					sendVisualKeys(state, 'h', true);
				}
				sendVisualKeys(state, command, false);
			} else {
				state->prevKeys = "";
				state->motion.clear();
				state->file->row = tempRow;
				state->file->col = tempCol;
				state->mode = NORMAL;
			}

			state->dotCommand.push_back(getEscapedChar(c));
		}
		state->dontRecordKey = false;
		return;
	} else if (state->prevKeys == "g" && c == '/') {
		state->mode = SEARCH;
	} else if (state->prevKeys == "g" && c == 'r') {
		initVisual(state, SELECT);
		setStateFromWordPosition(state, getWordPosition(state->file->data[state->file->row], state->file->col));
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
		state->status = state->file->filename;
		copyToClipboard(state, state->file->filename, true);
		state->prevKeys = "";
	} else if (state->prevKeys == "g" && c == 'Y') {
		std::string absPath = std::filesystem::canonical(state->file->filename);
		state->status = absPath;
		copyToClipboard(state, absPath, true);
		state->prevKeys = "";
	} else if (state->prevKeys == "g" && c == 'g') {
		state->file->row = 0;
		state->file->col = getNormalizedCol(state, state->file->hardCol);
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
		copyToClipboard(state, path, false);
		state->prevKeys = "";
	} else if (state->prevKeys == "g" && c == 'P') {
		Bounds b = paste(state, getFromClipboard(state, true));
		highlightRenderBounds(state, b);
		setDotCommand(state, c);
		state->prevKeys = "";
	} else if (state->prevKeys == "g" && c == 'p') {
		Bounds b = pasteAfter(state, getFromClipboard(state, true));
		highlightRenderBounds(state, b);
		setDotCommand(state, c);
		state->prevKeys = "";
	} else if (c == ' ') {
		createNewestHarpoon(state);
	} else if (state->prevKeys != "") {
		state->prevKeys = "";
	} else if (c == ':') {
		state->mode = COMMAND;
	} else if (c == '<') {
		deindent(state);
		state->file->col = getIndexFirstNonSpace(state);
		setDotCommand(state, c);
	} else if (c == '>') {
		indent(state);
		state->file->col = getIndexFirstNonSpace(state);
		setDotCommand(state, c);
	} else if (c == 'u') {
		if (state->file->historyPosition >= 0) {
			state->file->row = applyDiff(state, state->file->history[state->file->historyPosition], true);
			state->file->historyPosition--;
		}
	} else if (c == ctrl('r')) {
		if (state->file->historyPosition < ((int32_t)state->file->history.size()) - 1) {
			state->file->row = applyDiff(state, state->file->history[state->file->historyPosition + 1], false);
			state->file->historyPosition++;
		}
	} else if (c == ctrl('i')) {
		forwardFileStack(state);
	} else if (c == ctrl('o')) {
		backwardFileStack(state);
	} else if (c == 'q' && state->recording.on) {
		state->recording.on = false;
	} else if (c == 'q' || c == '@' || c == 'r' || c == 'g' || c == 'c' || c == 'd' || c == 'y' || c == 'f' || c == 't' || c == 'F' || c == 'T') {
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
		state->file->row = getPrevEmptyLine(state);
	} else if (c == '}') {
		state->file->row = getNextEmptyLine(state);
	} else if (c == '[') {
		state->file->row = getPrevLineSameIndent(state);
	} else if (c == ']') {
		state->file->row = getNextLineSameIndent(state);
	} else if (c == '~') {
		swapCase(state, state->file->row, state->file->col);
		right(state);
	} else if (c == '#') {
		initVisual(state, SELECT);
		setStateFromWordPosition(state, getWordPosition(state->file->data[state->file->row], state->file->col));
		setQuery(&state->grep, getInVisual(state));
		state->mode = GREP;
		state->showAllGrep = false;
		generateGrepOutput(state, true);
	} else if (c == '*') {
		initVisual(state, SELECT);
		setStateFromWordPosition(state, getWordPosition(state->file->data[state->file->row], state->file->col));
		setQuery(&state->search, getInVisual(state));
		state->searching = true;
		state->file->col += 1;
		uint32_t temp_col = state->file->col;
		uint32_t temp_row = state->file->row;
		bool result = setSearchResult(state);
		if (result == false) {
			state->file->row = temp_row;
			state->file->col = temp_col - 1;
		}
		centerScreen(state);
	} else if (c == ctrl('g')) {
		state->showAllGrep = false;
		state->mode = GREP;
		generateGrepOutput(state, false);
	} else if (c == ctrl('p')) {
		state->mode = FIND;
		state->selectAll = true;
	} else if (c == ctrl('v')) {
		state->mode = VISUAL;
		initVisual(state, BLOCK);
	} else if (c == 'v') {
		state->mode = VISUAL;
		initVisual(state, SELECT);
	} else if (c == 'V') {
		state->mode = VISUAL;
		initVisual(state, LINE);
	} else if (c == 'b') {
		state->file->col = b(state);
	} else if (c == 'w') {
		state->file->col = w(state);
	} else if (c == 'i') {
		state->mode = INSERT;
	} else if (c == 'a') {
		right(state);
		state->mode = INSERT;
	} else if (c == ctrl('u')) {
		upHalfScreen(state);
	} else if (c == ctrl('d')) {
		downHalfScreen(state);
	} else if (c == 'o') {
		insertEmptyLineBelow(state);
		down(state);
		state->mode = INSERT;
	} else if (c == 'O') {
		insertEmptyLine(state);
		state->mode = INSERT;
	} else if (c == 'Y') {
		fixColOverMax(state);
		copyToClipboard(state, safeSubstring(state->file->data[state->file->row], state->file->col), false);
	} else if (c == 'D') {
		fixColOverMax(state);
		copyToClipboard(state, safeSubstring(state->file->data[state->file->row], state->file->col), false);
		state->file->data[state->file->row] = state->file->data[state->file->row].substr(0, state->file->col);
	} else if (c == 'C') {
		fixColOverMax(state);
		copyToClipboard(state, safeSubstring(state->file->data[state->file->row], state->file->col), false);
		state->file->data[state->file->row] = state->file->data[state->file->row].substr(0, state->file->col);
		state->mode = INSERT;
	} else if (c == 'I') {
		state->file->col = getIndexFirstNonSpace(state);
		state->mode = INSERT;
	} else if (c == 'A') {
		state->file->col = state->file->data[state->file->row].length();
		state->mode = INSERT;
	} else if (c == '\'') {
		if (state->mark.filename != "") {
			state->resetState(state->mark.filename);
			state->file->row = state->mark.mark;
		}
	} else if (c == ctrl('q')) {
		state->logLines = getLogLines(state);
		state->diffLines = getDiffLines(state);
		state->viewingDiff = true;
		state->mode = DIFF;
	} else if (c == 'M') {
		state->mark = { state->file->filename, state->file->row };
	} else if (c == 'N') {
		state->searching = true;
		searchNextResult(state, !state->searchBackwards);
		centerScreen(state);
	} else if (c == 'n') {
		state->searching = true;
		searchNextResult(state, state->searchBackwards);
		centerScreen(state);
	} else if (c == ';') {
		repeatPrevLineSearch(state, false);
	} else if (c == ',') {
		repeatPrevLineSearch(state, true);
	} else if (c == '.') {
		resetValidCursorState(state);
		for (uint32_t i = 0; i < state->dotCommand.size(); i++) {
			state->dontRecordKey = true;
			sendKeys(state, getUnEscapedChar(state->dotCommand[i]));
			cleanup(state, c);
		}
		state->dontRecordKey = true;
	} else if (c == 'K') {
		state->file->col = state->file->data[state->file->row].length();
		if (state->file->row + 1 < state->file->data.size()) {
			ltrim(state->file->data[state->file->row + 1]);
			state->file->data[state->file->row] += " " + state->file->data[state->file->row + 1];
			state->file->data.erase(state->file->data.begin() + state->file->row + 1);
		}
		setDotCommand(state, c);
	} else if (c == 'J') {
		state->file->col = state->file->data[state->file->row].length();
		if (state->file->row + 1 < state->file->data.size()) {
			ltrim(state->file->data[state->file->row + 1]);
			state->file->data[state->file->row] += state->file->data[state->file->row + 1];
			state->file->data.erase(state->file->data.begin() + state->file->row + 1);
		}
		setDotCommand(state, c);
	} else if (c == '?') {
		backspaceAll(&state->search);
		state->mode = SEARCH;
		state->searchBackwards = true;
	} else if (c == '/') {
		backspaceAll(&state->search);
		state->mode = SEARCH;
		state->searchBackwards = false;
	} else if (c == '^') {
		state->file->col = getIndexFirstNonSpace(state);
	} else if (c == ctrl('t')) {
		if (!state->dontSave) {
			state->mode = FILEEXPLORER;
			if (!state->fileExplorerOpen) {
				state->fileExplorer = new FileExplorerNode(std::filesystem::current_path());
				state->fileExplorer->open();
				state->fileExplorerIndex = state->fileExplorer->expand(state->file->filename);
				centerFileExplorer(state);
			}
			state->fileExplorerOpen = true;
		}
	} else if (c == ctrl('s')) {
		getAndAddNumber(state, state->file->row, state->file->col, -1);
	} else if (c == ctrl('a')) {
		getAndAddNumber(state, state->file->row, state->file->col, 1);
	} else if (c == 's') {
		if (state->file->col < state->file->data[state->file->row].length()) {
			copyToClipboard(state, state->file->data[state->file->row].substr(state->file->col, 1), false);
			state->file->data[state->file->row] = state->file->data[state->file->row].substr(0, state->file->col) + state->file->data[state->file->row].substr(state->file->col + 1);
			state->mode = INSERT;
		}
	} else if (c == 'x') {
		if (state->file->col < state->file->data[state->file->row].length()) {
			copyToClipboard(state, state->file->data[state->file->row].substr(state->file->col, 1), false);
			state->file->data[state->file->row] = state->file->data[state->file->row].substr(0, state->file->col) + state->file->data[state->file->row].substr(state->file->col + 1);
		}
		setDotCommand(state, c);
	} else if (c == ctrl('y')) {
		state->file->col = 0;
		state->mode = BLAME;
		try {
			state->blame = getGitBlame(state->file->filename);
		} catch (const std::exception &e) {
			state->status = "git blame error";
		}
	} else if (c == ctrl('f')) {
		state->mode = SEARCH;
		state->replacing = true;
		backspaceAll(&state->replace);
	} else if (c == '0') {
		state->file->col = 0;
	} else if (c == '$') {
		state->file->col = getLastCharIndex(state);
	} else if (c == 'z') {
		fixColOverMax(state);
		state->file->windowPosition.col = 0;
		centerScreen(state);
		renderScreen(state, true);
	} else if (c == 'P') {
		paste(state, getFromClipboard(state, false));
		setDotCommand(state, c);
	} else if (c == 'p') {
		pasteAfter(state, getFromClipboard(state, false));
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
		state->file->col = getIndexFirstNonSpace(state);
	} else if (c == '=') {
		indentLine(state);
		state->file->col = getIndexFirstNonSpace(state);
	} else if (c == 'Q') {
		removeAllLoggingCode(state);
		setDotCommand(state, c);
	} else if (c == 'm') {
		initVisual(state, SELECT);
		setStateFromWordPosition(state, getWordPosition(state->file->data[state->file->row], state->file->col));
		toggleLoggingCode(state, getInVisual(state), true);
		state->mode = NORMAL;
		setDotCommand(state, c);
	} else if (c == ctrl('w')) {
		jumpToPrevHarpoon(state);
	} else if (c == ctrl('e')) {
		jumpToNextHarpoon(state);
	} else if (c == '%') {
		auto pos = matchIt(state);
		state->file->row = pos.row;
		state->file->col = pos.col;
	} else if (c == '\\') {
		state->reloadFile(state->file->filename);
	} else if (c == 'G') {
		state->file->row = state->file->data.size() - 1;
		state->file->col = getNormalizedCol(state, state->file->hardCol);
		state->skipSetHardCol = true;
	} else if (c == ctrl('z')) {
		if (state->file->jumplist.index > 0) {
			state->file->jumplist.index--;
			state->file->row = state->file->jumplist.list[state->file->jumplist.index].row;
			state->file->col = state->file->jumplist.list[state->file->jumplist.index].col;
			state->file->jumplist.touched = true;
		}
	} else if (c == 'Z') {
		if (state->file->jumplist.index + 1 < state->file->jumplist.list.size()) {
			state->file->jumplist.index++;
			state->file->row = state->file->jumplist.list[state->file->jumplist.index].row;
			state->file->col = state->file->jumplist.list[state->file->jumplist.index].col;
			state->file->jumplist.touched = true;
		}
	}
	if (state->mode != NORMAL) {
		state->motion.clear();
		recordMotion(state, c);
	}
}
