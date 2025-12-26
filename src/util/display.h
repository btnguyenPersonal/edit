#pragma once

#include "state.h"

bool isOffScreenVertical(State *state);
uint32_t getCenteredWindowPosition(State *state);
bool isWindowPositionInvalid(State *state);
bool isWindowPositionHorizontalInvalid(State *state);
uint32_t getDisplayRows(State *state, uint32_t r, uint32_t c);
uint32_t getDisplayCol(State *state);
uint32_t getDisplayLength(State *state, std::string s);
uint32_t getDisplayRows(State *state, uint32_t r);
uint32_t getNormalizedCol(State *state, uint32_t hardCol);
bool isColTooSmall(State *state);
uint32_t isLargeFile(State *state);
uint32_t getLastCharIndex(State *state);
void calcWindowBounds(State *state);
void refocusFileExplorer(State *state, bool changeMode);
void centerFileExplorer(State *state);
uint32_t getLineNumberOffset(State *state);
bool isColTooBig(State *state);
