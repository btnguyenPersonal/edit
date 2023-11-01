import fs from 'fs';
import {
    SHORTCUTS,
} from '../util/modes.js';
import {
    getFileFromExplorer,
    processFile,
    renderScreen,
    setFileExplorerFiles,
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
    } else if (key === 'x') {
        if (!state.fileExplorerOutput[state.fileExplorerIndex].includes('__DIR')) {
            const selectedFile = getFileFromExplorer(state);
            if (selectedFile && fs.existsSync(selectedFile)) {
                fs.unlink(selectedFile);
                setFileExplorerFiles(state);
            }
        }
    } else if (key === 'ENTER') {
        if (!state.fileExplorerOutput[state.fileExplorerIndex].includes('__DIR')) {
            const selectedFile = getFileFromExplorer(state);
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
