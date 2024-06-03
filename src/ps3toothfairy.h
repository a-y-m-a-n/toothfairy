
#ifndef ps3toothfairy_h
#define ps3toothfairy_h 

#include <avr/io.h>

#define F_CPU 1000000UL  // 1 MHz

void run_state_startup(void);
void shutdown_macro(void);

extern uint8_t power_state;
extern uint8_t power_track;


#define TO10S 150
#define TO5S  75
#define TO3S  50
#define TO1S  16
#define TO2S  24
#define TO01S 3 // 2,3,4 all ok, so use 3
#define TO0S  1 // Skip



#endif


