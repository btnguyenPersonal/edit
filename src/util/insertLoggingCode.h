#pragma once

#include "state.h"
#include <string>
#include <vector>

void toggleLoggingCode(State *state, std::string variableName);
std::string getLoggingCode(State *state, std::string variableName);
void removeAllLoggingCode(State *state);
std::string getLoggingRegex(State *state);
