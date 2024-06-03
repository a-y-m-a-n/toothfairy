#ifndef state_machine_h
#define state_machine_h

#include "key_defs.h"

typedef enum
{ 
   EV_NONE  = 0,
   EV_MAGIC_PRS = 1,
   EV_MAGIC_RLS = 2,
   EV_KEY_PRS   = 3,
   EV_KEY_RLS   = 2,
   EV_TIMEOUT   = 5,
   EV_TICK_64MS = 6,
   EV_DISC_ON   = 7,
   EV_DISC_OFF  = 8,
}
event_t;

#define MAGIC_KEY KEY_MAGIC_DEFAULT


void state_machine_process(event_t ev);

#endif
