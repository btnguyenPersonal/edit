#pragma once

#include "../global.h"
#include <string>
#include <vector>
#include "state.h"

void copyToClipboard(const std::string& originalString);
std::vector<std::string> getFromClipboard();
void pasteFromClipboard(State* state);
void pasteFromClipboardAfter(State* state);
