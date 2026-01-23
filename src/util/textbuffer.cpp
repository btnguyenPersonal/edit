#include "textbuffer.h"
#include "string.h"
#include "assert.h"

// === INTERNAL VALIDATION ===

static bool isValidState(State *state)
{
	return state != nullptr && state->file != nullptr;
}

static void validateState(State *state)
{
	assert(isValidState(state) && "Invalid state or file pointer");
}

static bool isValidRow(State *state, uint32_t row)
{
	return row < state->file->data.size();
}

static bool isValidCol(State *state, uint32_t row, uint32_t col)
{
	return isValidRow(state, row) && col <= state->file->data[row].length();
}

static void validatePosition(State *state, uint32_t row, uint32_t col)
{
	validateState(state);
	assert(isValidRow(state, row) && isValidCol(state, row, col) && "Invalid row or column position");
}

// === CACHE MANAGEMENT ===

void textbuffer_invalidateCache(State *state)
{
	validateState(state);
	// Cache invalidation will be added here as we implement caching
	// For now, this is a placeholder for future cache invalidation
}

void textbuffer_markModified(State *state)
{
	validateState(state);
	textbuffer_invalidateCache(state);
	// History tracking integration point
	if (state->file->historyPosition < (int32_t)state->file->history.size()) {
		state->file->historyPosition++;
	}
}

// === READ-ONLY OPERATIONS ===

char textbuffer_getChar(State *state, uint32_t row, uint32_t col)
{
	validateState(state);
	if (!isValidRow(state, row) || col >= state->file->data[row].length()) {
		return '\0';
	}
	return state->file->data[row][col];
}

const std::string &textbuffer_getLine(State *state, uint32_t row)
{
	validateState(state);
	assert(isValidRow(state, row) && "Invalid row for getLine");
	return state->file->data[row];
}

uint32_t textbuffer_getLineCount(State *state)
{
	validateState(state);
	return static_cast<uint32_t>(state->file->data.size());
}

uint32_t textbuffer_getLineLength(State *state, uint32_t row)
{
	validateState(state);
	if (!isValidRow(state, row)) {
		return 0;
	}
	return static_cast<uint32_t>(state->file->data[row].length());
}

bool textbuffer_isValidPosition(State *state, uint32_t row, uint32_t col)
{
	return isValidState(state) && isValidRow(state, row) && isValidCol(state, row, col);
}

const std::vector<std::string> &textbuffer_getLines(State *state)
{
	validateState(state);
	return state->file->data;
}

// === WRITE OPERATIONS ===

void textbuffer_insertChar(State *state, uint32_t row, uint32_t col, char c)
{
	validatePosition(state, row, col);
	state->file->data[row].insert(col, 1, c);
	textbuffer_markModified(state);
}

void textbuffer_deleteChar(State *state, uint32_t row, uint32_t col)
{
	validateState(state);
	if (!isValidRow(state, row) || col >= state->file->data[row].length()) {
		return;
	}
	state->file->data[row].erase(col, 1);
	textbuffer_markModified(state);
}

void textbuffer_replaceChar(State *state, uint32_t row, uint32_t col, char c)
{
	validateState(state);
	if (!isValidRow(state, row) || col >= state->file->data[row].length()) {
		return;
	}
	state->file->data[row][col] = c;
	textbuffer_markModified(state);
}

char textbuffer_getChar(State *state, uint32_t row, uint32_t col)
{
	validateState(state);
	if (!isValidRow(state, row) || col >= state->file->data[row].length()) {
		return '\0';
	}
	return state->file->data[row][col];
}

// === LINE OPERATIONS ===

void textbuffer_insertLine(State *state, uint32_t row, const std::string &line)
{
	validateState(state);
	if (row > state->file->data.size()) {
		return;
	}
	state->file->data.insert(state->file->data.begin() + row, line);
	textbuffer_markModified(state);
}

void textbuffer_deleteLine(State *state, uint32_t row)
{
	validateState(state);
	if (!isValidRow(state, row)) {
		return;
	}
	state->file->data.erase(state->file->data.begin() + row);
	textbuffer_markModified(state);
}

void textbuffer_replaceLine(State *state, uint32_t row, const std::string &newLine)
{
	validateState(state);
	if (!isValidRow(state, row)) {
		return;
	}
	state->file->data[row] = newLine;
	textbuffer_markModified(state);
}

void textbuffer_splitLine(State *state, uint32_t row, uint32_t col)
{
	validatePosition(state, row, col);
	const std::string &line = state->file->data[row];
	std::string prefix = safeSubstring(line, 0, col);
	std::string suffix = safeSubstring(line, col);
	state->file->data[row] = prefix;
	state->file->data.insert(state->file->data.begin() + row + 1, suffix);
	textbuffer_markModified(state);
}

void textbuffer_joinLines(State *state, uint32_t row1, uint32_t row2)
{
	validateState(state);
	if (!isValidRow(state, row1) || !isValidRow(state, row2) || row1 >= row2) {
		return;
	}
	// Merge all lines from row1 to row2 into row1, then delete the rest
	for (uint32_t i = row1 + 1; i <= row2; i++) {
		state->file->data[row1] += state->file->data[i];
	}
	state->file->data.erase(state->file->data.begin() + row1 + 1, state->file->data.begin() + row2 + 1);
	textbuffer_markModified(state);
}

const std::string &textbuffer_getLine(State *state, uint32_t row)
{
	validateState(state);
	assert(isValidRow(state, row) && "Invalid row for getLine");
	return state->file->data[row];
}

// === RANGE OPERATIONS ===

void textbuffer_insertRange(State *state, uint32_t row, const std::vector<std::string> &lines)
{
	validateState(state);
	if (row > state->file->data.size()) {
		return;
	}
	state->file->data.insert(state->file->data.begin() + row, lines.begin(), lines.end());
	textbuffer_markModified(state);
}

void textbuffer_deleteRange(State *state, uint32_t startRow, uint32_t endRow)
{
	validateState(state);
	if (!isValidRow(state, startRow) || !isValidRow(state, endRow) || startRow > endRow) {
		return;
	}
	state->file->data.erase(state->file->data.begin() + startRow, state->file->data.begin() + endRow + 1);
	textbuffer_markModified(state);
}

void textbuffer_replaceRange(State *state, uint32_t startRow, uint32_t endRow, const std::vector<std::string> &lines)
{
	validateState(state);
	if (!isValidRow(state, startRow) || !isValidRow(state, endRow) || startRow > endRow) {
		return;
	}
	textbuffer_deleteRange(state, startRow, endRow);
	textbuffer_insertRange(state, startRow, lines);
	textbuffer_markModified(state);
}

// === UTILITY OPERATIONS ===

uint32_t textbuffer_getLineCount(State *state)
{
	validateState(state);
	return static_cast<uint32_t>(state->file->data.size());
}

uint32_t textbuffer_getLineLength(State *state, uint32_t row)
{
	validateState(state);
	if (!isValidRow(state, row)) {
		return 0;
	}
	return static_cast<uint32_t>(state->file->data[row].length());
}

bool textbuffer_isValidPosition(State *state, uint32_t row, uint32_t col)
{
	return isValidState(state) && isValidRow(state, row) && isValidCol(state, row, col);
}

// === READ-ONLY ACCESS ===

const std::vector<std::string> &textbuffer_getLines(State *state)
{
	validateState(state);
	return state->file->data;
}