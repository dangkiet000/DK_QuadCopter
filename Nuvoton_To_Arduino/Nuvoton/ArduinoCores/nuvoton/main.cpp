/*
  Copyright (c) 2011 Arduino.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "Arduino.h"

/*
 * \brief Main entry point of Arduino application
 */
#define LED_PIN 0
#define BUTTON_PIN 30
   
void Blinking(void)
{
  P00 ^= 1;
  digitalWrite(8, HIGH);
}

uint32_t i, u32_VCC;
void main( void )
{
	init();		
  
//	setup();
  pinMode(BUTTON_PIN, INPUT);
	pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  Serial.begin(9600);
  
  
  while(1)
	{

//		loop();		
	  serialEventRun();
    delay(1000);
    for(i=0; i<1000; i++)
    {
      u32_VCC += analogReadVcc();
    }
    u32_VCC = u32_VCC/1000;
    Serial.print(u32_VCC);
    Serial.println(" mV");
    u32_VCC = 0;
	}
  
}
