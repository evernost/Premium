// =============================================================================
// File         : init.c
// Date         : 29/09/2018
// Author       : QB
// Version      : 02
// Description  : main functions required for correct startup
// =============================================================================

// =============================================================================
// Includes
// =============================================================================
#include <xc.h>
#include "types.h"
#include "init.h"

// =============================================================================
// Defines
// =============================================================================

// ----------------------
// IO defines (see p.115)
// ----------------------

// Latch statut:
// 0 = output is at 0V
// 1 = output is at 5V

//                        ---210987--4--10
#define LATA_INIT       0b0000000000000000
//                        ---++++++--+--++ LATA reset (9 bits port)

//                        5432109876543210
#define LATB_INIT       0b0000000000000000
//                        ++++++++++++++++ PORTB reset (16 bits port)

//                        5-32109876543210
#define LATC_INIT       0b0000001110000000
//                        +-++++++++++++++ PORTC reset (15 bits port)

//                        -------8-65-----
#define LATD_INIT       0b0000000000000000
//                        -------+-++----- PORTD reset (3 bits port)

//                        5432------------
#define LATE_INIT       0b0000000000000000
//                        ++++------------ PORTE reset (4 bits port)

//                        --------------10
#define LATF_INIT       0b0000000000000000
//                        --------------++ PORTF reset (2 bits port)

//                        ------9876------
#define LATG_INIT       0b0000000000000000
//                        ------++++------ PORTG reset (4 bits port)

// TRIS registers (p.207)
// 0 = pin is defined as output
// 1 = pin is defined as input
// Default state is input

//                         ---210987--4--10
#define TRISA_INIT       0b0000010000000000
//                         ---++++++--+--++ TRISA

//                         5432109876543210
#define TRISB_INIT       0b0000000000000000
//                         ++++++++++++++++ TRISB

//                         5-32109876543210
#define TRISC_INIT       0b0000000000000000
//                         +-++++++++++++++ TRISC

//                         -------8-65-----
#define TRISD_INIT       0b0000000101100000
//                         -------+-++----- TRISD

//                         5432------------
#define TRISE_INIT       0b0000000000000000
//                         ++++------------ TRISE

//                         --------------10
#define TRISF_INIT       0b0000000000000000
//                         --------------++ TRISF

//                         ------9876------
#define TRISG_INIT       0b0000000000000000
//                         ------++++------ TRISG


// ANSEL registers
// 0 = pin is defined as digital IO
// 1 = pin is defined as analog IO
// Caution!!! default statut is analog for all pins! (p.209)
//                          ---2109-7--4--10
#define ANSELA_INIT       0b0000010000000000
//                          ---++++-+--+--++ ANSELA

//                          ------987---3210
#define ANSELB_INIT       0b0000000000000000
//                          ------+++---++++ ANSELB

//                          ---2109876543210
#define ANSELC_INIT       0b0000000000000000
//                          ---+++++++++++++ ANSELC

//                          5432------------
#define ANSELE_INIT       0b0000000000000000
//                          ++++------------ ANSELE

//                          ------9876------
#define ANSELG_INIT       0b0000000000000000
//                          ------++++------ ANSELG



// =============================================================================
// User functions
// =============================================================================

void clock_init(void)
{
  // ---------------
  // Oscillator init
  // ---------------
  
  // Warning:
  // It is NOT possible to change the PLL configuration while running in PLL 
  // mode!
  // The PIC must start with the FRC only, then we set the PLL in software,
  // and then a clock switch is necessary to run in PLL mode.
  CLKDIVbits.DOZEN = 0;       // Fcy = Fosc/2
  CLKDIVbits.FRCDIV = 0;      // No postscaler on FRC
  
  // Clock PLL settings
  CLKDIVbits.PLLPRE = 0;      // PLL prescaler set at 2
  PLLFBDbits.PLLDIV = 73;     // PLL feedback set at 75
  CLKDIVbits.PLLPOST = 0;     // PLL postscaler set at 2
  
  // Thus, Fosc = ((7.37/2) x 75)/2 = 138.18 MHz
  // Fcy = Fosc/2 = 69 MHz approx. i.e. 69 MIPS :)
  
  // Switch to FRC + PLL source
  __builtin_write_OSCCONH(0x01);              // New source = FRC + PLL
  __builtin_write_OSCCONL(OSCCON | 0x01);     // Send switch request   
 
  // Wait for mode switch to be done
  while (OSCCONbits.COSC != 0b001);
  
  // Wait for PLL to lock
  while (OSCCONbits.LOCK != 1);
}

