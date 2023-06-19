/* eslint-disable import/no-cycle */
import { handleKeys } from '../keybinds/normalKeys.js';
import { handleMultiCursorKeys } from '../keybinds/multiCursorKeys.js';
import { handleVimKeys } from '../keybinds/vimKeys.js';
import { handleVisualKeys } from '../keybinds/visualKeys.js';
import { handleVisualLineKeys } from '../keybinds/visualLineKeys.js';
import { handleVisualBlockKeys } from '../keybinds/visualBlockKeys.js';
import { handleSearchKeys } from '../keybinds/searchKeys.js';
import { handleFileFinderKeys } from '../keybinds/fileFinderKeys.js';
import { handleCommandKeys } from '../keybinds/commandKeys.js';

function sendKeys(keys, state, screen, term) {
    if (state.data.length === 0) {
        state.data = [''];
    }
    for (let i = 0; i < keys.length; i += 1) {
        if (keys[i] === 'ESCAPE') {
            state.commandHistory += '<esc>';
            state.totalCommandHistory += '<esc>';
        } else if (keys[i] === 'ENTER') {
            state.commandHistory += '<enter>';
            state.totalCommandHistory += '<enter>';
        } else if (keys[i] === 'BACKSPACE') {
            state.commandHistory += '<bs>';
            state.totalCommandHistory += '<bs>';
        } else if (keys[i].startsWith('CTRL_')) {
            state.commandHistory += '<c-' + keys[i].slice(-1).toLowerCase() + '>';
            state.totalCommandHistory += '<c-' + keys[i].slice(-1).toLowerCase() + '>';
        } else {
            state.commandHistory += keys[i];
            state.totalCommandHistory += keys[i];
        }
        if (state.allowCommandLogging && state.recording && !(state.mode === 'n' && keys[i] === 'q')) {
            state.macro.push(keys[i]);
        }
        if (state.mode === ':') {
            handleCommandKeys(keys[i], state, screen, term);
        } else if (state.mode === '/') {
            handleSearchKeys(keys[i], state, screen);
        } else if (state.mode === 'MULTI_CURSOR') {
            handleMultiCursorKeys(keys[i], state, screen);
        } else if (state.mode === 'CTRL_V') {
            handleVisualBlockKeys(keys[i], state, screen);
        } else if (state.mode === 'v') {
            handleVisualKeys(keys[i], state, screen);
        } else if ((state.mode === 'f' || state.mode === 'g')) {
            handleFileFinderKeys(keys[i], state, screen);
        } else if (state.mode === 'V') {
            handleVisualLineKeys(keys[i], state, screen);
        } else if ((state.mode === 'n' || state.mode === 'r')) {
            handleVimKeys(keys[i], state, screen);
        } else {
            handleKeys(keys[i], state, screen);
        }
    }
}

export {
    sendKeys
};
