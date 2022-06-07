// =============================================================================
// File         : main.c
// Date         : 03/07/2018
// Author       : Quentin
// Version      : 01
// Description  : Main source code for the brain core (dsPIC33EV256GM106)
// =============================================================================

// =============================================================================
// Pragmas
// =============================================================================
// FSEC
#pragma config BWRP = OFF               // Boot Segment Write-Protect Bit (Boot Segment may be written)
#pragma config BSS = DISABLED           // Boot Segment Code-Protect Level bits (No Protection (other than BWRP))
#pragma config BSS2 = OFF               // Boot Segment Control Bit (No Boot Segment)
#pragma config GWRP = OFF               // General Segment Write-Protect Bit (General Segment may be written)
#pragma config GSS = DISABLED           // General Segment Code-Protect Level bits (No Protection (other than GWRP))
#pragma config CWRP = OFF               // Configuration Segment Write-Protect Bit (Configuration Segment may be written)
#pragma config CSS = DISABLED           // Configuration Segment Code-Protect Level bits (No Protection (other than CWRP))
#pragma config AIVTDIS = DISABLE        // Alternate Interrupt Vector Table Disable Bit  (Disable Alternate Vector Table)

// FBSLIM
#pragma config BSLIM = 0x1FFF           // Boot Segment Code Flash Page Address Limit Bits (Boot Segment Flash Page Address Limit (0-0x1FFF))

// FOSCSEL
#pragma config FNOSC = FRC              // Initial oscillator Source Selection Bits (Internal Fast RC (FRC))
#pragma config IESO = OFF               // Two Speed Oscillator Start-Up Bit (Start up device with user selected oscillator source)

// FOSC
#pragma config POSCMD = NONE            // Primary Oscillator Mode Select Bits (Primary Oscillator disabled)
#pragma config OSCIOFNC = ON            // OSC2 Pin I/O Function Enable Bit (OSC2 is general purpose digital I/O pin)
#pragma config IOL1WAY = ON             // Peripheral Pin Select Configuration Bit (Allow Only One reconfiguration)
#pragma config FCKSM = CSECMD           // Clock Switching Mode Bits (Clock Switching is enabled,Fail-safe Clock Monitor is disabled)
#pragma config PLLKEN = ON              // PLL Lock Enable Bit (Clock switch to PLL source will wait until the PLL lock signal is valid)

// FWDT
#pragma config WDTPOST = PS32768        // Watchdog Timer Postscaler Bits (1:32,768)
#pragma config WDTPRE = PR128           // Watchdog Timer Prescaler Bit (1:128)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable Bits (WDT and SWDTEN Disabled)
#pragma config WINDIS = OFF             // Watchdog Timer Window Enable Bit (Watchdog timer in Non-Window Mode)
#pragma config WDTWIN = WIN25           // Watchdog Window Select Bits (WDT Window is 25% of WDT period)

// FPOR
#pragma config BOREN0 = ON              // Brown Out Reset Detection Bit (BOR is Enabled)

// FICD
#pragma config ICS = PGD3               // ICD Communication Channel Select Bits (Communicate on PGEC3 and PGED3)

// FDMTINTVL
#pragma config DMTIVTL = 0xFFFF         // Lower 16 Bits of 32 Bit DMT Window Interval (Lower 16 bits of 32 bit DMT window interval (0-0xFFFF))

// FDMTINTVH
#pragma config DMTIVTH = 0xFFFF         // Upper 16 Bits of 32 Bit DMT Window Interval (Upper 16 bits of 32 bit DMT window interval (0-0xFFFF))

// FDMTCNTL
#pragma config DMTCNTL = 0xFFFF         // Lower 16 Bits of 32 Bit DMT Instruction Count Time-Out Value (Lower 16 bits of 32 bit DMT instruction count time-out value (0-0xFFFF))

// FDMTCNTH
#pragma config DMTCNTH = 0xFFFF         // Upper 16 Bits of 32 Bit DMT Instruction Count Time-Out Value (Upper 16 bits of 32 bit DMT instruction count time-out value (0-0xFFFF))

// FDMT
#pragma config DMTEN = DISABLE          // Dead Man Timer Enable Bit (Dead Man Timer is Disabled and can be enabled by software)

// FDEVOPT
#pragma config PWMLOCK = ON             // PWM Lock Enable Bit (Certain PWM registers may only be written after key sequence)
#pragma config ALTI2C1 = OFF            // Alternate I2C1 Pins Selection Bit (I2C1 mapped to SDA1/SCL1 pins)

