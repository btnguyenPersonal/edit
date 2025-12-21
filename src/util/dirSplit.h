#pragma once

#include "state.h"
#include "helper.h"
#include <string>

struct DirSplit {
	std::string currentUncompleted;
	std::string lastDirectory;
};

struct DirSplit getCurrentDirSplit(State *state, std::string currentPathQuery);
