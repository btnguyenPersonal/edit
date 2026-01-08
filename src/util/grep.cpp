#include "grep.h"
#include "ignore.h"
#include "search.h"
#include "fileops.h"

#include <fstream>
#include <thread>
#include <mutex>
#include <algorithm>

const static int32_t THREAD_MAX = 5;

bool isFunctionLine(std::string line, std::string s, std::string extension)
{
	std::vector<std::vector<std::string> > functionStrings;
	if (extension == "js" || extension == "jsx" || extension == "ts" || extension == "tsx") {
		functionStrings = {
			{ "enum", " {" }, { "async", "" }, { "class", " {" }, { " ", " (" }, { " ", "(" }, { "const", " " }, { "function", "(" }, { "struct", " {" }, { "interface", " {" },
		};
	} else if (extension == "c" || extension == "cc" || extension == "cpp" || extension == "h" || extension == "hpp") {
		functionStrings = {
			{ "", "(" },
		};
	}
	for (uint32_t i = 0; i < functionStrings.size(); i++) {
		if (line.find(functionStrings[i][0] + " " + s + functionStrings[i][1]) != std::string::npos) {
			return true;
		}
	}
	return false;
}

bool sortByLineNum(const grepMatch &first, const grepMatch &second)
{
	return first.lineNum < second.lineNum;
}

bool sortByFileType(const grepMatch &first, const grepMatch &second)
{
	std::string firstFile = first.path.string();
	std::string secondFile = second.path.string();
	if (isTestFile(firstFile) && !isTestFile(secondFile)) {
		return false;
	}
	if (!isTestFile(firstFile) && isTestFile(secondFile)) {
		return true;
	}
	if (firstFile == secondFile) {
		return sortByLineNum(first, second);
	}
	return firstFile < secondFile;
}

bool sortAllMatches(const std::vector<grepMatch> &first, const std::vector<grepMatch> &second)
{
	if (first.size() == 0) {
		return false;
	} else if (second.size() == 0) {
		return true;
	}
	grepMatch firstGrep = first[0];
	grepMatch secondGrep = second[0];
	return sortByFileType(firstGrep, secondGrep);
}

void grepFile(const std::filesystem::path &file_path, const std::string &query, const std::filesystem::path &dir_path, std::mutex &allMatchesMutex, std::vector<std::vector<grepMatch> > &allMatches)
{
	auto relativePath = file_path.lexically_relative(dir_path);
	std::vector<grepMatch> matches;
	std::ifstream file(file_path);
	std::string line;
	int32_t lineNumber = 0;
	while (std::getline(file, line)) {
		lineNumber++;
		try {
			if (line.find(query) != std::string::npos) {
				matches.emplace_back(relativePath, lineNumber, line);
			}
		} catch (const std::exception &e) {
			// TODO something flaky here but can't be bothered to fix it
			matches.emplace_back(relativePath, lineNumber, line);
		}
	}
	file.close();
	std::sort(matches.begin(), matches.end(), sortByLineNum);
	allMatchesMutex.lock();
	allMatches.push_back(matches);
	allMatchesMutex.unlock();
}

void grepThread(const std::string &query, const std::filesystem::path &dir_path, std::mutex &allMatchesMutex, std::vector<std::vector<grepMatch> > &allMatches, std::mutex &allFilesMutex, std::vector<std::filesystem::path> &allFiles)
{
	std::filesystem::path file;
	bool done = false;
	do {
		allFilesMutex.lock();
		if (allFiles.size() > 0) {
			file = allFiles.back();
			allFiles.pop_back();
		} else {
			done = true;
		}
		allFilesMutex.unlock();
		if (!done) {
			grepFile(file, query, dir_path, allMatchesMutex, allMatches);
		}
	} while (!done);
}

std::vector<grepMatch> grepFiles(std::filesystem::path dir_path, std::string query, bool allowAllFiles)
{
	std::vector<std::vector<grepMatch> > allMatches;
	std::mutex allMatchesMutex;
	std::vector<std::filesystem::path> allFiles;
	std::mutex allFilesMutex;
	std::vector<std::thread> threads;
	std::vector<std::filesystem::path> stack;
	stack.push_back(dir_path);

	while (!stack.empty()) {
		std::filesystem::path dir = stack.back();
		stack.pop_back();

		std::error_code error;
		for (const std::filesystem::directory_entry &entry : std::filesystem::directory_iterator(dir, error)) {
			if (error) {
				continue;
			}

			const std::filesystem::path &path = entry.path();

			if (!allowAllFiles && shouldIgnoreFile(path)) {
				continue;
			}

			if (entry.is_directory(error)) {
				stack.push_back(path);
			} else if (entry.is_regular_file(error)) {
				allFilesMutex.lock();
				allFiles.push_back(path);
				allFilesMutex.unlock();
			}
		}
	}
	for (uint32_t i = 0; i < THREAD_MAX; i++) {
		threads.push_back(std::thread(grepThread, query, dir_path, std::ref(allMatchesMutex), std::ref(allMatches), std::ref(allFilesMutex), std::ref(allFiles)));
	}
	for (uint32_t i = 0; i < threads.size(); i++) {
		if (threads[i].joinable()) {
			threads[i].join();
		}
	}
	std::sort(allMatches.begin(), allMatches.end(), sortAllMatches);
	uint32_t size = 0;
	for (uint32_t i = 0; i < allMatches.size(); i++) {
		size += allMatches[i].size();
	}
	std::vector<grepMatch> output;
	output.reserve(size);
	for (uint32_t i = 0; i < allMatches.size(); i++) {
		for (uint32_t j = 0; j < allMatches[i].size(); j++) {
			output.push_back(allMatches[i][j]);
		}
	}
	return output;
}

void grepDispatch(State *state, std::string query, std::string path, bool showAllGrep)
{
	std::vector<grepMatch> output;
	try {
		output = grepFiles(path == "" ? std::filesystem::current_path() : std::filesystem::path(path), query, showAllGrep);
	} catch (const std::exception &e) {
	}
	state->grepMutex.lock();
	if (query == state->grep.query) {
		state->grepOutput = output;
	}
	state->grepMutex.unlock();
	state->shouldNotReRender.clear();
}

void generateGrepOutput(State *state, bool resetCursor)
{
	if (state->grep.query == "") {
		state->grepMutex.lock();
		state->grepOutput.clear();
		state->grepMutex.unlock();
	} else {
		std::string query = state->grep.query;
		std::string path = state->grepPath;
		bool showAllGrep = state->showAllGrep;
		std::thread worker(grepDispatch, state, query, path, showAllGrep);
		worker.detach();
	}
	if (resetCursor) {
		state->grep.selection = 0;
	}
}

void changeToGrepFile(State *state)
{
	if (state->grep.selection < state->grepOutput.size()) {
		std::filesystem::path selectedFile = state->grepOutput[state->grep.selection].path;
		if (state->grepPath != "") {
			selectedFile = state->grepPath / selectedFile;
		}
		int32_t lineNum = state->grepOutput[state->grep.selection].lineNum;
		state->resetState(selectedFile);
		state->file->row = lineNum - 1;
		setSearchResultCurrentLine(state, state->grep.query);
	}
}

void findDefinitionFromGrepOutput(State *state, std::string s)
{
	std::string extension = getExtension(state->file->filename);
	for (uint32_t i = 0; i < state->grepOutput.size(); i++) {
		if (state->grepOutput[i].line.back() == '(' || state->grepOutput[i].line.back() == '{') {
			if (isFunctionLine(state->grepOutput[i].line, s, extension)) {
				state->grep.selection = i;
				changeToGrepFile(state);
			}
		}
	}
}
