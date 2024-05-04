#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <regex>

#include "helper.h"
#include "insertLoggingCode.h"
#include "modes.h"
#include "render.h"
#include "state.h"
#include <iostream>
#include <ncurses.h>

#define _COLOR_BLACK 16
#define _COLOR_GREY 242
#define _COLOR_RED 196
#define _COLOR_GREEN 46
#define _COLOR_YELLOW 226
#define _COLOR_BLUE 21
#define _COLOR_MAGENTA 201
#define _COLOR_CYAN 51
#define _COLOR_WHITE 231

#define BLACK 1
#define GREY 2
#define RED 3
#define GREEN 4
#define YELLOW 5
#define BLUE 6
#define MAGENTA 7
#define CYAN 8
#define WHITE 9

int invertColor(int color) { return color + 9; }

void initColors() {
    init_pair(BLACK, _COLOR_BLACK, _COLOR_BLACK);
    init_pair(GREY, _COLOR_GREY, _COLOR_BLACK);
    init_pair(RED, _COLOR_RED, _COLOR_BLACK);
    init_pair(GREEN, _COLOR_GREEN, _COLOR_BLACK);
    init_pair(YELLOW, _COLOR_YELLOW, _COLOR_BLACK);
    init_pair(BLUE, _COLOR_BLUE, _COLOR_BLACK);
    init_pair(MAGENTA, _COLOR_MAGENTA, _COLOR_BLACK);
    init_pair(CYAN, _COLOR_CYAN, _COLOR_BLACK);
    init_pair(WHITE, _COLOR_WHITE, _COLOR_BLACK);

    init_pair(invertColor(BLACK), _COLOR_BLACK, _COLOR_BLACK);
    init_pair(invertColor(GREY), _COLOR_BLACK, _COLOR_GREY);
    init_pair(invertColor(RED), _COLOR_BLACK, _COLOR_RED);
    init_pair(invertColor(GREEN), _COLOR_BLACK, _COLOR_GREEN);
    init_pair(invertColor(YELLOW), _COLOR_BLACK, _COLOR_YELLOW);
    init_pair(invertColor(BLUE), _COLOR_BLACK, _COLOR_BLUE);
    init_pair(invertColor(MAGENTA), _COLOR_BLACK, _COLOR_MAGENTA);
    init_pair(invertColor(CYAN), _COLOR_BLACK, _COLOR_CYAN);
    init_pair(invertColor(WHITE), _COLOR_BLACK, _COLOR_WHITE);
}

