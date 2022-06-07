// =============================================================================
// File         : init.h
// Date         : 31/08/2017
// Author       : Quentin
// Version      : 01
// Description  : Header file for the initialisation functions
// =============================================================================

#ifndef INIT_H
#define	INIT_H

// -----------------
// Types declaration
// -----------------
// None.

// -------
// Defines
// -------
#define ADC_START	AD1CON1bits.SAMP 
#define ADC_DONE	AD1CON1bits.DONE 

// ---------------------
// Functions declaration
// ---------------------
void clock_init(void);
void IO_init(void);
void timer1_init();
void timer2_init();
void timer3_init();
void timer4_init();
void PWM_init();
void ADC_init();
void interrupts_init(void);
void interrupts_enable(void);

#endif

