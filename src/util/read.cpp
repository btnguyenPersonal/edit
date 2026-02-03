#include "read.h"
#include <fstream>

std::vector<std::string> readFile(const std::string &filename) {
	std::ifstream file(filename);

	if (!file.is_open()) {
		return {""};
	}

	std::string line;
	std::vector<std::string> lines;
	while (std::getline(file, line)) {
		lines.push_back(line);
	}
	if (lines.empty()) {
		lines.push_back("");
	}
	file.close();
	return lines;
}
