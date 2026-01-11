#include "sendVisualKeys.h"
#include "../util/bounds.h"
#include "../util/clipboard.h"
#include "../util/comment.h"
#include "../util/indent.h"
#include "../util/insertLoggingCode.h"
#include "../util/modes.h"
#include "../util/query.h"
#include "../util/state.h"
#include "../util/grep.h"
#include "../util/movement.h"
#include "../util/visual.h"
#include "../util/search.h"
#include "../util/fileops.h"
#include "../util/history.h"
#include "../util/ctrl.h"
#include "../util/string.h"
#include "../util/textedit.h"
#include "../util/repeat.h"
#include "../util/switchMode.h"
#include <string>
#include <vector>

Bounds getBounds(State *state)
{
	Bounds bounds;
	if (state->file->row < state->visual.row) {
		bounds.minR = state->file->row;
		bounds.minC = state->file->col;
		bounds.maxR = state->visual.row;
		bounds.maxC = state->visual.col;
	} else if (state->file->row > state->visual.row) {
		bounds.minR = state->visual.row;
		bounds.minC = state->visual.col;
		bounds.maxR = state->file->row;
		bounds.maxC = state->file->col;
	} else {
		bounds.minR = state->visual.row;
		bounds.maxR = state->file->row;
		bounds.minC = std::min(state->file->col, state->visual.col);
		bounds.maxC = std::max(state->file->col, state->visual.col);
	}
	return bounds;
}

void replaceAllWithChar(State *state, int32_t c)
{
	Bounds bounds = getBounds(state);
	if (state->visualType == SELECT) {
		uint32_t col = bounds.minC;
		for (uint32_t row = bounds.minR; row < bounds.maxR; row++) {
			while (col < state->file->data[row].size()) {
				state->file->data[row][col] = c;
				col++;
			}
			col = 0;
		}
		while (col <= bounds.maxC && col < state->file->data[bounds.maxR].size()) {
			state->file->data[bounds.maxR][col] = c;
			col++;
		}
	} else if (state->visualType == BLOCK) {
		uint32_t min = std::min(bounds.minC, bounds.maxC);
		uint32_t max = std::max(bounds.minC, bounds.maxC);
		for (uint32_t row = bounds.minR; row <= bounds.maxR; row++) {
			for (uint32_t col = min; col <= max; col++) {
				if (col < state->file->data[row].size()) {
					state->file->data[row][col] = c;
				}
			}
		}
	} else if (state->visualType == LINE) {
		for (uint32_t row = bounds.minR; row <= bounds.maxR; row++) {
			for (uint32_t col = 0; col < state->file->data[row].size(); col++) {
				state->file->data[row][col] = c;
			}
		}
	}
}

char changeCase(char c, bool upper, bool swap)
{
	if (swap) {
		if (std::isupper(c)) {
			return std::tolower(c);
		}
		return std::toupper(c);
	} else if (upper) {
		return std::toupper(c);
	} else {
		return std::tolower(c);
	}
}

void changeCaseVisual(State *state, bool upper, bool swap)
{
	Bounds bounds = getBounds(state);
	if (state->visualType == SELECT) {
		uint32_t col = bounds.minC;
		for (uint32_t row = bounds.minR; row < bounds.maxR; row++) {
			while (col < state->file->data[row].size()) {
				state->file->data[row][col] = changeCase(state->file->data[row][col], upper, swap);
				col++;
			}
			col = 0;
		}
		while (col <= bounds.maxC && col < state->file->data[bounds.maxR].size()) {
			state->file->data[bounds.maxR][col] = changeCase(state->file->data[bounds.maxR][col], upper, swap);
			col++;
		}
	} else if (state->visualType == BLOCK) {
		uint32_t min = std::min(bounds.minC, bounds.maxC);
		uint32_t max = std::max(bounds.minC, bounds.maxC);
		for (uint32_t row = bounds.minR; row <= bounds.maxR; row++) {
			for (uint32_t col = min; col <= max; col++) {
				if (col < state->file->data[row].size()) {
					state->file->data[row][col] = changeCase(state->file->data[row][col], upper, swap);
				}
			}
		}
	} else if (state->visualType == LINE) {
		for (uint32_t row = bounds.minR; row <= bounds.maxR; row++) {
			for (uint32_t col = 0; col < state->file->data[row].size(); col++) {
				state->file->data[row][col] = changeCase(state->file->data[row][col], upper, swap);
			}
		}
	}
}

