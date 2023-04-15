#!/usr/bin/env node
import fs from 'fs';
import pkg from 'terminal-kit';
import * as helper from './util/helper.js';
import { handleMouseInputs } from './keybinds/mouse.js';

const { terminal, ScreenBuffer } = pkg;

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

const term = terminal();
const filepath = getFile();
process.title = filepath;
const state = {
    allowCommandLogging: true,
    vim: process.argv[2] === '-v',
    mode: 'n',
    clipboard: [],
    clipboardNewLine: false,
    visual: {
        row: null,
        col: null
    },
    previousKeys: '',
    previousCommand: [],
    file: filepath,
    data: getData(filepath),
    row: 0,
    col: 0,
    windowLine: 0,
    snapshots: [],
    currentSnapshot: 0,
    isSaved: true
};
const screen = new ScreenBuffer({ dst: term, noFill: true });

term.grabInput({ mouse: 'button' });
term.fullscreen(true);
term.windowTitle('edit');
helper.renderScreen(state, screen);
helper.createSnapshot(state);
state.isSaved = true;

term.on('key', (key) => {
    try {
        if (key === 'CTRL_S') {
            helper.saveFile(state, term);
            helper.renderScreen(state, screen);
        } else if (key === 'CTRL_C') {
            term.fullscreen(false);
            process.exit();
        } else {
            helper.sendKeys([key], state, screen, term);
        }
    } catch (e) {
        term.fullscreen(false);
        console.log(e);
        process.exit();
    }
});

term.on('mouse', (name, coor) => {
    try {
        handleMouseInputs(name, coor, state, screen);
    } catch (e) {
        term.fullscreen(false);
        console.log(e);
        process.exit();
    }
});

term.on('resize', () => {
    try {
        helper.renderScreen(state, screen);
    } catch (e) {
        term.fullscreen(false);
        console.log(e);
        process.exit();
    }
});
