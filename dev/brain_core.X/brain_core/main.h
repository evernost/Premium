// =============================================================================
// File         : main.h
// Date         : 31/08/2017
// Author       : Quentin
// Version      : 01
// Description  : Header file for the main
// =============================================================================

#ifndef MAIN_H
#define	MAIN_H

// -----------------
// Types declaration
// -----------------


// -------
// Defines
// -------
// Display
#define SEVEN_SEG_LUT_SIZE  91
#define TEMP_LUT_SIZE       580
#define ASCII_LUT_OFFSET    32
#define BLANKING_EN         1
#define BLANKING_DIS        0

// FSM states
#define FSM_INIT            0
#define FSM_MENU            1
#define FSM_TYPE_NUMBER     2
#define FSM_FACTOR          3
#define SHOW_PRIMALITY  	4
#define FSM_FACTOR_EXPLORE  5

// ---------------------
// Functions declaration
// ---------------------

int main(void);

void __attribute__ ((interrupt, no_auto_psv)) _T1Interrupt();
void __attribute__ ((interrupt, no_auto_psv)) _T2Interrupt();
void __attribute__ ((interrupt, no_auto_psv)) _T3Interrupt();
void __attribute__ ((interrupt, no_auto_psv)) _T4Interrupt();

uint8 keypad_decode(uint8,uint8);
void conv_BCD(uint32,uint8*,uint8);
void show_menu(uint8,uint8*);
void print7s(int8*,uint8*);

#endif

