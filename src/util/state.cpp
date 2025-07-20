#include "state.h"
#include "helper.h"
#include "modes.h"
#include "visualType.h"
#include "keys.h"
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

void printVec(std::ofstream &out, std::vector<std::string> vec, std::string name)
{
	if (vec.size() == 0) {
		out << name << " []" << std::endl;
		return;
	}
	out << name << " [" << std::endl;
	for (uint32_t i = 0; i < vec.size(); i++) {
		out << "    " << vec[i] << std::endl;
	}
	out << "]" << std::endl;
}

void printVecChar(std::ofstream &out, std::vector<int32_t> vec, std::string name)
{
	if (vec.size() == 0) {
		out << name << " []" << std::endl;
		return;
	}
	out << name << " [" << std::endl;
	for (uint32_t i = 0; i < vec.size(); i++) {
		out << "    " << getEscapedChar(vec[i]) << std::endl;
	}
	out << "]" << std::endl;
}

void printVecKeys(std::ofstream &out, std::vector<ModeKey> vec, std::string name)
{
	if (vec.size() == 0) {
		out << name << " []" << std::endl;
		return;
	}
	out << name << " [" << std::endl;
	for (uint32_t i = 0; i < vec.size(); i++) {
		out << "    { key: " << vec[i].key << " mode: " << getMode(vec[i].mode) << " }" << std::endl;
	}
	out << "]" << std::endl;
}

void printVecPath(std::ofstream &out, std::vector<std::filesystem::path> vec, std::string name)
{
	if (vec.size() == 0) {
		out << name << " []" << std::endl;
		return;
	}
	out << name << " [" << std::endl;
	for (uint32_t i = 0; i < vec.size(); i++) {
		out << "    " << vec[i].string() << std::endl;
	}
	out << "]" << std::endl;
}

void printHistory(std::ofstream &out, std::vector<std::vector<diffLine> > vec, std::string name)
{
	if (vec.size() == 0) {
		out << name << " []" << std::endl;
		return;
	}
	out << name << " [" << std::endl;
	for (uint32_t i = 0; i < vec.size(); i++) {
		out << "    [" << std::endl;
		for (uint32_t j = 0; j < vec[i].size(); j++) {
			out << "        " << std::to_string(vec[i][j].lineNum) << " "
			    << std::string(vec[i][j].add ? "+" : "-") << vec[i][j].line << std::endl;
		}
		out << "    ]" << std::endl;
	}
	out << "]" << std::endl;
}

void printGrepOutput(std::ofstream &out, std::vector<grepMatch> vec, std::string name)
{
	if (vec.size() == 0) {
		out << name << " []" << std::endl;
		return;
	}
	out << name << " [" << std::endl;
	for (uint32_t i = 0; i < vec.size(); i++) {
		out << "    {" << std::endl;
		out << "        " << vec[i].path.string() << ":" << std::to_string(vec[i].lineNum) << std::endl;
		out << "        " << vec[i].line << std::endl;
		out << "    }" << std::endl;
	}
	out << "]" << std::endl;
}

void printQuery(std::ofstream &out, Query q, std::string name)
{
	out << name << " {" << std::endl;
	out << "    " << q.query << std::endl;
	out << "    " << std::to_string(q.cursor) << std::endl;
	out << "    " << std::to_string(q.selection) << std::endl;
	out << "}" << std::endl;
}

