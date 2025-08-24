#include "sendDiffKeys.h"

void upDiff(State* state) {
	if (state->diffIndex > 0) {
		state->diffIndex--;
	}
}
void downDiff(State* state) {
	if (state->diffIndex < state->diffLines.size()) {
		state->diffIndex++;
	}
}
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
	} else if (c == ctrl('u') || c == 'u') {
		if (state->viewingDiff) {
			for (uint32_t i = 0; i < state->maxY / 2; i++) {
				upDiff(state);
			}
		}
	} else if (c == ctrl('d') || c == 'd') {
		if (state->viewingDiff) {
			for (uint32_t i = 0; i < state->maxY / 2; i++) {
				downDiff(state);
			}
		}
	} else if (c == 'j') {
		if (state->viewingDiff) {
			downDiff(state);
		} else {
			if (state->logIndex < state->logLines.size()) {
				state->logIndex++;
			}
		}
	} else if (c == 'k') {
		if (state->viewingDiff) {
			upDiff(state);
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
