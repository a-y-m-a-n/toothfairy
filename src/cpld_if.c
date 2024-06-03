#include "cpld_if.h"
#include <avr/io.h>
#include "hex_string.h"
#include <avr/pgmspace.h>

/*
 * On the REV B PCB, the strobe is on PD0,
 * and DATA0 is at PB0, DATA1 at PB2 and
 * DATA2 at PB4
 */

void cpld_reset(void)
{
  DDRB |= _BV(DDB0)|_BV(DDB2)|_BV(DDB4);
  DDRD |= _BV(DDD0);

  /*
  uint8_t j = 0;
  do
    {
      cpld_command( j );
      j+=0x10;
    }
  while(j);
  */
  cpld_command(0);
}


#define DATAMASK (_BV(DDB0)|_BV(DDB2)|_BV(DDB4))

void cpld_command(unsigned char command)
{

  // clear all signals
  PORTB &= ~(DATAMASK);
  PORTD &= ~(_BV(PORTD0));   //REV B PCB

  // Put the sense line address on the bus
  //PORTB |= (DATAMASK & (command>>4));  // REVA PCB
  PORTB |= (DATAMASK & (command>>1));
  // raise the strobe
  PORTD |= _BV(PORTD0);

  // erase the sense line address
  PORTB &= ~DATAMASK;

  // put the scan select on the bus
  //PORTB |= (DATAMASK & (command));  // REV A PCB
  PORTB |= (DATAMASK & (command));
  
  // lower the strobe
  PORTD &= ~_BV(PORTD0);

}
