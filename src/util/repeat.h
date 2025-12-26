#pragma once

#include "state.h"
#include <cstdint>
#include <vector>

void setDotCommand(State *state, int32_t c);
void setDotCommand(State *state, std::vector<int32_t> s);
