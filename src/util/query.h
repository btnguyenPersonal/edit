#pragma once

#include <string>
#include <vector>
#include "state.h"

void add(Query* input, const char& c);
void addFromClipboard(Query* input);
void moveCursorLeft(Query* input);
void moveCursorRight(Query* input);
void setQuery(Query* input, std::string s);
void backspace(Query* input);
void backspaceAll(Query* input);