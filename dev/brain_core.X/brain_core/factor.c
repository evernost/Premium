// =============================================================================
// File         : factor.c
// Date         : 29/09/2018
// Author       : QB
// Version      : 01
// Description  : Integer factorisation library
// =============================================================================


// =============================================================================
// Includes
// =============================================================================
#include "types.h"
#include "factor.h"


// =============================================================================
// Global variables
// =============================================================================

// ---------
// Variables
// ---------


// --------------
// Look-up tables
// --------------


// =============================================================================
// User functions
// =============================================================================

// ----------------------
// Factorisation function
// ----------------------
void factor(uint32 dividend, uint16 divider, divider_log* divider_log_instance)
{
  // Initialise variables
  uint32 input_number;
  uint32 output_quotient;
  uint16 Nh;
  uint16 Nl;
  uint32 div_output;
  uint16 temp_remainder;
  uint32 temp_dividend;
  int8 k = 0;
  uint8 pow = 0;

  input_number = dividend;

  // Initialise output object
  divider_log_instance -> power = 0;
  divider_log_instance -> remainder = dividend;

  do
  {
    // Split the input number into 2 halves
    Nh = (input_number & 0xFFFF0000) >> 16;
    Nl = (input_number & 0x0000FFFF) >> 0;

    // Step 1: divide the highest half by the quotient.
    div_output = Q_div(Nh,divider);
    output_quotient = (div_output & 0x0000FFFF) << 16;
    temp_remainder  = (div_output & 0xFFFF0000) >> 16;

    // Step 2: add the LSB of the lowest half to the remainder one bit at a time
    temp_dividend = 0;
    k = 15;
    
    for(k = 15; k >= 0; k--)
    {
      temp_dividend = (temp_remainder << 1) | ((Nl >> k) & 0x0001);
      if (temp_dividend >= divider)
      {
        output_quotient = output_quotient | (1U << k);
        temp_remainder = temp_dividend - divider;
      }
      else
      {
        temp_remainder = temp_dividend;
      }
    }

    // Prepare the next iteration
    input_number = output_quotient;
    if(temp_remainder == 0)
    {
      pow++;
      divider_log_instance -> power = pow;
      divider_log_instance -> remainder = output_quotient;
    }
  }
  while(temp_remainder == 0);   // divide as long as is it possible
}

void reset_factors(factor_log* factor_log_instance)
{
  // Reset the factor count
  factor_log_instance -> N_factors = 0;

  // Reset the factorisation arrays
  uint16 k = 0;
  for(k = 0; k < MAX_NUMBER_FACTORS; k++)
  {
    factor_log_instance -> divider[k] = 0;
    factor_log_instance -> power[k] = 0;
  }
}

void add_factor(uint32 divider, uint16 power, factor_log* factor_log_instance)
{
  factor_log_instance -> divider[factor_log_instance -> N_factors] = divider;
  factor_log_instance -> power[factor_log_instance -> N_factors] = power;
  (factor_log_instance -> N_factors)++;
}
