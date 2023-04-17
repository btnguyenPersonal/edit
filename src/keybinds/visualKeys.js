/* eslint-disable import/no-cycle */
import ncp from 'copy-paste';
import {
    renderScreen,
    createSnapshot,
    logCommand
} from '../util/helper.js';
import {
    up,
    down,
    left,
    right,
    firstNonSpace,
    getCoorBeginningLastWord,
    getCoorBeginningNextWord,
    upHalfScreen,
    downHalfScreen,
    getCoorsInsideCharSame,
    getCoorsInsideCharDiff,
    getCoorsInsideWord,
    setVisualHighlight,
    copyInVisual,
    deleteInVisual,
} from '../util/movement.js';

function handleVisualKeys(key, state, screen) {
    if (state.previousKeys === 'vi') {
        if (key === 'w') {
            const { beginning, end } = getCoorsInsideWord(state);
            setVisualHighlight(state, beginning, end);
        } else if (key === '(' || key === ')' || key === 'b') {
            const { beginning, end } = getCoorsInsideCharDiff(state, '(', ')');
            setVisualHighlight(state, beginning, end);
        } else if (key === '{' || key === '}' || key === 'B') {
            const { beginning, end } = getCoorsInsideCharDiff(state, '{', '}');
            setVisualHighlight(state, beginning, end);
        } else if (key === '\'' || key === '"') {
            const { beginning, end } = getCoorsInsideCharSame(state, key);
            setVisualHighlight(state, beginning, end);
        }
        state.previousKeys = '';
    } else if (key === 'CTRL_U') {
        upHalfScreen(state);
    } else if (key === 'CTRL_D') {
        downHalfScreen(state);
    } else if (key === 'UP' || key === 'k') {
        up(state);
    } else if (key === 'DOWN' || key === 'j') {
        down(state);
    } else if (key === 'LEFT' || key === 'h') {
        left(state);
    } else if (key === 'RIGHT' || key === 'l') {
        right(state);
    } else if (key === 'w') {
        state.col = getCoorBeginningNextWord(state);
    } else if (key === 'b') {
        state.col = getCoorBeginningLastWord(state);
    } else if (key === '$') {
        state.col = state.data[state.row].length; // issue with long lines rendering %^$
    } else if (key === '0') {
        state.col = 0;
    } else if (key === '^') {
        state.col = firstNonSpace(state, state.row);
    } else if (key === 'p' || key === 'P') {
        const systemPaste = ncp.paste().split('\n');
        if (state.clipboard !== systemPaste) {
            state.clipboard = systemPaste;
            state.clipboardNewLine = false;
        }
        if (state.clipboard.length > 0) {
            if (state.row === state.visual.row) {
                if (state.visual.col <= state.col) {
                    state.data[state.row] = state.data[state.row].substring(0, state.visual.col) + state.data[state.row].substring(state.col + 1);
                    state.col = state.visual.col;
                } else if (state.visual.col > state.col) {
                    state.data[state.row] = state.data[state.row].substring(0, state.col) + state.data[state.row].substring(state.visual.col + 1);
                }
            } else if (state.row > state.visual.row) {
                state.data[state.visual.row] = state.data[state.visual.row].substring(0, state.visual.col) + state.data[state.row].substring(state.col + 1);
                state.data.splice(state.visual.row + 1, state.row - state.visual.row);
                state.row = state.visual.row;
                state.col = state.visual.col;
            } else if (state.row < state.visual.row) {
                state.data[state.row] = state.data[state.row].substring(0, state.col) + state.data[state.visual.row].substring(state.visual.col + 1);
                state.data.splice(state.row + 1, state.visual.row - state.row);
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
    } else if (key === 'y') {
        copyInVisual(state);
        state.mode = 'n';
    } else if (key === 'x') {
        deleteInVisual(state);
        state.mode = 'n';
        createSnapshot(state);
    } else if (key === 'd') {
        copyInVisual(state);
        deleteInVisual(state);
        state.mode = 'n';
        createSnapshot(state);
    } else if (key === 'c') {
        copyInVisual(state);
        deleteInVisual(state);
        state.mode = 'i';
        createSnapshot(state);
    } else if (key === 'i') {
        state.previousKeys = 'vi';
    } else if (key === 'ESCAPE') {
        state.mode = 'n';
    }
    logCommand(false, state, key);
    renderScreen(state, screen);
}

export {
    handleVisualKeys
};
