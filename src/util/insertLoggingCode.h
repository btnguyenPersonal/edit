#pragma once

#include "state.h"
#include <string>
#include <vector>

std::string getLoggingSearch(State *state);
bool isCExtension(std::string extension);
std::string getLoggingCode(State *state, std::string variableName);
std::string getLoggingRegex(State *state);
void toggleLoggingCode(State *state, std::string variableName);
void removeAllLoggingCode(State *state);
