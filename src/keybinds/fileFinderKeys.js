import fs from 'fs';
import {
    renderScreen,
    isWritable,
    processFile,
    calcFileFinderOutput,
} from '../util/helper.js';

function handleFileFinderKeys(key, state, screen) {
    if (isWritable(key) && key !== '\\') {
        if (key === '"') {
            state.fileFinderQuery += '\\';
        }
        state.fileFinderQuery += key;
        state.fileFinderIndex = 0;
        calcFileFinderOutput(state);
    } else if (key === 'CTRL_U') {
        for (let i = 0; i < process.stdout.rows / 2; i += 1) {
            if (state.fileFinderIndex > 0) {
                state.fileFinderIndex -= 1;
            }
        }
    } else if (key === 'CTRL_D') {
        for (let i = 0; i < process.stdout.rows / 2; i += 1) {
            if (state.fileFinderIndex < state.fileFinderOutput.length - 2) {
                state.fileFinderIndex += 1;
            }
        }
    } else if (key === 'UP' || key === 'CTRL_P') {
        if (state.fileFinderIndex > 0) {
            state.fileFinderIndex -= 1;
        }
    } else if (key === 'DOWN' || key === 'CTRL_N') {
        if (state.fileFinderIndex < state.fileFinderOutput.length - 2) {
            state.fileFinderIndex += 1;
        }
    } else if (key === 'ESCAPE') {
        state.fileFinderQuery = '';
        state.mode = 'n';
        state.fileFinderIndex = 0;
    } else if (key === 'ENTER') {
        let newFile = state.fileFinderOutput[state.fileFinderIndex];
        const lineNum = 0;
        state.fileFinderQuery = '';
        state.fileFinderIndex = 0;
        if (newFile !== undefined) {
            let fileExists = fs.existsSync(newFile);
            if (!fileExists) {
                newFile += '.js';
                fileExists = fs.existsSync(newFile);
            }
            processFile(state, newFile, lineNum, fileExists);
        }
        state.mode = 'n';
    } else if (key === 'CTRL_L') {
        state.fileFinderQuery = '';
        calcFileFinderOutput(state);
    } else if (key === 'BACKSPACE') {
        if (state.fileFinderQuery.length > 0) {
            if (state.fileFinderQuery.endsWith('"')) {
                state.fileFinderQuery = state.fileFinderQuery.substring(0, state.fileFinderQuery.length - 2);
            } else {
                state.fileFinderQuery = state.fileFinderQuery.substring(0, state.fileFinderQuery.length - 1);
            }
        }
        state.fileFinderIndex = 0;
        calcFileFinderOutput(state);
    }
    renderScreen(state, screen);
}

export {
    handleFileFinderKeys
};
