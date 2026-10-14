/* SPDX-License-Identifier: MIT */
#pragma once
/*
 * PROVISIONAL: Index assignments below are only an electrical-chain GUESS.
 * Confirm D64..D75 with PCB layout or a software identification test before
 * claiming exact key-to-LED correspondence.
 *
 * Physical 60% ANSI keyboard: 61 ZMK positions, rows 14/14/13/12/8.
 * The 12 LEDs appear to be 5 upper, 5 lower, and one at each end.
 * Chain indices here are assigned clockwise from the UPPER LEFT as a
 * starting hypothesis, NOT an observation of the copper traces.
 *
 *  top left to right: 0,1,2,3,4
 *  middle right:      5
 *  bottom right to left: 6,7,8,9,10
 *  middle left:       11
 */

static const unsigned char nice60_region_for_position[61] = {
    /* ESC..Backspace : top row (14) */
     0,0,0, 1,1,1, 2,2, 3,3,3, 4,4,4,
    /* Tab..Backslash : 14 */
    11,0,0, 1,1,1, 2,2, 3,3, 4,4,4,5,
    /* Caps..Enter : 13 */
    11,11,10, 9,9, 8,8, 7,7, 6,6, 5,5,
    /* Shift..Shift : 12 */
    10,10,10, 9,9, 8,8, 7,7, 6,6,6,
    /* Bottom row : 8 */
    10,10,9, 8,8, 7,6,6,
};
