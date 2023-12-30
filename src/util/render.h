#pragma once

#include "state.h"

void initColors();
int renderStatusBar(State state);
void renderVisibleLines(State state);
void renderScreen(State state, int maxY, int maxX);
