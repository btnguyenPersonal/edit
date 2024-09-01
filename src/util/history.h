#pragma once

#include "state.h"
#include <string>
#include <vector>

unsigned int applyDiff(State* state, std::vector<diffLine> diff, bool reverse);
std::vector<diffLine> generateDiff(const std::vector<std::string>& prev, const std::vector<std::string>& curr);
std::vector<diffLine> generateFastDiff(const std::vector<std::string>& prev, const std::vector<std::string>& curr);