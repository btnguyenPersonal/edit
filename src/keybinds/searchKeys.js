/* eslint-disable import/no-cycle */
import {
    renderScreen,
    logCommand,
    isWritable,
    searchForString,
    createSnapshot,
    centerScreen
} from '../util/helper.js';

function handleSearchKeys(key, state, screen) {
    if (state.replacing) {
        if (isWritable(key)) {
            state.replaceQuery += key;
        } else if (key === 'ESCAPE') {
            state.replaceQuery = '';
            state.replacing = false;
            state.mode = 'n';
        } else if (key === 'ENTER') {
            state.replacing = false;
            state.mode = 'n';
            for (let i = 0; i < state.data.length; i += 1) {
                state.data[i] = state.data[i].replaceAll(state.searchQuery, state.replaceQuery);
            }
            createSnapshot(state);
            state.replaceQuery = '';
        } else if (key === 'BACKSPACE') {
            if (state.replaceQuery.length > 0) {
                state.replaceQuery = state.replaceQuery.substring(0, state.replaceQuery.length - 1);
            }
        }
        logCommand(false, state, key);
    } else {
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
        } else if (key === 'CTRL_F') {
            state.replacing = true;
        } else if (key === 'BACKSPACE') {
            if (state.searchQuery.length > 0) {
                state.searchQuery = state.searchQuery.substring(0, state.searchQuery.length - 1);
            }
        }
    }
    renderScreen(state, screen);
}

export {
    handleSearchKeys
};
