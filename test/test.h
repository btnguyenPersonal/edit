#pragma once

#include "../src/util/compare.h"

#include <string>
#include <vector>

struct testRun {
	std::string name;
	struct boolWithError result;
};

struct testSuiteRun {
	std::string file;
	std::vector<struct testRun> runs;
};
