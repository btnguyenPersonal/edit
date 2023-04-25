/* eslint-disable import/no-cycle */
import ncp from 'copy-paste';
import {
    copyToClipboard,
    renderScreen,
    createSnapshot,
    logCommand,
    isWritable,
    searchForString,
    centerScreen
} from '../util/helper.js';
import {
    up,
    down,
    firstNonSpace,
    increaseIndentLevel,
    decreaseIndentLevel,
    bottomOfFile,
    upHalfScreen,
    downHalfScreen,
    getIndentLevelFrom,
    isEmptyRow,
    isCommented,
    uncomment,
    comment,
} from '../util/movement.js';

function handleSearchKeys(key, state, screen) {
    if (isWritable(key)) {
        state.searchQuery += key;
        searchForString(state, state.searchQuery);
        centerScreen(state);
    } else if (key === 'ESCAPE') {
        state.mode = 'n';
        state.row = state.search.row;
        state.col = state.search.col;
    } else if (key === 'ENTER') {
        state.mode = 'n';
    }
    logCommand(false, state, key);
    renderScreen(state, screen);
}

export {
    handleSearchKeys
};
