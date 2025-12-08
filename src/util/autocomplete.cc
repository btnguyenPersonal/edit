#include "autocomplete.h"

std::string autocomplete(State *state, const std::string &query)
{
	if (query == "") {
		return "";
	}
	std::map<std::string, int> wordCounts;
	for (std::string line : state->data) {
		line += ' ';
		std::string word = "";
		for (uint32_t i = 0; i < line.length(); i++) {
			if (isAlphanumeric(line[i])) {
				word += line[i];
			} else {
				if (word.substr(0, query.length()) == query) {
					wordCounts[word]++;
				}
				word = "";
			}
		}
		if (word.substr(0, query.length()) == query) {
			wordCounts[word]++;
		}
	}
	std::string mostCommonWord = "";
	int32_t maxCount = 0;
	for (const auto &pair : wordCounts) {
		if (pair.second > maxCount && pair.first != query) {
			mostCommonWord = pair.first;
			maxCount = pair.second;
		}
	}
	if (!mostCommonWord.empty()) {
		return safeSubstring(mostCommonWord, query.length());
	} else {
		return "";
	}
}
