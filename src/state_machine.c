#include "ps3toothfairy.h"
#include <inttypes.h>
#include "state_machine.h"
#include "led.h"
#include "key.h"
#include "scanner.h"
#include "scan_mapping.h"
#include "user_config.h"
#include "soft_reset.h"
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include "key_seq.h"

// allow 0-8 for the values of the registers based on the 
// sony key codes.
#define MAX_REG_NUM 9

//#define scanner_command(x) ;


typedef struct state_output
{
  struct state_output (*next)(event_t);
}
state_t;

// the timeout requested by the current state
static uint8_t timeout;
static uint8_t selected_reg;
static volatile uint8_t sub_state;

state_t state_run_start(event_t ev);
state_t state_run(event_t ev);
state_t state_run_key_prs(event_t ev);
state_t state_run_hold_key_prs(event_t ev);
state_t state_run_magic_prs(event_t ev);
state_t state_run_magic_rls(event_t ev);
state_t state_run_shutdown(event_t ev);
state_t state_run_shutdown_macro(event_t ev);

state_t state_prog_start(event_t ev);
state_t state_prog_wait(event_t ev);
state_t state_prog_magic_prs_1(event_t ev);
state_t state_prog_magic_rls_1(event_t ev);
state_t state_prog_magic_prs_2(event_t ev);
state_t state_prog_ready(event_t ev);
state_t state_prog_reg_sel_prs(event_t ev);
state_t state_prog_reg_sel_rls(event_t ev);
state_t state_prog_val_sel_prs(event_t ev);
state_t state_prog_macro_save(event_t ev);

state_t state_wait_reset(event_t ev);

// First state entered on boot up
state_t state_prog_start(event_t ev)
{
  state_t state;
  state.next = state_prog_wait;
  timeout  = TO10S;
  led_on();
  return state;
}



state_t state_prog_wait(event_t ev)
{
  state_t state;
  switch(ev)
    {
      // TRIM Able to trim code here by getting rid of the quick
      // escape to run mode  Otherwise, on timeout we go to run more
      // or if the user presses anything other than magic
    case EV_TIMEOUT:
    case EV_KEY_PRS:
      {
	state.next = state_run_start;
      }
      break;

    case EV_MAGIC_PRS:
      {
	led_blink(1);
	timeout  = TO5S;
	state.next = state_prog_magic_prs_1;
      }
      break;

    default:
      {
	state.next = state_prog_wait;
      }
    }
  return state;
}


state_t state_prog_magic_prs_1(event_t ev)
{
  state_t state;
  switch(ev)
    {

    case EV_TIMEOUT:
      {
	// if they are still holding after so many seconds, then 
	// restore the defaults
        config_load_reset_default();
        led_off();
	state.next = state_wait_reset;
	  
      }
      break;
      
    case EV_MAGIC_RLS:
      {
	state.next = state_prog_ready;
      }
      break;
      
    default:
      state.next = state_prog_magic_prs_1;
    }

  return state;
}


state_t state_wait_reset(event_t ev)
{
   state_t state;
   // once the magic key is released, then go ahead
   // and reboot the system
   if(EV_MAGIC_RLS == ev)
      soft_reset();
   state.next = state_wait_reset;
   return state;
}




state_t state_prog_ready(event_t ev)
{
  state_t state;
  switch(ev)
    {
  
    case EV_KEY_PRS:
    {
       if (active_key_code <= MAX_REG_NUM)
       {
          led_blink(1); 
          selected_reg = active_key_code;
	  state.next = state_prog_reg_sel_prs; 
       }
       else
       {
          led_blink(2);
	  timeout = TO10S;
          state.next = state_prog_ready;
       }
    }
    break;

    // If they press magic again or timeout, the go to 
    // run state.  
    // TRIM could get rid of the press again
    case EV_MAGIC_PRS:
    case EV_TIMEOUT:
      {
	// no action, so go ahead and run the program
	state.next = state_run_start;
      }
      break;
      
    default:
      state.next = state_prog_ready;
    }
  return state;
}


state_t state_prog_macro_save(event_t ev)
{
  // selected_reg is now the counter for which eeprom location
  // we are updating
  timeout = TO10S;
  uint8_t count=1;
  uint8_t key = active_key_code;
  state_t state;
  state.next = state_prog_macro_save;

  switch(ev)
    {
      
    case EV_TIMEOUT:
      count=3;
      key = 0xFF;
      state.next = state_prog_ready;
      //led_blink(2);
      
    case EV_KEY_PRS:
      // For each key press, we need to save the key
      // value into the next eeprom location and finally
      // after a 10S timeout, then we commit the final
      // sentinel and move to the next state; make sure
      // the value does not exceed 78 before writing
      do
	{
	  eeprom_write_byte(((uint8_t*)(&shutdown_sequence_user[0]))+sub_state,
			    key);
	  sub_state++;
	}
      while(--count);
      led_blink(1);

    default:
      break;
    }
  return state;

}



