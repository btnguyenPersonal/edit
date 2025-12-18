#include "sendSearchKeys.h"
#include "../util/clipboard.h"
#include "../util/helper.h"
#include "../util/modes.h"
#include "../util/query.h"
#include "../util/state.h"
#include <climits>
#include <ncurses.h>

void sendSearchKeys(State *state, int32_t c)
{
	if (c == 27) { // ESC
		state->searching = false;
		state->replacing = false;
		state->replaceBounds = {0, 0, 0, 0};
		state->mode = NORMAL;
	} else if (c == ctrl('t')) {
		if (state->replacing) {
			add(&state->replace, '\t');
		} else {
			add(&state->search, '\t');
		}
	} else if (' ' <= c && c <= '~') {
		if (state->replacing) {
			add(&state->replace, c);
		} else {
			add(&state->search, c);
		}
	} else if (c == KEY_BACKSPACE || c == 127) {
		if (state->replacing) {
			backspace(&state->replace);
		} else {
			backspace(&state->search);
		}
	} else if (c == ctrl('f')) {
		state->replacing = true;
	} else if (c == KEY_LEFT) {
		if (state->replacing) {
			moveCursorLeft(&state->replace);
		} else {
			moveCursorLeft(&state->search);
		}
	} else if (c == KEY_RIGHT) {
		if (state->replacing) {
			moveCursorRight(&state->replace);
		} else {
			moveCursorRight(&state->search);
		}
	} else if (c == ctrl('e')) {
		if (state->replacing) {
			moveCursorEnd(&state->replace);
		} else {
			moveCursorEnd(&state->search);
		}
	} else if (c == ctrl('a')) {
		if (state->replacing) {
			moveCursorStart(&state->replace);
		} else {
			moveCursorStart(&state->search);
		}
	} else if (c == ctrl('r')) {
		state->replacing = true;
		state->replace.query += state->search.query;
		state->replace.cursor += state->search.query.length();
	} else if (c == ctrl('l')) {
		if (state->replacing) {
			backspaceAll(&state->replace);
		} else {
			backspaceAll(&state->search);
		}
	} else if (c == ctrl('w')) {
		if (state->replacing) {
			backspaceWord(&state->replace);
		} else {
			backspaceWord(&state->search);
		}
	} else if (c == ctrl('v')) {
		if (state->replacing) {
			add(&state->replace, getFromClipboard(state, true));
		} else {
			add(&state->search, getFromClipboard(state, true));
		}
	} else if (c == '\n') {
		if (state->replacing) {
			replaceAll(state, state->search.query, state->replace.query);
		}
		state->replacing = false;
		state->replaceBounds = {0, 0, 0, 0};
		state->mode = NORMAL;
		return;
	}
	bool result = state->searchBackwards ? setSearchResultReverse(state, true) : setSearchResult(state);
	state->searchFail = !result;
}
