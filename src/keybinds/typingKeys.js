import {
    isWritable,
    renderScreen,
    createSnapshot,
    getSortedSubstrings,
    getCurrentWord,
    logCommand
} from '../util/helper.js';
import {
    SHORTCUTS
} from '../util/modes.js';
import {
    up,
    down,
    left,
    right,
    getIndentLevelFrom,
    isEmptyRow,
    endOfLine,
    firstNonSpace,
    getCoorBeginningLastWord,
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
    } else if (key === 'CTRL_W') {
        const coor = getCoorBeginningLastWord(state);
        state.data[state.row] = state.data[state.row].substring(0, coor) + state.data[state.row].substring(state.col + 1);
        state.col = coor;
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
    } else if (key === 'TAB') {
        const str = getCurrentWord(state.data[state.row], state.col);
        const replaceString = getSortedSubstrings(str, state.data);
        state.data[state.row] = state.data[state.row].substring(0, state.col - str.length)
            + replaceString
            + state.data[state.row].substring(state.col);
        state.col += replaceString.length - str.length;
    } else if (key === 'ENTER') {
        let indentLevel = state.data[state.row].search(/\S|$/);
        if (state.data[state.row].endsWith('{') && state.col === state.data[state.row].length) {
            indentLevel += 4;
        }
        if (state.data[state.row].endsWith('(') && state.col === state.data[state.row].length) {
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
        if (state.row >= state.windowLine + process.stdout.rows - 1) {
            state.windowLine += 1;
        }
    } else if (key === 'CTRL_A') {
        state.col = firstNonSpace(state, state.row);
    } else if (key === 'CTRL_E') {
        state.col = endOfLine(state, state.row);
        right(state);
    } else if (isWritable(key)) {
        state.data[state.row] = state.data[state.row].substring(0, state.col)
            + key
            + state.data[state.row].substring(state.col);
        state.col += 1;
        if ((key === '}' && firstNonSpace(state, state.row) === state.col - 1) || (key === ')' && firstNonSpace(state, state.row) === state.col - 1)) {
            let indentLevel = state.row - 1 < 0 ? 0 : getIndentLevelFrom(state, state.row - 1);
            if (state.data[state.row].trim().startsWith(')') || state.data[state.row].trim().startsWith('}')) {
                indentLevel = indentLevel - 4 >= 0 ? indentLevel - 4 : 0;
            }
            state.data[state.row] = ' '.repeat(indentLevel) + state.data[state.row].trim();
            state.col = firstNonSpace(state, state.row) + 1;
        }
    } else if (key === 'ESCAPE') {
        if (isEmptyRow(state, state.row)) {
            state.data[state.row] = '';
        }
        left(state);
        state.mode = SHORTCUTS;
        createSnapshot(state);
    }
    logCommand(false, state, key);
    renderScreen(state, screen);
}

export {
    handleKeys
};
