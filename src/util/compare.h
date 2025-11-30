#pragma once

#include <vector>
#include <string>

struct boolWithError {
	bool equal;
	std::string error;
};

struct boolWithError compare(std::vector<std::string> a, std::vector<std::string> b);
