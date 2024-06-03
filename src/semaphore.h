#ifndef semaphore_h
#define semaphore_h

#include <inttypes.h>

typedef uint8_t semaphore;

uint8_t sem_wait(semaphore *s);


extern inline void sem_give(semaphore *s) 
{
  *s=0x01;
}

extern inline void sem_init(semaphore *s,
			    const uint8_t initial) 
{
  *s=initial;
}

#endif
