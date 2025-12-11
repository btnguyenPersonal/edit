#include "indent.h"
#include "comment.h"
#include "helper.h"
#include <string>
#include <vector>

void indentLineWhenTypingLastChar(State *state)
{
	if (!autoIndentDisabledFileType(state->filename)) {
		if (state->col + 1 == state->data[state->row].length()) {
			indentLine(state);
			state->col = getLastCharIndex(state);
		}
	}
}

void indentLineWhenTypingFirstChar(State *state)
{
	if (!autoIndentDisabledFileType(state->filename)) {
		if ((int32_t)state->col == getIndexFirstNonSpace(state)) {
			indentLine(state);
			state->col = getIndexFirstNonSpace(state);
		}
	}
}

bool autoIndentDisabledFileType(std::string filename)
{
	if (getExtension(filename) == "py") {
		return true;
	}
	return false;
}

int32_t getNumLeadingIndentCharacters(State *state, std::string s)
{
	int32_t num = 0;
	for (uint32_t i = 0; i < s.length(); i++) {
		if (s[i] == getIndentCharacter(state)) {
			num++;
		} else {
			break;
		}
	}
	return num;
}

std::string getPrevLine(State *state, uint32_t row)
{
	for (int32_t i = row - 1; i >= 0; i--) {
		if (state->data[i] != "") {
			return state->data[i];
		}
	}
	return "";
}

std::string trim(const std::string &str)
{
	size_t first = str.find_first_not_of(' ');
	if (std::string::npos == first) {
		return str;
	}
	size_t last = str.find_last_not_of(' ');
	return str.substr(first, (last - first + 1));
}

bool hasHTML(std::string line, std::string extension)
{
	if (extension == "js" || extension == "jsx" || extension == "ts" || extension == "tsx" || extension == "html") {
		auto trimmed = trim(line);
		if (trimmed.empty()) {
			return false;
		}
		if (safeSubstring(trimmed, 0, std::string("#include ").length()) == "#include ") {
			return false;
		}
		return trimmed.front() == '<' || trimmed.back() == '>' || trimmed.front() == '>' || trimmed.back() == '<';
	}
	return false;
}

enum TagType {
	EMPTY,
	OPEN,
	CLOSE,
};

int32_t getIndentLevel(State *state, uint32_t row)
{
	std::string prevLine = getPrevLine(state, row);
	prevLine = trimComment(state, prevLine);
	std::string currLine = state->data[row];
	ltrim(currLine);
	int32_t indentLevel = getNumLeadingIndentCharacters(state, prevLine);
	uint32_t indentSize = getIndentSize(state);

	if (hasHTML(prevLine, getExtension(state->filename))) {
		int32_t tagType = EMPTY;
		int32_t tagStack = 0;
		for (uint32_t i = 0; i < prevLine.length(); i++) {
			if (tagType == EMPTY && prevLine[i] == '<') {
				if (i + 1 < prevLine.length() && prevLine[i + 1] == '/') {
					tagType = CLOSE;
					tagStack--;
				} else {
					tagType = OPEN;
					tagStack++;
				}
			} else if (prevLine[i] == '>') {
				if (i > 0 && prevLine[i - 1] == '=') {
					continue;
				} else if (tagType == EMPTY) {
					tagType = CLOSE;
				} else {
					if (i - 1 >= 0 && prevLine[i - 1] == '/') {
						tagStack--;
					}
					tagType = EMPTY;
				}
			}
		}
		if (tagType == OPEN || tagStack > 0) {
			indentLevel += indentSize;
		}
	} else {
		for (uint32_t i = 0; i < prevLine.length(); i++) {
			if (prevLine.substr(i, state->commentSymbol.length()) == state->commentSymbol) {
				break;
			} else if (prevLine[i] == '(' || prevLine[i] == '{' || prevLine[i] == '[' || prevLine[i] == ':') {
				if (i + 1 == prevLine.length()) {
					indentLevel += indentSize;
				}
			}
		}
	}

	if (hasHTML(currLine, getExtension(state->filename))) {
		int32_t tagType = EMPTY;
		int32_t tagStack = 0;
		for (uint32_t i = 0; i < currLine.length(); i++) {
			if (tagType == EMPTY && currLine[i] == '<') {
				if (i + 1 < currLine.length() && currLine[i + 1] == '/') {
					tagType = CLOSE;
					tagStack--;
				} else {
					tagType = OPEN;
					tagStack++;
				}
			} else if (currLine[i] == '>') {
				if (i > 0 && currLine[i - 1] == '=') {
					continue;
				} else if (tagType == EMPTY) {
					tagType = CLOSE;
				} else {
					tagType = EMPTY;
				}
			}
		}
		if (tagType == CLOSE || tagStack < 0) {
			indentLevel -= indentSize;
		}
	} else {
		for (uint32_t i = 0; i < currLine.length(); i++) {
			if (currLine.substr(i, state->commentSymbol.length()) == state->commentSymbol) {
				break;
			} else if (currLine.substr(i, std::string("default:").length()) == "default:") {
				if (i == 0) {
					indentLevel -= indentSize;
				}
			} else if (currLine.substr(i, std::string("case ").length()) == "case ") {
				if (i == 0) {
					indentLevel -= indentSize;
				}
			} else if (currLine[i] == ')' || currLine[i] == '}' || currLine[i] == ']') {
				if (i == 0) {
					indentLevel -= indentSize;
				}
			}
		}
	}

	return indentLevel;
}

void indentLine(State *state, uint32_t row)
{
	ltrim(state->data[row]);
	if (state->data[row].length() != 0) {
		int32_t indentLevel = getIndentLevel(state, row);
		for (int32_t i = 0; i < indentLevel; i++) {
			state->data[row] = getIndentCharacter(state) + state->data[row];
		}
	}
}

void indentLine(State *state)
{
	ltrim(state->data[state->row]);
	if (state->data[state->row].length() != 0) {
		int32_t indentLevel = getIndentLevel(state, state->row);
		for (int32_t i = 0; i < indentLevel; i++) {
			state->data[state->row] = getIndentCharacter(state) + state->data[state->row];
		}
	}
}

void indentRange(State *state)
{
	uint32_t firstNonEmptyRow = state->row;
	for (int32_t i = state->row; i <= (int32_t)state->visual.row; i++) {
		if (state->data[i] != "") {
			firstNonEmptyRow = i;
			break;
		}
	}
	int32_t indentDifference = getIndentLevel(state, state->row) - getNumLeadingIndentCharacters(state, state->data[firstNonEmptyRow]);
	if (indentDifference > 0) {
		for (int32_t i = state->row; i <= (int32_t)state->visual.row; i++) {
			if (state->data[i] != "") {
				for (int32_t j = 0; j < indentDifference; j++) {
					state->data[i] = getIndentCharacter(state) + state->data[i];
				}
			}
		}
	} else if (indentDifference < 0) {
		for (int32_t i = state->row; i <= (int32_t)state->visual.row; i++) {
			if (state->data[i] != "") {
				for (int32_t j = 0; j < -1 * indentDifference; j++) {
					if (state->data[i].length() > 0 && state->data[i][0] == getIndentCharacter(state)) {
						state->data[i] = safeSubstring(state->data[i], 1);
					}
				}
			}
		}
	}
}
