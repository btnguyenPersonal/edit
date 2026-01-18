#include "computeFrame.h"
#include "render.h"
#include "cleanup.h"
#include "../keybinds/sendKeys.h"

void computeFrame(State *state)
{
	if (state->debug) {
		endwin();
		printCheckpoints(state->timers);
		initTerminal();
	} else {
		clearCheckpoints(state->timers);
	}

	startCheckpoint("==== editor logic ====", state->timers);

	int32_t cachedHistoryPosition = -1;
	if (state->file) {
		cachedHistoryPosition = state->file->historyPosition;
	}

	bool sentKey = false;
	int32_t c = ERR;
	int32_t temp = ERR;
	while ((temp = getch()) != ERR) {
		c = temp;
		startCheckpoint("sendKeys", state->timers);
		sendKeys(state, c);
		startCheckpoint("cleanup", state->timers);
		cleanup(state, c);
		sentKey = true;
	}

	if (sentKey) {
		startCheckpoint("history", state->timers);
		history(state, c);
	}

	if (state->file && cachedHistoryPosition != state->file->historyPosition) {
		startCheckpoint("autosaveFile", state->timers);
		autosaveFile(state);
	}

	state->frameCounter++;
	if (state->frameCounter >= 60) {
		state->frameCounter = 0;
		startCheckpoint("autoloadFile", state->timers);
		autoloadFile(state);
	}

	if (sentKey || !state->shouldNotReRender.test_and_set()) {
		startCheckpoint("preRenderCleanup", state->timers);
		preRenderCleanup(state);

		startCheckpoint("==== render logic ====", state->timers);
		renderScreen(state);
	}
}
