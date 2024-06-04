
#ifndef ps3toothfairy_h
#define ps3toothfairy_h 

#include <avr/io.h>

void run_state_startup(void);
void shutdown_macro(void);

extern uint8_t power_state;
extern uint8_t power_track;


#define TO10S 255
#define TO5S  150
#define TO3S  100
#define TO1S  32
#define TO2S  48
#define TO01S 6 // 2,3,4 all ok, so use 3
#define TO0S  1 // Skip



#endif


