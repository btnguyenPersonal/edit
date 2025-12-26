#include "sendTypingKeys.h"
#include "../util/helper.h"
#include "../util/indent.h"
#include "../util/modes.h"
#include "../util/state.h"
#include "../util/sanity.h"
#include "../util/autocomplete.h"
#include "../util/movement.h"
#include <ncurses.h>
#include <string>
#include <vector>

void insertNewline(State *state)
{
	fixColOverMax(state);
	std::string current = state->file->data[state->file->row];
	state->file->data[state->file->row] = current.substr(0, state->file->col);
	state->file->data.insert(state->file->data.begin() + state->file->row + 1, safeSubstring(current, state->file->col));
	state->file->row += 1;
	state->file->col = 0;
}

void sendTypingKeys(State *state, int32_t c)
{
	fixColOverMax(state);
	recordMotion(state, c);
	if (state->prevKeys == "v") {
		state->prevKeys = "";
		if (c == '\n') {
			insertNewline(state);
		} else {
			std::string current = state->file->data[state->file->row];
			state->file->data[state->file->row] = current.substr(0, state->file->col) + (char)c + current.substr(state->file->col);
			state->file->col += 1;
		}
	} else if (c == 27) { // ESC
		left(state);
		state->mode = NORMAL;
		setDotCommand(state, state->motion);
		state->motion.clear();
		state->prevKeys = "";
		return;
	} else if (c == '\n') {
		insertNewline(state);
	} else if (c == KEY_BACKSPACE || c == 127) {
		if (state->file->col > 0) {
			std::string current = state->file->data[state->file->row];
			state->file->data[state->file->row] = current.substr(0, state->file->col - 1) + current.substr(state->file->col);
			state->file->col -= 1;
		} else if (state->file->row > 0) {
			state->file->col = state->file->data[state->file->row - 1].length();
			state->file->data[state->file->row - 1] += state->file->data[state->file->row];
			state->file->data.erase(state->file->data.begin() + state->file->row);
			state->file->row -= 1;
		}
	} else if (c == ctrl('w')) {
		std::string current = state->file->data[state->file->row];
		uint32_t index = b(state);
		state->file->data[state->file->row] = current.substr(0, index) + current.substr(state->file->col);
		state->file->col = index;
	} else if (' ' <= c && c <= '~') {
		std::string current = state->file->data[state->file->row];
		state->file->data[state->file->row] = current.substr(0, state->file->col) + (char)c + current.substr(state->file->col);
		if (c == ':') {
			indentLineWhenTypingLastChar(state);
		}
		if (c != ' ') {
			indentLineWhenTypingFirstChar(state);
		}
		state->file->col += 1;
	} else if (c == ctrl('t')) {
		std::string current = state->file->data[state->file->row];
		state->file->data[state->file->row] = current.substr(0, state->file->col) + '\t' + current.substr(state->file->col);
		state->file->col += 1;
	} else if (c == ctrl('v')) {
		state->prevKeys = 'v';
	} else if (c == ctrl('i')) { // TAB
		std::string completion = autocomplete(state, getCurrentWord(state));
		if (safeSubstring(state->file->data[state->file->row], state->file->col, completion.length()) != completion) {
			std::string current = state->file->data[state->file->row];
			state->file->data[state->file->row] = current.substr(0, state->file->col) + completion + safeSubstring(current, state->file->col);
			state->file->col += completion.length();
		}
	} else if (c == KEY_LEFT) {
		left(state);
	} else if (c == KEY_RIGHT) {
		right(state);
	} else if (c == KEY_UP) {
		up(state);
	} else if (c == KEY_DOWN) {
		down(state);
	}
}
