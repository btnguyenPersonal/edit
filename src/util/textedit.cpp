#include "textedit.h"
#include "string.h"
#include "textbuffer.h"

void getAndAddNumber(State *state, uint32_t row, uint32_t col, int32_t num)
{
	if (col < state->file->data[row].length() && std::isdigit(state->file->data[row][col])) {
		std::string number;
		int32_t startPos = col;
		number += state->file->data[row][col];
		for (int32_t i = (int32_t)col - 1; i >= 0; i--) {
			if (std::isdigit(state->file->data[row][i])) {
				number = state->file->data[row][i] + number;
				startPos = i;
			} else {
				break;
			}
		}
		for (uint32_t i = col + 1; i < state->file->data[row].length(); i++) {
			if (std::isdigit(state->file->data[row][i])) {
				number += state->file->data[row][i];
			} else {
				break;
			}
		}
		try {
			long long temp = std::stoll(number);
			if (temp + num < 0) {
				temp = 0;
			} else {
				temp += num;
			}
			std::string line = textbuffer_getLine(state, row);
			std::string newContent = line.substr(0, startPos) + std::to_string(temp) + safeSubstring(line, startPos + number.length());
			textbuffer_replaceLine(state, row, newContent);
		} catch (const std::exception &e) {
			state->status = "number too large";
		}
	}
}

std::string setStringToLength(const std::string &s, uint32_t length, bool showTilde)
{
	if (s.length() <= length) {
		return s;
	} else {
		std::string output = safeSubstring(s, s.length() - length, length);
		if (showTilde) {
			output[0] = '~';
		}
		return output;
	}
}

void insertEmptyLineBelow(State *state)
{
	textbuffer_insertLine(state, state->file->row + 1, "");
}

void insertEmptyLine(State *state)
{
	textbuffer_insertLine(state, state->file->row, "");
}

std::string padTo(const std::string &str, const uint32_t num, const char paddingChar)
{
	auto tmp = str;
	if (num > str.size()) {
		tmp.insert(0, num - str.size(), paddingChar);
	}
	return tmp;
}

void swapCase(State *state, uint32_t r, uint32_t c)
{
	if (textbuffer_isValidPosition(state, r, c)) {
		char tmp = textbuffer_getChar(state, r, c);
		if (isupper(tmp)) {
			textbuffer_replaceChar(state, r, c, std::tolower(tmp));
		} else if (islower(tmp)) {
			textbuffer_replaceChar(state, r, c, std::toupper(tmp));
		}
	}
}

void insertFinalEmptyNewline(State *state)
{
	if (state->file) {
		if (textbuffer_getLineCount(state) > 0 && textbuffer_getLine(state, textbuffer_getLineCount(state) - 1) != "") {
			textbuffer_insertLine(state, textbuffer_getLineCount(state), "");
		}
	}
}

void rtrim(std::string &s)
{
	s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !isWhitespace(ch); }).base(), s.end());
}

void ltrim(std::string &s)
{
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) { return !isWhitespace(ch); }));
}

void trimTrailingWhitespace(State *state)
{
	for (uint32_t i = 0; i < state->file->data.size(); i++) {
		rtrim(state->file->data[i]);
	}
}
