#include "query.h"
#include "clipboard.h"
#include <string>

void add(Query *input, const char &c)
{
	input->query.insert(input->cursor, std::string(1, c));
	input->cursor += 1;
}

void add(Query *input, std::string s)
{
	input->query.insert(input->cursor, s);
	input->cursor += s.length();
}

void setQuery(Query *input, std::string s)
{
	input->query = s;
	input->cursor = s.length();
}

void moveCursorStart(Query *input)
{
	input->cursor = 0;
}

void moveCursorEnd(Query *input)
{
	input->cursor = input->query.length();
}

void moveCursorLeft(Query *input)
{
	if (input->cursor > 0) {
		input->cursor -= 1;
	}
}

void moveCursorRight(Query *input)
{
	if (input->cursor < input->query.length()) {
		input->cursor += 1;
	}
}

void backspaceWord(Query *input)
{
	while (input->cursor > 0) {
		backspace(input);
		if (input->cursor == 0 || input->query[input->cursor - 1] == ' ') {
			backspace(input);
			return;
		}
	}
}

void backspace(Query *input)
{
	if (input->query.length() > 0 && input->cursor > 0) {
		input->query.erase(input->cursor - 1, 1);
		input->cursor -= 1;
	}
}

void backspaceAll(Query *input)
{
	input->query = std::string("");
	input->cursor = 0;
}