// FALTREG
#pragma config CTXT1 = NONE             // Interrupt Priority Level (IPL) Selection Bits For Alternate Working Register Set 1 (Not Assigned)
#pragma config CTXT2 = NONE             // Interrupt Priority Level (IPL) Selection Bits For Alternate Working Register Set 2 (Not Assigned)


// =============================================================================
// Includes
// =============================================================================
#include <xc.h>
#include "types.h"
#include "main.h"
#include "board.h"
#include "init.h"
#include "FIFO.h"
#include "stopwatch.h"
#include "factor.h"

// =============================================================================
// Global variables
// =============================================================================

// ---------
// Variables
// ---------
// This array is an image of the actual 7 segments display.
// display_array[0] is the lowest significant digit, display_array[8] the highest.
// The actual content of display_array will call the
uint8 display_array[9]  = {0,0,0,0,0,0,0,0,0};
uint8 DP_array[9]       = {0,0,0,0,0,0,0,0,0};

// Keypad variables
volatile uint8 keypad_update = 0;
volatile uint8 keypad_change = 0;

// ADC variables
uint8 ADC_update = 0;

// --------------
// Look-up tables
// --------------
// 7 segment LUT
// Usage: char_to_7seg['s' - ASCII_LUT_OFFSET] gives the LEDs to be ON to
// display 's'.
uint8 char_to_7seg[SEVEN_SEG_LUT_SIZE] = 
{
  #include "7seg_LUT.txt"
};

uint16 temp_LUT[TEMP_LUT_SIZE] = 
{
  #include "temp_LUT.txt"
};

