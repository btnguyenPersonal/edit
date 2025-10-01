#include "keys.h"
#include "helper.h"
#include "comment.h"
#include "state.h"
#include "visualType.h"
#include "name.h"
#include "expect.h"
#include <algorithm>
#include <climits>
#include <cstdio>
#include <fstream>
#include <future>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <ncurses.h>
#include <stdexcept>
#include <string>
#include <vector>
#include <regex>

std::vector<std::string> getLogLines(State* state)
{
	std::vector<std::string> gitLogLines = {"current"};
	try {
		std::string command = "git log --oneline | cat 2>/dev/null";
		std::unique_ptr<FILE, int (*)(FILE *)> pipe(popen(command.c_str(), "r"), pclose);
		if (!pipe) {
			throw std::runtime_error("popen() failed!");
		}
		std::stringstream ss;
		char *line = nullptr;
		size_t len = 0;
		ssize_t read;
		while ((read = getline(&line, &len, pipe.get())) != -1) {
			ss << line;
		}
		free(line);
		std::string str;
		while (std::getline(ss, str)) {
			if (!str.empty()) {
				gitLogLines.push_back(str);
			}
		}
	} catch (const std::exception &e) {
	}
	return gitLogLines;
}

std::vector<std::string> getDiffLines(State* state)
{
	std::vector<std::string> gitDiffLines;
	std::string hash = "";
	try {
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
		std::unique_ptr<FILE, int (*)(FILE *)> pipe(popen(command.c_str(), "r"), pclose);
		if (!pipe) {
			throw std::runtime_error("popen() failed!");
		}
		std::stringstream ss;
		char *line = nullptr;
		size_t len = 0;
		ssize_t read;
		while ((read = getline(&line, &len, pipe.get())) != -1) {
			ss << line;
		}
		free(line);
		std::string str;
		while (std::getline(ss, str)) {
			if (!str.empty()) {
				gitDiffLines.push_back(str);
			}
		}
	} catch (const std::exception &e) {
	}
	if (hash == "" && gitDiffLines.size() == 0) {
		return {"No local changes"};
	}
	return gitDiffLines;
}

std::string getMode(uint32_t mode)
{
	switch (mode) {
	case 0:
		return "VISUAL";
	case 1:
		return "COMMANDLINE";
	case 2:
		return "TYPING";
	case 3:
		return "SHORTCUTS";
	case 4:
		return "FINDFILE";
	case 5:
		return "GREP";
	case 6:
		return "BLAME";
	case 7:
		return "MULTICURSOR";
	case 8:
		return "SEARCH";
	case 9:
		return "FILEEXPLORER";
	case 10:
		return "NAMING";
	default:
		return "";
	}
}

