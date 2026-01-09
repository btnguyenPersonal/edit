#include "indent.h"
#include "comment.h"
#include "string.h"
#include "fileops.h"
#include "display.h"
#include "textedit.h"
#include <string>
#include <vector>

uint32_t getIndent(State *state, const std::string &str)
{
	for (uint32_t i = 0; i < str.length(); i++) {
		if (str[i] != getIndentCharacter(state)) {
			return i;
		}
	}
	return 0;
}

uint32_t getIndentSize(State *state)
{
	if (state->options.indent_style == "tab") {
		return 1;
	}
	return state->options.indent_size;
}

char getIndentCharacter(State *state)
{
	if (state->options.indent_style == "tab") {
		return '\t';
	} else if (state->options.indent_style == "space") {
		return ' ';
	}
	return 'E';
}

void indent(State *state)
{
	for (uint32_t i = 0; i < getIndentSize(state); i++) {
		state->file->data[state->file->row] = getIndentCharacter(state) + state->file->data[state->file->row];
	}
}

void deindent(State *state)
{
	for (uint32_t i = 0; i < getIndentSize(state); i++) {
		if (state->file->data[state->file->row].substr(0, 1) == std::string("") + getIndentCharacter(state)) {
			state->file->data[state->file->row] = state->file->data[state->file->row].substr(1);
		}
	}
}

void indentLineWhenTypingLastChar(State *state)
{
	if (!autoIndentDisabledFileType(state->file->filename)) {
		if (state->file->col + 1 == state->file->data[state->file->row].length()) {
			indentLine(state);
			state->file->col = getLastCharIndex(state);
		}
	}
}

void indentLineWhenTypingFirstChar(State *state)
{
	if (!autoIndentDisabledFileType(state->file->filename)) {
		if ((int32_t)state->file->col == getIndexFirstNonSpace(state->file->data[state->file->row], getIndentCharacter(state))) {
			indentLine(state);
			state->file->col = getIndexFirstNonSpace(state->file->data[state->file->row], getIndentCharacter(state));
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
		if (state->file->data[i] != "") {
			return state->file->data[i];
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
		if (trimmed.length() > 0) {
			return trimmed.front() == '<' || trimmed.back() == '>' || trimmed.front() == '>' || trimmed.back() == '<';
		}
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
	std::string currLine = state->file->data[row];
	ltrim(currLine);
	int32_t indentLevel = getNumLeadingIndentCharacters(state, prevLine);
	uint32_t indentSize = getIndentSize(state);

	if (hasHTML(prevLine, state->extension)) {
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
					if (i > 0 && prevLine[i - 1] == '/') {
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
			if (prevLine.substr(i, state->file->commentSymbol.length()) == state->file->commentSymbol) {
				break;
			} else if (prevLine[i] == '(' || prevLine[i] == '{' || prevLine[i] == '[' || prevLine[i] == ':') {
				if (i + 1 == prevLine.length()) {
					indentLevel += indentSize;
				}
			}
		}
	}

	if (hasHTML(currLine, state->extension)) {
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
			if (currLine.substr(i, state->file->commentSymbol.length()) == state->file->commentSymbol) {
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
	ltrim(state->file->data[row]);
	if (state->file->data[row].length() != 0) {
		int32_t indentLevel = getIndentLevel(state, row);
		for (int32_t i = 0; i < indentLevel; i++) {
			state->file->data[row] = getIndentCharacter(state) + state->file->data[row];
		}
	}
}

void indentLine(State *state)
{
	ltrim(state->file->data[state->file->row]);
	if (state->file->data[state->file->row].length() != 0) {
		int32_t indentLevel = getIndentLevel(state, state->file->row);
		for (int32_t i = 0; i < indentLevel; i++) {
			state->file->data[state->file->row] = getIndentCharacter(state) + state->file->data[state->file->row];
		}
	}
}

void indentRange(State *state)
{
	uint32_t firstNonEmptyRow = state->file->row;
	for (int32_t i = state->file->row; i <= (int32_t)state->visual.row; i++) {
		if (state->file->data[i] != "") {
			firstNonEmptyRow = i;
			break;
		}
	}
	int32_t indentDifference = getIndentLevel(state, state->file->row) - getNumLeadingIndentCharacters(state, state->file->data[firstNonEmptyRow]);
	if (indentDifference > 0) {
		for (int32_t i = state->file->row; i <= (int32_t)state->visual.row; i++) {
			if (state->file->data[i] != "") {
				for (int32_t j = 0; j < indentDifference; j++) {
					state->file->data[i] = getIndentCharacter(state) + state->file->data[i];
				}
			}
		}
	} else if (indentDifference < 0) {
		for (int32_t i = state->file->row; i <= (int32_t)state->visual.row; i++) {
			if (state->file->data[i] != "") {
				for (int32_t j = 0; j < -1 * indentDifference; j++) {
					if (state->file->data[i].length() > 0 && state->file->data[i][0] == getIndentCharacter(state)) {
						state->file->data[i] = safeSubstring(state->file->data[i], 1);
					}
				}
			}
		}
	}
}
