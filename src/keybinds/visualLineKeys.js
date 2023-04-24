/* eslint-disable import/no-cycle */
import ncp from 'copy-paste';
import {
    copyToClipboard,
    renderScreen,
    createSnapshot,
    logCommand
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
    isCommented,
    uncomment,
    comment,
} from '../util/movement.js';

function handleVisualLineKeys(key, state, screen) {
    if (key === 'CTRL_U') {
        upHalfScreen(state);
    } else if (key === 'CTRL_D') {
        downHalfScreen(state);
    } else if (key === 'UP' || key === 'k') {
        up(state);
    } else if (key === 'DOWN' || key === 'j') {
        down(state);
    } else if (key === 'y') {
        if (state.row >= state.visualLine.row) {
            const newClipboard = [];
            for (let i = state.visualLine.row; i <= state.row; i += 1) {
                newClipboard.push(state.data[i]);
            }
            copyToClipboard(state, newClipboard, true);
            state.row = state.visualLine.row;
        } else if (state.row < state.visualLine.row) {
            const newClipboard = [];
            for (let i = state.row; i <= state.visualLine.row; i += 1) {
                newClipboard.push(state.data[i]);
            }
            copyToClipboard(state, newClipboard, true);
        }
        state.mode = 'n';
    } else if (key === 'c') {
        if (state.previousKeys === 'g') {
            let areAllCommented = true;
            if (state.row >= state.visualLine.row) {
                for (let i = state.visualLine.row; i <= state.row; i += 1) {
                    if (!isCommented(state, i)) {
                        areAllCommented = false;
                        break;
                    }
                }
                if (areAllCommented) {
                    for (let i = state.visualLine.row; i <= state.row; i += 1) {
                        uncomment(state, i);
                    }
                } else {
                    for (let i = state.visualLine.row; i <= state.row; i += 1) {
                        comment(state, i, firstNonSpace(state, state.visualLine.row));
                    }
                }
                state.row = state.visualLine.row;
            } else if (state.row < state.visualLine.row) {
                for (let i = state.row; i <= state.visualLine.row; i += 1) {
                    if (!isCommented(state, i)) {
                        areAllCommented = false;
                        break;
                    }
                }
                if (areAllCommented) {
                    for (let i = state.row; i <= state.visualLine.row; i += 1) {
                        uncomment(state, i);
                    }
                } else {
                    for (let i = state.row; i <= state.visualLine.row; i += 1) {
                        comment(state, i, firstNonSpace(state, state.row));
                    }
                }
            }
            state.mode = 'n';
            createSnapshot(state);
            state.previousKeys = '';
        } else {
            if (state.row >= state.visualLine.row) {
                const newClipboard = [];
                for (let i = state.visualLine.row; i <= state.row; i += 1) {
                    newClipboard.push(state.data[i]);
                }
                copyToClipboard(state, newClipboard, true);
                state.data.splice(state.visualLine.row, state.row - state.visualLine.row + 1);
                state.row = state.visualLine.row;
            } else if (state.row < state.visualLine.row) {
                const newClipboard = [];
                for (let i = state.row; i <= state.visualLine.row; i += 1) {
                    newClipboard.push(state.data[i]);
                }
                copyToClipboard(state, newClipboard, true);
                state.data.splice(state.row, state.visualLine.row - state.row + 1);
            }
            const indentLevel = getIndentLevelFrom(state, state.row);
            state.data.splice(state.row, 0, ' '.repeat(indentLevel));
            state.col = indentLevel;
            state.mode = 'i';
        }
    } else if (key === 'd') {
        if (state.row >= state.visualLine.row) {
            const newClipboard = [];
            for (let i = state.visualLine.row; i <= state.row; i += 1) {
                newClipboard.push(state.data[i]);
            }
            copyToClipboard(state, newClipboard, true);
            state.data.splice(state.visualLine.row, state.row - state.visualLine.row + 1);
            state.row = state.visualLine.row;
        } else if (state.row < state.visualLine.row) {
            const newClipboard = [];
            for (let i = state.row; i <= state.visualLine.row; i += 1) {
                newClipboard.push(state.data[i]);
            }
            copyToClipboard(state, newClipboard, true);
            state.data.splice(state.row, state.visualLine.row - state.row + 1);
        }
        firstNonSpace(state, state.row);
        state.mode = 'n';
        createSnapshot(state);
    } else if (key === 'g') {
        state.previousKeys = 'g';
    } else if (key === 'G') {
        bottomOfFile(state);
    } else if (key === '<') {
        if (state.row >= state.visualLine.row) {
            for (let i = state.visualLine.row; i <= state.row; i += 1) {
                decreaseIndentLevel(state, i);
            }
        } else if (state.row < state.visualLine.row) {
            for (let i = state.row; i <= state.visualLine.row; i += 1) {
                decreaseIndentLevel(state, i);
            }
        }
        state.col = 0;
        state.row = state.visualLine.row;
        state.mode = 'n';
        createSnapshot(state);
    } else if (key === '>') {
        if (state.row >= state.visualLine.row) {
            for (let i = state.visualLine.row; i <= state.row; i += 1) {
                increaseIndentLevel(state, i);
            }
            state.row = state.visualLine.row;
        } else if (state.row < state.visualLine.row) {
            for (let i = state.row; i <= state.visualLine.row; i += 1) {
                increaseIndentLevel(state, i);
            }
        }
        state.mode = 'n';
        createSnapshot(state);
    } else if (key === 'x') {
        if (state.row >= state.visualLine.row) {
            state.data.splice(state.visualLine.row, state.row - state.visualLine.row + 1);
            state.row = state.visualLine.row;
        } else if (state.row < state.visualLine.row) {
            state.data.splice(state.row, state.visualLine.row - state.row + 1);
        }
        state.mode = 'n';
        createSnapshot(state);
    } else if (key === 'p' || key === 'P') {
        const systemPaste = ncp.paste().split('\n');
        if (state.clipboard !== systemPaste) {
            state.clipboard = systemPaste;
            state.clipboardNewLine = true;
        }
        if (state.clipboard.length > 0) {
            if (state.row >= state.visualLine.row) {
                state.data.splice(state.visualLine.row, state.row - state.visualLine.row + 1);
                state.row = state.visualLine.row;
            } else if (state.row < state.visualLine.row) {
                state.data.splice(state.row, state.visualLine.row - state.row + 1);
            }
            if (state.clipboardNewLine) {
                for (let i = state.clipboard.length - 1; i >= 0; i -= 1) {
                    state.data.splice(state.row, 0, state.clipboard[i]);
                }
            } else {
                const cutoff = state.data[state.row].substring(state.col);
                state.data[state.row] = state.data[state.row].substring(0, state.col) + state.clipboard[0];
                let counterRow = state.row;
                for (let i = state.clipboard.length - 1; i >= 1; i -= 1) {
                    state.data.splice(state.row + 1, 0, state.clipboard[i]);
                    counterRow += 1;
                }
                state.data[counterRow] += cutoff;
            }
        }
        state.mode = 'n';
        createSnapshot(state);
    } else if (key === 'ESCAPE') {
        state.mode = 'n';
    }
    logCommand(false, state, key);
    renderScreen(state, screen);
}

export {
    handleVisualLineKeys
};
