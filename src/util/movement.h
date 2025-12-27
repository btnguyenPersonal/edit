#pragma once

#include "state.h"

void centerScreen(State *state);
int32_t findChar(State *state, bool reverse, char c);
int32_t toChar(State *state, bool reverse, char c);
uint32_t w(State *state);
uint32_t b(State *state);
uint32_t findNextChar(State *state, char c);
uint32_t toNextChar(State *state, char c);
uint32_t findPrevChar(State *state, char c);
uint32_t toPrevChar(State *state, char c);
uint32_t getNextEmptyLine(State *state);
uint32_t getNextLineSameIndent(State *state);
uint32_t getPrevEmptyLine(State *state);
uint32_t getPrevLineSameIndent(State *state);
void down(State *state);
void downHalfScreen(State *state);
void downScreen(State *state);
void downVisual(State *state);
void left(State *state);
void right(State *state);
void up(State *state);
void upHalfScreen(State *state);
void upScreen(State *state);
void upVisual(State *state);
