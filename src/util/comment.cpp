#include "comment.h"
#include "bounds.h"
#include "fileops.h"
#include "indent.h"
#include "state.h"
#include "textedit.h"
#include <climits>
#include <string>
#include <vector>

std::string getCommentSymbol(const std::string &filename) {
	std::string extension = getExtension(filename);
	if (
		extension == "js"
		|| extension == "jsx"
		|| extension == "ts"
		|| extension == "tsx"
		|| extension == "cc"
		|| extension == "cpp"
		|| extension == "hpp"
		|| extension == "c"
		|| extension == "h"
		|| extension == "java"
		|| extension == "cs"
		|| extension == "go"
		|| extension == "php"
		|| extension == "rs"
		|| extension == "css"
		|| extension == "scss"
		|| extension == "vb"
		|| extension == "lua"
		|| extension == "scad"
	) {
		return "//";
	} else if (
		extension == "py"
		|| extension == "sh"
		|| extension == "bash"
		|| extension == "rb"
		|| extension == "pl"
		|| extension == "pm"
		|| extension == "r"
		|| extension == "yaml"
		|| extension == "yml"
		|| extension == "bashrc"
		|| extension == "zshrc"
		|| extension == "Makefile"
		|| extension == "md"
		|| extension == "gitignore"
		|| extension == "env"
	) {
		return "#";
	} else if (extension == "html" || extension == "xml" || extension == "xhtml" || extension == "svg") {
		return "<!--";
	} else if (extension == "sql") {
		return "--";
	} else if (extension == "lua") {
		return "--";
	} else if (extension == "json") {
		return "//";
	} else {
		return "#";
	}
}

std::string trimLeadingComment(State *state, std::string line) {
	std::string outputLine = line;
	for (size_t i = 0; i < line.length(); i++) {
		if (line[i] != getIndentCharacter(state)) {
			if (line.substr(i, state->file->commentSymbol.length()) == state->file->commentSymbol) {
				if (line.substr(i, state->file->commentSymbol.length() + 1) == state->file->commentSymbol + " ") {
					outputLine = line.substr(0, i) + line.substr(i + state->file->commentSymbol.length() + 1);
				} else {
					outputLine = line.substr(0, i) + line.substr(i + state->file->commentSymbol.length());
				}
				break;
			}
		}
	}
	return outputLine;
}

std::string trimComment(State *state, std::string line) {
	std::string outputLine = line;
	bool foundNonSpace = false;
	bool skipNext = false;
	bool inString = false;
	char stringType = 'E';
	for (size_t i = 0; i < line.length(); i++) {
		char c = line[i];
		if (skipNext) {
			skipNext = false;
		} else if (inString && c == '\\') {
			skipNext = true;
		} else if (!inString && (c == '"' || c == '`' || c == '\'')) {
			inString = true;
			stringType = c;
		} else if (inString && c == stringType) {
			inString = false;
		}
		if (!inString && line.substr(i, state->file->commentSymbol.length()) == state->file->commentSymbol) {
			if (foundNonSpace == true) {
				outputLine = line.substr(0, i);
			}
			break;
		} else if (c != getIndentCharacter(state)) {
			foundNonSpace = true;
		}
	}
	rtrim(outputLine);
	return outputLine;
}

void toggleComment(State *state) {
	toggleCommentHelper(state, state->file->row, -1);
}

void toggleCommentHelper(State *state, uint32_t row, int32_t commentIndex) {
	std::string line = state->file->data[row];
	if (commentIndex == -1) {
		int32_t i = getNumLeadingIndentCharacters(state, line);
		if (isCommentWithSpace(state, line)) {
			state->file->data[row] = line.substr(0, i) + line.substr(i + state->file->commentSymbol.length() + 1);
			return;
		} else if (isComment(state, line)) {
			state->file->data[row] = line.substr(0, i) + line.substr(i + state->file->commentSymbol.length());
			return;
		}
	}
	if (line.length() != 0) {
		int32_t spaces = commentIndex != -1 ? commentIndex : getNumLeadingIndentCharacters(state, line);
		state->file->data[row] = line.substr(0, spaces) + state->file->commentSymbol + ' ' + line.substr(spaces);
	}
}

bool isCommentWithSpace(State *state, std::string line) {
	ltrim(line);
	return line.substr(0, state->file->commentSymbol.length() + 1) == state->file->commentSymbol + ' ';
}

bool isComment(State *state, std::string line) {
	ltrim(line);
	return line.substr(0, state->file->commentSymbol.length()) == state->file->commentSymbol;
}

void toggleCommentLines(State *state, Bounds bounds) {
	bool foundNonComment = false;
	for (size_t i = bounds.minR; i <= bounds.maxR; i++) {
		if (!isComment(state, state->file->data[i]) && state->file->data[i] != "") {
			foundNonComment = true;
			break;
		}
	}
	int32_t minIndentLevel = -1;
	if (foundNonComment) {
		minIndentLevel = INT_MAX;
		for (size_t i = bounds.minR; i <= bounds.maxR; i++) {
			int32_t indent = getNumLeadingIndentCharacters(state, state->file->data[i]);
			if (indent < minIndentLevel && state->file->data[i] != "") {
				minIndentLevel = indent;
			}
		}
	}
	for (size_t i = bounds.minR; i <= bounds.maxR; i++) {
		toggleCommentHelper(state, i, minIndentLevel);
	}
}

void unCommentBlock(State *state) {
	bool foundComment = false;
	Bounds bounds;
	bounds.minC = 0;
	bounds.maxC = 0;
	if (isComment(state, state->file->data[state->file->row])) {
		for (int32_t i = (int32_t)state->file->row; i >= 0; i--) {
			if (!isComment(state, state->file->data[i])) {
				state->file->row = i;
				break;
			}
		}
	}
	for (uint32_t i = state->file->row; i < state->file->data.size(); i++) {
		if (!foundComment) {
			if (isComment(state, state->file->data[i])) {
				foundComment = true;
				bounds.minR = i;
				bounds.maxR = i;
			}
		} else {
			if (!isComment(state, state->file->data[i])) {
				break;
			} else {
				bounds.maxR = i;
			}
		}
	}
	if (foundComment) {
		toggleCommentLines(state, bounds);
		state->file->row = bounds.minR;
	}
}
