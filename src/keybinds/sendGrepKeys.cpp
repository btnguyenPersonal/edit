#include "sendGrepKeys.h"
#include "../util/clipboard.h"
#include "../util/render.h"
#include "../util/helper.h"
#include "../util/modes.h"
#include "../util/query.h"
#include "../util/state.h"
#include "../util/grep.h"
#include "../util/find.h"
#include <ncurses.h>
#include <string>
#include <vector>

void sendGrepKeys(State *state, int32_t c)
{
	std::string cachedGrepString = state->grep.query;
	if (c == 27) { // ESC
		state->mode = NORMAL;
	} else if (c == KEY_LEFT) {
		moveCursorLeft(&state->grep);
	} else if (c == KEY_RIGHT) {
		moveCursorRight(&state->grep);
	} else if (c == KEY_BACKSPACE || c == 127) {
		backspace(&state->grep);
		state->grep.selection = 0;
	} else if (c == ctrl('g')) {
		backspaceAll(&state->find);
		generateFindOutput(state);
		state->find.selection = 0;
		state->selectAll = false;
		state->mode = FIND;
	} else if (c == ctrl('a')) {
		moveCursorStart(&state->grep);
	} else if (c == ctrl('e')) {
		moveCursorEnd(&state->grep);
	} else if (c == ctrl('t')) {
		state->showAllGrep = !state->showAllGrep;
	} else if (c == ctrl('d')) {
		for (uint32_t i = 0; i < state->maxY; i++) {
			if (state->grep.selection + 1 < state->grepOutput.size()) {
				state->grep.selection += 1;
			}
		}
	} else if (c == ctrl('u')) {
		for (uint32_t i = 0; i < state->maxY; i++) {
			if (state->grep.selection > 0) {
				state->grep.selection -= 1;
			}
		}
	} else if (c == KEY_DOWN || c == ctrl('n')) {
		if (state->grep.selection + 1 < state->grepOutput.size()) {
			state->grep.selection += 1;
		}
	} else if (c == KEY_UP || c == ctrl('p')) {
		if (state->grep.selection > 0) {
			state->grep.selection -= 1;
		}
	} else if (c == ctrl('q')) {
		state->grepPath = "";
	} else if (c == ctrl('r')) {
		if (state->grep.selection < state->grepOutput.size()) {
			std::filesystem::path selectedFile = state->grepOutput[state->grep.selection].path.string();
			std::filesystem::path currentDir = ((std::filesystem::path)state->file->filename).parent_path();
			std::filesystem::path relativePath = std::filesystem::relative(selectedFile, currentDir);
			copyToClipboard(state, relativePath.string(), false);
			state->mode = NORMAL;
		}
	} else if (c == ctrl('y')) {
		if (state->grep.selection < state->grepOutput.size()) {
			std::filesystem::path selectedFile = state->grepOutput[state->grep.selection].path.string();
			copyToClipboard(state, selectedFile, false);
			state->mode = NORMAL;
		}
	} else if (c == ctrl('v')) {
		add(&state->grep, getFromClipboard(state, true));
	} else if (c == ctrl('w')) {
		backspaceWord(&state->grep);
	} else if (c == '\n') {
		changeToGrepFile(state);
		state->showGrep = true;
		centerScreen(state);
	} else if (c == ctrl('l')) {
		backspaceAll(&state->grep);
	} else if (' ' <= c && c <= '~') {
		add(&state->grep, c);
		state->grep.selection = 0;
	}
	if (state->mode == GREP && cachedGrepString != state->grep.query) {
		renderScreen(state);
		generateGrepOutput(state, true);
	}
}
