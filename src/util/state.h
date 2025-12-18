#pragma once

#include "modes.h"
#include "visualType.h"
#include "bounds.h"
#include "fileExplorerNode.h"
#include <filesystem>
#include <string>
#include <vector>
#include <mutex>
#include <map>

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

struct File {
	Jumplist jumplist;
	int32_t historyPosition;
	int32_t lastSave;
	std::string commentSymbol;
	std::string filename;
	std::vector<std::string> data;
	std::vector<std::string> previousState;
	std::vector<std::vector<diffLine> > history;
	struct Position windowPosition;
	uint32_t col;
	uint32_t hardCol;
	uint32_t row;
};

struct Query {
	std::string query;
	uint32_t cursor;
	uint32_t selection;
};

struct PrevSearch {
	char type;
	char search;
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

struct Recording {
	bool on;
	char c;
};

class State {
    public:
	std::mutex renderMutex;
	std::mutex findMutex;
	std::mutex grepMutex;
	FileExplorerNode *fileExplorer;
	Mark mark;
	Mode mode;
	Options options;
	PrevSearch prevSearch;
	Position matching;
	Query commandLine;
	Query find;
	Query grep;
	Query name;
	Query replace;
	Query search;
	Bounds replaceBounds;
	VisualType visualType;
	bool searchBackwards;
	bool pasteAsBlock;
	bool dontRecordKey;
	bool dontSave;
	bool fileExplorerOpen;
	bool playingCommand;
	Recording recording;
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
	int32_t buildErrorIndex;
	int32_t fileExplorerIndex;
	int32_t fileExplorerWindowLine;
	static uint32_t maxX;
	static uint32_t maxY;
	std::string clipboard;
	std::string prompt;
	std::string buildDir;
	std::string grepPath;
	std::string prevKeys;
	std::string status;
	File *file;
	std::vector<File *> files;
	std::vector<grepMatch> grepOutput;
	std::vector<int32_t> motion;
	std::vector<std::filesystem::path> findOutput;
	std::vector<std::string> diffLines;
	std::vector<std::string> logLines;
	std::vector<std::string> blame;
	std::vector<std::string> dotCommand;
	std::vector<std::string> fileStack;
	std::map<char, std::vector<std::string>> macroCommand;
	std::vector<Harpoon> harpoon;
	std::vector<ModeKey> keys;
	struct Position visual;
	int32_t lastMacro;
	uint32_t currentFile;
	uint32_t fileExplorerSize;
	uint32_t fileStackIndex;
	uint32_t lineNumSize;
	uint32_t workspace;
	uint32_t logIndex;
	uint32_t diffIndex;
	State();
	State(std::string filename);
	State(std::string name, std::vector<std::string> data);
	void readStringConfigValue(std::string &option, std::string configValue, std::string line);
	void readUintConfigValue(uint32_t &option, std::string configValue, std::string line);
	void readBoolConfigValue(bool &option, std::string configValue, std::string line);
	void loadAllConfigFiles();
	void loadConfigFile(std::string fileLocation);
	void readConfigLine(std::string optionLine);
	void init();
	void init(std::string name, std::vector<std::string> data);
	bool resetState(std::string filename);
	void pushFileStack(std::string filename);
	void reloadFile(std::string filename);
	void changeFile(std::string filename);
	void setDefaultOptions();
	void print(std::string filename, bool printGrep, bool printArchives);
	static void setMaxYX(int32_t y, int32_t x);
};
