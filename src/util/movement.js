/* eslint-disable import/no-cycle */
import * as helper from './helper.js';

function upHalfScreen(state) {
    for (let i = 0; i < process.stdout.rows / 2; i += 1) {
        if (state.row > 0) {
            state.row -= 1;
            if (state.windowLine > 0) {
                state.windowLine -= 1;
            }
        }
    }
}

function downHalfScreen(state) {
    for (let i = 0; i < process.stdout.rows / 2; i += 1) {
        if (state.row < state.data.length - 1) {
            state.row += 1;
            state.windowLine += 1;
        }
    }
}

function bottomOfFile(state) {
    while (state.row !== state.data.length - 1) {
        if (state.row < state.data.length - 1) {
            state.row += 1;
            if (state.row >= state.windowLine + process.stdout.rows) {
                state.windowLine += 1;
            }
        }
    }
}

function topOfFile(state) {
    while (state.row !== 0) {
        if (state.row > 0) {
            state.row -= 1;
            if (state.row < state.windowLine) {
                state.windowLine -= 1;
            }
        }
    }
}

function getCoorForwardWord(state) {
    let endOfWord = state.col;
    for (let i = state.col; i < state.data[state.row].length; i += 1) {
        if (!helper.isAlphaNumeric(state.data[state.row].substring(i, i + 1))) {
            endOfWord = i;
            break;
        } else if (i === state.data[state.row].length - 1) {
            endOfWord = state.data[state.row].length;
        }
    }
    return endOfWord;
}

function lowerIndentLevel(state) {
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
}

export {
    lowerIndentLevel,
    getCoorForwardWord,
    topOfFile,
    bottomOfFile,
    upHalfScreen,
    downHalfScreen,
};
