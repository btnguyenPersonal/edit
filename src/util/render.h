#pragma once

#include "state.h"
#include <ncurses.h>
#include <string>

struct Cursor {
	int32_t row;
	int32_t col;
};

std::string getHarpoonName(State *state, uint32_t index);
std::string minimize_filename(const std::string &filename);
int32_t invertColor(int32_t color);
void initColors();
int32_t renderStatusBar(State *state);
Cursor renderVisibleLines(State *state);
void renderScreen(State *state);
void renderScreen(State *state, bool fullRedraw);
void initTerminal();
void advancePosition(State *state, int &renderRow, int &renderCol);
void printChar(State *state, int32_t row, int32_t col, char c, int32_t color);
void printChar(State *state, int &row, int &col, char c, int32_t color,
	       bool advance);
int32_t getSearchColor(State *state, int32_t row, uint32_t startOfSearch);
uint32_t renderAutoComplete(State *state, int32_t row, uint32_t col,
			    uint32_t renderCol);
std::string getRenderBlameString(State *state);
void printLineNumber(State *state, int32_t row, int32_t renderRow);
int32_t printLineContent(State *state, int32_t row, int32_t renderRow,
			 Cursor *cursor);
bool isRowColInVisual(State *state, uint32_t i, uint32_t j);