std::string minimize_filename(const std::string &filename)
{
	std::vector<std::string> parts;
	std::stringstream ss(filename);
	std::string part;
	std::string minimized;
	while (std::getline(ss, part, '/')) {
		parts.push_back(part);
	}
	for (size_t i = 0; i < parts.size() - 1; ++i) {
		if (!parts[i].empty()) {
			minimized += parts[i][0];
			minimized += '/';
		}
	}
	minimized += parts.back();
	return minimized;
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
	if (state->fileStack.size() > 0) {
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
	if (state->fileStack.size() > 0) {
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
	if (c < state->data[r].length()) {
		char tmp = state->data[r][c];
		if (isupper(tmp)) {
			state->data[r][c] = std::tolower(tmp);
		} else if (islower(tmp)) {
			state->data[r][c] = std::toupper(tmp);
		}
	}
}

void focusHarpoon(State *state)
{
	for (uint32_t i = 0; i < 10; i++) {
		if (state->harpoonFiles.count(i) > 0 && state->harpoonFiles[i] == state->filename) {
			state->harpoonIndex = i;
		}
	}
}

void recordHistory(State *state, std::vector<diffLine> diff)
{
	if (state->historyPosition < (int32_t)state->history.size()) {
		state->history.erase(state->history.begin() + state->historyPosition + 1, state->history.end());
	}
	state->history.push_back(diff);
	state->historyPosition = (int32_t)state->history.size() - 1;
	state->diffIndex = state->historyPosition;
	expect(state->historyPosition >= 0);
}

void recordJumpList(State *state)
{
	if (state->jumplist.list.size() > 0) {
		auto pos = state->jumplist.list.back();
		if (pos.row != state->row || pos.col != state->col) {
			state->jumplist.list.erase(state->jumplist.list.begin() + state->jumplist.index + 1,
						   state->jumplist.list.end());
			state->jumplist.list.push_back({ state->row, state->col });
			state->jumplist.index = state->jumplist.list.size() - 1;
		}
	} else {
		state->jumplist.list.push_back({ state->row, state->col });
		state->jumplist.index = state->jumplist.list.size() - 1;
	}
}

void recordMacroCommand(State *state, char c)
{
	state->macroCommand.push_back(getEscapedChar(c));
}

void insertFinalEmptyNewline(State *state)
{
	if (state->data.size() > 0 && state->data[state->data.size() - 1] != "") {
		state->data.push_back("");
	}
}

std::string replaceAll(std::string str, const std::string &from, const std::string &to)
{
	size_t pos = 0;
	while ((pos = str.find(from, pos)) != std::string::npos) {
		str.replace(pos, from.length(), to);
		pos += to.length();
	}
	return str;
}

bool isLineFileRegex(const std::string &line)
{
	return line.front() == '[' && line.back() == ']';
}

bool matchesEditorConfigGlob(const std::string &pattern, const std::string &filepath)
{
	auto cleanPattern = safeSubstring(pattern, 1, pattern.length() - 2);
	cleanPattern = replaceAll(cleanPattern, ".", "\\.");
	cleanPattern = replaceAll(cleanPattern, "*", ".*");
	cleanPattern = replaceAll(cleanPattern, "{", "(");
	cleanPattern = replaceAll(cleanPattern, "}", ")");
	cleanPattern = replaceAll(cleanPattern, ",", "|");

	try {
		return std::regex_search(filepath, std::regex(cleanPattern));
	} catch (const std::exception &e) {
		return false;
	}
}

std::string getRelativeToLastAndRoute(State *state)
{
	if (state->fileStackIndex == 0) {
		return "";
	}
	std::string lastFile = state->fileStack[state->fileStackIndex - 1];
	std::filesystem::path lastDir = std::filesystem::path(std::string("./") + lastFile).parent_path();
	std::filesystem::path currentDir = std::filesystem::path(std::string("./") + state->filename).parent_path();
	auto relativePath = std::filesystem::relative(state->filename, lastDir).string();
	if (std::filesystem::is_regular_file(lastFile.c_str())) {
		state->changeFile(lastFile);
	}
	if (safeSubstring(relativePath, 0, 3) != "../") {
		relativePath = "./" + relativePath;
	}
	return relativePath;
}

std::string getRelativeToCurrent(State *state, std::string p)
{
	std::filesystem::path currentDir = std::filesystem::path(std::string("./") + state->filename).parent_path();
	auto relativePath = std::filesystem::relative(p, currentDir).string();
	if (safeSubstring(relativePath, 0, 3) != "../") {
		relativePath = "./" + relativePath;
	}
	return relativePath;
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
	state->dotCommand.clear();
	state->dotCommand.push_back(getEscapedChar(c));
}

void setDotCommand(State *state, std::vector<int32_t> s)
{
	state->dotCommand.clear();
	for (uint32_t i = 0; i < s.size(); i++) {
		state->dotCommand.push_back(getEscapedChar(s[i]));
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

void jumpToPrevHarpoon(State *state)
{
	for (int32_t i = state->harpoonIndex - 1; i >= 0; i--) {
		if (jumpToHarpoon(state, i + 1)) {
			return;
		}
	}
	jumpToHarpoon(state, 1);
}

void jumpToNextHarpoon(State *state)
{
	for (int32_t i = state->harpoonIndex + 1; i < state->options.maxHarpoon; i++) {
		if (jumpToHarpoon(state, i + 1)) {
			return;
		}
	}
	jumpToHarpoon(state, state->options.maxHarpoon);
}

bool createNewestHarpoon(State *state)
{
	for (int32_t num = state->harpoonIndex + 1; num <= state->options.maxHarpoon; num++) {
		int32_t index = contains(state->harpoonFiles, state->filename);
		if (index == -1 && state->harpoonFiles.count(num - 1) == 0) {
			state->harpoonIndex = num - 1;
			state->harpoonFiles[state->harpoonIndex] = state->filename;
			state->prevKeys = "";
			return true;
		}
	}
	return false;
}

bool jumpToHarpoon(State *state, uint32_t num)
{
	if (num > 0) {
		if (state->prevKeys == "g") {
			int32_t index = contains(state->harpoonFiles, state->filename);
			std::string temp = "";
			if (index != -1) {
				state->harpoonFiles.erase(index);
				for (uint32_t i = 1; 0 < state->harpoonPageSize; i++) {
					if (state->harpoonFiles.count(num + i - 1) == 0) {
						state->harpoonFiles[num + i - 1] = state->filename;
						state->harpoonIndex = num + i - 1;
						break;
					}
				}
			} else {
				state->harpoonFiles[num - 1] = state->filename;
				state->harpoonIndex = num - 1;
			}
			state->prevKeys = "";
		} else if (state->harpoonFiles.count(num - 1) > 0) {
			if (!state->resetState(state->harpoonFiles[num - 1])) {
				state->harpoonFiles.erase(num - 1);
				return false;
			}
			state->harpoonIndex = num - 1;
			return true;
		}
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

bool isTestFile(const std::string &filepath)
{
	return filepath.find(".spec") != std::string::npos || filepath.find(".test") != std::string::npos ||
	       filepath.find(".cy") != std::string::npos;
}

void rename(State *state, const std::filesystem::path &oldPath, const std::string &newName)
{
	if (!std::filesystem::exists(oldPath)) {
		state->status = "path does not exist";
		return;
	}

	std::filesystem::path newPath =
		std::filesystem::relative(oldPath.parent_path() / newName, std::filesystem::current_path());

	try {
		std::filesystem::rename(oldPath, newPath);
	} catch (const std::filesystem::filesystem_error &e) {
		state->status = std::string("Failed to rename: ") + std::string(e.what());
	}

	auto relativePath = std::filesystem::relative(oldPath, std::filesystem::current_path()).string();
	if (state->filename == relativePath) {
		state->filename = newPath.string();
	}
}

std::filesystem::path getUniqueFilePath(const std::filesystem::path &basePath)
{
	if (!std::filesystem::exists(basePath)) {
		return basePath;
	}

	std::filesystem::path stem = basePath.stem();
	std::filesystem::path extension = basePath.extension();
	std::filesystem::path directory = basePath.parent_path();

	for (int32_t i = 1;; ++i) {
		std::filesystem::path newPath =
			directory / (stem.string() + " (" + std::to_string(i) + ")" + extension.string());
		if (!std::filesystem::exists(newPath)) {
			return newPath;
		}
	}
}

void createFolder(State *state, std::filesystem::path path, std::string name)
{
	std::filesystem::path fullPath = path / name;
	std::filesystem::create_directories(fullPath);
}

void createFile(State *state, std::filesystem::path path, std::string name)
{
	std::filesystem::path fullPath = path / name;
	auto uniquePath = getUniqueFilePath(fullPath);
	std::ofstream file(fullPath);
	if (!file) {
		state->status = "failed to create file";
	}
	file.close();
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
		state->row = lineNum - 1;
		setSearchResultCurrentLine(state, state->grep.query);
	}
}

bool isFunctionLine(std::string line, std::string s, std::string extension)
{
	std::vector<std::vector<std::string> > functionStrings;
	if (extension == "js" || extension == "jsx" || extension == "ts" || extension == "tsx") {
		functionStrings = {
			{ "enum", " {" }, { "async", "" },     { "class", " {" },  { " ", " (" },	  { " ", "(" },
			{ "const", " " }, { "function", "(" }, { "struct", " {" }, { "interface", " {" },
		};
	} else if (extension == "c" || extension == "cpp" || extension == "h" || extension == "hpp") {
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
	std::string extension = getExtension(state->filename);
	for (uint32_t i = 0; i < state->grepOutput.size(); i++) {
		if (state->grepOutput[i].line.back() == '(' || state->grepOutput[i].line.back() == '{') {
			if (isFunctionLine(state->grepOutput[i].line, s, extension)) {
				state->grep.selection = i;
				changeToGrepFile(state);
			}
		}
	}
}

std::string normalizeFilename(std::string filename)
{
	std::string current_path = std::filesystem::current_path().string() + "/";
	if (filename.substr(0, current_path.length()) == current_path) {
		return filename.substr(current_path.length());
	} else {
		return filename;
	}
}

void refocusFileExplorer(State *state, bool changeMode)
{
	auto normalizedFilename = normalizeFilename(state->filename);
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

bool isOpenParen(char c)
{
	return c == '(' || c == '{' || c == '[';
}

bool isCloseParen(char c)
{
	return c == ')' || c == '}' || c == ']';
}

char getCorrespondingParen(char c)
{
	if (c == '(') {
		return ')';
	} else if (c == '{') {
		return '}';
	} else if (c == '[') {
		return ']';
	} else if (c == ')') {
		return '(';
	} else if (c == '}') {
		return '{';
	} else if (c == ']') {
		return '[';
	}
	return ')';
}

Position matchIt(State *state)
{
	char firstParen = state->data[state->row][state->col];
	int32_t stack = 0;
	if (isOpenParen(firstParen)) {
		char secondParen = getCorrespondingParen(firstParen);
		uint32_t col = state->col;
		for (uint32_t row = state->row; row < state->data.size(); row++) {
			while (col < state->data[row].size()) {
				if (state->data[row][col] == secondParen) {
					if (stack == 1) {
						return { row, col };
					} else {
						stack--;
					}
				} else if (state->data[row][col] == firstParen) {
					stack++;
				}
				col++;
			}
			col = 0;
		}
	} else if (isCloseParen(firstParen)) {
		char secondParen = getCorrespondingParen(firstParen);
		int32_t col = (int32_t)state->col;
		bool first = true;
		for (int32_t row = (int32_t)state->row; row >= 0; row--) {
			if (!first) {
				col = state->data[row].length() > 0 ? state->data[row].length() - 1 : 0;
			}
			while (col >= 0) {
				if (state->data[row][col] == secondParen) {
					if (stack == 1) {
						return { (uint32_t)row, (uint32_t)col };
					} else {
						stack--;
					}
				} else if (state->data[row][col] == firstParen) {
					stack++;
				}
				col--;
			}
			first = false;
		}
	}
	return { state->row, state->col };
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
	if (std::isdigit(state->data[row][col])) {
		number += state->data[row][col];
		for (int32_t i = (int32_t)col - 1; i >= 0; i--) {
			if (std::isdigit(state->data[row][i])) {
				number = state->data[row][i] + number;
				startPos = i;
			} else {
				break;
			}
		}
		for (uint32_t i = col + 1; i < state->data[row].length(); i++) {
			if (std::isdigit(state->data[row][i])) {
				number += state->data[row][i];
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
			state->data[row] = state->data[row].substr(0, startPos) + std::to_string(temp) +
					   safeSubstring(state->data[row], startPos + number.length());
		} catch (const std::exception &e) {
			state->status = "number too large";
		}
	}
}

std::string getCommentSymbol(const std::string &filename)
{
	std::string extension = getExtension(filename);
	if (extension == "js" || extension == "jsx" || extension == "ts" || extension == "tsx" || extension == "cpp" ||
	    extension == "hpp" || extension == "c" || extension == "h" || extension == "java" || extension == "cs" ||
	    extension == "go" || extension == "php" || extension == "rs" || extension == "css" || extension == "scss" ||
	    extension == "vb" || extension == "lua") {
		return "//";
	} else if (extension == "py" || extension == "sh" || extension == "bash" || extension == "rb" ||
		   extension == "pl" || extension == "pm" || extension == "r" || extension == "yaml" ||
		   extension == "yml" || extension == "bashrc" || extension == "zshrc" || extension == "Makefile" ||
		   extension == "md" || extension == "gitignore" || extension == "env") {
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

uint32_t findNextChar(State *state, char c)
{
	for (uint32_t i = state->col; i < state->data[state->row].length(); i++) {
		if (state->data[state->row][i] == c) {
			return i;
		}
	}
	return state->col;
}

uint32_t toNextChar(State *state, char c)
{
	uint32_t index = state->col;
	for (uint32_t i = state->col; i < state->data[state->row].length(); i++) {
		if (state->data[state->row][i] == c) {
			return index;
		} else {
			index = i;
		}
	}
	return state->col;
}

std::string getGitHash(State *state)
{
	std::stringstream command;
	command << "git blame -l -L " << state->row + 1 << ",+1 " << state->filename << " | awk '{print $1}'";
	std::unique_ptr<FILE, int (*)(FILE *)> pipe(popen(command.str().c_str(), "r"), pclose);
	if (!pipe) {
		state->status = "popen() failed!";
		return "";
	}
	std::string output, line;
	char buffer[128];
	while (fgets(buffer, sizeof(buffer), pipe.get()) != NULL) {
		output += buffer;
	}
	std::stringstream ss(output);
	std::getline(ss, line);
	if (safeSubstring(line, 0, 1) == "^") {
		return safeSubstring(line, 1);
	}
	return line;
}

std::vector<std::string> getGitBlame(const std::string &filename)
{
	std::vector<std::string> blameLines;
	try {
		std::string command = "git --no-pager blame ./" + filename +
				      " --date=short 2>/dev/null | awk '{print $1, $2, $3, $4, \")\"}'";
		std::unique_ptr<FILE, int (*)(FILE *)> pipe(popen(command.c_str(), "r"), pclose);
		if (!pipe) {
			throw std::runtime_error("popen() failed!");
		}
		std::stringstream ss;
		char *line = nullptr;
		size_t len = 0;
		ssize_t read;
		while ((read = getline(&line, &len, pipe.get())) != -1) {
			ss << line;
		}
		free(line);
		std::string str;
		while (std::getline(ss, str)) {
			if (!str.empty()) {
				blameLines.push_back(str);
			}
		}
	} catch (const std::exception &e) {
	}
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

std::string getExtension(const std::string &filename)
{
	if (filename == "") {
		return "";
	}
	size_t slashPosition = filename.find_last_of("/\\");
	std::string file = (slashPosition != std::string::npos) ? filename.substr(slashPosition + 1) : filename;
	size_t dotPosition = file.find_last_of(".");
	return (dotPosition != std::string::npos && dotPosition != 0) ? file.substr(dotPosition + 1) : file;
}

void moveHarpoonRight(State *state)
{
	if (state->harpoonFiles.count(state->harpoonIndex) > 0) {
		if (state->harpoonIndex + 1 < 9) {
			std::string temp = state->harpoonFiles[state->harpoonIndex];
			state->harpoonFiles.erase(state->harpoonIndex);
			if (state->harpoonFiles.count(state->harpoonIndex + 1) > 0) {
				state->harpoonFiles[state->harpoonIndex] = state->harpoonFiles[state->harpoonIndex + 1];
			}
			state->harpoonFiles[state->harpoonIndex + 1] = temp;
		}
	}
}

void moveHarpoonLeft(State *state)
{
	if (state->harpoonFiles.count(state->harpoonIndex) > 0) {
		if (state->harpoonIndex > 0) {
			std::string temp = state->harpoonFiles[state->harpoonIndex];
			state->harpoonFiles.erase(state->harpoonIndex);
			if (state->harpoonFiles.count(state->harpoonIndex - 1) > 0) {
				state->harpoonFiles[state->harpoonIndex] = state->harpoonFiles[state->harpoonIndex - 1];
			}
			state->harpoonFiles[state->harpoonIndex - 1] = temp;
		}
	}
}

void trimTrailingWhitespace(State *state)
{
	for (uint32_t i = 0; i < state->data.size(); i++) {
		rtrim(state->data[i]);
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

std::string getCurrentWord(State *state)
{
	std::string currentWord = "";
	for (int32_t i = (int32_t)state->col - 1; i >= 0; i--) {
		if (isAlphanumeric(state->data[state->row][i])) {
			currentWord = state->data[state->row][i] + currentWord;
		} else {
			break;
		}
	}
	return currentWord;
}

std::string autocomplete(State *state, const std::string &query)
{
	if (query == "") {
		return "";
	}
	std::map<std::string, int> wordCounts;
	for (std::string line : state->data) {
		line += ' ';
		std::string word = "";
		for (uint32_t i = 0; i < line.length(); i++) {
			if (isAlphanumeric(line[i])) {
				word += line[i];
			} else {
				if (word.substr(0, query.length()) == query) {
					++wordCounts[word];
				}
				word = "";
			}
		}
		if (word.substr(0, query.length()) == query) {
			++wordCounts[word];
		}
	}
	std::string mostCommonWord = "";
	int32_t maxCount = 0;
	for (const auto &pair : wordCounts) {
		if (pair.second > maxCount && pair.first != query) {
			mostCommonWord = pair.first;
			maxCount = pair.second;
		}
	}
	if (!mostCommonWord.empty()) {
		return mostCommonWord.substr(query.length());
	} else {
		return "";
	}
}

void replaceCurrentLine(State *state, const std::string &query, const std::string &replace)
{
	if (query.empty()) {
		return;
	}
	size_t startPos = 0;
	while ((startPos = state->data[state->row].find(query, startPos)) != std::string::npos) {
		state->data[state->row].replace(startPos, query.length(), replace);
		startPos += replace.length();
	}
}

void runCommand(State *state, const std::string &command)
{
	try {
		std::string prompt = std::string("bash -ic '") + (command + " >/dev/null 2>/dev/null") + "'";
		int32_t returnValue = std::system(prompt.c_str());
		if (returnValue != 0) {
			throw std::exception();
		}
	} catch (const std::exception &e) {
		state->status = "command failed";
	}
	state->changeFile(state->filename);
}

void replaceAllGlobally(State *state, const std::string &query, const std::string &replace)
{
	try {
		std::string command;
#ifdef __APPLE__
		command = ("git ls-files | xargs -I {} sed -i '' \"s/" + query + '/' + replace +
			   "/g\" \"{}\" 2>/dev/null");
#elif defined(__linux__)
		command = ("git ls-files | xargs -I {} sed -i'' \"s/" + query + '/' + replace +
			   "/g\" \"{}\" 2>/dev/null");
#else
#error "Platform not supported"
#endif
		int32_t returnValue = std::system(command.c_str());
		if (returnValue != 0) {
			throw std::exception();
		}
		state->changeFile(state->filename);
	} catch (const std::exception &e) {
		state->status = "command failed";
	}
}

void replaceAll(State *state, const std::string &query, const std::string &replace)
{
	for (uint32_t i = 0; i < state->data.size(); i++) {
		if (query.empty()) {
			return;
		}
		size_t startPos = 0;
		while ((startPos = state->data[i].find(query, startPos)) != std::string::npos) {
			state->data[i].replace(startPos, query.length(), replace);
			startPos += replace.length();
		}
	}
}

bool setSearchResultReverse(State *state)
{
	fixColOverMax(state);
	uint32_t initialCol = state->col;
	uint32_t initialRow = state->row;
	uint32_t col = initialCol;
	uint32_t row = initialRow;
	bool isFirst = true;
	do {
		std::string line = isFirst ? state->data[row].substr(0, col) : state->data[row];
		size_t index = line.rfind(state->search.query);
		if (index != std::string::npos) {
			state->row = row;
			state->col = static_cast<uint32_t>(index);
			return true;
		}
		if (row == 0) {
			row = state->data.size() - 1;
		} else {
			row--;
		}
		isFirst = false;
	} while (row != initialRow);
	// try last row again
	std::string line = state->data[row];
	size_t index = line.rfind(state->search.query);
	if (index != std::string::npos) {
		state->row = row;
		state->col = static_cast<uint32_t>(index);
		return true;
	}
	return false;
}

bool searchFromTop(State *state)
{
	for (uint32_t i = 0; i < state->data.size(); i++) {
		size_t index = state->data[i].rfind(state->search.query);
		if (index != std::string::npos) {
			state->row = i;
			state->col = static_cast<uint32_t>(index);
			return true;
		}
	}
	return false;
}

bool setSearchResultCurrentLine(State *state, const std::string &query)
{
	std::string line = state->data[state->row];
	size_t index = line.find(query);
	if (index != std::string::npos) {
		state->col = static_cast<uint32_t>(index);
		return true;
	}
	return false;
}

bool setSearchResult(State *state)
{
	fixColOverMax(state);
	uint32_t initialCol = state->col;
	uint32_t initialRow = state->row;
	uint32_t col = initialCol;
	uint32_t row = initialRow;
	do {
		std::string line = state->data[row].substr(col);
		size_t index = line.find(state->search.query);
		if (index != std::string::npos) {
			state->row = row;
			state->col = static_cast<uint32_t>(index) + col;
			return true;
		}
		row = (row + 1) % state->data.size();
		col = 0;
	} while (row != initialRow);
	std::string line = state->data[row];
	size_t index = line.find(state->search.query);
	if (index != std::string::npos) {
		state->row = row;
		state->col = static_cast<uint32_t>(index) + col;
		return true;
	}
	return false;
}

void setPosition(State *state, Position pos)
{
	state->row = pos.row;
	state->col = pos.col;
}

void initVisual(State *state, VisualType visualType)
{
	state->visualType = visualType;
	state->visual.row = state->row;
	state->visual.col = state->col;
}

bool is_number(const std::string &s)
{
	std::string::const_iterator it = s.begin();
	while (it != s.end() && std::isdigit(*it))
		++it;
	return !s.empty() && it == s.end();
}

char ctrl(char c)
{
	return c - 'a' + 1;
}

char unctrl(char c)
{
	return c + 'a' - 1;
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

uint32_t getPrevEmptyLine(State *state)
{
	bool hitNonEmpty = false;
	for (int32_t i = (int32_t)state->row; i >= 0; i--) {
		if (state->data[i] != "") {
			hitNonEmpty = true;
		} else if (hitNonEmpty && state->data[i] == "") {
			return i;
		}
	}
	return state->row;
}

uint32_t getNextEmptyLine(State *state)
{
	bool hitNonEmpty = false;
	for (uint32_t i = state->row; i < state->data.size(); i++) {
		if (state->data[i] != "") {
			hitNonEmpty = true;
		} else if (hitNonEmpty && state->data[i] == "") {
			return i;
		}
	}
	return state->row;
}

uint32_t getPrevLineSameIndent(State *state)
{
	uint32_t current = getIndent(state, trimLeadingComment(state, state->data[state->row]));
	for (int32_t i = (int32_t)state->row - 1; i >= 0; i--) {
		if (current == getIndent(state, trimLeadingComment(state, state->data[i])) && state->data[i] != "") {
			return i;
		}
	}
	return state->row;
}

uint32_t getNextLineSameIndent(State *state)
{
	uint32_t current = getIndent(state, trimLeadingComment(state, state->data[state->row]));
	for (uint32_t i = state->row + 1; i < state->data.size(); i++) {
		if (current == getIndent(state, trimLeadingComment(state, state->data[i])) && state->data[i] != "") {
			return i;
		}
	}
	return state->row;
}

WordPosition findQuoteBounds(const std::string &str, char quoteChar, uint32_t cursor, bool includeQuote)
{
	int32_t lastQuoteIndex = -1;
	for (uint32_t i = 0; i <= cursor; i++) {
		if (str[i] == quoteChar) {
			lastQuoteIndex = i;
		}
	}
	uint32_t i;
	for (i = cursor + 1; i < str.length(); i++) {
		if (str[i] == quoteChar) {
			if (lastQuoteIndex != -1 && lastQuoteIndex < (int32_t)cursor) {
				break;
			} else {
				if (lastQuoteIndex == -1) {
					lastQuoteIndex = i;
				} else {
					break;
				}
			}
		}
	}
	if (i != str.length()) {
		if (i - lastQuoteIndex == 1 || includeQuote) {
			return { (uint32_t)(lastQuoteIndex < 0 ? 0 : lastQuoteIndex), (uint32_t)i };
		} else {
			return { (uint32_t)lastQuoteIndex + 1, (uint32_t)i - 1 };
		}
	} else {
		return { 0, 0 };
	}
}

WordPosition findParentheses(const std::string &str, char openParen, char closeParen, uint32_t cursor,
			     bool includeParen)
{
	int32_t balance = 0;
	int32_t openParenIndex = -1;
	// look back for openParen
	for (int32_t i = cursor; i >= 0; i--) {
		if (str[i] == openParen) {
			if (balance == 0) {
				openParenIndex = i;
				break;
			} else {
				balance--;
			}
		} else if (str[i] == closeParen && i != (int32_t)cursor) {
			balance++;
		}
	}
	balance = 0;
	// if haven't found yet look forward for openParen
	if (openParenIndex == -1) {
		for (int32_t i = cursor; i < (int32_t)str.length(); i++) {
			if (str[i] == openParen) {
				if (balance == 0) {
					openParenIndex = i;
					break;
				} else {
					balance--;
				}
			} else if (str[i] == closeParen) {
				balance++;
			}
		}
	}
	balance = 0;
	// if haven't found return {0,0}
	if (openParenIndex == -1) {
		return { 0, 0 };
	}
	for (int32_t i = openParenIndex + 1; i < (int32_t)str.length(); i++) {
		if (str[i] == openParen) {
			balance--;
		} else if (str[i] == closeParen) {
			if (balance == 0) {
				if (i - openParenIndex == 1 || includeParen) {
					return { (uint32_t)openParenIndex, (uint32_t)i };
				} else {
					return { (uint32_t)openParenIndex + 1, (uint32_t)i - 1 };
				}
			} else {
				balance++;
			}
		}
	}
	return { 0, 0 };
}

WordPosition getBigWordPosition(const std::string &str, uint32_t cursor)
{
	if (cursor >= str.size()) {
		return { 0, 0 };
	}
	// Move cursor to the start of the current chunk
	while (cursor > 0 && str[cursor - 1] != ' ') {
		cursor--;
	}
	// If cursor is on a space, move to the next chunk
	if (str[cursor] == ' ') {
		while (cursor < str.size() && str[cursor] == ' ') {
			cursor++;
		}
	}
	// If no non-space chunk is found
	if (cursor >= str.size()) {
		return { 0, 0 };
	}
	// Find the end of the chunk
	uint32_t start = cursor;
	uint32_t end = start;
	while (end < str.size() && str[end] != ' ') {
		end++;
	}

	return { start, end - 1 };
}

WordPosition getWordPosition(const std::string &str, uint32_t cursor)
{
	if (cursor >= str.size()) {
		return { 0, 0 };
	}
	// Move cursor to the start of the current chunk
	while (cursor > 0 && str[cursor - 1] != ' ' && str[cursor] != ' ' &&
	       (isAlphanumeric(str[cursor]) == isAlphanumeric(str[cursor - 1]))) {
		cursor--;
	}
	// If cursor is on a space, move to the next chunk
	if (str[cursor] == ' ') {
		while (cursor < str.size() && str[cursor] == ' ') {
			cursor++;
		}
	}
	// If no non-space chunk is found
	if (cursor >= str.size()) {
		return { 0, 0 };
	}
	// Find the end of the chunk
	uint32_t start = cursor;
	uint32_t end = start;
	while (end < str.size() && str[end] != ' ' && (isAlphanumeric(str[start]) == isAlphanumeric(str[end]))) {
		end++;
	}

	return { start, end - 1 };
}

bool isAllLowercase(const std::string &str)
{
	for (char ch : str) {
		if (!std::islower(ch) && !std::isspace(ch)) {
			return false;
		}
	}
	return true;
}

int32_t maxConsecutiveMatch(const std::filesystem::path &filePath, const std::string &query)
{
	std::string filePathStr = filePath.string();
	std::string queryLower = query;

	if (isAllLowercase(queryLower)) {
		std::transform(filePathStr.begin(), filePathStr.end(), filePathStr.begin(),
			       [](unsigned char c) { return std::tolower(c); });
		std::transform(queryLower.begin(), queryLower.end(), queryLower.begin(),
			       [](unsigned char c) { return std::tolower(c); });
	}

	int32_t maxLength = 0;
	int32_t currentLength = 0;
	for (size_t i = 0, j = 0; i < filePathStr.size();) {
		if (filePathStr[i] == queryLower[j]) {
			currentLength++;
			i++;
			j++;
			if (j == queryLower.size()) {
				maxLength = std::max(maxLength, currentLength);
				currentLength = 0;
				j = 0;
			}
		} else {
			i = i - currentLength;
			j = 0;
			currentLength = 0;
			i++;
		}
	}
	return maxLength;
}

void resetValidCursorState(State *state)
{
	if (state->data[state->row].length() <= state->col) {
		if (state->data[state->row].length() != 0) {
			state->col = state->data[state->row].length() - 1;
		} else {
			state->col = 0;
		}
	}
}

bool filePathContainsSubstring(const std::filesystem::path &filePath, const std::string &query)
{
	std::string filePathStr = filePath.string();
	std::string queryLower = query;

	if (isAllLowercase(queryLower)) {
		std::transform(filePathStr.begin(), filePathStr.end(), filePathStr.begin(),
			       [](unsigned char c) { return std::tolower(c); });
		std::transform(queryLower.begin(), queryLower.end(), queryLower.begin(),
			       [](unsigned char c) { return std::tolower(c); });
	}

	uint32_t filePathIndex = 0;
	uint32_t queryIndex = 0;
	while (queryIndex < queryLower.length() && filePathIndex < filePathStr.length()) {
		if (filePathStr[filePathIndex] == queryLower[queryIndex]) {
			filePathIndex++;
			queryIndex++;
		} else {
			filePathIndex++;
		}
	}

	return queryIndex == queryLower.length();
}

bool shouldIgnoreFile(const std::filesystem::path &path)
{
	std::vector<std::string> allowList = { "[...nextauth]", ".github", ".gitconfig", ".gitignore" };
	for (uint32_t i = 0; i < allowList.size(); i++) {
		if (path.string().find(allowList[i]) != std::string::npos) {
			return false;
		}
	}
	std::vector<std::string> ignoreList = {
		".nx/",	    ".mozilla/",    ".vim/",	"nyc_output/",	 "results/",
		"target/",  "resources/",   ".git",	"node_modules/", "build/",
		"dist/",    "cdk.out/",	    ".next/",	"tmp/",		 "__pycache__/",
		"autogen/", "coverage/",    "assets/",	"extra/",	 ".jar",
		".jpg",	    ".jpeg",	    ".png",	".pdf",		 "package-lock.json",
		".cache/",  ".eslintcache", ".DS_Store", "snapshots/"
	};
	for (uint32_t i = 0; i < ignoreList.size(); i++) {
		if (path.string().find(ignoreList[i]) != std::string::npos) {
			return true;
		}
	}
	return false;
}

std::vector<grepMatch> grepFile(const std::filesystem::path &file_path, const std::string &query,
				const std::filesystem::path &dir_path)
{
	auto relativePath = file_path.lexically_relative(dir_path);
	std::vector<grepMatch> matches;
	std::ifstream file(file_path);
	std::string line;
	int32_t lineNumber = 0;
	while (std::getline(file, line)) {
		lineNumber++;
		if (line.find(query) != std::string::npos) {
			matches.emplace_back(relativePath, lineNumber, line);
		}
	}
	return matches;
}

bool sortByFileType(const grepMatch &first, const grepMatch &second)
{
	std::string firstFile = first.path.string();
	std::string secondFile = second.path.string();
	if (isTestFile(firstFile) && !isTestFile(secondFile)) {
		return false;
	}
	if (!isTestFile(firstFile) && isTestFile(secondFile)) {
		return true;
	}
	if (firstFile == secondFile) {
		return first.lineNum < second.lineNum;
	}
	return firstFile < secondFile;
}

std::vector<grepMatch> grepFiles(const std::filesystem::path &dir_path, const std::string &query, bool allowAllFiles)
{
	std::vector<std::future<std::vector<grepMatch> > > futures;
	for (auto it = std::filesystem::recursive_directory_iterator(dir_path);
	     it != std::filesystem::recursive_directory_iterator(); ++it) {
		if (!allowAllFiles && shouldIgnoreFile(it->path())) {
			it.disable_recursion_pending();
			continue;
		}
		if (std::filesystem::is_regular_file(it->path())) {
			futures.push_back(std::async(std::launch::async, grepFile, it->path(), query, dir_path));
		}
	}
	std::vector<grepMatch> allMatches;
	for (auto &future : futures) {
		auto matches = future.get();
		allMatches.insert(allMatches.end(), matches.begin(), matches.end());
	}
	std::sort(allMatches.begin(), allMatches.end(), sortByFileType);

	return allMatches;
}

std::vector<std::filesystem::path> findFiles(const std::filesystem::path &dir_path, const std::string &query)
{
	std::vector<std::filesystem::path> matching_files;
	for (auto it = std::filesystem::recursive_directory_iterator(dir_path);
	     it != std::filesystem::recursive_directory_iterator(); ++it) {
		if (shouldIgnoreFile(it->path())) {
			it.disable_recursion_pending();
			continue;
		}
		if (std::filesystem::is_regular_file(it->path())) {
			auto relativePath = it->path().lexically_relative(dir_path);
			if (filePathContainsSubstring(relativePath, query)) {
				matching_files.push_back(relativePath);
			}
		}
	}
	std::sort(matching_files.begin(), matching_files.end(),
		  [&](const std::filesystem::path &a, const std::filesystem::path &b) {
			  int32_t matchA = maxConsecutiveMatch(a, query);
			  int32_t matchB = maxConsecutiveMatch(b, query);
			  if (matchA == matchB) {
				  if (isTestFile(a.string()) && !isTestFile(b.string())) {
					  return false;
				  }
				  if (!isTestFile(a.string()) && isTestFile(b.string())) {
					  return true;
				  }
				  return a.string() < b.string();
			  }
			  return matchA > matchB;
		  });
	return matching_files;
}

void generateGrepOutput(State *state, bool resetCursor)
{
	if (state->grep.query == "") {
		state->grepOutput.clear();
	} else {
		state->grepOutput = grepFiles(state->grepPath == "" ? std::filesystem::current_path() :
								      std::filesystem::path(state->grepPath),
					      state->grep.query, state->showAllGrep);
	}
	if (resetCursor) {
		state->grep.selection = 0;
	}
}

void generateFindFileOutput(State *state)
{
	state->findFileOutput = findFiles(std::filesystem::current_path(), state->findFile.query);
}

uint32_t w(State *state)
{
	fixColOverMax(state);
	bool space = state->data[state->row][state->col] == ' ';
	for (uint32_t i = state->col + 1; i < state->data[state->row].size(); i += 1) {
		if (state->data[state->row][i] == ' ') {
			space = true;
		} else if (space && state->data[state->row][i] != ' ') {
			return i;
		} else if (isAlphanumeric(state->data[state->row][state->col]) !=
			   isAlphanumeric(state->data[state->row][i])) {
			return i;
		}
	}
	return state->col;
}

uint32_t b(State *state)
{
	fixColOverMax(state);
	if (state->col == 0 || state->data[state->row].empty()) {
		return 0;
	}
	int32_t i = state->col - 1;
	while (i >= 0 && state->data[state->row][i] == ' ') {
		i--;
	}
	if (i < 0) {
		return 0;
	}
	bool isAlnum = isAlphanumeric(state->data[state->row][i]);
	for (i -= 1; i >= 0; i--) {
		if (state->data[state->row][i] == ' ') {
			return i + 1;
		} else if ((isAlphanumeric(state->data[state->row][i])) != isAlnum) {
			return i + 1;
		}
	}
	return 0;
}

int32_t getLastModifiedDate(State* state, std::string filename)
{
	try {
		std::string command;
	#ifdef __APPLE__
		command = "stat -f %m -t %s " + filename + " 2>/dev/null";
	#elif defined(__linux__)
		command = "stat -c %Y" + filename + " 2>/dev/null";
	#else
	#error "Platform not supported"
	#endif
		std::unique_ptr<FILE, int (*)(FILE *)> pipe(popen(command.c_str(), "r"), pclose);
		if (!pipe) {
			throw std::runtime_error("popen() failed!");
		}
		std::stringstream ss;
		char *line = nullptr;
		size_t len = 0;
		ssize_t read;
		while ((read = getline(&line, &len, pipe.get())) != -1) {
			ss << line;
		}
		free(line);
		std::string str;
		std::getline(ss, str);
		return std::stoi(str);
	} catch (const std::exception &e) {
		return 0;
	}
}

void saveFile(State *state)
{
	bool overwrite = true;
	if (state->lastModifiedDate < getLastModifiedDate(state, state->filename)) {
		state->namingPrefix = std::string("new changes found for ") + state->filename + " -> overwrite? (y/n):";
		std::string prompt = inputName(state, "");
		overwrite = prompt == "y";
	}
	if (overwrite) {
		state->lastSave = state->historyPosition;
		if (!state->dontSave) {
			std::ofstream file(state->filename);
			if (!state->data.empty()) {
				for (size_t i = 0; i < state->data.size() - 1; ++i) {
					file << state->data[i] << "\n";
				}
				file << state->data.back();
			}
			file.close();
			state->lastModifiedDate = getLastModifiedDate(state, state->filename);
		}
	}
}

std::vector<std::string> readFile(const std::string &filename)
{
	std::ifstream file(filename);
	std::string str;
	std::vector<std::string> file_contents;
	while (file.good()) {
		std::getline(file, str);
		file_contents.push_back(str);
	}
	file.close();
	return file_contents;
}

bool isColTooSmall(State *state)
{
	uint32_t col = getDisplayCol(state);
	return col < state->windowPosition.col;
}

bool isColTooBig(State *state)
{
	uint32_t col = getDisplayCol(state);
	return (int32_t)col - (int32_t)state->windowPosition.col >
	       (int32_t)state->maxX - (int32_t)getLineNumberOffset(state) - 1;
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
	if (state->row < state->windowPosition.row) {
		return true;
	}
	uint32_t windowRow = state->windowPosition.row;
	uint32_t rowsBelow = 0;
	while (windowRow < state->data.size() && rowsBelow + 2 < state->maxY) {
		if (state->row == windowRow) {
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
		for (uint32_t i = 0; i < state->col; i++) {
			if (state->data[state->row][i] == '\t') {
				sum += state->options.indent_size;
			} else {
				sum++;
			}
		}
		return sum;
	} else {
		return state->col;
	}
}

uint32_t getCenteredWindowPosition(State *state)
{
	uint32_t windowRow = state->row;
	uint32_t rowsAbove = getDisplayRows(state, state->row, state->col);
	while (windowRow > 0 && rowsAbove < state->maxY / 2) {
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
	auto physicalCol = state->data[r].length() < c ? state->data[r].length() : c;
	return 1 + physicalCol / (state->maxX - getLineNumberOffset(state));
}

uint32_t getDisplayRows(State *state, uint32_t r)
{
	if (!state->options.wordwrap) {
		return 1;
	}
	return 1 + getDisplayLength(state, state->data[r]) / (state->maxX - getLineNumberOffset(state));
}

void centerScreen(State *state)
{
	state->windowPosition.row = getCenteredWindowPosition(state);
	uint32_t col = getDisplayCol(state);
	if (!state->options.wordwrap) {
		if (isColTooSmall(state)) {
			state->windowPosition.col = col;
		} else if (isColTooBig(state)) {
			state->windowPosition.col = col + getLineNumberOffset(state) + 1 - state->maxX;
		}
	}
}

void upScreen(State *state)
{
	if (state->row > 0) {
		state->row -= 1;
		state->windowPosition.row -= 1;
		state->col = getNormalizedCol(state, state->hardCol);
		state->skipSetHardCol = true;
	}
}

void downScreen(State *state)
{
	if (state->row < state->data.size() - 1) {
		state->row += 1;
		state->windowPosition.row += 1;
		state->col = getNormalizedCol(state, state->hardCol);
		state->skipSetHardCol = true;
	}
}

void upHalfScreen(State *state)
{
	for (uint32_t i = 0; i < state->maxY / 2; i++) {
		if (state->row > 0) {
			state->row -= 1;
			state->windowPosition.row -= 1;
		}
	}
	state->col = getNormalizedCol(state, state->hardCol);
	state->skipSetHardCol = true;
}

void downHalfScreen(State *state)
{
	for (uint32_t i = 0; i < state->maxY / 2; i++) {
		if (state->row < state->data.size() - 1) {
			state->row += 1;
			state->windowPosition.row += 1;
		}
	}
	state->col = getNormalizedCol(state, state->hardCol);
	state->skipSetHardCol = true;
}

uint32_t getNormalizedCol(State* state, uint32_t hardCol)
{
	if (state->options.indent_style != "tab") {
		return state->col;
	}
	int32_t tmpHardCol = hardCol;
	int32_t output = 0;
	for (uint32_t i = 0; i < state->data[state->row].length() && (int32_t) i < tmpHardCol; i++) {
		if (i < state->data[state->row].length() && state->data[state->row][i] == '\t') {
			tmpHardCol -= (state->options.indent_size - 1);
			if ((int32_t) i < tmpHardCol) {
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

void up(State *state)
{
	if (state->row > 0) {
		state->row -= 1;
		state->col = getNormalizedCol(state, state->hardCol);
		state->skipSetHardCol = true;
	}
	if (isOffScreenVertical(state)) {
		state->windowPosition.row -= 1;
	}
}

void upVisual(State *state)
{
	auto visualCol = state->col;
	if (visualCol > state->data[state->row].length()) {
		visualCol = state->data[state->row].length();
	}
	if (visualCol < state->maxX - getLineNumberOffset(state)) {
		if (state->row > 0) {
			state->row -= 1;
			state->col = getNormalizedCol(state, state->hardCol);
			state->col += (state->maxX - getLineNumberOffset(state)) *
				      (state->data[state->row].length() / (state->maxX - getLineNumberOffset(state)));
		}
		if (isOffScreenVertical(state)) {
			state->windowPosition.row -= 1;
		}
	} else {
		state->col -= state->maxX - getLineNumberOffset(state);
	}
}

void down(State *state)
{
	if (state->row < state->data.size() - 1) {
		state->row += 1;
		state->col = getNormalizedCol(state, state->hardCol);
		state->skipSetHardCol = true;
	}
	if (isOffScreenVertical(state)) {
		while (isOffScreenVertical(state) && state->windowPosition.row <= state->data.size()) {
			state->windowPosition.row += 1;
		}
	}
}

bool isOnLastVisualLine(State *state)
{
	auto lastLineStarts = (state->maxX - getLineNumberOffset(state)) *
			      (state->data[state->row].length() / (state->maxX - getLineNumberOffset(state)));
	return state->col > lastLineStarts;
}

void downVisual(State *state)
{
	if (isOnLastVisualLine(state)) {
		if (state->row < state->data.size() - 1) {
			state->row += 1;
			state->col = getNormalizedCol(state, state->hardCol);
			state->col = state->col % (state->maxX - getLineNumberOffset(state));
		}
		if (isOffScreenVertical(state)) {
			state->windowPosition.row += 1;
		}
	} else {
		state->col += state->maxX - getLineNumberOffset(state);
	}
}

void left(State *state)
{
	fixColOverMax(state);
	if (state->col > 0) {
		state->col -= 1;
	}
}

void right(State *state)
{
	fixColOverMax(state);
	if (state->col < state->data[state->row].length()) {
		state->col += 1;
	}
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
		state->data[state->row] = getIndentCharacter(state) + state->data[state->row];
	}
}

void deindent(State *state)
{
	for (uint32_t i = 0; i < getIndentSize(state); i++) {
		if (state->data[state->row].substr(0, 1) == std::string("") + getIndentCharacter(state)) {
			state->data[state->row] = state->data[state->row].substr(1);
		}
	}
}

int32_t getIndexFirstNonSpace(State *state)
{
	int32_t i;
	for (i = 0; i < (int32_t)state->data[state->row].length(); i++) {
		if (state->data[state->row][i] != getIndentCharacter(state)) {
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
	state->data.insert(state->data.begin() + state->row + 1, "");
}

void insertEmptyLine(State *state)
{
	state->data.insert(state->data.begin() + state->row, "");
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

void sanityCheckGrepSelection(State *state)
{
	if (state->mode == GREP && state->grep.selection >= state->grepOutput.size()) {
		if (state->grepOutput.size() > 0) {
			state->grep.selection = state->grepOutput.size() - 1;
		} else {
			state->grep.selection = 0;
		}
	}
}

void sanityCheckFindFileSelection(State *state)
{
	if (state->mode == FINDFILE && state->findFile.selection >= state->findFileOutput.size()) {
		if (state->findFileOutput.size() > 0) {
			state->findFile.selection = state->findFileOutput.size() - 1;
		} else {
			state->findFile.selection = 0;
		}
	}
}

void sanityCheckDocumentEmpty(State *state)
{
	if (state->data.size() == 0) {
		state->data.push_back("");
	}
}

void fixColOverMax(State *state)
{
	if (state->col > state->data[state->row].length()) {
		state->col = state->data[state->row].length();
	}
}

void sanityCheckRowColOutOfBounds(State *state)
{
	if (state->row >= state->data.size()) {
		state->row = state->data.size() - 1;
	}
	if (state->mode == TYPING) {
		fixColOverMax(state);
	}
}