state_t state_prog_reg_sel_prs(event_t ev)
{
   state_t state;

   if (EV_KEY_RLS == ev)
   {
      timeout = TO5S;
      sub_state =0;
      state.next = (selected_reg != 9 ? 
		    state_prog_reg_sel_rls : 
		    state_prog_macro_save);
   }
   else
     {
       state.next = state_prog_reg_sel_prs;
     }
   return state;
}


state_t state_prog_reg_sel_rls(event_t ev)
{
   state_t state;
   switch(ev)
   {
   case EV_TIMEOUT:
   {
      led_blink(2);
      timeout=TO10S;
      state.next = state_prog_ready;
   }
   break;

   case EV_MAGIC_PRS:
   case EV_KEY_PRS:
     {
       // The sony keys map 1-9,0 to the hex values 0h,9h, so it is
       // convenient for direct implementation here.
       led_blink(1+config_save_user_value(selected_reg, active_key_code));
#if 0
       if (!config_save_user_value(selected_reg, active_key_code))
	 {
	   led_blink(1); 
	 }
       else
	 {
	   led_blink(2);
	 }
#endif
       // need to wait for a key release in either case 
       state.next = state_prog_val_sel_prs;
     }
     break;

   default:
     state.next = state_prog_reg_sel_rls;
   }
   return state;
}

state_t state_prog_val_sel_prs(event_t ev)
{
   state_t state;
   if (EV_KEY_RLS == ev)
   {
      timeout = TO10S;
      state.next = state_prog_ready;
   }
   else
   {
      state.next = state_prog_val_sel_prs;
   }
   return state;
}


state_t state_run_start(event_t ev)
{
  state_t state;
  state.next = state_run;
  led_off();
  //led_blink(3);

  // load the users configuration from eeprom
  run_state_startup();

  return state;
}


// Running states

state_t state_run(event_t ev)
{
  state_t state;
  state.next = state_run;

  uint8_t cmd;
  
 switch(ev)
    {  
    case EV_KEY_PRS:
      // If pressed, then send the command to the scanner and
      // illuminate the led, 
      cmd = map_get_scanner_code(active_key_code);
      scanner_command(cmd);
      state.next = state_run_key_prs;
      led_on();
      power_state = 1;
      break;
      
    case EV_DISC_OFF:
      led_on();
      // choose the proper start point for the macro, simply take the value
      // from the user and mult by 2.  A value of 0 results in the 6 step
      // macro, 3->3 step macro, 6->no macro
      sub_state = config_load_user_value(reg_power_off_disc)<<1;
      timeout = TO01S;
      state.next = state_run_shutdown_macro;
      break;
      
    case EV_MAGIC_PRS:
      // if magic pressed then blink once, then on and move
      // to the magic pressed state
      led_blink(2);
      led_on();
      timeout = TO5S;
      state.next = state_run_magic_prs;
      break;
      
    case EV_TIMEOUT:
      // We were set to auto reset by the transitory state 
      // because a key was held;
      scanner_reset();
      led_off();
      break;


    default:
      // no action, stay in same state
      break;
    }

  return state;
}


#if 0
state_t state_run_shutdown(event_t ev)
{
  power_state = 0;
  state_t state;
#if 0
  if (EV_TIMEOUT == ev)// return state;
    {
      timeout = TO01S + config_load_user_value(reg_key_delay);
#if 1      
      switch(sub_state)
	{
	case 6:
	  timeout = TO3S;

	case 0:
	  cpld_command(CPLD_POWER_RESET);
	  break;
	  
	case 2: case 4:
	  cpld_command(CPLD_DOWN);
	  break;
	  
	case 8: case 10: case 12:
	  cpld_command(CPLD_X);
	  break;
	  
	  // When 14 is reached, the sequence is complete .. originally a
          // safety value here, but no need
	case 14:
	  led_off();
	  state.next = state_run;
          return state;
     
        case 16:
          state.next = state_run_shutdown_macro;

        case 1: // release the key with the default but leave the timeout
                // long enough to switch the screen
          timeout = TO2S;

	default:
	  cpld_reset();
	}
#endif
      sub_state++;
    }
#endif

          state.next = state_run_shutdown_macro;
  state.next = state_run_shutdown;
  return state;
}
#endif

