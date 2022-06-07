// =============================================================================
// File         : board.h
// Date         : 31/08/2017
// Author       : Quentin
// Version      : 01
// Description  : Header file containing the settings of the board.
// =============================================================================

#ifndef BOARD_H
#define	BOARD_H

// -----------------
// Types declaration
// -----------------


// -------
// Defines
// -------
// LEDs
#define GREEN_LED           LATFbits.LATF0
#define RED_LED             LATFbits.LATF1

// Keypad
#define COLUMN_VECTOR       PORTD
#define ROW_VECTOR          LATG
#define KEY_UP              12
#define KEY_DOWN            10

// ---------------------
// Functions declaration
// ---------------------

#endif

