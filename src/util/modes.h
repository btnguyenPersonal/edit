#pragma once

#include <string>
#include <cstdint>

std::string getMode(uint32_t mode);

enum Mode { VISUAL, COMMANDLINE, TYPING, SHORTCUT, FIND, GREP, BLAME, MULTICURSOR, SEARCH, FILEEXPLORER, NAMING, DIFF };
