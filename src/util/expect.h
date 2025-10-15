#include <iostream>
#include <ncurses.h>

#define expect(condition)                                                                                    \
	if (!(condition)) {                                                                                  \
		endwin();                                                                                    \
		std::cerr << "\n\e[31mAssertion failed: \e[0m" << #condition << " \e[32m" << __FILE__ << ":" \
			  << __LINE__ << "\e[0m\n\n";                                                        \
		std::abort();                                                                                \
	}

#pragma once
