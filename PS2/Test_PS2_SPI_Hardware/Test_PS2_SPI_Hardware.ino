#include "PS2X_lib.h" 

/* 
DATA and ACK pin is open-collector 
=> Connect this pin to 4.7k pull-up resistor
if this pin not conect r-pullup, DATA is wrong
and byte1 return 0x63 instead 0x73
*/
#define PS2_DAT        12 
#define PS2_CMD        11 
#define PS2_SEL        10  
#define PS2_CLK        13

//These are our button constants
#define PSB_L2          0x0100
#define PSB_L1          0x0400

/* These are button of byte5 */
#define PS2_TRIANGLE    0xEF
#define PS2_CIRCLE      0xBF
#define PS2_CROSS       0xDF
#define PS2_SQUARE      0x7F
#define PS2_R1          0xF7
#define PS2_R2          0xFD

/* These are button of byte4 */
#define PS2_START       0xF7
#define PS2_PAD_UP      0xEF
#define PS2_PAD_RIGHT   0xDF
#define PS2_PAD_DOWN    0xBF
#define PS2_PAD_LEFT    0x7F
#define PS2_SELECT      0xFE
#define PS2_L3          0xFD
#define PSB_R3          0xFB
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

void setup(){
 
  
 
  Serial.begin(57600);
  
  delay(300);  //added delay to give wireless ps2 module some time to startup, before configuring it
   
  //CHANGES for v1.6 HERE!!! **************PAY ATTENTION*************
  
  //setup pins and settings: GamePad(clock, command, attention, data, Pressures?, Rumble?) check for error
  ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, pressures, rumble);
}

void loop() {
  /* You must Read Gamepad to get new values and set vibration values
     ps2x.read_gamepad(small motor on/off, larger motor strenght from 0-255)
     if you don't enable the rumble, use ps2x.read_gamepad(); with no values
     You should call this at least once a second
   */  
  //DualShock Controller
  ps2x.read_gamepad(false, vibrate);

  Serial.print(ps2x.PS2data[3], HEX);
  Serial.print(" ");
  Serial.println(ps2x.PS2data[4], HEX);
  
  delay(200);  
}
