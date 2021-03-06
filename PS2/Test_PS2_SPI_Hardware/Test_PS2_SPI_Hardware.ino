#include "PS2X_DK_lib.h" 

void setup()
{
  Serial.begin(57600);
  //Added delay to give wireless ps2 module some time to startup, 
  //before configuring it
  delay(300);  
  
  PS2_Init();
  PS2_SetMode(PS2_SET_ANALOG_MODE);
}

void loop() 
{
  /* DualShock Controller */
  PS2_Read();
  delay(2000);

}
