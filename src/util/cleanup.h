#pragma once

#include "state.h"
#include <string>
#include <vector>

int32_t cleanKey(int32_t c);
void preRenderCleanup(State *state);
void cleanup(State *state, char c);
void cleanup(State *state);
void history(State *state);
void autosaveFile(State *state);
void autoloadFile(State *state);
