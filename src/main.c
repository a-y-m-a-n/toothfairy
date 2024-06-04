#include "ps3toothfairy.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "pulse_stream.h"
#include "semaphore.h"
#include "key.h"
#include "led.h"
#include "state_machine.h"
#include <avr/wdt.h>
#include <avr/pgmspace.h>
#include "soft_reset.h"
#include "user_config.h"
#include "key_defs.h"
#include "scanner.h"

#ifdef __DEBUG__
#include "hex_string.h"
#endif

// Turn on this define to output a wave when data is found on the
// PORTD1 bit
#define SCOPE_TRIGGER 1

void timer0_rearm(void);
void process_data(void);


semaphore sem_data_ready;
semaphore sem_tick_64;

// This semaphore is triggered whenever the interrupt detects a pulse
// so that the main routine can pass the pulse to the bottom handler
semaphore sem_pulse;

// low order bits of the device id, should be 0x2 for all sony devices
// that this unit supports
uint8_t user_device_id_lo = 0x2;

uint8_t power_state = 2;  // 2=config, 1=on, 0=off,
uint8_t power_track;


// Timer 0 is never altered and results in a tick every 64ms
// based on the internal RC oscillator.
ISR(TIMER0_OVF_vect)
{
  sem_give(&sem_tick_64);
}

ISR(TIMER2_OVF_vect)
{
}

ISR(TIMER0_COMPA_vect)
{
  // rearm for 10ms
  timer0_rearm();
  // process any pulses since we have not seen one for a while, 
  // reset capture logic
  sem_give(&sem_data_ready);
}

ISR(TIMER2_COMPA_vect)
{
   // disable my interrupt and reenable the 
   // capture interrupt
   TIMSK2&=~(1<<OCIE2A);
   TIFR1 |= (1<<ICF1);
   TIMSK1 |= (1<<ICIE1);
#ifdef SCOPE_TRIGGER
   PORTD |= (1<<PORTD0);
#endif
}

unsigned char pulse_time;

ISR(TIMER1_CAPT_vect)
{
  // TODO This should probably be done atomically
  unsigned char lb = ICR1L;
  unsigned char hb = ICR1H;
  unsigned char edge_time = hb<<6 | lb>>2;

  // Setup deglitch
  TIMSK1 &= ~(1<<ICIE1);
  OCR2A = TCNT2 + 0x30;
  TIMSK2 |= (1<<OCIE2A);
  TIFR2 |= (1<<OCF2A);
#ifdef SCOPE_TRIGGER
  PORTD &= ~(1<<PORTD0);
#endif
  
   
  // if rising edge
  if (TCCR1B & (1<<ICES1))
    {

      //led_off();
      
      // determine pulse time, verify if this is correct even, the
      // value is 8 bits, so there is no need to truncate or mask
      pulse_time = (edge_time - pulse_time);
#ifdef SCOPE_TRIGGER
      PORTD &= ~(1<<PORTD1);  // scope trigger
#endif

      // process pulse
      sem_give(&sem_pulse);
    }
  else // falling edge
    {

      pulse_time = edge_time;

#ifdef SCOPE_TRIGGER
      PORTD |= (1<<PORTD1);
#endif
    }

  
  // reset our expiration timer
  timer0_rearm();

  // toggle edge polarity
  TCCR1B ^= (1<<ICES1);
  
}


void timer0_rearm(void)
{
  // add about 10ms based on a divider of 256
  OCR0A = TCNT0 + 80;
  // clear any pending interrupt
  TIFR0 |= (1<<OCF0A);

  // NOTE: This code is duped in the init to save time
}



// Initialization routines for the timer and input capture circuit

void timer0_init(void)
{
  TCCR0A = 0;
  TCCR0B = (1<<CS02)|(1<<CS00); // div 256 -- 0.25ms period

  // Instead of calling timer0_rearm, just duplicate the
  // code for the reset of the counter and interrupt ack.
  OCR0A = TCNT0 + 80;

  // Enable the output compare a and the overflow
  TIMSK0 |= (1<<OCIE0A) | (1<<TOIE0); 

  // clear any spurious captures that already occurred
//  TIFR0 |= (1<<OCF0A);
}


void icp_init(void)
{
  pulse_time = 0;

  // Set to 10 bit pwm mode, at 8us -> 8ms total
  // Requires wgm[12:10]=1;, also setup the LED bits here
  TCCR1A |= (1<<WGM10)|(1<<WGM11)|   _BV(COM1A1)|_BV(COM1A0);
  // Noise cancel, falling edge, scale by 8, 8us period
  TCCR1B |= (1<<ICNC1)|(1<<CS11)|(1<<CS10)|(1<<WGM12);

  TIMSK1 |= (1<<ICIE1);

  //AME Temporary only, redo this with the Analog comparator
  // This will enable the bandgap, but still allow the control
  // of the pin PB0
  //DIDR = (1<<AIN1D);
  //ACSR = (1<<ACBG) | (1<<ACIC);  // THIS AND THE ONE AFTER IT ...
  //PORTB |= (1<<PORTB0);

}

void timer2_init(void)
{
   // based on 8MHz, clock, divide down to 1us period
   TCCR2A = 0;
   TCCR2B |= (1<<CS21);
   // we enable the interrupt later
   TIMSK2 |= (1<<TOIE2);
}

#ifdef __DEBUG__

