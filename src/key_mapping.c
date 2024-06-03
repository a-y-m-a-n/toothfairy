/************************************************************************************
 *
 * Revisions:
 * 1) Eliminated the duplicate codes for Up / Down / Left / Right, since they can 
 * conflict with other function IDs, particularly the Display Key
 */

#include "key_mapping.h"
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
const key_map_element_t key_map[] PROGMEM = 
  {
    // Sense 1
     // Sense/Input 0x01
    {0x16,0x01}, // Eject    <UNVERIFIED> 
     // Sense/Input 0x02
    {0x00,0x04}, // 1 
     // Sense/Input 0x03
    {0x01,0x05}, // 2 
     // Sense/Input 0x04
    {0x02,0x10}, // 3 
     // Sense/Input 0x05
    {0x64,0x11}, // Audio 
     // Sense/Input 0x06
    {0x5C,0x14}, // Triangle 
     // Sense/Input 0x07
    //{0x54,0x15}, // Up    <2 CODES> 
     // Sense/Input 0x07
    {0x79,0x15}, // Up    <2 CODES> 

    // Sense 2
     // Sense/Input 0x11
    {0x03,0x03}, // 4 
     // Sense/Input 0x12
    {0x04,0x06}, // 5 
     // Sense/Input 0x13
    {0x05,0x07}, // 6 
     // Sense/Input 0x14
    {0x65,0x12}, // Angle 
     // Sense/Input 0x15
    {0x06,0x13}, // 7 
     // Sense/Input 0x16
    {0x5D,0x16}, // Circle 
     // Sense/Input 0x17
    //{0x57,0x17}, // Left  <2 CODES> 
     // Sense/Input 0x17
    {0x7B,0x17}, // Left  <2 CODES> 

    // Sense 3
     // Sense/Input 0x21
    {0x07,0x09}, // 8 
     // Sense/Input 0x22
    {0x08,0x0C}, // 9 
     // Sense/Input 0x23
    {0x63,0x0D}, // Subtitle 
     // Sense/Input 0x24
    {0x0F,0x18}, // Clear 
     // Sense/Input 0x25
    {0x09,0x19}, // 0 
     // Sense/Input 0x26
    {0x0B,0x1C}, // Enter 
     // Sense/Input 0x27
    //{0x55,0x1D}, // Right  <2 CODES> 
     // Sense/Input 0x27
    {0x7C,0x1D}, // Right  <2 CODES> 

    // Sense 4
     // Sense/Input 0x31
    {0x28,0x0B}, // Time    
     // Sense/Input 0x32
    {0x35,0x0E}, // Shuffle - Map to RED 
     // Sense/Input 0x33
    {0x1F,0x0F}, // Program - Map to GREEN 
     // Sense/Input 0x34
    {0x2C,0x1A}, // Repeat  - Map to BLUE 
     // Sense/Input 0x35
    {0x2A,0x1B}, // A_B     - Map to YELLOW 
     // Sense/Input 0x36
    {0x5F,0x1E}, // Square <TRANSMIT ON RLSE ON VERSION 1 PS2 REMOTE> 
     // Sense/Input 0x37
    //{0x56,0x1F}, // Down  <2 CODES> 
     // Sense/Input 0x37
    {0x7A,0x1F}, // Down  <2 CODES> 

    // Sense 5 
     // Sense/Input 0x41
    {0x54,0x21}, // Display 
     // Sense/Input 0x42
    {0x1B,0x24}, // DVD_Menu 
     // Sense/Input 0x43
    {0x1A,0x25}, // Title    <UNVERIFIED - TOP MENU>  
     // Sense/Input 0x44
    {0x0E,0x30}, // Return 
     // Sense/Input 0x45
    {0x33,0x31}, // Scan< 
     // Sense/Input 0x46
    {0x5E,0x34}, // X 
     // Sense/Input 0x47
    {0x5A,0x35}, // L1 
    
    // Sense 6
     // Sense/Input 0x51
    {0x32,0x23}, // Play 
     // Sense/Input 0x52
    {0x34,0x26}, // Scan> 
     // Sense/Input 0x53
    {0x51,0x27}, // L3 
     // Sense/Input 0x54
    {0x30,0x32}, // Prev 
     // Sense/Input 0x55
    {0x38,0x33}, // Stop 
     // Sense/Input 0x56
    {0x15,0x36}, // Power_Reset <UNVERIFIED> 
     // Sense/Input 0x57
    {0x5B,0x37}, // R1 

    // Sense 7
     // Sense/Input 0x61
    {0x31,0x29}, // Next 
     // Sense/Input 0x62
    {0x60,0x2C}, // Slow< 
     // Sense/Input 0x63
    {0x39,0x2D}, // Pause 
     // Sense/Input 0x64
    {0x50,0x38}, // Select 
     // Sense/Input 0x65
    {0x61,0x39}, // Slow> 
     // Sense/Input 0x66
    {0x58,0x3C}, // L2 
     // Sense/Input 0x67
    {0x59,0x3D}, // R2 

     // Sense/Input 0x00
    // {0x2E,0x00}, // PowerOn   <UNVERIFIED> 
     // Sense/Input 0x00
    // {0x2F,0x00}, // PowerOff  <UNVERIFIED> 
    // Sense 8
     // Sense/Input 0x71
    {0x52,0x2B}, // R3 
     // Sense/Input 0x72
    {0x53,0x2E}, // Start 


  };




uint8_t  map_get_cpld_code(const uint8_t command)
{
  uint8_t i;
  for (i=0; i<sizeof(key_map)/sizeof(key_map_element_t); i++)
    {
      uint8_t ir_cmd = pgm_read_byte(&(key_map[i].ir_code));
      if (ir_cmd == command)
	return pgm_read_byte(&(key_map[i].cpld_code));
    }
  return 0;
}
	
