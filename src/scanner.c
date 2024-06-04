#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "scan_mapping.h"
#include "semaphore.h"

int8_t current_scan_line;
volatile uint8_t ddrc_next;
volatile uint8_t ddrd_next;
//uint8_t next_scan, next_sense;

// the values are 0 to 7, and 8+ for no action
int8_t scan_line[20]; // 9 to 28 (i.e. element 0 is scan line 9 )


static inline uint8_t is_passive_scan (void)
{
  return (0 == (PIND & ((1 << PIND3) | (1 << PIND2))));
}


static void inline set_highz(void)
{
  // disable the outputs
  DDRC &= ~(0x3f);
  DDRD &= ~(0x30);
}


//extern void scanner_prep_next_go(void);


void scanner_reset (void)
{
  uint8_t j = 0;
  for (j = 0; j < 20; j++)
  {
    // valid values are 9 to 28, 0 is off
    scan_line[j] = 8;
  }
  set_highz();
}

void scanner_init (void)
{
  scanner_reset();
  // setup sense outputs, then are always low (using the
  // other device pullups.  we just control the outputs.
  set_highz();
  DDRB |= ((1<<DDB3) | (1 << DDB2));
}


void scanner_command (unsigned char command)
{
  // they are stored in reverse order so we get the 
  // scan line by taking what is passed and subbing
  // it from 8.
  int8_t sense = 8 - SENS (command);

  scan_line[SCAN(command)-9] = sense;

  // now we need to assert the sense line if scanning
  // is in the passive state, otherwise it will get 
  // picked up during active scan
  if (is_passive_scan ())
  {
    if (sense<6)  // sense 8,7,6,5,4,3 (values 7:2)
       {
          DDRC|=(1<<(sense));
       }
       else      // sense 2,1 (values 1:0)
       {
          // subtract 2, so that we map 6 and 7 to
          // sense physical 1 and 2, which is PD5
          // and PD4 (sense 7 - 2 = 5, which is PD5)
          // same for PD4
          DDRD|=(0x10<<(sense-6));
       }
  }
}
