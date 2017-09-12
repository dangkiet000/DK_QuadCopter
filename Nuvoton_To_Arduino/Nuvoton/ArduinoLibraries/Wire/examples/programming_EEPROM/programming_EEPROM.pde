// Wire Master Writer
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Writes data to an I2C/TWI slave device
// Refer to the "Wire Slave Receiver" example for use with this

// Created 29 March 2006

// This example code is in the public domain.


#include <Wire.h>

void setup()
{
  Serial.begin(9600);
  Wire.begin(); // join i2c bus (address optional for master)
  
  /* Programming EEPROM */
  int reading;
  /* Write EEPROM */
  for(int i=0;i<256;i++)
  {
    Wire.beginTransmission(80); // transmit to device #80(0x50)
    Wire.write(i>>8); // high address
    Wire.write(i);  // low address
    Wire.write(i);  // data
    Wire.endTransmission();
  }
  
   /* Read EEPROM */
  for(int i=0;i<256;i++)
  {
      Wire.beginTransmission(0x50); // transmit to device #80(0x50)
      Wire.write(byte(i>>8)); // high address
      Wire.write(byte(i)); //low address
      Wire.endTransmission(); // stop transmitting
      
      Wire.requestFrom(80, 1);    // request 6 bytes from slave device #80(0x50)
      
      // step 5: receive reading from sensor
      if( Wire.available() >=1)    // if two bytes were received
      {
        reading = Wire.read();  // receive high byte (overwrites previous reading) 
        Serial.print(reading);   // print the reading
        Serial.print("\t");
        
      }   
  }
}

void loop()
{

}
