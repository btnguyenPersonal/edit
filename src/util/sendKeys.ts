/* eslint-disable import/no-cycle */
import { handleKeys } from '../keybinds/normalKeys.ts';
import { handleMultiCursorKeys } from '../keybinds/multiCursorKeys.ts';
import { handleVimKeys } from '../keybinds/vimKeys.ts';
import { handleVisualKeys } from '../keybinds/visualKeys.ts';
import { handleVisualLineKeys } from '../keybinds/visualLineKeys.ts';
import { handleVisualBlockKeys } from '../keybinds/visualBlockKeys.ts';
import { handleSearchKeys } from '../keybinds/searchKeys.ts';
import { handleFileFinderKeys } from '../keybinds/fileFinderKeys.ts';
import { handleCommandKeys } from '../keybinds/commandKeys.ts';

function sendKeys(keys, state, screen, term) {
    if (state.data.length === 0) {
        state.data = [''];
    }
    for (let i = 0; i < keys.length; i += 1) {
        if (state.allowCommandLogging) {
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
        }
        if (state.allowCommandLogging && state.recording && !(state.mode === 'n' && keys[i] === 'Q')) {
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
