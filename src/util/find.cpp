#include "find.h"
#include "ignore.h"

#include <thread>

bool isAllLowercase(const std::string &str) {
	for (char ch : str) {
		if (!std::islower(ch) && !std::isspace(ch)) {
			return false;
		}
	}
	return true;
}

int32_t maxConsecutiveMatch(const std::filesystem::path &filePath, const std::string &query) {
	if (query.empty()) {
		return 0;
	}

	auto to_lower = [](std::string s) {
		std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::tolower(c); });
		return s;
	};

	std::string fileStr = filePath.string();
	std::string fileLower = to_lower(fileStr);
	std::string queryLower = to_lower(query);

	auto isWordBoundary = [&](const std::string &str, size_t pos) {
		if (pos == 0)
			return true;
		if (pos >= str.length())
			return true;
		char prev = str[pos - 1];
		char curr = str[pos];
		return (std::isalnum(prev) && !std::isalnum(curr)) || (!std::isalnum(prev) && std::isalnum(curr));
	};

	int32_t caseSensitiveScore = 0;
	size_t filePos = 0;
	bool caseSensitiveMatch = true;

	for (char queryChar : query) {
		size_t foundPos = fileStr.find(queryChar, filePos);
		if (foundPos == std::string::npos) {
			caseSensitiveMatch = false;
			break;
		}

		int32_t points = 2;
		if (foundPos == filePos) {
			points += 2;
		}
		if (isWordBoundary(fileStr, foundPos)) {
			points += 3;
		}

		caseSensitiveScore += points;
		filePos = foundPos + 1;
	}

	if (caseSensitiveMatch) {
		if (fileStr.find(query) != std::string::npos) {
			caseSensitiveScore += query.size() * 3;
		}
		caseSensitiveScore += fileStr.length() / 10;
		return caseSensitiveScore;
	}

	int32_t caseInsensitiveScore = 0;
	filePos = 0;

	for (char queryChar : queryLower) {
		size_t foundPos = fileLower.find(queryChar, filePos);
		if (foundPos == std::string::npos) {
			return 0;
		}

		int32_t points = 1;
		if (foundPos == filePos) {
			points += 1;
		}
		if (isWordBoundary(fileLower, foundPos)) {
			points += 2;
		}

		caseInsensitiveScore += points;
		filePos = foundPos + 1;
	}

	if (fileLower.find(queryLower) != std::string::npos) {
		caseInsensitiveScore += queryLower.size() * 2;
	}
	caseInsensitiveScore += fileStr.length() / 20;

	return caseInsensitiveScore;
}

std::vector<std::filesystem::path> find(const std::filesystem::path &dir_path, const std::string &query) {
	std::vector<std::filesystem::path> matching_files;
	std::unordered_map<std::filesystem::path, int32_t> cache;
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

			if (shouldIgnoreFile(path)) {
				continue;
			}

			if (entry.is_directory(error)) {
				stack.push_back(path);
			} else if (entry.is_regular_file(error)) {
				std::filesystem::path rel = path.lexically_relative(dir_path);
				int32_t score = query.empty() ? 1 : maxConsecutiveMatch(rel, query);

				if (score > 0) {
					cache[rel] = score;
					matching_files.push_back(rel);
				}
			}
		}
	}
	std::sort(matching_files.begin(), matching_files.end(), [&](const std::filesystem::path &a, const std::filesystem::path &b) {
		auto itA = cache.find(a);
		auto itB = cache.find(b);
		if (itA == cache.end() || itB == cache.end()) {
			return a.string() < b.string();
		}
		int32_t matchA = itA->second;
		int32_t matchB = itB->second;
		if (matchA == matchB) {
			return a.string() < b.string();
		}
		return matchA > matchB;
	});
	return matching_files;
}

void findDispatch(State *state, std::string query) {
	std::vector<std::filesystem::path> output;
	try {
		output = find(std::filesystem::current_path(), query);
	} catch (const std::exception &e) {
	}
	state->findMutex.lock();
	if (query == state->find.query) {
		state->findOutput = output;
	}
	state->findMutex.unlock();
	state->shouldNotReRender.clear();
}

void generateFindOutput(State *state) {
	std::string query = state->find.query;
	std::thread worker(findDispatch, state, query);
	worker.detach();
}
