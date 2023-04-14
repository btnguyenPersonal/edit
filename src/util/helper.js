import fs from 'fs';

function isAlphaNumeric(s) {
    return '1234567890QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm'.indexOf(s) > -1;
}

function isWritable(s) {
    return ' qwertyuiop[]\\asdfghjkl;\'zxcvbnm,./`1234567890-=~!@#$%^&*()_+QWERTYUIOP{}|ASDFGHJKL:"ZXCVBNM<>?'.indexOf(s) > -1;
}

function moveCursor(state, screen) {
    const r = state.row < state.data.length
        ? state.row
        : state.data.length - 1;
    screen.moveTo(
        (state.col < state.data[r].length
            ? state.col + 1
            : state.data[r].length + 1) + 4,
        (state.row < state.data.length ? state.row : state.data.length) - state.windowLine,
    );
    screen.drawCursor();
}

function isHighlighted(state, i, j) {
    if (state.mode === 'V'){
        if (state.row === i && state.col === j) {
            return false;
        }
        if ((i <= state.row && i >= state.visualLine.row) || (i >= state.row && i <= state.visualLine.row)) {
            return true;
        }
    } else if (state.mode === 'v'){
        if (state.row === i && state.col === j) {
            return false;
        }
        if ((i < state.row && i > state.visual.row) || (i > state.row && i < state.visual.row)) {
            return true;
        } else {
            if (i === state.row && i === state.visual.row) {
                if (state.visual.col <= state.col) {
                    if (j >= state.visual.col && j <= state.col) {
                        return true;
                    }
                } else if (state.visual.col > state.col) {
                    if (j >= state.col && j <= state.visual.col) {
                        return true;
                    }
                }
            } else if (i === state.visual.row) {
                if (j >= state.visual.col && state.row > state.visual.row) {
                    return true;
                } else if (j <= state.visual.col && state.row < state.visual.row) {
                    return true
                }
            } else if (i === state.row) {
                if (j <= state.col && state.row > state.visual.row) {
                    return true;
                } else if (j >= state.col && state.row < state.visual.row) {
                    return true
                }
            }
        }
    }
    return false;
}

function getColor(s) {
    if (s === '(' || s === ')') {
        return 'yellow';
    } else if (s === '"' || s === '\'') {
        return 'red';
    } else if (s === '[' || s === ']') {
        return 'green';
    } else if (s === '{' || s === '}') {
        return 'cyan';
    } else {
        return 'white';
    }
}

function renderScreen(state, screen) {
    screen.fill({ char: ' ' });
    screen.moveTo(0, 0);
    for (let i = state.windowLine; i < (state.windowLine + process.stdout.rows); i += 1) {
        if (state.data[i] !== undefined) {
            screen.put({ x: 0 }, i.toString().padStart(4) + ' ');
            for (let j = 0; j < state.data[i].length; j++) {
                screen.put({
                    attr: {
                        color: getColor(state.data[i].substring(j, j + 1)),
                        inverse: isHighlighted(state, i, j)
                    },
                    wrap: true
                }, state.data[i].substring(j, j + 1));
            }
            screen.put({ newLine: true }, '\n');
        }
    }
    screen.draw({ delta: false });
    moveCursor(state, screen);
}

function saveFile(term, f, d) {
    (async () => fs.writeFile(f, d.join('\n'), (err) => {
        if (err) {
            term.fullScreen(false);
            console.log(err);
            process.exit();
        }
    }))();
}

export {
    isAlphaNumeric,
    isWritable,
    moveCursor,
    renderScreen,
    saveFile
};
