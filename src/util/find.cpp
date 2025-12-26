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
	if (query.empty()) {
		return 0;
	}

	auto to_lower = [](std::string s) {
		std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::tolower(c); });
		return s;
	};

	std::string fileLower = filePath.string();
	std::string queryLower = query;
	if (isAllLowercase(query)) {
		fileLower = to_lower(fileLower);
		queryLower = to_lower(queryLower);
	}

	if (fileLower.find(queryLower) != std::string::npos) {
		return query.size();
	}

	for (size_t len = queryLower.size() - 1; len > 0; len--) {
		if (fileLower.find(queryLower.substr(0, len)) != std::string::npos) {
			return static_cast<int32_t>(len);
		}
	}

	return 0;
}

std::vector<std::filesystem::path> find(const std::filesystem::path &dir_path, const std::string &query)
{
	std::vector<std::filesystem::path> matching_files;
	std::unordered_map<std::filesystem::path, int32_t> cache;
	for (auto it = std::filesystem::recursive_directory_iterator(dir_path); it != std::filesystem::recursive_directory_iterator(); ++it) {
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
	std::sort(matching_files.begin(), matching_files.end(), [&](const std::filesystem::path &a, const std::filesystem::path &b) {
		int32_t matchA = cache[a];
		int32_t matchB = cache[b];
		if (matchA == matchB) {
			return a.string() < b.string();
		}
		return matchA > matchB;
	});
	return matching_files;
}

void findDispatch(State *state, std::string query)
{
	std::vector<std::filesystem::path> output = find(std::filesystem::current_path(), query);
	state->findMutex.lock();
	if (query == state->find.query) {
		state->findOutput = output;
	}
	state->findMutex.unlock();
	renderScreen(state, false);
}

void generateFindOutput(State *state)
{
	std::string query = state->find.query;
	std::thread worker(findDispatch, state, query);
	worker.detach();
}
