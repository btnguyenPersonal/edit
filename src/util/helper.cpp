#include "helper.h"
#include "state.h"
#include "visualType.h"
#include <algorithm>
#include <climits>
#include <cstdio>
#include <fstream>
#include <future>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <ncurses.h>
#include <stdexcept>
#include <string>
#include <vector>

void recordAction(State* state) {
    auto now = std::chrono::steady_clock::now();
    state->actionTimestamps.push_back(now);
    while (!state->actionTimestamps.empty() && now - state->actionTimestamps.front() > std::chrono::seconds(60)) {
        state->actionTimestamps.pop_front();
    }
}

int calculateAPM(State* state) {
    if (state->actionTimestamps.empty()) {
        return 0;
    }
    auto now = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - state->actionTimestamps.front());
    double minutes = duration.count() / 60.0;
    if (minutes == 0.0) {
        minutes = 1.0 / 60.0;
    }
    return static_cast<int>(state->actionTimestamps.size() / minutes);
}

std::string getCommentSymbol(std::string filename) {
    std::string extension = getExtension(filename);
    if (extension == "js" || extension == "jsx" || extension == "ts" || extension == "tsx" || extension == "cpp" || extension == "hpp" || extension == "c" || extension == "h" ||
        extension == "java" || extension == "cs" || extension == "go" || extension == "php" || extension == "rs" || extension == "css" || extension == "scss" ||
        extension == "vb" || extension == "lua") {
        return "//";
    } else if (extension == "py" || extension == "sh" || extension == "bash" || extension == "rb" || extension == "pl" || extension == "pm" || extension == "r" ||
               extension == "yaml" || extension == "yml" || extension == "bashrc" || extension == "zshrc" || extension == "Makefile" || extension == "md" ||
               extension == "gitignore" || extension == "env") {
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

bool isAlphanumeric(char c) { return std::isalnum(c) || c == '_' ? 1 : 0; }

unsigned int findNextChar(State* state, char c) {
    for (unsigned int i = state->col; i < state->data[state->row].length(); i++) {
        if (state->data[state->row][i] == c) {
            return i;
        }
    }
    return state->col;
}

unsigned int toNextChar(State* state, char c) {
    unsigned int index = state->col;
    for (unsigned int i = state->col; i < state->data[state->row].length(); i++) {
        if (state->data[state->row][i] == c) {
            return index;
        } else {
            index = i;
        }
    }
    return state->col;
}

std::string getGitHash(State* state) {
    std::stringstream command;
    command << "git blame -l -L " << state->row + 1 << ",+1 " << state->filename << " | awk '{print $1}'";
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.str().c_str(), "r"), pclose);
    if (!pipe)
        throw std::runtime_error("popen() failed!");
    std::string output, line;
    char buffer[128];
    while (fgets(buffer, sizeof(buffer), pipe.get()) != NULL) {
        output += buffer;
    }
    std::stringstream ss(output);
    std::getline(ss, line);
    return line;
}

std::vector<std::string> getGitBlame(const std::string& filename) {
    std::vector<std::string> blameLines;
    try {
        std::string command = "git --no-pager blame ./" + filename + " --date=short 2>/dev/null | awk '{print $1, $2, $3, $4, \")\"}'";
        std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
        if (!pipe) {
            throw std::runtime_error("popen() failed!");
        }
        std::stringstream ss;
        char* line = nullptr;
        size_t len = 0;
        ssize_t read;
        while ((read = getline(&line, &len, pipe.get())) != -1) {
            ss << line;
        }
        free(line);
        std::string str;
        while (std::getline(ss, str)) {
            if (!str.empty()) {
                blameLines.push_back(str);
            }
        }
    } catch (const std::exception& e) {
    }
    blameLines.push_back("");
    return blameLines;
}

unsigned int getLineNumberOffset(State* state) {
    if (state->mode == BLAME) {
        return 6 + 65;
    }
    return 6;
}

