#include "keys.h"
#include "ctrl.h"
#include "helper.h"
#include "comment.h"
#include "state.h"
#include "visualType.h"
#include "prompt.h"
#include "assert.h"
#include "ignore.h"
#include "sanity.h"
#include "movement.h"
#include "search.h"
#include "fileops.h"
#include <string>
#include <fstream>
#include <vector>
#include <regex>

std::string replace(std::string str, const std::string &from, const std::string &to)
{
	size_t pos = 0;
	while ((pos = str.find(from, pos)) != std::string::npos) {
		str.replace(pos, from.length(), to);
		pos += to.length();
	}
	return str;
}

uint32_t buildNumberFromString(std::string s)
{
	uint32_t output = 0;
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
			break;
		}
	}
	return output;
}

std::string runCommand(std::string command)
{
	std::string output = "";
	char temp[4096];
	FILE *fp = popen(command.c_str(), "r");
	if (fp) {
		while (fgets(temp, sizeof(temp), fp)) {
			output += temp;
		}
	}
	return output;
}

std::vector<std::string> splitByChar(std::string text, char c)
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
	if (line != "") {
		clip.push_back(line);
	}
	return clip;
}

std::vector<std::string> getLogLines(State *state)
{
	std::vector<std::string> gitLogLines = { "current" };
	std::vector<std::string> temp = splitByChar(runCommand("git log --oneline | cat 2>/dev/null"), '\n');
	for (uint32_t i = 0; i < temp.size(); i++) {
		gitLogLines.push_back(temp[i]);
	}
	return gitLogLines;
}

std::vector<std::string> getDiffLines(State *state)
{
	std::vector<std::string> gitDiffLines;
	std::string hash = "";
	if (state->logIndex != 0) {
		for (uint32_t i = 0; i < state->logLines[state->logIndex].length(); i++) {
			if (state->logLines[state->logIndex][i] != ' ') {
				hash += state->logLines[state->logIndex][i];
			} else {
				break;
			}
		}
	}
	std::string command = "";
	if (hash == "") {
		command = "git add -N :/ && git diff HEAD | expand -t " + std::to_string(state->options.indent_size) + " 2>/dev/null";
	} else {
		command = "git show " + hash + " | expand -t " + std::to_string(state->options.indent_size) + " 2>/dev/null";
	}
	gitDiffLines = splitByChar(runCommand(command), '\n');
	if (hash == "" && gitDiffLines.size() == 0) {
		return { "No local changes" };
	}
	return gitDiffLines;
}

std::string padTo(std::string str, const uint32_t num, const char paddingChar)
{
	auto tmp = str;
	if (num > str.size()) {
		tmp.insert(0, num - str.size(), paddingChar);
	}
	return tmp;
}

void forwardFileStack(State *state)
{
	if (state->fileStack.size() > 0 && state->fileStackIndex < state->fileStack.size()) {
		if (state->fileStackIndex + 1 < state->fileStack.size()) {
			state->fileStackIndex += 1;
		}
		if (std::filesystem::is_regular_file(state->fileStack[state->fileStackIndex].c_str())) {
			state->changeFile(state->fileStack[state->fileStackIndex]);
			state->showFileStack = true;
		} else {
			state->status = "file not found: " + state->fileStack[state->fileStackIndex];
			state->fileStack.erase(state->fileStack.begin() + state->fileStackIndex);
		}
	}
}

