#include "visual.h"
#include "render.h"
#include "string.h"
#include "repeat.h"
#include "indent.h"
#include "ctrl.h"
#include "clipboard.h"

bool isOpenParen(char c) {
	return c == '(' || c == '{' || c == '[';
}

bool isCloseParen(char c) {
	return c == ')' || c == '}' || c == ']';
}

char getCorrespondingParen(char c) {
	if (c == '(') {
		return ')';
	} else if (c == '{') {
		return '}';
	} else if (c == '[') {
		return ']';
	} else if (c == ')') {
		return '(';
	} else if (c == '}') {
		return '{';
	} else if (c == ']') {
		return '[';
	}
	return ')';
}

void initVisual(State *state, VisualType visualType) {
	state->visualType = visualType;
	state->visual.row = state->file->row;
	state->visual.col = state->file->col;
}

void highlightRenderBounds(State *state, Bounds b) {
	uint32_t tempR = state->file->row;
	uint32_t tempC = state->file->col;
	state->file->row = b.minR;
	state->file->col = b.minC;
	state->visual.row = b.maxR;
	state->visual.col = b.maxC;
	auto tempMode = state->mode;
	state->mode = VISUAL;
	state->visualType = SELECT;
	renderScreen(state, true);
	state->mode = tempMode;
	state->file->row = tempR;
	state->file->col = tempC;
}

WordPosition getWordPosition(const std::string &str, uint32_t cursor) {
	if (cursor >= str.size()) {
		return {0, 0};
	}
	// Move cursor to the start of the current chunk
	while (cursor > 0 && str[cursor - 1] != ' ' && str[cursor] != ' ' && (isAlphanumeric(str[cursor]) == isAlphanumeric(str[cursor - 1]))) {
		cursor--;
	}
	// If cursor is on a space, move to the next chunk
	if (str[cursor] == ' ') {
		while (cursor < str.size() && str[cursor] == ' ') {
			cursor++;
		}
	}
	// If no non-space chunk is found
	if (cursor >= str.size()) {
		return {0, 0};
	}
	// Find the end of the chunk
	uint32_t start = cursor;
	uint32_t end = start;
	while (end < str.size() && str[end] != ' ' && (isAlphanumeric(str[start]) == isAlphanumeric(str[end]))) {
		end++;
	}

	return {start, end - 1};
}

WordPosition getBigWordPosition(const std::string &str, uint32_t cursor) {
	if (cursor >= str.size()) {
		return {0, 0};
	}
	// Move cursor to the start of the current chunk
	while (cursor > 0 && str[cursor - 1] != ' ') {
		cursor--;
	}
	// If cursor is on a space, move to the next chunk
	if (str[cursor] == ' ') {
		while (cursor < str.size() && str[cursor] == ' ') {
			cursor++;
		}
	}
	// If no non-space chunk is found
	if (cursor >= str.size()) {
		return {0, 0};
	}
	// Find the end of the chunk
	uint32_t start = cursor;
	uint32_t end = start;
	while (end < str.size() && str[end] != ' ') {
		end++;
	}

	return {start, end - 1};
}

WordPosition findQuoteBounds(const std::string &str, char quoteChar, uint32_t cursor, bool includeQuote) {
	int32_t lastQuoteIndex = -1;
	for (uint32_t i = 0; i <= cursor; i++) {
		if (str[i] == quoteChar) {
			lastQuoteIndex = i;
		}
	}
	uint32_t i;
	for (i = cursor + 1; i < str.length(); i++) {
		if (str[i] == quoteChar) {
			if (lastQuoteIndex != -1 && lastQuoteIndex < (int32_t)cursor) {
				break;
			} else {
				if (lastQuoteIndex == -1) {
					lastQuoteIndex = i;
				} else {
					break;
				}
			}
		}
	}
	if (i != str.length()) {
		if (i - lastQuoteIndex == 1 || includeQuote) {
			return {(uint32_t)(lastQuoteIndex < 0 ? 0 : lastQuoteIndex), (uint32_t)i};
		} else {
			return {(uint32_t)lastQuoteIndex + 1, (uint32_t)i - 1};
		}
	} else {
		return {0, 0};
	}
}

