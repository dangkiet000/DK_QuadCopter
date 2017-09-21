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

}

void main( void )
{
	init();		
	
//	#if defined(__M451__) | defined(__NUC240__) |defined(__NANO100__)
//	USBDevice.attach();
//	#endif
  
  attachInterrupt(BUTTON_PIN, Blinking, FALLING);
//	setup();
	pinMode(LED_PIN, OUTPUT);

  digitalWrite(LED_PIN, LOW);
  while(1)
	{

//		loop();		
//		if(serialEventRun) serialEventRun();
//    delay(1000);
//    digitalWrite(LED_PIN, HIGH);
//    delay(1000);
//    digitalWrite(LED_PIN, LOW);
    //Blinking();
    //delay(1000);
    //P00 ^= 1;
	}
  
}
