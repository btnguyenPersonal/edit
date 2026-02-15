#pragma once

#include "bounds.h"
#include "state.h"
#include <string>
#include <vector>

bool useLocalClipboard(State *state, bool useSystemClipboard);
void pasteFileFromClipboard(State *state, const std::string &destFolder);
void copyPathToClipboard(State *state, const std::string &filePath);
void copyToClipboard(State *state, const std::string &clip, bool useSystemClipboard);
std::string getFromClipboard(State *state, bool useSystemClipboard);
Bounds paste(State *state, std::string text, uint32_t offset);
Bounds pasteVisual(State *state, std::string text);
