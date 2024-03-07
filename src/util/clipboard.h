#pragma once

#include "state.h"
#include <string>
#include <vector>

void copyToClipboard(const std::string& originalString);
std::string getFromClipboard();
void pasteFromClipboard(State* state);
void pasteFromClipboardAfter(State* state);
