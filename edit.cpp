#include <string>
#include <iostream>
#include <vector>
#include <ncurses.h>
#include "util/helper.cpp"
#include "util/render.cpp"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "usage: edit [file]" << std::endl;
        exit(1);
    }
    std::vector<std::string> file_contents = readFile(argv[1]);
    for (std::string line : file_contents) {
        std::cout << line << std::endl;
    }
    initscr();
    renderScreen(file_contents);
    refresh();
    while (true) {
        char c = getchar();
        renderScreen(file_contents);
        refresh();
    }
    endwin();
    return 0;
}