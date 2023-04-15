/* eslint-disable import/no-cycle */
import * as helper from '../util/helper.js';

function handleKeys(key, state, screen) {
    if (key === 'UP') {
        if (state.row > 0) {
            state.row -= 1;
            if (state.row < state.windowLine) {
                state.windowLine -= 1;
            }
        }
        helper.renderScreen(state, screen);
    } else if (key === 'DOWN') {
        if (state.row < state.data.length - 1) {
            state.row += 1;
            if (state.row >= state.windowLine + process.stdout.rows) {
                state.windowLine += 1;
            }
        }
        helper.renderScreen(state, screen);
    } else if (key === 'LEFT') {
        if (state.col > state.data[state.row].length) {
            state.col = state.data[state.row].length;
        }
        if (state.col > 0) {
            state.col -= 1;
        }
        helper.renderScreen(state, screen);
    } else if (key === 'RIGHT') {
        if (state.col > state.data[state.row].length) {
            state.col = state.data[state.row].length;
        }
        if (state.col < state.data[state.row].length) {
            state.col += 1;
        }
        helper.renderScreen(state, screen);
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
            helper.createSnapshot(state);
        }
        helper.renderScreen(state, screen);
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
            helper.createSnapshot(state);
        }
        helper.renderScreen(state, screen);
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
            helper.createSnapshot(state);
        }
        helper.renderScreen(state, screen);
    } else if (key === 'TAB') {
        state.data[state.row] = '    ' + state.data[state.row];
        state.col += 4;
        state.isSaved = false;
        if (!state.vim) {
            helper.createSnapshot(state);
        }
        helper.renderScreen(state, screen);
    } else if (key === 'SHIFT_TAB') { // shit implementation but can't be bothered
        let tempLine = state.data[state.row];
        let dont = false;
        for (let i = 3; i >= 0; i -= 1) {
            if (dont) {
                break;
            }
            for (let j = i; j >= 0; j -= 1) {
                if (tempLine.substring(j, j + 1) !== ' ') {
                    dont = true;
                }
            }
            if (dont) {
                break;
            }
            if (tempLine.substring(i, i + 1) === ' ') {
                tempLine = tempLine.substring(0, i) + tempLine.substring(i + 1);
                if (state.col > 1) {
                    state.col -= 1;
                }
            } else {
                break;
            }
        }
        state.data[state.row] = tempLine;
        state.isSaved = false;
        if (!state.vim) {
            helper.createSnapshot(state);
        }
        helper.renderScreen(state, screen);
    } else if (helper.isWritable(key)) {
        state.data[state.row] = state.data[state.row].substring(0, state.col)
            + key
            + state.data[state.row].substring(state.col);
        state.col += 1;
        state.isSaved = false;
        if (!state.vim) {
            helper.createSnapshot(state);
        }
        helper.renderScreen(state, screen);
    } else if (key === 'ESCAPE') {
        state.mode = 'n';
        if (state.vim) {
            helper.createSnapshot(state);
        }
        helper.renderScreen(state, screen);
    } else if (!state.vim && key === 'CTRL_Y') {
        if (state.currentSnapshot + 1 < state.snapshots.length) {
            helper.applySnapshot(state, state.currentSnapshot + 1);
        }
        helper.renderScreen(state, screen);
    } else if (!state.vim && key === 'CTRL_Z') {
        if (state.currentSnapshot - 1 >= 0) {
            helper.applySnapshot(state, state.currentSnapshot - 1);
        }
        helper.renderScreen(state, screen);
    }
    if (state.vim) {
        helper.logCommand(false, state, key);
    }
}

export {
    handleKeys
};