int renderStatusBar(State* state) {
    int offset = 0;
    if (state->showFileStack == true) {
        // TODO make work for fileStack > maxY
        for (int i = (int)(state->fileStack.size() - 1); i >= 0; i--) {
            if (i == (int)state->fileStackIndex) {
                attron(COLOR_PAIR(RED));
            }
            mvprintw(state->fileStack.size() - i - 1, state->maxX - state->fileStack[i].length() - 2, "\"%s\"", state->fileStack[i].c_str());
            if (i == (int)state->fileStackIndex) {
                attroff(COLOR_PAIR(RED));
            }
        }
    } else {
        auto apm = calculateAPM(state);
        auto stringAPM = std::to_string(apm);
        mvprintw(0, state->maxX - (stringAPM.length() + state->filename.length() + state->prevKeys.length() + 4), "%s ", state->prevKeys.c_str());
        if (apm > 300) {
            attron(COLOR_PAIR(RED));
        } else if (apm > 200) {
            attron(COLOR_PAIR(YELLOW));
        }
        mvprintw(0, state->maxX - (stringAPM.length() + state->filename.length() + 3), "%s ", stringAPM.c_str());
        if (apm > 300) {
            attroff(COLOR_PAIR(RED));
        } else if (apm > 200) {
            attroff(COLOR_PAIR(YELLOW));
        }
        mvprintw(0, state->maxX - (state->filename.length() + 2), "\"%s\"", state->filename.c_str());
    }
    if (state->status.length() > 0) {
        attron(COLOR_PAIR(RED));
        mvprintw(0, offset, "%s ", state->status.c_str());
        attroff(COLOR_PAIR(RED));
        offset += state->status.length() + 1;
    }
    if (state->mode == COMMANDLINE) {
        mvprintw(0, offset, ":%s", state->commandLine.query.c_str());
        offset += state->commandLine.cursor + 1;
        return offset;
    } else if (state->mode == GREP) {
        attron(COLOR_PAIR(GREEN));
        mvprintw(0, offset, "> %s", state->grep.query.c_str());
        attroff(COLOR_PAIR(GREEN));
        offset += state->grep.cursor + 2;
        return offset;
    } else if (state->mode == FINDFILE) {
        attron(COLOR_PAIR(YELLOW));
        mvprintw(0, offset, "> ");
        attroff(COLOR_PAIR(YELLOW));
        offset += 2;
        if (state->selectAll) {
            attron(COLOR_PAIR(invertColor(YELLOW)));
        }
        mvprintw(0, offset, "%s", state->findFile.query.c_str());
        offset += state->findFile.cursor;
        if (state->selectAll) {
            attroff(COLOR_PAIR(invertColor(YELLOW)));
        }
        return offset;
    } else {
        if (state->searchFail) {
            attron(COLOR_PAIR(RED));
        } else {
            attron(COLOR_PAIR(GREEN));
        }
        std::string displayQuery = state->search.query;
        for (size_t i = 0; i < displayQuery.length(); ++i) {
            if (displayQuery[i] == '\n') {
                displayQuery.replace(i, 1, "\\n");
                i++;
            }
        }
        if (displayQuery.length() > 60) {
            displayQuery = safeSubstring(displayQuery, 0, 60);
            displayQuery += "...";
        }
        mvprintw(0, offset, "/%s", displayQuery.c_str());
        offset += displayQuery.length() + 1;
        if (state->searchFail) {
            attroff(COLOR_PAIR(RED));
        } else {
            attroff(COLOR_PAIR(GREEN));
        }
        if (state->replacing) {
            attron(COLOR_PAIR(MAGENTA));
            mvprintw(0, offset, "/%s", state->replace.query.c_str());
            offset += state->replace.query.length() + 1;
            attroff(COLOR_PAIR(MAGENTA));
        }
        for (unsigned int i = 0; i < state->harpoonFiles.size(); i++) {
            if (state->harpoonFiles[i] == state->filename) {
                attron(COLOR_PAIR(YELLOW));
            } else {
                attron(COLOR_PAIR(GREY));
            }
            auto min_name = minimize_filename(state->harpoonFiles[i]);
            mvprintw(0, offset, " %s", min_name.c_str());
            offset += min_name.length() + 1;
            if (state->harpoonFiles[i] == state->filename) {
                attroff(COLOR_PAIR(YELLOW));
            } else {
                attroff(COLOR_PAIR(GREY));
            }
        }
        if (state->replacing) {
            return state->search.query.length() + state->replace.cursor + 2;
        } else if (state->mode == SEARCH) {
            return state->search.cursor + 1;
        }
    }
    return -1;
}

std::string minimize_filename(const std::string& filename) {
    std::vector<std::string> parts;
    std::stringstream ss(filename);
    std::string part;
    std::string minimized;
    while (std::getline(ss, part, '/')) {
        parts.push_back(part);
    }
    for (size_t i = 0; i < parts.size() - 1; ++i) {
        if (!parts[i].empty()) {
            minimized += parts[i][0];
            minimized += '/';
        }
    }
    minimized += parts.back();
    return minimized;
}

int getColorFromChar(char c) {
    if (c == '[' || c == ']') {
        return GREEN;
    } else if (c == '(' || c == ')') {
        return YELLOW;
    } else if (c == '{' || c == '}') {
        return MAGENTA;
    } else if (c == '\'' || c == '"' || c == '`') {
        return CYAN;
    } else {
        return WHITE;
    }
}

int getSearchColor(State* state, int row, unsigned int startOfSearch) {
    if (state->row == (unsigned int)row && startOfSearch + state->search.query.length() >= state->col && startOfSearch <= state->col) {
        return invertColor(MAGENTA);
    } else {
        return invertColor(CYAN);
    }
}

