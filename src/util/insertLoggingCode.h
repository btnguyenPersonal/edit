#pragma once

#include <string>
#include <vector>
#include "state.h"

void toggleLoggingCode(State* state);
void insertLoggingCode(State* state);
std::string getLoggingCode(State* state, uint row);
void removeAllLoggingCode(State* state);
