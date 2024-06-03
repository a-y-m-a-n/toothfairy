#include "ps3toothfairy.h"
#include "led.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "semaphore.h"

// LED is on PB3, OC1A, PCINT3
// Always setup in the capture to trigger the end after
// 10ms elapsed

void led_init(uint8_t bright_level)
{
  //DDRB  &= ~_BV(DDB3);
  // TCCR1A |= _BV(COM1A1)|_BV(COM1A0);  // DONE IN MAIN.C 

  OCR1AH=0x00;
  OCR1AL=bright_level;
}


// Toggle the LED on / off using the DDR Registers and just
// leave the PWM operating all the time.  Note that these methods
// are inlined to save space

#if 0

void led_off(void)
{
  DDRB  &= ~_BV(DDB3);
}


void led_on(void)
{
  DDRB  |= _BV(DDB3);
}

#endif


// Blinks the led the specified number of times with a 50% duty cycle
void led_blink(uint8_t count)
{
  // could optimize by getting rid of this and change the calls to include
  // the mul by 2
  count<<=1;
  do
    {
      _delay_loop_2(64<<8);
      DDRB ^= _BV(DDB3);
    }
  while(--count);
}
