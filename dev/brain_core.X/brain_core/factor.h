// =============================================================================
// File         : factor.h
// Date         : 29/09/2018
// Author       : QB
// Version      : 01
// Description  : Header file for the integer factorisation library
// =============================================================================

#ifndef FACTOR_H
#define	FACTOR_H

// -------
// Defines
// -------
// Define the maximum amount of different factors an integer can be made of.
// Since the maximum input number is 999 999 999, any integer below than that
// CANNOT be made of more than 9 different prime numbers.
// Indeed: the product of the 9 first primes is greater than 999 999 999.
#define MAX_NUMBER_FACTORS	10

// -----------------
// Types declaration
// -----------------
typedef struct divider_log divider_log;
struct divider_log
{
  uint16 power;
  uint32 remainder;
};

typedef struct factor_log factor_log;
struct factor_log
{
  uint16 N_factors;
  uint32 divider[MAX_NUMBER_FACTORS];
  uint16 power[MAX_NUMBER_FACTORS];
};

// ---------------------
// Functions declaration
// ---------------------
void factor(uint32,uint16,divider_log*);
void reset_factors(factor_log*);
void add_factor(uint32,uint16,factor_log*);

extern uint32 Q_div(uint16,uint16);

#endif