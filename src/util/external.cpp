#include "external.h"
#include "string.h"
#include "textedit.h"

std::vector<std::string> getDiffLines(State *state) {
	std::vector<std::string> gitDiffLines;
	std::string hash = "";
	if (state->logIndex != 0) {
		for (uint32_t i = 0; i < state->logLines[state->logIndex].length(); i++) {
			if (state->logLines[state->logIndex][i] != ' ') {
				hash += state->logLines[state->logIndex][i];
			} else {
				break;
			}
		}
	}
	std::string command = "";
	if (hash == "") {
		command = "git add -N :/ && git diff HEAD | expand -t " + std::to_string(state->options.indent_size) + " 2>/dev/null";
	} else {
		command = "git show " + hash + " | expand -t " + std::to_string(state->options.indent_size) + " 2>/dev/null";
	}
	gitDiffLines = splitByChar(runCommand(command), '\n');
	if (hash == "" && gitDiffLines.size() == 0) {
		return {"No local changes"};
	}
	return gitDiffLines;
}

std::vector<std::string> getLogLines() {
	std::vector<std::string> gitLogLines = {"current"};
	std::vector<std::string> temp = splitByChar(runCommand("git log --oneline | cat 2>/dev/null"), '\n');
	for (uint32_t i = 0; i < temp.size(); i++) {
		gitLogLines.push_back(temp[i]);
	}
	return gitLogLines;
}

std::string getGitHash(State *state) {
	std::string command = std::string("git blame -l -L ") + std::to_string(state->file->row + 1) + ",+1 " + state->file->filename + " | awk '{print $1}'";
	std::string line = runCommand(command);
	rtrim(line);
	if (safeSubstring(line, 0, 1) == "^") {
		return safeSubstring(line, 1);
	}
	return line;
}

std::vector<std::string> getGitBlame(const std::string &filename) {
	std::vector<std::string> blameLines;
	std::string command = "git --no-pager blame ./" + filename + " --date=short 2>/dev/null | awk '{print $1, $2, $3, $4, \")\"}'";
	blameLines = splitByChar(runCommand(command), '\n');
	blameLines.push_back("");
	return blameLines;
}

std::string runCommand(std::string command) {
	std::string output = "";
	char temp[4096];
	FILE *fp = popen(command.c_str(), "r");
	if (fp) {
		while (fgets(temp, sizeof(temp), fp)) {
			output += temp;
		}
	}
	return output;
}

std::string runLinter(const std::string &filename) {
	if (filename == "") {
		return "";
	}
	size_t dotPosition = filename.find_last_of(".");
	std::string file = (dotPosition != std::string::npos) ? filename.substr(dotPosition + 1) : filename;
	std::string command = "";
	if (file == "cpp" || file == "h" || file == "c") {
		command = "clang-format -i --Werror ./" + filename + " 2>&1";
	} else if (file == "js" || file == "jsx") {
		command = "npx eslint --fix ./" + filename + " 2>&1";
	} else if (file == "ts" || file == "tsx") {
		command = "npx eslint --fix ./" + filename + " --ext .ts,.tsx 2>&1";
	} else if (file == "rs") {
		command = "cargo clippy --fix 2>&1";
	} else if (file == "py") {
		command = "python -m pylint --fix ./" + filename + " 2>&1";
	} else if (file == "go") {
		command = "golangci-lint run --fix 2>&1";
	} else if (file == "java") {
		command = "mvn compile 2>&1";
	} else if (file == "rb") {
		command = "rubocop --fix ./" + filename + " 2>&1";
	} else if (file == "sh") {
		command = "shellcheck -f ./" + filename + " 2>&1";
	} else if (file == "zig") {
		command = "zig fmt ./" + filename + " 2>&1";
	} else if (file == "lua") {
		command = "luacheck ./" + filename + " 2>&1";
	} else {
		return "No linter available for ." + file + " files";
	}
	return runCommand(command);
}
