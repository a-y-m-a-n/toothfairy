#include "soft_reset.h"



// init3 section of code to disable the watchdog timer after
// the system is reset, since the wdt stays enabled through a
// reset sequence.  Do not call this yourself
void wdt_init(void) __attribute__((naked)) __attribute__((section(".init3")));

void wdt_init(void)
{
   MCUSR = 0;
   wdt_disable();
}

