#pragma once

#include "../util/state.h"
#include "../util/bounds.h"

void setStateFromWordPosition(State* state, WordPosition pos);
Bounds getBounds(State* state);
bool sendVisualKeys(State* state, char c);
std::string getInVisual(State* state);
Position changeInVisual(State* state);
Position copyInVisual(State* state);
Position deleteInVisual(State* state);
