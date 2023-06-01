/* eslint-disable import/no-cycle */
import fs from 'fs';
import {
    renderScreen,
    isWritable,
    searchForString,
    createSnapshot,
    changeFile,
    centerScreen
} from '../util/helper.js';

function handleFileFinderKeys(key, state, screen) {
    if (isWritable(key)) {
        state.fileFinderQuery += key;
    } else if (key === 'UP') {
        if (state.fileFinderIndex > 0) {
            state.fileFinderIndex -= 1;
        } else {
            state.fileFinderIndex = state.fileFindingOutput.length - 1;
        }
    } else if (key === 'DOWN') {
        state.fileFinderIndex += 1;
    } else if (key === 'ESCAPE') {
        state.fileFinderQuery = '';
        state.mode = 'n';
    } else if (key === 'ENTER') {
        state.fileFinderQuery = '';
        state.mode = 'n';
        if (state.fileFindingOutput[state.fileFinderIndex] !== undefined) {
            let convertedPath = state.fileFindingOutput[state.fileFinderIndex];
            let fileExists = fs.existsSync(convertedPath);
            if (!fileExists) {
                convertedPath += '.js';
                fileExists = fs.existsSync(convertedPath);
            }
            if (fileExists) {
                state.file = convertedPath;
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
                state.fileIndex += 1;
                changeFile(state);
            }
            state.mode = 'n';
        }
    } else if (key === 'BACKSPACE') {
        if (state.fileFinderQuery.length > 0) {
            state.fileFinderQuery = state.fileFinderQuery.substring(0, state.fileFinderQuery.length - 1);
        }
    }
    renderScreen(state, screen);
}

export {
    handleFileFinderKeys
};
