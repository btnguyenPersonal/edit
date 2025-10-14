#pragma once

#include "state.h"
#include <filesystem>
#include <vector>

void generateGrepOutput(State *state, bool resetCursor);
std::vector<grepMatch> grepFiles(const std::filesystem::path &dir_path, const std::string &query, bool allowAllFiles);
