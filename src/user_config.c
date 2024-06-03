#include "user_config.h"
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include "key_defs.h"
#include "ps3toothfairy.h"
#include "soft_reset.h"

// The default configuration is lower bits of device id is
// 0x2, the default led intensity is highest and the default
// delay is 50ms between led pulses.  Store into the NV flash
// Default magic key is the angle key since it is on both old
// and new remotes. 

// The new sequence is:
// Hold PS for 3 seconds
// Wait 1 second
// Hold Down for 2 seconds
// Press Up
// Hold PS for 3 seconds
// Wait 1 second
// Press X 
// Wait 2 seconds
// Press X
// Finish

const user_config_t default_user_config PROGMEM = 
	{ 
	  {KEY_ANGLE, 0x01, 0x2, 0x00, 0x00, 0x00, 0x00, 0x01}, 0xAA ,
	  {
	    {	KEY_POWER,       { TO3S,    TO01S  }},
	    {   KEY_DOWN_1,      { TO3S,    TO01S  }},
	    {   KEY_UP_1,        { TO01S,   TO01S  }},
	    {	KEY_POWER,       { TO3S,    TO01S  }},
	    {	KEY_X,           { TO01S,   TO2S   }},      
	    {	KEY_X,           { TO01S,   TO01S  }},      
	    {	0xFF,            { 0xFF,    0xFF   }},      
	  }

	};  
        // magic key, id, bright, sensor, power, disc off

user_config_t user_config EEMEM = 
  {
    {KEY_ANGLE, 0x01, 0x2, 0x00, 0x00, 0x00, 0x00, 0x01}, 0xAA,
    {
	    {	KEY_POWER,       { TO3S,    TO01S  }},
	    {   KEY_DOWN_1,      { TO3S,    TO01S  }},
	    {   KEY_UP_1,        { TO01S,   TO01S  }},
	    {	KEY_POWER,       { TO3S,    TO01S  }},
	    {	KEY_X,           { TO01S,   TO2S   }},      
	    {	KEY_X,           { TO01S,   TO01S  }},      
	    {	0xFF,            { 0xFF,    0xFF   }},      
#if 0
      {	KEY_POWER,       { TO3S,    TO1S   }},
      { KEY_DOWN_1,      { TO3S,    TO01S  }},
      { KEY_UP_1,        { TO01S,   TO01S  }},
      {	KEY_POWER,       { TO3S,    TO1S   }},
      {	KEY_X,           { TO01S,   TO1S   }},      
      {	KEY_X,           { TO01S,   TO01S  }},      
      {	0xFF,            { 0xFF,    0xFF   }},      
#endif
    }
  };  

// Starts immediately after the rest of the commands, setting the 
// macro to 9, would begin to read this part of the memory
key_seq_t shutdown_sequence_user[24] EEMEM =
  {
    {	0xFF,             { 0xFF,    0xFF   }},      
  }
;


// 
// Takes a register and a value, returns a 0 on success or a 
// non zero value on error
//
uint8_t config_save_user_value(uint8_t reg, uint8_t val)
{
  // don't allow overwrite of the sentinel, the 1 is to allow the user
  // to trash the sentinel by changing 0 to another value and causing
  // a full refresh

  if (reg >= (1+sizeof(user_config.user_data))) return 1;

  // now check the value per the requirements of the parameter,
  // for magic key, we allow anything, for all others, the values
  // are 0:3 inclusive
  if (reg_magic_key == reg)
    {
      if (val < 0xA) return 1;
    }
  else
    {
      if (val > 0x8) return 1;
    }

  eeprom_write_byte(((uint8_t*)&user_config)+reg, val);
  return 0;

}


uint8_t config_load_user_value(uint8_t reg)
{
   return eeprom_read_byte(((uint8_t*)&user_config)+reg);
}


void config_init(void)
{
  //uint8_t sentinel = eeprom_read_byte(((uint8_t*)&user_config.eeprom_valid_sentinel));
  // Small, because the address is resolved at link time
  uint8_t sentinel = config_load_user_value(
	  (unsigned)&user_config.eeprom_valid_sentinel - (unsigned)&user_config);

  if (EEPROM_SENTINEL != sentinel)
    config_load_reset_default();
}


void config_load_reset_default(void)
{
   uint8_t val;
   uint8_t i;
   for (i=0; i<sizeof(user_config_t); i++)
   {
      val = pgm_read_byte( ((uint8_t*)&default_user_config)+i);
      eeprom_write_byte(((uint8_t*)&user_config)+i, val);
   };
}

