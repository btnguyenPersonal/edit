#include "keybinds/sendKeys.h"
#include "util/helper.h"
#include "util/render.h"
#include "util/state.h"
#include <cstdint>

int main(int argc, char* argv[]) {
    int c;
    State* state;
    std::string filename;
    if (argc == 2) {
        filename = argv[1];
    }
    if (!filename.empty()) {
        state = new State(filename);
    } else {
        state = new State();
        generateFindFileOutput(state);
    }
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
