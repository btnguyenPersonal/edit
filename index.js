const term = require('terminal-kit').terminal;

term.grabInput({ mouse: 'button' });
term.fullscreen(true);
term.windowTitle('rotide');

term.on('key', (key, data) => {
    if (key === 'CTRL_C') {
        term.fullscreen(false);
        process.exit();
    } else if (key === 'UP') {
        term.up(1);
    } else if (key === 'DOWN') {
        term.down(1);
    } else if (key === 'LEFT') {
        term.left(1);
    } else if (key === 'RIGHT') {
        term.right(1);
    } else if (key === 'BACKSPACE') {
        term.backDelete();
    } else {
        term.noFormat(key);
    }
});

term.on('mouse', (name, data) => {
    if (name === 'MOUSE_LEFT_BUTTON_PRESSED') {
        term.moveTo(data.x, data.y);
    }
});
