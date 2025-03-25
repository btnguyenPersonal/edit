#pragma once

#include "state.h"
#include <ncurses.h>
#include <string>

struct Cursor {
    int x;
    int y;
};

std::string minimize_filename(const std::string& filename);
int invertColor(int color);
void initColors();
int renderStatusBar(State* state);
Cursor renderVisibleLines(State* state);
void renderScreen(State* state);
void renderScreen(State* state, bool fullRedraw);
void initTerminal();
void printChar(State* state, int row, int col, char c, int color);
int getSearchColor(State* state, int row, unsigned int startOfSearch);
unsigned int renderAutoComplete(State* state, int row, unsigned int col, unsigned int renderCol);
std::string getRenderBlameString(State* state);
int printLineContent(State* state, int row, int renderRow);
void printLineNumber(State* state, int row, int renderRow);
bool isRowColInVisual(State* state, unsigned int i, unsigned int j);
