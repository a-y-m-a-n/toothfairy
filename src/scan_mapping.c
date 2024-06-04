/************************************************************************************
 *
 * Revisions:
 * 1) Eliminated the duplicate codes for Up / Down / Left / Right, since they can 
 * conflict with other function IDs, particularly the Display Key
 */

#include "scan_mapping.h"
#include <inttypes.h>
#include <avr/pgmspace.h>


/*
 * 2 Device Codes for the PS2, the first is for DVD which is device 13A and has
 * codes that are 9DXX.  The other is for PS2 and is 15A, mapping to ADXX
 */

// Maps Key Code for IR (left) to CPLD Code (right).  The CPLD has the data
// bus mapped to D0/D2/D4 on the REVB PCB, so we store the data at 0,2,4 and
// the line select is at bits 1,3,5.  This is more confusing here, but easier
// to map to the hardware
static const key_map_element_t key_map[] PROGMEM = 
  {
    // Sense 1
     // Sense/Input 0x01
    {0x16,SS(1,9)}, // Eject    <UNVERIFIED> 
     // Sense/Input 0x02
    {0x00,SS(1,10)}, // 1 
     // Sense/Input 0x03
    {0x01,SS(1,11)}, // 2 
     // Sense/Input 0x04
    {0x02,SS(1,12)}, // 3 
     // Sense/Input 0x05
    {0x64,SS(1,13)}, // Audio 
     // Sense/Input 0x06
    {0x5C,SS(1,14)}, // Triangle 
     // Sense/Input 0x07
    //{0x54,0x15}, // Up    <2 CODES> 
     // Sense/Input 0x07
    {0x79,SS(1,22)}, // Up    <2 CODES> 

    // Sense 2
     // Sense/Input 0x11
    {0x03,SS(2,9)}, // 4 
     // Sense/Input 0x12
    {0x04,SS(2,10)}, // 5 
     // Sense/Input 0x13
    {0x05,SS(2,11)}, // 6 
     // Sense/Input 0x14
    {0x65,SS(2,12)}, // Angle 
     // Sense/Input 0x15
    {0x06,SS(2,13)}, // 7 
     // Sense/Input 0x16
    {0x5D,SS(2,15)}, // Circle 
     // Sense/Input 0x17
    //{0x57,0x17}, // Left  <2 CODES> 
     // Sense/Input 0x17
    {0x7B,SS(2,23)}, // Left  <2 CODES> 

    // Sense 3
     // Sense/Input 0x21
    {0x07,SS(3,9)}, // 8 
     // Sense/Input 0x22
    {0x08,SS(3,10)}, // 9 
     // Sense/Input 0x23
    {0x63,SS(3,11)}, // Subtitle 
     // Sense/Input 0x24
    {0x0F,SS(3,12)}, // Clear 
     // Sense/Input 0x25
    {0x09,SS(3,13)}, // 0 
     // Sense/Input 0x26
    {0x0B,SS(3,16)}, // Enter 
     // Sense/Input 0x27
    //{0x55,0x1D}, // Right  <2 CODES> 
     // Sense/Input 0x27
    {0x7C,SS(3,24)}, // Right  <2 CODES> 

    // Sense 4
     // Sense/Input 0x31
    {0x28,SS(4,9)}, // Time    
     // Sense/Input 0x32
    {0x35,SS(4,10)}, // Shuffle - Map to RED 
     // Sense/Input 0x33
    {0x1F,SS(4,11)}, // Program - Map to GREEN 
     // Sense/Input 0x34
    {0x2C,SS(4,12)}, // Repeat  - Map to BLUE 
     // Sense/Input 0x35
    {0x2A,SS(4,13)}, // A_B     - Map to YELLOW 
     // Sense/Input 0x36
    {0x5F,SS(4,17)}, // Square <TRANSMIT ON RLSE ON VERSION 1 PS2 REMOTE> 
     // Sense/Input 0x37
    //{0x56,0x1F}, // Down  <2 CODES> 
     // Sense/Input 0x37
    {0x7A,SS(4,25)}, // Down  <2 CODES> 

    // Sense 5 
     // Sense/Input 0x41
    {0x54,SS(5,9)}, // Display 
     // Sense/Input 0x42
    {0x1B,SS(5,10)}, // DVD_Menu 
     // Sense/Input 0x43
    {0x1A,SS(5,11)}, // Title    <UNVERIFIED - TOP MENU>  
     // Sense/Input 0x44
    {0x0E,SS(5,12)}, // Return 
     // Sense/Input 0x45
    {0x33,SS(5,13)}, // Scan< 
     // Sense/Input 0x46
    {0x5E,SS(5,18)}, // X 
     // Sense/Input 0x47
    {0x5A,SS(5,26)}, // L1 
    
    // Sense 6
     // Sense/Input 0x51
    {0x32,SS(6,9)}, // Play 
     // Sense/Input 0x52
    {0x34,SS(6,10)}, // Scan> 
     // Sense/Input 0x53
    {0x51,SS(6,11)}, // L3 
     // Sense/Input 0x54
    {0x30,SS(6,12)}, // Prev 
     // Sense/Input 0x55
    {0x38,SS(6,13)}, // Stop 
     // Sense/Input 0x56
    {0x15,SS(6,19)}, // Power_Reset <UNVERIFIED> 
     // Sense/Input 0x57
    {0x5B,SS(6,27)}, // R1 

    // Sense 7
     // Sense/Input 0x61
    {0x31,SS(7,9)}, // Next 
     // Sense/Input 0x62
    {0x60,SS(7,10)}, // Slow< 
     // Sense/Input 0x63
    {0x39,SS(7,11)}, // Pause 
     // Sense/Input 0x64
    {0x50,SS(7,12)}, // Select 
     // Sense/Input 0x65
    {0x61,SS(7,13)}, // Slow> 
     // Sense/Input 0x66
    {0x58,SS(7,20)}, // L2 
     // Sense/Input 0x67
    {0x59,SS(7,28)}, // R2 

     // Sense/Input 0x00
    // {0x2E,0x00}, // PowerOn   <UNVERIFIED> 
     // Sense/Input 0x00
    // {0x2F,0x00}, // PowerOff  <UNVERIFIED> 
    // Sense 8
     // Sense/Input 0x71
    {0x52,SS(8,13)}, // R3 
     // Sense/Input 0x72
    {0x53,SS(8,21)}, // Start 


  };




uint8_t  map_get_scanner_code(const uint8_t command)
{
  uint8_t i;
  for (i=0; i<sizeof(key_map)/sizeof(key_map_element_t); i++)
    {
      uint8_t ir_cmd = pgm_read_byte(&(key_map[i].ir_code));
      if (ir_cmd == command)
	return pgm_read_byte(&(key_map[i].scanner_code));
    }
  return 0;
}
	
