#include "endpoint.h"

#ifdef DEBUG
#include <stdio.h>

int tx_data;
int portd;

#define PORTD portd
#define USIDR usidr

#define tx_byte(x) (tx_data=x)
#else
#include <avr/io.h>
#endif

// Bit definitions for the conduit
#define CMD_TDI 7
#define CMD_TCK 6
#define CMD_TMS 5
#define CMD_REQ 3
#define CMD_ROK 1
#define CMD_TDO 0

// Signals are connected to port_d pins as listed
#define SIG_TCK 2 // AVR OUT
#define SIG_TDI 3 // AVR OUT
#define SIG_TDO 4 // AVR IN
#define SIG_TMS 5 // AVR OUT


unsigned char init_process_command(void)
{
  // Setup DDRD for the outputs on the proper pins
  DDRD = (1<<SIG_TCK)|(1<<SIG_TDI)|(1<<SIG_TMS);
  return (1<<CMD_ROK);
}


unsigned char process_output(void)
{
  unsigned char o;

  o = (PIND & (1<<SIG_TDO)) ? 1 : 0;


  if (o)
    {
      PORTB |= (1<<PORTB6)|(1<<PORTB3);
    }
  else
    {
      PORTB &= ~((1<<PORTB6)|(1<<PORTB3));
    }
	  
  return o;
}


// Return the cmd_out, if it is 0, then do nothing, if it has a value,
// then transmit
unsigned char process_command(unsigned char cmd)
{
  unsigned char cmd_out = 0;
  
  // Validate the command, make sure that ROK was not set by the master, 
  // if so, then we return 0 to force a failure
#if 0
  if (cmd & (1<<CMD_ROK))
    return 0;
#endif

  if (cmd & (1<<CMD_REQ))
    {
#if 0
      // Read the TDO signal from the hardware port, recall that the
      // inputs are on PINx rather than PORTx registers
      if (PIND & (1<<SIG_TDO))
	{
	  cmd_out =  ( (1<<CMD_ROK) | (1<<CMD_TDO) );
	}
      else
	{
	  cmd_out =  ( (1<<CMD_ROK) | (0<<CMD_TDO) );
	}
#endif
    }
  else
    {
      // Write the incoming bits to the hardware port

      if (cmd & (1<<CMD_TDI))
	PORTD |= (1<<SIG_TDI);
      else
	PORTD &= ~(1<<SIG_TDI);
      
      if (cmd & (1<<CMD_TMS))
	PORTD |= (1<<SIG_TMS);
      else
	PORTD &= ~(1<<SIG_TMS);
      
      if (cmd & (1<<CMD_TCK))
	PORTD |= (1<<SIG_TCK);
      else
	PORTD &= ~(1<<SIG_TCK);


      // Make sure to clear the read data so that we don't
      // mistake the host into thinking that a previous value
      // is still available.
      //cmd_out = (1<<CMD_ROK);
    }


  return process_output();
}


#ifdef DEBUG
int main(void)
{
  unsigned char cmd;
  unsigned int portd0;

  portd = 0;
  portd0 = portd;
  cmd = (1<<CMD_REQ);
  process_command(cmd);
  printf("cmd=%02X  PD0=%02X  PD1=%02X  TX=%02X\n", cmd, portd0, portd, tx_data);

  portd = (1<<SIG_TDO);
  portd0 = portd;
  cmd = (1<<CMD_REQ);
  process_command(cmd);
  printf("cmd=%02X  PD0=%02X  PD1=%02X  TX=%02X\n", cmd, portd0, portd, tx_data);

  portd0 = portd;
  cmd = (1<<CMD_TCK);
  process_command(cmd);
  printf("cmd=%02X  PD0=%02X  PD1=%02X  TX=%02X\n", cmd, portd0, portd, tx_data);

  portd0 = portd;
  cmd = (1<<CMD_TMS);
  process_command(cmd);
  printf("cmd=%02X  PD0=%02X  PD1=%02X  TX=%02X\n", cmd, portd0, portd, tx_data);

  portd0 = portd;
  cmd = (1<<CMD_TDI);
  process_command(cmd);
  printf("cmd=%02X  PD0=%02X  PD1=%02X  TX=%02X\n", cmd, portd0, portd, tx_data);

  portd0 = portd;
  cmd = (1<<CMD_TDI)|(1<<CMD_TMS);
  process_command(cmd);
  printf("cmd=%02X  PD0=%02X  PD1=%02X  TX=%02X\n", cmd, portd0, portd, tx_data);

  portd = (1<<SIG_TDO);
  portd0 = portd;
  cmd = (1<<CMD_TDI)|(1<<CMD_TMS)|(1<<CMD_REQ);
  process_command(cmd);
  printf("cmd=%02X  PD0=%02X  PD1=%02X  TX=%02X\n", cmd, portd0, portd, tx_data);

  return 0;
}

#endif
