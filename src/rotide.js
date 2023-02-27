#!/usr/bin/env node
import fs from 'fs';
import pkg from 'terminal-kit';
const { terminal } = pkg;
import * as helper from './util/helper.js';
import { handleKeys } from './keybinds/normal.js';
import { handleVimKeys } from './keybinds/vim.js';

function getFile() {
    return process.argv[2] === '-v' ? process.argv[3] : process.argv[2];
}

const term = terminal();
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

helper.renderScreen(state, term);

term.on('key', (key) => {
    if (state.vim && state.mode !== 'i') {
        handleVimKeys(key, state, term);
    } else {
        handleKeys(key, state, term);
    }
});

term.on('mouse', (name, coor) => {
    if (name === 'MOUSE_LEFT_BUTTON_PRESSED') {
        state.col = (coor.x - 1) - 4 >= 0 ? (coor.x - 1) - 4 : 0;
        state.row = (coor.y - 1) + state.windowLine < state.data.length
            ? (coor.y - 1) + state.windowLine
            : state.data.length - 1;
        helper.moveCursor(state, term);
    } else if (name === 'MOUSE_WHEEL_DOWN') {
        if (state.windowLine > 0) {
            state.windowLine -= 1;
            helper.renderScreen(state, term);
        }
    } else if (name === 'MOUSE_WHEEL_UP') {
        if (state.windowLine + process.stdout.rows < state.data.length) {
            state.windowLine += 1;
            helper.renderScreen(state, term);
        }
    }
});

term.on('resize', () => {
    helper.renderScreen(state, term);
});
