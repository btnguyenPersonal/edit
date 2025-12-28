#include "insertLoggingCode.h"
#include "string.h"
#include "indent.h"
#include "fileops.h"
#include "textedit.h"
#include <regex>
#include <string>
#include <vector>

bool isCExtension(std::string extension)
{
	return extension == "h" || extension == "c" || extension == "cc" || extension == "cpp";
}

std::string getLoggingCode(State *state, std::string variableName)
{
	if (variableName == "") {
		return "";
	}
	std::string rowStr = std::to_string(state->file->row + 1);
	if (isCExtension(getExtension(state->file->filename))) {
		std::string escapedVar = getPrintableString(variableName);
		escapedVar = replace(escapedVar, "%", "%%");
		return "printf(\"" + rowStr + " " + escapedVar + ": %d\\n\", " + variableName + ");";
	} else {
		std::string s = "console.log('" + rowStr + " " + replace(variableName, "'", "\\'") + "'";
		s += ", " + variableName;
		s += ");";
		return s;
	}
}

std::string getLoggingRegex(State *state)
{
	std::string pattern = "";
	if (isCExtension(getExtension(state->file->filename))) {
		pattern = "printf\\(\"[0-9]+ .+?\\);";
	} else {
		pattern = "console\\.log\\('[0-9]+', .+?\\);";
	}
	return pattern;
}

void toggleLoggingCode(State *state, std::string variableName)
{
	std::string loggingCode = getLoggingCode(state, variableName);
	if (loggingCode == "") {
		return;
	}
	if (state->file->row + 1 < state->file->data.size()) {
		auto nextLine = state->file->data[state->file->row + 1];
		ltrim(nextLine);
		if (nextLine == loggingCode) {
			state->file->data.erase(state->file->data.begin() + state->file->row + 1);
			return;
		}
	}
	if (state->file->row < state->file->data.size()) {
		state->file->data.insert(state->file->data.begin() + state->file->row + 1, loggingCode);
		indentLine(state, state->file->row + 1);
	}
}

void removeAllLoggingCode(State *state)
{
	std::string logPattern = getLoggingRegex(state);
	if (logPattern != "") {
		for (int32_t i = state->file->data.size() - 1; i >= 0; i--) {
			if (std::regex_search(state->file->data[i], std::regex(logPattern))) {
				state->file->data.erase(state->file->data.begin() + i);
			}
		}
	}
}

