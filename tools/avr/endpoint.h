#ifndef endpoint_h
#define endpoint_h

unsigned char init_process_command(void);

// Return the cmd_out, if it is 0, then do nothing, if it has a value,
// then transmit
unsigned char process_command(unsigned char cmd);

unsigned char process_output(void);

#endif
