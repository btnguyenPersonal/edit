import {
    TYPING
} from '../util/modes.js';
import { render } from '../util/render.js';

function handleMouseInputs(key, coor, state, screen) {
    if (key === 'MOUSE_LEFT_BUTTON_PRESSED') {
        if (state.mode !== TYPING) {
            if (coor.y - 1 < 0 || coor.x - 1 < 0) {
                state.col = 0;
                state.row = 0;
                render(state, screen);
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
            render(state, screen);
        }
    } else if (key === 'MOUSE_WHEEL_UP') {
        state.totalCommandHistory += '<w-u>';
        for (let i = 0; i < 5; i += 1) {
            if (state.windowLine > 0) {
                state.windowLine -= 1;
            }
        }
    } else if (key === 'MOUSE_WHEEL_DOWN') {
        state.totalCommandHistory += '<w-d>';
        for (let i = 0; i < 5; i += 1) {
            if (state.windowLine < state.data.length - 1) {
                state.windowLine += 1;
            }
        }
    }
    render(state, screen, true);
}

export {
    handleMouseInputs
};