std::string getExtension(std::string filename) {
    if (filename == "") {
        return "";
    }
    size_t slashPosition = filename.find_last_of("/\\");
    std::string file = (slashPosition != std::string::npos) ? filename.substr(slashPosition + 1) : filename;
    size_t dotPosition = file.find_last_of(".");
    return (dotPosition != std::string::npos && dotPosition != 0) ? file.substr(dotPosition + 1) : file;
}

void moveHarpoonRight(State* state) {
    if (state->harpoonIndex + 1 < state->harpoonFiles.size()) {
        std::string temp = state->harpoonFiles[state->harpoonIndex];
        state->harpoonFiles[state->harpoonIndex] = state->harpoonFiles[state->harpoonIndex + 1];
        state->harpoonFiles[state->harpoonIndex + 1] = temp;
        state->harpoonIndex += 1;
    }
}

void moveHarpoonLeft(State* state) {
    if (state->harpoonIndex > 0) {
        std::string temp = state->harpoonFiles[state->harpoonIndex];
        state->harpoonFiles[state->harpoonIndex] = state->harpoonFiles[state->harpoonIndex - 1];
        state->harpoonFiles[state->harpoonIndex - 1] = temp;
        state->harpoonIndex -= 1;
    }
}

void rtrim(std::string& s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(), s.end());
}

void ltrim(std::string& s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) { return !std::isspace(ch); }));
}

std::string getCurrentWord(State* state) {
    std::string currentWord = "";
    for (int i = (int)state->col - 1; i >= 0; i--) {
        if (isAlphanumeric(state->data[state->row][i])) {
            currentWord = state->data[state->row][i] + currentWord;
        } else {
            break;
        }
    }
    return currentWord;
}

std::string autocomplete(State* state, std::string query) {
    if (query == "") {
        return "";
    }
    std::map<std::string, int> wordCounts;
    for (std::string line : state->data) {
        line += ' ';
        std::string word = "";
        for (unsigned int i = 0; i < line.length(); i++) {
            if (isAlphanumeric(line[i])) {
                word += line[i];
            } else {
                if (word.substr(0, query.length()) == query) {
                    ++wordCounts[word];
                }
                word = "";
            }
        }
        if (word.substr(0, query.length()) == query) {
            ++wordCounts[word];
        }
    }
    std::string mostCommonWord = "";
    int maxCount = 0;
    for (const auto& pair : wordCounts) {
        if (pair.second > maxCount && pair.first != query) {
            mostCommonWord = pair.first;
            maxCount = pair.second;
        }
    }
    if (!mostCommonWord.empty()) {
        return mostCommonWord.substr(query.length());
    } else {
        return "";
    }
}

void replaceCurrentLine(State* state, std::string query, std::string replace) {
    if (query.empty()) {
        return;
    }
    size_t startPos = 0;
    while ((startPos = state->data[state->row].find(query, startPos)) != std::string::npos) {
        state->data[state->row].replace(startPos, query.length(), replace);
        startPos += replace.length();
    }
}

void replaceAllGlobally(State* state, std::string query, std::string replace) {
    try {
        std::string command = ("git ls-files | xargs -I {} sed -i'' \"s/" + query + '/' + replace + "/g\" \"{}\" 2>/dev/null");
        int returnValue = std::system(command.c_str());
        if (returnValue != 0) {
            throw std::exception();
        }
        state->changeFile(state->filename);
    } catch (const std::exception& e) {
        state->status = "command failed";
    }
}

void replaceAll(State* state, std::string query, std::string replace) {
    for (unsigned int i = 0; i < state->data.size(); i++) {
        if (query.empty()) {
            return;
        }
        size_t startPos = 0;
        while ((startPos = state->data[i].find(query, startPos)) != std::string::npos) {
            state->data[i].replace(startPos, query.length(), replace);
            startPos += replace.length();
        }
    }
}