void sortReverseLines(State *state)
{
	Bounds bounds = getBounds(state);
	std::vector<std::string> lines;
	for (uint32_t i = bounds.minR; i <= bounds.maxR; i++) {
		lines.push_back(state->file->data[i]);
	}
	std::sort(lines.begin(), lines.end(), std::greater<>());
	int32_t index = 0;
	for (uint32_t i = bounds.minR; i <= bounds.maxR; i++) {
		state->file->data[i] = lines[index];
		index++;
	}
}

void sortLines(State *state)
{
	Bounds bounds = getBounds(state);
	std::vector<std::string> lines;
	for (uint32_t i = bounds.minR; i <= bounds.maxR; i++) {
		lines.push_back(state->file->data[i]);
	}
	std::sort(lines.begin(), lines.end());
	int32_t index = 0;
	for (uint32_t i = bounds.minR; i <= bounds.maxR; i++) {
		state->file->data[i] = lines[index];
		index++;
	}
}

void setStateFromWordPosition(State *state, WordPosition pos)
{
	if (pos.min != 0 || pos.max != 0) {
		state->visual.col = pos.min;
		state->file->col = pos.max;
		state->visual.row = state->file->row;
	}
}

void surroundParagraph(State *state, bool includeLastLine)
{
	auto start = state->file->row;
	for (int32_t i = (int32_t)start; i >= 0; i--) {
		if (state->file->data[i] == "") {
			break;
		} else {
			start = i;
		}
	}
	state->visual.row = start;
	auto end = state->file->row;
	for (uint32_t i = state->file->row; i < state->file->data.size(); i++) {
		if (state->file->data[i] == "") {
			if (includeLastLine) {
				end = i;
			}
			break;
		} else {
			end = i;
		}
	}
	state->file->row = end;
}

bool isValidMoveableChunk(State *state, Bounds bounds)
{
	int32_t start = getNumLeadingIndentCharacters(state, state->file->data[bounds.minR]);
	for (uint32_t i = bounds.minR + 1; i <= bounds.maxR; i++) {
		if (getNumLeadingIndentCharacters(state, state->file->data[i]) < start && state->file->data[i] != "") {
			return false;
		}
	}
	return true;
}

std::string getInVisual(State *state)
{
	return getInVisual(state, true);
}

std::string getInVisual(State *state, bool addNewlines)
{
	Bounds bounds = getBounds(state);
	std::string clip = "";
	if (state->visualType == BLOCK) {
		uint32_t min = std::min(bounds.minC, bounds.maxC);
		uint32_t max = std::max(bounds.minC, bounds.maxC);
		for (uint32_t i = bounds.minR; i <= bounds.maxR; i++) {
			clip += safeSubstring(state->file->data[i], min, max + 1 - min);
			if (addNewlines) {
				clip += "\n";
			}
		}
	} else if (state->visualType == LINE) {
		for (size_t i = bounds.minR; i <= bounds.maxR; i++) {
			clip += state->file->data[i];
			if (addNewlines) {
				clip += "\n";
			}
		}
	} else if (state->visualType == SELECT) {
		uint32_t index = bounds.minC;
		for (size_t i = bounds.minR; i < bounds.maxR; i++) {
			while (index < state->file->data[i].size()) {
				clip += state->file->data[i][index];
				index += 1;
			}
			index = 0;
			if (addNewlines) {
				clip += "\n";
			}
		}
		clip += safeSubstring(state->file->data[bounds.maxR], index, bounds.maxC - index + 1);
	}
	return clip;
}

Position changeInVisual(State *state)
{
	Bounds bounds = getBounds(state);
	Position pos = Position();
	pos.row = bounds.minR;
	pos.col = bounds.minC;
	if (state->visualType == LINE) {
		state->file->data.erase(state->file->data.begin() + bounds.minR, state->file->data.begin() + bounds.maxR);
		state->file->data[bounds.minR] = std::string("");
	} else if (state->visualType == BLOCK) {
		deleteInVisual(state);
		uint32_t min = std::min(bounds.minC, bounds.maxC);
		pos.col = min;
	} else if (state->visualType == SELECT) {
		std::string firstPart = "";
		std::string secondPart = "";
		if (bounds.minC <= state->file->data[bounds.minR].length()) {
			firstPart = safeSubstring(state->file->data[bounds.minR], 0, bounds.minC);
		}
		if (bounds.maxC < state->file->data[bounds.maxR].length()) {
			secondPart = safeSubstring(state->file->data[bounds.maxR], bounds.maxC + 1);
		}
		state->file->data[bounds.minR] = firstPart + secondPart;
		state->file->data.erase(state->file->data.begin() + bounds.minR + 1, state->file->data.begin() + bounds.maxR + 1);
	}
	return pos;
}

