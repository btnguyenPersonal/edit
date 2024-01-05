#pragma once

#include "state.h"
#include <string>

void initColors();
int renderStatusBar(State* state);
void renderVisibleLines(State* state);
void renderScreen(State* state);
void initTerminal();
void printChar(State* state, size_t i, size_t j);
void printLine(std::string line, int i, int windowPosition);
void printLineNumber(int i, int windowPosition);
