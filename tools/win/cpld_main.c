#include <windows.h>
#include "bitbang.h"
#include "ppi.h"
#include <stdio.h>
#include <unistd.h>
#include "interface.h"
#include "xvsf_file.h"
#include "conduit.h"

// For the execute function
#include "playxsvf_src/micro.h"

static void conduit_test(void)
{
  int j,k;
  int tests = 1000000l;
  int loops = 200l;
  long dlyl = 10l;

  for (k=0; k<tests; k++)
    {

      //printf("TCK TOGGLE\n");
      for (j=0; j<loops; j++)
	{
	  //printf("%d\n", j);
	  conduit_set_TCK(0);
	  //usleep(dlyl);
	  conduit_set_TCK(1);
	  //usleep(dlyl);
	}
      conduit_set_TCK(0);

      //printf("TDI TOGGLE\n");
      for (j=0; j<loops; j++)
	{
	  //printf("%d\n", j);
	  conduit_set_TDI(0);
	  //usleep(dlyl);
	  conduit_set_TDI(1);
	  //usleep(dlyl);
	}
      conduit_set_TDI(0);

      //printf("TMS TOGGLE\n");
      for (j=0; j<loops; j++)
	{
	  //printf("%d\n", j);
	  conduit_set_TMS(0);
	  //usleep(dlyl);
	  conduit_set_TMS(1);
	  //usleep(dlyl);
	}
      conduit_set_TMS(0);

#if 0
      //printf("TDO SAMPLING and MIRROR on TMS\n");
      for (j=0; j<loops; j++)
	{
	  unsigned char tdo;
	  //usleep(dlyl);      
	  //printf("%d ", j);
	  tdo = conduit_get_TDO();
	  //printf("TDO=%d\n", tdo);
	  conduit_set_TMS(tdo);
	}
      conduit_set_TMS(0);
#endif
    }
}


static int go_time(void)
{
  // open the input file


  interface_open();
  

  //conduit_test();
  printf("Execute\n");
  
  int rc = xsvfExecute();
#if 0
  {
    // TEST MODE
    printf("Test Loop\n");
    
    int j,k;
    
    for (k=0; k<1000;k++)
      for (j=0; j<256; j++)
	{
	  unsigned char co;
	  interface_txrx(j,&co);
	  // 100us is sufficient between bytes
	  usleep(100l); 
	  // co should be equal to j on 2nd read
	  interface_txrx(j,&co);
	  usleep(100l);
	  co = ~co;
	  if (j!=co) { printf("ERROR %02X  %02X\n", j, co); exit (1); }
	}
  }
#endif
  
  
  return rc;

  interface_close();
  
}


int main(int argc, char* argv[])
{
  char xvsf_name[1024]={0};
  int c;
  while ((c = getopt (argc, argv, "f:")) != -1)
    {
      switch (c)
	{
	case 'f':
	  if ('f'==optopt)
	    {
	      fprintf(stderr, "-f <filename> required\n");
	      exit(1);
	    }
	  strcpy(xvsf_name, optarg);
	  break;
	default:
	  abort ();
	}
    }


  open_input(xvsf_name);

  
  // do the deed
  int rc = go_time();
  printf("%d\n", rc);

  return rc;
}