state_t state_run_key_prs(event_t ev)
{
  state_t state;

  // Key released, so turn off led and clear cpld register
  // TRIM, make events the same so they don't conflict
  if (ev == EV_KEY_RLS || ev == EV_MAGIC_RLS)
    {
      led_off();
      scanner_reset();
      state.next = state_run;
    }
  else
    {
      state.next = state_run_key_prs;
    }
  return state;
}



state_t state_run_magic_prs(event_t ev)
{
  state_t state;
  // Key released, so turn off led and clear cpld register
  switch(ev)
    {
    case EV_MAGIC_RLS:
      led_off();
      timeout = TO5S;
      state.next = state_run_magic_rls;
      break;

    case EV_TIMEOUT:
      led_off();
      state.next = state_wait_reset;
      break;

    default:
      state.next = state_run_magic_prs;
    }
  
  return state;
}



state_t state_run_magic_rls(event_t ev)
{
  state_t state;
  uint8_t cmd;

  switch(ev)
    {
    // Waits for 3s for another key and then transmit last choice
    case EV_TIMEOUT:
      led_blink(2);
      cmd = map_get_scanner_code(active_key_code);
      scanner_command(cmd);
      scanner_reset();
      state.next = state_run;
      break;

      // Magic pressed a 2nd time so go ahead and send the code and 
      // then wait for a key_release_event.  We will use the standard
      // state that waits for the release event to move forward
    case EV_MAGIC_PRS:
      led_on();
      cmd = map_get_scanner_code(active_key_code);
      scanner_command(cmd);
      state.next = state_run_key_prs;
      break;

    case EV_KEY_PRS:
      // if another key is pressed, then press it and hold the key
      // but do not release in the next state
      led_on();
      led_blink(1);
      cmd = map_get_scanner_code(active_key_code);
      scanner_command(cmd);
      state.next = state_run_hold_key_prs;
      break;

    default:
      state.next = state_run_magic_rls;
    }

  return state;
}


state_t state_run_hold_key_prs(event_t ev)
{
  state_t state;
  switch(ev)
    {
    case EV_KEY_RLS:
      // Wait for 5s for the user to press another key, otherwise,
      // cause the state machine to reset to the start state
      timeout = TO5S;
      state.next = state_run;
      break;

    default:
      state.next = state_run_hold_key_prs;
    } 
  return state;
 
}



state_t state_run_shutdown_macro(event_t ev)
{
  power_state = 0;

  state_t state; 

  state.next = state_run_shutdown_macro; 

  if (EV_TIMEOUT == ev)
    {
      static key_seq_t key_seq;

      uint8_t idx = (sub_state>>1);
  
      key_seq_t *key_seq_p = &(user_config.shutdown_sequence[0]);



      // Move through the instruction array and get the ones that we want
      uint8_t k;

      for (k=0;k<idx;k++) { key_seq_p++; }  
      for (k=0;k<3;k++)
	{
	  *(((uint8_t*)&key_seq)+k) = eeprom_read_byte(((uint8_t*)key_seq_p)+k);
	}
  

      // determine what phase of the operation we are in.  if we are in phase
      // 0, then we are pressing a key, if we are in phase 1, we are releasing
      // (if timeout > 1) and waiting for timeout.
      uint8_t phase = sub_state & 0x1;

      // We allow up to a 24 step macro + 7 step built in macro
      if ((0xFF == key_seq.key) || (idx>=31))
	{
	  // Done with the sequence
	  led_off();
	  state.next = state_run;
	  return state;
	}

      uint8_t val = key_seq.to[phase];

      // Set the timeout so the FSM is triggered again shortly, each tick
      // is about 64 ms.
      timeout = val;
  
      if (phase) // release key phase
	{
	  // If timeout is greater than 1, then release the key
	  if (val>1) { scanner_reset(); } 
	}
      else       // press key phase
	{
      
	  scanner_command(map_get_scanner_code(key_seq.key));
	}

      sub_state++;
    }
  return state;
}



/*
 * Main state processing routine
 */

void state_machine_process(event_t ev)
{
   static state_t state = { state_prog_start };
   
   // If the timeout is set and there is a tick, then 
   // decrement the timeout counter.  If we hit 0, then
   // a timeout has occurred, so change the event and 
   // process it.  Otherwise, change the event to none
   // and return;
   if (EV_TICK_64MS == ev)
     {
       if (timeout)
	 {
	   timeout-- ;
	   if (!timeout)
	     {
	       ev = EV_TIMEOUT;
	     }
	   else
	     {
	       return;
	     }
	 }
     }
   
   state = state.next(ev);
}



