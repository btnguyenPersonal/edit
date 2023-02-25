const term = require('terminal-kit').terminal;
const fs = require('fs');

function moveCursor(state, terminal) {
    terminal.moveTo(
        state.col <= state.data[state.row - 1].length
            ? state.col
            : state.data[state.row - 1].length + 1,
        (state.row < state.data.length ? state.row : state.data.length) - state.windowLine,
    );
}

function renderScreen(state, terminal) {
    terminal.clear();
    const displayData = [];
    for (let i = state.windowLine; i < (state.windowLine + process.stdout.rows); i += 1) {
        displayData.push(state.data[i]);
    }
    terminal(displayData.join('\n'));
    moveCursor(state, terminal);
}

function saveFile(f, d) {
    (async () => fs.writeFile(f, d.join('\n'), (err) => {
        if (err) {
            console.log(err);
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
    row: 1,
    col: 1,
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
        if (state.row > 1) {
            state.row -= 1;
            if (state.row - 1 < state.windowLine) {
                state.windowLine -= 1;
            }
            renderScreen(state, term);
        }
    } else if (key === 'DOWN') {
        if (state.row < state.data.length) {
            state.row += 1;
            if (state.row - 1 >= state.windowLine + process.stdout.rows) {
                state.windowLine += 1;
            }
            renderScreen(state, term);
        }
    } else if (key === 'LEFT') {
        if (state.col > state.data[state.row - 1].length) {
            state.col = state.data[state.row - 1].length + 1;
        }
        if (state.col > 1) {
            state.col -= 1;
        }
        moveCursor(state, term);
    } else if (key === 'RIGHT') {
        if (state.col > state.data[state.row - 1].length) {
            state.col = state.data[state.row - 1].length + 1;
        }
        if (state.col <= state.data[state.row - 1].length) {
            state.col += 1;
        }
        moveCursor(state, term);
    } else if (key === 'BACKSPACE') {
        if (state.col > 0) {
            state.data[state.row - 1] = state.data[state.row - 1].substring(0, state.col - 2)
                + state.data[state.row - 1].substring(state.col - 1);
            state.col -= 1;
        }
        renderScreen(state, term);
    } else {
        state.data[state.row - 1] = state.data[state.row - 1].substring(0, state.col - 1)
            + key
            + state.data[state.row - 1].substring(state.col - 1);
        state.col += 1;
        renderScreen(state, term);
    }
});

term.on('mouse', (name, coor) => {
    if (name === 'MOUSE_LEFT_BUTTON_PRESSED') {
        state.col = coor.x;
        state.row = coor.y < state.data.length ? coor.y : state.data.length;
        moveCursor(state, term);
    }
});