void backwardFileStack(State *state)
{
	if (state->fileStack.size() > 0 && state->fileStackIndex < state->fileStack.size()) {
		if (state->fileStackIndex > 0) {
			state->fileStackIndex -= 1;
		}
		if (std::filesystem::is_regular_file(state->fileStack[state->fileStackIndex].c_str())) {
			state->changeFile(state->fileStack[state->fileStackIndex]);
			state->showFileStack = true;
		} else {
			state->status = "file not found: " + state->fileStack[state->fileStackIndex];
			state->fileStack.erase(state->fileStack.begin() + state->fileStackIndex);
		}
	}
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

void recordHistory(State *state, std::vector<diffLine> diff)
{
	if (state->file->historyPosition < (int32_t)state->file->history.size()) {
		state->file->history.erase(state->file->history.begin() + state->file->historyPosition + 1, state->file->history.end());
	}
	state->file->history.push_back(diff);
	state->file->historyPosition = (int32_t)state->file->history.size() - 1;
	state->diffIndex = state->file->historyPosition;
	assert(state->file->historyPosition >= 0);
}

void recordJumpList(State *state)
{
	if (state->file->jumplist.list.size() > 0) {
		auto pos = state->file->jumplist.list.back();
		if (pos.row != state->file->row || pos.col != state->file->col) {
			state->file->jumplist.list.erase(state->file->jumplist.list.begin() + state->file->jumplist.index + 1, state->file->jumplist.list.end());
			state->file->jumplist.list.push_back({ state->file->row, state->file->col });
			state->file->jumplist.index = state->file->jumplist.list.size() - 1;
		}
	} else {
		state->file->jumplist.list.push_back({ state->file->row, state->file->col });
		state->file->jumplist.index = state->file->jumplist.list.size() - 1;
	}
	state->file->jumplist.touched = false;
}

void recordMacroCommand(State *state, char c)
{
	state->macroCommand[state->recording.c].push_back(getEscapedChar(c));
}

void insertFinalEmptyNewline(State *state)
{
	if (state->file) {
		if (state->file->data.size() > 0 && state->file->data[state->file->data.size() - 1] != "") {
			state->file->data.push_back("");
		}
	}
}

uint32_t getLastCharIndex(State *state)
{
	if (state->file->data[state->file->row].length() != 0) {
		return state->file->data[state->file->row].length() - 1;
	} else {
		return 0;
	}
}

void recordMotion(State *state, int32_t c)
{
	if (c == KEY_BACKSPACE) {
		state->motion.push_back(127);
	} else {
		state->motion.push_back(c);
	}
}

void setDotCommand(State *state, int32_t c)
{
	if (!state->dontRecordKey) {
		state->dotCommand.clear();
		state->dotCommand.push_back(getEscapedChar(c));
	}
}

void setDotCommand(State *state, std::vector<int32_t> s)
{
	if (!state->dontRecordKey) {
		state->dotCommand.clear();
		for (uint32_t i = 0; i < s.size(); i++) {
			state->dotCommand.push_back(getEscapedChar(s[i]));
		}
	}
}

int32_t contains(const std::map<uint32_t, std::string> &myMap, const std::string &s)
{
	for (const auto &pair : myMap) {
		if (pair.second == s) {
			return pair.first;
		}
	}
	return -1;
}

bool eraseHarpoon(State *state)
{
	if (state->harpoon[state->workspace].index < state->harpoon[state->workspace].list.size()) {
		state->harpoon[state->workspace].list.erase(state->harpoon[state->workspace].list.begin() + state->harpoon[state->workspace].index);
		return true;
	}
	return false;
}

void clearHarpoon(State *state)
{
	state->harpoon[state->workspace].index = 0;
	state->harpoon[state->workspace].list.clear();
}

void jumpToPrevHarpoon(State *state)
{
	if (state->harpoon[state->workspace].index > 0) {
		jumpToHarpoon(state, state->harpoon[state->workspace].index - 1);
	} else {
		focusHarpoon(state);
	}
}

void jumpToNextHarpoon(State *state)
{
	if (state->harpoon[state->workspace].index + 1 < state->harpoon[state->workspace].list.size()) {
		jumpToHarpoon(state, state->harpoon[state->workspace].index + 1);
	} else {
		focusHarpoon(state);
	}
}

void focusHarpoon(State *state)
{
	jumpToHarpoon(state, state->harpoon[state->workspace].index);
}

void realignHarpoon(State *state)
{
	for (uint32_t i = 0; i < state->harpoon[state->workspace].list.size(); i++) {
		if (state->harpoon[state->workspace].list[i] == state->file->filename) {
			state->harpoon[state->workspace].index = i;
		}
	}
}

bool containsHarpoon(State *state)
{
	for (uint32_t i = 0; i < state->harpoon[state->workspace].list.size(); i++) {
		if (state->file->filename == state->harpoon[state->workspace].list[i]) {
			return true;
		}
	}
	return false;
}

bool createNewestHarpoon(State *state)
{
	if (containsHarpoon(state)) {
		return false;
	}
	if (state->workspace >= state->harpoon.size()) {
		state->status = "harpoon workspace over limit";
		return false;
	}
	state->harpoon[state->workspace].list.push_back(state->file->filename);
	state->harpoon[state->workspace].index = state->harpoon[state->workspace].list.size() - 1;
	return true;
}

bool jumpToHarpoon(State *state, uint32_t num)
{
	if (num < state->harpoon[state->workspace].list.size()) {
		if (!state->resetState(state->harpoon[state->workspace].list[num])) {
			state->harpoon[state->workspace].list.erase(state->harpoon[state->workspace].list.begin() + num);
			return false;
		}
		state->harpoon[state->workspace].index = num;
		return true;
	}
	return false;
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

void changeToGrepFile(State *state)
{
	if (state->grep.selection < state->grepOutput.size()) {
		std::filesystem::path selectedFile = state->grepOutput[state->grep.selection].path;
		if (state->grepPath != "") {
			selectedFile = state->grepPath / selectedFile;
		}
		int32_t lineNum = state->grepOutput[state->grep.selection].lineNum;
		state->resetState(selectedFile);
		state->file->row = lineNum - 1;
		setSearchResultCurrentLine(state, state->grep.query);
	}
}

bool isFunctionLine(std::string line, std::string s, std::string extension)
{
	std::vector<std::vector<std::string> > functionStrings;
	if (extension == "js" || extension == "jsx" || extension == "ts" || extension == "tsx") {
		functionStrings = {
			{ "enum", " {" }, { "async", "" }, { "class", " {" }, { " ", " (" }, { " ", "(" }, { "const", " " }, { "function", "(" }, { "struct", " {" }, { "interface", " {" },
		};
	} else if (extension == "c" || extension == "cc" || extension == "cpp" || extension == "h" || extension == "hpp") {
		functionStrings = {
			{ "", "(" },
		};
	}
	for (uint32_t i = 0; i < functionStrings.size(); i++) {
		if (line.find(functionStrings[i][0] + " " + s + functionStrings[i][1]) != std::string::npos) {
			return true;
		}
	}
	return functionStrings.size() == 0;
}

void findDefinitionFromGrepOutput(State *state, std::string s)
{
	std::string extension = getExtension(state->file->filename);
	for (uint32_t i = 0; i < state->grepOutput.size(); i++) {
		if (state->grepOutput[i].line.back() == '(' || state->grepOutput[i].line.back() == '{') {
			if (isFunctionLine(state->grepOutput[i].line, s, extension)) {
				state->grep.selection = i;
				changeToGrepFile(state);
			}
		}
	}
}

void refocusFileExplorer(State *state, bool changeMode)
{
	auto normalizedFilename = normalizeFilename(state->file->filename);
	if (state->fileExplorerOpen) {
		state->fileExplorerIndex = state->fileExplorer->expand(normalizedFilename);
		centerFileExplorer(state);
	}
	if (changeMode) {
		state->mode = FILEEXPLORER;
	}
}

void centerFileExplorer(State *state)
{
	if ((state->fileExplorerIndex - ((int32_t)state->maxY / 2)) > 0) {
		if (state->fileExplorer->getTotalChildren() > ((int32_t)state->maxY)) {
			state->fileExplorerWindowLine = (state->fileExplorerIndex - ((int32_t)state->maxY / 2));
		}
	} else {
		state->fileExplorerWindowLine = 0;
	}
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

std::string getCommentSymbol(const std::string &filename)
{
	std::string extension = getExtension(filename);
	if (extension == "js" || extension == "jsx" || extension == "ts" || extension == "tsx" || extension == "cc" || extension == "cpp" || extension == "hpp" || extension == "c" || extension == "h" || extension == "java" || extension == "cs" || extension == "go" || extension == "php" || extension == "rs" || extension == "css" || extension == "scss" || extension == "vb" || extension == "lua") {
		return "//";
	} else if (extension == "py" || extension == "sh" || extension == "bash" || extension == "rb" || extension == "pl" || extension == "pm" || extension == "r" || extension == "yaml" || extension == "yml" || extension == "bashrc" || extension == "zshrc" || extension == "Makefile" || extension == "md" || extension == "gitignore" || extension == "env") {
		return "#";
	} else if (extension == "html" || extension == "xml" || extension == "xhtml" || extension == "svg") {
		return "<!--";
	} else if (extension == "sql") {
		return "--";
	} else if (extension == "lua") {
		return "--";
	} else if (extension == "json") {
		return "//";
	} else {
		return "#";
	}
}

bool isAlphanumeric(char c)
{
	return std::isalnum(c) || c == '_' ? 1 : 0;
}

std::string getGitHash(State *state)
{
	std::string command = std::string("git blame -l -L ") + std::to_string(state->file->row + 1) + ",+1 " + state->file->filename + " | awk '{print $1}'";
	std::string line = runCommand(command);
	rtrim(line);
	if (safeSubstring(line, 0, 1) == "^") {
		return safeSubstring(line, 1);
	}
	return line;
}

std::vector<std::string> getGitBlame(const std::string &filename)
{
	std::vector<std::string> blameLines;
	std::string command = "git --no-pager blame ./" + filename + " --date=short 2>/dev/null | awk '{print $1, $2, $3, $4, \")\"}'";
	blameLines = splitByChar(runCommand(command), '\n');
	blameLines.push_back("");
	return blameLines;
}

uint32_t getLineNumberOffset(State *state)
{
	uint32_t offset = state->lineNumSize + 1;
	if (state->mode == BLAME) {
		offset += state->blameSize;
	}
	if (state->fileExplorerOpen) {
		offset += state->fileExplorerSize;
	}
	return offset;
}

void moveHarpoonRight(State *state)
{
	if (state->harpoon[state->workspace].index + 1 < state->harpoon[state->workspace].list.size()) {
		auto temp = state->harpoon[state->workspace].list[state->harpoon[state->workspace].index];
		state->harpoon[state->workspace].list[state->harpoon[state->workspace].index] = state->harpoon[state->workspace].list[state->harpoon[state->workspace].index + 1];
		state->harpoon[state->workspace].list[state->harpoon[state->workspace].index + 1] = temp;
		state->harpoon[state->workspace].index += 1;
	}
}

void moveHarpoonLeft(State *state)
{
	if (state->harpoon[state->workspace].index > 0 && state->harpoon[state->workspace].index < state->harpoon[state->workspace].list.size()) {
		auto temp = state->harpoon[state->workspace].list[state->harpoon[state->workspace].index];
		state->harpoon[state->workspace].list[state->harpoon[state->workspace].index] = state->harpoon[state->workspace].list[state->harpoon[state->workspace].index - 1];
		state->harpoon[state->workspace].list[state->harpoon[state->workspace].index - 1] = temp;
		state->harpoon[state->workspace].index -= 1;
	}
}

void trimTrailingWhitespace(State *state)
{
	for (uint32_t i = 0; i < state->file->data.size(); i++) {
		rtrim(state->file->data[i]);
	}
}

bool isWhitespace(char c)
{
	return std::isspace(c) || c == '\t';
}

void rtrim(std::string &s)
{
	s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !isWhitespace(ch); }).base(), s.end());
}

