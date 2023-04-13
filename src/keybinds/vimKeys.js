import * as helper from '../util/helper.js';

function handleVimKeys(key, state, screen, term) {
    if (state.previousKeys === 'd') {
        if (key === 'CTRL_S') {
            helper.saveFile(term, state.file, state.data);
        } else if (key === 'CTRL_C') {
            term.fullscreen(false);
            process.exit();
        } else if (key === 'd') {
            state.clipboard = [];
            state.clipboard.push(state.data[state.row]);
            state.clipboardNewLine = true;
            state.data.splice(state.row, 1);
            helper.renderScreen(state, screen);
        }
        state.previousKeys = '';
   } else if (state.previousKeys === 'y') {
        if (key === 'CTRL_S') {
            helper.saveFile(term, state.file, state.data);
        } else if (key === 'CTRL_C') {
            term.fullscreen(false);
            process.exit();
        } else if (key === 'j') {
            state.clipboard = [];
            state.clipboard.push(state.data[state.row]);
            if (state.data[state.row + 1]) {
                state.clipboard.push(state.data[state.row + 1]);
            }
            state.clipboardNewLine = true;
        } else if (key === 'k') {
            state.clipboard = [];
            if (state.data[state.row - 1]) {
                state.clipboard.push(state.data[state.row - 1]);
            }
            state.clipboard.push(state.data[state.row]);
            state.clipboardNewLine = true;
        } else if (key === 'y') {
            state.clipboard = [];
            state.clipboard.push(state.data[state.row]);
            state.clipboardNewLine = true;
        }
        state.previousKeys = '';
    } else if (state.previousKeys === 'r') {
        if (key === 'CTRL_S') {
            helper.saveFile(term, state.file, state.data);
        } else if (key === 'CTRL_C') {
            term.fullscreen(false);
            process.exit();
        } else if (helper.isWritable(key)) {
            state.data[state.row] = state.data[state.row].substring(0, state.col)
                + key
                + state.data[state.row].substring(state.col + 1);
            helper.renderScreen(state, screen);
        }
        state.previousKeys = '';
    } else if (state.previousKeys === 'y') {
        if (key === 'CTRL_S') {
            helper.saveFile(term, state.file, state.data);
        } else if (key === 'CTRL_C') {
            term.fullscreen(false);
            process.exit();
        } else if (key === 'j') {
            state.clipboard = [];
            state.clipboard.push(state.data[state.row]);
            if (state.data[state.row + 1]) {
                state.clipboard.push(state.data[state.row + 1]);
            }
            state.clipboardNewLine = true;
        } else if (key === 'k') {
            state.clipboard = [];
            if (state.data[state.row - 1]) {
                state.clipboard.push(state.data[state.row - 1]);
            }
            state.clipboard.push(state.data[state.row]);
            state.clipboardNewLine = true;
        } else if (key === 'y') {
            state.clipboard = [];
            state.clipboard.push(state.data[state.row]);
            state.clipboardNewLine = true;
        }
        state.previousKeys = '';
     } else if (state.previousKeys === 'g') {
        if (key === 'CTRL_S') {
            helper.saveFile(term, state.file, state.data);
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
            helper.renderScreen(state, screen);
        }
        state.previousKeys = '';
    } else if (state.previousKeys === 'c') {
        if (key === 'CTRL_S') {
            helper.saveFile(term, state.file, state.data);
        } else if (key === 'CTRL_C') {
            term.fullscreen(false);
            process.exit();
        } else if (key === 'c') {
            state.data[state.row] = '';
            state.mode = 'i';
            helper.renderScreen(state, screen);
        }
        state.previousKeys = '';
    } else if (state.mode === 'v') {
        if (key === 'CTRL_S') {
            helper.saveFile(term, state.file, state.data);
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
            helper.renderScreen(state, screen);
        } else if (key === 'CTRL_D') {
            for (let i = 0; i < process.stdout.rows / 2; i += 1) {
                if (state.row < state.data.length - 1) {
                    state.row += 1;
                    state.windowLine += 1;
                }
            }
            helper.renderScreen(state, screen);
        } else if (key === 'UP' || key === 'k') {
            if (state.row > 0) {
                state.row -= 1;
                if (state.row < state.windowLine) {
                    state.windowLine -= 1;
                }
                helper.renderScreen(state, screen);
            }
        } else if (key === 'DOWN' || key === 'j') {
            if (state.row < state.data.length - 1) {
                state.row += 1;
                if (state.row >= state.windowLine + process.stdout.rows) {
                    state.windowLine += 1;
                }
                helper.renderScreen(state, screen);
            }
        } else if (key === 'LEFT' || key === 'h') {
            if (state.col > state.data[state.row].length) {
                state.col = state.data[state.row].length;
            }
            if (state.col > 0) {
                state.col -= 1;
            }
            helper.renderScreen(state, screen);
        } else if (key === 'RIGHT' || key === 'l') {
            if (state.col > state.data[state.row].length) {
                state.col = state.data[state.row].length;
            }
            if (state.col < state.data[state.row].length) {
                state.col += 1;
            }
            helper.renderScreen(state, screen);
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
            helper.renderScreen(state, screen);
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
            helper.renderScreen(state, screen);
        } else if (key === '$') {
            state.col = state.data[state.row].length; // issue with long lines rendering %^$
            helper.renderScreen(state, screen);
        } else if (key === '0') {
            state.col = 0;
            helper.renderScreen(state, screen);
        } else if (key === '^') {
            let firstNonSpace = 0;
            for (let i = 0; i < state.data[state.row].length; i += 1) {
                if (state.data[state.row].charAt(i) !== ' ') {
                    firstNonSpace = i;
                    break;
                }
            }
            state.col = firstNonSpace;
            helper.renderScreen(state, screen);
        } else if (key === 'y') {
            if (state.row === state.visual.row) {
                if (state.visual.col <= state.col) {
                    state.clipboard = [];
                    state.clipboard.push(state.data[state.row].substring(state.visual.col, state.col + 1));
                    state.clipboardNewLine = false;
                    state.col = state.visual.col;
                } else if (state.visual.col > state.col) {
                    state.clipboard = [];
                    state.clipboard.push(state.data[state.row].substring(state.col, state.visual.col + 1));
                    state.clipboardNewLine = false;
                }
            } else if (state.row > state.visual.row) {
                state.clipboard = [];
                state.clipboard.push(state.data[state.visual.row].substring(state.visual.col));
                for (let i = state.visual.row + 1; i < state.row; i++) {
                    state.clipboard.push(state.data[i]);
                }
                state.clipboard.push(state.data[state.row].substring(0, state.col + 1));
                state.clipboardNewLine = false;
            } else if (state.row < state.visual.row) {
                state.clipboard = [];
                state.clipboard.push(state.data[state.row].substring(state.col));
                for (let i = state.row + 1; i < state.visual.row; i++) {
                    state.clipboard.push(state.data[i]);
                }
                state.clipboard.push(state.data[state.visual.row].substring(0, state.visual.col + 1));
                state.clipboardNewLine = false;
            }
            state.mode = 'n';
            helper.renderScreen(state, screen);
        } else if (key === 'x') {
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
            state.mode = 'n';
            helper.renderScreen(state, screen);
        } else if (key === 'd') {
            if (state.row === state.visual.row) {
                if (state.visual.col <= state.col) {
                    state.clipboard = [];
                    state.clipboard.push(state.data[state.row].substring(state.visual.col, state.col + 1));
                    state.clipboardNewLine = false;
                    state.data[state.row] = state.data[state.row].substring(0, state.visual.col) + state.data[state.row].substring(state.col + 1);
                    state.col = state.visual.col;
                } else if (state.visual.col > state.col) {
                    state.clipboard = [];
                    state.clipboard.push(state.data[state.row].substring(state.col, state.visual.col + 1));
                    state.clipboardNewLine = false;
                    state.data[state.row] = state.data[state.row].substring(0, state.col) + state.data[state.row].substring(state.visual.col + 1);
                }
            } else if (state.row > state.visual.row) {
                state.clipboard = [];
                state.clipboard.push(state.data[state.visual.row].substring(state.visual.col));
                for (let i = state.visual.row + 1; i < state.row; i++) {
                    state.clipboard.push(state.data[i]);
                }
                state.clipboard.push(state.data[state.row].substring(0, state.col + 1));
                state.clipboardNewLine = false;
            } else if (state.row < state.visual.row) {
                state.clipboard = [];
                state.clipboard.push(state.data[state.row].substring(state.col));
                for (let i = state.row + 1; i < state.visual.row; i++) {
                    state.clipboard.push(state.data[i]);
                }
                state.clipboard.push(state.data[state.visual.row].substring(0, state.visual.col + 1));
                state.clipboardNewLine = false;
            }
            state.mode = 'n';
            helper.renderScreen(state, screen);
        } else if (key === 'c') {
            if (state.row === state.visual.row) {
                if (state.visual.col <= state.col) {
                    state.clipboard = [];
                    state.clipboard.push(state.data[state.row].substring(state.visual.col, state.col + 1));
                    state.clipboardNewLine = false;
                    state.data[state.row] = state.data[state.row].substring(0, state.visual.col) + state.data[state.row].substring(state.col + 1);
                    state.col = state.visual.col;
                } else if (state.visual.col > state.col) {
                    state.clipboard = [];
                    state.clipboard.push(state.data[state.row].substring(state.col, state.visual.col + 1));
                    state.clipboardNewLine = false;
                    state.data[state.row] = state.data[state.row].substring(0, state.col) + state.data[state.row].substring(state.visual.col + 1);
                }
            } else if (state.row > state.visual.row) {
                state.clipboard = [];
                state.clipboard.push(state.data[state.visual.row].substring(state.visual.col));
                for (let i = state.visual.row + 1; i < state.row; i++) {
                    state.clipboard.push(state.data[i]);
                }
                state.clipboard.push(state.data[state.row].substring(0, state.col + 1));
                state.clipboardNewLine = false;
            } else if (state.row < state.visual.row) {
                state.clipboard = [];
                state.clipboard.push(state.data[state.row].substring(state.col));
                for (let i = state.row + 1; i < state.visual.row; i++) {
                    state.clipboard.push(state.data[i]);
                }
                state.clipboard.push(state.data[state.visual.row].substring(0, state.visual.col + 1));
                state.clipboardNewLine = false;
            }
            state.mode = 'i';
            helper.renderScreen(state, screen);
        } else if (key === 'ESCAPE') {
            state.mode = 'n';
            helper.renderScreen(state, screen);
        }
    } else if (state.mode === 'n') {
        if (key === 'CTRL_S') {
            helper.saveFile(term, state.file, state.data);
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
            helper.renderScreen(state, screen);
        } else if (key === 'CTRL_D') {
            for (let i = 0; i < process.stdout.rows / 2; i += 1) {
                if (state.row < state.data.length - 1) {
                    state.row += 1;
                    state.windowLine += 1;
                }
            }
            helper.renderScreen(state, screen);
        } else if (key === 'UP' || key === 'k') {
            if (state.row > 0) {
                state.row -= 1;
                if (state.row < state.windowLine) {
                    state.windowLine -= 1;
                }
                helper.renderScreen(state, screen);
            }
        } else if (key === 'DOWN' || key === 'j') {
            if (state.row < state.data.length - 1) {
                state.row += 1;
                if (state.row >= state.windowLine + process.stdout.rows) {
                    state.windowLine += 1;
                }
                helper.renderScreen(state, screen);
            }
        } else if (key === 'LEFT' || key === 'h') {
            if (state.col > state.data[state.row].length) {
                state.col = state.data[state.row].length;
            }
            if (state.col > 0) {
                state.col -= 1;
            }
            helper.renderScreen(state, screen);
        } else if (key === 'RIGHT' || key === 'l') {
            if (state.col > state.data[state.row].length) {
                state.col = state.data[state.row].length;
            }
            if (state.col < state.data[state.row].length) {
                state.col += 1;
            }
            helper.renderScreen(state, screen);
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
            helper.renderScreen(state, screen);
        } else if (key === 'x') {
            if (state.col < state.data[state.row].length) {
                state.data[state.row] = state.data[state.row].substring(0, state.col)
                    + state.data[state.row].substring(state.col + 1);
            }
            helper.renderScreen(state, screen);
        } else if (key === '$') {
            state.col = state.data[state.row].length; // issue with long lines rendering %^$
            helper.renderScreen(state, screen);
        } else if (key === '0') {
            state.col = 0;
            helper.renderScreen(state, screen);
        } else if (key === '^') {
            let firstNonSpace = 0;
            for (let i = 0; i < state.data[state.row].length; i += 1) {
                if (state.data[state.row].charAt(i) !== ' ') {
                    firstNonSpace = i;
                    break;
                }
            }
            state.col = firstNonSpace;
            helper.renderScreen(state, screen);
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
            helper.renderScreen(state, screen);
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
            helper.renderScreen(state, screen);
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
            helper.renderScreen(state, screen);
        } else if (key === 'A') {
            state.col = state.data[state.row].length;
            state.mode = 'i';
            helper.renderScreen(state, screen);
        } else if (key === 'S') {
            state.data[state.row] = '';
            state.mode = 'i';
            helper.renderScreen(state, screen);
        } else if (key === 'C') {
            state.data[state.row] = state.data[state.row].substring(0, state.col);
            state.mode = 'i';
            helper.renderScreen(state, screen);
        } else if (key === 'D') {
            state.data[state.row] = state.data[state.row].substring(0, state.col);
            helper.renderScreen(state, screen);
        } else if (key === 's') {
            if (state.col < state.data[state.row].length) {
                state.data[state.row] = state.data[state.row].substring(0, state.col)
                    + state.data[state.row].substring(state.col + 1);
            }
            state.mode = 'i';
            helper.renderScreen(state, screen);
        } else if (key === 'O') {
            state.col = 0;
            state.data.splice(state.row, 0, '');
            state.mode = 'i';
            helper.renderScreen(state, screen);
        } else if (key === 'o') {
            state.data.splice(state.row + 1, 0, '');
            state.row += 1;
            state.col = 0;
            state.mode = 'i';
            helper.renderScreen(state, screen);
        } else if (key === '>') {
            state.data[state.row] = '    ' + state.data[state.row];
            state.col += 4;
            helper.renderScreen(state, screen);
        } else if (key === '<') {
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
            helper.renderScreen(state, screen);
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
            helper.renderScreen(state, screen);
        } else if (key === 'p') {
            if (state.clipboard.length > 0) {
                if (state.clipboardNewLine) {
                    for (let i = state.clipboard.length - 1; i >= 0; i--) {
                        state.data.splice(state.row + 1, 0, state.clipboard[i]);
                    }
                } else {
                    const cutoff = state.data[state.row].substring(state.col + 1);
                    state.data[state.row] = state.data[state.row].substring(0, state.col + 1) + state.clipboard[0];
                    let counterRow = state.row;
                    for (let i = state.clipboard.length - 1; i >= 1; i--) {
                        state.data.splice(state.row + 1, 0, state.clipboard[i]);
                        counterRow++;
                    }
                    state.data[counterRow] += cutoff;
                }
                helper.renderScreen(state, screen);
            }
        } else if (key === 'P') {
            if (state.clipboard.length > 0) {
                if (state.clipboardNewLine) {
                    for (let i = state.clipboard.length - 1; i >= 0; i--) {
                        state.data.splice(state.row, 0, state.clipboard[i]);
                    }
                } else {
                    const cutoff = state.data[state.row].substring(state.col);
                    state.data[state.row] = state.data[state.row].substring(0, state.col) + state.clipboard[0];
                    let counterRow = state.row;
                    for (let i = state.clipboard.length - 1; i >= 1; i--) {
                        state.data.splice(state.row + 1, 0, state.clipboard[i]);
                        counterRow++;
                    }
                    state.data[counterRow] += cutoff;
                }
                helper.renderScreen(state, screen);
            }
        } else if (key === 'r') {
            state.previousKeys = 'r';
        } else if (key === 'c') {
            state.previousKeys = 'c';
        } else if (key === 'd') {
            state.previousKeys = 'd';
        } else if (key === 'y') {
            state.previousKeys = 'y';
        } else if (key === '/') {
            state.previousKeys = '/';
        } else if (key === 'v') {
            state.mode = 'v';
            state.visual = {
                row: state.row,
                col: state.col
            }
        }
    }
}

export {
    handleVimKeys
};
