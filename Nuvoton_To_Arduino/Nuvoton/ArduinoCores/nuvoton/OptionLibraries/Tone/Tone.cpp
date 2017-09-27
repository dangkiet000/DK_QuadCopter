/* Tone.cpp

  A Tone Generator Library

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

*/


#include "Arduino.h"

/* Init and get address of object Timer1 */
HardwareTimer *ToneTimer = &Timer1;

typedef struct {
  HardwareTimer* timer;
  uint8_t pin;  
  uint8_t enabled;  
  volatile uint8_t waitISR;
} tone_t;

tone_t Tone={0};

void tone_ISR(void)
{		
	Tone.waitISR = 0;	
  
  #if defined(__NANO100__) | defined(__NANO1X2__)
	Tone.timer->clearIntFlag();
  #else
  
  Tone.timer->close();        
	PWM_DisableOutput(ARDU_PINTO_PWMGROUP(Tone.pin), \
                        (1<<(ARDU_PINTO_PWMCHANNEL(Tone.pin))));		
  
  #endif 
}

static uint8_t PinToneEnabled[PWM_MAX_PIN];

/* 
  frequency (in hertz) and duration (in milliseconds).
*/
void tone(uint8_t ulPin, unsigned int frequency, unsigned long duration)
{	
	tone_t *tone;
  PWM_T *LpPWMGroup;
  uint32_t LulPWMChannel;
  
  LpPWMGroup = ARDU_PINTO_PWMGROUP(ulPin);
    
  LulPWMChannel = ARDU_PINTO_PWMCHANNEL(ulPin);
	
	
	if (!PinToneEnabled[ulPin] && frequency!=0)
  {
		/* Set arduino pin to PWM pin */
		Ardu_PinConfigAsPWM(ulPin);		
    
		/* Config PWMs */
		PWM_ConfigOutputChannel(LpPWMGroup, LulPWMChannel, frequency, 50);
		
		/* Enable PWM output */
		PWM_EnableOutput(LpPWMGroup, (1<<LulPWMChannel));
		
		/* Start PWM */
		PWM_Start(LpPWMGroup, (1<<LulPWMChannel));
		
		PinToneEnabled[ulPin] = 1;
	}
	
  tone =& Tone;		
  tone->pin = ulPin;
  tone->timer = ToneTimer;		
  tone->enabled = 1;
  
  if(duration == 0)
  {
    if(frequency != 0)
    {
      PWM_EnableOutput(LpPWMGroup, (1<<LulPWMChannel));
      PWM_ConfigOutputChannel(LpPWMGroup, LulPWMChannel, frequency, 50);

    }
  }
  else
  {  			
    while(tone->waitISR);		
    
    /* Config PWMs*/
    if(frequency != 0)
    {
      PWM_EnableOutput(LpPWMGroup, (1<<LulPWMChannel));
      PWM_ConfigOutputChannel(LpPWMGroup, LulPWMChannel, frequency, 50);		
    }

    tone->waitISR = 1;
    
    /* Config Timer	mode as ONESHOT */
    tone->timer->open(ONESHOT, tone->timer->getModuleClock());
    tone->timer->setPrescaleFactor(0);
    tone->timer->setCompare(tone->timer->getModuleClock()/1000*duration);  /* milliseconds */ 
    tone->timer->attachInterrupt(tone_ISR);
    tone->timer->start();			
  }		
}

void noTone(uint8_t ulPin)
{
  tone_t *tone;
  PWM_T *LpPWMGroup;
  uint32_t LulPWMChannel;
  
  LpPWMGroup = ARDU_PINTO_PWMGROUP(ulPin);
    
  LulPWMChannel = ARDU_PINTO_PWMCHANNEL(ulPin);
  
	/* close tone	*/
	tone =& Tone;
  
	if(tone->pin == ulPin)
	{	 									
		while(tone->waitISR);		
    
		PWM_DisableOutput(LpPWMGroup, (1<<LulPWMChannel));
    						
		tone->pin = 0;
		tone->enabled = 0;
		tone->timer->close();		
	}
}