bool isMergeConflict(const std::string& str) {
    const std::vector<std::string> markers = {"<<<<<<<", "=======", ">>>>>>>", "|||||||"};
    for (const auto& marker : markers) {
        if (str.length() >= marker.length()) {
            if (str.substr(0, marker.length()) == marker) {
                return true;
            }
        }
    }
    return false;
}

void printChar(State* state, int row, int col, char c, int color) {
    if (' ' <= c && c <= '~') {
        attron(COLOR_PAIR(color));
        mvaddch(row - state->windowPosition.row + 1, col + getLineNumberOffset(state), c);
        attroff(COLOR_PAIR(color));
    } else if (c == '\t') {
        attron(COLOR_PAIR(invertColor(RED)));
        mvaddch(row - state->windowPosition.row + 1, col + getLineNumberOffset(state), ' ');
        attroff(COLOR_PAIR(invertColor(RED)));
    } else if (' ' <= unctrl(c) && unctrl(c) <= '~') {
        attron(COLOR_PAIR(invertColor(MAGENTA)));
        mvaddch(row - state->windowPosition.row + 1, col + getLineNumberOffset(state), unctrl(c));
        attroff(COLOR_PAIR(invertColor(MAGENTA)));
    } else {
        attron(COLOR_PAIR(invertColor(MAGENTA)));
        mvaddch(row - state->windowPosition.row + 1, col + getLineNumberOffset(state), ' ');
        attroff(COLOR_PAIR(invertColor(MAGENTA)));
    }
}

void printLineNumber(State* state, int r, int i, bool isCurrentRow, bool recording, std::string blame) {
    if (isCurrentRow == true) {
        attron(COLOR_PAIR(WHITE));
        mvprintw(r, 0, "%5d", i + 1);
        attroff(COLOR_PAIR(WHITE));
    } else if (recording) {
        attron(COLOR_PAIR(RED));
        mvprintw(r, 0, "%5d", i + 1);
        attroff(COLOR_PAIR(RED));
    } else {
        attron(COLOR_PAIR(GREY));
        mvprintw(r, 0, "%5d", i + 1);
        attroff(COLOR_PAIR(GREY));
    }
    bool isLogging = std::regex_search(state->data[i], std::regex(getLoggingRegex(state)));
    char spacingChar = ((int)state->mark.mark == i && state->mark.filename == state->filename) || isLogging ? '|' : ' ';
    if (isLogging) {
        attron(COLOR_PAIR(YELLOW));
    } else {
        attron(COLOR_PAIR(CYAN));
    }
    mvprintw(r, 5, "%c", spacingChar);
    if (isLogging) {
        attroff(COLOR_PAIR(YELLOW));
    } else {
        attroff(COLOR_PAIR(CYAN));
    }
    if (state->mode == BLAME) {
        if (i == (int)state->row) {
            attron(COLOR_PAIR(invertColor(WHITE)));
        }
        mvprintw(r, 6, "%-65s", blame.substr(0, 65).c_str());
        if (i == (int)state->row) {
            attroff(COLOR_PAIR(invertColor(WHITE)));
        }
    }
}

bool isRowColInVisual(State* state, unsigned int i, unsigned int j) {
    if (state->mode == VISUAL) {
        unsigned int minR;
        unsigned int minC;
        unsigned int maxR;
        unsigned int maxC;
        if (state->row < state->visual.row) {
            minR = state->row;
            minC = state->col;
            maxR = state->visual.row;
            maxC = state->visual.col;
        } else {
            minR = state->visual.row;
            minC = state->visual.col;
            maxR = state->row;
            maxC = state->col;
        }
        if (state->visualType == LINE) {
            if (minR <= i && i <= maxR) {
                return true;
            }
        } else if (state->visualType == BLOCK) {
            if (minR <= i && i <= maxR) {
                return (minC <= j && j <= maxC) || (maxC <= j && j <= minC);
            }
        } else if (state->visualType == NORMAL) {
            if (minR < i && i < maxR) {
                return true;
            } else if (minR == i && maxR == i) {
                return (minC <= j && j <= maxC) || (maxC <= j && j <= minC);
            } else if (minR == i) {
                return minC <= j;
            } else if (maxR == i) {
                return maxC >= j;
            }
        }
    }
    return false;
}

