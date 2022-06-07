// =============================================================================
// File         : FIFO.h
// Date         : 02/09/2018
// Author       : Quentin
// Version      : 01
// Description  : Header file for the FIFO library
// =============================================================================

#ifndef FIFO_H
#define	FIFO_H

// -------
// Defines
// -------
#define FIFO_SIZE	10

// -----------------
// Types declaration
// -----------------
typedef struct key_FIFO key_FIFO;
struct key_FIFO
{
  uint16 count;
  uint16 r_pointer;
  uint16 w_pointer;
  uint8 key_array[FIFO_SIZE];
};

// ---------------------
// Functions declaration
// ---------------------
void init_FIFO(key_FIFO*);
void push_key(key_FIFO*, uint8);
uint8 pop_key(key_FIFO*);


#endif