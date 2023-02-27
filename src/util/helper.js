import 'fs';

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
    moveCursor(state, terminal);
}

function saveFile(f, d) {
    (async () => fs.writeFile(f, d.join('\n'), (err) => {
        if (err) {
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