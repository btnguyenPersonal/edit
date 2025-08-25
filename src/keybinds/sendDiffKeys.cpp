#include "sendDiffKeys.h"

void upLog(State* state)
{
	if (state->logIndex > 0) {
		state->logIndex--;
	}
}

void downLog(State* state)
{
	if (state->logIndex < state->logLines.size()) {
		state->logIndex++;
	}
}

void upDiff(State* state)
{
	if (state->diffIndex > 0) {
		state->diffIndex--;
	}
}

void downDiff(State* state)
{
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
	if (c == 'q') {
		state->mode = SHORTCUTS;
	} else if (c == 27) { // ESC
		if (state->viewingDiff) {
			state->logLines = getLogLines(state);
			state->viewingDiff = false;
			state->diffIndex = 0;
		} else {
			state->mode = SHORTCUTS;
		}
	} else if (c == ctrl('u') || c == 'u') {
		for (uint32_t i = 0; i < state->maxY / 2; i++) {
			if (state->viewingDiff) {
				upDiff(state);
			} else {
				upLog(state);
			}
		}
	} else if (c == ctrl('d') || c == 'd') {
		for (uint32_t i = 0; i < state->maxY / 2; i++) {
			if (state->viewingDiff) {
				downDiff(state);
			} else {
				downLog(state);
			}
		}
	} else if (c == 'h') {
		if (state->viewingDiff) {
			downLog(state);
			state->diffIndex = 0;
			state->diffLines = getDiffLines(state);
		}
	} else if (c == 'l') {
		if (state->viewingDiff) {
			upLog(state);
			state->diffIndex = 0;
			state->diffLines = getDiffLines(state);
		}
	} else if (c == 'j') {
		if (state->viewingDiff) {
			downDiff(state);
		} else {
			downLog(state);
		}
	} else if (c == 'k') {
		if (state->viewingDiff) {
			upDiff(state);
		} else {
			upLog(state);
		}
	} else if (c == '\n') {
		state->diffLines = getDiffLines(state);
		state->viewingDiff = true;
	}
}
