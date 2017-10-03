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
/*******************************************************************************
**                      Include Section                                       **
*******************************************************************************/
#include "Arduino.h"
#include "Pins_arduino.h"


#if defined(__M051__)

/*******************************************************************************
**                      Global Data Types                                     **
*******************************************************************************/
/* Index of ArduinoPinType is digital pin number */
const Ard_PinType Arduino_PinType[] = 
{
  GPIO_TYPE, /* D0 : P0.0  */
  GPIO_TYPE, /* D1 : P0.1  */
  GPIO_TYPE, /* D2 : P0.2  */
  GPIO_TYPE, /* D3 : P0.3  */
  SPI_TYPE , /* D4 : P0.4  */
  SPI_TYPE , /* D5 : P0.5  */
  SPI_TYPE , /* D6 : P0.6  */
  SPI_TYPE , /* D7 : P0.7  */
  ADC_TYPE            , /* D8 : P1.0  */
  ADC_TYPE            , /* D9 : P1.1  */
  ADC_TYPE | UART_TYPE, /* D10: P1.2  */
  ADC_TYPE | UART_TYPE, /* D11: P1.3  */
  ADC_TYPE | SPI_TYPE , /* D12: P1.4  */
  ADC_TYPE | SPI_TYPE , /* D13: P1.5  */
  ADC_TYPE | SPI_TYPE , /* D14: P1.6  */
  ADC_TYPE | SPI_TYPE , /* D15: P1.7  */
  PWM_TYPE, /* D16: P2.0  */
  PWM_TYPE, /* D17: P2.1  */
  PWM_TYPE, /* D18: P2.2  */
  PWM_TYPE, /* D19: P2.3  */
  PWM_TYPE, /* D20: P2.4  */
  PWM_TYPE, /* D21: P2.5  */
  PWM_TYPE, /* D22: P2.6  */
  PWM_TYPE, /* D23: P2.7  */
  UART_TYPE, /* D24: P3.0  */
  UART_TYPE, /* D25: P3.1  */
  GPIO_TYPE, /* D26: P3.2  */
  GPIO_TYPE, /* D27: P3.3  */
  I2C_TYPE , /* D28: P3.4  */
  I2C_TYPE , /* D29: P3.5  */
  GPIO_TYPE, /* D30: P3.6  */
  GPIO_TYPE, /* D31: P3.7  */
  GPIO_TYPE, /* D32: P4.0  */
  GPIO_TYPE, /* D33: P4.1  */
  GPIO_TYPE, /* D34: P4.2  */
  GPIO_TYPE, /* D35: P4.3  */
  I2C_TYPE , /* D36: P4.4  */
  I2C_TYPE , /* D37: P4.5  */
  GPIO_TYPE, /* D38: P4.6  */
  GPIO_TYPE, /* D39: P4.7  */ 
};

/*******************************************************************************
**                      Function Definitions                                  **
*******************************************************************************/
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

void Ardu_PinConfigAsADC(uint8_t u8Pin)
{
  volatile uint32_t *LpMFPReg;
  uint8_t LucMCUPin;
  
  LucMCUPin = ARDU_PINTO_MCUPIN(u8Pin);
  LpMFPReg = ARDU_PINTO_MFPREG(u8Pin);
  
  /* Set bit MFP */
  bitSet(*LpMFPReg, LucMCUPin); 
  
  /* Clear bit ALT */
  bitClear(*LpMFPReg, LucMCUPin + 8); 
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

/*******************************************************************************
**                          End of File                                       **
*******************************************************************************/
