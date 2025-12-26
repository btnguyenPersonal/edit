#pragma once

#include "state.h"

void getAndAddNumber(State *state, uint32_t row, uint32_t col, int32_t num);
void insertEmptyLineBelow(State *state);
void insertEmptyLine(State *state);
std::string setStringToLength(const std::string &s, uint32_t length, bool showTilde);
std::string padTo(std::string str, const uint32_t num, const char paddingChar);
void swapCase(State *state, uint32_t r, uint32_t c);
void insertFinalEmptyNewline(State *state);
void ltrim(std::string &s);
void rtrim(std::string &s);
void trimTrailingWhitespace(State *state);