// =============================================================================
// User functions
// =============================================================================
int main(void)
{    
  // ---------------
  // Local variables
  // ---------------

  // Main FSM variables
  uint8 FSM_state = FSM_INIT;
  uint8 FSM_next_state = FSM_INIT;
  uint8 current_digit = 0;
  uint32 input_number = 0;

  uint8 key_pressed = 0;
  uint8 current_item = 0;

  uint8 k = 0;
  
  // Keypad variables
  uint8 current_row = 0;
  uint8 column_status = 0;
  uint8 keypad_image[4] = {0,0,0,0};
  uint8 change_vector = 0;

  // Keypad FIFO
  key_FIFO key_FIFO_inst;

  // Factorisation log
  divider_log divider_log_instance;
  factor_log factor_log_instance;
  uint16 is_prime = 0;
  volatile uint16 factor_index = 0;
  
  // --------
  // PIC init
  // --------
  clock_init();
  IO_init();
  timer1_init();
  timer2_init();
  timer3_init();
  timer4_init();
  //PWM_init();
  ADC_init();
  
  interrupts_init();
  interrupts_enable();
  
  // Stopwatch 
  init_stopwatch();

  // Keypad FIFO
  init_FIFO(&key_FIFO_inst);

  // Start a new ADC acquisition
  ADC_START = 1;

  GREEN_LED = 0;
  RED_LED   = 0;

  // ---------
  // Main loop
  // ---------
  while(1)
  {
    // -----------------------------------------------------------------------------------
    // ADC handling
    // -----------------------------------------------------------------------------------
    if (ADC_update == 1)
    {
      uint16 ADC_result = 0;

      // Note:
      // At this point, we assume the ADC conversion is done (T_timer_2 >> T_acq)
      // ADC_DONE should be 1 here, otherwise, send error message!!
      //DBG_LED_1 = ~ADC_DONE;

      // Read result
      ADC_result = ADC1BUF0;

      if (ADC_result < 207)
      {
        ADC_result = 0;
      }
      else if (ADC_result > 786)
      {
        ADC_result = 579;
      }
      else
      {
        ADC_result = ADC_result - 207;
      }

      ADC_result = temp_LUT[ADC_result];
      
      // Call conversion function
      //conv_BCD(ADC_result,display_array);

      // Start a new acquisition
      ADC_START = 1;

      // Close activity
      ADC_update = 0;
    }
    
    // -----------------------------------------------------------------------------------
    // Keypad event handling
    // -----------------------------------------------------------------------------------
    if (keypad_update == 1)
    {
      // Interrogate the next row
      if (current_row == 3)
      {
        current_row = 0;
      }
      else    
      {
        current_row++;
      }

      // Update the corresponding row outputs
      ROW_VECTOR = 64 << current_row;

      // ---------------------
      // Sample the key status
      // ---------------------

      // Wait a bit before sampling
      // CPU is so fast, I/Os may not be established when 
      // sampling the columns!
      // See PORT/LAT difference.
      for(k = 0; k < 20; k++) {asm("nop");}
      
      // Sample the keypad columns for the current row
      uint16 temp = COLUMN_VECTOR;
      
      column_status = 0;
      column_status = (temp & 0x0060) >> 5;
      column_status = column_status | ((temp & 0x0100) >> 6);

      // Detect keypress event in the current column
      change_vector = ~keypad_image[current_row] & column_status;
      
      // Loop on each key of this row to detect whether it was pressed or not.
      // If yes, push it in the FIFO.
      if((change_vector & 0x01) == 1)
      { 
        key_pressed = keypad_decode(current_row,1);
        push_key(&key_FIFO_inst,key_pressed);
      }
      else if((change_vector & 0x02) == 2)
      { 
        key_pressed = keypad_decode(current_row,2);
        push_key(&key_FIFO_inst,key_pressed);
      }
      else if((change_vector & 0x04) == 4)
      { 
        key_pressed = keypad_decode(current_row,4);
        push_key(&key_FIFO_inst,key_pressed);
      }

      // Update the current column of the keypad image    
      keypad_image[current_row] = column_status;

      // Close activity
      keypad_update = 0;
    }

    // -----------------------------------------------------------------------------------
    // Debug timer event handling
    // -----------------------------------------------------------------------------------
    if (0)
    {
      
      //temp32 = Q_div(dividend,divider);

      
      // Show that!
      //conv_BCD(temp32,display_array,1);

    }

    // -----------------------------------------------------------------------------------
    // Main FSM
    // -----------------------------------------------------------------------------------
    
    // Compute the next state
    switch(FSM_state)
    {
      // ----------------------------------
      // FSM init: show the menu first item
      // ----------------------------------
      // Display "splash" screen
      case(FSM_INIT):
        GREEN_LED = 0;
        RED_LED = 0;
        show_menu(0,display_array);
        FSM_next_state = FSM_MENU;
        break;

      // ------------------------------------------------
      // FSM menu: navigation through the available items
      // ------------------------------------------------
      case(FSM_MENU):
        if (key_FIFO_inst.count > 0)
        {
          // Pop the current key
          key_pressed = pop_key(&key_FIFO_inst);

          // Handle the navigation through the menu
          if (key_pressed == KEY_DOWN)
          {
            if (current_item < 3){current_item++;}
            show_menu(current_item,display_array);
          }
          else if (key_pressed == KEY_UP)
          {
            if (current_item > 0){current_item--;}
            show_menu(current_item,display_array);
          }
          // '0' key hit: jump to the corresponding FSM state
          else if (key_pressed == 0)
          {
            if (current_item == 0)
            {
              FSM_next_state = FSM_TYPE_NUMBER;
              current_digit = 0;
              input_number = 0;
              
              print7s("   type  ",display_array);
            }
          }
        }
        break;

      // ------------------------------------------------
      // FSM type number: enter the number to be factored
      // ------------------------------------------------
      case(FSM_TYPE_NUMBER):
        if (key_FIFO_inst.count > 0)
        {
          // Pop the current key
          key_pressed = pop_key(&key_FIFO_inst);

          if (key_pressed == KEY_UP)
          {
            // Factor the number
            FSM_next_state = FSM_FACTOR;
          }
          else if (key_pressed == KEY_DOWN)
          {
            // Discard the number typed in.
            input_number = 0;
            current_digit = 0;
          }
          else
          {
            // Show the digit that was typed
            if (current_digit <= 8)
            {
              input_number = (10*input_number) + key_pressed;
              current_digit++;
            }
          }
          
          // Update the display
          conv_BCD(input_number,display_array,BLANKING_EN);

          // Reset the flag
          keypad_change = 0;
        }
        break;

      // -----------------------------------
      // FSM factor: factorisation procedure
      // -----------------------------------
      case(FSM_FACTOR):
        
        // Start stopwatch
        START_STOPWATCH;
        
        // Reset the factors list
        reset_factors(&factor_log_instance);

        // ----------
        // Try with 2
        // ----------
        uint16 D = 2;
        factor(input_number,D,&divider_log_instance);
        
        if (divider_log_instance.power != 0)
        {
          add_factor(D,divider_log_instance.power,&factor_log_instance);
          input_number = divider_log_instance.remainder;
        }

        // ----------
        // Try with 3
        // ----------
        D = 3;
        factor(input_number,D,&divider_log_instance);
        
        if (divider_log_instance.power != 0)
        {
          add_factor(D,divider_log_instance.power,&factor_log_instance);
          input_number = divider_log_instance.remainder;
        }

        // ----------
        // Try with 5
        // ----------
        D = 5;
        factor(input_number,D,&divider_log_instance);
        
        if (divider_log_instance.power != 0)
        {
          add_factor(D,divider_log_instance.power,&factor_log_instance);
          input_number = divider_log_instance.remainder;
        }

        // ----------------------
        // Try with 6k+1 and 6k+5
        // ----------------------
        uint16 n = 0;
        for(n = 1; n <= 5270; n++)
        {
          D = ((6*n) + 1);
          factor(input_number,D,&divider_log_instance);
          
          if (divider_log_instance.power != 0)
          {
            add_factor(D,divider_log_instance.power,&factor_log_instance);
            input_number = divider_log_instance.remainder;
          }

          D = ((6*n) + 5);
          factor(input_number,D,&divider_log_instance);
          
          if (divider_log_instance.power != 0)
          {
            add_factor(D,divider_log_instance.power,&factor_log_instance);
            input_number = divider_log_instance.remainder;
          }
        }

        // Check if the number is now fully factored.
        // If the remainder is not "1", it means that what is remaining is prime.
        // Example: try with 887 422 = 2 x 443 711
        if (input_number != 1)
        {
          add_factor(input_number,1,&factor_log_instance);
        }

        if ((factor_log_instance.N_factors == 0) || ((factor_log_instance.N_factors == 1) && (factor_log_instance.power[0] == 1)))
        {
          is_prime = 1;
        }
        else
        {
          is_prime = 0;
        }

        // Freeze stopwatch
        FREEZE_STOPWATCH;
        
        init_stopwatch();
        factor_index = 0;

        // Move to the next state
        FSM_next_state = SHOW_PRIMALITY;
        break;
      
      // --------------------------------------------------------
      // FSM show primality: display the log of the factorisation
      // --------------------------------------------------------
      case(SHOW_PRIMALITY):
        
        // Display primality
        if (is_prime == 1)
        {
          GREEN_LED = 1;
          print7s("  prime  ",display_array);
        }
        else
        {
          RED_LED = 1;
          print7s("not prime",display_array);
        }

        // Wait for a key hit to show the factors or to go back to the menu
        if (key_FIFO_inst.count > 0)
        {
          // Pop the current key
          key_pressed = pop_key(&key_FIFO_inst);
          
          if (is_prime == 1)
          {
            FSM_next_state = FSM_INIT; 
          }
          else
          {
           FSM_next_state = FSM_FACTOR_EXPLORE;    
          }
        }
        break;

      // ---------------------------------------------------------
      // FSM factor explore: show the integer factors if composite
      // ---------------------------------------------------------
      case(FSM_FACTOR_EXPLORE):
        
        // Show the first factor
        conv_BCD(factor_log_instance.divider[factor_index],display_array,BLANKING_EN);
        
        // Wait for a keyboard event
        if (key_FIFO_inst.count > 0)
        {
          // Pop the current key
          key_pressed = pop_key(&key_FIFO_inst);
          
          // Handle the navigation through the factors
          if (key_pressed == KEY_DOWN)
          {
            if (factor_index < (factor_log_instance.N_factors-1)){factor_index++;}
            conv_BCD(factor_log_instance.divider[factor_index],display_array,BLANKING_EN);
          }
          else if (key_pressed == KEY_UP)
          {
            if (factor_index > 0){factor_index--;}
            conv_BCD(factor_log_instance.divider[factor_index],display_array,BLANKING_EN);
          }
          // '0' key hit: jump to the main menu
          else if (key_pressed == 0)
          {
            if (current_item == 0)
            {
              FSM_next_state = FSM_INIT;
            }
          }
               
        }
        break;   
    }

    // Update state
    FSM_state = FSM_next_state;
  }
  return(0);
}

