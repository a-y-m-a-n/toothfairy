//#define DEBUG

#ifndef DEBUG

#include <avr/io.h>

#else
#include <sys/types.h>
typedef u_int8_t uint8_t ;
#endif


//heavily overloaded member to perform the bits, determine if we have
// started and keep track of the pattern
// Has tolerance built in to handle the start bit, the 1 bit and the
// 0 bit.  
// Nominal : Start=2.4ms 1=1.2ms 0=0.6ms
// Band : Start = [2.04,2.76] 1=[1.0,1.38] 0=[0.44,0.8]
//
// Timer is set to 8uS period, but divided by 4 before coming in so
// really a 32uS period with 8 bit resolution
//
// Add the guard bands and here is what we get
// 
// 2.4ms = 75  [63,86] 
// 1.2ms = 37  [31,43]
// 0.6ms = 17  [15,25]

uint16_t pulse_stream_bits;
uint8_t pulse_bit_count;

void pulse_stream_init(void)
{
  pulse_stream_bits = 0;
  pulse_bit_count = 0;
}

void pulse_stream_add(uint8_t pulse_width)
{

  // Is this a start bit?, if so, then reset stream
  if (pulse_width >= 56 && pulse_width <= 95)
    {
      pulse_bit_count = 1;
      return;
    }
  if (pulse_width >= 11 && pulse_width <= 49)
  {
      if (pulse_bit_count > 16) return;

      pulse_bit_count++;
      pulse_stream_bits>>=1;

      if (pulse_width >= 27) 
      {
	pulse_stream_bits |= ((unsigned)0x8000);
      }
   }
   else
   {
      pulse_stream_init();
   }

}


// Returns one more than the count if there is data, and 0 if there is no data
unsigned char pulse_stream_bit_count(void)
{
  return pulse_bit_count;
}


// Returns the code for this data
unsigned int pulse_stream_get_code(void)
{
  return pulse_stream_bits;
}

#ifdef DEBUG
#include <stdio.h>


int main(void)
{
  /*
   * This example outputs 9d64, which maps to the audio button
   * used for debug to generate an appropriate signal
   */


  pulse_stream_init();
  
pulse_stream_add(75);
pulse_stream_add(18);
pulse_stream_add(18);
pulse_stream_add(35);
pulse_stream_add(18);
pulse_stream_add(18);
pulse_stream_add(35);
pulse_stream_add(35);
pulse_stream_add(18);
pulse_stream_add(35);
pulse_stream_add(18);
pulse_stream_add(35);
pulse_stream_add(35);
pulse_stream_add(35);
pulse_stream_add(18);
pulse_stream_add(18);
pulse_stream_add(35);

  printf("%08X\n", pulse_stream_get_code());

}

#endif
