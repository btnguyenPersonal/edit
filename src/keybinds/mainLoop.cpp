#include "mainLoop.h"
#include "sendKeys.h"
#include "../util/render.h"
#include "../util/cleanup.h"
#include <thread>
#include <chrono>

void mainLoop(State *state, int32_t c)
{
	if (c != ERR) {
		sendKeys(state, c);
		cleanup(state, c);
		history(state, c);
		renderScreen(state);
	} else if (state->shouldReRender) {
		state->shouldReRender = false;
		renderScreen(state);
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(1));
}
