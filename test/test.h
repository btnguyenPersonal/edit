#pragma once

#include <string>
#include <vector>

struct testRun {
	std::string name;
	bool pass;
};

struct testSuiteRun {
	std::string file;
	std::vector<struct testRun> runs;
};
