/* eslint-disable import/no-cycle */
import {
    SHORTCUTS,
} from '../util/modes.js';
import {
    createSnapshot,
    isWritable,
    logCommand,
} from '../util/helper.js';
import { render } from '../util/render.js';
import {
    decreaseIndentLevel,
    endOfLine,
    firstNonSpace,
    getCoorBeginningLastWord,
    increaseIndentLevel,
    isEmptyRow,
    left,
    right,
} from '../util/movement.js';

function handleMultiCursorKeys(key, state, screen) {
    if (key === 'DELETE') {
        if (state.row < state.visual.row) {
            for (let r = state.row; r < state.visual.row + 1; r += 1) {
                if (state.col < state.data[r].length) {
                    state.data[r] = state.data[r].substring(0, state.col)
                        + state.data[r].substring(state.col + 1);
                } else if (r < state.data.length - 1) {
                    state.data[r] += state.data[r + 1];
                    state.data.splice(r + 1, 1);
                }
            }
        } else {
            for (let r = state.visual.row; r < state.row + 1; r += 1) {
                if (state.col < state.data[r].length) {
                    state.data[r] = state.data[r].substring(0, state.col)
                        + state.data[r].substring(state.col + 1);
                } else if (r < state.data.length - 1) {
                    state.data[r] += state.data[r + 1];
                    state.data.splice(r + 1, 1);
                }
            }
        }
    } else if (key === 'CTRL_W') {
        let coor;
        if (state.row < state.visual.row) {
            for (let r = state.row; r < state.visual.row + 1; r += 1) {
                coor = getCoorBeginningLastWord(state);
                state.data[r] = state.data[r].substring(0, coor) + state.data[r].substring(state.col);
            }
        } else {
            for (let r = state.visual.row; r < r + 1; r += 1) {
                coor = getCoorBeginningLastWord(state);
                state.data[r] = state.data[r].substring(0, coor) + state.data[r].substring(state.col);
            }
        }
        state.col = coor;
    } else if (key === 'BACKSPACE') {
        let moveBack = false;
        if (state.row < state.visual.row) {
            for (let r = state.row; r < state.visual.row + 1; r += 1) {
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
            for (let r = state.visual.row; r < state.row + 1; r += 1) {
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
    } else if (key === 'TAB') {
        if (state.row < state.visual.row) {
            for (let r = state.row; r < state.visual.row + 1; r += 1) {
                increaseIndentLevel(state, r);
            }
        } else {
            for (let r = state.visual.row; r < state.row + 1; r += 1) {
                increaseIndentLevel(state, r);
            }
        }
        state.col += state.indentAmount;
    } else if (key === 'SHIFT_TAB') {
        if (state.row < state.visual.row) {
            for (let r = state.row; r < state.visual.row + 1; r += 1) {
                decreaseIndentLevel(state, r);
            }
        } else {
            for (let r = state.visual.row; r < state.row + 1; r += 1) {
                decreaseIndentLevel(state, r);
            }
        }
        state.col = state.col - state.indentAmount >= 0 ? state.col - state.indentAmount : 0;
    } else if (key === 'CTRL_A') {
        state.col = firstNonSpace(state, state.row);
    } else if (key === 'CTRL_E') {
        state.col = endOfLine(state, state.row);
        right(state);
    } else if (isWritable(key)) {
        if (state.row < state.visual.row) {
            for (let r = state.row; r < state.visual.row + 1; r += 1) {
                state.data[r] = state.data[r].substring(0, state.col)
                    + key
                    + state.data[r].substring(state.col);
            }
        } else {
            for (let r = state.visual.row; r < state.row + 1; r += 1) {
                state.data[r] = state.data[r].substring(0, state.col)
                    + key
                    + state.data[r].substring(state.col);
            }
        }
        state.col += 1;
    } else if (key === 'ESCAPE') {
        if (isEmptyRow(state, state.row)) {
            state.data[state.row] = '';
        }
        left(state);
        state.mode = SHORTCUTS;
        createSnapshot(state);
    }
    logCommand(false, state, key);
    render(state, screen);
}

export {
    handleMultiCursorKeys
};
