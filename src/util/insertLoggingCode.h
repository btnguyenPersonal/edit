#pragma once

#include "state.h"
#include <string>
#include <vector>

void toggleLoggingCode(State *state, std::string variableName, bool showValue);
std::string getLoggingCode(State *state, uint32_t row, std::string variableName, bool showValue);
void removeAllLoggingCode(State *state);
std::string getLoggingRegex(State *state);
