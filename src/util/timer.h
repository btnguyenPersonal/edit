#pragma once

#include <string>
#include <cstdint>
#include <chrono>

struct aggregatedTimer {
	std::string name;
	uint32_t totalElapsed;
	uint32_t num;
};

struct timer {
	std::string name;
	std::chrono::time_point<std::chrono::high_resolution_clock> start;
	std::chrono::time_point<std::chrono::high_resolution_clock> end;
	bool closed;
};

