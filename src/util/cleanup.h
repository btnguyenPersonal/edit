#pragma once

#include "state.h"
#include <string>
#include <vector>

void cleanup(State *state, char c);
void history(State *state, char c);
void autosaveFile(State* state);
