#pragma once

#include <vector>
#include <chrono>
#include <string>

struct timer {
	std::string name;
	std::chrono::time_point<std::chrono::high_resolution_clock> start;
	std::chrono::time_point<std::chrono::high_resolution_clock> end;
	bool closed;
};

void startCheckpoint(const std::string &name, std::vector<timer> &timers);
void printCheckpoints(std::vector<timer> &timers);
void endLastCheckpoint(std::vector<timer> &timers);
