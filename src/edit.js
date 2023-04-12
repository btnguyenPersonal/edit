#!/usr/bin/env node
import fs from 'fs';
import pkg from 'terminal-kit';
const { terminal } = pkg;
import * as helper from './util/helper.js';
import { handleKeys } from './keybinds/normalKeys.js';
import { handleVimKeys } from './keybinds/vimKeys.js';
import { handleMouseInputs } from './keybinds/mouse.js';

function getFile() {
    return process.argv[2] === '-v' ? process.argv[3] : process.argv[2];
}

process.title = 'edit';
const term = terminal();
const state = {
    vim: process.argv[2] === '-v',
    mode: 'n',
    clipboard: [],
    visual: {
        begin: {
            row: null,
            col: null
        },
        end: {
            row: null,
            col: null
        },
    },
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
term.windowTitle('edit');

helper.renderScreen(state, term);

term.on('key', (key) => {
    if (state.vim && state.mode !== 'i') {
        handleVimKeys(key, state, term);
    } else {
        handleKeys(key, state, term);
    }
});

term.on('mouse', (name, coor) => {
    handleMouseInputs(name, coor, state, term);
});

term.on('resize', () => {
    helper.renderScreen(state, term);
});
