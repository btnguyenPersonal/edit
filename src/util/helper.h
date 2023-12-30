#pragma once

#include <string>
#include <vector>
#include <fstream>
#include "state.h"

std::vector<std::string> readFile(std::string filename);
void saveFile(std::string filename, std::vector<std::string> data);
bool isWindowPositionInvalid(State state, int maxY);
void centerScreen(State* state, int maxY);
