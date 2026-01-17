#pragma once

#include "state.h"
#include <string>
#include <vector>

uint32_t getIndent(const std::string &str);
char getIndentCharacter(State *state);
uint32_t getIndentSize(State *state);
uint32_t getIndent(State *state, const std::string &str);
void indent(State *state);
void deindent(State *state);
void indentLineWhenTypingLastChar(State *state);
void indentLineWhenTypingFirstChar(State *state);
bool autoIndentDisabledFileType(State *state);
bool isFirstNonSpace(State *state);
int32_t getNumLeadingIndentCharacters(State *state, std::string s);
std::string getPrevLine(State *state, uint32_t row);
int32_t getIndentLevel(State *state, uint32_t row);
void indentLine(State *state, uint32_t row);
void indentLine(State *state);
void indentRange(State *state);
