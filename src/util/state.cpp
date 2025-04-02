#include "state.h"
#include "helper.h"
#include "modes.h"
#include "visualType.h"
#include <iostream>
#include <ncurses.h>
#include <string>
#include <vector>

uint32_t State::maxX = 0;
uint32_t State::maxY = 0;

void State::loadConfigFile(std::string fileLocation) {
    auto config = readFile(fileLocation.c_str());
    for (uint32_t i = 0; i < config.size(); i++) {
        readConfigLine(config[i]);
    }
}

void State::readConfigLine(std::string optionLine) {
    if (optionLine == "wordwrap") {
        this->options.wordwrap = true;
    } else if (optionLine == "harpoonNum") {
        this->options.harpoonNum = true;
    } else if (optionLine == "autosave") {
        this->options.autosave = true;
    } else if (safeSubstring(optionLine, 0, std::string("indent_size ").length()) == "indent_size ") {
        this->options.indent = std::stoi(safeSubstring(optionLine, std::string("indent_size ").length()));
    }
}

void State::setDefaultOptions() {
    this->options.harpoonNum = false;
    this->options.autosave = false;
    this->options.wordwrap = false;
    this->options.indent = 4;
}

void State::changeFile(std::string filename) {
    auto normalizedFilename = normalizeFilename(filename);
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
    this->history = std::vector<std::vector<diffLine>>();
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
    this->prevKeys = std::string("");
    this->status = std::string("");
    this->searchFail = false;
    this->motion = std::string("");
    this->mode = SHORTCUTS;
    refocusFileExplorer(this, false);
}

void State::pushFileStack(std::string filename) {
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

void State::resetState(std::string filename) {
    auto normalizedFilename = normalizeFilename(filename);
    this->pushFileStack(this->filename);
    this->pushFileStack(normalizedFilename);
    this->fileStackIndex = this->fileStack.size() - 1;
    if (!std::filesystem::is_regular_file(normalizedFilename.c_str())) {
        this->status = "file not found: " + normalizedFilename;
        exit(1);
    }
    this->changeFile(normalizedFilename);
}

State::State() {
    this->showGrep = false;
    this->fileExplorer = new FileExplorerNode(std::filesystem::current_path());
    this->fileExplorer->open();
    this->fileExplorerOpen = false;
    this->fileExplorerSize = 40;
    this->fileExplorerWindowLine = 0;
    this->fileExplorerIndex = 0;
    this->buildDir = ".";
    this->mark = {"", 0};
    this->matching = {0, 0};
    this->searching = false;
    this->dontSave = false;
    this->replacing = false;
    this->jumplist = {0, std::vector<Position>()};
    this->buildErrorIndex = 0;
    this->buildErrors = std::vector<std::string>();
    this->blame = std::vector<std::string>();
    this->harpoonFiles = std::vector<std::string>();
    this->harpoonIndex = 0;
    this->data = std::vector<std::string>();
    this->previousState = std::vector<std::string>();
    this->history = std::vector<std::vector<diffLine>>();
    this->grepOutput = std::vector<grepMatch>();
    this->findFileOutput = std::vector<std::filesystem::path>();
    this->historyPosition = -1;
    this->windowPosition.row = 0;
    this->windowPosition.col = 0;
    this->visualType = NORMAL;
    this->visual.row = 0;
    this->visual.col = 0;
    this->row = 0;
    this->col = 0;
    this->search = {std::string(""), 0, 0};
    this->replace = {std::string(""), 0, 0};
    this->commandLine = {std::string(""), 0, 0};
    this->findFile = {std::string(""), 0, 0};
    this->grep = {std::string(""), 0, 0};
    this->prevKeys = std::string("");
    this->status = std::string("");
    this->searchFail = false;
    this->mode = FINDFILE;
    this->dotCommand = std::string("");
    this->macroCommand = std::string("");
    this->playingCommand = false;
    this->recording = false;
    this->selectAll = false;
    this->dontRecordKey = false;
    this->fileStack = std::vector<std::string>();
    this->showFileStack = false;
    this->fileStackIndex = 0;
    this->lastLoggingVar = std::string("");
    this->motion = std::string("");
    this->setDefaultOptions();
    this->loadConfigFile(std::string(getenv("HOME")) + "/.editorconfig");
}

State::State(std::string filename) : State() {
    auto normalizedFilename = normalizeFilename(filename);
    this->unsavedFile = !std::filesystem::is_regular_file(normalizedFilename.c_str());
    std::vector<std::string> data;
    if (this->unsavedFile) {
        data = {""};
    } else {
        data = readFile(normalizedFilename.c_str());
    }
    this->filename = std::string(normalizedFilename);
    this->data = data;
    this->previousState = data;
    this->commentSymbol = getCommentSymbol(normalizedFilename);
    this->mode = SHORTCUTS;
    this->fileStack = {normalizedFilename};
    this->fileStackIndex = 0;
    refocusFileExplorer(this, false);
}

void State::setMaxYX(int32_t y, int32_t x) {
    State::maxY = (uint32_t)y;
    State::maxX = (uint32_t)x;
}
