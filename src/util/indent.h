#pragma once

#include <vector>
#include <string>
#include "state.h"

int getNumLeadingSpaces(std::string s);
void indentLine(State* state);
std::string getPrevLine(State* state);
uint getIndentLevel(State* state);
