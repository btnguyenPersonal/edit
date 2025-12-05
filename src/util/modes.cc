#include "modes.h"

std::string getMode(uint32_t mode)
{
	switch (mode) {
	case 0:
		return "VISUAL";
	case 1:
		return "COMMANDLINE";
	case 2:
		return "TYPING";
	case 3:
		return "SHORTCUTS";
	case 4:
		return "FINDFILE";
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
