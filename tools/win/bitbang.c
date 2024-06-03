#include "bitbang.h"
#include "ppi.h"
#include <stdio.h>

// heavily influenced from avrdude


void bitbang_delay(int us)
{
  volatile int del = us * 250;
  while (del > 0)
    del--;
}

int bitbang_power(union filedescriptor *fd, int power_enable)
{
  if (power_enable)
    ppi_set(fd, PPIDATA, B6|B5|B4|B3|B2);
  else
    ppi_clr(fd, PPIDATA, B6|B5|B4|B3|B2);

  return 0;
}


int bitbang_initialize(union filedescriptor *fd)
{
  
  bitbang_power(fd, 1);
  
  usleep(20000);

  // 0 the clock and reset pins
  
  ppi_clr(fd, PPIDATA, B0|B1);
  usleep(20000);
 
  // pulse reset
  ppi_set(fd, PPIDATA, B1);
  ppi_clr(fd, PPIDATA, B0);
  
  usleep(20000);

  return 0;
}


int bitbang_status(union filedescriptor *fd, unsigned char *co)
{
  unsigned int  read_data;

   // Read the data bit, PPISTATUS/7 is the only bit inverted on the
  // status register.  So we need to invert below with the 1/0 in
  // the read_data line
  read_data = ppi_get(fd, PPISTATUS, B7);
  
  // Remap the bit back to the low order bit, then shift, note that
  // the 0 and 1 are inverted for status/b7
  *co = (read_data&0x80 ? 0 : 1);

}


int bitbang_txrx(union filedescriptor *fd, unsigned char ci, unsigned char *co)
{
  unsigned char b;
  unsigned int  read_data;
  int i;

  *co = 0;

  for (i=7; i>=0; i--)
    {
      /*
       * Write and read one bit on SPI.
       * Some notes on timing: Let T be the time it takes to do
       * one pgm->setpin()-call resp. par clrpin()-call, then
       * - SCK is high for 2T
       * - SCK is low for 2T
       * - MOSI setuptime is 1T
       * - MOSI holdtime is 3T
       * - SCK low to MISO read is 2T to 3T
       * So we are within programming specs (expect for AT90S1200),
       * if and only if T>t_CLCL (t_CLCL=clock period of target system).
       *
       * Due to the delay introduced by "IN" and "OUT"-commands,
       * T is greater than 1us (more like 2us) on x86-architectures.
       * So programming works safely down to 1MHz target clock.
    */

      b = (ci >> i) & 0x01;
      

      // Set the input to the part, raise clock
      if (b)
	ppi_set(fd, PPIDATA, B7); 
      else
	ppi_clr(fd, PPIDATA, B7);

      // raise clock
      ppi_set(fd, PPIDATA, B0);


      // Read the data bit, PPISTATUS/7 is the only bit inverted on the
      // status register.  So we need to invert below with the 1/0 in
      // the read_data line
      read_data = ppi_get(fd, PPISTATUS, B7);


      // fall clock
      ppi_clr(fd, PPIDATA, B0);


      // Remap the bit back to the low order bit, then shift, note that
      // the 0 and 1 are inverted for status/b7
      *co |= ( (read_data & B7 ? 0 : 1) << i);

      //printf("%02X   %02X   %02X\n", b, *co, read_data);


    }

  return 0;
}