WordPosition findParentheses(const std::string &str, char openParen, char closeParen, uint32_t cursor, bool includeParen) {
	int32_t balance = 0;
	int32_t openParenIndex = -1;
	// look back for openParen
	for (int32_t i = cursor; i >= 0; i--) {
		if (str[i] == openParen) {
			if (balance == 0) {
				openParenIndex = i;
				break;
			} else {
				balance--;
			}
		} else if (str[i] == closeParen && i != (int32_t)cursor) {
			balance++;
		}
	}
	balance = 0;
	// if haven't found yet look forward for openParen
	if (openParenIndex == -1) {
		for (int32_t i = cursor; i < (int32_t)str.length(); i++) {
			if (str[i] == openParen) {
				if (balance == 0) {
					openParenIndex = i;
					break;
				} else {
					balance--;
				}
			} else if (str[i] == closeParen) {
				balance++;
			}
		}
	}
	balance = 0;
	// if haven't found return {0,0}
	if (openParenIndex == -1) {
		return {0, 0};
	}
	for (int32_t i = openParenIndex + 1; i < (int32_t)str.length(); i++) {
		if (str[i] == openParen) {
			balance--;
		} else if (str[i] == closeParen) {
			if (balance == 0) {
				if (i - openParenIndex == 1 || includeParen) {
					return {(uint32_t)openParenIndex, (uint32_t)i};
				} else {
					return {(uint32_t)openParenIndex + 1, (uint32_t)i - 1};
				}
			} else {
				balance++;
			}
		}
	}
	return {0, 0};
}

Position matchIt(State *state) {
	if (!state->file) {
		return {0, 0};
	}
	if (state->file->col < state->file->data[state->file->row].length()) {
		char firstParen = state->file->data[state->file->row][state->file->col];
		int32_t stack = 0;
		if (isOpenParen(firstParen)) {
			char secondParen = getCorrespondingParen(firstParen);
			uint32_t col = state->file->col;
			for (uint32_t row = state->file->row; row < state->file->data.size(); row++) {
				while (col < state->file->data[row].size()) {
					if (state->file->data[row][col] == secondParen) {
						if (stack == 1) {
							return {row, col};
						} else {
							stack--;
						}
					} else if (state->file->data[row][col] == firstParen) {
						stack++;
					}
					col++;
				}
				col = 0;
			}
		} else if (isCloseParen(firstParen)) {
			char secondParen = getCorrespondingParen(firstParen);
			int32_t col = (int32_t)state->file->col;
			bool first = true;
			for (int32_t row = (int32_t)state->file->row; row >= 0; row--) {
				if (!first) {
					col = state->file->data[row].length() > 0 ? state->file->data[row].length() - 1 : 0;
				}
				while (col >= 0) {
					if (state->file->data[row][col] == secondParen) {
						if (stack == 1) {
							return {(uint32_t)row, (uint32_t)col};
						} else {
							stack--;
						}
					} else if (state->file->data[row][col] == firstParen) {
						stack++;
					}
					col--;
				}
				first = false;
			}
		}
	}
	return {state->file->row, state->file->col};
}

Bounds getBounds(State *state) {
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

void replaceAllWithChar(State *state, int32_t c) {
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

char changeCase(char c, bool upper, bool swap) {
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

void changeCaseVisual(State *state, bool upper, bool swap) {
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

void sortReverseLines(State *state) {
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

void sortLines(State *state) {
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

void setStateFromWordPosition(State *state, WordPosition pos) {
	if (pos.min != 0 || pos.max != 0) {
		state->visual.col = pos.min;
		state->file->col = pos.max;
		state->visual.row = state->file->row;
	}
}

void surroundParagraph(State *state, bool includeLastLine) {
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

bool isValidMoveableChunk(State *state, Bounds bounds) {
	int32_t start = getNumLeadingIndentCharacters(state, state->file->data[bounds.minR]);
	for (uint32_t i = bounds.minR + 1; i <= bounds.maxR; i++) {
		if (getNumLeadingIndentCharacters(state, state->file->data[i]) < start && state->file->data[i] != "") {
			return false;
		}
	}
	return true;
}

std::string getInVisual(State *state) {
	return getInVisual(state, true);
}

std::string getInVisual(State *state, bool addNewlines) {
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

Position changeInVisual(State *state) {
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

Position copyInVisualSystem(State *state) {
	Bounds bounds = getBounds(state);
	Position pos = Position();
	pos.row = bounds.minR;
	pos.col = bounds.minC;
	copyToClipboard(state, getInVisual(state), true);
	state->pasteAsBlock = state->mode == VISUAL && state->visualType == BLOCK;
	return pos;
}

Position copyInVisual(State *state) {
	Bounds bounds = getBounds(state);
	Position pos = Position();
	pos.row = bounds.minR;
	pos.col = bounds.minC;
	copyToClipboard(state, getInVisual(state), false);
	state->pasteAsBlock = state->mode == VISUAL && state->visualType == BLOCK;
	return pos;
}

Position deleteInVisual(State *state) {
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

void swapChars(std::vector<int32_t> &v, int32_t x, int32_t y) {
	for (uint32_t i = 0; i < v.size(); i++) {
		if (v[i] == x) {
			v[i] = y;
		} else if (v[i] == y) {
			v[i] = x;
		}
	}
}

void logDotCommand(State *state) {
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

