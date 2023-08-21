import fs from 'fs';
import {
    SHORTCUTS,
    GREP,
} from '../util/modes.js';
import {
    renderScreen,
    isWritable,
    processFile,
    calcGrepOutput,
    calcFileFinderOutput,
} from '../util/helper.js';

function handleFileFinderKeys(key, state, screen, term) {
    if (isWritable(key) && key !== '\\' && key !== '"') {
        state.fileFinderQuery = state.fileFinderQuery.slice(0, state.fileFinderCursorPosition)
            + key
            + state.fileFinderQuery.slice(state.fileFinderCursorPosition);
        state.fileFinderCursorPosition += 1;
        state.fileFinderIndex = 0;
        calcFileFinderOutput(state);
    } else if (key === 'CTRL_G') {
        state.mode = GREP;
        calcGrepOutput(state);
    } else if (key === 'CTRL_A') {
        state.fileFinderCursorPosition = 0;
    } else if (key === 'CTRL_E') {
        state.fileFinderCursorPosition = state.fileFinderQuery.length;
    } else if (key === 'CTRL_U') {
        for (let i = 0; i < process.stdout.rows / 2; i += 1) {
            if (state.fileFinderIndex > 0) {
                state.fileFinderIndex -= 1;
            }
        }
    } else if (key === 'CTRL_D') {
        for (let i = 0; i < process.stdout.rows / 2; i += 1) {
            if (state.fileFinderIndex < state.fileFinderOutput.length - 1) {
                state.fileFinderIndex += 1;
            }
        }
    } else if (key === 'LEFT') {
        if (state.fileFinderCursorPosition > 0) {
            state.fileFinderCursorPosition -= 1;
        }
    } else if (key === 'RIGHT') {
        if (state.fileFinderCursorPosition < state.fileFinderQuery.length) {
            state.fileFinderCursorPosition += 1;
        }
    } else if (key === 'UP' || key === 'CTRL_P') {
        if (state.fileFinderIndex > 0) {
            state.fileFinderIndex -= 1;
        }
    } else if (key === 'DOWN' || key === 'CTRL_N') {
        if (state.fileFinderIndex < state.fileFinderOutput.length - 1) {
            state.fileFinderIndex += 1;
        }
    } else if (key === 'ESCAPE') {
        if (state.file === '') {
            term.fullscreen(false);
            process.exit();
        }
        state.fileFinderQuery = '';
        state.mode = SHORTCUTS;
        state.fileFinderIndex = 0;
    } else if (key === 'ENTER') {
        let newFile = state.fileFinderOutput[state.fileFinderIndex];
        const lineNum = 0;
        if (newFile !== undefined) {
            let fileExists = fs.existsSync(newFile);
            if (!fileExists) {
                newFile += '.js';
                fileExists = fs.existsSync(newFile);
            }
            processFile(state, newFile, lineNum, fileExists);
        }
        state.mode = SHORTCUTS;
    } else if (key === 'CTRL_L') {
        state.fileFinderQuery = '';
        state.fileFinderCursorPosition = 0;
        calcFileFinderOutput(state);
    } else if (key === 'BACKSPACE') {
        if (state.fileFinderCursorPosition > 0) {
            state.fileFinderQuery = state.fileFinderQuery.slice(0, state.fileFinderCursorPosition - 1)
                + state.fileFinderQuery.slice(state.fileFinderCursorPosition);
            state.fileFinderCursorPosition -= 1;
        }
        state.fileFinderIndex = 0;
        calcFileFinderOutput(state);
    }
    renderScreen(state, screen);
}

export {
    handleFileFinderKeys
};
