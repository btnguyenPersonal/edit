#pragma once

#include "state.h"
#include <ncurses.h>
#include <string>

std::string minimize_filename(const std::string& filename);
int invertColor(int color);
void initColors();
int renderStatusBar(State* state);
void renderVisibleLines(State* state);
void renderScreen(State* state);
void renderScreen(State* state, bool fullRedraw);
void initTerminal();
void printChar(State* state, int row, int col, char c, int color);
int getSearchColor(State* state, int row, unsigned int startOfSearch);
unsigned int renderAutoComplete(State* state, int row, unsigned int col, unsigned int renderCol);
void printLine(State* state, int i);
void printLineNumber(State* state, int r, int i, bool isCurrentRow, bool recording, std::string blame);
bool isRowColInVisual(State* state, unsigned int i, unsigned int j);
