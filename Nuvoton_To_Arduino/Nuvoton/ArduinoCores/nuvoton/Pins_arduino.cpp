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
#include "Pins_arduino.h"

#if defined(__M051__)

void Ardu_PinConfigAsGPIO(uint32_t ulArduPin)
{
  volatile uint32_t *LpMFPReg;
  uint8_t LucMCUPin;
  
  LucMCUPin = ARDU_PINTO_MCUPIN(ulArduPin);
  LpMFPReg = ARDU_PINTO_MFPREG(ulArduPin);
  
  /* Clear bit MFP */
  bitClear(*LpMFPReg, LucMCUPin); 
  
  /* Clear bit ALT */
  bitClear(*LpMFPReg, LucMCUPin + 8); 
}

void Ardu_PinConfigAsPWM(uint32_t ulArduPin)
{
  volatile uint32_t *LpMFPReg;
  uint8_t LucMCUPin;
  
  LucMCUPin = ARDU_PINTO_MCUPIN(ulArduPin);
  LpMFPReg = ARDU_PINTO_MFPREG(ulArduPin);
  
  /* Clear bit MFP */
  bitClear(*LpMFPReg, LucMCUPin); 
  
  /* Set bit ALT */
  bitSet(*LpMFPReg, LucMCUPin + 8); 
}

/* RX0  D24
   TX0  D25
   RX1  D10
   TX1  D11
  */
void Ardu_PinConfigAsUART(UART_T *uart)
{
  volatile uint32_t *LpMFPReg;
  uint8_t LucRXPin;
  uint8_t LucTXPin;
  
  if(uart == UART0)
  {
    LucRXPin = ARDU_PINTO_MCUPIN(24);
    LucTXPin = ARDU_PINTO_MCUPIN(25);
    /* TX and RX pin is the same port */
    LpMFPReg = ARDU_PINTO_MFPREG(24);
  }
  else if(uart == UART1)
  {
    LucRXPin = ARDU_PINTO_MCUPIN(10);
    LucTXPin = ARDU_PINTO_MCUPIN(11);
    /* TX and RX pin is the same port */
    LpMFPReg = ARDU_PINTO_MFPREG(10);
  }
  else
  {
    /* DET error report */  
  }
  
  /* Bit MFP = 1 */
  bitSet(*LpMFPReg, LucRXPin); 
  
  /* Bit ALT = 0 */
  bitClear(*LpMFPReg, LucRXPin + 8); 
  
  /* Bit MFP = 1 */
  bitSet(*LpMFPReg, LucTXPin); 
  
  /* Bit ALT = 0 */
  bitClear(*LpMFPReg, LucTXPin + 8); 
}

#endif
