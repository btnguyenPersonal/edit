import sinon from 'sinon';
import sinonChai from 'sinon-chai';
import chai, { expect } from 'chai';
import { handleMouseInputs } from '../../src/keybinds/mouse.js';

chai.use(sinonChai);

describe('mouse input', () => {

    let term, state;

    describe('handleMouseInput', () => {
        beforeEach(() => {
            term = () => ({
            });
            term.moveTo = sinon.stub();
            term.clear = sinon.stub();
            state = {
                data: [
                    'aaaaaaaa',
                    'aaaaaaaa',
                    'aaaaaaaa',
                    'aaaaaaaa',
                    'aaaaaaaa',
                    'aaaaaaaa',
                    'aaaaaaaa',
                    'aaaaaaaa',
                ],
                row: 0,
                col: 0,
                windowLine: 0
            };
        });
        afterEach(() => {
            sinon.restore();
        });
        it('when empty state, should move row and col to 0, 0 when clicked on 10, 10', () => {
            state.data = [''];
            handleMouseInputs('MOUSE_LEFT_BUTTON_PRESSED', {x: 10, y: 10}, state, term);
            expect(state.col).to.equal(0);
            expect(state.row).to.equal(0);
        });
        it('when empty state, should move row and col to 0, 0 when clicked on 1, 10', () => {
            state.data = [''];
            handleMouseInputs('MOUSE_LEFT_BUTTON_PRESSED', {x: 1, y: 10}, state, term);
            expect(state.col).to.equal(0);
            expect(state.row).to.equal(0);
        });
        it('when empty state, should move row and col to 0, 0 when clicked on 10, 1', () => {
            state.data = [''];
            handleMouseInputs('MOUSE_LEFT_BUTTON_PRESSED', {x: 10, y: 1}, state, term);
            expect(state.col).to.equal(0);
            expect(state.row).to.equal(0);
        });
        it('should move row and col to 6, 2 when clicked on 11, 3', () => {
            handleMouseInputs('MOUSE_LEFT_BUTTON_PRESSED', {x: 11, y: 3}, state, term);
            expect(state.col).to.equal(6);
            expect(state.row).to.equal(2);
        });
        it('should move row and col to 7, 8 when clicked on 100, 100', () => {
            handleMouseInputs('MOUSE_LEFT_BUTTON_PRESSED', {x: 100, y: 100}, state, term);
            expect(state.col).to.equal(8);
            expect(state.row).to.equal(7);
        });
        it('should move row and col to 7, 0 when clicked on 1, 100', () => {
            handleMouseInputs('MOUSE_LEFT_BUTTON_PRESSED', {x: 1, y: 100}, state, term);
            expect(state.col).to.equal(0);
            expect(state.row).to.equal(7);
        });
        it('should move row and col to 0, 8 when clicked on 100, 1', () => {
            handleMouseInputs('MOUSE_LEFT_BUTTON_PRESSED', {x: 100, y: 1}, state, term);
            expect(state.col).to.equal(8);
            expect(state.row).to.equal(0);
        });
        it('should move row and col to 1, 1 when clicked on 6, 2', () => {
            handleMouseInputs('MOUSE_LEFT_BUTTON_PRESSED', {x: 6, y: 2}, state, term);
            expect(state.col).to.equal(1);
            expect(state.row).to.equal(1);
        });
        it('should move row and col to 0, 0 when clicked on 5, 1', () => {
            handleMouseInputs('MOUSE_LEFT_BUTTON_PRESSED', {x: 5, y: 1}, state, term);
            expect(state.col).to.equal(0);
            expect(state.row).to.equal(0);
        });
        it('should move row and col to 0, 0 when clicked on 5, 1', () => {
            handleMouseInputs('MOUSE_LEFT_BUTTON_PRESSED', {x: 5, y: 1}, state, term);
            expect(state.col).to.equal(0);
            expect(state.row).to.equal(0);
        });
        it('should move windowLine up when mouse wheel is moved up depending on terminal size', () => { //depends on terminal size lmao
            state.windowLine = 0;
            handleMouseInputs('MOUSE_WHEEL_UP', undefined, state, term);
            if (process.stdout.rows < 8) {
                expect(state.windowLine).to.equal(1);
            } else {
                expect(state.windowLine).to.equal(0);
            }
        });
        it('should not move windowLine up when mouse wheel is moved up and windowLine is max length', () => {
            state.windowLine = 7;
            handleMouseInputs('MOUSE_WHEEL_UP', undefined, state, term);
            expect(state.windowLine).to.equal(7);
        });
        it('should move windowLine down when mouse wheel is moved down', () => {
            state.windowLine = 5;
            handleMouseInputs('MOUSE_WHEEL_DOWN', undefined, state, term);
            expect(state.windowLine).to.equal(4);
        });
        it('should not move windowLine down when mouse wheel is moved down and windowLine is zero', () => {
            state.windowLine = 0;
            handleMouseInputs('MOUSE_WHEEL_DOWN', undefined, state, term);
            expect(state.windowLine).to.equal(0);
        });
    });

});
