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
#ifndef _PINS_ARDUINO_H_
#define _PINS_ARDUINO_H_

#if defined(__M051__)

/*------------------------------------------------------------------------------ 
          Arduino  ----------------   Nuvoton (M0516LDN)
              D0    ----------------     P0.0
              D1    ----------------     P0.1
              D2    ----------------     P0.2
              D3    ----------------     P0.3
              D4    ----------------     P0.4
              D5    ----------------     P0.5
              D6    ----------------     P0.6
              D7    ----------------     P0.7

          A0  D8    ----------------     P1.0  AIN0
          A1  D9    ----------------     P1.1  AIN1
      RX1 A2  D10   ----------------     P1.2  AIN2  RXD1
      TX1 A3  D11   ----------------     P1.3  AIN3  TXD1
          A4  D12   ----------------     P1.4  AIN4
          A5  D13   ----------------     P1.5  AIN5
          A6  D14   ----------------     P1.6  AIN6
(Measure Vcc) A7  D15   ----------------     P1.7  AIN7

       PWM0  D16   ----------------     P2.0 PWM0(TIMER0)
       PWM1  D17   ----------------     P2.1 PWM1(TIMER1)
       PWM2  D18   ----------------     P2.2 PWM2(TIMER2)
       PWM3  D19   ----------------     P2.3 PWM3(TIMER3)
       PWM4  D20   ----------------     P2.4 PWM4(TIMER0)
       PWM5  D21   ----------------     P2.5 PWM5(TIMER1)
       PWM6  D22   ----------------     P2.6 PWM6(TIMER2)
       PWM7  D23   ----------------     P2.7 PWM7(TIMER3)
             
         RX0  D24   ----------------     P3.0 RXD
         TX0  D25   ----------------     P3.1 TXD
              D26   ----------------     P3.2
              D27   ----------------     P3.3
              D28   ----------------     P3.4
              D29   ----------------     P3.5
              D30   ----------------     P3.6
              D31   ----------------     P3.7
              
              D32   ----------------     P4.0
              D33   ----------------     P4.1
              D34   ----------------     P4.2
              D35   ----------------     P4.3
              D36   ----------------     P4.4
              D37   ----------------     P4.5
              D38   ----------------     P4.6
              D39   ----------------     P4.7
------------------------------------------------------------------------------*/
/* ARDU_GET_ABPnBASE(number)
  @param[in] number : 0..1
  @return: address of APBn_BASE
           number = 0 => return APB1_BASE
           number = 1 => return APB2_BASE
*/
#define ARDU_GET_ABPnBASE(number) (APB1_BASE | ((number)<<20))

/*------------------------       PORT/DIO        -----------------------------*/
#define ARDU_PINTO_PORTNUMBER(pin) (pin/8)

#define ARDU_PINTO_PORT(pin) ((GPIO_T *)(GPIO_BASE | (ARDU_PINTO_PORTNUMBER(pin)*0x40)))

#define ARDU_PINTO_MCUPIN(pin) (pin%8)

#define ARDU_PINTO_PDIO(pin) (*((volatile uint32_t *)((GPIO_PIN_DATA_BASE | ((pin)<<2)))))

#define P0_MFP_OFFSET 0x30UL
#define ARDU_PINTO_MFPREG(pin) ((volatile uint32_t *)(GCR_BASE | (P0_MFP_OFFSET + 4*ARDU_PINTO_PORTNUMBER(pin))))

extern void Ardu_PinConfigAsGPIO(uint32_t ulArduPin);

/*-------------------------        PWM        --------------------------------*/
#define ARDU_PINTO_PWMGROUP(pin) ((PWM_T *) ((ARDU_GET_ABPnBASE(pin/20)) | 0x00040000))
#define ARDU_PINTO_PWMCHANNEL(pin) (pin%4)

#define PWM_MAX_PIN 8U

#define ARDU_PINTO_ADCCHANNEL(pin) (pin%8)
extern void Ardu_PinConfigAsPWM(uint32_t ulArduPin);

/*-------------------------      ANALOG       --------------------------------*/
#define ADC_MAX_PIN 8U

void Ardu_PinConfigAsADC(uint32_t ulArduPin);

/*-------------------------      UART         --------------------------------*/
#if defined(__M051__)
  #define UART_MAX_COUNT 2
#elif defined(__NUC240__)
	#define UART_MAX_COUNT 3
#elif defined(__NUC131__)
  #define UART_MAX_COUNT 1
#endif

void Ardu_PinConfigAsUART(UART_T *uart);

#endif /* End if __M051__ */

#endif /* End if _PINS_ARDUINO_H_ */