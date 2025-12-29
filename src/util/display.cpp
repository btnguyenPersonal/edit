#include "display.h"
#include "fileops.h"
#include <ncurses.h>

bool isOffScreenVertical(State *state)
{
	if (state->file->row < state->file->windowPosition.row) {
		return true;
	}
	uint32_t windowRow = state->file->windowPosition.row;
	uint32_t rowsBelow = 0;
	while (windowRow < state->file->data.size() && rowsBelow + 1 < state->maxY) {
		if (state->file->row == windowRow && getDisplayRows(state, state->file->row) <= state->maxY) {
			return false;
		}
		rowsBelow += getDisplayRows(state, windowRow);
		windowRow++;
	}
	return true;
}

uint32_t getCenteredWindowPosition(State *state)
{
	uint32_t windowRow = state->file->row;
	uint32_t rowsAbove = getDisplayRows(state, state->file->row, state->file->col);
	while (windowRow > 0 && rowsAbove < state->maxY / 2) {
		if (rowsAbove + getDisplayRows(state, windowRow - 1) >= state->maxY / 2) {
			break;
		}
		windowRow--;
		rowsAbove += getDisplayRows(state, windowRow);
	}
	return windowRow;
}

uint32_t getDisplayLength(State *state, std::string s)
{
	if (state->options.indent_style == "tab") {
		uint32_t output = 0;
		for (uint32_t i = 0; i < s.length(); i++) {
			if (s[i] == '\t') {
				output += state->options.indent_size;
			} else {
				output++;
			}
		}
		return output;
	} else {
		return s.length();
	}
}

uint32_t getDisplayRows(State *state, uint32_t r, uint32_t c)
{
	if (!state->options.wordwrap) {
		return 1;
	}
	auto physicalCol = state->file->data[r].length() < c ? state->file->data[r].length() : c;
	return 1 + physicalCol / (state->maxX - getLineNumberOffset(state));
}

uint32_t getDisplayRows(State *state, uint32_t r)
{
	if (!state->options.wordwrap) {
		return 1;
	}
	return 1 + getDisplayLength(state, state->file->data[r]) / (state->maxX - getLineNumberOffset(state));
}

uint32_t getNormalizedCol(State *state, uint32_t hardCol)
{
	if (state->options.indent_style != "tab") {
		return state->file->col;
	}
	int32_t tmpHardCol = hardCol;
	int32_t output = 0;
	for (uint32_t i = 0; i < state->file->data[state->file->row].length() && (int32_t)i < tmpHardCol; i++) {
		if (i < state->file->data[state->file->row].length() && state->file->data[state->file->row][i] == '\t') {
			tmpHardCol -= (state->options.indent_size - 1);
			if ((int32_t)i < tmpHardCol) {
				output++;
			} else {
				return output;
			}
		} else {
			output++;
		}
	}
	return output;
}

uint32_t isLargeFile(State *state)
{
	return state->file && state->file->data.size() > 50000;
}

bool isColTooSmall(State *state)
{
	uint32_t col = getDisplayCol(state);
	return col < state->file->windowPosition.col;
}

bool isColTooBig(State *state)
{
	uint32_t col = getDisplayCol(state);
	return (int32_t)col - (int32_t)state->file->windowPosition.col > (int32_t)state->maxX - (int32_t)getLineNumberOffset(state) - 1;
}

bool isWindowPositionInvalid(State *state)
{
	if (isOffScreenVertical(state)) {
		return true;
	} else if (!state->options.wordwrap && (isColTooSmall(state) || isColTooBig(state))) {
		return true;
	}
	return false;
}

uint32_t getDisplayCol(State *state)
{
	if (state->options.indent_style == "tab") {
		uint32_t sum = 0;
		for (uint32_t i = 0; i < state->file->col && i < state->file->data[state->file->row].length(); i++) {
			if (state->file->data[state->file->row][i] == '\t') {
				sum += state->options.indent_size;
			} else {
				sum++;
			}
		}
		return sum;
	} else {
		return state->file->col;
	}
}

uint32_t getLastCharIndex(State *state)
{
	if (state->file->data[state->file->row].length() != 0) {
		return state->file->data[state->file->row].length() - 1;
	} else {
		return 0;
	}
}

void calcWindowBounds(State *state)
{
	int32_t y, x;
	getmaxyx(stdscr, y, x);
	if (y == -1) {
		y = 24;
	}
	if (x == -1) {
		x = 80;
	}
	state->maxY = y;
	state->maxX = x;
}

void refocusFileExplorer(State *state, bool changeMode)
{
	auto normalizedFilename = normalizeFilename(state->file->filename);
	if (state->fileExplorerOpen) {
		state->fileExplorerIndex = state->fileExplorer->expand(normalizedFilename);
		centerFileExplorer(state);
	}
	if (changeMode) {
		state->mode = FILEEXPLORER;
	}
}

void centerFileExplorer(State *state)
{
	if ((state->fileExplorerIndex - ((int32_t)state->maxY / 2)) > 0) {
		if (state->fileExplorer->getTotalChildren() > ((int32_t)state->maxY)) {
			state->fileExplorerWindowLine = (state->fileExplorerIndex - ((int32_t)state->maxY / 2));
		}
	} else {
		state->fileExplorerWindowLine = 0;
	}
}

uint32_t getLineNumberOffset(State *state)
{
	uint32_t offset = state->lineNumSize + 1;
	if (state->mode == BLAME) {
		offset += state->blameSize;
	}
	if (state->fileExplorerOpen) {
		offset += state->fileExplorerSize;
	}
	return offset;
}
