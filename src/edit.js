#!/usr/bin/env node
import fs from 'fs';
import pkg from 'terminal-kit';
import {
    saveFile,
    centerScreen,
    renderScreen,
    createSnapshot
} from './util/helper.js';
import { sendKeys } from './util/sendKeys.js';
import { handleMouseInputs } from './keybinds/mouse.js';

const { terminal, ScreenBuffer } = pkg;

function getFile() {
    try {
        const file = process.argv[2] === '-n' ? process.argv[3] : process.argv[2];
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
        console.log(filepath + ' not found\nusage: edit [file]');
        process.exit();
    }
}

const term = terminal();
const filepath = getFile();
process.title = filepath;
const state = {
    allowCommandLogging: false,
    vim: process.argv[2] !== '-n',
    mode: 'n',
    clipboard: [],
    clipboardNewLine: true,
    searchQuery: '',
    searching: false,
    loadFileOkay: true,
    search: {
        row: null,
        col: null
    },
    visualLine: {
        row: null
    },
    visual: {
        row: null,
        col: null
    },
    lineNumber: '',
    previousKeys: '',
    previousCommand: [],
    file: filepath,
    data: getData(filepath),
    row: Number.isInteger(parseInt(process.argv[3])) ? parseInt(process.argv[3]) - 1 : 0,
    col: 0,
    prevRow: Number.isInteger(parseInt(process.argv[3])) ? parseInt(process.argv[3]) - 1 : 0,
    prevCol: 0,
    windowLine: 0,
    windowLineHorizontal: 0,
    snapshots: [],
    currentSnapshot: 0,
    savePoint: 0,
    recording: false,
    macro: [],
    mark: -1
};

term.grabInput({ mouse: 'button' });
term.fullscreen(true);
term.windowTitle('edit');
const screen = new ScreenBuffer({ dst: term, noFill: true });
centerScreen(state);
createSnapshot(state);
state.allowCommandLogging = true;
renderScreen(state, screen);

term.on('key', (key) => {
    try {
        if (key === 'CTRL_S') {
            saveFile(state);
            renderScreen(state, screen);
        } else if (key === 'CTRL_G') {
            term.fullscreen(false);
            process.exit(2);
        } else if (key === 'CTRL_P') {
            term.fullscreen(false);
            process.exit(1);
        } else if (key === 'CTRL_C') {
            term.fullscreen(false);
            process.exit(0);
        } else {
            sendKeys([key], state, screen, term);
        }
    } catch (e) {
        term.fullscreen(false);
        console.log(e);
        process.exit(0);
    }
});

term.on('mouse', (name, coor) => {
    try {
        handleMouseInputs(name, coor, state, screen);
    } catch (e) {
        term.fullscreen(false);
        console.log(e);
        process.exit(0);
    }
});

term.on('resize', () => {
    try {
        renderScreen(state, screen);
    } catch (e) {
        term.fullscreen(false);
        console.log(e);
        process.exit(0);
    }
});

const loadTime = new Date().getTime();
fs.watch(state.file, () => {
    fs.stat(state.file, (err, stats) => {
        if (err) {
            return;
        }
        if (stats.mtime.getTime() > loadTime && state.loadFileOkay) {
            const newData = getData(state.file);
            state.data = [];
            for (let i = 0; i < newData.length; i += 1) {
                state.data[i] = newData[i];
            }
            renderScreen(state, screen);
        }
    });
});
