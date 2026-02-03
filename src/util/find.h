#pragma once

#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

#include "state.h"

bool isAllLowercase(const std::string &str);
bool filePathContainsSubstring(const std::filesystem::path &filePath, const std::string &query);
std::vector<std::filesystem::path> find(const std::filesystem::path &dir_path, const std::string &query);
void generateFindOutput(State *state);
int32_t maxConsecutiveMatch(const std::filesystem::path &filePath, const std::string &query);
