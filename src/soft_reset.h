#ifndef soft_reset_h
#define soft_reset_h


#include <avr/wdt.h>


static inline void soft_reset(void) { wdt_enable(WDTO_15MS); while(1); }



#endif

