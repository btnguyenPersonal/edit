import {
    SHORTCUTS,
} from '../util/modes.js';
import {
    renderScreen,
} from '../util/helper.js';

function handleFileExplorerKeys(key, state, screen) {
    if (key === 'ESCAPE') {
        state.mode = SHORTCUTS;
    } else if (key === 'k' || key === 'UP') {
        if (state.fileExplorerIndex - 1 >= 0) {
            state.fileExplorerIndex -= 1;
        }
    } else if (key === 'j' || key === 'DOWN') {
        if (state.fileExplorerIndex + 1 < state.fileExplorerOutput.length - 1) {
            state.fileExplorerIndex += 1;
        }
    }
    renderScreen(state, screen);
}

export {
    handleFileExplorerKeys
};
