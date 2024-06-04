#ifndef led_h
#define led_h

#include <inttypes.h>
#include <avr/io.h>

void led_init(uint8_t bright_level);

//void led_on(void);

//void led_off(void);


static inline void led_off(void)
{
  DDRB  &= ~(1<<DDB1);
}


static inline void led_on(void)
{
  DDRB  |= (1<<DDB1);
}


void led_blink(uint8_t count);

#endif
