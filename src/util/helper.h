#pragma once

#include <string>
#include <vector>
#include <fstream>

std::vector<std::string> readFile(std::string filename);
void saveFile(std::string filename, std::vector<std::string> data);
