import fs from 'fs';
import {
    SHORTCUTS,
    FILEFINDER,
} from '../util/modes.js';
import {
    firstNonSpace,
} from '../util/movement.js';
import {
    renderScreen,
    isWritable,
    processFile,
    calcGrepOutput,
    setFileSearchOutput,
    centerScreen,
    calcFileFinderOutput,
} from '../util/helper.js';

function handleGrepKeys(key, state, screen) {
    if (isWritable(key)) {
        state.grepQuery = state.grepQuery.slice(0, state.grepCursorPosition) + key + state.grepQuery.slice(state.grepCursorPosition);
        state.grepCursorPosition += 1;
        state.grepIndex = 0;
        calcGrepOutput(state);
    } else if (key === 'CTRL_G') {
        state.mode = FILEFINDER;
        setFileSearchOutput(state);
        state.gitFinding = true;
        state.fileFinderQuery = '';
        state.fileFinderCursorPosition = 0;
        state.fileFinderIndex = 0;
        calcFileFinderOutput(state);
    } else if (key === 'CTRL_A') {
        state.grepCursorPosition = 0;
    } else if (key === 'CTRL_E') {
        state.grepCursorPosition = state.grepQuery.length;
    } else if (key === 'CTRL_U') {
        for (let i = 0; i < process.stdout.rows / 2; i += 1) {
            if (state.grepIndex > 0) {
                state.grepIndex -= 1;
            }
        }
    } else if (key === 'CTRL_D') {
        for (let i = 0; i < process.stdout.rows / 2; i += 1) {
            if (state.grepIndex < state.fileFinderOutput.length - 1) {
                state.grepIndex += 1;
            }
        }
    } else if (key === 'LEFT') {
        if (state.grepCursorPosition > 0) {
            state.grepCursorPosition -= 1;
        }
    } else if (key === 'RIGHT') {
        if (state.grepCursorPosition < state.grepQuery.length) {
            state.grepCursorPosition += 1;
        }
    } else if (key === 'UP' || key === 'CTRL_P') {
        if (state.grepIndex > 0) {
            state.grepIndex -= 1;
        }
    } else if (key === 'DOWN' || key === 'CTRL_N') {
        if (state.grepIndex + 1 < state.fileFinderOutput.length - 1) {
            state.grepIndex += 1;
        }
    } else if (key === 'ESCAPE') {
        state.mode = SHORTCUTS;
        state.grepIndex = 0;
    } else if (key === 'ENTER') {
        let newFile = state.fileFinderOutput[state.grepIndex];
        if (newFile !== undefined) {
            const arrayForm = newFile.split(':');
            newFile = arrayForm[0];
            const lineNum = parseInt(arrayForm[1], 10);
            let fileExists = fs.existsSync(newFile);
            if (!fileExists) {
                newFile += '.js';
                fileExists = fs.existsSync(newFile);
            }
            if (fileExists) {
                processFile(state, newFile, lineNum);
            }
            state.mode = SHORTCUTS;
            centerScreen(state);
            state.col = firstNonSpace(state, state.row);
            state.mark2 = state.row;
        }
    } else if (key === 'CTRL_L') {
        state.grepQuery = '';
        state.grepCursorPosition = 0;
        calcGrepOutput(state);
    } else if (key === 'BACKSPACE') {
        if (state.grepCursorPosition > 0) {
            state.grepQuery = state.grepQuery.slice(0, state.grepCursorPosition - 1) + state.grepQuery.slice(state.grepCursorPosition);
            state.grepCursorPosition -= 1;
        }
        state.grepIndex = 0;
        calcGrepOutput(state);
    }
    renderScreen(state, screen);
}

export {
    handleGrepKeys
};
