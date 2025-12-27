#include "ignore.h"
#include "find.h"

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
	std::vector<std::filesystem::path> stack;
	stack.push_back(dir_path);

	while (!stack.empty()) {
		std::filesystem::path dir = stack.back();
		stack.pop_back();

		std::error_code error;
		for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(dir, error)) {
			if (error) {
				continue;
			}

			const std::filesystem::path& path = entry.path();

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
	try {
		std::vector<std::filesystem::path> output = find(std::filesystem::current_path(), query);
		state->findMutex.lock();
		if (query == state->find.query) {
			state->findOutput = output;
		}
		state->shouldReRender = true;
		state->findMutex.unlock();
	} catch (const std::exception &e) {}
}

void generateFindOutput(State *state)
{
	std::string query = state->find.query;
	std::thread worker(findDispatch, state, query);
	worker.detach();
}
