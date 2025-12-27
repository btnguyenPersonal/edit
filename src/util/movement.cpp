#include "movement.h"
#include "sanity.h"
#include "comment.h"
#include "indent.h"
#include "display.h"
#include "string.h"

int32_t findChar(State *state, bool reverse, char c)
{
	if (reverse) {
		return findPrevChar(state, c);
	} else {
		return findNextChar(state, c);
	}
}

int32_t toChar(State *state, bool reverse, char c)
{
	if (reverse) {
		return toPrevChar(state, c);
	} else {
		return toNextChar(state, c);
	}
}

bool isOnLastVisualLine(State *state)
{
	auto lastLineStarts = (state->maxX - getLineNumberOffset(state)) * (state->file->data[state->file->row].length() / (state->maxX - getLineNumberOffset(state)));
	return state->file->col > lastLineStarts;
}

uint32_t w(State *state)
{
	fixColOverMax(state);
	bool space = state->file->data[state->file->row][state->file->col] == ' ';
	for (uint32_t i = state->file->col + 1; i < state->file->data[state->file->row].size(); i += 1) {
		if (state->file->data[state->file->row][i] == ' ') {
			space = true;
		} else if (space && state->file->data[state->file->row][i] != ' ') {
			return i;
		} else if (isAlphanumeric(state->file->data[state->file->row][state->file->col]) != isAlphanumeric(state->file->data[state->file->row][i])) {
			return i;
		}
	}
	return state->file->col;
}

uint32_t b(State *state)
{
	fixColOverMax(state);
	if (state->file->col == 0 || state->file->data[state->file->row].empty()) {
		return 0;
	}
	int32_t i = state->file->col - 1;
	while (i >= 0 && state->file->data[state->file->row][i] == ' ') {
		i--;
	}
	if (i < 0) {
		return 0;
	}
	bool isAlnum = isAlphanumeric(state->file->data[state->file->row][i]);
	for (i -= 1; i >= 0; i--) {
		if (state->file->data[state->file->row][i] == ' ') {
			return i + 1;
		} else if ((isAlphanumeric(state->file->data[state->file->row][i])) != isAlnum) {
			return i + 1;
		}
	}
	return 0;
}

uint32_t findNextChar(State *state, char c)
{
	state->prevSearch.type = 'f';
	state->prevSearch.search = c;
	for (uint32_t i = state->file->col + 1; i < state->file->data[state->file->row].length(); i++) {
		if (state->file->data[state->file->row][i] == c) {
			return i;
		}
	}
	return state->file->col;
}

uint32_t toNextChar(State *state, char c)
{
	state->prevSearch.type = 't';
	state->prevSearch.search = c;
	uint32_t index = state->file->col;
	for (uint32_t i = state->file->col + 1; i < state->file->data[state->file->row].length(); i++) {
		if (state->file->data[state->file->row][i] == c) {
			return index;
		} else {
			index = i;
		}
	}
	return state->file->col;
}

uint32_t findPrevChar(State *state, char c)
{
	state->prevSearch.type = 'F';
	state->prevSearch.search = c;
	for (int32_t i = state->file->col - 1; i >= 0; i--) {
		if (state->file->data[state->file->row][i] == c) {
			return (int32_t)i;
		}
	}
	return state->file->col;
}

uint32_t toPrevChar(State *state, char c)
{
	state->prevSearch.type = 'T';
	state->prevSearch.search = c;
	int32_t index = state->file->col;
	for (int32_t i = state->file->col - 1; i >= 0; i--) {
		if (state->file->data[state->file->row][i] == c) {
			return (uint32_t)index;
		} else {
			index = i;
		}
	}
	return state->file->col;
}

uint32_t getPrevEmptyLine(State *state)
{
	bool hitNonEmpty = false;
	for (int32_t i = (int32_t)state->file->row; i >= 0; i--) {
		if (state->file->data[i] != "") {
			hitNonEmpty = true;
		} else if (hitNonEmpty && state->file->data[i] == "") {
			return i;
		}
	}
	return state->file->row;
}

uint32_t getNextEmptyLine(State *state)
{
	bool hitNonEmpty = false;
	for (uint32_t i = state->file->row; i < state->file->data.size(); i++) {
		if (state->file->data[i] != "") {
			hitNonEmpty = true;
		} else if (hitNonEmpty && state->file->data[i] == "") {
			return i;
		}
	}
	return state->file->row;
}

uint32_t getPrevLineSameIndent(State *state)
{
	uint32_t current = getIndent(state, trimLeadingComment(state, state->file->data[state->file->row]));
	for (int32_t i = (int32_t)state->file->row - 1; i >= 0; i--) {
		if (current == getIndent(state, trimLeadingComment(state, state->file->data[i])) && state->file->data[i] != "") {
			return i;
		}
	}
	return state->file->row;
}

