#include "textedit.h"
#include "string.h"

void getAndAddNumber(State *state, uint32_t row, uint32_t col, int32_t num)
{
	std::string number;
	int32_t startPos = col;
	if (std::isdigit(state->file->data[row][col])) {
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
			state->file->data[row] = state->file->data[row].substr(0, startPos) + std::to_string(temp) + safeSubstring(state->file->data[row], startPos + number.length());
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
	state->file->data.insert(state->file->data.begin() + state->file->row + 1, "");
}

void insertEmptyLine(State *state)
{
	state->file->data.insert(state->file->data.begin() + state->file->row, "");
}

std::string padTo(std::string str, const uint32_t num, const char paddingChar)
{
	auto tmp = str;
	if (num > str.size()) {
		tmp.insert(0, num - str.size(), paddingChar);
	}
	return tmp;
}

void swapCase(State *state, uint32_t r, uint32_t c)
{
	if (c < state->file->data[r].length()) {
		char tmp = state->file->data[r][c];
		if (isupper(tmp)) {
			state->file->data[r][c] = std::tolower(tmp);
		} else if (islower(tmp)) {
			state->file->data[r][c] = std::toupper(tmp);
		}
	}
}

void insertFinalEmptyNewline(State *state)
{
	if (state->file) {
		if (state->file->data.size() > 0 && state->file->data[state->file->data.size() - 1] != "") {
			state->file->data.push_back("");
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

