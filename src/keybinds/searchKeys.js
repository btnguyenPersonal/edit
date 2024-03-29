/* eslint-disable import/no-cycle */
import {
    SHORTCUTS
} from '../util/modes.js';
import {
    logCommand,
    isWritable,
    searchForString,
    searchBackForString,
    createSnapshot,
    centerScreen
} from '../util/helper.js';
import { render } from '../util/render.js';

function handleSearchKeys(key, state, screen) {
    if (state.replacing) {
        logCommand(false, state, key);
        if (isWritable(key)) {
            state.replaceQuery += key;
        } else if (key === 'ESCAPE') {
            state.replaceQuery = '';
            state.replacing = false;
            state.mode = SHORTCUTS;
        } else if (key === 'ENTER') {
            state.replacing = false;
            state.mode = SHORTCUTS;
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
    } else {
        if (isWritable(key)) {
            state.searchQuery += key;
            state.searching = true;
            if (state.reverseSearch) {
                searchBackForString(state, state.searchQuery, true);
            } else {
                searchForString(state, state.searchQuery);
            }
            centerScreen(state);
        } else if (key === 'ESCAPE') {
            state.mode = SHORTCUTS;
            state.row = state.search.row;
            state.col = state.search.col;
        } else if (key === 'ENTER') {
            state.mode = SHORTCUTS;
        } else if (key === 'CTRL_F') {
            state.replacing = true;
        } else if (key === 'BACKSPACE') {
            if (state.searchQuery.length > 0) {
                state.searchQuery = state.searchQuery.substring(0, state.searchQuery.length - 1);
            }
        }
    }
    render(state, screen);
}

export {
    handleSearchKeys
};
