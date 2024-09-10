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
    } else if (c == 'k') {
        if (state->fileExplorerIndex > 0) {
            state->fileExplorerIndex--;
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
            state->changeFile(node->path.string());
        }
    // } else if (c == '[') {
    //     state->row = getPrevLineSameIndent(state);
    // } else if (c == ']') {
    //     state->row = getNextLineSameIndent(state);
    // } else if (c == ctrl('u')) {
    //     upHalfScreen(state);
    // } else if (c == ctrl('d')) {
    //     downHalfScreen(state);
    // } else if (c == 'y' || c == ctrl('y')) {
    //     std::string gitHash = getGitHash(state);
    //     copyToClipboard(gitHash);
    //     state->status = gitHash;
    //     state->mode = SHORTCUTS;
    }
}
