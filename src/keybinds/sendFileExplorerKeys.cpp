#include "sendFileExplorerKeys.h"
#include "../util/fileExplorerNode.h"
#include "../util/clipboard.h"
#include "../util/helper.h"
#include "../util/modes.h"
#include "../util/state.h"
#include <string>
#include <vector>

void sendFileExplorerKeys(State* state, int c) {
    if (c == 27) { // ESC
        state->mode = SHORTCUTS;
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
    }
    centerFileExplorer(state);
}
