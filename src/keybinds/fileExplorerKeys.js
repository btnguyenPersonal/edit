import fs from 'fs';
import {
    SHORTCUTS,
} from '../util/modes.js';
import {
    renderScreen,
    processFile,
} from '../util/helper.js';

function handleFileExplorerKeys(key, state, screen) {
    if (key === 'ESCAPE') {
        state.mode = SHORTCUTS;
    } else if (key === 'k' || key === 'UP' || key === 'CTRL_P') {
        if (state.fileExplorerIndex - 1 >= 0) {
            state.fileExplorerIndex -= 1;
        }
    } else if (key === 'j' || key === 'DOWN' || key === 'CTRL_N') {
        if (state.fileExplorerIndex + 1 < state.fileExplorerOutput.length - 1) {
            state.fileExplorerIndex += 1;
        }
    } else if (key === 'CTRL_U') {
        for (let i = 0; i < process.stdout.rows / 2; i += 1) {
            if (state.fileExplorerIndex - 1 >= 0) {
                state.fileExplorerIndex -= 1;
            }
        }
    } else if (key === 'CTRL_D') {
        for (let i = 0; i < process.stdout.rows / 2; i += 1) {
            if (state.fileExplorerIndex + 1 < state.fileExplorerOutput.length - 1) {
                state.fileExplorerIndex += 1;
            }
        }
    } else if (key === 'ENTER') {
        if (!state.fileExplorerOutput[state.fileExplorerIndex].includes('__DIR')) {
            let numDir = 0;
            for (let i = state.fileExplorerIndex; i >= 0; i -= 1) {
                if (state.fileExplorerOutput[i].includes('__DIR')) {
                    numDir += 1;
                }
            }
            const selectedFile = state.fileExplorerCopyOutput[state.fileExplorerIndex - numDir];
            if (selectedFile && fs.existsSync(selectedFile)) {
                processFile(state, selectedFile, -1);
            }
        }
    }
    renderScreen(state, screen);
}

export {
    handleFileExplorerKeys
};
