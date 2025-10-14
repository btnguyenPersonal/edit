#pragma once

#include <filesystem>

bool isTestFile(const std::string &filepath);
bool shouldIgnoreFile(const std::filesystem::path &path);
