#pragma once

#include <string>
#include <vector>
#include "state.h"

void copyToClipboard(std::string s);
std::vector<std::string> getFromClipboard();
void pasteFromClipboard(State* state);
void pasteFromClipboardAfter(State* state);
