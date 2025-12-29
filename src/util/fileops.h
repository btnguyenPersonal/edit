#pragma once

#include "state.h"

bool locateFile(State *state, std::string vis, std::vector<std::string> extensions);
bool locateFileAbsolute(State *state, std::string vis);
bool locateFileRelative(State *state, std::string vis, std::vector<std::string> extensions);
void locateNodeModule(State *state, std::string vis);
void createFile(State *state, std::filesystem::path path, std::string name);
void createFolder(std::filesystem::path path, std::string name);
void rename(State *state, const std::filesystem::path &oldPath, const std::string &newName);
std::filesystem::path getUniqueFilePath(const std::filesystem::path &basePath);
std::string normalizeFilename(std::string filename);
std::string minimize_filename(std::string filename);
std::string getRelativeToLastAndRoute(State *state);
std::string getExtension(const std::string &filename);
bool isLineFileRegex(std::string line);
bool matchesEditorConfigGlob(const std::string &pattern, const std::string &filepath);
std::string getRelativeToCurrent(State *state, std::string p);
