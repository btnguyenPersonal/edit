#pragma once

#include "state.h"
#include <string>
#include <vector>

int32_t getNumLeadingSpaces(std::string s);
std::string getPrevLine(State* state, uint32_t row);
int32_t getIndentLevel(State* state, uint32_t row);
void indentLine(State* state, uint32_t row);
void indentLine(State* state);
void indentRange(State* state);
