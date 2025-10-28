#pragma once

#include "state.h"
#include <string>
#include <vector>

uint32_t applyDiff(State *state, std::vector<diffLine> diff, bool reverse);
std::vector<diffLine> generateDiff(const Rope &a, const Rope &b);
