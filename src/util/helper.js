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

function renderScreen(state, screen) {
    screen.fill({ char: ' ' });
    screen.moveTo(0, 0);
    for (let i = state.windowLine; i < (state.windowLine + process.stdout.rows); i += 1) {
        if (state.data[i] !== undefined) {
            screen.put({ x: 0 }, i.toString().padStart(4) + ' ');
            screen.put({ wrap: true }, state.data[i]);
            screen.put({ newLine: true }, '\n');
        }
    }
    screen.draw({ delta: false });
    moveCursor(state, screen);
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

                // color highlighting stuff
                // if (state.mode === 'v'){
                //     if (i < state.row && i > state.visual.begin.row) {
                //         screen.bgBlue();
                //     } else {
                //         if (i === state.row && i === state.visual.begin.row) {
                //             if (j >= state.visual.begin.col && j <= state.col) {
                //                 screen.bgBlue();
                //             } else {
                //                 screen.bgDefaultColor();
                //             }
                //         } else if (i === state.visual.begin.row) {
                //             if (j >= state.visual.begin.col) {
                //                 screen.bgBlue();
                //             } else {
                //                 screen.bgDefaultColor();
                //             }
                //         } else if (i === state.row) {
                //             if (j <= state.col) {
                //                 screen.bgBlue();
                //             } else {
                //                 screen.bgDefaultColor();
                //             }
                //         }
                //     }
                // } else {
                //     screen.bgDefaultColor();
                // }