void IO_init(void)
{
  // I/O value init
  LATA = LATA_INIT;
  LATB = LATB_INIT;
  LATC = LATC_INIT;
  LATD = LATD_INIT;
  LATE = LATE_INIT;
  LATF = LATF_INIT;
  LATG = LATG_INIT;
  
  // I/O mode init (input or output)
  TRISA = TRISA_INIT;
  TRISB = TRISB_INIT;
  TRISC = TRISC_INIT;
  TRISD = TRISD_INIT;
  TRISE = TRISE_INIT;
  TRISF = TRISF_INIT;
  TRISG = TRISG_INIT;
  
  // I/O mode init (analog or digital)
  ANSELA = ANSELA_INIT;
  ANSELB = ANSELB_INIT;
  ANSELC = ANSELC_INIT;
  ANSELE = ANSELE_INIT;
  ANSELG = ANSELG_INIT;
}

// -------------------------------------
// Timer 1 init / Display MUXing (1 kHz)
// -------------------------------------
void timer1_init(void)
{
  T1CONbits.TON = 0;          // Turn timer off
  
  T1CONbits.TSIDL = 0;        // Continue in idle mode
  T1CONbits.TGATE = 0;        // No gated time accumulation
  T1CONbits.TCKPS = 0b10;     // 1:64 prescaler    
  T1CONbits.TCS = 0;          // Timer source is Tcy (= 69 MHz)

  TMR1 = 0;                   // Reset timer value
  PR1 = 1080;                 // Timer period = 1080 => ~ 1 kHz
  
  IFS0bits.T1IF = 0;          // Clear interrupt flag
  
  T1CONbits.TON = 1;          // Turn timer on
}

// -----------------------------------------
// Timer 2 init / Keypad acquisition (50 Hz)
// -----------------------------------------
void timer2_init(void)
{
  T2CONbits.TON = 0;          // Turn timer off

  T2CONbits.TSIDL = 0;        // Keep running in idle mode
  T2CONbits.TGATE = 0;        // No gated time accumulation
  T2CONbits.TCKPS = 0b11;     // 1:256 prescaler
  T2CONbits.T32 = 0;          // 16 bits mode
  T2CONbits.TCS = 0;          // Timer source is Tcy (= 69 MHz)

  TMR2 = 0;                   // Reset timer value
  PR2 = 2699;                 // Timer period = 2699 => ~ 100 Hz

  IFS0bits.T2IF = 0;          // Clear interrupt flag

  T2CONbits.TON = 1;          // Turn timer on
}

// --------------------------------------
// Timer 3 init / ADC acquisition (20 Hz)
// --------------------------------------
void timer3_init(void)
{
  T3CONbits.TON = 0;          // Turn timer off

  T3CONbits.TSIDL = 0;        // Keep running in idle mode
  T3CONbits.TGATE = 0;        // No gated time accumulation
  T3CONbits.TCKPS = 0b11;     // 1:256 prescaler  
  T3CONbits.TCS = 0;          // Timer source is Tcy (= 69 MHz)

  TMR3 = 0;                   // Reset timer value
  PR3 = 65535;                // Timer period = 65536 => ~ 4.11 Hz

  IFS0bits.T3IF = 0;          // Clear interrupt flag

  T3CONbits.TON = 1;          // Turn timer on
}

// ------------------------------------
// Timer 4 init / Stopwatch (1 ms tick)
// ------------------------------------
void timer4_init(void)
{
  T4CONbits.TON = 0;          // Turn timer off

  T4CONbits.TSIDL = 0;        // Keep running in idle mode
  T4CONbits.TGATE = 0;        // No gated time accumulation
  T4CONbits.TCKPS = 0b01;     // 1:8 prescaler  
  T4CONbits.TCS = 0;          // Timer source is Tcy (= 69 MHz)

  TMR4 = 0;                   // Reset timer value
  PR4 = 8637;                 // Timer period = 8 637 => ~ 1 ms tick

  IFS1bits.T4IF = 0;          // Clear interrupt flag

  T4CONbits.TON = 1;          // Turn timer on
}

// --------
// PWM init
// --------
void PWM_init(void)
{
  // Set PWM period to about 2.1 kHz
  PTPER = 65535;
  
  // Set phase shift
  PHASE1 = 0;
  PHASE2 = 0;
  PHASE3 = 0;

  // Set duty cycle
  PDC1 = 100;
  PDC2 = 300;
  PDC3 = 500;

  // Set Dead Time Values 
  DTR1 = 0;
  DTR2 = 0;
  DTR3 = 0;
  
  // Set Dead Time alternate Values 
  ALTDTR1 = 0;
  ALTDTR2 = 0;
  ALTDTR3 = 0;
  
  // Set PWM Mode to Independent and assign PWM I/Os
  IOCON1 = 0xCC00;
  IOCON2 = 0xCC00;
  IOCON3 = 0xCC00;
  
  // Set Primary Time Base, Edge-Aligned Mode and Independent Duty Cycles
  PWMCON1 = 0x0000;
  PWMCON2 = 0x0000;
  PWMCON3 = 0x0000;
  
  // No fault detection
  FCLCON1 = 0x0003;
  FCLCON2 = 0x0003;
  FCLCON3 = 0x0003;
  
  // 1:1 Prescaler 
  PTCON2 = 0x0000;
  
  // Enable PWM Module
  PTCON = 0x8000;
}

