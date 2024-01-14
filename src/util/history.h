#pragma once

#include <vector>
#include <string>
#include "state.h"

uint applyDiff(State* state, const std::vector<diffLine>& diff, bool reverse);
std::vector<diffLine> generateDiff(const std::vector<std::string>& prev, const std::vector<std::string>& curr);