uint32_t getNextLineSameIndent(State *state)
{
	uint32_t current = getIndent(state, trimLeadingComment(state, state->file->data[state->file->row]));
	for (uint32_t i = state->file->row + 1; i < state->file->data.size(); i++) {
		if (current == getIndent(state, trimLeadingComment(state, state->file->data[i])) && state->file->data[i] != "") {
			return i;
		}
	}
	return state->file->row;
}

void down(State *state)
{
	if (state->file->row < state->file->data.size() - 1) {
		state->file->row += 1;
		state->file->col = getNormalizedCol(state, state->file->hardCol);
		state->skipSetHardCol = true;
	}
	if (isOffScreenVertical(state)) {
		while (isOffScreenVertical(state) && state->file->windowPosition.row <= state->file->data.size()) {
			state->file->windowPosition.row += 1;
		}
	}
}

void downVisual(State *state)
{
	if (isOnLastVisualLine(state)) {
		if (state->file->row < state->file->data.size() - 1) {
			state->file->row += 1;
			state->file->col = getNormalizedCol(state, state->file->hardCol);
			state->file->col = state->file->col % (state->maxX - getLineNumberOffset(state));
		}
		if (isOffScreenVertical(state)) {
			state->file->windowPosition.row += 1;
		}
	} else {
		state->file->col += state->maxX - getLineNumberOffset(state);
	}
}

void left(State *state)
{
	fixColOverMax(state);
	if (state->file->col > 0) {
		state->file->col -= 1;
	}
}

void right(State *state)
{
	fixColOverMax(state);
	if (state->file->col < state->file->data[state->file->row].length()) {
		state->file->col += 1;
	}
}

void up(State *state)
{
	if (state->file->row > 0) {
		state->file->row -= 1;
		state->file->col = getNormalizedCol(state, state->file->hardCol);
		state->skipSetHardCol = true;
	}
	if (isOffScreenVertical(state)) {
		state->file->windowPosition.row -= 1;
	}
}

void upVisual(State *state)
{
	auto visualCol = state->file->col;
	if (visualCol > state->file->data[state->file->row].length()) {
		visualCol = state->file->data[state->file->row].length();
	}
	if (visualCol < state->maxX - getLineNumberOffset(state)) {
		if (state->file->row > 0) {
			state->file->row -= 1;
			state->file->col = getNormalizedCol(state, state->file->hardCol);
			state->file->col += (state->maxX - getLineNumberOffset(state)) * (state->file->data[state->file->row].length() / (state->maxX - getLineNumberOffset(state)));
		}
		if (isOffScreenVertical(state)) {
			state->file->windowPosition.row -= 1;
		}
	} else {
		state->file->col -= state->maxX - getLineNumberOffset(state);
	}
}

void upScreen(State *state)
{
	if (state->file->row > 0) {
		state->file->row -= 1;
		state->file->windowPosition.row -= 1;
		state->file->col = getNormalizedCol(state, state->file->hardCol);
		state->skipSetHardCol = true;
	}
}

void downScreen(State *state)
{
	if (state->file->row < state->file->data.size() - 1) {
		state->file->row += 1;
		state->file->windowPosition.row += 1;
		state->file->col = getNormalizedCol(state, state->file->hardCol);
		state->skipSetHardCol = true;
	}
}

void upHalfScreen(State *state)
{
	for (uint32_t i = 0; i < state->maxY / 2; i++) {
		if (state->file->row > 0) {
			state->file->row -= 1;
			state->file->windowPosition.row -= 1;
		}
	}
	state->file->col = getNormalizedCol(state, state->file->hardCol);
	state->skipSetHardCol = true;
}

void downHalfScreen(State *state)
{
	for (uint32_t i = 0; i < state->maxY / 2; i++) {
		if (state->file->row < state->file->data.size() - 1) {
			state->file->row += 1;
			state->file->windowPosition.row += 1;
		}
	}
	state->file->col = getNormalizedCol(state, state->file->hardCol);
	state->skipSetHardCol = true;
}

void centerScreen(State *state)
{
	if (state->file) {
		state->file->windowPosition.row = getCenteredWindowPosition(state);
		uint32_t col = getDisplayCol(state);
		if (!state->options.wordwrap) {
			if (isColTooSmall(state)) {
				state->file->windowPosition.col = col;
			} else if (isColTooBig(state)) {
				state->file->windowPosition.col = col + getLineNumberOffset(state) + 1 - state->maxX;
			}
		}
	}
}
