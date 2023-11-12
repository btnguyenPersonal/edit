import fs from 'fs';
import {
    SHORTCUTS,
} from '../util/modes.js';
import {
    createFolderIfNotExists,
    calcFileExplorerOutput,
    getFileFromExplorer,
    getFolderFromExplorer,
    isWritable,
    processFile,
    renderScreen,
} from '../util/helper.js';

function handleFileExplorerKeys(key, state, screen) {
    if (state.typing && key === 'ESCAPE') {
        state.selectedFileExplorerIndex = -1;
        state.typing = false;
        state.renamingFile = false;
        state.copyingFile = false;
        state.creatingFile = false;
        state.newFile = '';
        state.newFileIndex = 0;
    } else if (state.typing && key === 'CTRL_W') {
        state.newFile = state.newFile.substring(state.newFileIndex);
        state.newFileIndex = 0;
    } else if (state.typing && key === 'CTRL_L') {
        state.newFile = '';
        state.newFileIndex = 0;
    } else if (state.typing && key === 'BACKSPACE') {
        if (state.newFileIndex > 0) {
            state.newFile = state.newFile.slice(0, state.newFileIndex - 1)
                + state.newFile.slice(state.newFileIndex);
            state.newFileIndex -= 1;
        }
    } else if (state.typing && key === 'LEFT') {
        if (state.newFileIndex > 0) {
            state.newFileIndex -= 1;
        }
    } else if (state.typing && key === 'RIGHT') {
        if (state.newFileIndex < state.newFile.length) {
            state.newFileIndex += 1;
        }
    } else if (state.typing && isWritable(key)) {
        state.newFile = state.newFile.slice(0, state.newFileIndex)
            + key
            + state.newFile.slice(state.newFileIndex);
        state.newFileIndex += 1;
    } else if (key === 'ESCAPE') {
        state.selectedFileExplorerIndex = -1;
        state.mode = SHORTCUTS;
    } else if (key === ']') {
        for (let i = state.fileExplorerIndex + 1; i < state.fileExplorerOutput.length; i += 1) {
            if (state.fileExplorerOutput[i].startsWith('__DIR')) {
                state.fileExplorerIndex = i;
                break;
            }
        }
    } else if (key === '[') {
        for (let i = state.fileExplorerIndex - 1; i >= 0; i -= 1) {
            if (state.fileExplorerOutput[i].startsWith('__DIR')) {
                state.fileExplorerIndex = i;
                break;
            }
        }
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
    } else if (key === 'n') {
        if (state.fileExplorerOutput[state.fileExplorerIndex].includes('__DIR')) {
            const selectedFolder = getFolderFromExplorer(state);
            if (selectedFolder && fs.existsSync(selectedFolder)) {
                state.selectedFolder = selectedFolder;
                state.typing = true;
                state.creatingFile = true;
            }
        }
    } else if (key === 'p' || key === 'P') {
        if (state.fileExplorerOutput[state.fileExplorerIndex].includes('__DIR') && state.selectedFile) {
            const selectedFolder = getFolderFromExplorer(state);
            if (selectedFolder && fs.existsSync(selectedFolder)) {
                state.selectedFolder = selectedFolder;
                state.typing = true;
                state.copyingFile = true;
            }
        }
    } else if (key === 'y') {
        if (!state.fileExplorerOutput[state.fileExplorerIndex].includes('__DIR')) {
            state.copiedFileName = state.fileExplorerOutput[state.fileExplorerIndex];
            const selectedFile = getFileFromExplorer(state);
            if (selectedFile && fs.existsSync(selectedFile)) {
                state.selectedFile = selectedFile;
                state.selectedFileExplorerIndex = state.fileExplorerIndex;
            }
        }
    } else if (key === 'r') {
        if (!state.fileExplorerOutput[state.fileExplorerIndex].includes('__DIR')) {
            const selectedFile = getFileFromExplorer(state);
            if (selectedFile && fs.existsSync(selectedFile)) {
                state.selectedFile = selectedFile;
                state.selectedFileExplorerIndex = state.fileExplorerIndex;
                state.newFile = selectedFile.split('/').pop();
                state.newFileIndex = state.newFile.length;
                state.typing = true;
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
                calcFileExplorerOutput(state, true);
            }
        }
    } else if (key === 'ENTER') {
        if (state.creatingFile) {
            createFolderIfNotExists(state);
            const newFilePath = state.selectedFolder + '/' + state.newFile;
            if (state.newFile !== '' && !fs.existsSync(newFilePath)) {
                fs.openSync(newFilePath, 'w');
                calcFileExplorerOutput(state, true);
            }
        } else if (state.copyingFile) {
            createFolderIfNotExists(state);
            const newFilePath = state.selectedFolder + '/' + state.newFile;
            if (state.selectedFile !== '' && state.newFile !== '' && newFilePath !== state.selectedFile && !fs.existsSync(newFilePath)) {
                fs.copyFileSync(state.selectedFile, newFilePath);
                calcFileExplorerOutput(state, true);
            }
        } else if (state.renamingFile) {
            let newFilePath = state.selectedFile.split('/');
            newFilePath[newFilePath.length - 1] = state.newFile;
            newFilePath = newFilePath.join('/');
            if (state.selectedFile !== '' && state.newFile !== '' && newFilePath !== state.selectedFile) {
                fs.renameSync(state.selectedFile, newFilePath);
                calcFileExplorerOutput(state, true);
            }
        } else {
            if (!state.fileExplorerOutput[state.fileExplorerIndex].includes('__DIR')) {
                const selectedFile = getFileFromExplorer(state);
                if (selectedFile && fs.existsSync(selectedFile)) {
                    processFile(state, selectedFile, -1);
                }
            }
        }
        if (state.copiedFileName !== state.fileExplorerOutput[state.selectedFileExplorerIndex]) {
            state.selectedFileExplorerIndex += 1;
        }
        state.typing = false;
        state.renamingFile = false;
        state.creatingFile = false;
        state.copyingFile = false;
        state.newFile = '';
        state.newFileIndex = 0;
    }
    renderScreen(state, screen);
}

export {
    handleFileExplorerKeys
};
