import fs from 'fs';
import {
    SHORTCUTS,
} from '../util/modes.js';
import {
    calcFileExplorerOutput,
    getFileFromExplorer,
    isWritable,
    processFile,
    renderScreen,
} from '../util/helper.js';

function handleFileExplorerKeys(key, state, screen) {
    if (state.renamingFile && key === 'ESCAPE') {
        state.renamingFile = false;
        state.newFile = '';
    } else if (state.renamingFile && key === 'BACKSPACE') {
        if (state.newFile.length > 0) {
            state.newFile = state.newFile.substring(0, state.newFile.length - 1);
        }
    } else if (state.renamingFile && isWritable(key)) {
        state.newFile += key;
    } else if (key === 'ESCAPE') {
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
    } else if (key === 'r') {
        if (!state.fileExplorerOutput[state.fileExplorerIndex].includes('__DIR')) {
            const selectedFile = getFileFromExplorer(state);
            if (selectedFile && fs.existsSync(selectedFile)) {
                state.selectedFile = selectedFile;
                state.renamingFile = true;
            }
        }
    } else if (key === 'x') {
        if (!state.fileExplorerOutput[state.fileExplorerIndex].includes('__DIR')) {
            const selectedFile = getFileFromExplorer(state);
            if (selectedFile && fs.existsSync(selectedFile)) {
                fs.rmSync(selectedFile, {
                    force: true,
                });
                calcFileExplorerOutput(state);
            }
        }
    } else if (key === 'ENTER') {
        if (state.renamingFile) {
            let newFilePath = state.selectedFile.split('/');
            newFilePath[newFilePath.length - 1] = state.newFile;
            newFilePath.join('/');
            if (state.selectedFile !== '' && state.newFile !== '' && newFilePath !== state.selectedFile) {
                fs.renameSync(state.selectedFile, newFilePath);
                state.newFile = '';
                state.renamingFile = false;
                calcFileExplorerOutput(state);
            }
        } else {
            if (!state.fileExplorerOutput[state.fileExplorerIndex].includes('__DIR')) {
                const selectedFile = getFileFromExplorer(state);
                if (selectedFile && fs.existsSync(selectedFile)) {
                    processFile(state, selectedFile, -1);
                }
            }
        }
    }
    renderScreen(state, screen);
}

export {
    handleFileExplorerKeys
};
