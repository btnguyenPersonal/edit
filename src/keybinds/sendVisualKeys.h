#pragma once

#include "../util/bounds.h"
#include "../util/state.h"

Bounds getBounds(State* state);
void setStateFromWordPosition(State* state, WordPosition pos);
bool sendVisualKeys(State* state, char c);
std::string getInVisual(State* state);
Position changeInVisual(State* state);
Position copyInVisual(State* state);
Position deleteInVisual(State* state);
