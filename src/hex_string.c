#include "hex_string.h"
#include <inttypes.h>
#include <avr/pgmspace.h>

#ifdef __DEBUG__

const char ht[] PROGMEM = "0123456789ABCDEF";

void hex_string(uint32_t val, char *str)
{
  int8_t i;
  for (i=7;i>=0;i--)
    {
      str[i] = pgm_read_byte(&ht[val&0xF]);
      val = val >>4;
    }
  str[8]=0;
} 

#endif
