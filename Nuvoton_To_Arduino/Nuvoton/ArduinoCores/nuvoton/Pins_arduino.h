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

       D8    ----------------     P1.0
       D9    ----------------     P1.1
       D10   ----------------     P1.2
       D11   ----------------     P1.3
       D12   ----------------     P1.4
       D13   ----------------     P1.5
       D14   ----------------     P1.6
       D15   ----------------     P1.7

   PWM D16   ----------------     P2.0 PWM0(TIMER0)
   PWM D17   ----------------     P2.1 PWM1(TIMER1)
   PWM D18   ----------------     P2.2 PWM2(TIMER2)
   PWM D19   ----------------     P2.3 PWM3(TIMER3)
   PWM D20   ----------------     P2.4 PWM4(TIMER0)
   PWM D21   ----------------     P2.5 PWM5(TIMER1)
   PWM D22   ----------------     P2.6 PWM6(TIMER2)
   PWM D23   ----------------     P2.7 PWM7(TIMER3)
       
       D24   ----------------     P3.0
       D25   ----------------     P3.1
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
#define ARDU_PINTO_PORTNUMBER(pin) (pin/8)

#define ARDU_PINTO_PORT(pin) ((GPIO_T *)(GPIO_BASE | (ARDU_PINTO_PORTNUMBER(pin)*0x40)))

#define ARDU_PINTO_MCUPIN(pin) (pin%8)

#define ARDU_PINTO_PDIO(pin) (*((volatile uint32_t *)((GPIO_PIN_DATA_BASE | ((pin)<<2)))))

#define P0_MFP_OFFSET 0x30UL
#define ARDU_PINTO_MFPREG(pin) ((volatile uint32_t *)(GCR_BASE | (P0_MFP_OFFSET + 4*ARDU_PINTO_PORTNUMBER(pin))))

void Ardu_PinConfigAsGPIO(uint32_t ulArduPin);

/*-------------------------        PWM        --------------------------------*/
/* ARDU_GET_ABPnBASE(number)
  @param[in] number : 0..1
                      0: APB1_BASE
                      1: APB2_BASE
*/
#define ARDU_GET_ABPnBASE(number) (APB1_BASE | ((number)<<20))

#define ARDU_PINTO_PWMGROUP(pin) ((PWM_T *) ((ARDU_GET_ABPnBASE(pin/20)) | 0x00040000))
#define ARDU_PINTO_PWMCHANNEL(pin) (pin%4)

#define PWM_MAX_PIN 8U

void Ardu_PinConfigAsPWM(uint32_t ulArduPin);

#endif /* End if __M051__ */

#endif /* End if _PINS_ARDUINO_H_ */