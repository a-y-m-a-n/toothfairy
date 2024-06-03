#ifndef user_config_h
#define user_config_h 

#include <inttypes.h>
#include "key_seq.h"

#define EEPROM_SENTINEL 0xAA

const typedef enum
  {
    reg_magic_key      = 0, // button 1
    reg_device_id      = 1, // button 2
    reg_led_bright     = 2, // button 3
    reg_sensor_sel     = 3, // button 4
    reg_power_track    = 4, // button 5
    reg_power_off_disc = 5, // button 6
    reg_use_magic_key  = 6, // button 7
    reg_spare_0        = 7, // button 8
  }
reg_number_t;


typedef struct user_config
{

  struct
  {
    uint8_t magic_key;      // register 0
    uint8_t device_id;      // register 1
    uint8_t led_bright;     // register 2
    uint8_t sensor_sel;     // register 3
    uint8_t power_track;    // register 4
    uint8_t power_off_disc; // register 5
    uint8_t use_magic_key;  // register 6
    uint8_t spare_0;        // register 7
  }
  user_data;

  // Flag to indicate if the value in 
  // the eeprom is valid or not, this is 0xAA since we need
  // the high bit set
   uint8_t eeprom_valid_sentinel;

  // Must set to the number of commands in the macro
  key_seq_t shutdown_sequence[7];
  
}
user_config_t;

uint8_t config_save_user_value(uint8_t reg, uint8_t val);

uint8_t config_load_user_value(uint8_t reg);

void config_load_reset_default(void);

void config_init(void);


extern user_config_t user_config;
extern key_seq_t shutdown_sequence_user[];

#endif


