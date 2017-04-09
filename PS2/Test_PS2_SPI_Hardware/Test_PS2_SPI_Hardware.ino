#include "PS2X_lib.h"
#include "PS2X_DK_lib.h" 

#define PS2_DAT        12 
#define PS2_CMD        11 
#define PS2_SEL        10  
#define PS2_CLK        13

/******************************************************************
 * select modes of PS2 controller:
 *   - pressures: analog reading of push-butttons 
 *   - rumble   : motor rumbling
 * uncomment 1 of the lines for each mode selection
 ******************************************************************/
#define pressures   false
#define rumble      false

PS2X ps2x; // create PS2 Controller Class

//right now, the library does NOT support hot pluggable controllers, meaning 
//you must always either restart your Arduino after you connect the controller, 
//or call config_gamepad(pins) again after connecting the controller.

int error = 0;
byte type = 0;
byte vibrate = 0;

void setup()
{
  Serial.begin(57600);
  delay(300);  //added delay to give wireless ps2 module some time to startup, before configuring it
  
  PS2_Init();
  //setup pins and settings: GamePad(clock, command, attention, data, Pressures?, Rumble?) check for error
  //ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, pressures, rumble);
}

void loop() 
{
  /* You must Read Gamepad to get new values and set vibration values
     ps2x.read_gamepad(small motor on/off, larger motor strenght from 0-255)
     if you don't enable the rumble, use ps2x.read_gamepad(); with no values
     You should call this at least once a second
   */  
  /* DualShock Controller */
  /*
  ps2x.read_gamepad(false, vibrate);

  Serial.print(ps2x.PS2data[3], HEX);
  Serial.print(" ");
  Serial.print(ps2x.PS2data[4], HEX);
  Serial.print(" ");
  Serial.print(ps2x.PS2data[5], HEX);
  Serial.print(" ");
  Serial.print(ps2x.PS2data[6], HEX);
  Serial.print(" ");
  Serial.print(ps2x.PS2data[7], HEX);
  Serial.print(" ");
  Serial.print(ps2x.PS2data[8], HEX);
  Serial.println();
  delay(800);
    */
}
