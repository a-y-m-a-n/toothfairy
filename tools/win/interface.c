#include <stdio.h>
#include "interface.h"
#include "bitbang.h"
#include "ppi.h"

static union filedescriptor fd;


int interface_open(void)
{
  ppi_open("lpt1", &fd);
  printf("Opened fd=%d\n", fd.ifd);
  bitbang_initialize(&fd);
  usleep(1000000l);
  return 0;
}


int interface_close(void)
{
  ppi_close(&fd);
  return 0;
}


int interface_tx(unsigned char ci)
{
  unsigned char c_junk;
  bitbang_txrx(&fd, ci, &c_junk);
  return 0;
}

int interface_txrx(unsigned char ci, unsigned char *co)
{
  bitbang_txrx(&fd, ci, co);
  return 0;
}


int interface_status(unsigned char *co)
{
  bitbang_status(&fd, co);
  return 0;
}