// =============================================================================
// Interrupts handling
// =============================================================================

// Remark:
// To get the name of every interrupt vector, click on the icon (?) on the right
// named "Compiler help". Then click on "Interrupt Vector Tables Reference - HTML"

// ------------------------------------------
// Timer 1 interrupt / Display MUXing (1 kHz)
// ------------------------------------------
void __attribute__ ((interrupt, no_auto_psv)) _T1Interrupt(void)
{      
  static uint8 display_index = 0;
  uint16 data_buffer = 0;

  // Reset interrupt flag
  IFS0bits.T1IF = 0;
  
  // ------------------------
  // Update display procedure
  // ------------------------
  
  // Increment the digit pointer
  if (display_index == 8)
  {
    display_index = 0;
  }
  else
  {
    display_index++;
  }

  // Turn off the display while switching the MUX position
  LATC = 0;

  // Assert the MUX selection with the digit pointer
  LATE = (display_index << 12);

  // Initialize 7 segment bus with the data to be sent
  data_buffer = 0;
  
  // Add char and the decimal point data
  data_buffer = data_buffer | display_array[display_index];
  data_buffer = data_buffer | (DP_array[display_index] << 7);

  // Send data
  LATC = data_buffer;
}

// ----------------------------------------------
// Timer 2 interrupt / Keypad acquisition (50 Hz)
// ----------------------------------------------
void __attribute__ ((interrupt, no_auto_psv)) _T2Interrupt(void)
{
  // Reset interrupt flag
  IFS0bits.T2IF = 0;

  // Set flag
  keypad_update = 1;
}

