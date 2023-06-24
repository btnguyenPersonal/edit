import sinon from 'sinon';
import sinonChai from 'sinon-chai';
import chai, { expect } from 'chai';
import * as helper from '../../src/util/helper.js';

chai.use(sinonChai);

describe('helper functions', () => {
    let term, state;

    describe('createDiff', () => {
        beforeEach(() => {
            term = {
                moveTo: sinon.stub(),
                clear: sinon.stub(),
            };
            state = {
                data: [
                    'h',
                    'a',
                    't',
                    'c',
                    'a',
                    't',
                ],
                oldData: [
                    'h',
                    'a',
                    't',
                    'c',
                    'a',
                    't',
                ],
            };
        });
        afterEach(() => {
            sinon.restore();
        });
        it('should return two empty arrays when data is the same', () => {
            expect(helper.createDiff(state.oldData, state.data)).to.deep.equal([[], []]);
        });
        it('should return one added line when second is added', () => {
            state.data = [
                'h',
                'a',
                'a',
                't',
                'c',
                'a',
                't',
            ],
            expect(helper.createDiff(state.oldData, state.data)).to.deep.equal([
                [
                ],
                [
                    { l: 2 , s: 'a' }
                ]
            ]);
        });
        it('should return one removed line when second is removed', () => {
            state.data = [
                'h',
                't',
                'c',
                'a',
                't',
            ],
            expect(helper.createDiff(state.oldData, state.data)).to.deep.equal([
                [
                    { l: 1 , s: 'a' }
                ],
                [
                ]
            ]);
        });
        it('should return one added line when second is added', () => {
            state.data = [
                'h',
                'add',
                'a',
                't',
                'c',
                'a',
                't',
            ],
            expect(helper.createDiff(state.oldData, state.data)).to.deep.equal([
                [
                ],
                [
                    { l: 1 , s: 'add' }
                ]
            ]);
        });
        it('should return one added line to end when second to end line is added', () => {
            state.data = [
                'h',
                'a',
                't',
                'c',
                'a',
                'end',
                't',
            ],
            expect(helper.createDiff(state.oldData, state.data)).to.deep.equal([
                [
                ],
                [
                    { l: 5 , s: 'end' }
                ]
            ]);
        });
        it('should return one added line to end when end line is added', () => {
            state.data = [
                'h',
                'a',
                't',
                'c',
                'a',
                't',
                'end',
            ],
            expect(helper.createDiff(state.oldData, state.data)).to.deep.equal([
                [
                ],
                [
                    { l: 6 , s: 'end' }
                ]
            ]);
        });
        it('should return one added line when first line is added', () => {
            state.data = [
                'f',
                'h',
                'a',
                't',
                'c',
                'a',
                't',
            ],
            expect(helper.createDiff(state.oldData, state.data)).to.deep.equal([
                [
                ],
                [
                    { l: 0 , s: 'f' }
                ]
            ]);
        });
        it('should return one removed line when first line is removed', () => {
            state.data = [
                'a',
                't',
                'c',
                'a',
                't',
            ],
            expect(helper.createDiff(state.oldData, state.data)).to.deep.equal([
                [
                    { l: 0 , s: 'h' }
                ],
                [
                ]
            ]);
        });
    });

    describe('isAlphaNumeric', () => {
        it('should return true given an uppercase letter', () => {
            expect(helper.isAlphaNumeric('A')).to.equal(true);
            expect(helper.isAlphaNumeric('X')).to.equal(true);
            expect(helper.isAlphaNumeric('Y')).to.equal(true);
            expect(helper.isAlphaNumeric('Z')).to.equal(true);
        });
        it('should return true given a lowercase letter', () => {
            expect(helper.isAlphaNumeric('a')).to.equal(true);
            expect(helper.isAlphaNumeric('b')).to.equal(true);
            expect(helper.isAlphaNumeric('c')).to.equal(true);
            expect(helper.isAlphaNumeric('z')).to.equal(true);
        });
        it('should return true given a number', () => {
            expect(helper.isAlphaNumeric('0')).to.equal(true);
            expect(helper.isAlphaNumeric('1')).to.equal(true);
            expect(helper.isAlphaNumeric('2')).to.equal(true);
            expect(helper.isAlphaNumeric('3')).to.equal(true);
            expect(helper.isAlphaNumeric('4')).to.equal(true);
            expect(helper.isAlphaNumeric('5')).to.equal(true);
            expect(helper.isAlphaNumeric('6')).to.equal(true);
            expect(helper.isAlphaNumeric('7')).to.equal(true);
            expect(helper.isAlphaNumeric('8')).to.equal(true);
            expect(helper.isAlphaNumeric('9')).to.equal(true);
        });
        it('should return false given a forward or backslash', () => {
            expect(helper.isAlphaNumeric('/')).to.equal(false);
            expect(helper.isAlphaNumeric('\\')).to.equal(false);
        });
        it('should return false given a space', () => {
            expect(helper.isAlphaNumeric(' ')).to.equal(false);
        });
    });

    describe('isWritable', () => {
        it('should return true given an uppercase letter', () => {
            expect(helper.isWritable('A')).to.equal(true);
            expect(helper.isWritable('X')).to.equal(true);
            expect(helper.isWritable('Y')).to.equal(true);
            expect(helper.isWritable('Z')).to.equal(true);
        });
        it('should return true given a lowercase letter', () => {
            expect(helper.isWritable('a')).to.equal(true);
            expect(helper.isWritable('b')).to.equal(true);
            expect(helper.isWritable('c')).to.equal(true);
            expect(helper.isWritable('z')).to.equal(true);
        });
        it('should return true given a number', () => {
            expect(helper.isWritable('0')).to.equal(true);
            expect(helper.isWritable('1')).to.equal(true);
            expect(helper.isWritable('2')).to.equal(true);
            expect(helper.isWritable('3')).to.equal(true);
            expect(helper.isWritable('4')).to.equal(true);
            expect(helper.isWritable('5')).to.equal(true);
            expect(helper.isWritable('6')).to.equal(true);
            expect(helper.isWritable('7')).to.equal(true);
            expect(helper.isWritable('8')).to.equal(true);
            expect(helper.isWritable('9')).to.equal(true);
        });
        it('should return true given a space', () => {
            expect(helper.isWritable(' ')).to.equal(true);
        });
        it('should return false given CTRL keys', () => {
            expect(helper.isWritable('CTRL_A')).to.equal(false);
            expect(helper.isWritable('CTRL_S')).to.equal(false);
            expect(helper.isWritable('CTRL_W')).to.equal(false);
            expect(helper.isWritable('CTRL_X')).to.equal(false);
            expect(helper.isWritable('CTRL_Y')).to.equal(false);
            expect(helper.isWritable('CTRL_Z')).to.equal(false);
        });
    });

//     describe('isHighlighted', () => {
//         beforeEach(() => {
//             term = {
//                 moveTo: sinon.stub(),
//                 clear: sinon.stub(),
//             };
//             state = {
//                 data: [
//                     'bbbbbbbb',
//                     'aaaaaaaa',
//                     'bbbbbbbb',
//                     'aaaaaaaa',
//                     'bbbbbbbb',
//                     'aaaaaaaa',
//                     'bbbbbbbb',
//                     'aaaaaaaa',
//                 ],
//                 oldData: [
//                     'bbbbbbbb',
//                     'aaaaaaaa',
//                     'bbbbbbbb',
//                     'aaaaaaaa',
//                     'bbbbbbbb',
//                     'aaaaaaaa',
//                     'bbbbbbbb',
//                     'aaaaaaaa',
//                 ],
//                 row: 0,
//                 col: 0,
//                 windowLine: 0,
//                 visual: { row: undefined, col: undefined },
//             };
//         });
//         afterEach(() => {
//             sinon.restore();
//         });
//         it('should return false if cursor', () => {
//             state.visual = { row: 7, col: 7 };
//             state.mode = 'v';
//             expect(helper.isHighlighted(state, 0, 0)).to.equal(false);
//         });
//         it('should return false if cursor', () => {
//             state.visualLine = { row: 7 };
//             state.mode = 'V';
//             expect(helper.isHighlighted(state, 0, 0)).to.equal(false);
//         });
//         it('should return true given entire screen is highlighted with visual mode', () => {
//             state.visual = { row: 7, col: 7 };
//             state.mode = 'v';
//             expect(helper.isHighlighted(state, 1, 1)).to.equal(true);
//         });
//         it('should return true given entire screen is highlighted with visualLine mode', () => {
//             state.visualLine = { row: 7 };
//             state.mode = 'V';
//             expect(helper.isHighlighted(state, 1, 1)).to.equal(true);
//         });
//         it('should return true given visualLine is highlighted', () => {
//             state.visualLine = { row: 0 };
//             state.mode = 'V';
//             expect(helper.isHighlighted(state, 0, 1)).to.equal(true);
//         });
//         it('should return true given until end of line is highlighted', () => {
//             state.visual = { row: 0, col: 7 };
//             state.mode = 'v';
//             expect(helper.isHighlighted(state, 0, 1)).to.equal(true);
//         });
//         it('should return false given only cursor is highlighted', () => {
//             state.visual = { row: 0, col: 0 };
//             state.mode = 'v';
//             expect(helper.isHighlighted(state, 0, 0)).to.equal(false);
//         });
//         it('should return false given not in highlight at all', () => {
//             state.row = 2;
//             state.col = 5;
//             state.visual = { row: 3, col: 6 };
//             state.mode = 'v';
//             expect(helper.isHighlighted(state, 0, 0)).to.equal(false);
//         });
//         it('should return false given highlight is down and col is before top highlight', () => {
//             state.row = 2;
//             state.col = 5;
//             state.visual = { row: 3, col: 6 };
//             state.mode = 'v';
//             expect(helper.isHighlighted(state, 2, 1)).to.equal(false);
//         });
//         it('should return false given highlight is down and col is before top highlight', () => {
//             state.row = 3;
//             state.col = 6;
//             state.visual = { row: 2, col: 5 };
//             state.mode = 'v';
//             expect(helper.isHighlighted(state, 2, 1)).to.equal(false);
//         });
//         it('should return true given highlight is down and col is after top highlight', () => {
//             state.row = 3;
//             state.col = 6;
//             state.visual = { row: 2, col: 5 };
//             state.mode = 'v';
//             expect(helper.isHighlighted(state, 2, 6)).to.equal(true);
//         });
//         it('should return true given highlight is down and col is after top highlight', () => {
//             state.row = 2;
//             state.col = 5;
//             state.visual = { row: 3, col: 6 };
//             state.mode = 'v';
//             expect(helper.isHighlighted(state, 2, 6)).to.equal(true);
//         });
//         it('should return true given highlight is down and col is before bottom highlight', () => {
//             state.row = 2;
//             state.col = 5;
//             state.visual = { row: 3, col: 6 };
//             state.mode = 'v';
//             expect(helper.isHighlighted(state, 3, 1)).to.equal(true);
//         });
//         it('should return true given highlight is down and col is before bottom highlight', () => {
//             state.row = 3;
//             state.col = 6;
//             state.visual = { row: 2, col: 5 };
//             state.mode = 'v';
//             expect(helper.isHighlighted(state, 3, 1)).to.equal(true);
//         });
//         it('should return false given highlight is down and col is after bottom highlight', () => {
//             state.row = 3;
//             state.col = 6;
//             state.visual = { row: 2, col: 5 };
//             state.mode = 'v';
//             expect(helper.isHighlighted(state, 3, 6)).to.equal(false);
//         });
//         it('should return false given highlight is down and col is after bottom highlight', () => {
//             state.row = 2;
//             state.col = 5;
//             state.visual = { row: 3, col: 6 };
//             state.mode = 'v';
//             expect(helper.isHighlighted(state, 3, 6)).to.equal(true);
//         });
//     });
});
