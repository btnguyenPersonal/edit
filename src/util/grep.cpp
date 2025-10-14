#include "./grep.h"
#include "./ignore.h"

#include <future>
#include <fstream>
#include <thread>
#include <mutex>
#include <functional>
#include <semaphore>

// TODO rm
#include <iostream>

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
		return first.lineNum < second.lineNum;
	}
	return firstFile < secondFile;
}

void grepFile(const std::filesystem::path &file_path, const std::string &query,
				const std::filesystem::path &dir_path, std::mutex &allMatchesMutex, std::vector<grepMatch> &allMatches,
				std::counting_semaphore<10> &fileSemaphore)
{
	auto relativePath = file_path.lexically_relative(dir_path);
	std::vector<grepMatch> matches;
	fileSemaphore.acquire();
	std::ifstream file(file_path);
	std::string line;
	int32_t lineNumber = 0;
	while (std::getline(file, line)) {
		lineNumber++;
		if (line.find(query) != std::string::npos) {
			matches.emplace_back(relativePath, lineNumber, line);
		}
	}
	file.close();
	fileSemaphore.release();
	allMatchesMutex.lock();
	for (uint32_t i = 0; i < matches.size(); i++) {
		allMatches.push_back(matches[i]);
	}
	allMatchesMutex.unlock();
}

std::vector<grepMatch> grepFiles(const std::filesystem::path &dir_path, const std::string &query, bool allowAllFiles)
{
	std::vector<grepMatch> allMatches;
	std::mutex allMatchesMutex;
	std::vector<std::thread> threads;
	std::counting_semaphore<10> fileSemaphore(0);
	for (auto it = std::filesystem::recursive_directory_iterator(dir_path);
	     it != std::filesystem::recursive_directory_iterator(); ++it) {
		if (!allowAllFiles && shouldIgnoreFile(it->path())) {
			it.disable_recursion_pending();
			continue;
		}
		if (std::filesystem::is_regular_file(it->path())) {
			threads.push_back(
				std::thread(
					grepFile,
					it->path(),
					query,
					dir_path,
					std::ref(allMatchesMutex),
					std::ref(allMatches),
					std::ref(fileSemaphore)
				)
			);
		}
	}
	std::cout << threads.size() << std::endl;
	for (uint32_t i = 0; i < threads.size(); i++) {
		threads[i].join();
	}
	std::sort(allMatches.begin(), allMatches.end(), sortByFileType);

	return allMatches;
}

void generateGrepOutput(State *state, bool resetCursor)
{
	if (state->grep.query == "") {
		state->grepOutput.clear();
	} else {
		state->grepOutput = grepFiles(state->grepPath == "" ? std::filesystem::current_path() :
								      std::filesystem::path(state->grepPath),
					      state->grep.query, state->showAllGrep);
	}
	if (resetCursor) {
		state->grep.selection = 0;
	}
}
