/* eslint-disable import/no-cycle */
import {
    isWritable,
    renderScreen,
    createSnapshot,
    applySnapshot,
    logCommand
} from '../util/helper.js';
import {
    left,
    right,
    increaseIndentLevel,
    decreaseIndentLevel,
    isEmptyRow,
    endOfLine,
    firstNonSpace,
    getCoorBeginningLastWord,
} from '../util/movement.js';

function handleMultiCursorKeys(key, state, screen) {
    if (key === 'DELETE') {
        if (state.row < state.visualBlock.row) {
            for (let r = state.row; r < state.visualBlock.row + 1; r += 1) {
                if (state.col < state.data[r].length) {
                    state.data[r] = state.data[r].substring(0, state.col)
                        + state.data[r].substring(state.col + 1);
                } else if (r < state.data.length - 1) {
                    state.data[r] += state.data[r + 1];
                    state.data.splice(r + 1, 1);
                }
            }
        } else {
            for (let r = state.visualBlock.row; r < state.row + 1; r += 1) {
                if (state.col < state.data[r].length) {
                    state.data[r] = state.data[r].substring(0, state.col)
                        + state.data[r].substring(state.col + 1);
                } else if (r < state.data.length - 1) {
                    state.data[r] += state.data[r + 1];
                    state.data.splice(r + 1, 1);
                }
            }
        }
        if (!state.vim) {
            createSnapshot(state);
        }
    } else if (key === 'CTRL_W') {
        let coor;
        if (state.row < state.visualBlock.row) {
            for (let r = state.row; r < state.visualBlock.row + 1; r += 1) {
                coor = getCoorBeginningLastWord(state);
                state.data[r] = state.data[r].substring(0, coor) + state.data[r].substring(state.col);
            }
        } else {
            for (let r = state.visualBlock.row; r < r + 1; r += 1) {
                coor = getCoorBeginningLastWord(state);
                state.data[r] = state.data[r].substring(0, coor) + state.data[r].substring(state.col);
            }
        }
        state.col = coor;
        if (!state.vim) {
            createSnapshot(state);
        }
    } else if (key === 'BACKSPACE') {
        let moveBack = false;
        if (state.row < state.visualBlock.row) {
            for (let r = state.row; r < state.visualBlock.row + 1; r += 1) {
                if (state.col > 0) {
                    state.data[r] = state.data[r].substring(0, state.col - 1)
                        + state.data[r].substring(state.col);
                    moveBack = true;
                } else if (r > 0) {
                    state.col = state.data[r - 1].length;
                    state.data[r - 1] += state.data[r];
                    state.data.splice(r, 1);
                }
            }
        } else {
            for (let r = state.visualBlock.row; r < state.row + 1; r += 1) {
                if (state.col > 0) {
                    state.data[r] = state.data[r].substring(0, state.col - 1)
                        + state.data[r].substring(state.col);
                    moveBack = true;
                } else if (r > 0) {
                    state.col = state.data[r - 1].length;
                    state.data[r - 1] += state.data[r];
                    state.data.splice(r, 1);
                }
            }
        }
        if (moveBack) {
            state.col -= 1;
        }
        if (!state.vim) {
            createSnapshot(state);
        }
    } else if (key === 'TAB') {
        if (state.row < state.visualBlock.row) {
            for (let r = state.row; r < state.visualBlock.row + 1; r += 1) {
                increaseIndentLevel(state, r);
            }
        } else {
            for (let r = state.visualBlock.row; r < state.row + 1; r += 1) {
                increaseIndentLevel(state, r);
            }
        }
        state.col += state.indentAmount;
        if (!state.vim) {
            createSnapshot(state);
        }
    } else if (key === 'SHIFT_TAB') {
        if (state.row < state.visualBlock.row) {
            for (let r = state.row; r < state.visualBlock.row + 1; r += 1) {
                decreaseIndentLevel(state, r);
            }
        } else {
            for (let r = state.visualBlock.row; r < state.row + 1; r += 1) {
                decreaseIndentLevel(state, r);
            }
        }
        state.col = state.col - state.indentAmount >= 0 ? state.col - state.indentAmount : 0;
        if (!state.vim) {
            createSnapshot(state);
        }
    } else if (key === 'CTRL_A') {
        state.col = firstNonSpace(state, state.row);
    } else if (key === 'CTRL_E') {
        state.col = endOfLine(state, state.row);
        right(state);
    } else if (isWritable(key)) {
        if (state.row < state.visualBlock.row) {
            for (let r = state.row; r < state.visualBlock.row + 1; r += 1) {
                state.data[r] = state.data[r].substring(0, state.col)
                    + key
                    + state.data[r].substring(state.col);
            }
        } else {
            for (let r = state.visualBlock.row; r < state.row + 1; r += 1) {
                state.data[r] = state.data[r].substring(0, state.col)
                    + key
                    + state.data[r].substring(state.col);
            }
        }
        state.col += 1;
        if (!state.vim) {
            createSnapshot(state);
        }
    } else if (key === 'ESCAPE') {
        if (state.vim && isEmptyRow(state, state.row)) {
            state.data[state.row] = '';
        }
        left(state);
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
    handleMultiCursorKeys
};
