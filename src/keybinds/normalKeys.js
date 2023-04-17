/* eslint-disable import/no-cycle */
import {
    isWritable,
    renderScreen,
    createSnapshot,
    applySnapshot,
    logCommand
} from '../util/helper.js';
import {
    up,
    down,
    left,
    right,
    increaseIndentLevel,
    decreaseIndentLevel,
} from '../util/movement.js';

function handleKeys(key, state, screen) {
    if (key === 'UP') {
        up(state);
    } else if (key === 'DOWN') {
        down(state);
    } else if (key === 'LEFT') {
        left(state);
    } else if (key === 'RIGHT') {
        right(state);
    } else if (key === 'DELETE') {
        if (state.col < state.data[state.row].length) {
            state.data[state.row] = state.data[state.row].substring(0, state.col)
                + state.data[state.row].substring(state.col + 1);
        } else if (state.row < state.data.length - 1) {
            state.data[state.row] += state.data[state.row + 1];
            state.data.splice(state.row + 1, 1);
        }
        state.isSaved = false;
        if (!state.vim) {
            createSnapshot(state);
        }
    } else if (key === 'BACKSPACE') {
        if (state.col > 0) {
            state.data[state.row] = state.data[state.row].substring(0, state.col - 1)
                + state.data[state.row].substring(state.col);
            state.col -= 1;
        } else if (state.row > 0) {
            state.col = state.data[state.row - 1].length;
            state.data[state.row - 1] += state.data[state.row];
            state.data.splice(state.row, 1);
            state.row -= 1;
        }
        state.isSaved = false;
        if (!state.vim) {
            createSnapshot(state);
        }
    } else if (key === 'ENTER') {
        let indentLevel = state.data[state.row].search(/\S|$/);
        if (state.data[state.row].endsWith('{')) {
            state.data.splice(state.row + 1, 0, ' '.repeat(indentLevel) + '}');
            indentLevel += 4;
        }
        if (state.data[state.row].endsWith('(')) {
            state.data.splice(state.row + 1, 0, ' '.repeat(indentLevel) + ')');
            indentLevel += 4;
        }
        if (state.data[state.row].substring(state.col)) {
            state.data.splice(state.row + 1, 0, ' '.repeat(indentLevel) + state.data[state.row].substring(state.col));
            state.data[state.row] = state.data[state.row].substring(0, state.col);
        } else {
            state.data.splice(state.row + 1, 0, ' '.repeat(indentLevel));
        }
        state.row += 1;
        state.col = indentLevel;
        if (state.row >= state.windowLine + process.stdout.rows) {
            state.windowLine += 1;
        }
        state.isSaved = false;
        if (!state.vim) {
            createSnapshot(state);
        }
    } else if (key === 'TAB') {
        increaseIndentLevel(state, state.row);
        state.col += 4;
        state.isSaved = false;
        if (!state.vim) {
            createSnapshot(state);
        }
    } else if (key === 'SHIFT_TAB') {
        decreaseIndentLevel(state, state.row);
        state.isSaved = false;
        if (!state.vim) {
            createSnapshot(state);
        }
    } else if (isWritable(key)) {
        state.data[state.row] = state.data[state.row].substring(0, state.col)
            + key
            + state.data[state.row].substring(state.col);
        state.col += 1;
        state.isSaved = false;
        if (!state.vim) {
            createSnapshot(state);
        }
    } else if (key === 'ESCAPE') {
        state.mode = 'n';
        if (state.vim) {
            createSnapshot(state);
        }
    } else if (!state.vim && key === 'CTRL_Y') {
        if (state.currentSnapshot + 1 < state.snapshots.length) {
            applySnapshot(state, state.currentSnapshot + 1);
        }
    } else if (!state.vim && key === 'CTRL_Z') {
        if (state.currentSnapshot - 1 >= 0) {
            applySnapshot(state, state.currentSnapshot - 1);
        }
    }
    if (state.vim) {
        logCommand(false, state, key);
    }
    renderScreen(state, screen);
}

export {
    handleKeys
};
