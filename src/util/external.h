#pragma once

#include "state.h"

std::vector<std::string> getDiffLines(State *state);
std::vector<std::string> getLogLines(State *state);
std::string runCommand(std::string command);
std::string getGitHash(State *state);
std::vector<std::string> getGitBlame(const std::string &filename);
