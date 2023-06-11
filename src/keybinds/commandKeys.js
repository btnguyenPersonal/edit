/* eslint-disable import/no-cycle */
import {
    renderScreen,
    logCommand,
    isWritable,
    searchForString,
    createSnapshot,
    evaluateCommand,
    centerScreen
} from '../util/helper.js';

function handleCommandKeys(key, state, screen, term) {
    if (isWritable(key)) {
        state.commandString += key;
    } else if (key === 'ESCAPE') {
        state.commandString = '';
        state.mode = 'n';
    } else if (key === 'ENTER') {
        evaluateCommand(state, term);
        state.commandString = '';
        state.mode = 'n';
    } else if (key === 'BACKSPACE') {
        if (state.commandString.length > 0) {
            state.commandString = state.commandString.substring(0, state.commandString.length - 1);
        }
    }
    renderScreen(state, screen);
}

export {
    handleCommandKeys
};
