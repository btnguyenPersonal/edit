#pragma once

#include <string>
#include <vector>
#include "state.h"
#include "../global.h"

void toggleLoggingCode(State* state);
void insertLoggingCode(State* state);
std::string getLoggingCode(State* state, unsigned int row);
void removeAllLoggingCode(State* state);
