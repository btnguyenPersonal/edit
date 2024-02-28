#pragma once

#include <string>
#include <vector>
#include "state.h"

void toggleLoggingCode(State* state, std::string variableName);
std::string getLoggingCode(State* state, unsigned int row, std::string variableName);
void removeAllLoggingCode(State* state);