void ltrim(std::string &s)
{
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) { return !isWhitespace(ch); }));
}

bool setSearchResultCurrentLine(State *state, const std::string &query)
{
	std::string line = state->file->data[state->file->row];
	size_t index = line.find(query);
	if (index != std::string::npos) {
		state->file->col = static_cast<uint32_t>(index);
		return true;
	}
	return false;
}

void setPosition(State *state, Position pos)
{
	state->file->row = pos.row;
	state->file->col = pos.col;
}

bool isNumber(std::string s)
{
	for (uint32_t i = 0; i < s.length(); i++) {
		if (i > '0' && i < '9') {
			return false;
		}
	}
	return true;
}

uint32_t getIndent(State *state, const std::string &str)
{
	for (uint32_t i = 0; i < str.length(); i++) {
		if (str[i] != getIndentCharacter(state)) {
			return i;
		}
	}
	return 0;
}

void resetValidCursorState(State *state)
{
	if (state->file->data[state->file->row].length() <= state->file->col) {
		if (state->file->data[state->file->row].length() != 0) {
			state->file->col = state->file->data[state->file->row].length() - 1;
		} else {
			state->file->col = 0;
		}
	}
}

bool isColTooSmall(State *state)
{
	uint32_t col = getDisplayCol(state);
	return col < state->file->windowPosition.col;
}

