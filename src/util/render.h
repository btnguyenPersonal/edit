#pragma once

#include "state.h"

void initColors();
int renderStatusBar(State state);
void renderVisibleLines(State state);
void renderScreen(State state);
void initTerminal();
void printChar(State state, size_t i, size_t j);
