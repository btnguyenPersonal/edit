#pragma once

#include <string>
#include <vector>
#include <fstream>
#include "state.h"

void generateFindFileOutput(State* state);
bool isAlphaNumeric(char c);
uint w(State* state);
uint b(State* state);
void insertEmptyLineBelow(State* state);
void indent(State* state);
void deindent(State* state);
void insertEmptyLine(State* state);
std::vector<std::string> readFile(std::string filename);
void saveFile(std::string filename, std::vector<std::string> data);
bool isWindowPositionInvalid(State* state);
void centerScreen(State* state);
void up(State* state);
void down(State* state);
void left(State* state);
void right(State* state);
void downHalfScreen(State* state);
void upHalfScreen(State* state);
char ctrl(char c);
void calcWindowBounds();
int getIndexFirstNonSpace(State* state);
bool handleMotion(State* state, char c, std::string motion);
bool isMotionCompleted(State* state);
void sanityCheckRowColOutOfBounds(State* state);
int minimum(int a, int b);
int maximum(int a, int b);
