import * as helper from '../util/helper.js';

function handleMouseInputs(key, coor, state, screen) {
    if (key === 'MOUSE_LEFT_BUTTON_PRESSED') {
        if (state.mode !== 'i') {
            if (coor.y - 1 < 0 || coor.x - 1 < 0) {
                state.col = 0;
                state.row = 0;
                helper.renderScreen(state, screen);
                return;
            }
            const adjustedRow = coor.y - 1 + state.windowLine - 1;
            const adjustedCol = coor.x - 6;
            if (state.data[adjustedRow] && adjustedCol >= 0) {
                const rowDataLength = state.data[adjustedRow].length;
                state.col = Math.max(0, Math.min(adjustedCol, rowDataLength));
            } else {
                state.col = 0;
            }
            state.row = Math.max(0, Math.min(adjustedRow, state.data.length - 1));
            helper.renderScreen(state, screen);
        }
    } else if (key === 'MOUSE_WHEEL_UP') {
        if (state.windowLine > 0) {
            state.windowLine -= 1;
        }
    } else if (key === 'MOUSE_WHEEL_DOWN') {
        if (state.windowLine <= state.data.length) {
            state.windowLine += 1;
        }
    }
    helper.renderScreen(state, screen, true);
}

export {
    handleMouseInputs
};