// --------------------
// ADC init (see p.285)
// --------------------
void ADC_init(void)
{
  // ADxCON1 register
  AD1CON1bits.ADSIDL = 0;         // Keep operating in idle mode
  AD1CON1bits.ADDMABM = 0;        // DMA mode. Do not care
  AD1CON1bits.AD12B = 0;          // 4 channels modes, 10 bits
  AD1CON1bits.FORM = 0b00;        // Output format is integer
  
  AD1CON1bits.ASAM = 0;           // Start conversion when SAMP is set
  AD1CON1bits.SSRCG = 0;          // Select sample clock source group 0
  AD1CON1bits.SSRC = 0b111;       // Auto-convert after sampling
  
  AD1CON1bits.SIMSAM = 1;         // 4 simultaneous channels sampling mode
  
  // ADxCON2 register
  AD1CON2bits.VCFG = 0;           // ADC references are the supplies 
  AD1CON2bits.CSCNA = 0;          // No input scanning
  AD1CON2bits.CHPS = 0b11;        // Sample all the 4 channels
  AD1CON2bits.SMPI = 0;           // Generate interrupt at the end of every acquisition
                                  // (= 4 samples/interrupt, since 4 channels are acquired simultaneously)
  AD1CON2bits.BUFM = 0;           // Fill buffer every time from start address
  AD1CON2bits.ALTS = 0;           // Always route signals on MUX A
  
  // ADxCON3 register
  // Warning: TAD must be greater than 75 ns (see Electrical AC charac.)
  AD1CON3bits.ADRC = 0;           // Clock source is system clock
  AD1CON3bits.SAMC = 31;          // Sample time = 31 x TAD = 4.5 µs
  AD1CON3bits.ADCS = 9;           // TAD = (9+1) x Tcy = 145 ns > 75 ns (min TAD)
  
  // ADxCON4 register
  AD1CON4bits.ADDMAEN = 0;        // No DMA is used
  
  // ADC channel 0 config
  AD1CHS0bits.CH0NA = 0;          // Negative reference is GND
  AD1CHS0bits.CH0SA = 56;         // AN56 is routed on Channel 0
  
  // ADC channel 1-2-3 config
  AD1CHS123bits.CH123NA = 0;      // Negative reference is GND
  AD1CHS123bits.CH123SA2 = 0;     //
  AD1CHS123bits.CH123SA1 = 0;     // AN0 is routed on Channel 1
  AD1CHS123bits.CH123SA0 = 0;     //
  
  // Start ADC module
  AD1CON1bits.ADON = 1; 
}

void interrupts_init(void)
{
  // See p.129 & especially "Interrupts" section in the family ref. manual p.6-26 "Code example"
  // IFSO: flags registers
  // IECO: interrupt enable registers
  // IPC: priority bits
  
  // Interrupts settings
  INTCON2bits.GIE = 0;        // Disable all interrupts
  INTCON1bits.NSTDIS = 0;     // Interrupts nesting enabled
  
  // Timer 1
  IFS0bits.T1IF = 0;          // Clear TIMER1 interrupt flag
  IPC0bits.T1IP = 5;          // Priority level is set at 5
  IEC0bits.T1IE = 1;          // Interrupts enabled
  
  // Timer 2
  IFS0bits.T2IF = 0;          // Clear TIMER2 interrupt flag
  IPC1bits.T2IP = 4;          // Priority level is set at 4
  IEC0bits.T2IE = 1;          // Interrupts enabled
  
  // Timer 3
  IFS0bits.T3IF = 0;          // Clear TIMER3 interrupt flag
  IPC2bits.T3IP = 3;          // Priority level is set at 3
  IEC0bits.T3IE = 1;          // Interrupts enabled

  // Timer 4
  IFS1bits.T4IF = 0;          // Clear TIMER4 interrupt flag
  IPC6bits.T4IP = 2;          // Priority level is set at 2
  IEC1bits.T4IE = 1;          // Interrupts enabled

  // SPI
  IFS0bits.SPI1IF = 0;        // Clear SPI1 interrupt flag
  IPC3bits.U1TXIP = 3;        // Priority level is set at 0
  IEC0bits.SPI1IE = 0;        // Interrupts disabled
  
  // UART
  IFS0bits.U1RXIF = 0;        // Clear UART1 RX interrupt flag
  IPC2bits.U1RXIP = 3;        // Priority level is set at 1    
  IEC0bits.U1RXIE = 0;        // Interrupts enabled   
  
  IFS0bits.U1TXIF = 0;        // Clear UART1 TX interrupt flag
  IPC3bits.U1TXIP = 0;        // Priority level is set at 0
  IEC0bits.U1TXIE = 0;        // Interrupts disabled
}

void interrupts_enable(void)
{
  INTCON2bits.GIE = 1;
}