bool isInSearchQuery(State* state, unsigned int row, unsigned int col) {
    if (state->search.query != "" && col + state->search.query.length() <= state->data[row].length()) {
        if (state->search.query == state->data[row].substr(col, state->search.query.length())) {
            return true;
        }
    }
    return false;
}

void printLine(State* state, int row) {
    if (isRowColInVisual(state, row, 0) == true && state->data[row].length() == 0) {
        printChar(state, row, 0, ' ', invertColor(WHITE));
    } else {
        bool isInString = false;
        bool skipNext = false;
        unsigned int searchCounter = 0;
        int renderCol = 0;
        unsigned int startOfSearch = 0;
        bool isComment = false;
        char stringType;
        unsigned int col = 0;
        while (col < state->data[row].length() && col < state->windowPosition.col + state->maxX - getLineNumberOffset(state)) {
            renderCol = renderAutoComplete(state, row, col, renderCol);
            if (searchCounter == 0 && isInSearchQuery(state, row, col)) {
                searchCounter = state->search.query.length();
                startOfSearch = col;
            }
            if (skipNext == false) {
                char current = state->data[row][col];
                if (isInString && current == '\\') {
                    skipNext = true;
                } else {
                    if (isInString == false && (current == '"' || current == '`' || current == '\'')) {
                        isInString = true;
                        stringType = current;
                    } else if (isInString == true && current == stringType) {
                        isInString = false;
                    }
                }
            } else {
                skipNext = false;
            }
            if (!isInString && state->data[row].substr(col, state->commentSymbol.length()) == state->commentSymbol && state->commentSymbol != "") {
                isComment = true;
            }
            if (col >= state->windowPosition.col) {
                if (state->replacing && searchCounter != 0) {
                    for (unsigned int i = 0; i < state->replace.query.length(); i++) {
                        printChar(state, row, renderCol, state->replace.query[i], getSearchColor(state, row, startOfSearch));
                        renderCol++;
                    }
                    col += state->search.query.length();
                    searchCounter = 0;
                } else {
                    int color;
                    if (state->matching.row == (unsigned int)row && state->matching.col == col && (state->matching.row != state->row || state->matching.col != state->col)) {
                        color = invertColor(GREY);
                    } else {
                        if (searchCounter != 0 && state->searching == true) {
                            color = getSearchColor(state, row, startOfSearch);
                        } else if (isMergeConflict(state->data[row])) {
                            color = RED;
                        } else if (isComment) {
                            color = GREEN;
                        } else if (isInString == true && getExtension(state->filename) != "md" && getExtension(state->filename) != "txt") {
                            color = CYAN;
                        } else {
                            color = getColorFromChar(state->data[row][col]);
                        }
                        if (isRowColInVisual(state, row, col)) {
                            color = invertColor(color);
                        }
                    }
                    printChar(state, row, renderCol, state->data[row][col], color);
                    renderCol++;
                    col++;
                }
            } else {
                col++;
            }
            if (searchCounter != 0) {
                searchCounter -= 1;
            }
        }
        renderCol = renderAutoComplete(state, row, col, renderCol);
    }
}

bool isRowInVisual(State* state, int row) { return ((int)state->visual.row <= row && row <= (int)state->row) || ((int)state->row <= row && row <= (int)state->visual.row); }

unsigned int renderAutoComplete(State* state, int row, unsigned int col, unsigned int renderCol) {
    if ((state->mode == TYPING || state->mode == MULTICURSOR) && row == (int)state->row && col == state->col && isRowInVisual(state, row)) {
        std::string completion = autocomplete(state, getCurrentWord(state));
        if (state->data[row].substr(col, completion.length()) != completion) {
            for (unsigned int i = 0; i < completion.length(); i++) {
                printChar(state, row, col + i, completion[i], GREY);
            }
            return renderCol + completion.length();
        }
    }
    return renderCol;
}

