#include "PS2X_lib.h"  //for v1.6

/******************************************************************
 * set pins connected to PS2 controller:
 *   - 1e column: original 
 *   - 2e colmun: Stef?
 * replace pin numbers by the ones you use
 ******************************************************************/
#define PS2_DAT        12  //14    
#define PS2_CMD        11  //15
#define PS2_SEL        10  //16
#define PS2_CLK        13  //17

/******************************************************************
 * select modes of PS2 controller:
 *   - pressures = analog reading of push-butttons 
 *   - rumble    = motor rumbling
 * uncomment 1 of the lines for each mode selection
 ******************************************************************/
//#define pressures   true
#define pressures   false
//#define rumble      true
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
    ps2x.read_gamepad(false, vibrate); //read controller and set large motor to spin at 'vibrate' speed
    
    if(ps2x.Button(PSB_START))
    {//will be TRUE as long as button is pressed
      Serial.print("Start ");
      Serial.println(ps2x.PS2data[3], HEX);
    }
       

    if(ps2x.Button(PSB_PAD_UP)) {      //will be TRUE as long as button is pressed
      Serial.print("Up ");
      Serial.println(ps2x.PS2data[3], HEX);
    }
    if(ps2x.Button(PSB_PAD_RIGHT)){
      Serial.print("Right ");
      Serial.println(ps2x.PS2data[3], HEX);
    }
    if(ps2x.Button(PSB_PAD_LEFT)){
      Serial.print("LEFT ");
      Serial.println(ps2x.PS2data[3], HEX);
    }
    if(ps2x.Button(PSB_PAD_DOWN)){
      Serial.print("DOWN ");
      Serial.println(ps2x.PS2data[3], HEX);
    }   

    if(ps2x.Button(PSB_TRIANGLE)){
      Serial.print("TRIANGLE ");
      Serial.println(ps2x.buttons, HEX);
    } 

    if(ps2x.Button(PSB_CIRCLE)){
      Serial.print("CIRCLE ");
      Serial.println(ps2x.buttons, HEX);
    } 
    
    if(ps2x.Button(PSB_CROSS)){
      Serial.print("CROSS ");
      Serial.println(ps2x.buttons, HEX);
    } 

    if(ps2x.Button(PSB_SQUARE)){
      Serial.print("SQUARE ");
      Serial.println(ps2x.buttons, HEX);
    }     
  
  delay(50);  
}
