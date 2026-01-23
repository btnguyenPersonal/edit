#include "insertLoggingCode.h"
#include "string.h"
#include "indent.h"
#include "fileops.h"
#include "textedit.h"
#include "textbuffer.h"
#include <regex>
#include <string>
#include <vector>

bool isCExtension(std::string extension)
{
	return extension == "h" || extension == "c" || extension == "cc" || extension == "cpp";
}

std::string getLoggingCode(State *state, std::string variableName)
{
	std::string rowStr = std::to_string(state->file->row + 1);
	if (isCExtension(state->extension)) {
		std::string ret = "printf(\"#### ";
		if (variableName != "") {
			ret += rowStr;
			std::string escapedVar = getPrintableString(variableName);
			escapedVar = replace(escapedVar, "%", "%%");
			ret += " " + escapedVar;
			ret += ": %d";
		}
		ret += "\\n\"";
		if (variableName != "") {
			ret += ", " + variableName;
		}
		ret += ");";
		return ret;
	} else {
		std::string s = "console.log('#### ";
		if (variableName != "") {
			s += rowStr + " " + replace(variableName, "'", "\\'");
		}
		s += "'";
		if (variableName != "") {
			s += ", " + variableName;
		}
		s += ");";
		return s;
	}
}

std::string getLoggingSearch(State *state)
{
	std::string pattern = "";
	if (isCExtension(state->extension)) {
		pattern = "printf(\"#### ";
	} else {
		pattern = "console.log('#### ";
	}
	return pattern;
}

std::string getLoggingRegex(State *state)
{
	std::string pattern = "";
	if (isCExtension(state->extension)) {
		pattern = "printf\\(\"#### .+?\\);";
	} else {
		pattern = "console\\.log\\('#### .+?\\);";
	}
	return pattern;
}

void insertLoggingCode(State *state, std::string loggingCode)
{
	if (state->file->row < textbuffer_getLineCount(state)) {
		textbuffer_insertLine(state, state->file->row + 1, loggingCode);
		indentLine(state, state->file->row + 1);
	}
}

void toggleLoggingCode(State *state, std::string variableName)
{
	std::string loggingCode = getLoggingCode(state, variableName);
	if (loggingCode == "") {
		return;
	}
	if (state->file->row + 1 < textbuffer_getLineCount(state)) {
		auto nextLine = textbuffer_getLine(state, state->file->row + 1);
		if (nextLine == loggingCode) {
			textbuffer_deleteLine(state, state->file->row + 1);
		}
	}
}

void removeAllLoggingCode(State *state)
{
	std::string logPattern = getLoggingRegex(state);
	if (logPattern != "") {
		for (int32_t i = textbuffer_getLineCount(state) - 1; i >= 0; i--) {
			std::string line = textbuffer_getLine(state, i);
			if (std::regex_search(line, std::regex(logPattern))) {
				textbuffer_deleteLine(state, i);
			}
		}
	}
}
