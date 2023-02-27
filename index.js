#!/usr/bin/env node
const term = require('terminal-kit').terminal;
const fs = require('fs');

function isAlphaNumeric(s) {
    return '1234567890QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm'.indexOf(s) > -1;
}

function isWritable(s) {
    return ' qwertyuiop[]\\asdfghjkl;\'zxcvbnm,./`1234567890-=~!@#$%^&*()_+QWERTYUIOP{}|ASDFGHJKL:"ZXCVBNM<>?'.indexOf(s) > -1;
}

function moveCursor(state, terminal) {
    terminal.moveTo(
        (state.col < state.data[state.row].length
            ? state.col + 1
            : state.data[state.row].length + 1) + 4,
        (state.row < state.data.length ? state.row + 1 : state.data.length) - state.windowLine,
    );
}

function renderScreen(state, terminal) {
    terminal.clear();
    const displayData = [];
    for (let i = state.windowLine; i < (state.windowLine + process.stdout.rows); i += 1) {
        if (state.data[i] !== undefined) {
            displayData.push(i.toString().padStart(3) + ' ' + state.data[i]);
        }
    }
    terminal(displayData.join('\n'));
    moveCursor(state, term);
}

function saveFile(f, d) {
    (async () => fs.writeFile(f, d.join('\n'), (err) => {
        if (err) {
            console.log(err);
            process.exit();
        }
    }))();
}

function getFile() {
    return process.argv[2] === '-v' ? process.argv[3] : process.argv[2];
}

const state = {
    vim: process.argv[2] === '-v',
    mode: 'n',
    previousKeys: '',
    file: getFile(),
    data: (
        process.argv.length >= 3
            ? fs.readFileSync(getFile(), 'utf-8')
            : ''
    ).split('\n'),
    row: 0,
    col: 0,
    windowLine: 0,
};

term.grabInput({ mouse: 'button' });
term.fullscreen(true);
term.windowTitle('rotide');

renderScreen(state, term);

