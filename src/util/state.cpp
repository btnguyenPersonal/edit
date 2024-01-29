#include "../global.h"
#include <string>
#include <vector>
#include <iostream>
#include "helper.h"
#include "state.h"
#include "modes.h"
#include "visualType.h"
#include "state.h"
#include <ncurses.h>

uint State::maxX = 0;
uint State::maxY = 0;

std::string getCommentSymbol(std::string filename) {
    if (filename == "") {
        return "";
    }
    size_t slashPosition = filename.find_last_of("/\\");
    std::string file = (slashPosition != std::string::npos) ? filename.substr(slashPosition + 1) : filename;
    size_t dotPosition = file.find_last_of(".");
    std::string extension = (dotPosition != std::string::npos && dotPosition != 0) ? file.substr(dotPosition + 1) : file;
    if (extension == "js"
        || extension == "jsx"
        || extension == "ts"
        || extension == "tsx"
        || extension == "cpp"
        || extension == "hpp"
        || extension == "c"
        || extension == "h"
        || extension == "java"
        || extension == "cs"
        || extension == "go"
        || extension == "php"
        || extension == "rs"
        || extension == "css"
        || extension == "scss"
        || extension == "vb"
        || extension == "lua"
    ) {
        return "//";
    } else if (
        extension == "py"
        || extension == "sh"
        || extension == "bash"
        || extension == "rb"
        || extension == "pl"
        || extension == "pm"
        || extension == "r"
        || extension == "yaml"
        || extension == "yml"
        || extension == "bashrc"
        || extension == "zshrc"
        || extension == "Makefile"
        || extension == "md"
        || extension == "gitignore"
        || extension == "env"
    ) {
        return "#";
    } else if (extension == "html" || extension == "xml" || extension == "xhtml" || extension == "svg") {
        return "<!--";
    } else if (extension == "sql") {
        return "--";
    } else if (extension == "lua") {
        return "--";
    } else if (extension == "json") {
        return ""; // JSON does not support comments
    } else {
        return ""; // Default case for unknown extensions
    }
}

void State::changeFile(std::string filename) {
    bool found = false;
    for (uint i = 0; i < this->archives.size(); i++) {
        if (this->archives[i].filename == this->filename) {
            this->archives[i].previousState = this->previousState;
            this->archives[i].history = this->history;
            this->archives[i].historyPosition = this->historyPosition;
            this->archives[i].windowPosition = this->windowPosition;
            this->archives[i].row = this->row;
            this->archives[i].col = this->col;
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
        });
    }
    for (uint i = 0; i < this->archives.size(); i++) {
        if (this->archives[i].filename == std::string(filename)) {
            auto archive = this->archives[i];
            this->filename = std::string(filename);
            this->data = readFile(filename);
            this->commentSymbol = getCommentSymbol(filename);
            this->previousState = archive.previousState;
            this->history = archive.history;
            this->historyPosition = archive.historyPosition;
            this->windowPosition = archive.windowPosition;
            this->row = archive.row;
            this->col = archive.col;
            this->mode = SHORTCUTS;
            return;
        }
    }
    this->filename = std::string(filename);
    this->data = readFile(filename);
    this->commentSymbol = getCommentSymbol(filename);
    this->previousState = std::vector<std::string>();
    this->history = std::vector<std::vector<diffLine>>();
    this->historyPosition = -1;
    this->windowPosition.row = 0;
    this->windowPosition.col = 0;
    this->visualType = NORMAL;
    this->visual.row = 0;
    this->visual.col = 0;
    this->row = 0;
    this->col = 0;
    this->indent = 4;
    this->commandLineQuery = std::string("");
    this->prevKeys = std::string("");
    this->status = std::string("");
    this->motionComplete = false;
    this->motion = std::string("");
    this->mode = SHORTCUTS;
}

void State::pushFileStack(std::string filename) {
    for (auto it = this->fileStack.begin(); it != this->fileStack.end(); ) {
        if (*it == filename) {
            it = this->fileStack.erase(it);
        } else {
            ++it;
        }
    }
    this->fileStack.push_back(filename);
}

void State::resetState(std::string filename) {
    this->pushFileStack(this->filename);
    this->pushFileStack(filename);
    this->fileStackIndex = this->fileStack.size() - 1;
    if (!std::filesystem::exists(filename.c_str())) {
        this->status = "file not found: " + filename;
        exit(1);
    }
    this->changeFile(filename);
}

State::State() {
    this->searching = false;
    this->replacing = false;
    this->harpoonFiles = std::vector<std::string>();
    this->harpoonIndex = 0;
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
    this->indent = 4;
    this->searchQuery = std::string("");
    this->replaceQuery = std::string("");
    this->commandLineQuery = std::string("");
    this->findFileQuery = std::string("");
    this->findFileSelection = 0;
    this->grepQuery = std::string("");
    this->grepSelection = 0;
    this->prevKeys = std::string("");
    this->status = std::string("");
    this->mode = FINDFILE;
    this->dotCommand = std::string("");
    this->macroCommand = std::string("");
    this->playingCommand = false;
    this->recording = false;
    this->dontRecordKey = false;
    this->fileStack = std::vector<std::string>();
    this->showFileStack = false;
    this->fileStackIndex = 0;
    this->motionComplete = false;
    this->motion = std::string("");
}

State::State(std::string filename) : State() {
    if (!std::filesystem::exists(filename.c_str())) {
        endwin();
        std::cout << "file not found: " << filename << std::endl;
        exit(1);
    }
    this->filename = std::string(filename);
    this->data = readFile(filename.c_str());
    this->commentSymbol = getCommentSymbol(filename);
    this->mode = SHORTCUTS;
    this->fileStack = {filename};
    this->fileStackIndex = 0;
}

void State::setMaxYX(int y, int x) {
    State::maxY = (uint) y;
    State::maxX = (uint) x;
}
