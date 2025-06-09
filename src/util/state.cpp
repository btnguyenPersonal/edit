#include "state.h"
#include "helper.h"
#include "modes.h"
#include "visualType.h"
#include <iostream>
#include <ncurses.h>
#include <string>
#include <filesystem>
#include <vector>
#include <map>
#include <unistd.h>

uint32_t State::maxX = 0;
uint32_t State::maxY = 0;

void State::loadAllConfigFiles()
{
	std::filesystem::path home = std::filesystem::absolute(getenv("HOME"));
	std::filesystem::path current = std::filesystem::absolute(std::filesystem::current_path());

	this->loadConfigFile(home.string() + "/.editorconfig");

	std::vector<std::filesystem::path> dirs;
	for (std::filesystem::path p = current; p != home && p != p.parent_path(); p = p.parent_path()) {
		dirs.push_back(p);
	}

	for (auto it = dirs.rbegin(); it != dirs.rend(); ++it) {
		this->loadConfigFile(it->string() + "/.editorconfig");
	}

	this->loadConfigFile("./.editorconfig");
}

void State::loadConfigFile(std::string fileLocation)
{
	auto config = readFile(fileLocation.c_str());
	bool found = false;
	for (uint32_t i = 0; i < config.size(); i++) {
		if (found) {
			if (isLineFileRegex(config[i])) {
				found = false;
			} else {
				readConfigLine(config[i]);
			}
		} else if (!found && isLineFileRegex(config[i]) && matchesEditorConfigGlob(config[i], this->filename)) {
			found = true;
		}
	}
}

void State::readStringConfigValue(std::string &option, std::string configValue, std::string line)
{
	std::string prefix = configValue + " = ";
	if (safeSubstring(line, 0, prefix.length()) == prefix) {
		option = safeSubstring(line, prefix.length());
	}
}

void State::readUintConfigValue(uint32_t &option, std::string configValue, std::string line)
{
	std::string prefix = configValue + " = ";
	if (safeSubstring(line, 0, prefix.length()) == prefix) {
		option = stoi(safeSubstring(line, prefix.length()));
	}
}

void State::readBoolConfigValue(bool &option, std::string configValue, std::string line)
{
	std::string prefix = configValue + " = ";
	if (safeSubstring(line, 0, prefix.length()) == prefix) {
		std::string s = safeSubstring(line, prefix.length());
		if (s == "true") {
			option = true;
		} else if (s == "false") {
			option = false;
		}
	}
}

void State::readConfigLine(std::string line)
{
	this->readBoolConfigValue(this->options.autosave, "autosave", line);
	this->readBoolConfigValue(this->options.wordwrap, "wordwrap", line);
	this->readBoolConfigValue(this->options.insert_final_newline, "insert_final_newline", line);
	this->readStringConfigValue(this->options.indent_style, "indent_style", line);
	this->readUintConfigValue(this->options.indent_size, "indent_size", line);
}

void State::setDefaultOptions()
{
	this->options.insert_final_newline = false;
	this->options.autosave = true;
	this->options.maxHarpoon = 100;
	this->options.wordwrap = true;
	this->options.indent_size = 4;
	this->options.indent_style = "space";
}

void State::changeFile(std::string filename)
{
	auto normalizedFilename = normalizeFilename(filename);
	if (!std::filesystem::is_regular_file(normalizedFilename)) {
		this->status = "file not found: " + normalizedFilename;
		return;
	}
	bool found = false;
	for (uint32_t i = 0; i < this->archives.size(); i++) {
		if (this->archives[i].filename == this->filename) {
			this->archives[i].previousState = this->previousState;
			this->archives[i].history = this->history;
			this->archives[i].historyPosition = this->historyPosition;
			this->archives[i].windowPosition = this->windowPosition;
			this->archives[i].row = this->row;
			this->archives[i].col = this->col;
			this->archives[i].jumplist = this->jumplist;
			found = true;
			break;
		}
	}
	if (!found) {
		this->archives.push_back({
			this->filename,
			this->previousState,
			this->history,
			this->historyPosition,
			this->windowPosition,
			this->row,
			this->col,
			this->jumplist,
		});
	}
	for (uint32_t i = 0; i < this->archives.size(); i++) {
		if (this->archives[i].filename == normalizedFilename) {
			auto archive = this->archives[i];
			this->filename = normalizedFilename;
			this->data = readFile(normalizedFilename);
			this->commentSymbol = getCommentSymbol(normalizedFilename);
			this->previousState = archive.previousState;
			this->history = archive.history;
			this->historyPosition = archive.historyPosition;
			this->windowPosition = archive.windowPosition;
			this->row = archive.row;
			this->col = archive.col;
			this->jumplist = archive.jumplist;
			this->mode = SHORTCUTS;
			return;
		}
	}
	auto data = readFile(normalizedFilename);
	this->filename = normalizedFilename;
	this->data = data;
	this->previousState = data;
	this->commentSymbol = getCommentSymbol(normalizedFilename);
	this->history = std::vector<std::vector<diffLine> >();
	this->historyPosition = -1;
	this->windowPosition.row = 0;
	this->windowPosition.col = 0;
	this->visualType = NORMAL;
	this->visual.row = 0;
	this->visual.col = 0;
	this->row = 0;
	this->col = 0;
	this->commandLine.query = std::string("");
	this->selectAll = false;
	this->showAllGrep = false;
	this->prevKeys = std::string("");
	this->status = std::string("");
	this->searchFail = false;
	this->motion = std::vector<int32_t>();
	this->mode = SHORTCUTS;
	refocusFileExplorer(this, false);
	this->loadAllConfigFiles();
}

