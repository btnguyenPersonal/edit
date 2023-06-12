#!/usr/bin/env node
import { execSync } from 'child_process';
import pkg from 'terminal-kit';
import path from 'path';
import os from 'os';
import fs from 'fs';
import {
    saveFile,
    centerScreen,
    renderScreen,
    createSnapshot,
    getData
} from './util/helper.js';
import { sendKeys } from './util/sendKeys.js';
import { handleMouseInputs } from './keybinds/mouse.js';

const { terminal, ScreenBuffer } = pkg;

function getFile() {
    return process.argv[2];
}

const term = terminal();
const filePath = getFile();
const state = {
    allowCommandLogging: false,
    mode: filePath === undefined ? 'f' : 'n',
    clipboardVisualBlock: false,
    searchQuery: '',
    searching: false,
    replaceQuery: '',
    replacing: false,
    fileFinderQuery: '',
    fileFinderIndex: 0,
    fileFindingOutput: [],
    search: {
        row: undefined,
        col: undefined
    },
    visualBlock: {
        row: undefined,
        col: undefined
    },
    visualLine: {
        row: undefined
    },
    visual: {
        row: undefined,
        col: undefined
    },
    lineNumber: '',
    previousKeys: '',
    commandHistory: '',
    commandString: '',
    commandIndex: 0,
    commands: [],
    commandsIndex: [],
    previousCommand: [],
    file: filePath === undefined ? '' : filePath,
    harpoonIndex: 0,
    harpoonIndexes: [],
    files: [],
    storePosition: [],
    fileIndex: 0,
    data: filePath === undefined ? [] : getData(filePath),
    row: 0,
    col: 0,
    gitFinding: execSync('git rev-parse --is-inside-work-tree').toString().includes('true'),
    prevRow: 0,
    prevCol: 0,
    windowLine: 0,
    windowLineHorizontal: 0,
    snapshots: [],
    currentSnapshot: 0,
    recording: false,
    macro: [],
    mark: 0,
    indentAmount: 4
};

term.grabInput({ mouse: 'button' });
term.fullscreen(true);
const screen = new ScreenBuffer({ dst: term, noFill: true });
if (filePath !== undefined) {
    centerScreen(state);
    createSnapshot(state);
    state.allowCommandLogging = true;
    state.files.push(state.file);
} else {
    if (state.gitFinding) {
        state.fileFindingOutput = execSync('fd -t f --hidden -E .git').toString().split('\n');
    } else {
        state.fileFindingOutput = execSync('find * -type f').toString().split('\n');
    }
}
state.allowCommandLogging = true;
renderScreen(state, screen);

term.on('key', (key) => {
    try {
        if (key === 'CTRL_S') {
            saveFile(state);
            renderScreen(state, screen);
        } else if (key === 'END') {
            console.log('row: ' + state.row);
            console.log('col: ' + state.col);
            console.log('previousCommand: ' + state.previousCommand);
            console.log('previousKeys: ' + state.previousKeys);
            console.log('file: ' + state.file);
            console.log('files: ' + state.files);
            console.log('fileIndex: ' + state.fileIndex);
            console.log('windowLine: ' + state.windowLine);
            console.log('windowLineHorizontal: ' + state.windowLineHorizontal);
            console.log('currentSnapshot: ' + state.currentSnapshot);
            console.log('mark: ' + state.mark);
        } else if (key === 'HOME') {
            console.log(process.argv);
        } else {
            sendKeys([key], state, screen, term);
        }
    } catch (e) {
        term.fullscreen(false);
        console.log(e);
        console.log('crash report logged at ~/.edit_crash_report');
        fs.writeFileSync(path.join(os.homedir(), '.edit_crash_report'), JSON.stringify({ key, ...state }), () => {});
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
