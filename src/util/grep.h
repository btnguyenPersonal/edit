#pragma once

#include "state.h"
#include <filesystem>
#include <vector>

void generateGrepOutput(State *state, bool resetCursor);
std::vector<grepMatch> grepFiles(std::filesystem::path dir_path, std::string query, bool allowAllFiles);
void changeToGrepFile(State *state);
void findDefinitionFromGrepOutput(State *state, std::string s);
