#include "visual.h"
#include "render.h"
#include "string.h"

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
