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
    } else if (key === 'UP') {
        if (state.fileFinderIndex > 0) {
            state.fileFinderIndex -= 1;
        } else {
            state.fileFinderIndex = state.fileFindingOutput.length - 1;
        }
    } else if (key === 'DOWN') {
        if (state.fileFinderIndex < state.fileFindingOutput.length - 2) {
            state.fileFinderIndex += 1;
        }
    } else if (key === 'ESCAPE') {
        if (state.mode === 'f') {
            state.fileFinderQuery = '';
        }
        state.mode = 'n';
        state.fileFinderIndex = 0;
    } else if (key === 'ENTER') {
        let newFile = state.fileFindingOutput[state.fileFinderIndex];
        let lineNum = 0;
        if (state.mode === 'g') {
            const arrayForm = newFile.split(':');
            newFile = arrayForm[0];
            lineNum = parseInt(arrayForm[1], 10);
        } else {
            state.fileFinderQuery = '';
            state.fileFinderIndex = 0;
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
