#include "sendVisualKeys.h"
#include "../util/bounds.h"
#include "../util/clipboard.h"
#include "../util/comment.h"
#include "../util/ctrl.h"
#include "../util/defines.h"
#include "../util/fileops.h"
#include "../util/grep.h"
#include "../util/history.h"
#include "../util/indent.h"
#include "../util/insertLoggingCode.h"
#include "../util/modes.h"
#include "../util/movement.h"
#include "../util/query.h"
#include "../util/repeat.h"
#include "../util/search.h"
#include "../util/state.h"
#include "../util/string.h"
#include "../util/switchMode.h"
#include "../util/textedit.h"
#include "../util/visual.h"
#include <string>
#include <vector>

bool sendVisualKeys(State *state, char c, bool onlyMotions) {
	recordMotion(state, c);
	if (c == KEY_ESCAPE) {
		switchMode(state, NORMAL);
		logDotCommand(state);
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
	} else if (state->prevKeys == "i" && c == '`') {
		setStateFromWordPosition(state, findQuoteBounds(state->file->data[state->file->row], '`', state->file->col, false));
		state->prevKeys = "";
	} else if (state->prevKeys == "a" && c == '`') {
		setStateFromWordPosition(state, findQuoteBounds(state->file->data[state->file->row], '`', state->file->col, true));
		state->prevKeys = "";
	} else if (state->prevKeys == "i" && c == '"') {
		setStateFromWordPosition(state, findQuoteBounds(state->file->data[state->file->row], '"', state->file->col, false));
		state->prevKeys = "";
	} else if (state->prevKeys == "a" && c == '"') {
		setStateFromWordPosition(state, findQuoteBounds(state->file->data[state->file->row], '"', state->file->col, true));
		state->prevKeys = "";
	} else if (state->prevKeys == "i" && c == '\'') {
		setStateFromWordPosition(state, findQuoteBounds(state->file->data[state->file->row], '\'', state->file->col, false));
		state->prevKeys = "";
	} else if (state->prevKeys == "a" && c == '\'') {
		setStateFromWordPosition(state, findQuoteBounds(state->file->data[state->file->row], '\'', state->file->col, true));
		state->prevKeys = "";
	} else if (state->prevKeys == "a" && c == 't') {
		setStateFromWordPosition(state, findParentheses(state->file->data[state->file->row], '>', '<', state->file->col, true));
		state->prevKeys = "";
	} else if (state->prevKeys == "a" && c == 'T') {
		setStateFromWordPosition(state, findParentheses(state->file->data[state->file->row], '<', '>', state->file->col, true));
		state->prevKeys = "";
	} else if (state->prevKeys == "a" && c == 'd') {
		setStateFromWordPosition(state, findParentheses(state->file->data[state->file->row], '[', ']', state->file->col, true));
		state->prevKeys = "";
	} else if (state->prevKeys == "a" && c == 'B') {
		setStateFromWordPosition(state, findParentheses(state->file->data[state->file->row], '{', '}', state->file->col, true));
		state->prevKeys = "";
	} else if (state->prevKeys == "a" && c == 'b') {
		setStateFromWordPosition(state, findParentheses(state->file->data[state->file->row], '(', ')', state->file->col, true));
		state->prevKeys = "";
	} else if (state->prevKeys == "i" && c == 't') {
		setStateFromWordPosition(state, findParentheses(state->file->data[state->file->row], '>', '<', state->file->col, false));
		state->prevKeys = "";
	} else if (state->prevKeys == "i" && (c == 'T' || c == '>' || c == '<')) {
		setStateFromWordPosition(state, findParentheses(state->file->data[state->file->row], '<', '>', state->file->col, false));
		state->prevKeys = "";
	} else if (state->prevKeys == "i" && c == 'd') {
		setStateFromWordPosition(state, findParentheses(state->file->data[state->file->row], '[', ']', state->file->col, false));
		state->prevKeys = "";
	} else if (state->prevKeys == "i" && c == 'B') {
		setStateFromWordPosition(state, findParentheses(state->file->data[state->file->row], '{', '}', state->file->col, false));
		state->prevKeys = "";
	} else if (state->prevKeys == "i" && c == 'b') {
		setStateFromWordPosition(state, findParentheses(state->file->data[state->file->row], '(', ')', state->file->col, false));
		state->prevKeys = "";
	} else if (state->prevKeys == "i" && c == 'w') {
		setStateFromWordPosition(state, getWordPosition(state->file->data[state->file->row], state->file->col));
		state->prevKeys = "";
	} else if (state->prevKeys == "a" && c == 'w') {
		setStateFromWordPosition(state, getWordPosition(state->file->data[state->file->row], state->file->col));
		moveRightIfEmpty(state);
		state->prevKeys = "";
	} else if (state->prevKeys == "i" && c == 'W') {
		setStateFromWordPosition(state, getBigWordPosition(state->file->data[state->file->row], state->file->col));
		state->prevKeys = "";
	} else if (state->prevKeys == "a" && c == 'W') {
		setStateFromWordPosition(state, getBigWordPosition(state->file->data[state->file->row], state->file->col));
		moveRightIfEmpty(state);
		state->prevKeys = "";
	} else if (state->prevKeys == "i" && c == 'p') {
		state->visualType = LINE;
		surroundParagraph(state, false);
		state->prevKeys = "";
	} else if (state->prevKeys == "a" && c == 'p') {
		state->visualType = LINE;
		surroundParagraph(state, true);
		state->prevKeys = "";
	} else if (!onlyMotions && state->visualType == BLOCK && state->prevKeys == "g" && c == ctrl('s')) {
		Bounds bounds = getBounds(state);
		int32_t iterations = 1;
		for (uint32_t i = bounds.minR; i <= bounds.maxR; i++) {
			getAndAddNumber(state, i, state->file->col, -1 * iterations);
			iterations++;
		}
		state->prevKeys = "";
	} else if (!onlyMotions && state->prevKeys == "" && c == '_') {
		sortLines(state);
		switchMode(state, NORMAL);
		state->prevKeys = "";
	} else if (!onlyMotions && state->prevKeys == "g" && c == '_') {
		sortReverseLines(state);
		switchMode(state, NORMAL);
		state->prevKeys = "";
	} else if (!onlyMotions && state->visualType == BLOCK && state->prevKeys == "g" && c == ctrl('a')) {
		Bounds bounds = getBounds(state);
		int32_t iterations = 1;
		for (uint32_t i = bounds.minR; i <= bounds.maxR; i++) {
			getAndAddNumber(state, i, state->file->col, iterations);
			iterations++;
		}
		state->prevKeys = "";
	} else if (!onlyMotions && state->prevKeys == "g" && c == 'r') {
		if (state->visualType == SELECT || state->visualType == LINE) {
			std::string vis = getInVisual(state, false);
			locateNodeModule(state, vis);
		}
		state->prevKeys = "";
	} else if (!onlyMotions && state->prevKeys == "g" && c == 'f') {
		if (state->visualType == SELECT || state->visualType == LINE) {
			std::vector<std::string> extensions = {"", ".js", ".jsx", ".ts", ".tsx"};
			std::string vis = getInVisual(state, false);
			locateFile(state, vis, extensions);
		}
		state->prevKeys = "";
	} else if (!onlyMotions && state->prevKeys == "g" && c == 'U') {
		logDotCommand(state);
		changeCaseVisual(state, true, false);
		state->prevKeys = "";
		switchMode(state, NORMAL);
	} else if (!onlyMotions && state->prevKeys == "g" && c == 'u') {
		logDotCommand(state);
		changeCaseVisual(state, false, false);
		state->prevKeys = "";
		switchMode(state, NORMAL);
	} else if (!onlyMotions && state->prevKeys == "r") {
		replaceAllWithChar(state, c);
		state->prevKeys = "";
		switchMode(state, NORMAL);
	} else if (state->prevKeys == "g" && c == 'g') {
		state->file->row = 0;
		state->prevKeys = "";
	} else if (state->prevKeys == "g" && (c == 'p' || c == 'P')) {
		logDotCommand(state);
		pasteVisual(state, getFromClipboard(state, true));
		switchMode(state, NORMAL);
		state->prevKeys = "";
	} else if (state->prevKeys == "g" && c == 'y') {
		logDotCommand(state);
		auto pos = copyInVisualSystem(state);
		state->file->row = pos.row;
		state->file->col = pos.col;
		switchMode(state, NORMAL);
		state->prevKeys = "";
	} else if (state->prevKeys != "") {
		state->prevKeys = "";
	} else if (c == 'g' || c == 'i' || c == 'a' || c == 'r') {
		state->prevKeys += c;
	} else if (!onlyMotions && state->visualType == BLOCK && c == ctrl('s')) {
		Bounds bounds = getBounds(state);
		for (uint32_t i = bounds.minR; i <= bounds.maxR; i++) {
			getAndAddNumber(state, i, state->file->col, -1);
		}
	} else if (!onlyMotions && state->visualType == BLOCK && c == ctrl('a')) {
		Bounds bounds = getBounds(state);
		for (uint32_t i = bounds.minR; i <= bounds.maxR; i++) {
			getAndAddNumber(state, i, state->file->col, 1);
		}
	} else if (!onlyMotions && c == '~') {
		changeCaseVisual(state, true, true);
		switchMode(state, NORMAL);
	} else if (!onlyMotions && c == 'm') {
		logDotCommand(state);
		if (state->visualType == SELECT || state->visualType == LINE) {
			toggleLoggingCode(state, getInVisual(state, false));
		}
		switchMode(state, NORMAL);
	} else if (!onlyMotions && c == ':') {
		switchMode(state, COMMAND);
	} else if (c == ctrl('f')) {
		if (state->visualType == LINE) {
			state->replaceBounds = getBounds(state);
		} else if (state->visualType == SELECT) {
			state->search.query = getInVisual(state);
			state->replaceBounds = {state->file->row, state->file->row, 0, (uint32_t)state->file->data[state->file->row].length()};
		}
		if (state->search.query != "") {
			state->replacing = true;
		}
		switchMode(state, SEARCH);
	} else if (c == '^') {
		state->file->col = getIndexFirstNonSpace(state->file->data[state->file->row], getIndentCharacter(state));
	} else if (c == '0') {
		state->file->col = 0;
	} else if (c == '$') {
		if (state->file->data[state->file->row].length() != 0) {
			state->file->col = state->file->data[state->file->row].length() - 1;
		} else {
			state->file->col = 0;
		}
	} else if (c == 'F') {
		state->prevKeys = "F";
	} else if (c == 'T') {
		state->prevKeys = "T";
	} else if (c == 'f') {
		state->prevKeys = "f";
	} else if (c == 't') {
		state->prevKeys = "t";
	} else if (c == 'b') {
		state->file->col = b(state);
	} else if (c == ctrl('u')) {
		upHalfScreen(state);
	} else if (c == ctrl('d')) {
		downHalfScreen(state);
	} else if (c == 'w') {
		state->file->col = w(state);
	} else if (c == 'h') {
		left(state);
	} else if (c == 'I' && state->visualType == BLOCK) {
		state->file->col = std::min(state->file->col, state->visual.col);
		switchMode(state, MULTICURSOR);
	} else if (c == 'A' && state->visualType == BLOCK) {
		state->file->col = std::max(state->file->col, state->visual.col) + 1;
		switchMode(state, MULTICURSOR);
	} else if (c == '[') {
		state->file->row = getPrevLineSameIndent(state);
	} else if (c == ']') {
		state->file->row = getNextLineSameIndent(state);
	} else if (c == '{') {
		state->file->row = getPrevEmptyLine(state);
	} else if (c == '}') {
		state->file->row = getNextEmptyLine(state);
	} else if (!onlyMotions && c == '*') {
		if (state->visualType == SELECT || state->visualType == LINE) {
			logDotCommand(state);
			state->searching = true;
			setQuery(&state->search, getInVisual(state, false));
			switchMode(state, NORMAL);
			setSearchResult(state);
		}
	} else if (!onlyMotions && c == '#') {
		setQuery(&state->grep, getInVisual(state, false));
		switchMode(state, GREP);
		state->showAllGrep = false;
		generateGrepOutput(state, true);
	} else if (c == 'l') {
		right(state);
	} else if (!onlyMotions && c == 'K') {
		Bounds bounds = getBounds(state);
		if (bounds.minR > 0) {
			if (isValidMoveableChunk(state, bounds)) {
				auto temp = state->file->data[bounds.minR - 1];
				state->file->data.erase(state->file->data.begin() + bounds.minR - 1);
				state->file->data.insert(state->file->data.begin() + bounds.maxR, temp);
				state->file->row = bounds.minR - 1;
				state->visual.row = bounds.maxR - 1;
				indentRange(state);
				state->file->col = getIndexFirstNonSpace(state->file->data[state->file->row], getIndentCharacter(state));
			} else {
				state->status = "not a valid moveable chunk";
			}
		}
	} else if (!onlyMotions && c == 'J') {
		Bounds bounds = getBounds(state);
		if (bounds.maxR + 1 < state->file->data.size()) {
			if (isValidMoveableChunk(state, bounds)) {
				auto temp = state->file->data[bounds.maxR + 1];
				state->file->data.erase(state->file->data.begin() + bounds.maxR + 1);
				state->file->data.insert(state->file->data.begin() + bounds.minR, temp);
				state->file->row = bounds.minR + 1;
				state->visual.row = bounds.maxR + 1;
				indentRange(state);
				state->file->col = getIndexFirstNonSpace(state->file->data[state->file->row], getIndentCharacter(state));
			} else {
				state->status = "not a valid moveable chunk";
			}
		}
	} else if (c == 'k') {
		up(state);
	} else if (c == 'j') {
		down(state);
	} else if (!onlyMotions && c == 'V') {
		state->visualType = LINE;
	} else if (!onlyMotions && c == 'v') {
		state->visualType = SELECT;
	} else if (!onlyMotions && c == ctrl('v')) {
		state->visualType = BLOCK;
	} else if (c == 'G') {
		state->file->row = state->file->data.size() - 1;
	} else if (!onlyMotions && c == '=') {
		logDotCommand(state);
		Bounds bounds = getBounds(state);
		for (int32_t i = bounds.minR; i <= (int32_t)bounds.maxR; i++) {
			indentLine(state, i);
		}
		state->file->row = bounds.minR;
		state->visual.row = bounds.maxR;
		state->file->col = getIndexFirstNonSpace(state->file->data[state->file->row], getIndentCharacter(state));
		switchMode(state, NORMAL);
	} else if (c == '%') {
		auto pos = matchIt(state);
		state->file->row = pos.row;
		state->file->col = pos.col;
	} else if (c == 'N') {
		state->searching = true;
		searchNextResult(state, !state->searchBackwards);
		centerScreen(state);
	} else if (c == 'n') {
		state->searching = true;
		searchNextResult(state, state->searchBackwards);
		centerScreen(state);
	} else if (!onlyMotions && c == '<') {
		logDotCommand(state);
		Bounds bounds = getBounds(state);
		state->file->row = bounds.minR;
		for (uint32_t i = bounds.minR; i <= bounds.maxR; i++) {
			deindent(state);
			state->file->row += 1;
		}
		state->file->row = bounds.minR;
		state->visual.row = bounds.maxR;
		state->file->col = getIndexFirstNonSpace(state->file->data[state->file->row], getIndentCharacter(state));
		switchMode(state, NORMAL);
	} else if (!onlyMotions && c == '>') {
		logDotCommand(state);
		Bounds bounds = getBounds(state);
		state->file->row = bounds.minR;
		for (uint32_t i = bounds.minR; i <= bounds.maxR; i++) {
			indent(state);
			state->file->row += 1;
		}
		state->file->row = bounds.minR;
		state->visual.row = bounds.maxR;
		state->file->col = getIndexFirstNonSpace(state->file->data[state->file->row], getIndentCharacter(state));
		switchMode(state, NORMAL);
	} else if (!onlyMotions && (c == 'p' || c == 'P')) {
		logDotCommand(state);
		pasteVisual(state, getFromClipboard(state, false));
		switchMode(state, NORMAL);
	} else if (!onlyMotions && c == 'x') {
		logDotCommand(state);
		auto pos = deleteInVisual(state);
		state->file->row = pos.row;
		state->file->col = pos.col;
		switchMode(state, NORMAL);
	} else if (!onlyMotions && c == 'd') {
		logDotCommand(state);
		copyInVisual(state);
		auto pos = deleteInVisual(state);
		state->file->row = pos.row;
		state->file->col = pos.col;
		switchMode(state, NORMAL);
	} else if (!onlyMotions && c == 'y') {
		logDotCommand(state);
		auto pos = copyInVisual(state);
		state->file->row = pos.row;
		state->file->col = pos.col;
		switchMode(state, NORMAL);
	} else if (!onlyMotions && c == 'e') {
		logDotCommand(state);
		Bounds bounds = getBounds(state);
		toggleCommentLines(state, bounds);
		state->file->row = bounds.minR;
		state->file->col = getIndexFirstNonSpace(state->file->data[state->file->row], getIndentCharacter(state));
		switchMode(state, NORMAL);
	} else if (c == 'o') {
		auto tempRow = state->file->row;
		auto tempCol = state->file->col;
		state->file->row = state->visual.row;
		state->file->col = state->visual.col;
		state->visual.row = tempRow;
		state->visual.col = tempCol;
	} else if (!onlyMotions && (c == 'c' || c == 's')) {
		copyInVisual(state);
		auto pos = changeInVisual(state);
		if (state->visualType == BLOCK) {
			switchMode(state, MULTICURSOR);
		} else {
			switchMode(state, INSERT);
		}
		if (state->file->row != pos.row) {
			auto temp = state->file->row;
			state->file->row = pos.row;
			state->visual.row = temp;
		}
		state->file->col = pos.col;
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
	} else {
		return false;
	}
	return true;
}
