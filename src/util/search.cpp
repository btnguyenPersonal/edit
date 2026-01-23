#include "search.h"
#include "string.h"
#include "sanity.h"
#include "movement.h"
#include "textbuffer.h"
#include <regex>
#include <fstream>

bool setSearchResultCurrentLine(State *state, const std::string &query)
{
	std::string line = state->file->data[state->file->row];
	size_t index = line.find(query);
	if (index != std::string::npos) {
		state->file->col = static_cast<uint32_t>(index);
		return true;
	}
	return false;
}

void searchNextResult(State *state, bool reverse)
{
	if (reverse) {
		bool result = setSearchResultReverse(state, false);
		if (result == false) {
			state->searchFail = true;
		}
	} else {
		state->file->col += 1;
		uint32_t temp_col = state->file->col;
		uint32_t temp_row = state->file->row;
		bool result = setSearchResult(state);
		if (result == false) {
			state->searchFail = true;
			state->file->row = temp_row;
			state->file->col = temp_col - 1;
		}
	}
}

void repeatPrevLineSearch(State *state, bool reverse)
{
	char temp = state->prevSearch.type;
	switch (state->prevSearch.type) {
	case 'f':
		state->file->col = findChar(state, reverse, state->prevSearch.search);
		break;
	case 'F':
		state->file->col = findChar(state, !reverse, state->prevSearch.search);
		break;
	case 't':
		state->file->col = toChar(state, reverse, state->prevSearch.search);
		break;
	case 'T':
		state->file->col = toChar(state, !reverse, state->prevSearch.search);
		break;
	default:
		break;
	}
	state->prevSearch.type = temp;
}

bool setSearchResultReverse(State *state, bool allowCurrent)
{
	if (state->search.query == "") {
		return false;
	}
	fixColOverMax(state);
	uint32_t initialCol = state->file->col;
	uint32_t initialRow = state->file->row;
	uint32_t col = initialCol;
	if (allowCurrent) {
		col += state->search.query.length();
	}
	uint32_t row = initialRow;
	bool isFirst = true;
	do {
		std::string line = isFirst ? state->file->data[row].substr(0, col) : state->file->data[row];
		size_t index = line.rfind(state->search.query);
		if (index != std::string::npos) {
			state->file->row = row;
			state->file->col = static_cast<uint32_t>(index);
			return true;
		}
		if (row == 0) {
			row = state->file->data.size() - 1;
		} else {
			row--;
		}
		isFirst = false;
	} while (row != initialRow);
	// try last row again
	std::string line = state->file->data[row];
	size_t index = line.rfind(state->search.query);
	if (index != std::string::npos) {
		state->file->row = row;
		state->file->col = static_cast<uint32_t>(index);
		return true;
	}
	return false;
}

void replaceAll(State *state, const std::string &query, const std::string &replace)
{
	uint32_t i = 0;
	uint32_t max = state->file->data.size();
	if (state->replaceBounds.minR != 0 || state->replaceBounds.maxR != 0) {
		i = state->replaceBounds.minR;
		max = state->replaceBounds.maxR + 1;
	}
	for (; i < max && i < state->file->data.size(); i++) {
		if (query.empty()) {
			return;
		}
		size_t startPos = 0;
		std::string line = textbuffer_getLine(state, i);
		while ((startPos = line.find(query, startPos)) != std::string::npos) {
			line.replace(startPos, query.length(), replace);
			textbuffer_replaceLine(state, i, line);
			startPos += replace.length();
			startPos += replace.length();
		}
	}
}

bool setSearchResult(State *state)
{
	if (state->search.query == "") {
		return false;
	}
	fixColOverMax(state);
	uint32_t initialCol = state->file->col;
	uint32_t initialRow = state->file->row;
	uint32_t col = initialCol;
	uint32_t row = initialRow;
	do {
		std::string line = state->file->data[row].substr(col);
		size_t index = line.find(state->search.query);
		if (index != std::string::npos) {
			state->file->row = row;
			state->file->col = static_cast<uint32_t>(index) + col;
			return true;
		}
		row = (row + 1) % state->file->data.size();
		col = 0;
	} while (row != initialRow);
	std::string line = state->file->data[row];
	size_t index = line.find(state->search.query);
	if (index != std::string::npos) {
		state->file->row = row;
		state->file->col = static_cast<uint32_t>(index) + col;
		return true;
	}
	return false;
}

void replaceAllGlobally(State *state, const std::string &query, const std::string &replace)
{
	if (query.empty()) {
		state->status = "query empty";
		return;
	}

	try {
		std::regex re(query);
	} catch (const std::regex_error &) {
		state->status = "invalid regex";
		return;
	}

	std::regex re(query);

	FILE *pipe = popen("git ls-files", "r");
	if (!pipe) {
		state->status = "git not available";
		return;
	}

	char path[1024];
	int modified_count = 0;

	while (fgets(path, sizeof(path), pipe)) {
		std::string filename = path;
		if (!filename.empty() && filename.back() == '\n') {
			filename.pop_back();
		}

		std::filesystem::path full_path = std::filesystem::current_path() / filename;
		if (!std::filesystem::is_regular_file(full_path)) {
			continue;
		}

		std::ifstream in(full_path, std::ios::binary);
		if (!in) {
			continue;
		}

		std::string content((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());

		std::string replaced = std::regex_replace(content, re, replace);

		if (replaced != content) {
			std::ofstream out(full_path, std::ios::binary);
			if (out) {
				out << replaced;
				modified_count++;
			}
		}
	}
	pclose(pipe);

	if (state->file) {
		state->reloadFile(state->file->filename);
	}

	state->status = modified_count > 0 ? "replaced in " + std::to_string(modified_count) + " files" : "no matches found";
}

void replaceCurrentLine(State *state, const std::string &query, const std::string &replace)
{
	if (query.empty()) {
		return;
	}
	size_t startPos = 0;
	std::string line = textbuffer_getLine(state, state->file->row);
	while ((startPos = line.find(query, startPos)) != std::string::npos) {
		line.replace(startPos, query.length(), replace);
		textbuffer_replaceLine(state, state->file->row, line);
		startPos += replace.length();
	}
}

std::string getCurrentWord(State *state)
{
	std::string currentWord = "";
	for (int32_t i = (int32_t)state->file->col - 1; i >= 0; i--) {
		if (isAlphanumeric(state->file->data[state->file->row][i])) {
			currentWord = state->file->data[state->file->row][i] + currentWord;
		} else {
			break;
		}
	}
	return currentWord;
}