bool setSearchResultReverse(State* state) {
    fixColOverMax(state);
    unsigned int initialCol = state->col;
    unsigned int initialRow = state->row;
    unsigned int col = initialCol;
    unsigned int row = initialRow;
    bool isFirst = true;
    do {
        std::string line = isFirst ? state->data[row].substr(0, col) : state->data[row];
        size_t index = line.rfind(state->searchQuery);
        if (index != std::string::npos) {
            state->row = row;
            state->col = static_cast<unsigned int>(index);
            return true;
        }
        if (row == 0) {
            row = state->data.size() - 1;
        } else {
            row--;
        }
        isFirst = false;
    } while (row != initialRow);
    // try last row again
    std::string line = state->data[row];
    size_t index = line.rfind(state->searchQuery);
    if (index != std::string::npos) {
        state->row = row;
        state->col = static_cast<unsigned int>(index);
        return true;
    }
    return false;
}

// TODO make this look cleaner
bool setSearchResult(State* state) {
    fixColOverMax(state);
    unsigned int initialCol = state->col;
    unsigned int initialRow = state->row;
    unsigned int col = initialCol;
    unsigned int row = initialRow;
    do {
        std::string line = state->data[row].substr(col);
        size_t index = line.find(state->searchQuery);
        if (index != std::string::npos) {
            state->row = row;
            state->col = static_cast<unsigned int>(index) + col;
            return true;
        }
        row = (row + 1) % state->data.size();
        col = 0;
    } while (row != initialRow);
    // try last row again
    std::string line = state->data[row];
    size_t index = line.find(state->searchQuery);
    if (index != std::string::npos) {
        state->row = row;
        state->col = static_cast<unsigned int>(index) + col;
        return true;
    }
    return false;
}

void setPosition(State* state, Position pos) {
    state->row = pos.row;
    state->col = pos.col;
}

void initVisual(State* state, VisualType visualType) {
    state->visualType = visualType;
    state->visual.row = state->row;
    state->visual.col = state->col;
}

bool is_number(const std::string& s) {
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it))
        ++it;
    return !s.empty() && it == s.end();
}

char ctrl(char c) { return c - 'a' + 1; }

unsigned int getIndent(const std::string& str) {
    for (unsigned int i = 0; i < str.length(); i++) {
        if (str[i] != ' ') {
            return i;
        }
    }
    return 0;
}

unsigned int getPrevEmptyLine(State* state) {
    bool hitNonEmpty = false;
    for (int i = (int)state->row; i >= 0; i--) {
        if (state->data[i] != "") {
            hitNonEmpty = true;
        } else if (hitNonEmpty && state->data[i] == "") {
            return i;
        }
    }
    return state->row;
}

unsigned int getNextEmptyLine(State* state) {
    bool hitNonEmpty = false;
    for (unsigned int i = state->row; i < state->data.size(); i++) {
        if (state->data[i] != "") {
            hitNonEmpty = true;
        } else if (hitNonEmpty && state->data[i] == "") {
            return i;
        }
    }
    return state->row;
}

unsigned int getPrevLineSameIndent(State* state) {
    unsigned int current = getIndent(state->data[state->row]);
    for (int i = (int)state->row - 1; i >= 0; i--) {
        if (current == getIndent(state->data[i]) && state->data[i] != "") {
            return i;
        }
    }
    return state->row;
}

unsigned int getNextLineSameIndent(State* state) {
    unsigned int current = getIndent(state->data[state->row]);
    for (unsigned int i = state->row + 1; i < state->data.size(); i++) {
        if (current == getIndent(state->data[i]) && state->data[i] != "") {
            return i;
        }
    }
    return state->row;
}

WordPosition findQuoteBounds(const std::string& str, char quoteChar, unsigned int cursor, bool includeQuote) {
    int lastQuoteIndex = -1;
    for (unsigned int i = 0; i <= cursor; i++) {
        if (str[i] == quoteChar) {
            lastQuoteIndex = i;
        }
    }
    unsigned int i;
    for (i = cursor + 1; i < str.length(); i++) {
        if (str[i] == quoteChar) {
            if (lastQuoteIndex != -1 && lastQuoteIndex < (int)cursor) {
                break;
            } else {
                if (lastQuoteIndex == -1) {
                    lastQuoteIndex = i;
                } else {
                    break;
                }
            }
        }
    }
    if (i != str.length()) {
        if (i - lastQuoteIndex == 1 || includeQuote) {
            return {(unsigned int)lastQuoteIndex, (unsigned int)i};
        } else {
            return {(unsigned int)lastQuoteIndex + 1, (unsigned int)i - 1};
        }
    } else {
        return {0, 0};
    }
}

