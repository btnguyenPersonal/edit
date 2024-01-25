#pragma once

#include "state.h"
#include <string>

std::string minimize_filename(const std::string& filename);
int invertColor(int color);
void initColors();
int renderStatusBar(State* state);
void renderVisibleLines(State* state);
void renderScreen(State* state);
void initTerminal();
void printChar(State* state, size_t i, size_t j);
void printLine(State* state, int i);
void printLineNumber(int r, int i, bool isCurrentRow, bool recording);
bool isRowColInVisual(State* state, uint i, uint j);
