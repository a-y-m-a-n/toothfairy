#ifndef led_h
#define led_h

#include <inttypes.h>
#include <avr/io.h>

void led_init(uint8_t bright_level);

void led_on(void);

void led_off(void);


extern inline void led_off(void)
{
  DDRB  &= ~_BV(DDB3);
}


extern inline void led_on(void)
{
  DDRB  |= _BV(DDB3);
}


void led_blink(uint8_t count);

#endif
