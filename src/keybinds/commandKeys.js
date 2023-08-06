/* eslint-disable import/no-cycle */
import {
    SHORTCUTS,
} from '../util/modes.js';
import {
    renderScreen,
    isWritable,
    evaluateCommand,
    saveFile,
} from '../util/helper.js';

function handleCommandKeys(key, state, screen, term) {
    if (isWritable(key)) {
        state.currentCommand = state.currentCommand.slice(0, state.commandCursorPosition)
            + key
            + state.currentCommand.slice(state.commandCursorPosition);
        state.commandCursorPosition += 1;
    } else if (key === 'CTRL_L') {
        state.currentCommand = '';
        state.commandCursorPosition = 0;
    } else if (key === 'CTRL_A') {
        state.commandCursorPosition = 0;
    } else if (key === 'CTRL_E') {
        state.commandCursorPosition = state.currentCommand.length;
    } else if (key === 'DOWN' || key === 'CTRL_N') {
        if (state.historyPosition < state.previousCommands.length - 1) {
            state.historyPosition += 1;
            state.currentCommand = state.previousCommands[state.historyPosition];
            state.commandCursorPosition = 0;
        }
    } else if (key === 'UP' || key === 'CTRL_P') {
        if (state.historyPosition > 0) {
            state.historyPosition -= 1;
            state.currentCommand = state.previousCommands[state.historyPosition];
            state.commandCursorPosition = 0;
        }
    } else if (key === 'RIGHT') {
        if (state.commandCursorPosition < state.currentCommand.length) {
            state.commandCursorPosition += 1;
        }
    } else if (key === 'LEFT') {
        if (state.commandCursorPosition > 0) {
            state.commandCursorPosition -= 1;
        }
    } else if (key === 'ESCAPE') {
        state.currentCommand = '';
        state.mode = SHORTCUTS;
        state.commandCursorPosition = 0;
    } else if (key === 'ENTER') {
        const log = evaluateCommand(state, term);
        if (log) {
            state.previousCommands.push(state.currentCommand);
            state.historyPosition = state.previousCommands.length;
        }
        state.currentCommand = '';
        state.mode = SHORTCUTS;
        state.commandCursorPosition = 0;
        saveFile(state);
    } else if (key === 'BACKSPACE') {
        if (state.commandCursorPosition > 0) {
            state.currentCommand = state.currentCommand.substring(0, state.currentCommand.length - 1);
            state.commandCursorPosition -= 1;
        }
    }
    renderScreen(state, screen);
}

export {
    handleCommandKeys
};
