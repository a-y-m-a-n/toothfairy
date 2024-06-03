/**
 * Copyright 2007 Ayman M. El-Khashab
 *
 * CPLDProg 
 *
 * CPLDProg interfaces with the host application over the ISP port.  The ISP will transmit
 * bytes of data represeting the desired state of the CPLD jtag lines.  The micro will place
 * those signals onto the AVR->CPLD jtag bus and then read back the TDO signal from the JTAG
 * bus and place that into the return data to the host.  Every transaction from the host will
 * write a byte to alter the control signals, and then read the byte and send to host to get
 * the status of the TDO signal back to the host
 *
 * ATTiny2313
 * 
 * SCK  - PB7 - TO AVR
 * MISO - PB6 - TO HOST
 * MOSI - PB5 - TO AVR
 *
 * Will use the following encoding for host->avr
 *      TO AVR  TO HOST
 * ----------------------
 * D7   TDI      
 * D6   TCK 
 * D5   TMS 
 * D4   RES
 * D3   REQ         
 * D2            
 * D1            ROK
 * D0            TDO
 * 
 * The D3 bit is always set when transmitting TD0 as part of the procedure and is then reset
 * by the host.  So if the host does not see a bit high when reading a byte from the register
 * then it knows the controller did not update the byte properly
 *
 */

#define F_CPU 1000000UL  // 1 MHz


#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "endpoint.h"

// guar. to be zero
volatile unsigned char data_avail;
volatile unsigned char spi_active;


// Bit definitions for the conduit
#define CMD_TDI 7
#define CMD_TCK 6
#define CMD_TMS 5
#define CMD_REQ 3
#define CMD_ROK 1
#define CMD_TDO 0


// Triggered at the end of each byte transfer
ISR(USI_OVERFLOW_vect)
{
  // Ack interrupt by writing USIOIF
  USISR |= (1<<USIOIF);
  // Rearm the start interrupt
  USICR |= (1<<USISIE);
  
  // signal listener
  spi_active = 0;
  data_avail = 1;
}


// Interrupt vector is triggered whenever the SCLK is toggled but is 
// disabled after being armed
ISR(USI_START_vect)
{
  // Mark active
  spi_active = 1;

  // Ack interrupt by writing USIOIF and disable further interrupts
  // until transaction is complete
  USISR |= (1<<USISIF);
  USICR &= ~(1<<USISIE);

}



int init_serial_bus(void)
{
  // D3 is the led
  // Set D6 to an output, all others to input
  DDRB = (1<<DDB6) | (1<<DDB3) | (1<<DDB0);

  // Three wire mode, interrupt enable on counter overflow
  USICR = (1<<USISIE)|(1<<USIOIE) // Enable start and overflow interrupt
    | (1<<USIWM1)|(0<<USIWM0)     // select 3 wire mode
    | (1<<USICS1)|(0<<USICS0)|(0<<USICLK);  // pick counter clock src

  USIDR = 0; // zero data register

  return 0;
}




unsigned char rx_byte(void)
{
  return USIDR;
}



void tx_byte(unsigned char c)
{
  //USIDR |= c;
}


int main(void)
{
  unsigned char ci, co;

  init_serial_bus();

  co = init_process_command();
  //tx_byte(co);

  // enable interrupts
  sei();

  while(1)
    {
      if (data_avail)
	{
  
#if 0
	  // TEST MODE
	  // get the data and flip for testint
	  ci = rx_byte();
	  co = ci;
	  tx_byte(~co);
	  
	  // toggle the winky light
	  PORTB = ci & 0x1;
#else
	  
	  // FUNCTIONAL Code
	  
	  ci = rx_byte();
	  co = process_command(ci);
	  tx_byte(co);
#endif
	  // reset signal
	  data_avail = 0;
	}
      
      co = process_output();

    }      
  
  return 0;
}


