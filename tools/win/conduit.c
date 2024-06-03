#include <stdio.h>
#include "conduit.h"
#include "interface.h"
#include <unistd.h>
#include "ppi.h"
#include <stdlib.h>
#include "bitbang.h"


// TDI = D7
// TCK = D6
// TMS = D5
// XXX = D4
// REQ = D3  // Request to read TDO

// ROK = D1
// TDO = D0
// 

#define PB_TDI 7
#define PB_TCK 6
#define PB_TMS 5
#define PB_REQ 3
#define PB_ROK 1
#define PB_TDO 0


#define bit_set(x,b) (x|=b) 
#define bit_clr(x,b) (x&=~b) 


// Value of the port at the target
static unsigned char tgt_port_val;

static void conduit_verify_readout(unsigned char read_out)
{
#if 0
  if (0 == (read_out & (1<<PB_ROK)))
    {
      printf("Failed to find ROK bit set in response\n");
      exit(1);
    }
#endif
  // Add a delay for the next go round, 200 works
  usleep(100);
}

void conduit_set_TDI(const short v)
{
  unsigned char read_out;

  if (v)
    bit_set(tgt_port_val, (1<<PB_TDI));
  else
    bit_clr(tgt_port_val, (1<<PB_TDI));

  bit_clr(tgt_port_val, (1<<PB_REQ)|(1<<PB_ROK)|(1<<PB_TDO));

  // Transmit
  interface_txrx(tgt_port_val, &read_out);
  conduit_verify_readout(read_out);
}

void conduit_set_TCK(const short v)
{
  unsigned char read_out;
  if (v)
    bit_set(tgt_port_val, (1<<PB_TCK));
  else
    bit_clr(tgt_port_val, (1<<PB_TCK));

  bit_clr(tgt_port_val, (1<<PB_REQ)|(1<<PB_ROK)|(1<<PB_TDO));

  // Transmit
  interface_txrx(tgt_port_val, &read_out);
  conduit_verify_readout(read_out);
}

void conduit_set_TMS(const short v)
{
  unsigned char read_out;
  if (v)
    bit_set(tgt_port_val, (1<<PB_TMS));
  else
    bit_clr(tgt_port_val, (1<<PB_TMS));

  bit_clr(tgt_port_val, (1<<PB_REQ)|(1<<PB_ROK)|(1<<PB_TDO));

  // Transmit
  interface_txrx(tgt_port_val, &read_out);
  conduit_verify_readout(read_out);
}


unsigned char conduit_get_TDO(void)
{
  unsigned char read_out;
#if 0
  interface_txrx(0, &read_out);
  usleep(200);
  interface_txrx(0, &read_out);
  usleep(200);
  return (read_out & (1<<PB_TDO)) ? 1 : 0;
#else
  unsigned char co;
  unsigned char x;
  //interface_txrx(0, &read_out);
  //usleep(200);
  //interface_txrx(0, &read_out);
  //usleep(200);
  usleep(70); //was 50
  interface_status(&co);

  return co;
#endif
}

unsigned char conduit_get_TDO_x(void)
{
  unsigned char read_out;
  unsigned char tgt_port_val_out = 0;

  // Clear the request_OK and TDO bits
  //bit_clr(tgt_port_val, (1<<PB_ROK)|(1<<PB_TDO));
  //bit_set(tgt_port_val, (1<<PB_REQ));

  // Transmit request
  interface_txrx(tgt_port_val, &read_out);
  conduit_verify_readout(read_out);

  // Wait a moment to sample TDO signal, unneeded
  // since the conduit verifies the readout was
  // correct first.
  //usleep(100);
  
  // Resend the same info to also get the result
  // readback.  

  interface_txrx(0, &tgt_port_val_out);
  conduit_verify_readout(tgt_port_val_out);
  //tgt_port_val_out = read_out;

  //interface_status(&tgt_port_val_out);

  // DEBUG TDO Problems
  //printf("TDO=%d\n", (tgt_port_val_out & (1<<PB_TDO)) ? 1 : 0 );

  // ROK was set, so we read the data fine, lets
  // output the result
  return ( (tgt_port_val_out & (1<<PB_TDO)) ? 1 : 0 );



}
