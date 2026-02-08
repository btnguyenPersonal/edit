#include "state.h"
#include "comment.h"
#include "fileops.h"
#include "keys.h"
#include "read.h"
#include "string.h"
#include "autocomplete.h"
#include "visualType.h"
#include <filesystem>
#include <fstream>
#include <map>
#include <ncurses.h>
#include <string>
#include <unistd.h>
#include <vector>

File *getFile(const std::string &name, const std::vector<std::string> &data) {
	File *file = new File();
	file->newFile = !std::filesystem::is_regular_file(name);
	file->filename = name;
	file->data = data;
	file->previousState = data;
	file->commentSymbol = getCommentSymbol(name);
	file->history = std::vector<std::vector<diffLine>>();
	file->historyPosition = -1;
	file->lastSave = -1;
	file->windowPosition.row = 0;
	file->windowPosition.col = 0;
	file->row = 0;
	file->col = 0;
	file->hardCol = 0;
	if (!file->newFile) {
		file->lastModified = std::filesystem::last_write_time(name);
	}
	calcWords(file);
	return file;
}

void State::loadAllConfigFiles() {
	if (this->file) {
		try {
			auto homeEnv = getenv("HOME");
			if (homeEnv) {
				std::filesystem::path home = std::filesystem::absolute(homeEnv);

				this->loadConfigFile(home.string() + "/.editorconfig");

				this->loadConfigFile("./.editorconfig");
			}
		} catch (const std::exception &e) {
		}
	}
}

void State::loadConfigFile(const std::string &fileLocation) {
	auto config = readFile(fileLocation);
	bool found = false;
	for (uint32_t i = 0; i < config.size(); i++) {
		if (found) {
			if (isLineFileRegex(config[i])) {
				found = false;
			} else {
				readConfigLine(config[i]);
			}
		} else if (!found && isLineFileRegex(config[i]) && matchesEditorConfigGlob(config[i], this->file->filename)) {
			found = true;
		}
	}
}

void State::readStringConfigValue(std::string &option, const std::string &configValue, const std::string &line) {
	std::string prefix = configValue + " = ";
	if (safeSubstring(line, 0, prefix.length()) == prefix) {
		option = safeSubstring(line, prefix.length());
	}
}

void State::readUintConfigValue(uint32_t &option, const std::string &configValue, const std::string &line) {
	std::string prefix = configValue + " = ";
	if (safeSubstring(line, 0, prefix.length()) == prefix) {
		option = stoi(safeSubstring(line, prefix.length()));
	}
}