Position copyInVisualSystem(State *state)
{
	Bounds bounds = getBounds(state);
	Position pos = Position();
	pos.row = bounds.minR;
	pos.col = bounds.minC;
	copyToClipboard(state, getInVisual(state), true);
	state->pasteAsBlock = state->mode == VISUAL && state->visualType == BLOCK;
	return pos;
}

Position copyInVisual(State *state)
{
	Bounds bounds = getBounds(state);
	Position pos = Position();
	pos.row = bounds.minR;
	pos.col = bounds.minC;
	copyToClipboard(state, getInVisual(state), false);
	state->pasteAsBlock = state->mode == VISUAL && state->visualType == BLOCK;
	return pos;
}

Position deleteInVisual(State *state)
{
	Bounds bounds = getBounds(state);
	Position pos = Position();
	pos.row = bounds.minR;
	pos.col = bounds.minC;
	if (state->visualType == LINE) {
		state->file->data.erase(state->file->data.begin() + bounds.minR, state->file->data.begin() + bounds.maxR + 1);
	} else if (state->visualType == BLOCK) {
		uint32_t min = std::min(bounds.minC, bounds.maxC);
		uint32_t max = std::max(bounds.minC, bounds.maxC);
		for (uint32_t i = bounds.minR; i <= bounds.maxR; i++) {
			std::string firstPart = safeSubstring(state->file->data[i], 0, min);
			std::string secondPart = safeSubstring(state->file->data[i], max + 1);
			state->file->data[i] = firstPart + secondPart;
		}
		pos.col = min;
	} else if (state->visualType == SELECT) {
		std::string firstPart = "";
		std::string secondPart = "";
		if (bounds.minC <= state->file->data[bounds.minR].length()) {
			firstPart = safeSubstring(state->file->data[bounds.minR], 0, bounds.minC);
		}
		if (bounds.maxC < state->file->data[bounds.maxR].length()) {
			secondPart = safeSubstring(state->file->data[bounds.maxR], bounds.maxC + 1);
		}
		state->file->data[bounds.minR] = firstPart + secondPart;
		state->file->data.erase(state->file->data.begin() + bounds.minR + 1, state->file->data.begin() + bounds.maxR + 1);
	}
	return pos;
}

void swapChars(std::vector<int32_t> &v, int32_t x, int32_t y)
{
	for (uint32_t i = 0; i < v.size(); i++) {
		if (v[i] == x) {
			v[i] = y;
		} else if (v[i] == y) {
			v[i] = x;
		}
	}
}

void logDotCommand(State *state)
{
	if (state->file->row < state->visual.row) {
		swapChars(state->motion, 'j', 'k');
		swapChars(state->motion, '[', ']');
		swapChars(state->motion, '{', '}');
		swapChars(state->motion, ctrl('u'), ctrl('d'));
	}
	setDotCommand(state, state->motion);
	state->prevKeys = "";
	state->motion.clear();
}

bool sendVisualKeys(State *state, char c, bool onlyMotions)
{
	recordMotion(state, c);
	if (c == 27) { // ESC
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
	} else if (state->prevKeys == "i" && c == 'W') {
		setStateFromWordPosition(state, getBigWordPosition(state->file->data[state->file->row], state->file->col));
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
			std::vector<std::string> extensions = { "", ".js", ".jsx", ".ts", ".tsx" };
			std::string vis = getInVisual(state, false);
			locateFile(state, vis, extensions);
		}
		state->prevKeys = "";
	} else if (!onlyMotions && state->prevKeys == "g" && c == 'U') {
		logDotCommand(state);
		changeCaseVisual(state, true, false);
		state->prevKeys = "";
		switchMode(state, NORMAL);
	} else if (!onlyMotions && state->prevKeys == "r") {
		replaceAllWithChar(state, c);
		state->prevKeys = "";
		switchMode(state, NORMAL);
	} else if (!onlyMotions && state->prevKeys == "g" && c == 'u') {
		logDotCommand(state);
		changeCaseVisual(state, false, false);
		state->prevKeys = "";
		switchMode(state, NORMAL);
	} else if (state->prevKeys == "g" && c == 'g') {
		state->file->row = 0;
		state->prevKeys = "";
	} else if (state->prevKeys == "g" && (c == 'p' || c == 'P')) {
		logDotCommand(state);
		Bounds b = pasteVisual(state, getFromClipboard(state, true));
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
			state->replaceBounds = { state->file->row, state->file->row, 0, (uint32_t)state->file->data[state->file->row].length() };
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
