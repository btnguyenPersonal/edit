#include "perf.h"

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
	if (timers.size() > 0) {
		endLastCheckpoint(timers);
		std::sort(timers.begin(), timers.end(), [](struct timer a, struct timer b) { return a.name < b.name; });
		std::vector<aggregatedTimer> aggregatedTimers;
		std::string lastName = timers[0].name;
		aggregatedTimer current = {};
		for (uint32_t i = 0; i < timers.size(); i++) {
			std::chrono::microseconds elapsed = std::chrono::duration_cast<std::chrono::microseconds>(timers[i].end - timers[i].start);
			if (timers[i].name != lastName) {
				lastName = timers[i].name;
				aggregatedTimers.push_back(current);
				current = {};
			}
			current.name = timers[i].name;
			current.totalElapsed += elapsed.count();
			current.num++;
		}
		aggregatedTimers.push_back(current);
		aggregatedTimer total = {};
		total.name = "total";
		for (uint32_t i = 0; i < aggregatedTimers.size(); i++) {
			printf("%30s(%d): %d μs\n", aggregatedTimers[i].name.c_str(), aggregatedTimers[i].num, aggregatedTimers[i].totalElapsed);
			total.totalElapsed += aggregatedTimers[i].totalElapsed;
			total.num ++;
		}
		printf("%30s(%d): %d μs\n", total.name.c_str(), total.num, total.totalElapsed);
		printf("\n");
		timers.clear();
		system("sleep 0.1");
	}
}
