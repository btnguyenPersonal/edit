#pragma once

#include "../util/bounds.h"
#include "../util/state.h"

std::string getInVisual(State *state, bool addNewlines);
bool isValidMoveableChunk(State *state, Bounds bounds);
Bounds getBounds(State *state);
void setStateFromWordPosition(State *state, WordPosition pos);
bool sendVisualKeys(State *state, char c, bool onlyMotions);
std::string getInVisual(State *state);
Position changeInVisual(State *state);
Position copyInVisual(State *state);
Position deleteInVisual(State *state);
