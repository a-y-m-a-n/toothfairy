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
  OCR1AH=0x00;
  OCR1AL=bright_level;
}


// Blinks the led the specified number of times with a 50% duty cycle
void led_blink(uint8_t count)
{
  // could optimize by getting rid of this and change the calls to include
  // the mul by 2
  count<<=1;
  do
    {
      _delay_ms(40);
      DDRB ^= _BV(DDB1);
    }
  while(--count);
}
