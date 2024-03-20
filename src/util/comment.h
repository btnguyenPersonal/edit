#pragma once

#include "bounds.h"
#include "state.h"
#include <string>
#include <vector>

void toggleComment(State* state);
void toggleCommentHelper(State* state, unsigned int row, int commentIndex);
void toggleCommentLines(State* state, Bounds bounds);
std::string trimLeadingComment(State* state, std::string line);
std::string trimComment(State* state, std::string line);
bool isComment(State* state, std::string line);
bool isCommentWithSpace(State* state, std::string line);
void unCommentBlock(State* state);