bool isColTooBig(State *state)
{
	uint32_t col = getDisplayCol(state);
	return (int32_t)col - (int32_t)state->file->windowPosition.col > (int32_t)state->maxX - (int32_t)getLineNumberOffset(state) - 1;
}

bool isWindowPositionInvalid(State *state)
{
	if (isOffScreenVertical(state)) {
		return true;
	} else if (!state->options.wordwrap && (isColTooSmall(state) || isColTooBig(state))) {
		return true;
	}
	return false;
}

bool isOffScreenVertical(State *state)
{
	if (state->file->row < state->file->windowPosition.row) {
		return true;
	}
	uint32_t windowRow = state->file->windowPosition.row;
	uint32_t rowsBelow = 0;
	while (windowRow < state->file->data.size() && rowsBelow + 2 < state->maxY) {
		if (state->file->row == windowRow && getDisplayRows(state, state->file->row) <= state->maxY) {
			return false;
		}
		rowsBelow += getDisplayRows(state, windowRow);
		windowRow++;
	}
	return true;
}

uint32_t getDisplayCol(State *state)
{
	if (state->options.indent_style == "tab") {
		uint32_t sum = 0;
		for (uint32_t i = 0; i < state->file->col; i++) {
			if (state->file->data[state->file->row][i] == '\t') {
				sum += state->options.indent_size;
			} else {
				sum++;
			}
		}
		return sum;
	} else {
		return state->file->col;
	}
}

