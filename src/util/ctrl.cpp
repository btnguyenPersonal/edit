#include "ctrl.h"

char ctrl(char c)
{
	return c - 'a' + 1;
}

char unctrl(char c)
{
	return c + 'a' - 1;
}
