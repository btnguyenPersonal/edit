import * as helper from '../util/helper.js';

function handleVimKeys(key, state, term) {
    if (state.vim && state.previousKeys === 'd') {
        if (key === 'CTRL_S') {
            helper.saveFile(state.file, state.data);
        } else if (key === 'CTRL_C') {
            term.fullscreen(false);
            process.exit();
        } else if (key === 'd') {
            state.data.splice(state.row, 1);
            helper.renderScreen(state, term);
        }
        state.previousKeys = '';
    } else if (state.vim && state.previousKeys === 'g') {
        if (key === 'CTRL_S') {
            helper.saveFile(state.file, state.data);
        } else if (key === 'CTRL_C') {
            term.fullscreen(false);
            process.exit();
        } else if (key === 'g') {
            while (state.row !== 0) {
                if (state.row > 0) {
                    state.row -= 1;
                    if (state.row < state.windowLine) {
                        state.windowLine -= 1;
                    }
                }
            }
            helper.renderScreen(state, term);
        }
        state.previousKeys = '';
    } else if (state.vim && state.previousKeys === 'c') {
        if (key === 'CTRL_S') {
            saveFile(state.file, state.data);
        } else if (key === 'CTRL_C') {
            term.fullscreen(false);
            process.exit();
        } else if (key === 'c') {
            state.data[state.row] = '';
            state.mode = 'i';
            helper.renderScreen(state, term);
        }
        state.previousKeys = '';
    } else if (state.vim && state.mode === 'n') {
        if (key === 'CTRL_S') {
            helper.saveFile(state.file, state.data);
        } else if (key === 'CTRL_C') {
            term.fullscreen(false);
            process.exit();
        } else if (key === 'CTRL_U') {
            for (let i = 0; i < process.stdout.rows / 2; i += 1) {
                if (state.row > 0) {
                    state.row -= 1;
                    if (state.windowLine > 0) {
                        state.windowLine -= 1;
                    }
                }
            }
            helper.renderScreen(state, term);
        } else if (key === 'CTRL_D') {
            for (let i = 0; i < process.stdout.rows / 2; i += 1) {
                if (state.row < state.data.length - 1) {
                    state.row += 1;
                    state.windowLine += 1;
                }
            }
            helper.renderScreen(state, term);
        } else if (key === 'UP' || key === 'k') {
            if (state.row > 0) {
                state.row -= 1;
                if (state.row < state.windowLine) {
                    state.windowLine -= 1;
                    helper.renderScreen(state, term);
                }
                helper.moveCursor(state, term);
            }
        } else if (key === 'DOWN' || key === 'j') {
            if (state.row < state.data.length - 1) {
                state.row += 1;
                if (state.row >= state.windowLine + process.stdout.rows) {
                    state.windowLine += 1;
                    helper.renderScreen(state, term);
                }
                helper.moveCursor(state, term);
            }
        } else if (key === 'LEFT' || key === 'h') {
            if (state.col > state.data[state.row].length) {
                state.col = state.data[state.row].length;
            }
            if (state.col > 0) {
                state.col -= 1;
            }
            helper.moveCursor(state, term);
        } else if (key === 'RIGHT' || key === 'l') {
            if (state.col > state.data[state.row].length) {
                state.col = state.data[state.row].length;
            }
            if (state.col < state.data[state.row].length) {
                state.col += 1;
            }
            helper.moveCursor(state, term);
        } else if (key === 'i') {
            state.mode = 'i';
        } else if (key === 'a') {
            if (state.col > state.data[state.row].length) {
                state.col = state.data[state.row].length;
            }
            if (state.col < state.data[state.row].length) {
                state.col += 1;
            }
            state.mode = 'i';
            helper.moveCursor(state, term);
        } else if (key === 'x') {
            if (state.col < state.data[state.row].length) {
                state.data[state.row] = state.data[state.row].substring(0, state.col)
                    + state.data[state.row].substring(state.col + 1);
            }
            helper.renderScreen(state, term);
        } else if (key === '$') {
            state.col = state.data[state.row].length; // issue with long lines rendering %^$
            helper.moveCursor(state, term);
        } else if (key === '0') {
            state.col = 0;
            helper.moveCursor(state, term);
        } else if (key === '^') {
            let firstNonSpace = 0;
            for (let i = 0; i < state.data[state.row].length; i += 1) {
                if (state.data[state.row].charAt(i) !== ' ') {
                    firstNonSpace = i;
                    break;
                }
            }
            state.col = firstNonSpace;
            helper.moveCursor(state, term);
        } else if (key === 'I') {
            let firstNonSpace = 0;
            for (let i = 0; i < state.data[state.row].length; i += 1) {
                if (state.data[state.row].charAt(i) !== ' ') {
                    firstNonSpace = i;
                    break;
                }
            }
            state.col = firstNonSpace;
            state.mode = 'i';
            helper.moveCursor(state, term);
        } else if (key === 'w') {
            let hasHitNonAlphaNum = false;
            for (let i = state.col; i < state.data[state.row].length - 1; i += 1) {
                if (hasHitNonAlphaNum && helper.isAlphaNumeric(state.data[state.row].charAt(i))) {
                    state.col = i;
                    break;
                } else if (!hasHitNonAlphaNum) {
                    hasHitNonAlphaNum = !helper.isAlphaNumeric(state.data[state.row].charAt(i));
                }
            }
            helper.moveCursor(state, term);
        } else if (key === 'b') {
            let hasHitNonAlphaNum = false;
            for (let i = state.col; i >= 0; i -= 1) {
                if (hasHitNonAlphaNum && helper.isAlphaNumeric(state.data[state.row].charAt(i))) {
                    state.col = i;
                    break;
                } else if (!hasHitNonAlphaNum) {
                    hasHitNonAlphaNum = !helper.isAlphaNumeric(state.data[state.row].charAt(i));
                }
            }
            for (let i = state.col; i >= 0; i -= 1) {
                if (!helper.isAlphaNumeric(state.data[state.row].charAt(i))) {
                    break;
                }
                state.col = i;
            }
            helper.moveCursor(state, term);
        } else if (key === 'A') {
            state.col = state.data[state.row].length;
            state.mode = 'i';
            helper.moveCursor(state, term);
        } else if (key === 'S') {
            state.data[state.row] = '';
            state.mode = 'i';
            helper.renderScreen(state, term);
        } else if (key === 'C') {
            state.data[state.row] = state.data[state.row].substring(0, state.col);
            state.mode = 'i';
            helper.renderScreen(state, term);
        } else if (key === 'D') {
            state.data[state.row] = state.data[state.row].substring(0, state.col);
            helper.renderScreen(state, term);
        } else if (key === 's') {
            if (state.col < state.data[state.row].length) {
                state.data[state.row] = state.data[state.row].substring(0, state.col)
                    + state.data[state.row].substring(state.col + 1);
            }
            state.mode = 'i';
            helper.renderScreen(state, term);
        } else if (key === 'o') {
            state.data.splice(state.row + 1, 0, '');
            state.mode = 'i';
            helper.renderScreen(state, term);
        } else if (key === 'O') {
            state.data.splice(state.row, 0, '');
            state.mode = 'i';
            helper.renderScreen(state, term);
        } else if (key === 'g') {
            state.previousKeys = 'g';
        } else if (key === 'G') {
            while (state.row !== state.data.length - 1) {
                if (state.row < state.data.length - 1) {
                    state.row += 1;
                    if (state.row >= state.windowLine + process.stdout.rows) {
                        state.windowLine += 1;
                    }
                }
            }
            helper.renderScreen(state, term);
        } else if (key === 'c') {
            state.previousKeys = 'c';
        } else if (key === 'd') {
            state.previousKeys = 'd';
        } else if (key === 'y') {
            state.previousKeys = 'y';
        } else if (key === '/') {
            state.previousKeys = '/';
        } else if (key === 'v') {
            state.previousKeys = 'v';
        }
    }
}

export {
    handleVimKeys
};