void State::print(std::string filename, bool printGrep, bool printArchives)
{
	std::ofstream out(filename);
	if (!out.is_open()) {
		this->status = "Error printing to file";
	}
	// FileExplorerNode *fileExplorer;
	// Jumplist jumplist;
	// Mark mark;
	// Mode mode;
	// Options options;
	out << "matching.row " << std::to_string(this->matching.row) << std::endl;
	out << "matching.col " << std::to_string(this->matching.col) << std::endl;
	printQuery(out, this->commandLine, "commandLine");
	printQuery(out, this->findFile, "findFile");
	printQuery(out, this->grep, "grep");
	printQuery(out, this->name, "name");
	printQuery(out, this->replace, "replace");
	printQuery(out, this->search, "search");
	out << "visualType " << this->visualType << std::endl;
	out << "dontRecordKey " << this->dontRecordKey << std::endl;
	out << "dontSave " << this->dontSave << std::endl;
	out << "fileExplorerOpen " << this->fileExplorerOpen << std::endl;
	out << "playingCommand " << this->playingCommand << std::endl;
	out << "recording " << this->recording << std::endl;
	out << "replacing " << this->replacing << std::endl;
	out << "runningAsRoot " << this->runningAsRoot << std::endl;
	out << "searchFail " << this->searchFail << std::endl;
	out << "searching " << this->searching << std::endl;
	out << "selectAll " << this->selectAll << std::endl;
	out << "showAllGrep " << this->showAllGrep << std::endl;
	out << "showFileStack " << this->showFileStack << std::endl;
	out << "showGrep " << this->showGrep << std::endl;
	out << "blameSize " << this->blameSize << std::endl;
	out << "buildErrorIndex " << this->buildErrorIndex << std::endl;
	out << "fileExplorerIndex " << this->fileExplorerIndex << std::endl;
	out << "fileExplorerWindowLine " << this->fileExplorerWindowLine << std::endl;
	out << "historyPosition " << this->historyPosition << std::endl;
	out << "lastSave " << this->lastSave << std::endl;
	out << "maxY " << this->maxY << std::endl;
	out << "maxX " << this->maxX << std::endl;
	// TODO std::map<uint32_t, std::string> harpoonFiles;
	out << "buildDir " << this->buildDir << std::endl;
	out << "commentSymbol " << this->commentSymbol << std::endl;
	out << "filename " << this->filename << std::endl;
	out << "grepPath " << this->grepPath << std::endl;
	printVecKeys(out, this->keys, std::string("keys"));
	out << "prevKeys " << this->prevKeys << std::endl;
	out << "status " << this->status << std::endl;
	if (printArchives) {
		// TODO
		// printArchives(out, this->archives, std::string("archives"));
	}
	if (printGrep) {
		printGrepOutput(out, this->grepOutput, std::string("grepOutput"));
	}
	printVecChar(out, this->motion, std::string("motion"));
	printVecPath(out, this->findFileOutput, std::string("findFileOutput"));
	printVec(out, this->blame, std::string("blame"));
	printVec(out, this->data, std::string("data"));
	printVec(out, this->dotCommand, std::string("dotCommand"));
	printVec(out, this->fileStack, std::string("fileStack"));
	printVec(out, this->macroCommand, std::string("macroCommand"));
	printVec(out, this->previousState, std::string("previousState"));
	printHistory(out, this->history, std::string("history"));
	out << "visual.row " << std::to_string(this->visual.row) << std::endl;
	out << "visual.col " << std::to_string(this->visual.col) << std::endl;
	out << "windowPosition.row " << std::to_string(this->windowPosition.row) << std::endl;
	out << "windowPosition.col " << std::to_string(this->windowPosition.col) << std::endl;
	out << "col " << std::to_string(this->col) << std::endl;
	out << "fileExplorerSize " << std::to_string(this->fileExplorerSize) << std::endl;
	out << "fileStackIndex " << std::to_string(this->fileStackIndex) << std::endl;
	out << "harpoonIndex " << std::to_string(this->harpoonIndex) << std::endl;
	out << "lineNumSize " << std::to_string(this->lineNumSize) << std::endl;
	out << "row " << std::to_string(this->row) << std::endl;
	out.close();
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
			this->archives[i].lastSave = this->lastSave;
			this->archives[i].windowPosition = this->windowPosition;
			this->archives[i].row = this->row;
			this->archives[i].col = this->col;
			this->archives[i].hardCol = this->hardCol;
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
			this->lastSave,
			this->windowPosition,
			this->row,
			this->col,
			this->hardCol,
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
			this->diffIndex = historyPosition;
			this->lastSave = archive.lastSave;
			this->windowPosition = archive.windowPosition;
			this->row = archive.row;
			this->col = archive.col;
			this->hardCol = archive.hardCol;
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
	this->viewingDiff = false;
	this->diffIndex = 0;
	this->historyPosition = -1;
	this->lastSave = -1;
	this->windowPosition.row = 0;
	this->windowPosition.col = 0;
	this->visualType = NORMAL;
	this->visual.row = 0;
	this->visual.col = 0;
	this->row = 0;
	this->col = 0;
	this->hardCol = 0;
	this->skipSetHardCol = 0;
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
	this->pasteAsBlock = false;
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
	this->blameSize = 65;
	this->blame = std::vector<std::string>();
	this->harpoonFiles = std::map<uint32_t, std::string>();
	this->harpoonIndex = 0;
	this->harpoonPageSize = 7;
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
	this->hardCol = 0;
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
