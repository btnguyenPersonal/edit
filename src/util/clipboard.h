#pragma once

#include <string>
#include <vector>

void copyToClipboard(const std::string& originalString);
std::vector<std::string> getFromClipboard();
void pasteFromClipboard(State* state);
void pasteFromClipboardAfter(State* state);
