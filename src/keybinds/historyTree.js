/* eslint-disable import/no-cycle */
import {
    SHORTCUTS,
} from '../util/modes.js';
import {
    renderScreen,
    applySnapshot,
    saveFile,
} from '../util/helper.js';

function handleHistoryTreeKeys(key, state, screen) {
    if (key === 'UP' || key === 'k') {
        if (state.fileFinderIndex > 0) {
            state.fileFinderIndex -= 1;
        }
    } else if (key === 'DOWN' || key === 'j') {
        if (state.fileFinderIndex < state.fileFinderOutput.length - 1) {
            state.fileFinderIndex += 1;
        }
    } else if (key === 'ESCAPE') {
        state.mode = SHORTCUTS;
        state.fileFinderOutput = [];
        state.fileFinderIndex = 0;
    } else if (key === 'ENTER') {
        state.mode = SHORTCUTS;
        applySnapshot(state, state.fileFinderIndex, false);
        state.currentSnapshot = state.fileFinderIndex;
        saveFile(state);
        state.fileFinderOutput = [];
        state.fileFinderIndex = 0;
    }
    renderScreen(state, screen);
}

export {
    handleHistoryTreeKeys
};
