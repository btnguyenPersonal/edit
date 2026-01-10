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
		int32_t c;
		while ((c = getch()) != ERR) {
			sendKeys(state, c);
			cleanup(state, c);
			history(state, c);
			autosaveFile(state);
		}

		renderScreen(state);

		auto now = std::chrono::high_resolution_clock::now();
		if (now < nextFrame) {
			std::this_thread::sleep_until(nextFrame);
		}
		nextFrame += FRAME_TIME;
	}
}
