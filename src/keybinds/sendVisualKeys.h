#pragma once

#include "../util/state.h"

struct Bounds {
    unsigned int minR;
    unsigned int maxR;
    unsigned int minC;
    unsigned int maxC;
};

void setStateFromWordPosition(State* state, WordPosition pos);
Bounds getBounds(State* state);
void sendVisualKeys(State* state, char c);
std::string getInVisual(State* state);
Position changeInVisual(State* state);
Position copyInVisual(State* state);
Position deleteInVisual(State* state);
