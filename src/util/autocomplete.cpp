#include "autocomplete.h"
#include "string.h"

void calcWords(File *file) {
	file->words.clear();
	for (std::string line : file->data) {
		line += ' ';
		std::string word = "";
		for (uint32_t i = 0; i < line.length(); i++) {
			if (isAlphanumeric(line[i])) {
				word += line[i];
			} else {
				if (word != "") {
					file->words[word]++;
				}
				word = "";
			}
		}
		if (word != "") {
			file->words[word]++;
		}
	}
}

std::string autocomplete(State *state, const std::string &query) {
	if (query == "") {
		return "";
	}
	calcWords(state->file);
	std::string mostCommonWord = "";
	int32_t maxCount = 0;
	std::map<std::string, int> totalWords;
	for (uint32_t harpoonIndex = 0; harpoonIndex < state->harpoon[state->workspace].list.size(); harpoonIndex++) {
		for (uint32_t i = 0; i < state->files.size(); i++) {
			if (state->files[i]->filename == state->harpoon[state->workspace].list[harpoonIndex]) {
				for (const auto &pair : state->files[i]->words) {
					if (pair.first.substr(0, query.length()) == query) {
						totalWords[pair.first] += pair.second;
					}
				}
				break;
			}
		}
	}
	for (const auto &pair : state->file->words) {
		if (pair.first.substr(0, query.length()) == query) {
			// NOTE(ben): this weights words in current file twice as much
			totalWords[pair.first] += pair.second;
		}
	}
	for (const auto &pair : totalWords) {
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
