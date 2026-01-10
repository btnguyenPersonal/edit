#include "mainLoop.h"
#include "sendKeys.h"
#include "../util/render.h"
#include "../util/cleanup.h"
#include <thread>
#include <chrono>

constexpr std::chrono::microseconds FRAME_TIME = std::chrono::microseconds(16666); // 60 fps

void mainLoop(State *state)
{
	std::chrono::time_point<std::chrono::high_resolution_clock> nextFrame = std::chrono::high_resolution_clock::now() + FRAME_TIME;
	while (true) {
		if (state->debug) {
			endwin();
			printCheckpoints(state->timers);
			initTerminal();
		} else {
			clearCheckpoints(state->timers);
		}

		startCheckpoint("==== editor logic ====", state->timers);

		bool changed = false;

		int32_t c;
		while ((c = getch()) != ERR) {
			changed = true;
			startCheckpoint("sendKeys" + c, state->timers);
			sendKeys(state, c);
			startCheckpoint("cleanup" + c, state->timers);
			cleanup(state, c);
			startCheckpoint("history" + c, state->timers);
			history(state, c);
		}


		if (changed) {
			startCheckpoint("autosaveFile", state->timers);
			autosaveFile(state);
		}

		startCheckpoint("preRenderCleanup", state->timers);
		preRenderCleanup(state);

		startCheckpoint("==== render logic ====", state->timers);

		renderScreen(state);

		auto now = std::chrono::high_resolution_clock::now();
		if (now < nextFrame) {
			std::this_thread::sleep_until(nextFrame);
		}
		nextFrame += FRAME_TIME;
	}
}
