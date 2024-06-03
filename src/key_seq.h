/**************************************************************
 * key_seq.h
 *
 * Defines the data structure for storing the key sequences
 *************************************************************/

#ifndef key_seq_h
#define key_seq_h

#include <inttypes.h>

typedef struct
{
   uint8_t key;
   uint8_t to[2];
}
key_seq_t;

#endif


#if 0 
// UNIT TEST CODE FOR THE Sequencer

#if 0
#include <avr/eeprom.h>
#else
#include <stdio.h>
typedef unsigned char uint8_t;
#define EEMEM 
#define PROGMEM 
#endif

// KEY, HOLD, RELEASE
typedef struct
{
  uint8_t key;
  uint8_t to[2];
}
key_seq_t;


key_seq_t shutdown_sequence EEMEM[17]=
    {
      {8,{2,2}},
      {1,{2,1}},
      {4,{5,3}},
      {0xFF, {0,0}},
    }
;


uint8_t sub_state = 0;
uint8_t timeout;
uint8_t done;


static void cpld_reset(){printf("reset\n");}
static void cpld_command(uint8_t cmd){printf("command %02X\n", cmd);}

uint8_t power_state;

//void state_run_shutdown_macro(void)
void run_sequence(void)
{
  power_state = 0;

  //state_t state; 
  //state.next = state_run_shutdown_macro; 

  uint8_t idx = sub_state>>1;

  uint8_t k=0;

  key_seq_t *key_seq = shutdown_sequence;
  // Much smaller code than directly dereferencing the pointer as shown in 
  // the later code
  //while(k<idx) { key_seq++; }
  for (k=0;k<idx;k++) { key_seq++;}
  //key_seq_t *key_seq = (key_seq_t*)&((uint8_t*)shutdown_sequence)[idx+(idx>>1)];

  uint8_t phase = sub_state & 0x1;

  if (0xFF == key_seq->key) 
    {
      //state.next = state_run;
      done = 1;
    }
  else
    {
      uint8_t val = key_seq->to[phase];
      timeout = val;
      if (phase)
	{
	  if (val>1) { cpld_reset(); } 
	}
      else
	{
	  cpld_command(key_seq->key);
	}
    }
  sub_state++;

  //return state;
}




int main(void)
{
  sub_state = 0;
  while(!done  && sub_state<30) { run_sequence(); printf("----\n");}
}


#endif  // UNIT TEST CODE
