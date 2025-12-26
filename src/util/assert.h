#pragma once

#define assert(condition)                                                                                         \
	if (!(condition)) {                                                                                       \
		endwin();                                                                                         \
		printf("\n\e[31mAssertion failed: \e[0m%s \e[32m%s:%d\e[0m\n\n", #condition, __FILE__, __LINE__); \
		exit(1);                                                                                          \
	}
