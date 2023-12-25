#include <ncurses.h>
#include <string>
#include <vector>

void renderScreen(std::vector<std::string> file_contents) {
    // TODO partial update
    clear();
    for (int i = 0; i < file_contents.size(); i++) {
        printw(file_contents[i].c_str());
        printw("\n");
    }
    refresh();
}
