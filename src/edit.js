#!/usr/bin/env node
import pkg from 'terminal-kit';
import path from 'path';
import os from 'os';
import fs from 'fs';
import {
    FILEFINDER,
    SHORTCUTS,
} from './util/modes.js';
import {
    calcFileFinderOutput,
    centerScreen,
    createSnapshot,
    isInGit,
    getData,
    renderScreen,
    setFileSearchOutput,
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
    allowCommandLogging: true,
    clipboardVisualBlock: false,
    col: 0,
    commandCursorPosition: 0,
    commandHistory: '',
    currentCommand: '',
    currentSnapshot: 0,
    data: filePath === undefined ? [] : getData(filePath),
    file: filePath === undefined ? '' : filePath,
    fileExplorerCopyOutput: [],
    fileExplorerOutput: [],
    fileExplorerIndex: 0,
    fileFinderCursorPosition: 0,
    fileFinderFileCache: '',
    fileFinderIndex: 0,
    fileFinderOutput: [],
    fileFinderQuery: '',
    fileIndex: 0,
    fileStack: [],
    fileStackIndex: -1,
    files: [],
    git: isInGit(),
    grepCursorPosition: 0,
    grepIndex: 0,
    grepQuery: '',
    harpoonIndex: 0,
    harpoonIndexes: [],
    historyPosition: 0,
    indentAmount: 4,
    lastSearchCommand: [],
    lineNumber: '',
    macro: [],
    mark2: -1,
    mark: -1,
    mode: filePath === undefined ? FILEFINDER : SHORTCUTS,
    prevCol: 0,
    prevRow: 0,
    previousCommand: [],
    previousCommands: [],
    previousKeys: '',
    recording: false,
    replaceQuery: '',
    replacing: false,
    reverseSearch: false,
    row: 0,
    search: {
        row: undefined,
        col: undefined,
    },
    searchQuery: '',
    searching: false,
    selectedFile: '',
    selectedFolder: '',
    newFile: '',
    newFileIndex: 0,
    typing: false,
    renamingFile: false,
    copyingFile: false,
    creatingFile: false,
    skipSortingFileFinder: false,
    selectedFileExplorerIndex: -1,
    snapshots: [],
    storePosition: [],
    totalCommandHistory: '',
    visual: {
        row: undefined,
        col: undefined,
    },
    windowLine: 0,
    windowLineHorizontal: 0,
    copiedFileName: '',
    status: '',
};

term.grabInput({ mouse: 'button' });
term.fullscreen(true);
const screen = new ScreenBuffer({ dst: term, noFill: true });
if (filePath !== undefined) {
    centerScreen(state);
    createSnapshot(state);
    state.files.push(state.file);
    state.fileStack.push(state.file);
    state.fileStackIndex += 1;
} else {
    if (state.git) {
        setFileSearchOutput(state, true);
        calcFileFinderOutput(state);
    } else {
        term.fullscreen(false);
        console.log('TODO: implement searching for non-git directories that doesn\'t break');
        process.exit(0);
    }
}
renderScreen(state, screen);

term.on('key', (key) => {
    state.status = '';
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
