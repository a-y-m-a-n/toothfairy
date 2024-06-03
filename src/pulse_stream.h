#ifndef pulse_stream_h
#define pulse_stream_h

void pulse_stream_init(void);

void pulse_stream_add(unsigned short pulse_width);

unsigned int pulse_stream_get_code(void);

unsigned char pulse_stream_bit_count(void);

extern uint16_t pulse_stream_bits;
extern uint8_t pulse_bit_count;

#endif