WordPosition findParentheses(const std::string& str, char openParen, char closeParen, unsigned int cursor, bool includeParen) {
    int balance = 0;
    int openParenIndex = -1;
    // look back for openParen
    for (int i = cursor; i >= 0; i--) {
        if (str[i] == openParen) {
            if (balance == 0) {
                openParenIndex = i;
                break;
            } else {
                balance--;
            }
        } else if (str[i] == closeParen && i != (int) cursor) {
            balance++;
        }
    }
    balance = 0;
    // if haven't found yet look forward for openParen
    if (openParenIndex == -1) {
        for (int i = cursor; i < (int)str.length(); i++) {
            if (str[i] == openParen) {
                if (balance == 0) {
                    openParenIndex = i;
                    break;
                } else {
                    balance--;
                }
            } else if (str[i] == closeParen) {
                balance++;
            }
        }
    }
    balance = 0;
    // if haven't found return {0,0}
    if (openParenIndex == -1) {
        return {0, 0};
    }
    for (int i = openParenIndex + 1; i < (int)str.length(); i++) {
        if (str[i] == openParen) {
            balance--;
        } else if (str[i] == closeParen) {
            if (balance == 0) {
                if (i - openParenIndex == 1 || includeParen) {
                    return {(unsigned int)openParenIndex, (unsigned int)i};
                } else {
                    return {(unsigned int)openParenIndex + 1, (unsigned int)i - 1};
                }
            } else {
                balance++;
            }
        }
    }
    return {0, 0};
}

WordPosition getWordPosition(const std::string& str, unsigned int cursor) {
    if (cursor >= str.size()) {
        return {0, 0};
    }
    // Move cursor to the start of the current chunk
    while (cursor > 0 && str[cursor - 1] != ' ' && str[cursor] != ' ' && (isAlphanumeric(str[cursor]) == isAlphanumeric(str[cursor - 1]))) {
        cursor--;
    }
    // If cursor is on a space, move to the next chunk
    if (str[cursor] == ' ') {
        while (cursor < str.size() && str[cursor] == ' ') {
            cursor++;
        }
    }
    // If no non-space chunk is found
    if (cursor >= str.size()) {
        return {0, 0};
    }
    // Find the end of the chunk
    unsigned int start = cursor;
    unsigned int end = start;
    while (end < str.size() && str[end] != ' ' && (isAlphanumeric(str[start]) == isAlphanumeric(str[end]))) {
        end++;
    }

    return {start, end - 1};
}

