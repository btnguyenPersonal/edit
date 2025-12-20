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
#include <algorithm>
#include <climits>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <iterator>
#include <memory>
#include <ncurses.h>
#include <stdexcept>
#include <string>
#include <vector>
#include <regex>
#include <chrono>
#include <thread>

void highlightRenderBounds(State *state, Bounds b)
{
	uint32_t tempR = state->file->row;
	uint32_t tempC = state->file->col;
	state->file->row = b.minR;
	state->file->col = b.minC;
	state->visual.row = b.maxR;
	state->visual.col = b.maxC;
	auto tempMode = state->mode;
	state->mode = VISUAL;
	state->visualType = SELECT;
	renderScreen(state, true);
	state->mode = tempMode;
	state->file->row = tempR;
	state->file->col = tempC;
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
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

void searchNextResult(State *state, bool reverse)
{
	if (reverse) {
		bool result = setSearchResultReverse(state, false);
		if (result == false) {
			state->searchFail = true;
		}
	} else {
		state->file->col += 1;
		uint32_t temp_col = state->file->col;
		uint32_t temp_row = state->file->row;
		bool result = setSearchResult(state);
		if (result == false) {
			state->searchFail = true;
			state->file->row = temp_row;
			state->file->col = temp_col - 1;
		}
	}
}

std::vector<std::string> getLogLines(State *state)
{
	std::vector<std::string> gitLogLines = { "current" };
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

void locateNodeModule(State *state, std::string vis)
{
	try {
		std::filesystem::path filePath(state->file->filename);
		std::filesystem::path dir = filePath.parent_path();
		std::filesystem::path current = std::filesystem::absolute(std::filesystem::current_path());
		uint32_t i = 0;
		while (i < 50 && !std::filesystem::is_directory(dir / std::string("node_modules"))) {
			dir = dir.parent_path();
			if (dir == current) {
				break;
			}
			i++;
		}
		std::filesystem::path path = dir / "node_modules" / vis / "package.json";
		if (std::filesystem::is_regular_file(path.c_str())) {
			state->resetState(path.string());
		}
	} catch (const std::filesystem::filesystem_error &e) {
		state->status = "not found";
	}
}

bool locateFile(State *state, std::string vis, std::vector<std::string> extensions)
{
	ltrim(vis);
	rtrim(vis);
	bool result = locateFileRelative(state, vis, extensions);
	if (!result) {
		result = locateFileAbsolute(state, vis);
	}
	return result;
}

bool locateFileAbsolute(State *state, std::string vis)
{
	std::string path = vis;
	try {
		if (path.length() > 0 && path[0] == '~') {
			const char *home = std::getenv("HOME");
			if (home == nullptr) {
				home = std::getenv("USERPROFILE");
			}
			if (home != nullptr) {
				path = std::string(home) + path.substr(1);
			} else {
				return false;
			}
		}
		if (std::filesystem::is_regular_file(path)) {
			std::filesystem::path filePath(path);
			state->resetState(filePath);
			return true;
		} else {
			return false;
		}
	} catch (const std::filesystem::filesystem_error &e) {
	}
	return false;
}

bool locateFileRelative(State *state, std::string vis, std::vector<std::string> extensions)
{
	if (getExtension(vis) == "js") {
		for (int32_t i = vis.length() - 1; i >= 0; i--) {
			if (vis[i] == '.') {
				vis = safeSubstring(vis, 0, i);
				break;
			}
		}
	}
	for (uint32_t i = 0; i < extensions.size(); i++) {
		try {
			std::filesystem::path filePath(state->file->filename);
			std::filesystem::path dir = filePath.parent_path();
			auto newFilePath = dir / (vis + extensions[i]);
			if (std::filesystem::is_regular_file(newFilePath.c_str())) {
				auto baseDir = std::filesystem::current_path();
				auto relativePath = std::filesystem::relative(newFilePath, baseDir);
				state->resetState(relativePath.string());
				return true;
			}
		} catch (const std::filesystem::filesystem_error &e) {
		}
	}
	return false;
}

std::vector<std::string> getDiffLines(State *state)
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
		return { "No local changes" };
	}
	return gitDiffLines;
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

uint32_t getLastCharIndex(State *state)
{
	if (state->file->data[state->file->row].length() != 0) {
		return state->file->data[state->file->row].length() - 1;
	} else {
		return 0;
	}
}

std::string getRelativeToLastAndRoute(State *state)
{
	if (state->fileStackIndex == 0) {
		return "";
	}
	std::string lastFile = state->fileStack[state->fileStackIndex - 1];
	std::filesystem::path lastDir = std::filesystem::path(std::string("./") + lastFile).parent_path();
	std::filesystem::path currentDir = std::filesystem::path(std::string("./") + state->file->filename).parent_path();
	auto relativePath = std::filesystem::relative(state->file->filename, lastDir).string();
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
	std::filesystem::path currentDir = std::filesystem::path(std::string("./") + state->file->filename).parent_path();
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

void rename(State *state, const std::filesystem::path &oldPath, const std::string &newName)
{
	if (!std::filesystem::exists(oldPath)) {
		state->status = "path does not exist";
		return;
	}

	std::filesystem::path newPath = std::filesystem::relative(oldPath.parent_path() / newName, std::filesystem::current_path());

	try {
		std::filesystem::rename(oldPath, newPath);
	} catch (const std::filesystem::filesystem_error &e) {
		state->status = std::string("Failed to rename: ") + std::string(e.what());
	}

	auto relativePath = std::filesystem::relative(oldPath, std::filesystem::current_path()).string();
	if (state->file->filename == relativePath) {
		state->file->filename = newPath.string();
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
		std::filesystem::path newPath = directory / (stem.string() + " (" + std::to_string(i) + ")" + extension.string());
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
	char firstParen = state->file->data[state->file->row][state->file->col];
	int32_t stack = 0;
	if (isOpenParen(firstParen)) {
		char secondParen = getCorrespondingParen(firstParen);
		uint32_t col = state->file->col;
		for (uint32_t row = state->file->row; row < state->file->data.size(); row++) {
			while (col < state->file->data[row].size()) {
				if (state->file->data[row][col] == secondParen) {
					if (stack == 1) {
						return { row, col };
					} else {
						stack--;
					}
				} else if (state->file->data[row][col] == firstParen) {
					stack++;
				}
				col++;
			}
			col = 0;
		}
	} else if (isCloseParen(firstParen)) {
		char secondParen = getCorrespondingParen(firstParen);
		int32_t col = (int32_t)state->file->col;
		bool first = true;
		for (int32_t row = (int32_t)state->file->row; row >= 0; row--) {
			if (!first) {
				col = state->file->data[row].length() > 0 ? state->file->data[row].length() - 1 : 0;
			}
			while (col >= 0) {
				if (state->file->data[row][col] == secondParen) {
					if (stack == 1) {
						return { (uint32_t)row, (uint32_t)col };
					} else {
						stack--;
					}
				} else if (state->file->data[row][col] == firstParen) {
					stack++;
				}
				col--;
			}
			first = false;
		}
	}
	return { state->file->row, state->file->col };
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

int32_t findChar(State *state, bool reverse, char c)
{
	if (reverse) {
		return findPrevChar(state, c);
	} else {
		return findNextChar(state, c);
	}
}

int32_t toChar(State *state, bool reverse, char c)
{
	if (reverse) {
		return toPrevChar(state, c);
	} else {
		return toNextChar(state, c);
	}
}

void repeatPrevLineSearch(State *state, bool reverse)
{
	char temp = state->prevSearch.type;
	switch (state->prevSearch.type) {
	case 'f':
		state->file->col = findChar(state, reverse, state->prevSearch.search);
		break;
	case 'F':
		state->file->col = findChar(state, !reverse, state->prevSearch.search);
		break;
	case 't':
		state->file->col = toChar(state, reverse, state->prevSearch.search);
		break;
	case 'T':
		state->file->col = toChar(state, !reverse, state->prevSearch.search);
		break;
	default:
		break;
	}
	state->prevSearch.type = temp;
}

uint32_t findPrevChar(State *state, char c)
{
	state->prevSearch.type = 'F';
	state->prevSearch.search = c;
	for (int32_t i = state->file->col - 1; i >= 0; i--) {
		if (state->file->data[state->file->row][i] == c) {
			return (int32_t)i;
		}
	}
	return state->file->col;
}

uint32_t toPrevChar(State *state, char c)
{
	state->prevSearch.type = 'T';
	state->prevSearch.search = c;
	int32_t index = state->file->col;
	for (int32_t i = state->file->col - 1; i >= 0; i--) {
		if (state->file->data[state->file->row][i] == c) {
			return (uint32_t)index;
		} else {
			index = i;
		}
	}
	return state->file->col;
}

uint32_t findNextChar(State *state, char c)
{
	state->prevSearch.type = 'f';
	state->prevSearch.search = c;
	for (uint32_t i = state->file->col + 1; i < state->file->data[state->file->row].length(); i++) {
		if (state->file->data[state->file->row][i] == c) {
			return i;
		}
	}
	return state->file->col;
}

uint32_t toNextChar(State *state, char c)
{
	state->prevSearch.type = 't';
	state->prevSearch.search = c;
	uint32_t index = state->file->col;
	for (uint32_t i = state->file->col + 1; i < state->file->data[state->file->row].length(); i++) {
		if (state->file->data[state->file->row][i] == c) {
			return index;
		} else {
			index = i;
		}
	}
	return state->file->col;
}

std::string getGitHash(State *state)
{
	std::stringstream command;
	command << "git blame -l -L " << state->file->row + 1 << ",+1 " << state->file->filename << " | awk '{print $1}'";
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
		std::string command = "git --no-pager blame ./" + filename + " --date=short 2>/dev/null | awk '{print $1, $2, $3, $4, \")\"}'";
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

std::string getCurrentWord(State *state)
{
	std::string currentWord = "";
	for (int32_t i = (int32_t)state->file->col - 1; i >= 0; i--) {
		if (isAlphanumeric(state->file->data[state->file->row][i])) {
			currentWord = state->file->data[state->file->row][i] + currentWord;
		} else {
			break;
		}
	}
	return currentWord;
}

void replaceCurrentLine(State *state, const std::string &query, const std::string &replace)
{
	if (query.empty()) {
		return;
	}
	size_t startPos = 0;
	while ((startPos = state->file->data[state->file->row].find(query, startPos)) != std::string::npos) {
		state->file->data[state->file->row].replace(startPos, query.length(), replace);
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
	state->changeFile(state->file->filename);
}

void replaceAllGlobally(State* state, const std::string& query, const std::string& replace) {
	if (query.empty()) {
		state->status = "query empty";
		return;
	}

	try {
		std::regex re(query);
	} catch (const std::regex_error&) {
		state->status = "invalid regex";
		return;
	}

	std::regex re(query);

	FILE* pipe = popen("git ls-files", "r");
	if (!pipe) {
		// TODO use all files if git not available
		state->status = "git not available";
		return;
	}

	char path[1024];
	int modified_count = 0;

	while (fgets(path, sizeof(path), pipe)) {
		std::string filename = path;
		if (!filename.empty() && filename.back() == '\n') {
			filename.pop_back();
		}

		std::filesystem::path full_path = std::filesystem::current_path() / filename;
		if (!std::filesystem::is_regular_file(full_path)) {
			continue;
		}

		std::ifstream in(full_path, std::ios::binary);
		if (!in) {
			continue;
		}

		std::string content((std::istreambuf_iterator<char>(in)),
		std::istreambuf_iterator<char>());

		std::string replaced = std::regex_replace(content, re, replace);

		if (replaced != content) {
			std::ofstream out(full_path, std::ios::binary);
			if (out) {
				out << replaced;
				modified_count++;
			}
		}
	}
	pclose(pipe);

	if (state->file) {
		state->reloadFile(state->file->filename);
	}

	state->status = modified_count > 0
	? "Replaced in " + std::to_string(modified_count) + " files"
	: "No matches found";
}

void replaceAll(State *state, const std::string &query, const std::string &replace)
{
	uint32_t i = 0;
	uint32_t max = state->file->data.size();
	if (state->replaceBounds.minR != 0 || state->replaceBounds.maxR != 0) {
		i = state->replaceBounds.minR;
		max = state->replaceBounds.maxR + 1;
	}
	for (; i < max && i < state->file->data.size(); i++) {
		if (query.empty()) {
			return;
		}
		size_t startPos = 0;
		while ((startPos = state->file->data[i].find(query, startPos)) != std::string::npos) {
			state->file->data[i].replace(startPos, query.length(), replace);
			startPos += replace.length();
		}
	}
}

bool setSearchResultReverse(State *state, bool allowCurrent)
{
	if (state->search.query == "") {
		return false;
	}
	fixColOverMax(state);
	uint32_t initialCol = state->file->col;
	uint32_t initialRow = state->file->row;
	uint32_t col = initialCol;
	if (allowCurrent) {
		col += state->search.query.length();
	}
	uint32_t row = initialRow;
	bool isFirst = true;
	do {
		std::string line = isFirst ? state->file->data[row].substr(0, col) : state->file->data[row];
		size_t index = line.rfind(state->search.query);
		if (index != std::string::npos) {
			state->file->row = row;
			state->file->col = static_cast<uint32_t>(index);
			return true;
		}
		if (row == 0) {
			row = state->file->data.size() - 1;
		} else {
			row--;
		}
		isFirst = false;
	} while (row != initialRow);
	// try last row again
	std::string line = state->file->data[row];
	size_t index = line.rfind(state->search.query);
	if (index != std::string::npos) {
		state->file->row = row;
		state->file->col = static_cast<uint32_t>(index);
		return true;
	}
	return false;
}

bool searchFromTop(State *state)
{
	for (uint32_t i = 0; i < state->file->data.size(); i++) {
		size_t index = state->file->data[i].rfind(state->search.query);
		if (index != std::string::npos) {
			state->file->row = i;
			state->file->col = static_cast<uint32_t>(index);
			return true;
		}
	}
	return false;
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

bool setSearchResult(State *state)
{
	if (state->search.query == "") {
		return false;
	}
	fixColOverMax(state);
	uint32_t initialCol = state->file->col;
	uint32_t initialRow = state->file->row;
	uint32_t col = initialCol;
	uint32_t row = initialRow;
	do {
		std::string line = state->file->data[row].substr(col);
		size_t index = line.find(state->search.query);
		if (index != std::string::npos) {
			state->file->row = row;
			state->file->col = static_cast<uint32_t>(index) + col;
			return true;
		}
		row = (row + 1) % state->file->data.size();
		col = 0;
	} while (row != initialRow);
	std::string line = state->file->data[row];
	size_t index = line.find(state->search.query);
	if (index != std::string::npos) {
		state->file->row = row;
		state->file->col = static_cast<uint32_t>(index) + col;
		return true;
	}
	return false;
}

void setPosition(State *state, Position pos)
{
	state->file->row = pos.row;
	state->file->col = pos.col;
}

void initVisual(State *state, VisualType visualType)
{
	state->visualType = visualType;
	state->visual.row = state->file->row;
	state->visual.col = state->file->col;
}

bool is_number(const std::string &s)
{
	std::string::const_iterator it = s.begin();
	while (it != s.end() && std::isdigit(*it))
		++it;
	return !s.empty() && it == s.end();
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
	for (int32_t i = (int32_t)state->file->row; i >= 0; i--) {
		if (state->file->data[i] != "") {
			hitNonEmpty = true;
		} else if (hitNonEmpty && state->file->data[i] == "") {
			return i;
		}
	}
	return state->file->row;
}

uint32_t getNextEmptyLine(State *state)
{
	bool hitNonEmpty = false;
	for (uint32_t i = state->file->row; i < state->file->data.size(); i++) {
		if (state->file->data[i] != "") {
			hitNonEmpty = true;
		} else if (hitNonEmpty && state->file->data[i] == "") {
			return i;
		}
	}
	return state->file->row;
}

uint32_t getPrevLineSameIndent(State *state)
{
	uint32_t current = getIndent(state, trimLeadingComment(state, state->file->data[state->file->row]));
	for (int32_t i = (int32_t)state->file->row - 1; i >= 0; i--) {
		if (current == getIndent(state, trimLeadingComment(state, state->file->data[i])) && state->file->data[i] != "") {
			return i;
		}
	}
	return state->file->row;
}

uint32_t getNextLineSameIndent(State *state)
{
	uint32_t current = getIndent(state, trimLeadingComment(state, state->file->data[state->file->row]));
	for (uint32_t i = state->file->row + 1; i < state->file->data.size(); i++) {
		if (current == getIndent(state, trimLeadingComment(state, state->file->data[i])) && state->file->data[i] != "") {
			return i;
		}
	}
	return state->file->row;
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

WordPosition findParentheses(const std::string &str, char openParen, char closeParen, uint32_t cursor, bool includeParen)
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
	while (cursor > 0 && str[cursor - 1] != ' ' && str[cursor] != ' ' && (isAlphanumeric(str[cursor]) == isAlphanumeric(str[cursor - 1]))) {
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

uint32_t w(State *state)
{
	fixColOverMax(state);
	bool space = state->file->data[state->file->row][state->file->col] == ' ';
	for (uint32_t i = state->file->col + 1; i < state->file->data[state->file->row].size(); i += 1) {
		if (state->file->data[state->file->row][i] == ' ') {
			space = true;
		} else if (space && state->file->data[state->file->row][i] != ' ') {
			return i;
		} else if (isAlphanumeric(state->file->data[state->file->row][state->file->col]) != isAlphanumeric(state->file->data[state->file->row][i])) {
			return i;
		}
	}
	return state->file->col;
}

uint32_t b(State *state)
{
	fixColOverMax(state);
	if (state->file->col == 0 || state->file->data[state->file->row].empty()) {
		return 0;
	}
	int32_t i = state->file->col - 1;
	while (i >= 0 && state->file->data[state->file->row][i] == ' ') {
		i--;
	}
	if (i < 0) {
		return 0;
	}
	bool isAlnum = isAlphanumeric(state->file->data[state->file->row][i]);
	for (i -= 1; i >= 0; i--) {
		if (state->file->data[state->file->row][i] == ' ') {
			return i + 1;
		} else if ((isAlphanumeric(state->file->data[state->file->row][i])) != isAlnum) {
			return i + 1;
		}
	}
	return 0;
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

void centerScreen(State *state)
{
	if (state->file) {
		state->file->windowPosition.row = getCenteredWindowPosition(state);
		uint32_t col = getDisplayCol(state);
		if (!state->options.wordwrap) {
			if (isColTooSmall(state)) {
				state->file->windowPosition.col = col;
			} else if (isColTooBig(state)) {
				state->file->windowPosition.col = col + getLineNumberOffset(state) + 1 - state->maxX;
			}
		}
	}
}

void upScreen(State *state)
{
	if (state->file->row > 0) {
		state->file->row -= 1;
		state->file->windowPosition.row -= 1;
		state->file->col = getNormalizedCol(state, state->file->hardCol);
		state->skipSetHardCol = true;
	}
}

void downScreen(State *state)
{
	if (state->file->row < state->file->data.size() - 1) {
		state->file->row += 1;
		state->file->windowPosition.row += 1;
		state->file->col = getNormalizedCol(state, state->file->hardCol);
		state->skipSetHardCol = true;
	}
}

void upHalfScreen(State *state)
{
	for (uint32_t i = 0; i < state->maxY / 2; i++) {
		if (state->file->row > 0) {
			state->file->row -= 1;
			state->file->windowPosition.row -= 1;
		}
	}
	state->file->col = getNormalizedCol(state, state->file->hardCol);
	state->skipSetHardCol = true;
}

void downHalfScreen(State *state)
{
	for (uint32_t i = 0; i < state->maxY / 2; i++) {
		if (state->file->row < state->file->data.size() - 1) {
			state->file->row += 1;
			state->file->windowPosition.row += 1;
		}
	}
	state->file->col = getNormalizedCol(state, state->file->hardCol);
	state->skipSetHardCol = true;
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

void up(State *state)
{
	if (state->file->row > 0) {
		state->file->row -= 1;
		state->file->col = getNormalizedCol(state, state->file->hardCol);
		state->skipSetHardCol = true;
	}
	if (isOffScreenVertical(state)) {
		state->file->windowPosition.row -= 1;
	}
}

void upVisual(State *state)
{
	auto visualCol = state->file->col;
	if (visualCol > state->file->data[state->file->row].length()) {
		visualCol = state->file->data[state->file->row].length();
	}
	if (visualCol < state->maxX - getLineNumberOffset(state)) {
		if (state->file->row > 0) {
			state->file->row -= 1;
			state->file->col = getNormalizedCol(state, state->file->hardCol);
			state->file->col += (state->maxX - getLineNumberOffset(state)) * (state->file->data[state->file->row].length() / (state->maxX - getLineNumberOffset(state)));
		}
		if (isOffScreenVertical(state)) {
			state->file->windowPosition.row -= 1;
		}
	} else {
		state->file->col -= state->maxX - getLineNumberOffset(state);
	}
}

uint32_t isLargeFile(State *state)
{
	return state->file && state->file->data.size() > 50000;
}

void down(State *state)
{
	if (state->file->row < state->file->data.size() - 1) {
		state->file->row += 1;
		state->file->col = getNormalizedCol(state, state->file->hardCol);
		state->skipSetHardCol = true;
	}
	if (isOffScreenVertical(state)) {
		while (isOffScreenVertical(state) && state->file->windowPosition.row <= state->file->data.size()) {
			state->file->windowPosition.row += 1;
		}
	}
}

bool isOnLastVisualLine(State *state)
{
	auto lastLineStarts = (state->maxX - getLineNumberOffset(state)) * (state->file->data[state->file->row].length() / (state->maxX - getLineNumberOffset(state)));
	return state->file->col > lastLineStarts;
}

void downVisual(State *state)
{
	if (isOnLastVisualLine(state)) {
		if (state->file->row < state->file->data.size() - 1) {
			state->file->row += 1;
			state->file->col = getNormalizedCol(state, state->file->hardCol);
			state->file->col = state->file->col % (state->maxX - getLineNumberOffset(state));
		}
		if (isOffScreenVertical(state)) {
			state->file->windowPosition.row += 1;
		}
	} else {
		state->file->col += state->maxX - getLineNumberOffset(state);
	}
}

void left(State *state)
{
	fixColOverMax(state);
	if (state->file->col > 0) {
		state->file->col -= 1;
	}
}

void right(State *state)
{
	fixColOverMax(state);
	if (state->file->col < state->file->data[state->file->row].length()) {
		state->file->col += 1;
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
