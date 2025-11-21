#include "clipboard.h"
#include "../keybinds/sendVisualKeys.h"
#include "helper.h"
#include "indent.h"
#include "state.h"
#include <array>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

std::string getFromClipboard(State* state, bool useSystemClipboard)
{
	if (!useSystemClipboard || state->dontRecordKey) {
		return state->clipboard;
	}
	std::string command;
#ifdef __APPLE__
	command = "pbpaste 2>/dev/null";
#elif defined(__linux__)
	command = "xclip -selection clipboard -o 2>/dev/null";
#else
#error "Platform not supported"
#endif

	std::string result;
	std::array<char, 256> buffer;
	FILE *pipe = popen(command.c_str(), "r");

	if (!pipe)
		throw std::runtime_error("popen() failed!");

	while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
		result += buffer.data();
	}

	int32_t status = pclose(pipe);
	if (status != 0 || result.empty()) {
		return { "" };
	}
	return result;
}

std::string escapeForShell(const std::string &s)
{
	std::ostringstream oss;
	for (auto c : s) {
		if (c == '\'') {
			oss << "'\\''";
		} else {
			oss << c;
		}
	}
	return oss.str();
}

std::string getFileFromClipboard()
{
#ifdef __APPLE__
	FILE *pipe = popen("osascript -e 'POSIX path of (the clipboard as «class furl»)'", "r");
#elif defined(__linux__)
	FILE *pipe = popen("xclip -selection clipboard -o -t text/uri-list", "r");
#else
#error "Platform not supported"
#endif

	if (!pipe) {
		throw std::runtime_error("Failed to open pipe");
	}

	char buffer[128];
	std::string result = "";
	while (!feof(pipe)) {
		if (fgets(buffer, 128, pipe) != nullptr)
			result += buffer;
	}
	pclose(pipe);

	if (!result.empty() && result[result.length() - 1] == '\n') {
		result.erase(result.length() - 1);
	}

	return result;
}

void pasteFileFromClipboard(State *state, const std::string &destFolder)
{
	try {
		std::string sourcePathStr = getFileFromClipboard();
		state->status = sourcePathStr;
#ifdef __APPLE__
		std::filesystem::path sourcePath(sourcePathStr);
#elif defined(__linux__)
		std::filesystem::path sourcePath(sourcePathStr.substr(7));
#else
#error "Platform not supported"
#endif
		if (!std::filesystem::exists(sourcePath)) {
			state->status = std::string("Error: file does not exist: ") + sourcePath.string();
			return;
		}

		std::filesystem::path destPath = std::filesystem::path(destFolder) / sourcePath.filename();

		if (std::filesystem::is_directory(sourcePath)) {
			std::filesystem::path canonicalSrc = std::filesystem::canonical(sourcePath);
			std::filesystem::path canonicalDest = std::filesystem::canonical(destFolder);
			if (canonicalDest.string().find(canonicalSrc.string()) == 0) {
				state->status = "Error: Cannot paste a folder into itself or its subdirectories";
				return;
			}
		}

		destPath = getUniqueFilePath(destPath);

		if (std::filesystem::is_directory(sourcePath)) {
			std::filesystem::copy(sourcePath, destPath,
					      std::filesystem::copy_options::recursive |
						      std::filesystem::copy_options::overwrite_existing);
		} else {
			std::filesystem::copy(sourcePath, destPath, std::filesystem::copy_options::overwrite_existing);
		}
	} catch (const std::exception &e) {
		state->status = "File paste failed: " + std::string(e.what());
	}
}

void copyPathToClipboard(State *state, const std::string &filePath)
{
	std::filesystem::path fsPath(filePath);
	if (!std::filesystem::exists(fsPath)) {
		state->status = "Path does not exist: " + filePath;
		return;
	}
	if (!std::filesystem::is_regular_file(fsPath) && !std::filesystem::is_directory(fsPath)) {
		state->status = "Path is neither a file nor a directory: " + filePath;
		return;
	}

#ifdef __APPLE__
	std::string command = "osascript -e 'set the clipboard to POSIX file \"" + escapeForShell(filePath) + "\"'";
#elif defined(__linux__)
	auto path = std::string("file://") + escapeForShell(filePath);
	std::string command = "xclip -selection clipboard -t text/uri-list";
#else
#error "Platform not supported"
#endif
	try {
		FILE *pipe = popen(command.c_str(), "w");
		if (pipe != nullptr) {
#ifdef __APPLE__
			pclose(pipe);
#else
			fwrite(path.c_str(), sizeof(char), path.size(), pipe);
			pclose(pipe);
#endif
		}
		state->status = filePath;
	} catch (const std::exception &e) {
		state->status = std::string("file copy failed") + filePath;
	}
}