void State::pushFileStack(std::string filename)
{
	if (filename != "") {
		for (auto it = this->fileStack.begin(); it != this->fileStack.end();) {
			if (*it == filename) {
				it = this->fileStack.erase(it);
			} else {
				++it;
			}
		}
		this->fileStack.push_back(filename);
	}
}

bool State::resetState(std::string filename)
{
	auto normalizedFilename = normalizeFilename(filename);
	if (!std::filesystem::is_regular_file(normalizedFilename.c_str())) {
		this->status = "file not found: " + normalizedFilename;
		return false;
	}
	this->pushFileStack(this->filename);
	this->pushFileStack(normalizedFilename);
	this->fileStackIndex = this->fileStack.size() - 1;
	this->changeFile(normalizedFilename);
	return true;
}

State::State()
{
	this->runningAsRoot = geteuid() == 0;
	this->showGrep = false;
	this->fileExplorer = new FileExplorerNode(std::filesystem::current_path());
	this->fileExplorer->open();
	this->fileExplorerOpen = false;
	this->lineNumSize = 5;
	this->fileExplorerSize = 40;
	this->fileExplorerWindowLine = 0;
	this->fileExplorerIndex = 0;
	this->buildDir = ".";
	this->mark = { "", 0 };
	this->matching = { 0, 0 };
	this->searching = false;
	this->dontSave = false;
	this->replacing = false;
	this->jumplist = { 0, std::vector<Position>() };
	this->buildErrorIndex = 0;
	this->buildErrors = std::vector<std::string>();
	this->blameSize = 65;
	this->blame = std::vector<std::string>();
	this->harpoonFiles = std::map<uint32_t, std::string>();
	this->harpoonIndex = 0;
	this->data = std::vector<std::string>();
	this->previousState = std::vector<std::string>();
	this->history = std::vector<std::vector<diffLine> >();
	this->grepOutput = std::vector<grepMatch>();
	this->findFileOutput = std::vector<std::filesystem::path>();
	this->historyPosition = -1;
	this->lastSave = -1;
	this->windowPosition.row = 0;
	this->windowPosition.col = 0;
	this->visualType = NORMAL;
	this->visual.row = 0;
	this->visual.col = 0;
	this->row = 0;
	this->col = 0;
	this->search = { std::string(""), 0, 0 };
	this->replace = { std::string(""), 0, 0 };
	this->commandLine = { std::string(""), 0, 0 };
	this->findFile = { std::string(""), 0, 0 };
	this->grepPath = "";
	this->grep = { std::string(""), 0, 0 };
	this->prevKeys = std::string("");
	this->status = std::string("");
	this->searchFail = false;
	this->mode = FINDFILE;
	this->dotCommand = std::vector<std::string>();
	this->macroCommand = std::vector<std::string>();
	this->playingCommand = false;
	this->recording = false;
	this->selectAll = false;
	this->dontRecordKey = false;
	this->fileStack = std::vector<std::string>();
	this->showFileStack = false;
	this->fileStackIndex = 0;
	this->lastLoggingVar = std::string("");
	this->motion = std::vector<int32_t>();
	this->setDefaultOptions();
	this->loadAllConfigFiles();
}

State::State(std::string filename)
	: State()
{
	auto normalizedFilename = normalizeFilename(filename);
	std::vector<std::string> data;
	if (!std::filesystem::is_regular_file(normalizedFilename.c_str())) {
		data = { "" };
	} else {
		data = readFile(normalizedFilename.c_str());
	}
	this->filename = std::string(normalizedFilename);
	this->data = data;
	this->previousState = data;
	this->commentSymbol = getCommentSymbol(normalizedFilename);
	this->mode = SHORTCUTS;
	this->fileStack = { normalizedFilename };
	this->fileStackIndex = 0;
	refocusFileExplorer(this, false);
}

void State::setMaxYX(int32_t y, int32_t x)
{
	State::maxY = (uint32_t)y;
	State::maxX = (uint32_t)x;
}
