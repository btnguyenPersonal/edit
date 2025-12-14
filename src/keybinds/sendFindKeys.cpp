#include "sendFindKeys.h"
#include "../util/clipboard.h"
#include "../util/helper.h"
#include "../util/modes.h"
#include "../util/render.h"
#include "../util/query.h"
#include "../util/state.h"
#include "../util/grep.h"
#include "../util/find.h"
#include <ncurses.h>
#include <string>
#include <vector>

void sendFindKeys(State *state, int32_t c)
{
	std::string cachedFileString = state->find.query;
	if (c == 27) { // ESC
		state->selectAll = false;
		state->mode = NORMAL;
	} else if (' ' <= c && c <= '~') {
		if (state->selectAll == true) {
			backspaceAll(&state->find);
			state->find.selection = 0;
			state->selectAll = false;
		}
		add(&state->find, c);
		state->find.selection = 0;
	} else if (c == KEY_LEFT) {
		state->selectAll = false;
		moveCursorLeft(&state->find);
	} else if (c == KEY_RIGHT) {
		state->selectAll = false;
		moveCursorRight(&state->find);
	} else if (c == KEY_BACKSPACE || c == 127) {
		if (state->selectAll == true) {
			backspaceAll(&state->find);
			state->find.selection = 0;
			state->selectAll = false;
		} else {
			backspace(&state->find);
			state->find.selection = 0;
		}
	} else if (c == ctrl('a')) {
		state->selectAll = false;
		moveCursorStart(&state->find);
	} else if (c == ctrl('e')) {
		state->selectAll = false;
		moveCursorEnd(&state->find);
	} else if (c == ctrl('g')) {
		backspaceAll(&state->grep);
		generateGrepOutput(state, true);
		state->grep.selection = 0;
		state->selectAll = false;
		state->mode = GREP;
	} else if (c == ctrl('l')) {
		state->selectAll = false;
		backspaceAll(&state->find);
		state->find.selection = 0;
	} else if (c == ctrl('d')) {
		for (uint32_t i = 0; i < state->maxY; i++) {
			if (state->find.selection + 1 < state->findOutput.size()) {
				state->find.selection += 1;
			}
		}
	} else if (c == ctrl('u')) {
		for (uint32_t i = 0; i < state->maxY; i++) {
			if (state->find.selection > 0) {
				state->find.selection -= 1;
			}
		}
	} else if (c == KEY_DOWN || c == ctrl('n')) {
		state->selectAll = false;
		if (state->find.selection + 1 < state->findOutput.size()) {
			state->find.selection += 1;
		}
	} else if (c == KEY_UP || c == ctrl('p')) {
		state->selectAll = false;
		if (state->find.selection > 0) {
			state->find.selection -= 1;
		}
	} else if (c == ctrl('r')) {
		if (state->find.selection < state->findOutput.size()) {
			state->selectAll = false;
			auto selectedFile = state->findOutput[state->find.selection].string();
			std::filesystem::path currentDir = ((std::filesystem::path)state->file->filename).parent_path();
			std::filesystem::path relativePath = std::filesystem::relative(selectedFile, currentDir);
			copyToClipboard(state, relativePath.string(), false);
			state->mode = NORMAL;
		}
	} else if (c == ctrl('y')) {
		if (state->find.selection < state->findOutput.size()) {
			state->selectAll = false;
			auto selectedFile = state->findOutput[state->find.selection].string();
			copyToClipboard(state, selectedFile, false);
			state->mode = NORMAL;
		}
	} else if (c == ctrl('v')) {
		if (state->selectAll == true) {
			backspaceAll(&state->find);
			state->find.selection = 0;
			state->selectAll = false;
		}
		add(&state->find, getFromClipboard(state, true));
	} else if (c == ctrl('w')) {
		state->selectAll = false;
		backspaceWord(&state->find);
	} else if (c == '\n') {
		if (state->find.selection < state->findOutput.size()) {
			state->selectAll = false;
			auto selectedFile = state->findOutput[state->find.selection].string();
			state->resetState(selectedFile);
		}
	}
	if (state->mode == FIND && cachedFileString != state->find.query) {
		renderScreen(state);
		generateFindOutput(state);
	}
}
