import fs from 'fs';
import {
    renderScreen,
    isWritable,
    processFile,
    calcFileFinderOutput,
} from '../util/helper.js';

function handleGrepKeys(key, state, screen) {
    if (isWritable(key) && key !== '\\') {
        if (key === '"') {
            state.grepQuery += '\\';
        }
        state.grepQuery += key;
        state.grepIndex = 0;
        calcFileFinderOutput(state);
    } else if (key === 'CTRL_U') {
        for (let i = 0; i < process.stdout.rows / 2; i += 1) {
            if (state.grepIndex > 0) {
                state.grepIndex -= 1;
            }
        }
    } else if (key === 'CTRL_D') {
        for (let i = 0; i < process.stdout.rows / 2; i += 1) {
            if (state.grepIndex < state.fileFinderOutput.length - 2) {
                state.grepIndex += 1;
            }
        }
    } else if (key === 'UP' || key === 'CTRL_P') {
        if (state.grepIndex > 0) {
            state.grepIndex -= 1;
        }
    } else if (key === 'DOWN' || key === 'CTRL_N') {
        if (state.grepIndex < state.fileFinderOutput.length - 2) {
            state.grepIndex += 1;
        }
    } else if (key === 'ESCAPE') {
        if (state.mode === 'f') {
            state.grepQuery = '';
        }
        state.mode = 'n';
        state.grepIndex = 0;
    } else if (key === 'ENTER') {
        let newFile = state.fileFinderOutput[state.grepIndex];
        let lineNum = 0;
        if (state.mode === 'g') {
            const arrayForm = newFile.split(':');
            newFile = arrayForm[0];
            lineNum = parseInt(arrayForm[1], 10);
        } else {
            state.grepQuery = '';
            state.grepIndex = 0;
        }
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
        state.grepQuery = '';
        calcFileFinderOutput(state);
    } else if (key === 'BACKSPACE') {
        if (state.grepQuery.length > 0) {
            if (state.grepQuery.endsWith('"')) {
                state.grepQuery = state.grepQuery.substring(0, state.grepQuery.length - 2);
            } else {
                state.grepQuery = state.grepQuery.substring(0, state.grepQuery.length - 1);
            }
        }
        state.grepIndex = 0;
        calcFileFinderOutput(state);
    }
    renderScreen(state, screen);
}

export {
    handleGrepKeys
};
