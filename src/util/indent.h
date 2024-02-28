#pragma once

#include <vector>
#include <string>
#include "state.h"

int getNumLeadingSpaces(std::string s);
std::string getPrevLine(State* state, unsigned int row);
int getIndentLevel(State* state, unsigned int row);
void indentLine(State* state, unsigned int row);
void indentLine(State* state);
void indentRange(State* state);
