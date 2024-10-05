#include "sendFileExplorerKeys.h"
#include "../util/fileExplorerNode.h"
#include "../util/clipboard.h"
#include "../util/helper.h"
#include "../util/modes.h"
#include "../util/state.h"
#include <ncurses.h>
#include <string>
#include <vector>

void sendFileExplorerKeys(State* state, int c) {
    if (c == 27) { // ESC
        state->mode = SHORTCUTS;
    } else if (c == ctrl('g')) {
        state->mode = GREP;
    } else if (c == ctrl('p')) {
        state->mode = FINDFILE;
    } else if (c == ':') {
        state->mode = COMMANDLINE;
    // } else if (c == KEY_MOUSE) {
    //     MEVENT event;
    //     if (getmouse(&event) == OK) {
    //         if (event.bstate & BUTTON1_PRESSED) {
    //             handleMouseClick(state, event.y, event.x);
    //         } else if (event.bstate & BUTTON4_PRESSED) {
    //             if (state->fileExplorerIndex > 0) {
    //                 state->fileExplorerIndex--;
    //             }
    //         } else if (event.bstate & BUTTON5_PRESSED) {
    //             if (state->fileExplorerIndex + 1 < state->fileExplorer->getTotalChildren()) {
    //                 state->fileExplorerIndex++;
    //             }
    //         }
    //     }
    } else if (c == '-') {
        state->fileExplorerWindowLine = 0;
        state->fileExplorerIndex = 0;
        auto node = FileExplorerNode::getFileExplorerNode(state->fileExplorer, state->fileExplorerIndex);
        node->close();
        node->open();
    } else if (c == ctrl('z')) {
        state->mode = SHORTCUTS;
        state->fileExplorerOpen = false;
    } else if (c == ctrl('u')) {
        for (unsigned int i = 0; i < state->maxY / 2; i++) {
            if (state->fileExplorerIndex > 0) {
                state->fileExplorerIndex--;
            }
        }
    } else if (c == 'k') {
        if (state->fileExplorerIndex > 0) {
            state->fileExplorerIndex--;
        }
    } else if (c == ctrl('d')) {
        for (unsigned int i = 0; i < state->maxY / 2; i++) {
            if (state->fileExplorerIndex + 1 < state->fileExplorer->getTotalChildren()) {
                state->fileExplorerIndex++;
            }
        }
    } else if (c == 'j') {
        if (state->fileExplorerIndex + 1 < state->fileExplorer->getTotalChildren()) {
            state->fileExplorerIndex++;
        }
    } else if (c == '\n') {
        auto node = FileExplorerNode::getFileExplorerNode(state->fileExplorer, state->fileExplorerIndex);
        if (node->isFolder) {
            if (node->isOpen) {
                node->close();
            } else {
                node->open();
            }
        } else {
            auto baseDir = std::filesystem::current_path();
            auto relativePath = std::filesystem::relative(node->path.string(), baseDir);
            state->changeFile(relativePath);
        }
    } else {
        return;
    }
    centerFileExplorer(state);
}
