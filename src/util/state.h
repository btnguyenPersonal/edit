#pragma once

#include "modes.h"
#include "visualType.h"
#include "fileExplorerNode.h"
#include <filesystem>
#include <string>
#include <vector>
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

struct Jumplist {
	uint32_t index;
	std::vector<Position> list;
};

struct Archive {
	std::string filename;
	std::vector<std::string> previousState;
	std::vector<std::vector<diffLine> > history;
	int32_t historyPosition;
	struct Position windowPosition;
	uint32_t row;
	uint32_t col;
	Jumplist jumplist;
};

struct Query {
	std::string query;
	uint32_t cursor;
	uint32_t selection;
};

struct Options {
	bool wordwrap;
	bool showmode;
	bool autosave;
	uint32_t indent;
	std::string indentStyle;
};

class State {
    public:
	Options options;
	bool unsavedFile;
	bool showGrep;
	bool fileExplorerOpen;
	int32_t fileExplorerSize;
	int32_t fileExplorerIndex;
	int32_t fileExplorerWindowLine;
	Position matching;
	bool searchFail;
	Jumplist jumplist;
	std::vector<Archive> archives;
	int32_t buildErrorIndex;
	std::vector<std::string> buildErrors;
	std::vector<std::string> blame;
	std::map<uint32_t, std::string> harpoonFiles;
	uint32_t harpoonIndex;
	std::string buildDir;
	std::string filename;
	std::vector<std::string> data;
	std::vector<std::string> previousState;
	std::vector<std::string> fileStack;
	uint32_t fileStackIndex;
	std::vector<std::vector<diffLine> > history;
	int32_t historyPosition;
	bool searching;
	bool replacing;
	Mark mark;
	struct Position windowPosition;
	static uint32_t maxX;
	static uint32_t maxY;
	VisualType visualType;
	struct Position visual;
	uint32_t row;
	uint32_t col;
	std::string commentSymbol;
	Query name;
	Query search;
	Query replace;
	Query commandLine;
	std::string dotCommand;
	std::string macroCommand;
	bool playingCommand;
	std::string motion;
	bool recording;
	bool dontRecordKey;
	Query findFile;
	std::vector<std::filesystem::path> findFileOutput;
	std::string grepPath;
	Query grep;
	bool showAllGrep;
	std::vector<grepMatch> grepOutput;
	std::string prevKeys;
	std::string lastLoggingVar;
	std::string status;
	bool dontSave;
	bool selectAll;
	bool showFileStack;
	FileExplorerNode *fileExplorer;
	Mode mode;
	State();
	State(std::string filename);
	bool resetState(std::string filename);
	void pushFileStack(std::string filename);
	void changeFile(std::string filename);
	void loadConfigFile(std::string fileLocation);
	void readConfigLine(std::string optionLine);
	void setDefaultOptions();
	static void setMaxYX(int32_t y, int32_t x);
};
