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
void downHalfScreen(State* state);
void upHalfScreen(State* state);
char ctrl(char c);
void left(State* state);
void right(State* state);
void calcWindowBounds();
int getIndexFirstNonSpace(State* state);
void copyToClipboard(std::string s);
std::string getFromClipboard();
bool handleMotion(State* state, char c, std::string motion);
bool isMotionCompleted(State* state);