void State::readBoolConfigValue(bool &option, const std::string &configValue, const std::string &line) {
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

void State::readConfigLine(const std::string &line) {
	this->readBoolConfigValue(this->options.autosave, "autosave", line);
	this->readBoolConfigValue(this->options.autoload, "autoload", line);
	this->readBoolConfigValue(this->options.wordwrap, "wordwrap", line);
	this->readBoolConfigValue(this->options.insert_final_newline, "insert_final_newline", line);
	this->readStringConfigValue(this->options.indent_style, "indent_style", line);
	this->readUintConfigValue(this->options.indent_size, "indent_size", line);
}

void State::setDefaultOptions() {
	this->options.insert_final_newline = false;
	this->options.autosave = true;
	this->options.autoload = true;
	this->options.keysSize = 30;
	this->options.maxHarpoon = 100;
	this->options.wordwrap = true;
	this->options.indent_size = 4;
	this->options.indent_style = "space";
}

void State::reloadFile(const std::string &filename) {
	Position pos = {this->file->row, this->file->col};
	auto name = normalizeFilename(filename);
	if (!std::filesystem::is_regular_file(name)) {
		this->status = "file not found: " + name;
		return;
	}
	for (uint32_t i = 0; i < this->files.size(); i++) {
		if (this->files[i]->filename == name) {
			delete this->files[i];
			this->files.erase(this->files.begin() + i);
			break;
		}
	}
	std::vector<std::string> data = readFile(name);
	File *file = getFile(name, data);
	this->files.push_back(file);
	this->currentFile = this->files.size() - 1;
	this->file = file;
	this->loadAllConfigFiles();
	this->extension = getExtension(this->file->filename);
	this->mode = NORMAL;
	this->file->row = pos.row;
	this->file->col = pos.col;
	if (!this->file->newFile) {
		this->file->lastModified = std::filesystem::last_write_time(name);
	}
}

void State::changeFile(const std::string &filename) {
	auto name = normalizeFilename(filename);
	if (!std::filesystem::is_regular_file(name)) {
		this->status = "file not found: " + name;
		return;
	}
	bool found = false;
	for (uint32_t i = 0; i < this->files.size(); i++) {
		if (this->files[i]->filename == name) {
			this->file = this->files[i];
			this->file->data = readFile(name);
			if (!this->file->newFile) {
				this->file->lastModified = std::filesystem::last_write_time(name);
			}
			found = true;
			break;
		}
	}
	if (!found) {
		// TODO keep old data, show diff of current data and if you want to load most recent file? (y/n)
		// new mode APPLYDIFF?
		std::vector<std::string> data = readFile(name);
		File *file = getFile(name, data);
		this->files.push_back(file);
		this->currentFile = this->files.size() - 1;
		this->file = file;
	}
	this->loadAllConfigFiles();
	this->extension = getExtension(this->file->filename);
	this->mode = NORMAL;
}

void State::pushFileStack(const std::string &filename) {
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

bool State::resetState(const std::string &filename) {
	auto name = normalizeFilename(filename);
	if (!std::filesystem::is_regular_file(name.c_str())) {
		this->status = "file not found: " + name;
		return false;
	}
	if (this->file) {
		this->pushFileStack(this->file->filename);
	}
	this->pushFileStack(name);
	this->fileStackIndex = this->fileStack.size() - 1;
	this->changeFile(name);
	return true;
}

void State::init() {
	this->debug = false;
	this->explorer = {};
	this->explorer.root = nullptr;
	this->explorer.open = false;
	this->explorer.size = 40;
	this->explorer.windowLine = 0;
	this->explorer.index = 0;
	this->lastMacro = 'w';
	this->skipSetHardCol = false;
	this->maxX = 80;
	this->maxY = 24;
	this->viewingDiff = false;
	this->showAllGrep = false;
	this->file = nullptr;
	this->commandLineState = {0, "", ""};
	this->currentFile = 0;
	this->runningAsRoot = geteuid() == 0;
	this->showGrep = false;
	this->diffIndex = 0;
	this->logIndex = 0;
	this->pasteAsBlock = false;
	this->prevSearch = {' ', ' '};
	this->replaceBounds = {0, 0, 0, 0};
	this->searchBackwards = false;
	this->lineNumSize = 5;
	this->buildDir = ".";
	this->prompt = "";
	this->mark = {"", 0};
	this->matching = {0, 0};
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
		this->harpoon.push_back({0, std::vector<std::string>()});
	}
	this->workspace = 0;
	this->grepOutput = std::vector<grepMatch>();
	this->findOutput = std::vector<std::filesystem::path>();
	this->visualType = SELECT;
	this->visual.row = 0;
	this->visual.col = 0;
	this->name = {std::string(""), 0, 0};
	this->search = {std::string(""), 0, 0};
	this->replace = {std::string(""), 0, 0};
	this->commandLine = {std::string(""), 0, 0};
	this->find = {std::string(""), 0, 0};
	this->grepPath = "";
	this->grep = {std::string(""), 0, 0};
	this->prevKeys = std::string("");
	this->status = std::string("");
	this->extension = std::string("");
	this->searchFail = false;
	this->mode = FIND;
	this->prevMode = NORMAL;
	this->dotCommand = std::vector<std::string>();
	this->macroCommand = std::map<char, std::vector<std::string>>();
	this->playingCommand = false;
	this->recording = {false, 'w'};
	this->selectAll = false;
	this->dontRecordKey = false;
	this->dontComputeHistory = false;
	this->fileStack = std::vector<std::string>();
	this->showFileStack = false;
	this->fileStackIndex = 0;
	this->motion = std::vector<int32_t>();
	this->frameCounter = 0;
	this->setDefaultOptions();
}

void State::init(const std::string &name, const std::vector<std::string> &data) {
	this->init();
	File *file = getFile(name, data);
	this->files.push_back(file);
	this->currentFile = this->files.size() - 1;
	this->file = file;
	this->mode = NORMAL;
	this->fileStack = {name};
	this->fileStackIndex = 0;
	this->extension = getExtension(this->file->filename);
}

State::State() {
	this->init();
}

State::~State() {
	delete this->file;

	for (auto f : this->files) {
		if (f != nullptr) {
			delete f;
		}
	}
	this->file = nullptr;

	delete this->explorer.root;
	this->explorer.root = nullptr;
}

State::State(const std::string &name, const std::vector<std::string> &data) {
	this->init(name, data);
}

State::State(const std::string &filename) {
	std::string name = normalizeFilename(filename);
	std::vector<std::string> data;
	if (!std::filesystem::is_regular_file(name.c_str())) {
		data = {""};
	} else {
		data = readFile(name);
	}
	this->init(name, data);
	this->loadAllConfigFiles();
}
