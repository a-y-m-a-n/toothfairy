#ifndef conduit_h
#define conduit_h

#include <sys/types.h>


void conduit_set_TDI(const short v);
void conduit_set_TCK(const short v);
void conduit_set_TMS(const short v);

unsigned char conduit_get_TDO(void);




#endif
