#include <avr/io.h>
#include "led.h"
#include "key_mapping.h"
#include "cpld_if.h"
#include <inttypes.h>
#include "hex_string.h"
#include "state_machine.h"
#include "key_defs.h"
#include "ps3toothfairy.h"
#include "user_config.h"

// Last key code may be set to 0x80 when not used since the function
// codes are all 7 bits only
#define NO_ACTIVE_KEY 0x80
#define KEY_DISCRETE_OFF 0x2F
#define KEY_DISCRETE_ON  0x2E

uint8_t active_key_code = NO_ACTIVE_KEY;
uint8_t magic_key = KEY_ANGLE;

 
void key_assert(uint8_t key_code, uint8_t device_code)
{
  // If no key is asserted, then assert, otherwise, just split
  if (0x80 != active_key_code) return;
  
  active_key_code = key_code;
  
  // If we are using power tracking then before we allow a key press, make sure that we only
  // allow the power key
  
#if 1
  if (power_track && !power_state && 
     ((active_key_code != KEY_POWER ) ||  (power_track > device_code) ))
      {
	led_blink(4);
      return;
      }
  
#endif

  
  uint8_t event;
  
  if (magic_key == active_key_code)
    event = EV_MAGIC_PRS;
  else if (KEY_DISCRETE_OFF == active_key_code)
    event = EV_DISC_OFF;
  else
    event = EV_KEY_PRS;
  
  state_machine_process(event);
}



void key_deassert(void)
{
  if (NO_ACTIVE_KEY == active_key_code) return;


  // TODO, possibly save space by canning the rls event for magic and treating the
  // release event as any other release event
  state_machine_process( (magic_key == active_key_code) ? EV_MAGIC_RLS : EV_KEY_RLS);

  active_key_code = NO_ACTIVE_KEY;
}
