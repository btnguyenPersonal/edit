#pragma once

#include "state.h"
#include <string>
#include <vector>

void indentLineWhenTypingFirstChar(State *state);
bool autoIndentDisabledFileType(std::string filename);
int32_t getNumLeadingIndentCharacters(State *state, std::string s);
std::string getPrevLine(State *state, uint32_t row);
int32_t getIndentLevel(State *state, uint32_t row);
void indentLine(State *state, uint32_t row);
void indentLine(State *state);
void indentRange(State *state);
