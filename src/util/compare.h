#pragma once

#include "modes.h"
#include <vector>
#include <string>
#include <cstdint>

struct boolWithError {
	bool equal;
	std::string error;
};

struct boolWithError compare(std::vector<std::string> a, std::vector<std::string> b);
struct boolWithError compare(bool a, bool b);
struct boolWithError compare(std::string a, std::string b);
struct boolWithError compare(uint32_t a, uint32_t b);
struct boolWithError compare(Mode result, Mode expect);
