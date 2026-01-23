#include "computeFrame.h"
#include "render.h"
#include "cleanup.h"
#include "../keybinds/sendKeys.h"

void computeFrame(State *state)
{
	if (state->debug) {
		endwin();
		printCheckpoints(state->debug, state->timers);
		initTerminal();
	}

	startCheckpoint(state->debug, "==== editor logic ====", state->timers);

	int32_t cachedHistoryPosition = -1;
	if (state->file) {
		cachedHistoryPosition = state->file->historyPosition;
	}

	int32_t c = ERR;
	std::vector<char*> userInput;
	char temp[4096];
	while (getnstr(temp, 4096) == OK) {
		userInput.push_back(temp);
	}
	state->status = std::to_string(userInput.size());

	// for (uint32_t i = 0; i < userInput.size(); i++) {
	// 	c = userInput[i];
	// 	startCheckpoint(state->debug, "sendKeys", state->timers);
	// 	sendKeys(state, c);
	// 	startCheckpoint(state->debug, "cleanup", state->timers);
	// 	cleanup(state, c);
	// }

	// if (userInput.size() > 0) {
	// 	startCheckpoint(state->debug, "history", state->timers);
	// 	history(state, c);
	// }

	startCheckpoint(state->debug, "==== autosaveFile logic ====", state->timers);

	if (state->file && cachedHistoryPosition != state->file->historyPosition) {
		startCheckpoint(state->debug, "autosaveFile", state->timers);
		autosaveFile(state);
	}

	startCheckpoint(state->debug, "==== autoloadFile logic ====", state->timers);

	state->frameCounter++;
	if (state->frameCounter >= 60) {
		state->frameCounter = 0;
		startCheckpoint(state->debug, "autoloadFile", state->timers);
		autoloadFile(state);
	}

	endLastCheckpoint(state->debug, state->timers);

	if (userInput.size() > 0 || !state->shouldNotReRender.test_and_set()) {
		startCheckpoint(state->debug, "preRenderCleanup", state->timers);
		preRenderCleanup(state);

		startCheckpoint(state->debug, "==== render logic ====", state->timers);
		renderScreen(state);
	}
}
