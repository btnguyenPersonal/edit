#pragma once

#include <ncurses.h>

#define assert(condition) if (!(condition)) { endwin(); printf("\n\033[31mAssertion failed: \033[0m%s \033[32m%s:%d\033[0m\n\n", #condition, __FILE__, __LINE__); exit(1); }