void renderGrepOutput(State* state) {
    unsigned int index;
    if ((int)state->grep.selection - ((int)state->maxY / 2) > 0) {
        index = state->grep.selection - state->maxY / 2;
    } else {
        index = 0;
    }
    unsigned int renderIndex = 1;
    for (unsigned int i = index; i < state->grepOutput.size() && i < index + state->maxY; i++) {
        if (i == state->grep.selection) {
            attron(COLOR_PAIR(invertColor(WHITE)));
            mvprintw(renderIndex, 0, "%s:%d %s\n", state->grepOutput[i].path.c_str(), state->grepOutput[i].lineNum, state->grepOutput[i].line.c_str());
            attroff(COLOR_PAIR(invertColor(WHITE)));
        } else {
            mvprintw(renderIndex, 0, "%s:%d %s\n", state->grepOutput[i].path.c_str(), state->grepOutput[i].lineNum, state->grepOutput[i].line.c_str());
        }
        renderIndex++;
    }
}

void renderFindFileOutput(State* state) {
    unsigned int index;
    if ((int)state->findFile.selection - ((int)state->maxY / 2) > 0) {
        index = state->findFile.selection - state->maxY / 2;
    } else {
        index = 0;
    }
    unsigned int renderIndex = 1;
    for (unsigned int i = index; i < state->findFileOutput.size() && i < index + state->maxY; i++) {
        if (i == state->findFile.selection) {
            attron(COLOR_PAIR(invertColor(WHITE)));
            mvprintw(renderIndex, 0, "%s\n", state->findFileOutput[i].c_str());
            attroff(COLOR_PAIR(invertColor(WHITE)));
        } else {
            mvprintw(renderIndex, 0, "%s\n", state->findFileOutput[i].c_str());
        }
        renderIndex++;
    }
}

void renderVisibleLines(State* state) {
    std::vector<std::string> blame;
    bool blameError = false;
    if (state->mode == BLAME) {
        try {
            blame = getGitBlame(state->filename);
        } catch (const std::exception& e) {
            state->status = "git blame error";
            blameError = true;
        }
    }
    for (int i = state->windowPosition.row; i < (int)state->data.size() && i < (int)(state->maxY + state->windowPosition.row) - 1; i++) {
        printLineNumber(state, i - state->windowPosition.row + 1, i, i == (int)state->row, state->recording,
                        state->mode == BLAME && !blameError && blame.size() >= state->data.size() && i < (int)state->data.size() ? blame[i] : "");
        printLine(state, i);
    }
}

void moveCursor(State* state, int cursorPosition) {
    if (cursorPosition != -1) {
        move(0, cursorPosition);
    } else {
        unsigned int row = state->row + 1;
        if (row > state->windowPosition.row) {
            row -= state->windowPosition.row;
        } else {
            row = 1;
        }
        unsigned int col = state->col + getLineNumberOffset(state);
        if (state->col > state->data[state->row].length()) {
            col = state->data[state->row].length() + getLineNumberOffset(state);
            if (state->windowPosition.col > col) {
                col = getLineNumberOffset(state);
            } else {
                col -= state->windowPosition.col;
            }
        } else if (col > state->windowPosition.col) {
            col -= state->windowPosition.col;
        }
        move(row, col);
    }
}

void renderScreen(State* state) {
    erase();
    if (state->mode == FINDFILE) {
        renderFindFileOutput(state);
    } else if (state->mode == GREP) {
        renderGrepOutput(state);
    } else {
        renderVisibleLines(state);
    }
    int cursorPosition = renderStatusBar(state);
    moveCursor(state, cursorPosition);
    wnoutrefresh(stdscr);
    doupdate();
}

void initTerminal() {
    initscr();
    raw();
    keypad(stdscr, true);
    noecho();
    if (has_colors() == false) {
        endwin();
        std::cout << "Your terminal does not support color" << std::endl;
        exit(1);
    }
    start_color();
    if (COLORS < 256) {
        endwin();
        std::cout << "Your terminal does not support 256 colors" << std::endl;
        exit(1);
    }
    initColors();
}
