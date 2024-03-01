#pragma once

#include "state.h"
#include <string>
#include <vector>

void toggleLoggingCode(State* state, std::string variableName);
std::string getLoggingCode(State* state, unsigned int row, std::string variableName);
void removeAllLoggingCode(State* state);
