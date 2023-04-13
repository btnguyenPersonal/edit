#!/usr/bin/env node
import fs from 'fs';
import pkg from 'terminal-kit';
const { terminal, ScreenBuffer } = pkg;
import * as helper from './util/helper.js';
import { handleKeys } from './keybinds/normalKeys.js';
import { handleVimKeys } from './keybinds/vimKeys.js';
import { handleMouseInputs } from './keybinds/mouse.js';

function getFile() {
    try {
        const file = process.argv[2] === '-v' ? process.argv[3] : process.argv[2];
        if (file === undefined) {
            throw new Error('input file not found\nusage: edit [file]');
        }
        return file;
    } catch (e) {
        console.log('input file not found\nusage: edit [file]');
        process.exit();
    }
}

function getData(filepath) {
    try {
        return (fs.readFileSync(filepath, 'utf-8')).split('\n');
    } catch (e) {
        console.log('input file not found\nusage: edit [file]');
        process.exit();
    }
}

process.title = 'edit';
const term = terminal();
const filepath = getFile();
const state = {
    vim: process.argv[2] === '-v',
    mode: 'n',
    clipboard: [],
    clipboardNewLine: false,
    visual: {
        row: null,
        col: null
    },
    previousKeys: '',
    file: filepath,
    data: getData(filepath),
    row: 0,
    col: 0,
    windowLine: 0,
};

term.grabInput({ mouse: 'button' });
term.fullscreen(true);
term.windowTitle('edit');

var screen = new ScreenBuffer( { dst: term , noFill: true } ) ;

helper.renderScreen(state, screen);

term.on('key', (key) => {
    if (state.vim && state.mode !== 'i') {
        handleVimKeys(key, state, screen, term);
    } else {
        handleKeys(key, state, screen, term);
    }
});

term.on('mouse', (name, coor) => {
    handleMouseInputs(name, coor, state, screen);
});

term.on('resize', () => {
    helper.renderScreen(state, screen);
});
