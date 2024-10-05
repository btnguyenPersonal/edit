#include "keybinds/sendKeys.h"
#include "util/helper.h"
#include "util/render.h"
#include "util/state.h"
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <ncurses.h>
#include <unistd.h>
#include <sys/select.h>

int main(int argc, char* argv[]) {
    int c;
    State* state;
    int stdinFlag = 0;
    int commandFlag = 0;
    std::string filename;
    for (int i = 1; i < argc; ++i) {
        if (strncmp(argv[i], "-c", 2) == 0) {
            commandFlag = i;
            break;
        } else if (strncmp(argv[i], "-", 1) == 0) {
            stdinFlag = i;
            break;
        } else {
            filename = argv[i];
        }
    }
    if (stdinFlag != 0) {
        std::vector<std::string> data;
        for (std::string line; std::getline(std::cin, line);) {
            data.push_back(line);
        }
        state = new State(data);
    } else if (!filename.empty()) {
        state = new State(filename);
    } else {
        state = new State();
        generateFindFileOutput(state);
    }
    if (commandFlag != 0) {
        state->dontSave = true;
        for (int i = commandFlag + 1; i < argc; ++i) {
            const std::string& sequence = argv[i];
            for (size_t j = 0; j < sequence.size(); ++j) {
                sendKeys(state, sequence[j]);
            }
        }
        return 0;
    }
    close(0);
    open("/dev/tty", O_RDONLY);
    initTerminal();
    calcWindowBounds();
    centerFileExplorer(state);
    renderScreen(state);
    while (true) {
        c = getch();
        if (c != ERR) {
            sendKeys(state, c);
            renderScreen(state);
        }
    }
    endwin();
    return 0;
}
