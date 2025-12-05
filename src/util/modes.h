#pragma once

#include <string>
#include <cstdint>

std::string getMode(uint32_t mode);

enum Mode { VISUAL, COMMANDLINE, TYPING, SHORTCUTS, FINDFILE, GREP, BLAME, MULTICURSOR, SEARCH, FILEEXPLORER, NAMING, DIFF };
