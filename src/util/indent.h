#pragma once

#include <vector>
#include <string>
#include "state.h"

int getNumLeadingSpaces(std::string s);
std::string getPrevLine(State* state, uint row);
int getIndentLevel(State* state, uint row);
void indentLine(State* state);
