#include "sendTypingKeys.h"
#include "../util/indent.h"
#include "../util/modes.h"
#include "../util/state.h"
#include "../util/sanity.h"
#include "../util/autocomplete.h"
#include "../util/movement.h"
#include "../util/search.h"
#include "../util/history.h"
#include "../util/string.h"
#include "../util/ctrl.h"
#include "../util/repeat.h"
#include "../util/switchMode.h"
#include "../util/textbuffer.h"
#include <ncurses.h>
#include <string>
#include <vector>

void insertNewline(State *state)
{
	fixColOverMax(state);
	textbuffer_splitLine(state, state->file->row, state->file->col);
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
			textbuffer_insertChar(state, state->file->row, state->file->col, c);
			state->file->col += 1;
		}
	} else if (c == 27) { // ESC
		left(state);
		switchMode(state, NORMAL);
		setDotCommand(state, state->motion);
		state->motion.clear();
		state->prevKeys = "";
		return;
	} else if (c == '\n') {
		insertNewline(state);
	} else if (c == KEY_BACKSPACE || c == 127) {
		if (state->file->col > 0) {
			textbuffer_deleteChar(state, state->file->row, state->file->col - 1);
			state->file->col -= 1;
		} else if (state->file->row > 0) {
			state->file->col = textbuffer_getLineLength(state, state->file->row - 1);
			textbuffer_joinLines(state, state->file->row - 1, state->file->row);
			state->file->row -= 1;
		}
	} else if (c == ctrl('w')) {
		std::string current = textbuffer_getLine(state, state->file->row);
		uint32_t index = b(state);
		std::string newContent = current.substr(0, index) + current.substr(state->file->col);
		textbuffer_replaceRange(state, state->file->row, state->file->row, { newContent });
		state->file->col = index;
	} else if (' ' <= c && c <= '~') {
		textbuffer_insertChar(state, state->file->row, state->file->col, c);
		if (c == ':') {
			indentLineWhenTypingLastChar(state);
		}
		if (c != ' ') {
			indentLineWhenTypingFirstChar(state);
		}
		state->file->col += 1;
	} else if (c == ctrl('t')) {
		textbuffer_insertChar(state, state->file->row, state->file->col, '\t');
		state->file->col += 1;
	} else if (c == ctrl('v')) {
		state->prevKeys = 'v';
	} else if (c == ctrl('a') || c == KEY_HOME) {
		state->file->col = getIndexFirstNonSpace(textbuffer_getLine(state, state->file->row), getIndentCharacter(state));
	} else if (c == ctrl('e') || c == KEY_END) {
		state->file->col = getIndexLast(textbuffer_getLine(state, state->file->row));
		right(state);
	} else if (c == ctrl('i')) { // TAB
		std::string completion = autocomplete(state, getCurrentWord(state));
		if (safeSubstring(textbuffer_getLine(state, state->file->row), state->file->col, completion.length()) != completion) {
			for (size_t i = 0; i < completion.length(); i++) {
				textbuffer_insertChar(state, state->file->row, state->file->col + i, completion[i]);
			}
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
