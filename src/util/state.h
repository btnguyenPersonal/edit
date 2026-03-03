#pragma once

#include "bounds.h"
#include "fileExplorerNode.h"
#include "modes.h"
#include "perf.h"
#include "timer.h"
#include "visualType.h"

#include <atomic>
#include <filesystem>
#include <map>
#include <mutex>
#include <string>
#include <vector>

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
	std::filesystem::file_time_type lastModified;
	std::string commentSymbol;
	std::string filename;
	std::vector<std::string> data;
	std::vector<std::string> previousState;
	std::vector<std::vector<diffLine>> history;
	std::map<std::string, int> words;
	struct Position windowPosition;
	uint32_t col;
	uint32_t hardCol;
	uint32_t row;
	bool newFile;
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
	bool autoload;
	uint32_t keysSize;
	int32_t maxHarpoon;
	bool insert_final_newline;
	uint32_t indent_size;
	std::string indent_style;
};

struct CommandLineAutocompleteState {
	int32_t skips;
	std::string currentUncompleted;
	std::string lastDirectory;
};

struct Recording {
	bool on;
	char c;
};

struct FileExplorer {
	FileExplorerNode *root;
	bool open;
	int32_t index;
	int32_t windowLine;
	uint32_t size;
};

class State {
	public:
		std::atomic_flag shouldNotReRender;
		std::mutex findMutex;
		std::mutex grepMutex;
		bool debug;
		std::vector<timer> timers;
		Mark mark;
		Mode mode;
		Mode prevMode;
		FileExplorer explorer;
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
		bool dontComputeHistory;
		bool dontSave;
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
		uint32_t maxX;
		uint32_t maxY;
		uint32_t frameCounter;
		CommandLineAutocompleteState commandLineState;
		std::string extension;
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
		uint32_t fileStackIndex;
		uint32_t lineNumSize;
		uint32_t workspace;
		uint32_t logIndex;
		uint32_t diffIndex;
		State();
		~State();
		State(const std::string &filename);
		State(const std::string &name, const std::vector<std::string> &data);
		void readStringConfigValue(std::string &option, const std::string &configValue, const std::string &line);
		void readUintConfigValue(uint32_t &option, const std::string &configValue, const std::string &line);
		void readBoolConfigValue(bool &option, const std::string &configValue, const std::string &line);
		void loadAllConfigFiles();
		void loadConfigFile(const std::string &fileLocation);
		void readConfigLine(const std::string &optionLine);
		void init();
		void init(const std::string &name, const std::vector<std::string> &data);
		bool resetState(const std::string &filename);
		void pushFileStack(const std::string &filename);
		void reloadFile(const std::string &filename);
		void changeFile(const std::string &filename);
		void setDefaultOptions();
		void print(const std::string &filename, bool printGrep, bool printArchives);
	};
