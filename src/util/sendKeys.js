/* eslint-disable import/no-cycle */
import { handleKeys } from '../keybinds/normalKeys.js';
import { handleVimKeys } from '../keybinds/vimKeys.js';
import { handleVisualKeys } from '../keybinds/visualKeys.js';
import { handleVisualLineKeys } from '../keybinds/visualLineKeys.js';
import { handleSearchKeys } from '../keybinds/searchKeys.js';

function sendKeys(keys, state, screen) {
    for (let i = 0; i < keys.length; i += 1) {
        if (state.vim && state.mode === '/') {
            handleSearchKeys(keys[i], state, screen);
        } else if (state.vim && state.mode === 'v') {
            handleVisualKeys(keys[i], state, screen);
        } else if (state.vim && state.mode === 'V') {
            handleVisualLineKeys(keys[i], state, screen);
        } else if (state.vim && (state.mode === 'n' || state.mode === 'r')) {
            handleVimKeys(keys[i], state, screen);
        } else {
            handleKeys(keys[i], state, screen);
        }
    }
}

export {
    sendKeys
};
