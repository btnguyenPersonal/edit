#include "clipboard.h"
// TODO(ben): remove this eventually
#include "../keybinds/sendVisualKeys.h"
#include "indent.h"
#include "state.h"
#include "sanity.h"
#include "bounds.h"
#include "fileops.h"
#include "textedit.h"
#include "external.h"
#include "string.h"
#include "textbuffer.h"
#include <string>
#include <vector>
#include <unistd.h>
#include <fcntl.h>

static const char b64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
				"abcdefghijklmnopqrstuvwxyz"
				"0123456789+/";

std::string base64_encode(const std::string &input)
{
	std::string output;
	int val = 0;
	int valb = -6;

	for (unsigned char c : input) {
		val = (val << 8) + c;
		valb += 8;
		while (valb >= 0) {
			output.push_back(b64_table[(val >> valb) & 0x3F]);
			valb -= 6;
		}
	}

	if (valb > -6) {
		output.push_back(b64_table[((val << 8) >> (valb + 8)) & 0x3F]);
	}

	while (output.size() % 4) {
		output.push_back('=');
	}

	return output;
}

void osc52copy(std::string clip)
{
	std::string encoded = base64_encode(clip);
	std::string seq;
	if (getenv("TMUX")) {
		seq = "\033Ptmux;\033\033]52;c;" + encoded + "\033\033\\\033\\";
	} else {
		seq = "\033]52;c;" + encoded + "\033\\";
	}
	int fd = open("/dev/tty", O_WRONLY);
	if (fd != -1) {
		write(fd, seq.data(), seq.size());
		close(fd);
	}
}

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
	std::string output = runCommand(command);
	if (output.length() > 0 && output.back() == '\n') {
		output.pop_back();
	}
	return output;
}

std::string escapeForShell(const std::string &s)
{
	std::string output = "";
	for (char c : s) {
		if (c == '\'') {
			output += "'\\''";
		} else {
			output += c;
		}
	}
	return output;
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
		state->status = std::string("pasted: ") + destPath.string();
	} catch (const std::exception &e) {
		state->status = "file paste failed: " + std::string(e.what());
	}
}

void copyPathToClipboard(State *state, const std::string &filePath)
{
	std::filesystem::path fsPath(filePath);
	if (!std::filesystem::exists(fsPath)) {
		state->status = "path does not exist: " + filePath;
		return;
	}
	if (!std::filesystem::is_regular_file(fsPath) && !std::filesystem::is_directory(fsPath)) {
		state->status = "path is neither a file nor a directory: " + filePath;
		return;
	}

#ifdef __APPLE__
	std::string command = "osascript -e 'set the clipboard to POSIX file \"" + escapeForShell(filePath) + "\"'";
	runCommand(command);
#elif defined(__linux__)
	osc52copy("file://" + filePath);
#else
#error "Platform not supported"
#endif
}

Bounds pasteVisual(State *state, std::string text)
{
	std::vector<std::string> clip = splitByChar(text, '\n');
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
		std::string current = textbuffer_getLine(state, state->file->row);
		std::string prefix = current.substr(0, state->file->col);
		std::string suffix = current.substr(state->file->col);
		textbuffer_splitLine(state, state->file->row, state->file->col);
		textbuffer_insertLine(state, state->file->row + 1, suffix);
		state->file->row += 1;
		state->file->col = 0;
		indentLine(state);
	} else {
		auto pos = deleteInVisual(state);
		state->file->row = pos.row;
		state->file->col = pos.col;
	}
	Bounds bounds = { state->file->row, state->file->row, state->file->col, state->file->col };
	if (textbuffer_getLineCount(state) == 0) {
		for (uint32_t i = 0; i < clip.size(); i++) {
			textbuffer_insertLine(state, i, clip[i]);
		}
		bounds.minC = 0;
		bounds.maxC = 999999;
		bounds.maxR += clip.size();
	} else if (!text.empty() && text.back() == '\n') {
		textbuffer_insertRange(state, state->file->row, clip);
		bounds.minC = 0;
		bounds.maxC = 999999;
		bounds.maxR += clip.size();
	} else if (clip.size() > 0) {
		std::string current = textbuffer_getLine(state, state->file->row);
		std::string newContent = current.substr(0, state->file->col) + clip[0];
		textbuffer_replaceLine(state, state->file->row, newContent);
		int32_t lastRow = state->file->row;
		if (clip.size() > 1) {
			std::vector<std::string> remainingLines(clip.begin() + 1, clip.end());
			textbuffer_insertRange(state, state->file->row + 1, remainingLines);
		}
		for (int32_t i = 1; i < (int32_t)clip.size(); i++) {
			int32_t r = i + state->file->row;
			lastRow = r;
		}
		bounds.maxR = lastRow;
		std::string lastLine = textbuffer_getLine(state, lastRow);
		std::string finalLine = lastLine + safeSubstring(current, state->file->col);
		textbuffer_replaceLine(state, lastRow, finalLine);
		bounds.maxC = finalLine.length();
	}
	return bounds;
}

