#include "./ignore.h"
#include "./find.h"

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
			j = 0;
			currentLength = 0;
			i++;
		}
	}
	return maxLength;
}

bool filePathContainsSubstring(const std::filesystem::path &filePath, const std::string &query)
{
	std::string filePathStr = filePath.string();
	std::string queryLower = query;

	if (isAllLowercase(queryLower)) {
		std::transform(filePathStr.begin(), filePathStr.end(), filePathStr.begin(),
			       [](unsigned char c) { return std::tolower(c); });
		std::transform(queryLower.begin(), queryLower.end(), queryLower.begin(),
			       [](unsigned char c) { return std::tolower(c); });
	}

	uint32_t filePathIndex = 0;
	uint32_t queryIndex = 0;
	while (queryIndex < queryLower.length() && filePathIndex < filePathStr.length()) {
		if (filePathStr[filePathIndex] == queryLower[queryIndex]) {
			filePathIndex++;
			queryIndex++;
		} else {
			filePathIndex++;
		}
	}

	return queryIndex == queryLower.length();
}

std::vector<std::filesystem::path> find(const std::filesystem::path &dir_path, const std::string &query)
{
	std::vector<std::filesystem::path> matching_files;
	const auto start_time = std::chrono::steady_clock::now();
	for (auto it = std::filesystem::recursive_directory_iterator(dir_path);
	     it != std::filesystem::recursive_directory_iterator(); ++it) {
		if (shouldIgnoreFile(it->path())) {
			it.disable_recursion_pending();
			continue;
		}
		if (std::filesystem::is_regular_file(it->path())) {
			auto relativePath = it->path().lexically_relative(dir_path);
			if (filePathContainsSubstring(relativePath, query)) {
				matching_files.push_back(relativePath);
			}
		}
	}
	auto end_time = std::chrono::steady_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
	std::cout << "find time: " << duration.count() << " milliseconds" << std::endl;
	std::cout << "num: " << matching_files.size() << std::endl;
	std::unordered_map<std::filesystem::path, int32_t> map;
	int hit = 0;
	int miss = 0;
	// std::sort(matching_files.begin(), matching_files.end(),
	// 	[&](const std::filesystem::path &a, const std::filesystem::path &b) {
	// 		int32_t matchA = map[a];
	// 		if (matchA == 0) {
	// 			matchA += 10 * maxConsecutiveMatch(a, query);
	// 			if (isTestFile(a.string())) {
	// 				matchA -= 5;
	// 			}
	// 			map[b] = matchA;
	// 			miss++;
	// 		} else {
	// 			hit++;
	// 		}
	// 		int32_t matchB = map[b];
	// 		if (matchB == 0) {
	// 			matchB += 10 * maxConsecutiveMatch(b, query);
	// 			if (isTestFile(b.string())) {
	// 				matchB -= 5;
	// 			}
	// 			map[b] = matchB;
	// 			miss++;
	// 		} else {
	// 			hit++;
	// 		}
	// 		if (matchA == matchB) {
	// 			return a.string() < b.string();
	// 		}
	// 		return matchA > matchB;
	// 	});
	std::cout << hit << std::endl;
	std::cout << miss << std::endl;
	end_time = std::chrono::steady_clock::now();
	duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
	std::cout << "sort time: " << duration.count() << " milliseconds" << std::endl;
	return matching_files;
}

void generateFindOutput(State *state)
{
	state->findOutput = find(std::filesystem::current_path(), state->find.query);
}