int maxConsecutiveMatch(const std::filesystem::path& filePath, const std::string& query) {
    std::string filePathStr = filePath.string();
    std::transform(filePathStr.begin(), filePathStr.end(), filePathStr.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    std::string queryLower = query;
    std::transform(queryLower.begin(), queryLower.end(), queryLower.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    int maxLength = 0;
    int currentLength = 0;
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

bool filePathContainsSubstring(const std::filesystem::path& filePath, const std::string& query) {
    std::string filePathStr = filePath.string();
    std::string queryLower = query;

    std::transform(filePathStr.begin(), filePathStr.end(), filePathStr.begin(), [](unsigned char c) { return std::tolower(c); });
    std::transform(queryLower.begin(), queryLower.end(), queryLower.begin(), [](unsigned char c) { return std::tolower(c); });

    unsigned int filePathIndex = 0;
    unsigned int queryIndex = 0;
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

bool shouldIgnoreFile(const std::filesystem::path& path) {
    std::vector<std::string> allowList = {"[...nextauth]", ".github"};
    for (unsigned int i = 0; i < allowList.size(); i++) {
        if (path.string().find(allowList[i]) != std::string::npos) {
            return false;
        }
    }
    std::vector<std::string> ignoreList = {".git", "node_modules", "build", "dist", "cdk.out", ".next", "tmp", "coverage", ".png", "package-lock.json"};
    for (unsigned int i = 0; i < ignoreList.size(); i++) {
        if (path.string().find(ignoreList[i]) != std::string::npos) {
            return true;
        }
    }
    return false;
}

std::vector<grepMatch> grepFile(const std::filesystem::path& file_path, const std::string& query, const std::filesystem::path& dir_path) {
    auto relativePath = file_path.lexically_relative(dir_path);
    std::vector<grepMatch> matches;
    std::ifstream file(file_path);
    std::string line;
    int lineNumber = 0;
    while (std::getline(file, line)) {
        lineNumber++;
        if (line.find(query) != std::string::npos) {
            matches.emplace_back(relativePath, lineNumber, line);
        }
    }
    return matches;
}

std::vector<grepMatch> grepFiles(const std::filesystem::path& dir_path, const std::string& query) {
    std::vector<std::future<std::vector<grepMatch>>> futures;
    for (auto it = std::filesystem::recursive_directory_iterator(dir_path); it != std::filesystem::recursive_directory_iterator(); ++it) {
        if (shouldIgnoreFile(it->path())) {
            it.disable_recursion_pending();
            continue;
        }
        if (std::filesystem::is_regular_file(it->path())) {
            futures.push_back(std::async(std::launch::async, grepFile, it->path(), query, dir_path));
        }
    }
    std::vector<grepMatch> allMatches;
    for (auto& future : futures) {
        auto matches = future.get();
        allMatches.insert(allMatches.end(), matches.begin(), matches.end());
    }

    return allMatches;
}

std::vector<std::filesystem::path> findFiles(const std::filesystem::path& dir_path, const std::string& query) {
    std::vector<std::filesystem::path> matching_files;
    for (auto it = std::filesystem::recursive_directory_iterator(dir_path); it != std::filesystem::recursive_directory_iterator(); ++it) {
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
    std::sort(matching_files.begin(), matching_files.end(), [&](const std::filesystem::path& a, const std::filesystem::path& b) {
        int matchA = maxConsecutiveMatch(a, query);
        int matchB = maxConsecutiveMatch(b, query);
        if (matchA == matchB) {
            return a.string() < b.string(); // Alphabetical order if matches are equal
        }
        return matchA > matchB; // Descending order of matches
    });
    return matching_files;
}

void generateGrepOutput(State* state) {
    if (state->grepQuery == "") {
        state->grepOutput.clear();
    } else {
        state->grepOutput = grepFiles(std::filesystem::current_path(), state->grepQuery);
    }
}

void generateFindFileOutput(State* state) { state->findFileOutput = findFiles(std::filesystem::current_path(), state->findFileQuery); }

unsigned int w(State* state) {
    bool space = state->data[state->row][state->col] == ' ';
    for (unsigned int i = state->col + 1; i < state->data[state->row].size(); i += 1) {
        if (state->data[state->row][i] == ' ') {
            space = true;
        } else if (space && state->data[state->row][i] != ' ') {
            return i;
        } else if (isAlphanumeric(state->data[state->row][state->col]) != isAlphanumeric(state->data[state->row][i])) {
            return i;
        }
    }
    return state->col;
}

unsigned int b(State* state) {
    if (state->col == 0 || state->data[state->row].empty())
        return 0;
    int i = state->col - 1;
    while (i >= 0 && state->data[state->row][i] == ' ')
        i--;
    if (i < 0)
        return 0;
    bool isAlnum = isAlphanumeric(state->data[state->row][i]);
    for (i -= 1; i >= 0; i--) {
        if (state->data[state->row][i] == ' ') {
            return i + 1;
        } else if ((isAlphanumeric(state->data[state->row][i])) != isAlnum) {
            return i + 1;
        }
    }
    return 0;
}

void saveFile(std::string filename, std::vector<std::string> data) {
    std::ofstream file(filename);
    if (!data.empty()) {
        for (size_t i = 0; i < data.size() - 1; ++i) {
            file << data[i] << "\n";
        }
        file << data.back();
    }
}

std::vector<std::string> readFile(std::string filename) {
    std::ifstream file(filename);
    std::string str;
    std::vector<std::string> file_contents;
    while (file.good()) {
        std::getline(file, str);
        file_contents.push_back(str);
    }
    return file_contents;
}

bool isWindowPositionInvalid(State* state) {
    if (state->row < state->windowPosition.row || (int)state->row - (int)state->windowPosition.row > ((int)state->maxY - 2)) {
        return true;
    } else if (state->col < state->windowPosition.col || (int)state->col - (int)state->windowPosition.col > ((int)state->maxX - (int)getLineNumberOffset(state) - 1)) {
        return true;
    }
    return false;
}

void centerScreen(State* state) {
    if (state->row < state->maxY / 2) {
        state->windowPosition.row = 0;
    } else {
        state->windowPosition.row = state->row - state->maxY / 2;
    }
    if (state->col < state->windowPosition.col) {
        state->windowPosition.col = state->col;
    } else if (state->col > state->windowPosition.col + (state->maxX - getLineNumberOffset(state) - 1)) {
        state->windowPosition.col = state->col + getLineNumberOffset(state) + 1 - state->maxX;
    }
}

void upHalfScreen(State* state) {
    for (unsigned int i = 0; i < state->maxY / 2; i++) {
        if (state->row > 0) {
            state->row -= 1;
            state->windowPosition.row -= 1;
        }
    }
}

void downHalfScreen(State* state) {
    for (unsigned int i = 0; i < state->maxY / 2; i++) {
        if (state->row < state->data.size() - 1) {
            state->row += 1;
            state->windowPosition.row += 1;
        }
    }
}

void up(State* state) {
    if (state->row > 0) {
        state->row -= 1;
    }
    if (state->row < state->windowPosition.row) {
        state->windowPosition.row -= 1;
    }
}

void down(State* state) {
    if (state->row < state->data.size() - 1) {
        state->row += 1;
    }
    if ((int)state->row - (int)state->windowPosition.row > ((int)state->maxY - 2)) {
        state->windowPosition.row += 1;
    }
}

void left(State* state) {
    if (state->col >= state->data[state->row].length()) {
        state->col = state->data[state->row].length();
    }
    if (state->col > 0) {
        state->col -= 1;
    }
}

void right(State* state) {
    if (state->col >= state->data[state->row].length()) {
        state->col = state->data[state->row].length();
    }
    if (state->col < state->data[state->row].length()) {
        state->col += 1;
    }
}

void indent(State* state) {
    for (unsigned int i = 0; i < state->indent; i++) {
        state->data[state->row] = " " + state->data[state->row];
    }
}

void deindent(State* state) {
    for (unsigned int i = 0; i < state->indent; i++) {
        if (state->data[state->row].substr(0, 1) == " ") {
            state->data[state->row] = state->data[state->row].substr(1);
        }
    }
}

int getIndexFirstNonSpace(State* state) {
    int i;
    for (i = 0; i < (int)state->data[state->row].length(); i++) {
        if (state->data[state->row][i] != ' ') {
            return i;
        }
    }
    return i;
}

void calcWindowBounds() {
    int y, x;
    getmaxyx(stdscr, y, x);
    State::setMaxYX(y, x);
}

void insertEmptyLineBelow(State* state) { state->data.insert(state->data.begin() + state->row + 1, ""); }

void insertEmptyLine(State* state) { state->data.insert(state->data.begin() + state->row, ""); }

int maximum(int a, int b) {
    if (a > b) {
        return a;
    } else {
        return b;
    }
}

int minimum(int a, int b) {
    if (a <= b) {
        return a;
    } else {
        return b;
    }
}

void sanityCheckDocumentEmpty(State* state) {
    if (state->data.size() == 0) {
        state->data.push_back("");
    }
}

void fixColOverMax(State* state) {
    if (state->col > state->data[state->row].length()) {
        state->col = state->data[state->row].length();
    }
}

void sanityCheckRowColOutOfBounds(State* state) {
    if (state->row >= state->data.size()) {
        state->row = state->data.size() - 1;
    }
    if (state->mode == TYPING) {
        fixColOverMax(state);
    }
}