// -------------------------------------------
// Timer 3 interrupt / ADC acquisition (20 Hz)
// -------------------------------------------
void __attribute__ ((interrupt, no_auto_psv)) _T3Interrupt(void)
{      
  // Reset interrupt flag
  IFS0bits.T3IF = 0;

  ADC_update = 1;
}

// --------------------------------
// Timer 4 interrupt / Debug (4 Hz)
// --------------------------------
void __attribute__ ((interrupt, no_auto_psv)) _T4Interrupt(void)
{      
  static uint8 counter = 0;

  // Reset interrupt flag
  IFS1bits.T4IF = 0;

  // Funky "it's alive!" indicator with LED blinking
  if (counter == 15)
  {
    counter = 0;
    //GREEN_LED = 1;
  }
  else
  {
    counter++;
    //GREEN_LED = 0;
  } 
}

// ---------------------------------
// Keypad raw data decoding function
// ---------------------------------
uint8 keypad_decode(uint8 current_row, uint8 column_status)
{
  uint8 key_pressed = 0;

  if (column_status == 1)
  {
    key_pressed = 10 - (3*current_row);
  }
  else if (column_status == 2)
  {
    if (current_row == 0) // "0" button handling
    {
      key_pressed = 0;
    }
    else
    {
      key_pressed = 11 - (3*current_row);
    }
  }
  else if (column_status == 4)
  {
    key_pressed = 12 - (3*current_row);
  }
  else
  {
    key_pressed = 0;
  }

  return key_pressed;
}

// -----------------
// Conv_BCD function
// -----------------
// This function updates the display array with the number given as an input.
// It gives blanking features (blank = 1) for disabling the display of leading zeros.
void conv_BCD(uint32 number, uint8* p_display_array, uint8 blanking)
{
  // Initialize the temporary display array to work on.
  uint8 temp_array[9] = {16,16,16,16,16,16,16,16,16};
  uint8 k = 0;
  
  // Constant array
  uint32 const_digit[9] = 
  {
    1UL,
    10UL,
    100UL,
    1000UL,
    10000UL,
    100000UL,
    1000000UL,
    10000000UL,
    100000000UL,
  };
  
  // Loop on the digits and substract units, tens etc. as many times as it is possible.
  for (k = 9; k-- > 0; )
  {
    while (number >= const_digit[k])
    {
      number -= const_digit[k];
      temp_array[k]++;
    }
  }

  // Blanking procedure
  if (blanking == 1)
  {
    for (k = 9; k-- > 0; )
    {
      // Stop immediatly as soon as a non-zero value is encountered.
      if (temp_array[k] != 16)
      {
        break;
      }
      
      // If char is null, blank it.
      temp_array[k] = 0;   
    }
  }
  
  // Update the display
  for (k = 9; k-- > 0; )
  {
    *(p_display_array + k) = char_to_7seg[temp_array[k]];
  }
}

// ------------------
// show_menu function
// ------------------
// This function displays the menu corresponding to the input item
void show_menu(uint8 item, uint8* p_display_array)
{
  switch(item)
  {
    // 0: FACTOR
    case 0:
      print7s("Factor   ",p_display_array);
      break;

    // 1: PLAY
    case 1:
      print7s("Play     ",p_display_array);
      break;

    // 2: Show pi
    case 2:
      print7s("Show pi  ",p_display_array);
      break;

    // 3: Guess pi
    case 3:
      print7s("Guess pi ",p_display_array);
      break;
  }
}

// ------------------------
// 7 segment print function 
// ------------------------
void print7s(int8* string, uint8* p_display_array)
{
  uint8 k = 0;
  uint8 temp_char = 0;

  for(k = 0; k <= 8; k++)
  {
    temp_char = *((uint8*)(string + k));
    p_display_array[8-k] = char_to_7seg[temp_char - ASCII_LUT_OFFSET];
  }
}
