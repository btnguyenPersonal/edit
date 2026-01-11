#include "sendMultiCursorKeys.h"
#include "../util/bounds.h"
#include "../util/modes.h"
#include "../util/state.h"
#include "../util/autocomplete.h"
#include "../util/movement.h"
#include "../util/search.h"
#include "../util/history.h"
#include "../util/ctrl.h"
#include "../util/string.h"
#include "../util/repeat.h"
#include "../util/switchMode.h"
#include "sendVisualKeys.h"
#include <ncurses.h>
#include <string>
#include <vector>

void sendMultiCursorKeys(State *state, int32_t c)
{
	recordMotion(state, c);
	Bounds bounds = getBounds(state);
	if (c == 27) { // ESC
		left(state);
		switchMode(state, NORMAL);
		setDotCommand(state, state->motion);
		state->motion.clear();
		return;
	} else if (c == KEY_BACKSPACE || c == 127) {
		if (state->file->col > 0) {
			for (uint32_t i = bounds.minR; i <= bounds.maxR; i++) {
				std::string current = state->file->data[i];
				state->file->data[i] = current.substr(0, state->file->col - 1) + safeSubstring(current, state->file->col);
			}
			state->file->col -= 1;
		}
	} else if (' ' <= c && c <= '~') {
		for (uint32_t i = bounds.minR; i <= bounds.maxR; i++) {
			std::string current = state->file->data[i];
			state->file->data[i] = current.substr(0, state->file->col) + (char)c + safeSubstring(current, state->file->col);
		}
		state->file->col += 1;
	} else if (c == ctrl('t')) {
		for (uint32_t i = bounds.minR; i <= bounds.maxR; i++) {
			std::string current = state->file->data[i];
			state->file->data[i] = current.substr(0, state->file->col) + '\t' + safeSubstring(current, state->file->col);
		}
		state->file->col += 1;
	} else if (c == ctrl('i')) { // TAB
		std::string completion = autocomplete(state, getCurrentWord(state));
		if (safeSubstring(state->file->data[state->file->row], state->file->col, completion.length()) != completion) {
			for (uint32_t i = bounds.minR; i <= bounds.maxR; i++) {
				std::string current = state->file->data[i];
				state->file->data[i] = current.substr(0, state->file->col) + completion + safeSubstring(current, state->file->col);
			}
			state->file->col += completion.length();
		}
	}
}