uint32_t getCenteredWindowPosition(State *state)
{
	uint32_t windowRow = state->file->row;
	uint32_t rowsAbove = getDisplayRows(state, state->file->row, state->file->col);
	while (windowRow > 0 && rowsAbove < state->maxY / 2) {
		if (rowsAbove + getDisplayRows(state, windowRow - 1) >= state->maxY / 2) {
			break;
		}
		windowRow--;
		rowsAbove += getDisplayRows(state, windowRow);
	}
	return windowRow;
}

uint32_t getDisplayLength(State *state, std::string s)
{
	if (state->options.indent_style == "tab") {
		uint32_t output = 0;
		for (uint32_t i = 0; i < s.length(); i++) {
			if (s[i] == '\t') {
				output += state->options.indent_size;
			} else {
				output++;
			}
		}
		return output;
	} else {
		return s.length();
	}
}

uint32_t getDisplayRows(State *state, uint32_t r, uint32_t c)
{
	if (!state->options.wordwrap) {
		return 1;
	}
	auto physicalCol = state->file->data[r].length() < c ? state->file->data[r].length() : c;
	return 1 + physicalCol / (state->maxX - getLineNumberOffset(state));
}

uint32_t getDisplayRows(State *state, uint32_t r)
{
	if (!state->options.wordwrap) {
		return 1;
	}
	return 1 + getDisplayLength(state, state->file->data[r]) / (state->maxX - getLineNumberOffset(state));
}

uint32_t getNormalizedCol(State *state, uint32_t hardCol)
{
	if (state->options.indent_style != "tab") {
		return state->file->col;
	}
	int32_t tmpHardCol = hardCol;
	int32_t output = 0;
	for (uint32_t i = 0; i < state->file->data[state->file->row].length() && (int32_t)i < tmpHardCol; i++) {
		if (i < state->file->data[state->file->row].length() && state->file->data[state->file->row][i] == '\t') {
			tmpHardCol -= (state->options.indent_size - 1);
			if ((int32_t)i < tmpHardCol) {
				output++;
			} else {
				return output;
			}
		} else {
			output++;
		}
	}
	return output;
}

uint32_t isLargeFile(State *state)
{
	return state->file && state->file->data.size() > 50000;
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

int32_t getIndexFirstNonSpace(State *state)
{
	int32_t i;
	for (i = 0; i < (int32_t)state->file->data[state->file->row].length(); i++) {
		if (state->file->data[state->file->row][i] != getIndentCharacter(state)) {
			return i;
		}
	}
	return i;
}

void calcWindowBounds()
{
	int32_t y, x;
	getmaxyx(stdscr, y, x);
	if (y == -1) {
		y = 30;
	}
	if (x == -1) {
		x = 100;
	}
	State::setMaxYX(y, x);
}

void insertEmptyLineBelow(State *state)
{
	state->file->data.insert(state->file->data.begin() + state->file->row + 1, "");
}

void insertEmptyLine(State *state)
{
	state->file->data.insert(state->file->data.begin() + state->file->row, "");
}

int32_t maximum(int32_t a, int32_t b)
{
	if (a > b) {
		return a;
	} else {
		return b;
	}
}

int32_t minimum(int32_t a, int32_t b)
{
	if (a <= b) {
		return a;
	} else {
		return b;
	}
}
