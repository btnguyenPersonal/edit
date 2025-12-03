#include "./ignore.h"
#include "./find.h"
#include "./render.h"

#include <thread>

bool isAllLowercase(const std::string &str)
{
	for (char ch : str) {
		if (!std::islower(ch) && !std::isspace(ch)) {
			return false;
		}
	}
	return true;
}

int32_t maxConsecutiveMatch(const std::filesystem::path &filePath, const std::string &query)
{
	std::string filePathStr = filePath.string();
	std::string queryLower = query;

	if (isAllLowercase(queryLower)) {
		std::transform(filePathStr.begin(), filePathStr.end(), filePathStr.begin(),
			       [](unsigned char c) { return std::tolower(c); });
		std::transform(queryLower.begin(), queryLower.end(), queryLower.begin(),
			       [](unsigned char c) { return std::tolower(c); });
	}

	int32_t maxLength = 0;
	int32_t currentLength = 0;
	for (size_t i = 0, j = 0; i < filePathStr.size();) {
		if (filePathStr[i] == queryLower[j]) {
			currentLength++;
			i++;
			j++;
			if (j == queryLower.size()) {
				maxLength = std::max(maxLength, currentLength);
				currentLength = 0;
				j = 0;
			}
		} else {
			i = i - currentLength;
			currentLength = 0;
			i++;
		}
	}

	return maxLength;
}

std::vector<std::filesystem::path> find(const std::filesystem::path &dir_path, const std::string &query)
{
	std::vector<std::filesystem::path> matching_files;
	std::unordered_map<std::filesystem::path, int32_t> cache;
	for (auto it = std::filesystem::recursive_directory_iterator(dir_path);
	     it != std::filesystem::recursive_directory_iterator(); ++it) {
		auto path = it->path();
		if (shouldIgnoreFile(path)) {
			it.disable_recursion_pending();
			continue;
		}
		if (std::filesystem::is_regular_file(path)) {
			auto relativePath = path.lexically_relative(dir_path);
			int32_t matchQuery = query == "" ? 1 : maxConsecutiveMatch(relativePath, query);
			if (matchQuery > 0) {
				cache[relativePath] = matchQuery;
				matching_files.push_back(relativePath);
			}
		}
	}
	std::sort(matching_files.begin(), matching_files.end(),
		  [&](const std::filesystem::path &a, const std::filesystem::path &b) {
			  int32_t matchA = cache[a];
			  int32_t matchB = cache[b];
			  if (matchA == matchB) {
				  return a.string() < b.string();
			  }
			  return matchA > matchB;
		  });
	return matching_files;
}

void findDispatch(State *state)
{
	auto query = state->find.query;
	auto output = find(std::filesystem::current_path(), state->find.query);
	state->findMutex.lock();
	if (query == state->find.query) {
		state->findOutput = output;
	}
	state->findMutex.unlock();
	renderScreen(state, false);
}

void generateFindOutput(State *state)
{
	std::thread worker(findDispatch, state);
	worker.detach();
}