void pasteVisual(State *state, std::string text)
{
	std::vector<std::string> clip;
	std::stringstream ss(text);
	std::string line;
	while (std::getline(ss, line)) {
		clip.push_back(line);
	}
	fixColOverMax(state);
	bool isClipLine = !text.empty() && text.back() == '\n';
	if (state->visualType == LINE && !isClipLine) {
		auto pos = deleteInVisual(state);
		state->row = pos.row;
		state->col = pos.col;
		insertEmptyLine(state);
	} else if (state->visualType == NORMAL && isClipLine) {
		auto pos = changeInVisual(state);
		state->row = pos.row;
		state->col = pos.col;
		std::string current = state->data[state->row];
		state->data[state->row] = current.substr(0, state->col);
		state->data.insert(state->data.begin() + state->row + 1, current.substr(state->col));
		state->row += 1;
		state->col = 0;
		indentLine(state);
	} else {
		auto pos = deleteInVisual(state);
		state->row = pos.row;
		state->col = pos.col;
	}
	if (state->data.size() == 0) {
		for (uint32_t i = 0; i < clip.size(); i++) {
			state->data.push_back(clip[i]);
		}
	} else if (!text.empty() && text.back() == '\n') {
		for (int32_t i = 0; i < (int32_t)clip.size(); i++) {
			state->data.insert(state->data.begin() + i + state->row, clip[i]);
		}
	} else if (clip.size() > 0) {
		std::string current = state->data[state->row];
		state->data[state->row] = current.substr(0, state->col);
		state->data[state->row] += clip[0];
		int32_t lastRow = state->row;
		for (int32_t i = 1; i < (int32_t)clip.size(); i++) {
			int32_t r = i + state->row;
			state->data.insert(state->data.begin() + r, clip[i]);
			lastRow = r;
		}
		state->data[lastRow] += safeSubstring(current, state->col);
	}
}

void paste(State *state, std::string text)
{
	fixColOverMax(state);
	std::vector<std::string> clip;
	std::stringstream ss(text);
	std::string line;
	while (std::getline(ss, line)) {
		clip.push_back(line);
	}
	if (state->data.size() == 0) {
		for (uint32_t i = 0; i < clip.size(); i++) {
			state->data.push_back(clip[i]);
		}
	} else if (!text.empty() && state->pasteAsBlock) {
		for (int32_t i = 0; i < (int32_t)clip.size(); i++) {
			if (state->row + i >= state->data.size()) {
				state->data.push_back("");
			}
			std::string front = safeSubstring(state->data[state->row + i], 0, state->col);
			std::string back = safeSubstring(state->data[state->row + i], state->col);
			state->data[state->row + i] = front + clip[i] + back;
		}
	} else if (!text.empty() && text.back() == '\n') {
		for (int32_t i = 0; i < (int32_t)clip.size(); i++) {
			state->data.insert(state->data.begin() + i + state->row, clip[i]);
		}
	} else if (clip.size() > 0) {
		std::string current = state->data[state->row];
		state->data[state->row] = current.substr(0, state->col);
		state->data[state->row] += clip[0];
		int32_t lastRow = state->row;
		for (int32_t i = 1; i < (int32_t)clip.size(); i++) {
			int32_t r = i + state->row;
			state->data.insert(state->data.begin() + r, clip[i]);
			lastRow = r;
		}
		state->data[lastRow] += current.substr(state->col);
	}
}

void pasteAfter(State *state, std::string text)
{
	fixColOverMax(state);
	std::vector<std::string> clip;
	std::stringstream ss(text);
	std::string line;
	while (std::getline(ss, line)) {
		clip.push_back(line);
	}
	if (!text.empty() && state->pasteAsBlock) {
		for (int32_t i = 0; i < (int32_t)clip.size(); i++) {
			if (state->row + i >= state->data.size()) {
				state->data.push_back("");
			}
			std::string front = safeSubstring(state->data[state->row + i], 0, state->col + 1);
			std::string back = safeSubstring(state->data[state->row + i], state->col + 1);
			state->data[state->row + i] = front + clip[i] + back;
		}
	} else if (!text.empty() && text.back() == '\n') {
		for (int32_t i = 0; i < (int32_t)clip.size(); i++) {
			state->data.insert(state->data.begin() + i + state->row + 1, clip[i]);
		}
	} else if (clip.size() > 0) {
		std::string current = state->data[state->row];
		int32_t breakCol = state->col;
		if (state->col + 1 <= state->data[state->row].length()) {
			breakCol = state->col + 1;
		}
		state->data[state->row] = current.substr(0, breakCol);
		state->data[state->row] += clip[0];
		int32_t lastRow = state->row;
		for (int32_t i = 1; i < (int32_t)clip.size(); i++) {
			int32_t r = i + state->row;
			state->data.insert(state->data.begin() + r, clip[i]);
			lastRow = r;
		}
		state->data[lastRow] += current.substr(breakCol);
	}
}

void copyToClipboard(State *state, const std::string &clip, bool useSystemClipboard)
{
#ifdef __APPLE__
	FILE *pipe = popen("pbcopy", "w");
#elif defined(__linux__)
	FILE *pipe = popen("xclip -selection clipboard", "w");
#else
#error "OS not supported"
#endif
	if (!useSystemClipboard || state->dontRecordKey) {
		state->clipboard = clip;
	}
	if (pipe != nullptr) {
		fwrite(clip.c_str(), sizeof(char), clip.size(), pipe);
		pclose(pipe);
	}
	state->pasteAsBlock = false;
}
