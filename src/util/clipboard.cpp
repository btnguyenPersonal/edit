#include "clipboard.h"
#include "../keybinds/sendVisualKeys.h"
#include "helper.h"
#include "indent.h"
#include "state.h"
#include "sanity.h"
#include <array>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

std::string getFromClipboard(State *state, bool useSystemClipboard)
{
	if (!useSystemClipboard || state->dontRecordKey) {
		return state->clipboard;
	}
	state->pasteAsBlock = false;
	std::string command;
#ifdef __APPLE__
	command = "pbpaste 2>/dev/null";
#elif defined(__linux__)
	command = "[ \"$XDG_SESSION_TYPE\" = \"wayland\" ] && command -v wl-paste >/dev/null 2>&1 && wl-paste || xclip -selection clipboard -o 2>/dev/null";
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
	FILE *pipe = popen("[ \"$XDG_SESSION_TYPE\" = \"wayland\" ] && command -v wl-paste >/dev/null 2>&1 && wl-paste || xclip -selection clipboard -o -t text/uri-list", "r");
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
			std::filesystem::copy(sourcePath, destPath, std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing);
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
	std::string command = "[ \"$XDG_SESSION_TYPE\" = \"wayland\" ] && command -v wl-copy >/dev/null 2>&1 && wl-copy || xclip -selection clipboard -o -t text/uri-list";
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
		state->file->row = pos.row;
		state->file->col = pos.col;
		insertEmptyLine(state);
	} else if (state->visualType == SELECT && isClipLine) {
		auto pos = changeInVisual(state);
		state->file->row = pos.row;
		state->file->col = pos.col;
		std::string current = state->file->data[state->file->row];
		state->file->data[state->file->row] = current.substr(0, state->file->col);
		state->file->data.insert(state->file->data.begin() + state->file->row + 1, current.substr(state->file->col));
		state->file->row += 1;
		state->file->col = 0;
		indentLine(state);
	} else {
		auto pos = deleteInVisual(state);
		state->file->row = pos.row;
		state->file->col = pos.col;
	}
	if (state->file->data.size() == 0) {
		for (uint32_t i = 0; i < clip.size(); i++) {
			state->file->data.push_back(clip[i]);
		}
	} else if (!text.empty() && text.back() == '\n') {
		state->file->data.insert(state->file->data.begin() + state->file->row, clip.begin(), clip.end());
	} else if (clip.size() > 0) {
		std::string current = state->file->data[state->file->row];
		state->file->data[state->file->row] = current.substr(0, state->file->col);
		state->file->data[state->file->row] += clip[0];
		int32_t lastRow = state->file->row;
		if (clip.size() > 1) {
			state->file->data.insert(state->file->data.begin() + state->file->row + 1, clip.begin() + 1, clip.end());
		}
		for (int32_t i = 1; i < (int32_t)clip.size(); i++) {
			int32_t r = i + state->file->row;
			lastRow = r;
		}
		state->file->data[lastRow] += safeSubstring(current, state->file->col);
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
	if (state->file->data.size() == 0) {
		for (uint32_t i = 0; i < clip.size(); i++) {
			state->file->data.push_back(clip[i]);
		}
	} else if (!text.empty() && state->pasteAsBlock) {
		for (int32_t i = 0; i < (int32_t)clip.size(); i++) {
			if (state->file->row + i >= state->file->data.size()) {
				state->file->data.push_back("");
			}
			std::string front = safeSubstring(state->file->data[state->file->row + i], 0, state->file->col);
			std::string back = safeSubstring(state->file->data[state->file->row + i], state->file->col);
			state->file->data[state->file->row + i] = front + clip[i] + back;
		}
	} else if (!text.empty() && text.back() == '\n') {
		state->file->data.insert(state->file->data.begin() + state->file->row, clip.begin(), clip.end());
	} else if (clip.size() > 0) {
		std::string current = state->file->data[state->file->row];
		state->file->data[state->file->row] = current.substr(0, state->file->col);
		state->file->data[state->file->row] += clip[0];
		int32_t lastRow = state->file->row;
		if (clip.size() > 1) {
			state->file->data.insert(state->file->data.begin() + state->file->row + 1, clip.begin() + 1, clip.end());
		}
		for (int32_t i = 1; i < (int32_t)clip.size(); i++) {
			int32_t r = i + state->file->row;
			lastRow = r;
		}
		state->file->data[lastRow] += current.substr(state->file->col);
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
			if (state->file->row + i >= state->file->data.size()) {
				state->file->data.push_back("");
			}
			std::string front = safeSubstring(state->file->data[state->file->row + i], 0, state->file->col + 1);
			std::string back = safeSubstring(state->file->data[state->file->row + i], state->file->col + 1);
			state->file->data[state->file->row + i] = front + clip[i] + back;
		}
	} else if (!text.empty() && text.back() == '\n') {
		state->file->data.insert(state->file->data.begin() + state->file->row + 1, clip.begin(), clip.end());
	} else if (clip.size() > 0) {
		std::string current = state->file->data[state->file->row];
		int32_t breakCol = state->file->col;
		if (state->file->col + 1 <= state->file->data[state->file->row].length()) {
			breakCol = state->file->col + 1;
		}
		state->file->data[state->file->row] = current.substr(0, breakCol);
		state->file->data[state->file->row] += clip[0];
		int32_t lastRow = state->file->row;
		if (clip.size() > 1) {
			state->file->data.insert(state->file->data.begin() + state->file->row + 1, clip.begin() + 1, clip.end());
		}
		for (int32_t i = 1; i < (int32_t)clip.size(); i++) {
			int32_t r = i + state->file->row;
			lastRow = r;
		}
		state->file->data[lastRow] += current.substr(breakCol);
	}
}

void copyToClipboard(State *state, const std::string &clip, bool useSystemClipboard)
{
	state->clipboard = clip;
	if (!useSystemClipboard || state->dontRecordKey) {
		return;
	}
#ifdef __APPLE__
	FILE *pipe = popen("pbcopy", "w");
#elif defined(__linux__)
	FILE *pipe = popen("[ \"$XDG_SESSION_TYPE\" = \"wayland\" ] && command -v wl-copy >/dev/null 2>&1 && wl-copy || xclip -selection clipboard", "w");
#else
#error "OS not supported"
#endif
	if (pipe != nullptr) {
		fwrite(clip.c_str(), sizeof(char), clip.size(), pipe);
		pclose(pipe);
	}
	state->pasteAsBlock = false;
}
