#ifndef scan_mapping_h
#define scan_mapping_h

#include <inttypes.h>

typedef struct
{
  unsigned char ir_code;
  unsigned char scanner_code;
}
key_map_element_t;


uint8_t  map_get_scanner_code(const uint8_t command);

// sense is only 1-8, so need just 3 bits, 
// scan is 9-28, so 5 bits are needed 
#define SS(sense,scan) (((sense-1)<<5) | scan)

#define SENS(v) ((v>>5)+1)
#define SCAN(v) (v&0x1f)



#endif
