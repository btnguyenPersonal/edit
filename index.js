const term = require('terminal-kit').terminal;
const fs = require('fs');
let file;
if (process.argv.length >= 3) {
    file = process.argv[2];
}
let rawdata = file ? fs.readFileSync(file, 'utf-8') : 'default data';
let data = rawdata.split('\n');

term.grabInput({ mouse: 'button' });
term.fullscreen(true);
term.windowTitle('rotide');
term(data.join('\n'));
let row = 1;
let col = 1;
moveCursor(col, row, data, term);

function moveCursor(col, row, data, term) {
    term.moveTo(
        col < data[row - 1].length ? col : data[row - 1].length,
        row
    );
}

term.on('key', (key) => {
    if (key === 'CTRL_C') {
        term.fullscreen(false);
        process.exit();
    } else if (key === 'UP') {
        if (row > 1) {
            row--;
        }
        moveCursor(col, row, data, term);
    } else if (key === 'DOWN') {
        if (row < data.length) {
            row++;
        }
        moveCursor(col, row, data, term);
    } else if (key === 'LEFT') {
        if (col >= data[row - 1].length) {
            col = data[row - 1].length;
        }
        if (col > 1) {
            col--;
        }
        moveCursor(col, row, data, term);
    } else if (key === 'RIGHT') {
        if (col < data[row - 1].length) {
            col++;
        }
        moveCursor(col, row, data, term);
    } else if (key === 'BACKSPACE') {
        term.backDelete();
    } else {
        term.noFormat(key);
    }
});

term.on('mouse', (name, coor) => {
    if (name === 'MOUSE_LEFT_BUTTON_PRESSED') {
        col = coor.x;
        row = coor.y < data.length ? coor.y : data.length;
        moveCursor(col, row, data, term);
    }
});
