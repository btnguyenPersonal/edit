#!/usr/bin/env node
import { execSync } from 'child_process';
import pkg from 'terminal-kit';
import path from 'path';
import os from 'os';
import fs from 'fs';
import {
    SHORTCUTS,
    FILEFINDER,
} from './util/modes.js';
import {
    centerScreen,
    renderScreen,
    createSnapshot,
    calcFileFinderOutput,
    setFileSearchOutput,
    getData
} from './util/helper.js';
import { sendKeys } from './util/sendKeys.js';
import { handleMouseInputs } from './keybinds/mouse.js';

const { terminal, ScreenBuffer } = pkg;

function getFile() {
    return process.argv[2];
}

let isInGit = false;
// eslint-disable-next-line no-empty
try { isInGit = execSync('git rev-parse --is-inside-work-tree').toString().includes('true'); } catch (err) {}

const term = terminal();
const filePath = getFile();
const state = {
    allowCommandLogging: false,
    mode: filePath === undefined ? FILEFINDER : SHORTCUTS,
    replace: {
        query: '',
        replacing: false,
    },
    fileStack: {
        stack: [],
        index: -1,
    },
    fileFinder: {
        query: '',
        cursorPosition: 0,
        index: 0,
        fileCache: '',
        output: [],
    },
    grep: {
        query: '',
        cursorPosition: 0,
        index: 0,
        output: [],
    },
    search: {
        row: undefined,
        col: undefined,
        query: '',
        searching: false,
        reverse: false,
    },
    visual: {
        row: undefined,
        col: undefined
    },
    lineNumber: '',
    log: {
        prevKeys: '',
        history: '',
        totalHistory: '',
    },
    command: {
        current: '',
        cursorPosition: 0,
        history: [],
        historyPosition: 0,
    },
    lastSearchCommand: [],
    dot: [],
    file: {
        current: filePath === undefined ? '' : filePath,
        all: [],
        index: 0,
    },
    harpoon: {
        index: 0,
        files: [],
    },
    storePosition: [],
    data: filePath === undefined ? [] : getData(filePath),
    row: 0,
    col: 0,
    gitFinding: isInGit,
    prevRow: 0,
    prevCol: 0,
    window: {
        vertical: 0,
        horizontal: 0,
    },
    snapshots: {
        array: [],
        current: 0,
    },
    recording: false,
    macro: [],
    mark: -1,
    mark2: -1,
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
    state.fileStack.stack.push(state.file);
    state.fileStack.index += 1;
} else {
    if (state.gitFinding) {
        setFileSearchOutput(state);
        calcFileFinderOutput(state);
    } else {
        term.fullscreen(false);
        console.log('TODO: implement searching for non-git directories that doesn\'t break');
        process.exit(0);
    }
}
state.allowCommandLogging = true;
renderScreen(state, screen);

term.on('key', (key) => {
    try {
        sendKeys([key], state, screen, term);
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
