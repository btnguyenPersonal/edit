#include "mainLoop.h"
#include "computeFrame.h"
#include <chrono>
#include <thread>

constexpr std::chrono::microseconds FRAME_TIME = std::chrono::microseconds(16666); // 60hz render rate max

void mainLoop(State *state) {
	std::chrono::time_point<std::chrono::high_resolution_clock> nextFrame = std::chrono::high_resolution_clock::now() + FRAME_TIME;
	while (true) {
		computeFrame(state);

		if (std::chrono::high_resolution_clock::now() < nextFrame) {
			std::this_thread::sleep_until(nextFrame);
		}
		nextFrame += FRAME_TIME;
	}
}
