const term = require('terminal-kit').terminal;
const fs = require('fs');

function isWritable(s) {
    return 'qwertyuiop[]\\asdfghjkl;\'zxcvbnm,./`1234567890-=~!@#$%^&*()_+QWERTYUIOP{}|ASDFGHJKL:"ZXCVBNM<>?'.indexOf(s) > 1;
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
        displayData.push(`${i.toString().padStart(3)} ${state.data[i]}`);
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

const state = {
    file: process.argv[2],
    data: (
        process.argv.length >= 3
            ? fs.readFileSync(process.argv[2], 'utf-8')
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
        }
        renderScreen(state, term);
    } else if (key === 'BACKSPACE') {
        if (state.col > 0) {
            state.data[state.row] = state.data[state.row].substring(0, state.col - 1)
                + state.data[state.row].substring(state.col);
            state.col -= 1;
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
        state.data[state.row] = `    ${state.data[state.row]}`;
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
    }
});

term.on('mouse', (name, coor) => {
    if (name === 'MOUSE_LEFT_BUTTON_PRESSED') {
        state.col = coor.x;
        state.row = coor.y < state.data.length ? coor.y : state.data.length;
        moveCursor(state, term);
    }
});
