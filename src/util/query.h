#pragma once

#include "state.h"
#include <string>
#include <vector>

void add(Query *input, const char &c);
void addFromClipboard(Query *input);
void moveCursorLeft(Query *input);
void moveCursorRight(Query *input);
void setQuery(Query *input, std::string s);
void backspaceWord(Query *input);
void backspace(Query *input);
void backspaceAll(Query *input);
