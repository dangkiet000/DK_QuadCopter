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

#ifdef __cplusplus
 extern "C" {
#endif

#include "Arduino.h"

extern void pinMode( uint32_t ulPin, uint32_t ulMode )
{
  GPIO_T * LpPort;
  uint32_t LulPinMask;
  
  #if defined(__M051__)	
 
  //Ardu_PinConfigAsGPIO(ulPin);
  
  LulPinMask = (1 << (ARDU_PINTO_MCUPIN(ulPin)));
  LpPort = ARDU_PINTO_PORT(ulPin);

  switch ( ulMode )
  {
    case INPUT:                        
      ulMode = GPIO_PMD_INPUT;
    break ;

    case INPUT_PULLUP:            
      ulMode = GPIO_PMD_QUASI;                    
    break ;

    case OUTPUT:
      ulMode = GPIO_PMD_OUTPUT; 
    break ;

    default: 
      ulMode = GPIO_PMD_QUASI; 
    break ;
  }
  GPIO_SetMode(LpPort, LulPinMask, ulMode);
  #endif
}

extern void digitalWrite( uint32_t ulPin, uint32_t ulVal )
{
	#if defined(__M051__)	
  /*     PDIO address = GPIO_PIN_DATA_BASE + (4*ulPin) 
     <=> PDIO address = GPIO_PIN_DATA_BASE + (ulPin<<2)
     <=> PDIO address = GPIO_PIN_DATA_BASE | (ulPin<<2)
  */
  if(ulVal == LOW)
  {
    ARDU_PINTO_PDIO(ulPin) = LOW;
  }
  else
  {
    ARDU_PINTO_PDIO(ulPin) = HIGH;
  }

  #endif
}

extern int digitalRead( uint32_t ulPin )
{
	#if defined(__M051__)	
	if (ulPin > 47)
  {
    return 0;
  }
  #endif
  return ARDU_PINTO_PDIO(ulPin);
}

#ifdef __cplusplus
}
#endif

