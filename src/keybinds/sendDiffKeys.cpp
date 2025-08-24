#include "sendDiffKeys.h"

void sendDiffKeys(State *state, int32_t c)
{
	if (state->viewingDiff) {
		state->diffLines = getDiffLines(state);
	} else {
		state->logLines = getLogLines(state);
	}
	if (c == 'q' || c == 27) { // ESC
		if (state->viewingDiff) {
			state->logLines = getLogLines(state);
			state->viewingDiff = false;
			state->diffIndex = 0;
		} else {
			state->mode = SHORTCUTS;
		}
	} else if (c == 'j') {
		if (state->viewingDiff) {
			if (state->diffIndex < state->diffLines.size()) {
				state->diffIndex++;
			}
		} else {
			if (state->logIndex < state->logLines.size()) {
				state->logIndex++;
			}
		}
	} else if (c == 'k') {
		if (state->viewingDiff) {
			if (state->diffIndex > 0) {
				state->diffIndex--;
			}
		} else {
			if (state->logIndex > 0) {
				state->logIndex--;
			}
		}
	} else if (c == '\n') {
		state->diffLines = getDiffLines(state);
		state->viewingDiff = true;
	}
}
