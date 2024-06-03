#ifndef key_h
#define key_h

extern uint8_t active_key_code;

void key_assert(uint8_t key_code, uint8_t dev_code);

void key_deassert(void);

extern uint8_t magic_key;

#endif
