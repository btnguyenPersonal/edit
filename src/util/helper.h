#pragma once

#include <string>
#include <vector>
#include <fstream>
#include "state.h"

std::vector<std::string> readFile(std::string filename);
void saveFile(std::string filename, std::vector<std::string> data);
bool isWindowPositionInvalid(State state);
void centerScreen(State* state);
void up(State* state);
void down(State* state);
