#include "perf.h"

// 
// startCheckpoint("firstMakeCheckpoint", timers);

void endLastCheckpoint(std::vector<timer> &timers)
{
	if (timers.size() > 0 && !timers.back().closed) {
		timers.back().end = std::chrono::high_resolution_clock::now();
		timers.back().closed = true;
	}
}

void startCheckpoint(const std::string &name, std::vector<timer> &timers)
{
	endLastCheckpoint(timers);
	struct timer current = {};
	current.name = name;
	current.start = std::chrono::high_resolution_clock::now();
	timers.push_back(current);
}

void printCheckpoints(std::vector<timer> &timers)
{
	endLastCheckpoint(timers);
	for (uint32_t i = 0; i < timers.size(); i++) {
		std::chrono::microseconds elapsed = std::chrono::duration_cast<std::chrono::microseconds>(timers[i].end - timers[i].start);
		printf("%30s: %ld\n", timers[i].name.c_str(), elapsed.count());
	}
	printf("\n");
	timers.clear();
	system("sleep 0.1");
}

