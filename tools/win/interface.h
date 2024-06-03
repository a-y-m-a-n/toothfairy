#ifndef interface_h
#define interface_h

int interface_open(void);

int interface_close(void);

int interface_tx(unsigned char ci);

int interface_txrx(unsigned char ci, unsigned char *co);

int interface_status(unsigned char *co);

#endif
