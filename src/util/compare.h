#pragma once

#include "modes.h"
#include <vector>
#include <string>
#include <cstdint>

struct boolWithError {
	bool equal;
	std::string error;
};

std::string print(std::string prefix, std::string name, bool a);
std::string printInt(std::string prefix, std::string name, int32_t a);
std::string print(std::string prefix, std::string name, std::string a);
std::string print(std::string prefix, std::string name, std::vector<char> a);
std::string print(std::string prefix, std::string name, std::vector<std::string> a);
std::string print(std::string prefix, std::string name, uint32_t a);
std::string printMode(std::string prefix, std::string name, uint32_t a);
struct boolWithError compare(Mode result, Mode expect);
struct boolWithError compare(bool a, bool b);
struct boolWithError compareInt(int32_t result, int32_t expect);
struct boolWithError compare(std::string a, std::string b);
struct boolWithError compare(std::vector<std::string> a, std::vector<std::string> b);
struct boolWithError compare(uint32_t a, uint32_t b);
