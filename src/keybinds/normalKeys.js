import * as helper from '../util/helper.js';

function handleKeys(key, state, term) {
    if (key === 'CTRL_S') {
        helper.saveFile(state.file, state.data);
    } else if (key === 'CTRL_C') {
        term.fullscreen(false);
        process.exit();
    } else if (key === 'UP') {
        if (state.row > 0) {
            state.row -= 1;
            if (state.row < state.windowLine) {
                state.windowLine -= 1;
                helper.renderScreen(state, term);
            }
            helper.moveCursor(state, term);
        }
    } else if (key === 'DOWN') {
        if (state.row < state.data.length - 1) {
            state.row += 1;
            if (state.row >= state.windowLine + process.stdout.rows) {
                state.windowLine += 1;
                helper.renderScreen(state, term);
            }
            helper.moveCursor(state, term);
        }
    } else if (key === 'LEFT') {
        if (state.col > state.data[state.row].length) {
            state.col = state.data[state.row].length;
        }
        if (state.col > 0) {
            state.col -= 1;
        }
        helper.moveCursor(state, term);
    } else if (key === 'RIGHT') {
        if (state.col > state.data[state.row].length) {
            state.col = state.data[state.row].length;
        }
        if (state.col < state.data[state.row].length) {
            state.col += 1;
        }
        helper.moveCursor(state, term);
    } else if (key === 'DELETE') {
        if (state.col < state.data[state.row].length) {
            state.data[state.row] = state.data[state.row].substring(0, state.col)
                + state.data[state.row].substring(state.col + 1);
        } else if (state.row < state.data.length - 1) {
            state.data[state.row] += state.data[state.row + 1];
            state.data.splice(state.row + 1, 1);
        }
        helper.renderScreen(state, term);
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
        helper.renderScreen(state, term);
    } else if (key === 'ENTER') {
        if (state.data[state.row].substring(state.col)) {
            state.data.splice(state.row + 1, 0, state.data[state.row].substring(state.col));
            state.data[state.row] = state.data[state.row].substring(0, state.col);
        } else {
            state.data.splice(state.row + 1, 0, '');
        }
        state.row += 1;
        state.col = 0;
        if (state.row >= state.windowLine + process.stdout.rows) {
            state.windowLine += 1;
        }
        helper.renderScreen(state, term);
    } else if (key === 'TAB') {
        state.data[state.row] = '    ' + state.data[state.row];
        state.col += 4;
        helper.renderScreen(state, term);
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
        helper.renderScreen(state, term);
    } else if (helper.isWritable(key)) {
        state.data[state.row] = state.data[state.row].substring(0, state.col)
            + key
            + state.data[state.row].substring(state.col);
        state.col += 1;
        term.insert(1);
        term.noFormat(key);
        helper.moveCursor(state, term);
    } else if (key === 'ESCAPE') {
        state.mode = 'n';
    }
}

export {
    handleKeys
};