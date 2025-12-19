#pragma once

#include "state.h"
#include "bounds.h"
#include <string>
#include <vector>

void pasteFileFromClipboard(State *state, const std::string &destFolder);
void copyPathToClipboard(State *state, const std::string &filePath);
void copyToClipboard(State *state, const std::string &clip, bool useSystemClipboard);
std::string getFromClipboard(State *state, bool useSystemClipboard);
Bounds pasteVisual(State *state, std::string text);
Bounds paste(State *state, std::string text);
Bounds pasteAfter(State *state, std::string text);
