#pragma once

#include "state.h"
#include <string>
#include <vector>

// === READ-ONLY OPERATIONS ===
char textbuffer_getChar(State *state, uint32_t row, uint32_t col);
const std::string &textbuffer_getLine(State *state, uint32_t row);
uint32_t textbuffer_getLineCount(State *state);
uint32_t textbuffer_getLineLength(State *state, uint32_t row);
bool textbuffer_isValidPosition(State *state, uint32_t row, uint32_t col);
const std::vector<std::string> &textbuffer_getLines(State *state);

// === WRITE OPERATIONS ===
void textbuffer_insertChar(State *state, uint32_t row, uint32_t col, char c);
void textbuffer_deleteChar(State *state, uint32_t row, uint32_t col);
void textbuffer_replaceChar(State *state, uint32_t row, uint32_t col, char c);

void textbuffer_insertLine(State *state, uint32_t row, const std::string &line);
void textbuffer_deleteLine(State *state, uint32_t row);
void textbuffer_replaceLine(State *state, uint32_t row, const std::string &newLine);
void textbuffer_splitLine(State *state, uint32_t row, uint32_t col);
void textbuffer_joinLines(State *state, uint32_t row1, uint32_t row2);

void textbuffer_insertRange(State *state, uint32_t row, const std::vector<std::string> &lines);
void textbuffer_deleteRange(State *state, uint32_t startRow, uint32_t endRow);
void textbuffer_replaceRange(State *state, uint32_t startRow, uint32_t endRow, const std::vector<std::string> &lines);

// === CACHE MANAGEMENT ===
void textbuffer_invalidateCache(State *state);
void textbuffer_markModified(State *state);