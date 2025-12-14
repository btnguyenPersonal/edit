#include "modes.h"

std::string getMode(uint32_t mode)
{
	switch (mode) {
	case 0:
		return "VISUAL";
	case 1:
		return "COMMAND";
	case 2:
		return "INSERT";
	case 3:
		return "NORMAL";
	case 4:
		return "FIND";
	case 5:
		return "GREP";
	case 6:
		return "BLAME";
	case 7:
		return "MULTICURSOR";
	case 8:
		return "SEARCH";
	case 9:
		return "FILEEXPLORER";
	case 10:
		return "NAMING";
	case 11:
		return "DIFF";
	default:
		return "";
	}
}
