/* eslint-disable import/no-cycle */
import { handleKeys } from '../keybinds/typingKeys.js';
import { handleMultiCursorKeys } from '../keybinds/multiCursorKeys.js';
import { handleShortcutKeys } from '../keybinds/shortcutKeys.js';
import { handleVisualKeys } from '../keybinds/visualKeys.js';
import { handleVisualLineKeys } from '../keybinds/visualLineKeys.js';
import { handleVisualBlockKeys } from '../keybinds/visualBlockKeys.js';
import { handleSearchKeys } from '../keybinds/searchKeys.js';
import { handleFileFinderKeys } from '../keybinds/fileFinderKeys.js';
import { handleGrepKeys } from '../keybinds/grepKeys.js';
import { handleCommandKeys } from '../keybinds/commandKeys.js';
import { handleHistoryTreeKeys } from '../keybinds/historyTree.js';
import {
    SHORTCUTS,
    COMMAND,
    GREP,
    FILEFINDER,
    VISUAL,
    VISUALLINE,
    VISUALBLOCK,
    HISTORY,
    SEARCH,
    MULTICURSOR
} from './modes.js';

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
        if (state.allowCommandLogging && state.recording && !(state.mode === SHORTCUTS && keys[i] === 'q')) {
            state.macro.push(keys[i]);
        }
        if (state.mode === COMMAND) {
            handleCommandKeys(keys[i], state, screen, term);
        } else if (state.mode === SEARCH) {
            handleSearchKeys(keys[i], state, screen);
        } else if (state.mode === MULTICURSOR) {
            handleMultiCursorKeys(keys[i], state, screen);
        } else if (state.mode === VISUALBLOCK) {
            handleVisualBlockKeys(keys[i], state, screen);
        } else if (state.mode === VISUAL) {
            handleVisualKeys(keys[i], state, screen);
        } else if (state.mode === GREP) {
            handleGrepKeys(keys[i], state, screen);
        } else if (state.mode === FILEFINDER) {
            handleFileFinderKeys(keys[i], state, screen, term);
        } else if (state.mode === HISTORY) {
            handleHistoryTreeKeys(keys[i], state, screen);
        } else if (state.mode === VISUALLINE) {
            handleVisualLineKeys(keys[i], state, screen);
        } else if (state.mode === SHORTCUTS) {
            handleShortcutKeys(keys[i], state, screen);
        } else {
            handleKeys(keys[i], state, screen);
        }
    }
}

export {
    sendKeys
};