term.on('key', (key) => {
    if (state.vim && state.previousKeys === 'd') {
        if (key === 'CTRL_S') {
            saveFile(state.file, state.data);
        } else if (key === 'CTRL_C') {
            term.fullscreen(false);
            process.exit();
        } else if (key === 'd') {
            state.data.splice(state.row, 1);
            renderScreen(state, term);
        }
        state.previousKeys = '';
    } else if (state.vim && state.previousKeys === 'g') {
        if (key === 'CTRL_S') {
            saveFile(state.file, state.data);
        } else if (key === 'CTRL_C') {
            term.fullscreen(false);
            process.exit();
        } else if (key === 'g') {
            while (state.row !== 0) {
                if (state.row > 0) {
                    state.row -= 1;
                    if (state.row < state.windowLine) {
                        state.windowLine -= 1;
                        renderScreen(state, term);
                    }
                    moveCursor(state, term);
                }
            }
            renderScreen(state, term);
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
            renderScreen(state, term);
        }
        state.previousKeys = '';
    } else if (state.vim && state.mode === 'n') {
        if (key === 'CTRL_S') {
            saveFile(state.file, state.data);
        } else if (key === 'CTRL_C') {
            term.fullscreen(false);
            process.exit();
        } else if (key === 'CTRL_U') {
            for (let i = 0; i < process.stdout.rows / 2; i += 1) {
                if (state.row > 0) {
                    state.row -= 1;
                    state.windowLine -= 1;
                }
            }
            renderScreen(state, term);
        } else if (key === 'CTRL_D') {
            for (let i = 0; i < process.stdout.rows / 2; i += 1) {
                if (state.row < state.data.length - 1) {
                    state.row += 1;
                    state.windowLine += 1;
                }
            }
            renderScreen(state, term);
        } else if (key === 'UP' || key === 'k') {
            if (state.row > 0) {
                state.row -= 1;
                if (state.row < state.windowLine) {
                    state.windowLine -= 1;
                    renderScreen(state, term);
                }
                moveCursor(state, term);
            }
        } else if (key === 'DOWN' || key === 'j') {
            if (state.row < state.data.length - 1) {
                state.row += 1;
                if (state.row >= state.windowLine + process.stdout.rows) {
                    state.windowLine += 1;
                    renderScreen(state, term);
                }
                moveCursor(state, term);
            }
        } else if (key === 'LEFT' || key === 'h') {
            if (state.col > state.data[state.row].length) {
                state.col = state.data[state.row].length;
            }
            if (state.col > 0) {
                state.col -= 1;
            }
            moveCursor(state, term);
        } else if (key === 'RIGHT' || key === 'l') {
            if (state.col > state.data[state.row].length) {
                state.col = state.data[state.row].length;
            }
            if (state.col < state.data[state.row].length) {
                state.col += 1;
            }
            moveCursor(state, term);
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
            moveCursor(state, term);
        } else if (key === 'x') {
            if (state.col < state.data[state.row].length) {
                state.data[state.row] = state.data[state.row].substring(0, state.col)
                    + state.data[state.row].substring(state.col + 1);
            }
            renderScreen(state, term);
        } else if (key === '$') {
            state.col = state.data[state.row].length; // issue with long lines rendering %^$
            moveCursor(state, term);
        } else if (key === '0') {
            state.col = 0;
            moveCursor(state, term);
        } else if (key === '^') {
            let firstNonSpace = 0;
            for (let i = 0; i < state.data[state.row].length; i += 1) {
                if (state.data[state.row].charAt(i) !== ' ') {
                    firstNonSpace = i;
                    break;
                }
            }
            state.col = firstNonSpace;
            moveCursor(state, term);
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
            moveCursor(state, term);
        } else if (key === 'w') {
            let hasHitNonAlphaNum = false;
            for (let i = state.col; i < state.data[state.row].length - 1; i += 1) {
                if (hasHitNonAlphaNum && isAlphaNumeric(state.data[state.row].charAt(i))) {
                    state.col = i;
                    break;
                } else if (!hasHitNonAlphaNum) {
                    hasHitNonAlphaNum = !isAlphaNumeric(state.data[state.row].charAt(i));
                }
            }
            moveCursor(state, term);
        } else if (key === 'b') {
            let hasHitNonAlphaNum = false;
            for (let i = state.col; i >= 0; i -= 1) {
                if (hasHitNonAlphaNum && isAlphaNumeric(state.data[state.row].charAt(i))) {
                    state.col = i;
                    break;
                } else if (!hasHitNonAlphaNum) {
                    hasHitNonAlphaNum = !isAlphaNumeric(state.data[state.row].charAt(i));
                }
            }
            for (let i = state.col; i >= 0; i -= 1) {
                if (!isAlphaNumeric(state.data[state.row].charAt(i))) {
                    break;
                }
                state.col = i;
            }
            moveCursor(state, term);
        } else if (key === 'A') {
            state.col = state.data[state.row].length;
            state.mode = 'i';
            moveCursor(state, term);
        } else if (key === 'S') {
            state.data[state.row] = '';
            state.mode = 'i';
            renderScreen(state, term);
        } else if (key === 'C') {
            state.data[state.row] = state.data[state.row].substring(0, state.col);
            state.mode = 'i';
            renderScreen(state, term);
        } else if (key === 'D') {
            state.data[state.row] = state.data[state.row].substring(0, state.col);
            renderScreen(state, term);
        } else if (key === 's') {
            if (state.col < state.data[state.row].length) {
                state.data[state.row] = state.data[state.row].substring(0, state.col)
                    + state.data[state.row].substring(state.col + 1);
            }
            state.mode = 'i';
            renderScreen(state, term);
        } else if (key === 'o') {
            state.data.splice(state.row + 1, 0, '');
            state.mode = 'i';
            renderScreen(state, term);
        } else if (key === 'O') {
            state.data.splice(state.row, 0, '');
            state.mode = 'i';
            renderScreen(state, term);
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
            renderScreen(state, term);
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
    } else {
        if (key === 'CTRL_S') {
            saveFile(state.file, state.data);
        } else if (key === 'CTRL_C') {
            term.fullscreen(false);
            process.exit();
        } else if (key === 'UP') {
            if (state.row > 0) {
                state.row -= 1;
                if (state.row < state.windowLine) {
                    state.windowLine -= 1;
                    renderScreen(state, term);
                }
                moveCursor(state, term);
            }
        } else if (key === 'DOWN') {
            if (state.row < state.data.length - 1) {
                state.row += 1;
                if (state.row >= state.windowLine + process.stdout.rows) {
                    state.windowLine += 1;
                    renderScreen(state, term);
                }
                moveCursor(state, term);
            }
        } else if (key === 'LEFT') {
            if (state.col > state.data[state.row].length) {
                state.col = state.data[state.row].length;
            }
            if (state.col > 0) {
                state.col -= 1;
            }
            moveCursor(state, term);
        } else if (key === 'RIGHT') {
            if (state.col > state.data[state.row].length) {
                state.col = state.data[state.row].length;
            }
            if (state.col < state.data[state.row].length) {
                state.col += 1;
            }
            moveCursor(state, term);
        } else if (key === 'DELETE') {
            if (state.col < state.data[state.row].length) {
                state.data[state.row] = state.data[state.row].substring(0, state.col)
                    + state.data[state.row].substring(state.col + 1);
            } else if (state.row < state.data.length - 1) {
                state.data[state.row] += state.data[state.row + 1];
                state.data.splice(state.row + 1, 1);
            }
            renderScreen(state, term);
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
            renderScreen(state, term);
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
            renderScreen(state, term);
        } else if (key === 'TAB') {
            state.data[state.row] = '    ' + state.data[state.row];
            state.col += 4;
            renderScreen(state, term);
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
            renderScreen(state, term);
        } else if (isWritable(key)) {
            state.data[state.row] = state.data[state.row].substring(0, state.col)
                + key
                + state.data[state.row].substring(state.col);
            state.col += 1;
            term.insert(1);
            term.noFormat(key);
            moveCursor(state, term);
        } else if (key === 'ESCAPE') {
            state.mode = 'n';
        }
    }
});

term.on('mouse', (name, coor) => {
    if (name === 'MOUSE_LEFT_BUTTON_PRESSED') {
        state.col = (coor.x - 1) - 4 >= 0 ? (coor.x - 1) - 4 : 0;
        state.row = (coor.y - 1) + state.windowLine < state.data.length
            ? (coor.y - 1) + state.windowLine
            : state.data.length - 1;
        moveCursor(state, term);
    } else if (name === 'MOUSE_WHEEL_DOWN') {
        if (state.windowLine > 0) {
            state.windowLine -= 1;
            renderScreen(state, term);
        }
    } else if (name === 'MOUSE_WHEEL_UP') {
        if (state.windowLine + process.stdout.rows < state.data.length) {
            state.windowLine += 1;
            renderScreen(state, term);
        }
    }
});

term.on('resize', () => {
    renderScreen(state, term);
});
