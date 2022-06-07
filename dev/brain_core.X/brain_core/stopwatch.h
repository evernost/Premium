// =============================================================================
// File         : stopwatch.h
// Date         : 29/09/2018
// Author       : Quentin
// Version      : 01
// Description  : Header file for the stopwatch library
// =============================================================================

#ifndef STOPWATCH_H
#define	STOPWATCH_H

// -------
// Defines
// -------
#define START_STOPWATCH     T4CONbits.TON = 1
#define FREEZE_STOPWATCH    T4CONbits.TON = 0
#define STOPWATCH_VALUE		TMR4

// -----------------
// Types declaration
// -----------------

// ---------------------
// Functions declaration
// ---------------------
void init_stopwatch(void);

#endif