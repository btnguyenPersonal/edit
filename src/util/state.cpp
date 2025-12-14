#include "state.h"
#include "helper.h"
#include "modes.h"
#include "visualType.h"
#include "keys.h"
#include "read.h"
#include <iostream>
#include <ncurses.h>
#include <string>
#include <filesystem>
#include <vector>
#include <map>
#include <unistd.h>
#include <fstream>

uint32_t State::maxX = 0;
uint32_t State::maxY = 0;

File *getFile(std::string name, std::vector<std::string> data) {
	File *file = new File();
	file->filename = name;
	file->data = data;
	file->previousState = data;
	file->commentSymbol = getCommentSymbol(name);
	file->history = std::vector<std::vector<diffLine> >();
	file->historyPosition = -1;
	file->lastSave = -1;
	file->windowPosition.row = 0;
	file->windowPosition.col = 0;
	file->row = 0;
	file->col = 0;
	file->hardCol = 0;
	return file;
}

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
		} else if (!found && isLineFileRegex(config[i]) && this->file && matchesEditorConfigGlob(config[i], this->file->filename)) {
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
	this->options.keysSize = 30;
	this->options.maxHarpoon = 100;
	this->options.wordwrap = true;
	this->options.indent_size = 4;
	this->options.indent_style = "space";
}

void State::changeFile(std::string filename)
{
	auto name = normalizeFilename(filename);
	if (!std::filesystem::is_regular_file(name)) {
		this->status = "file not found: " + name;
		return;
	}
	bool found = false;
	for (uint32_t i = 0; i < this->files.size(); i++) {
		if (this->files[i]->filename == name) {
			this->file = this->files[i];
			found = true;
			break;
		}
	}
	if (!found) {
		std::vector<std::string> data = readFile(name);
		File *file = getFile(name, data);
		this->files.push_back(file);
		this->currentFile = this->files.size() - 1;
		this->file = file;
	}
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
	auto name = normalizeFilename(filename);
	if (!std::filesystem::is_regular_file(name.c_str())) {
		this->status = "file not found: " + name;
		return false;
	}
	this->pushFileStack(this->file->filename);
	this->pushFileStack(name);
	this->fileStackIndex = this->fileStack.size() - 1;
	this->changeFile(name);
	return true;
}

void State::init()
{
	this->currentFile = 0;
	this->runningAsRoot = geteuid() == 0;
	this->showGrep = false;
	this->diffIndex = 0;
	this->logIndex = 0;
	this->fileExplorer = new FileExplorerNode(std::filesystem::current_path());
	this->fileExplorer->open();
	this->fileExplorerOpen = false;
	this->pasteAsBlock = false;
	this->lineNumSize = 5;
	this->fileExplorerSize = 40;
	this->fileExplorerWindowLine = 0;
	this->fileExplorerIndex = 0;
	this->buildDir = ".";
	this->prompt = "";
	this->mark = { "", 0 };
	this->matching = { 0, 0 };
	this->searching = false;
	this->dontSave = true;
	this->replacing = false;
	this->buildErrorIndex = 0;
	this->blameSize = 65;
	this->diffLines = std::vector<std::string>();
	this->logLines = std::vector<std::string>();
	this->blame = std::vector<std::string>();
	this->harpoon = std::vector<Harpoon>();
	for (uint32_t i = 0; i < 10; i++) {
		this->harpoon.push_back({ 0, std::vector<std::string>() });
	}
	this->workspace = 0;
	this->grepOutput = std::vector<grepMatch>();
	this->findOutput = std::vector<std::filesystem::path>();
	this->visualType = SELECT;
	this->visual.row = 0;
	this->visual.col = 0;
	this->search = { std::string(""), 0, 0 };
	this->replace = { std::string(""), 0, 0 };
	this->commandLine = { std::string(""), 0, 0 };
	this->find = { std::string(""), 0, 0 };
	this->grepPath = "";
	this->grep = { std::string(""), 0, 0 };
	this->prevKeys = std::string("");
	this->status = std::string("");
	this->searchFail = false;
	this->mode = FIND;
	this->dotCommand = std::vector<std::string>();
	this->macroCommand = std::vector<std::string>();
	this->playingCommand = false;
	this->recording = false;
	this->selectAll = false;
	this->dontRecordKey = false;
	this->fileStack = std::vector<std::string>();
	this->showFileStack = false;
	this->fileStackIndex = 0;
	this->motion = std::vector<int32_t>();
	this->setDefaultOptions();
	this->loadAllConfigFiles();
}

void State::init(std::string name, std::vector<std::string> data)
{
	this->init();
	File *file = getFile(name, data);
	this->files.push_back(file);
	this->currentFile = this->files.size() - 1;
	this->file = file;
	this->mode = NORMAL;
	this->fileStack = { name };
	this->fileStackIndex = 0;
}

State::State()
{
	this->init();
}

State::State(std::string name, std::vector<std::string> data)
{
	this->init(name, data);
}

State::State(std::string filename)
{
	std::string name = normalizeFilename(filename);
	std::vector<std::string> data;
	if (!std::filesystem::is_regular_file(name.c_str())) {
		data = { "" };
	} else {
		data = readFile(name.c_str());
	}
	this->init(name, data);
}

void State::setMaxYX(int32_t y, int32_t x)
{
	State::maxY = (uint32_t)y;
	State::maxX = (uint32_t)x;
}
