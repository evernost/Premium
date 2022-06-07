// =============================================================================
// File         : FIFO.c
// Date         : 02/09/2018
// Author       : Quentin
// Version      : 01
// Description  : FIFO library
// =============================================================================


// =============================================================================
// Includes
// =============================================================================
#include "types.h"
#include "FIFO.h"


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

// -------------------
// FIFO initialisation
// -------------------
void init_FIFO(key_FIFO* key_FIFO_inst)
{
  key_FIFO_inst -> count = 0;
  key_FIFO_inst -> r_pointer = 0;
  key_FIFO_inst -> w_pointer = 0;
}

// -----------------
// push_key function
// -----------------
void push_key(key_FIFO* key_FIFO_inst, uint8 key_code)
{
  if ((key_FIFO_inst -> count) < FIFO_SIZE)
  {
    key_FIFO_inst -> key_array[key_FIFO_inst -> w_pointer] = key_code;

    if ((key_FIFO_inst -> w_pointer) == (FIFO_SIZE-1))
    {
      key_FIFO_inst -> w_pointer = 0;
    }
    else
    {
      key_FIFO_inst -> w_pointer  = (key_FIFO_inst -> w_pointer) + 1;
    }
    
    key_FIFO_inst -> count = (key_FIFO_inst -> count) + 1;
  }
}

uint8 pop_key(key_FIFO* key_FIFO_inst)
{
  // Look at the first element of the FIFO
  if ((key_FIFO_inst -> count) == 0)
  {
    // FIFO is empty!
    return -1;
  }
  else
  {
    uint8 output_key;

    output_key = key_FIFO_inst -> key_array[key_FIFO_inst -> r_pointer];
    
    if ((key_FIFO_inst -> r_pointer) == (FIFO_SIZE-1))
    {
      key_FIFO_inst -> r_pointer = 0;
    }
    else
    {
      key_FIFO_inst -> r_pointer = (key_FIFO_inst -> r_pointer) + 1;
    }
    
    key_FIFO_inst -> count = (key_FIFO_inst -> count) - 1;

    return (output_key);
  }
}
