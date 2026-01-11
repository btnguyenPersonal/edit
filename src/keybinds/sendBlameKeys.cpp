#include "sendBlameKeys.h"
#include "../util/clipboard.h"
#include "../util/modes.h"
#include "../util/state.h"
#include "../util/movement.h"
#include "../util/external.h"
#include "../util/ctrl.h"
#include "../util/switchMode.h"
#include <ncurses.h>
#include <string>
#include <vector>

void sendBlameKeys(State *state, int32_t c)
{
	if (c == 27) { // ESC
		switchMode(state, NORMAL);
	} else if (c == 'k') {
		up(state);
	} else if (c == 'j') {
		down(state);
	} else if (c == '[') {
		state->file->row = getPrevLineSameIndent(state);
	} else if (c == ']') {
		state->file->row = getNextLineSameIndent(state);
	} else if (c == ctrl('u')) {
		upHalfScreen(state);
	} else if (c == ctrl('d')) {
		downHalfScreen(state);
	} else if (c == KEY_BACKSPACE || c == ctrl('h')) {
		if (state->blameSize - 5 > 10) {
			state->blameSize -= 5;
		}
	} else if (c == ctrl('l')) {
		state->blameSize += 5;
	} else if (c == 'y' || c == ctrl('y')) {
		std::string gitHash = getGitHash(state);
		copyToClipboard(state, gitHash, true);
		state->status = gitHash;
		switchMode(state, NORMAL);
	}
}
