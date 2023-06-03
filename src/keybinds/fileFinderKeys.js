/* eslint-disable import/no-cycle */
import fs from 'fs';
import { execSync } from 'child_process';
import {
    renderScreen,
    isWritable,
    changeFile,
} from '../util/helper.js';

function handleFileFinderKeys(key, state, screen) {
    if (isWritable(key)) {
        state.fileFinderQuery += key;
        state.fileFinderIndex = 0;
        if (state.mode === 'g') {
            let output = '';
            if (state.fileFinderQuery.length !== 0) {
                output = execSync(
                    `git grep -n ${state.fileFinderQuery}`,
                    { maxBuffer: 1024 * 1024 * 1000 }
                ).toString();
            }
            state.fileFindingOutput = output.split('\n');
        } else {
            let output = '';
            if (state.fileFinderQuery.length !== 0) {
                output = execSync(`fd -t f | grep ${state.fileFinderQuery} || true`).toString();
            } else {
                output = execSync('fd -t f').toString();
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
        if (state.fileFinderIndex < state.fileFindingOutput.length) {
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
        }
        if (newFile !== undefined) {
            let fileExists = fs.existsSync(newFile);
            if (!fileExists) {
                newFile += '.js';
                fileExists = fs.existsSync(newFile);
            }
            if (fileExists) {
                state.file = newFile;
                state.files.push(state.file);
                const snapshotsCopy = [];
                for (let i = 0; i < state.snapshots.length; i += 1) {
                    snapshotsCopy.push(JSON.parse(JSON.stringify(state.snapshots[i])));
                }
                state.storePosition.push({
                    row: state.row,
                    col: state.col,
                    windowLine: state.windowLine,
                    windowLineHorizontal: state.windowLineHorizontal,
                    currentSnapshot: state.currentSnapshot,
                    snapshots: snapshotsCopy,
                    savePoint: state.savePoint,
                    mark: state.mark,
                    prevRow: state.prevRow,
                    prevCol: state.prevCol,
                });
                state.fileIndex = state.files.length - 1;
                changeFile(state);
                if (lineNum !== 0) {
                    state.row = lineNum - 1;
                    state.col = 0;
                }
            }
        }
        state.mode = 'n';
        state.fileFinderIndex = 0;
    } else if (key === 'BACKSPACE') {
        if (state.fileFinderQuery.length > 0) {
            state.fileFinderQuery = state.fileFinderQuery.substring(0, state.fileFinderQuery.length - 1);
        }
        state.fileFinderIndex = 0;
        if (state.mode === 'g') {
            let output = '';
            if (state.fileFinderQuery.length !== 0) {
                output = execSync(`git grep -n ${state.fileFinderQuery}`, { maxBuffer: 1024 * 1024 * 1000 }).toString();
            }
            state.fileFindingOutput = output.split('\n');
        } else {
            let output = '';
            if (state.fileFinderQuery.length !== 0) {
                output = execSync(`fd -t f | grep ${state.fileFinderQuery} || true`).toString();
            } else {
                output = execSync('fd -t f').toString();
            }
            state.fileFindingOutput = output.split('\n');
        }
    }
    renderScreen(state, screen);
}

export {
    handleFileFinderKeys
};
