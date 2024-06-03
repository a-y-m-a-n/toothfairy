#ifndef bitbang_h
#define bitbang_h

#include "ppi.h"



#ifdef __cplusplus
extern "C" {
#endif

void bitbang_delay(int us);
int bitbang_power(union filedescriptor *fd, int power_enable);
int bitbang_initialize(union filedescriptor *fd);

int bitbang_txrx(union filedescriptor *fd, unsigned char ci, unsigned char *co);

int bitbang_status(union filedescriptor *fd, unsigned char *co);



#ifdef __cplusplus
}
#endif


#endif
