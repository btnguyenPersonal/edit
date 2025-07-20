#include "sendDiffKeys.h"

void sendDiffKeys(State *state, int32_t c)
{
	if (c == 27) { // ESC
		if (state->viewingDiff) {
			state->viewingDiff = false;
		} else {
			state->mode = SHORTCUTS;
		}
	} else if (c == 'k') {
		if (state->diffIndex < state->history.size()) {
			state->diffIndex++;
		}
	} else if (c == 'j') {
		if (state->diffIndex > 0) {
			state->diffIndex--;
		}
	} else if (c == '\n') {
		state->viewingDiff = true;
	}
}
