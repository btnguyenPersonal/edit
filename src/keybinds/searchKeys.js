/* eslint-disable import/no-cycle */
import {
    renderScreen,
    logCommand,
    isWritable,
    searchForString,
    centerScreen
} from '../util/helper.js';

function handleSearchKeys(key, state, screen) {
    if (isWritable(key)) {
        state.searchQuery += key;
        state.searching = true;
        searchForString(state, state.searchQuery);
        centerScreen(state);
    } else if (key === 'ESCAPE') {
        state.mode = 'n';
        state.row = state.search.row;
        state.col = state.search.col;
    } else if (key === 'ENTER') {
        state.mode = 'n';
    } else if (key === 'BACKSPACE') {
        if (state.searchQuery.length > 1) {
            state.searchQuery = state.searchQuery.substring(0, state.searchQuery.length - 1);
        }
    }
    logCommand(false, state, key);
    renderScreen(state, screen);
}

export {
    handleSearchKeys
};
