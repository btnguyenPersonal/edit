#pragma once

#include "state.h"
#include <string>
#include <vector>

int copyToClipboard(const std::string& originalString);
std::string getFromClipboard();
void pasteFromClipboard(State* state);
void pasteFromClipboardAfter(State* state);
