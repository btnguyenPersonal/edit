const term = require('terminal-kit').terminal;
const fs = require('fs');

function moveCursor(c, r, data, terminal, windowLine) {
    terminal.moveTo(
        c <= data[r - 1].length ? c : data[r - 1].length + 1,
        (r < data.length ? r : data.length) - windowLine,
    );
}

function renderScreen(c, r, data, terminal, windowLine) {
    terminal.clear();
    const displayData = [];
    for (let i = windowLine; i < (windowLine + process.stdout.rows); i++) {
        displayData.push(data[i]);
    }
    terminal(displayData.join('\n'));
    moveCursor(c, r, data, terminal, windowLine);
}

function saveFile(f, d) {
    (async () => await fs.writeFile(f, d.join('\n'), (err) => {
        if (err) {
            console.log(err);
        }
    }))();
}

const file = process.argv.length >= 3 ? process.argv[2] : undefined;
const rawdata = file ? fs.readFileSync(file, 'utf-8') : '';
const data = rawdata.split('\n');
let row = 1;
let col = 1;
let windowLine = 0;

term.grabInput({ mouse: 'button' });
term.fullscreen(true);
term.windowTitle('rotide');

moveCursor(col, row, data, term, windowLine);
renderScreen(col, row, data, term, windowLine);

term.on('key', (key) => {
    if (key === 'CTRL_S') {
        saveFile(file, data);
    } else if (key === 'CTRL_C') {
        term.fullscreen(false);
        process.exit();
    } else if (key === 'UP') {
        if (row > 1) {
            row -= 1;
            if (row - 1 < windowLine) {
                windowLine -= 1;
            }
            renderScreen(col, row, data, term, windowLine);
        }
    } else if (key === 'DOWN') {
        if (row < data.length) {
            row += 1;
            if (row - 1 > windowLine + process.stdout.rows) {
                windowLine += 1;
            }
            renderScreen(col, row, data, term, windowLine);
        }
    } else if (key === 'LEFT') {
        if (col > data[row - 1].length) {
            col = data[row - 1].length + 1;
        }
        if (col > 1) {
            col -= 1;
        }
        moveCursor(col, row, data, term, windowLine);
    } else if (key === 'RIGHT') {
        if (col > data[row - 1].length) {
            col = data[row - 1].length + 1;
        }
        if (col <= data[row - 1].length) {
            col += 1;
        }
        moveCursor(col, row, data, term, windowLine);
    } else if (key === 'BACKSPACE') {
        if (col > 0) {
            data[row - 1] = data[row - 1].substring(0, col - 2) + data[row - 1].substring(col - 1);
            col -= 1;
        }
        renderScreen(col, row, data, term, windowLine);
    } else {
        data[row - 1] = data[row - 1].substring(0, col - 1)
            + key
            + data[row - 1].substring(col - 1);
        col += 1;
        renderScreen(col, row, data, term, windowLine);
    }
});

term.on('mouse', (name, coor) => {
    if (name === 'MOUSE_LEFT_BUTTON_PRESSED') {
        col = coor.x;
        row = coor.y < data.length ? coor.y : data.length;
        moveCursor(col, row, data, term, windowLine);
    }
});
