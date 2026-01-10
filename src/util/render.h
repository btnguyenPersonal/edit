#pragma once

#include "state.h"
#include <ncurses.h>
#include <string>

struct Cursor {
	int32_t row;
	int32_t col;
};

void insertPixels(State *state, std::vector<Pixel> *pixels, chtype c, int32_t color);
void insertPixels(State *state, std::vector<Pixel> *pixels, std::string s, int32_t color);
int32_t invertColor(int32_t color);
void initColors();
int32_t renderStatusBar(State *state);
Cursor renderVisibleLines(State *state);
void renderScreen(State *state);
void renderScreen(State *state, bool fullRedraw);
void initTerminal();
void printChar(State *state, int32_t row, int32_t col, char c, int32_t color);
void printChar(State *state, int &row, int &col, char c, int32_t color, bool advance);
int32_t getSearchColor(State *state, int32_t row, uint32_t startOfSearch);
uint32_t renderAutoComplete(State *state, int32_t row, uint32_t col, uint32_t renderCol);
std::string getRenderBlameString(State *state);
void renderLineNumber(State *state, int32_t row, int32_t renderRow);
int32_t renderLineContent(State *state, int32_t row, int32_t renderRow, Cursor *cursor, bool multiLineComment, bool changeVisualColor);
bool endsWithSymbol(State *state, uint32_t row, std::string symbol);
bool startsWithSymbol(State *state, uint32_t row, std::string symbol);
