#include "string.h"
#include "assert.h"

uint32_t buildNumberFromString(const std::string &s)
{
	uint32_t output = 0;
	if (!isNumber(s)) {
		return output;
	}
	for (uint32_t i = 0; i < s.length(); i++) {
		output *= 10;
		switch (s[i]) {
		case '0':
			output += 0;
			break;
		case '1':
			output += 1;
			break;
		case '2':
			output += 2;
			break;
		case '3':
			output += 3;
			break;
		case '4':
			output += 4;
			break;
		case '5':
			output += 5;
			break;
		case '6':
			output += 6;
			break;
		case '7':
			output += 7;
			break;
		case '8':
			output += 8;
			break;
		case '9':
			output += 9;
			break;
		default:
			assert(false);
			break;
		}
	}
	return output;
}

int32_t getIndexFirstNonSpace(const std::string &s, char indentCharacter)
{
	int32_t i = 0;
	for (; i < (int32_t)s.length(); i++) {
		if (s[i] != indentCharacter) {
			return i;
		}
	}
	return i;
}

std::vector<std::string> splitByChar(const std::string &text, char c)
{
	std::vector<std::string> clip;
	std::string line = "";
	for (uint32_t i = 0; i < text.length(); i++) {
		if (text[i] == c) {
			clip.push_back(line);
			line = "";
		} else {
			line += text[i];
		}
	}
	clip.push_back(line);
	return clip;
}

std::string replace(std::string str, const std::string &from, const std::string &to)
{
	size_t pos = 0;
	if (from.length() > 0) {
		while ((pos = str.find(from, pos)) != std::string::npos) {
			str.replace(pos, from.length(), to);
			pos += to.length();
		}
	}
	return str;
}

bool isWhitespace(char c)
{
	return c == ' ' || c == '\t';
}

bool isAlphanumeric(char c)
{
	return std::isalnum(c) || c == '_' ? 1 : 0;
}

std::string safeSubstring(const std::string &str, std::size_t pos, std::size_t len)
{
	if (pos >= str.size()) {
		return "";
	}
	return str.substr(pos, len);
}

std::string safeSubstring(const std::string &str, std::size_t pos)
{
	if (pos >= str.size()) {
		return "";
	}
	return str.substr(pos);
}

bool isNumber(const std::string &s)
{
	for (uint32_t i = 0; i < s.length(); i++) {
		if (s[i] < '0' || '9' < s[i]) {
			return false;
		}
	}
	return true;
}
