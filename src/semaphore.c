#include "semaphore.h"
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>


/* 
 * sem_wait
 *
 * returns non-zero if access is granted,
 * otherwise, returns 0
 */
uint8_t sem_wait(semaphore *sem)
{
  uint8_t sreg_temp = SREG;
  uint8_t result;
  cli();
  result = (*sem);
  *sem = 0;
  SREG=sreg_temp;
  sei();
  return result;
}


/*
 * sem_give
 *
 * just increments the semaphore, with
 * luck this should be an atomic instruction
 */
#if 0
void sem_give(semaphore *sem)
{
  *sem = 1;
}
#endif