Bounds paste(State *state, std::string text)
{
	std::vector<std::string> clip = splitByChar(text, '\n');
	Bounds bounds = { state->file->row, state->file->row, state->file->col, state->file->col };
	fixColOverMax(state);
	if (textbuffer_getLineCount(state) == 0) {
		for (uint32_t i = 0; i < clip.size(); i++) {
			textbuffer_insertLine(state, i, clip[i]);
		}
	} else if (!text.empty() && state->pasteAsBlock) {
		for (int32_t i = 0; i < (int32_t)clip.size(); i++) {
			if (state->file->row + i >= textbuffer_getLineCount(state)) {
				textbuffer_insertLine(state, textbuffer_getLineCount(state), "");
			}
			std::string line = textbuffer_getLine(state, state->file->row + i);
			std::string front = safeSubstring(line, 0, state->file->col);
			std::string back = safeSubstring(line, state->file->col);
			std::string newContent = front + clip[i] + back;
			textbuffer_replaceLine(state, state->file->row + i, newContent);
		}
	} else if (!text.empty() && text.back() == '\n') {
		textbuffer_insertRange(state, state->file->row, clip);
		bounds.minC = 0;
		bounds.maxC = 999999;
		bounds.maxR += clip.size();
	} else if (clip.size() > 0) {
		std::string current = textbuffer_getLine(state, state->file->row);
		std::string newContent = current.substr(0, state->file->col) + clip[0];
		textbuffer_replaceLine(state, state->file->row, newContent);
		int32_t lastRow = state->file->row;
		if (clip.size() > 1) {
			std::vector<std::string> remainingLines(clip.begin() + 1, clip.end());
			textbuffer_insertRange(state, state->file->row + 1, remainingLines);
		}
		for (int32_t i = 1; i < (int32_t)clip.size(); i++) {
			int32_t r = i + state->file->row;
			lastRow = r;
		}
		bounds.maxR = lastRow;
		std::string lastLine = textbuffer_getLine(state, lastRow);
		std::string finalLine = lastLine + current.substr(state->file->col);
		textbuffer_replaceLine(state, lastRow, finalLine);
		bounds.maxC = finalLine.length();
	}
	return bounds;
}

Bounds pasteAfter(State *state, std::string text)
{
	std::vector<std::string> clip = splitByChar(text, '\n');
	Bounds bounds = { state->file->row, state->file->row, state->file->col, state->file->col };
	fixColOverMax(state);
	if (!text.empty() && state->pasteAsBlock) {
		for (int32_t i = 0; i < (int32_t)clip.size(); i++) {
			if (state->file->row + i >= textbuffer_getLineCount(state)) {
				textbuffer_insertLine(state, textbuffer_getLineCount(state), "");
			}
			std::string line = textbuffer_getLine(state, state->file->row + i);
			std::string front = safeSubstring(line, 0, state->file->col + 1);
			std::string back = safeSubstring(line, state->file->col + 1);
			std::string newContent = front + clip[i] + back;
			textbuffer_replaceLine(state, state->file->row + i, newContent);
		}
	} else if (!text.empty() && text.back() == '\n') {
		textbuffer_insertRange(state, state->file->row + 1, clip);
		bounds.minR += 1;
		bounds.minC = 0;
		bounds.maxC = 999999;
		bounds.maxR += clip.size();
	} else if (clip.size() > 0) {
		bounds.minC += 1;
		std::string current = textbuffer_getLine(state, state->file->row);
		int32_t breakCol = state->file->col;
		if (state->file->col + 1 <= current.length()) {
			breakCol = state->file->col + 1;
		}
		std::string newContent = current.substr(0, breakCol) + clip[0];
		textbuffer_replaceLine(state, state->file->row, newContent);
		int32_t lastRow = state->file->row;
		if (clip.size() > 1) {
			std::vector<std::string> remainingLines(clip.begin() + 1, clip.end());
			textbuffer_insertRange(state, state->file->row + 1, remainingLines);
		}
		for (int32_t i = 1; i < (int32_t)clip.size(); i++) {
			int32_t r = i + state->file->row;
			lastRow = r;
		}
		bounds.maxR = lastRow;
		std::string lastLine = textbuffer_getLine(state, lastRow);
		std::string finalLine = lastLine + current.substr(breakCol);
		textbuffer_replaceLine(state, lastRow, finalLine);
		bounds.maxC = finalLine.length();
	}
	return bounds;
}

void copyToClipboard(State *state, const std::string &clip, bool useSystemClipboard)
{
	state->clipboard = clip;
	if (!useSystemClipboard || state->dontRecordKey) {
		return;
	}
	osc52copy(clip);
	state->pasteAsBlock = false;
}
