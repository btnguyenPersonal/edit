#include "fileops.h"
#include "string.h"
#include "textedit.h"
#include <fstream>
#include <regex>

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

std::string normalizeFilename(std::string filename)
{
	std::string current_path = std::filesystem::current_path().string() + "/";
	if (filename.substr(0, current_path.length()) == current_path) {
		return filename.substr(current_path.length());
	} else {
		return filename;
	}
}

std::string minimize_filename(std::string filename)
{
	std::vector<std::string> parts = splitByChar(filename, '/');
	std::string minimized;
	for (size_t i = 0; i < parts.size() - 1; ++i) {
		if (!parts[i].empty()) {
			minimized += parts[i][0];
			minimized += '/';
		}
	}
	minimized += parts.back();
	return minimized;
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

bool isLineFileRegex(const std::string &line)
{
	return line.front() == '[' && line.back() == ']';
}

bool matchesEditorConfigGlob(const std::string &pattern, const std::string &filepath)
{
	// TODO somehow do the real glob matching instead of this regex hack
	auto cleanPattern = safeSubstring(pattern, 1, pattern.length() - 2);
	cleanPattern = replace(cleanPattern, ",", "|");
	cleanPattern = replace(cleanPattern, ".", "\\.");
	cleanPattern = replace(cleanPattern, "*", ".*");
	cleanPattern = replace(cleanPattern, "{", "(");
	cleanPattern = replace(cleanPattern, "}", ")");

	try {
		return std::regex_search(filepath, std::regex(cleanPattern));
	} catch (const std::exception &e) {
		return false;
	}
}
