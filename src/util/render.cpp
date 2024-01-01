#include <ncurses.h>
#include "state.h"
#include "modes.h"

#define BLACK 232
#define GREY 242
#define RED 196
#define GREEN 46
#define YELLOW 226
#define BLUE 21
#define MAGENTA 201
#define CYAN 51
#define WHITE 231

void initColors() {
    init_pair(BLACK, BLACK, COLOR_BLACK);
    init_pair(GREY, GREY, COLOR_BLACK);
    init_pair(RED, RED, COLOR_BLACK);
    init_pair(GREEN, GREEN, COLOR_BLACK);
    init_pair(YELLOW, YELLOW, COLOR_BLACK);
    init_pair(BLUE, BLUE, COLOR_BLACK);
    init_pair(MAGENTA, MAGENTA, COLOR_BLACK);
    init_pair(CYAN, CYAN, COLOR_BLACK);
    init_pair(WHITE, WHITE, COLOR_BLACK);

}

int renderStatusBar(State state) {
    int offset = 0;
    if (state.status.length() > 0) {
        attron(COLOR_PAIR(RED));
        mvprintw(0, 0, "%s ", state.status.c_str());
        attroff(COLOR_PAIR(RED));
        offset += state.status.length() + 2;
    }
    if (state.mode == COMMANDLINE) {
        mvprintw(0, offset, ":%s", state.commandLineQuery.c_str());
        offset += state.commandLineQuery.length() + 1;
        return offset;
    }
    return 0;
}

void renderVisibleLines(State state) {
    // TODO fix maxX as well
    for (int i = state.windowPosition; i < (int) state.data.size() && i < (int) (state.maxY + state.windowPosition); i++) {
        attron(COLOR_PAIR(GREY));
        mvprintw(i - state.windowPosition + 1, 0, "%4d ", i + 1);
        attroff(COLOR_PAIR(GREY));
        mvprintw(i - state.windowPosition + 1, 5, "%s\n", state.data[i].c_str());
    }
}

void moveCursor(State state, int commandLineCursorPosition) {
    if (commandLineCursorPosition != 0) {
        move(0, commandLineCursorPosition);
    } else {
        uint row = state.row + 1;
        if (row > state.windowPosition) {
            row -= state.windowPosition;
        } else {
            row = 1;
        }
        move(row, state.col + 5);
    }
}

void renderScreen(State state) {
    // TODO partial update
    clear();
    initColors();
    renderVisibleLines(state);
    int commandLineCursorPosition = renderStatusBar(state);
    moveCursor(state, commandLineCursorPosition);
    refresh();
}
