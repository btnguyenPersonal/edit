#include "read.h"
#include <fstream>

std::vector<std::string> readFile(const std::string &filename)
{
	std::ifstream file(filename);
	std::string str;
	std::vector<std::string> file_contents;
	while (std::getline(file, str)) {
		file_contents.push_back(str);
	}
	if (file.eof() && str.empty()) {
		file_contents.push_back("");
	}
	file.close();
	return file_contents;
}
