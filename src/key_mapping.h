#ifndef key_mapping_h
#define key_mapping_h

#include <inttypes.h>

typedef struct
{
  unsigned char ir_code;
  unsigned char cpld_code;
}
key_map_element_t;


uint8_t  map_get_cpld_code(const uint8_t command);

#endif
