#include "xvsf_file.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

static FILE *fp;
static struct stat stat_buf;
static int size_bytes;
static int read_bytes;
static int percent_complete;



int open_input(const char *filename)
{

  if (0 == filename[0])
    {
      fprintf(stderr, "No input file name specified (use -f <name>)");
      exit(1);
    }

  if (stat(filename, &stat_buf))
    {
      fprintf(stderr, "Could not access input file\n");
      exit(1);
    }
  
  // Get the size info from the file
  size_bytes = stat_buf.st_size;
  

  fp = fopen(filename, "rb");
  if (0 == fp)
    {

      fprintf(stderr,"Could not open input file\n");
      exit(1);
    }




  return 0;
}


int close_input(void)
{
  fclose(fp);
  return 0;
}


unsigned char get_next_char(void)
{
  read_bytes++;
  int val = fgetc(fp);
  int last_pct_complete = percent_complete;

  percent_complete = (read_bytes * 50 / size_bytes);
  if (percent_complete != last_pct_complete)
    percent_complete%10 ? printf("=") : printf("X"); fflush(stdout);
  if (50 == percent_complete)
    printf("\n");

  return (unsigned char) (val & 0xFF);
}
