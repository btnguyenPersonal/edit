#include "mainLoop.h"
#include "sendKeys.h"
#include "../util/render.h"
#include "../util/cleanup.h"
#include <thread>
#include <chrono>

void mainLoop(State *state, int32_t c)
{
	auto start = std::chrono::high_resolution_clock::now();

	if (c != ERR) {
		sendKeys(state, c);
		cleanup(state, c);
		history(state, c);
		renderScreen(state);
	} else if (!state->shouldNotReRender.test_and_set()) {
		renderScreen(state);
	}

	auto end = std::chrono::high_resolution_clock::now();

	auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
	if (elapsed < std::chrono::microseconds(1000)) {
		std::this_thread::sleep_for(std::chrono::microseconds(1000) - elapsed);
	}
}
