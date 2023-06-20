/* eslint-disable import/no-cycle */
import fs from 'fs';
import { execSync } from 'child_process';
import {
    renderScreen,
    isWritable,
    changeFile,
} from '../util/helper.ts';

function handleFileFinderKeys(key, state, screen) {
    if (isWritable(key) && key !== '\\') {
        if (key === '"') {
            state.fileFinderQuery += '\\';
        }
        state.fileFinderQuery += key;
        state.fileFinderIndex = 0;
        if (state.mode === 'g') {
            let output = '';
            if (state.fileFinderQuery.length !== 0) {
                output = execSync(
                    `git grep -n "${state.fileFinderQuery}" || true`,
                    { maxBuffer: 1024 * 1024 * 1000 }
                ).toString();
            }
            state.fileFindingOutput = output.split('\n');
        } else {
            let output = '';
            if (state.gitFinding) {
                if (state.fileFinderQuery.length !== 0) {
                    output = execSync(`fd -t f --hidden -E .git | grep -F -i "${state.fileFinderQuery}" || true`).toString();
                } else {
                    output = execSync('fd -t f --hidden -E .git').toString();
                }
            } else {
                if (state.fileFinderQuery.length !== 0) {
                    output = execSync(`find * -type f -name "${state.fileFinderQuery}*"`).toString();
                }
            }
            state.fileFindingOutput = output.split('\n');
        }
    } else if (key === 'UP') {
        if (state.fileFinderIndex > 0) {
            state.fileFinderIndex -= 1;
        } else {
            state.fileFinderIndex = state.fileFindingOutput.length - 1;
        }
    } else if (key === 'DOWN') {
        if (state.fileFinderIndex < state.fileFindingOutput.length - 2) {
            state.fileFinderIndex += 1;
        }
    } else if (key === 'ESCAPE') {
        if (state.mode === 'f') {
            state.fileFinderQuery = '';
        }
        state.mode = 'n';
        state.fileFinderIndex = 0;
    } else if (key === 'ENTER') {
        let newFile = state.fileFindingOutput[state.fileFinderIndex];
        let lineNum = 0;
        if (state.mode === 'g') {
            const arrayForm = newFile.split(':');
            newFile = arrayForm[0];
            lineNum = parseInt(arrayForm[1], 10);
        } else {
            state.fileFinderQuery = '';
            state.fileFinderIndex = 0;
        }
        if (newFile !== undefined) {
            let fileExists = fs.existsSync(newFile);
            if (!fileExists) {
                newFile += '.js';
                fileExists = fs.existsSync(newFile);
            }
            if (fileExists) {
                if (!state.files.includes(newFile)) {
                    state.file = newFile;
                    state.files.push(state.file);
                    const snapshotsCopy = [];
                    for (let i = 0; i < state.snapshots.length; i += 1) {
                        snapshotsCopy.push(JSON.parse(JSON.stringify(state.snapshots[i])));
                    }
                    if (state.files.includes(state.file)) {
                        state.storePosition[state.fileIndex] = {
                            row: state.row,
                            col: state.col,
                            windowLine: state.windowLine,
                            windowLineHorizontal: state.windowLineHorizontal,
                            snapshots: snapshotsCopy,
                            mark: state.mark,
                            prevRow: state.prevRow,
                            prevCol: state.prevCol,
                        };
                    } else {
                        state.storePosition.push({
                            row: state.row,
                            col: state.col,
                            windowLine: state.windowLine,
                            windowLineHorizontal: state.windowLineHorizontal,
                            currentSnapshot: state.currentSnapshot,
                            snapshots: snapshotsCopy,
                            mark: state.mark,
                            prevRow: state.prevRow,
                            prevCol: state.prevCol,
                        });
                    }
                    state.fileIndex = state.files.length - 1;
                    changeFile(state);
                    if (lineNum !== 0) {
                        state.row = lineNum - 1;
                        state.col = 0;
                    }
                } else {
                    const snapshotsCopy = [];
                    for (let i = 0; i < state.snapshots.length; i += 1) {
                        snapshotsCopy.push(JSON.parse(JSON.stringify(state.snapshots[i])));
                    }
                    if (state.files.includes(state.file)) {
                        state.storePosition[state.fileIndex] = {
                            row: state.row,
                            col: state.col,
                            windowLine: state.windowLine,
                            windowLineHorizontal: state.windowLineHorizontal,
                            snapshots: snapshotsCopy,
                            mark: state.mark,
                            prevRow: state.prevRow,
                            prevCol: state.prevCol,
                        };
                    } else {
                        state.storePosition.push({
                            row: state.row,
                            col: state.col,
                            windowLine: state.windowLine,
                            windowLineHorizontal: state.windowLineHorizontal,
                            currentSnapshot: state.currentSnapshot,
                            snapshots: snapshotsCopy,
                            mark: state.mark,
                            prevRow: state.prevRow,
                            prevCol: state.prevCol,
                        });
                    }
                    state.file = newFile;
                    state.fileIndex = state.files.indexOf(state.file);
                    changeFile(state);
                    const pos = state.storePosition[state.fileIndex];
                    state.row = pos.row;
                    state.col = pos.col;
                    state.windowLine = pos.windowLine;
                    state.windowLineHorizontal = pos.windowLineHorizontal;
                    state.currentSnapshot = pos.currentSnapshot;
                    state.snapshots = pos.snapshots;
                    state.mark = pos.mark;
                    state.prevRow = pos.prevRow;
                    state.prevCol = pos.prevCol;
                    if (lineNum !== 0) {
                        state.row = lineNum - 1;
                        state.col = 0;
                    }
                }
            }
        }
        state.mode = 'n';
    } else if (key === 'BACKSPACE') {
        if (state.fileFinderQuery.length > 0) {
            if (state.fileFinderQuery.endsWith('"')) {
                state.fileFinderQuery = state.fileFinderQuery.substring(0, state.fileFinderQuery.length - 2);
            } else {
                state.fileFinderQuery = state.fileFinderQuery.substring(0, state.fileFinderQuery.length - 1);
            }
        }
        state.fileFinderIndex = 0;
        if (state.mode === 'g') {
            let output = '';
            if (state.fileFinderQuery.length !== 0) {
                output = execSync(`git grep -n "${state.fileFinderQuery}" || true`, { maxBuffer: 1024 * 1024 * 1000 }).toString();
            }
            state.fileFindingOutput = output.split('\n');
        } else {
            let output = '';
            if (state.gitFinding) {
                if (state.fileFinderQuery.length !== 0) {
                    output = execSync(`fd -t f --hidden -E .git | grep -F -i "${state.fileFinderQuery}" || true`).toString();
                } else {
                    output = execSync('fd -t f --hidden -E .git').toString();
                }
            } else {
                if (state.fileFinderQuery.length !== 0) {
                    output = execSync(`find * -type f -name "${state.fileFinderQuery}*"`).toString();
                }
            }
            state.fileFindingOutput = output.split('\n');
        }
    }
    renderScreen(state, screen);
}

export {
    handleFileFinderKeys
};
