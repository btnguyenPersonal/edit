#pragma once

#include "modes.h"
#include "visualType.h"
#include "fileExplorerNode.h"
#include <filesystem>
#include <string>
#include <vector>
#include <mutex>

struct buildError {
	std::string filename;
	int32_t row;
	int32_t col;
	std::string msg;
};

struct grepMatch {
	std::filesystem::path path;
	int32_t lineNum;
	std::string line;

	grepMatch(const std::filesystem::path &p, int32_t num, const std::string &l)
		: path(p)
		, lineNum(num)
		, line(l)
	{
	}
};

struct diffLine {
	uint32_t lineNum;
	bool add; // true for add, false for delete
	std::string line;
};

struct ModeKey {
	Mode mode;
	std::string key;
};

struct Position {
	uint32_t row;
	uint32_t col;
};

struct WordPosition {
	uint32_t min;
	uint32_t max;
};

struct Mark {
	std::string filename;
	uint32_t mark;
};

struct Harpoon {
	uint32_t index;
	std::vector<std::string> list;
};

struct Jumplist {
	bool touched;
	uint32_t index;
	std::vector<Position> list;
};

struct Archive {
	std::string filename;
	std::vector<std::string> previousState;
	std::vector<std::vector<diffLine> > history;
	int32_t historyPosition;
	int32_t lastSave;
	struct Position windowPosition;
	uint32_t row;
	uint32_t col;
	uint32_t hardCol;
	Jumplist jumplist;
};

struct Query {
	std::string query;
	uint32_t cursor;
	uint32_t selection;
};

struct Options {
	bool wordwrap;
	bool autosave;
	uint32_t keysSize;
	int32_t maxHarpoon;
	bool insert_final_newline;
	uint32_t indent_size;
	std::string indent_style;
};

class State {
    public:
	std::mutex findMutex;
	std::mutex grepMutex;
	FileExplorerNode *fileExplorer;
	Jumplist jumplist;
	Mark mark;
	Mode mode;
	Options options;
	Position matching;
	Query commandLine;
	Query find;
	Query grep;
	Query name;
	Query replace;
	Query search;
	VisualType visualType;
	bool pasteAsBlock;
	bool dontRecordKey;
	bool dontSave;
	bool fileExplorerOpen;
	bool playingCommand;
	bool recording;
	bool replacing;
	bool runningAsRoot;
	bool searchFail;
	bool searching;
	bool selectAll;
	bool showAllGrep;
	bool showFileStack;
	bool showGrep;
	bool skipSetHardCol;
	bool viewingDiff;
	int32_t blameSize;
	int32_t lastModifiedDate;
	int32_t buildErrorIndex;
	int32_t fileExplorerIndex;
	int32_t fileExplorerWindowLine;
	int32_t historyPosition;
	int32_t lastSave;
	static uint32_t maxX;
	static uint32_t maxY;
	std::string prompt;
	std::string buildDir;
	std::string commentSymbol;
	std::string filename;
	std::string grepPath;
	std::string prevKeys;
	std::string status;
	std::vector<Archive> archives;
	std::vector<grepMatch> grepOutput;
	std::vector<int32_t> motion;
	std::vector<std::filesystem::path> findOutput;
	std::vector<std::string> diffLines;
	std::vector<std::string> logLines;
	std::vector<std::string> blame;
	std::vector<std::string> data;
	std::vector<std::string> dotCommand;
	std::vector<std::string> fileStack;
	std::vector<std::string> macroCommand;
	std::vector<std::string> previousState;
	std::vector<std::vector<diffLine> > history;
	std::vector<Harpoon> harpoon;
	std::vector<ModeKey> keys;
	struct Position visual;
	struct Position windowPosition;
	uint32_t hardCol;
	uint32_t col;
	uint32_t fileExplorerSize;
	uint32_t fileStackIndex;
	uint32_t lineNumSize;
	uint32_t row;
	uint32_t workspace;
	uint32_t logIndex;
	uint32_t diffIndex;
	State();
	State(std::string filename);
	bool resetState(std::string filename);
	void pushFileStack(std::string filename);
	void changeFile(std::string filename);
	void readStringConfigValue(std::string &option, std::string configValue, std::string line);
	void readUintConfigValue(uint32_t &option, std::string configValue, std::string line);
	void readBoolConfigValue(bool &option, std::string configValue, std::string line);
	void loadAllConfigFiles();
	void loadConfigFile(std::string fileLocation);
	void readConfigLine(std::string optionLine);
	void setDefaultOptions();
	void print(std::string filename, bool printGrep, bool printArchives);
	static void setMaxYX(int32_t y, int32_t x);
};
