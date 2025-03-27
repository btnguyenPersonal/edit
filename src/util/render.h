#pragma once

#include "state.h"
#include <ncurses.h>
#include <string>

struct Cursor {
    int row;
    int col;
};

std::string minimize_filename(const std::string& filename);
int invertColor(int color);
void initColors();
int renderStatusBar(State* state);
Cursor renderVisibleLines(State* state);
void renderScreen(State* state);
void renderScreen(State* state, bool fullRedraw);
void initTerminal();
void advancePosition(State* state, int& renderRow, int& renderCol);
void printChar(State* state, int row, int col, char c, int color);
void printChar(State* state, int& row, int& col, char c, int color, bool advance);
int getSearchColor(State* state, int row, unsigned int startOfSearch);
unsigned int renderAutoComplete(State* state, int row, unsigned int col, unsigned int renderCol);
std::string getRenderBlameString(State* state);
void printLineNumber(State* state, int row, int renderRow);
int printLineContent(State* state, int row, int renderRow, Cursor* cursor);
bool isRowColInVisual(State* state, unsigned int i, unsigned int j);
