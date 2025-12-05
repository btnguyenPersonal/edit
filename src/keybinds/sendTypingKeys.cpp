#include "sendTypingKeys.h"
#include "../util/helper.h"
#include "../util/indent.h"
#include "../util/modes.h"
#include "../util/state.h"
#include "../util/sanity.h"
#include <ncurses.h>
#include <string>
#include <vector>

void insertNewline(State *state)
{
	fixColOverMax(state);
	std::string current = state->data[state->row];
	state->data[state->row] = current.substr(0, state->col);
	state->data.insert(state->data.begin() + state->row + 1, current.substr(state->col));
	state->row += 1;
	state->col = 0;
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
			std::string current = state->data[state->row];
			state->data[state->row] = current.substr(0, state->col) + (char)c + current.substr(state->col);
			state->col += 1;
		}
	} else if (c == 27) { // ESC
		left(state);
		state->mode = SHORTCUTS;
		setDotCommand(state, state->motion);
		state->motion.clear();
		state->prevKeys = "";
		return;
	} else if (c == '\n') {
		insertNewline(state);
	} else if (c == KEY_BACKSPACE || c == 127) {
		if (state->col > 0) {
			std::string current = state->data[state->row];
			state->data[state->row] = current.substr(0, state->col - 1) + current.substr(state->col);
			state->col -= 1;
		} else if (state->row > 0) {
			state->col = state->data[state->row - 1].length();
			state->data[state->row - 1] += state->data[state->row];
			state->data.erase(state->data.begin() + state->row);
			state->row -= 1;
		}
	} else if (c == ctrl('w')) {
		std::string current = state->data[state->row];
		uint32_t index = b(state);
		state->data[state->row] = current.substr(0, index) + current.substr(state->col);
		state->col = index;
	} else if (' ' <= c && c <= '~') {
		std::string current = state->data[state->row];
		state->data[state->row] = current.substr(0, state->col) + (char)c + current.substr(state->col);
		if (c != ' ') {
			indentLineWhenTypingFirstChar(state);
		}
		state->col += 1;
	} else if (c == ctrl('t')) {
		std::string current = state->data[state->row];
		state->data[state->row] = current.substr(0, state->col) + '\t' + current.substr(state->col);
		state->col += 1;
	} else if (c == ctrl('v')) {
		state->prevKeys = 'v';
	} else if (c == ctrl('i')) { // TAB
		std::string completion = autocomplete(state, getCurrentWord(state));
		if (safeSubstring(state->data[state->row], state->col, completion.length()) != completion) {
			std::string current = state->data[state->row];
			state->data[state->row] = current.substr(0, state->col) + completion + safeSubstring(current, state->col);
			state->col += completion.length();
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
