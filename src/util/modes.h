#pragma once

#include <cstdint>
#include <string>

std::string getMode(uint32_t mode);

enum Mode {
	VISUAL,
	COMMAND,
	INSERT,
	NORMAL,
	FIND,
	GREP,
	BLAME,
	MULTICURSOR,
	SEARCH,
	FILEEXPLORER,
	NAMING,
	DIFF
};
