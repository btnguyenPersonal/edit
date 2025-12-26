#pragma once

#include "state.h"
#include <string>
#include <vector>

uint32_t applyDiff(State *state, std::vector<diffLine> diff, bool reverse);
std::vector<diffLine> generateDiff(const std::vector<std::string> &prev, const std::vector<std::string> &curr);
void recordHistory(State *state, std::vector<diffLine> diff);
void recordJumpList(State *state);
void recordMacroCommand(State *state, char c);
void recordMotion(State *state, int32_t c);
void forwardFileStack(State *state);
void backwardFileStack(State *state);
