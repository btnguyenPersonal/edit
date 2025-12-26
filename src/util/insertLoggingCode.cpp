#include "insertLoggingCode.h"
#include "helper.h"
#include "indent.h"
#include "fileops.h"
#include <regex>
#include <string>
#include <vector>

void toggleLoggingCode(State *state, std::string variableName, bool showValue)
{
	std::string current = state->file->data[state->file->row];
	std::string loggingCode = getLoggingCode(state, state->file->row, variableName, showValue);
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
	state->file->data.insert(state->file->data.begin() + state->file->row + 1, loggingCode);
	indentLine(state, state->file->row + 1);
}

std::string getLoggingRegex(State *state)
{
	std::string extension = getExtension(state->file->filename);
	std::string pattern = "";
	if (extension == "cc" || extension == "cpp") {
		pattern = "std::cout << \"[0-9]+ .+? << std::endl;";
	} else {
		pattern = "console\\.log\\('[0-9]+', .+?\\);";
	}
	return pattern;
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

std::string getLoggingCode(State *state, uint32_t row, std::string variableName, bool showValue)
{
	std::string extension = getExtension(state->file->filename);
	std::string rowStr = std::to_string(row + 1);
	if (extension == "cc" || extension == "cpp") {
		std::string s = "std::cout << \"" + rowStr + " " + variableName + ": \"";
		if (showValue) {
			s += " << " + variableName;
		}
		s += " << std::endl;";
		return s;
	} else {
		std::string s = "console.log('" + rowStr + "', " + "'" + replace(variableName, "'", "\\'") + "'";
		if (showValue) {
			s += ", " + variableName;
		}
		s += ");";
		return s;
	}
}
