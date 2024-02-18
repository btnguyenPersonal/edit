#pragma once

#include <vector>
#include <string>
#include "state.h"
#include "bounds.h"

void toggleComment(State* state);
void toggleCommentHelper(State* state, unsigned int row, int commentIndex);
void toggleCommentLines(State* state, Bounds bounds);
std::string trimComment(State* state, std::string line);
bool isComment(State* state, std::string line);
bool isCommentWithSpace(State* state, std::string line);
void unCommentBlock(State* state);