void uart_init(void)
{
  // UBRR=12, U2X=1
  UBRRH = 0;
  UBRRL = 12;
  UCSRA = (1<<U2X);
  UCSRB = (1<<RXEN)|(1<<TXEN);
  UCSRC = (1<<UCSZ1)|(1<<UCSZ0);
}

#endif

// End of hardware init code


// Resets the capture setup by initializing the pulse measurement
// and classifier, the capture logic, and then resets the expiry
// timer.
void process_reset(void)
{
  pulse_stream_init();
  icp_init();
  timer0_rearm();
}



#ifdef __DEBUG__

static void dump_code(unsigned code)
{
  int k;
  char hex[9];
  hex_string(code,hex);
  hex[8]='\r';
  for (k=0; k<=9; k++)
    {
      while (! (UCSRA & (1<<UDRE)));
      UDR = hex[k];
    }
}
#endif




void process_data(void)
{
// Keeps count of the number of 10ms ticks we have seen with
// no activity.

  static unsigned char null_periods;

  // Note that to conserve space and instructions, the device_id is computed
  // in an odd way.  The normal device code is ADXX or 9DXX.  This is the 
  // value that is returned from the pulse_stream_get_code method.  First
  // the code is bitshifted by 7 and turned into an 8 bit value, resulting
  // in 5A or 3A in the normal case.  The high bit of the 16 bit word is
  // set, so that is the check against 0x8000.  Instead of comparing against
  // 5A or 3A, we compare against 58 or 38 and then or that value with the
  // user selected device Id.
  // 
  // As a result, the mapping from genuine to modified device code is:
  // ACXX, 9CXX => 5AXX, 3AXX => 0x38 | 0x0    
  // ADXX, 9DXX => 5AXX, 3AXX => 0x38 | 0x2   * Default
  // AEXX, 9EXX => 5CXX, 3CXX => 0x38 | 0x4   
  // AFXX, 9FXX => 5EXX, 3EXX => 0x38 | 0x6
  // 
  // Timing of Signals
  //
  // Code starts transmitting at 0, each bit received, resets the timeout.
  // After 10ms of inactivity, the process is started (time for full code
  // which is 45ms after the user pressed) (55ms total)
  // Now wait for 3 null periods (30ms, 85ms total)
  // Release the key
  // So the minimum time is about 85ms, if the key is not held down

  
  //unsigned char count = pulse_stream_bit_count();


   // we enable the interrupt later
  // To reduce code size, we will access the pulse_bit_count and pulse_stream_bits
  // directly. AME
  if (pulse_bit_count>=10)
    {
      uint16_t code = pulse_stream_bits;
      
      uint8_t  deviceid = code >> 7;
      
      // Check Device Id, sony cd player = 0x11, 0x38, ps2 remote = 13A, 0x58 
      // this code is ugly but much tighter than what was here before.
      if ((code&0x8000)  && ((deviceid == (0x38|user_device_id_lo)) || 
			     (deviceid == (0x58|user_device_id_lo))))  
	{
	  // Need to assert a key press
	  key_assert(code & 0x7F, deviceid>>5);
	  
	  null_periods = 0;	  
	} 
    }
  
  // not a valid or no code, does not match device id, previously, this had been
  // a count of 80ms but to slow, now it is about 50ms which is more than enough
  // for the codes
  else if (null_periods++ > 2)
    {
      null_periods = 0;
      key_deassert();
    }
  
  // Reset the capture logic and capture engine
  process_reset();
    
}


// run state startup is called back from the state machine when
// it enters the run macro state.  This will allow the system to
// apply the appropriate configuration data.
void run_state_startup(void)
{
  // This uses way less space then allocating on the stack ... but
  // the shifts are very space intensive, can allow reprogramming of
  // the blink rate and intensity
  led_init(config_load_user_value(reg_led_bright)<<5);

  magic_key = !config_load_user_value(reg_use_magic_key) ? 
     config_load_user_value(reg_magic_key) :  0xFF;

  user_device_id_lo = config_load_user_value(reg_device_id)<<1;
 
  // Activate wired sensor if we read a non-zero value for this
  // register location 
  if (config_load_user_value(reg_sensor_sel))
  {
     ACSR = (1<<ACBG) | (1<<ACIC); 

	 // for the analog in to work, we need to enabled the internal
	 // pullup to provide the termination.
     PORTD |= (1<<PORTD7);  
  }
  
  // Are we going to track the power status of the user?
  power_track = config_load_user_value(reg_power_track);

  // Default to power off
  power_state = 0;
  
}

int main(void)
{
#if 1
CLKPR = (1 << CLKPCE);
CLKPR = 0;
#endif

  led_init(0xFF);
  scanner_init();

#ifdef SCOPE_TRIGGER
  DDRD |= (1<<PORTD1) | (1<<PORTD0);
#endif

#ifdef __DEBUG__
  uart_init();
#endif

  timer0_init();
  timer2_init();  // deglitching timer

  // reset and init the configuration data
  config_init();

  // The actual data capture process
  process_reset();
 
  // setup scanning interrupts
#if 1
  EICRA  |= (1<<ISC10) | (1<<ISC00);
  EIMSK  |= (1<<INT1) | (1<<INT0);
#endif
  

  for(;;)
    {
      if(sem_wait(&sem_pulse))
	{
	  pulse_stream_add(pulse_time);
	}
      
      if(sem_wait(&sem_data_ready))
	{
	  process_data();
	}

      if (sem_wait(&sem_tick_64))
	{
	  state_machine_process(EV_TICK_64MS);
	}

    }

  return 0;
}


