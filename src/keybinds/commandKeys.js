/* eslint-disable import/no-cycle */
import {
    renderScreen,
    isWritable,
    evaluateCommand,
    createSnapshot,
    saveFile,
} from '../util/helper.js';

function handleCommandKeys(key, state, screen, term) {
    if (isWritable(key)) {
        state.commandString = state.commandString.slice(0, state.commandIndex)
            + key
            + state.commandString.slice(state.commandIndex);
        state.commandIndex += 1;
    } else if (key === 'DOWN') {
        if (state.commandsIndex < state.commands.length - 1) {
            state.commandsIndex += 1;
            state.commandString = state.commands[state.commandsIndex];
            state.commandIndex = 0;
        }
    } else if (key === 'UP') {
        if (state.commandsIndex > 0) {
            state.commandsIndex -= 1;
            state.commandString = state.commands[state.commandsIndex];
            state.commandIndex = 0;
        }
    } else if (key === 'RIGHT') {
        if (state.commandIndex < state.commandString.length) {
            state.commandIndex += 1;
        }
    } else if (key === 'LEFT') {
        if (state.commandIndex > 0) {
            state.commandIndex -= 1;
        }
    } else if (key === 'ESCAPE') {
        state.commandString = '';
        state.mode = 'n';
        state.commandIndex = 0;
    } else if (key === 'ENTER') {
        const log = evaluateCommand(state, term);
        if (log) {
            state.commands.push(state.commandString);
            state.commandsIndex = state.commands.length;
        }
        state.commandString = '';
        state.mode = 'n';
        state.commandIndex = 0;
        createSnapshot(state);
    } else if (key === 'BACKSPACE') {
        if (state.commandIndex > 0) {
            state.commandString = state.commandString.substring(0, state.commandString.length - 1);
            state.commandIndex -= 1;
        }
    }
    renderScreen(state, screen);
}

export {
    handleCommandKeys
};
