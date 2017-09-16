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

#ifdef __cplusplus
extern "C" {
#endif
  
static uint8_t PinPWMEnabled[PWM_MAX_PIN]={0};

static int _readResolution = 10;
static uint8_t _WriteResolution = 8;

void analogReadResolution(uint8_t res)
{
	_readResolution = res;
}

void analogWriteResolution(uint8_t res)
{
  PWM_T *LpPWMGroup;
  uint32_t LulPWMChannel;
  uint32_t LulArduPin;
  uint32_t i;
  
	_WriteResolution = res;
  
  for(i=0; i<PWM_MAX_PIN; i++)
  {
    if(PinPWMEnabled[i])
    {
      LulArduPin = 16 + i;
      LpPWMGroup = ARDU_PINTO_PWMGROUP(LulArduPin);
      LulPWMChannel = ARDU_PINTO_PWMCHANNEL(LulArduPin);
  
      /* Set the prescaler of the selected channel */
      PWM_SET_PRESCALER(LpPWMGroup, LulPWMChannel, (uint8_t)(1<<(16 -_WriteResolution)));
      /* Set the period of the selected channel */
      PWM_SET_CNR(LpPWMGroup, LulPWMChannel, (1<<_WriteResolution));
    }
  }
}

//eAnalogReference analog_reference = AR_DEFAULT;

//void analogReference(eAnalogReference ulMode)
//{
//	analog_reference = ulMode;
//}

uint32_t analogRead(uint32_t ulPin)
{
#if defined(__M051__)
  uint32_t ulValue = 0; 
  
#elif defined(__NUC240__)
  uint32_t ulValue = 0;  

  if(ulPin>ADC_MAX_COUNT || ADC_Desc[ulPin].A==NULL) return;  	  
  	
  ADC_Config(ADC_Desc[ulPin]);
  
  //GPIO_ENABLE_DIGITAL_PATH(GPIO_Desc[ADC_Desc[ulPin].pintype.num].P,GPIO_Desc[ADC_Desc[ulPin].pintype.num].bit);
  
  // Enable channel 0
	ADC_Open(ADC_Desc[ulPin].A, 0, 0, (1<<ADC_Desc[ulPin].ch));
		
	// Power on ADC
	ADC_POWER_ON(ADC_Desc[ulPin].A);

  // Wait for busy of conversion
  while(ADC_IS_BUSY(ADC_Desc[ulPin].A));	
  	
  // Start for conversion	
  ADC_START_CONV(ADC_Desc[ulPin].A);
  		
	// Wait for end of conversion
	while(!ADC_GET_INT_FLAG(ADC_Desc[ulPin].A,ADC_ADF_INT));
	
	// Clear ADC flag
	ADC_CLR_INT_FLAG(ADC_Desc[ulPin].A,ADC_ADF_INT);
	
	// Read the value
	ulValue = ADC_GET_CONVERSION_DATA(ADC_Desc[ulPin].A,ADC_Desc[ulPin].ch);	
	ulValue = mapResolution(ulValue, 12, _readResolution);
  
  //GPIO_DISABLE_DIGITAL_PATH(GPIO_Desc[ADC_Desc[ulPin-1].pintype.num].P,GPIO_Desc[ADC_Desc[ulPin].pintype.num-1].bit);
  
  // Close ADC
  ADC_Close(ADC_Desc[ulPin].A);
#elif defined(__NUC131__)
  uint32_t ulValue = 0;  

  if(ulPin>ADC_MAX_COUNT || ADC_Desc[ulPin].A==NULL) return;  	  
  	
  ADC_Config(ADC_Desc[ulPin]);
  
  //GPIO_ENABLE_DIGITAL_PATH(GPIO_Desc[ADC_Desc[ulPin].pintype.num].P,GPIO_Desc[ADC_Desc[ulPin].pintype.num].bit);
  
  // Enable channel 0
	ADC_Open(ADC_Desc[ulPin].A, 0, 0, (1<<ADC_Desc[ulPin].ch));
		
	// Power on ADC
	ADC_POWER_ON(ADC_Desc[ulPin].A);

  // Wait for busy of conversion
  while(ADC_IS_BUSY(ADC_Desc[ulPin].A));	
  	
  // Start for conversion	
  ADC_START_CONV(ADC_Desc[ulPin].A);
  		
	// Wait for end of conversion
	while(!ADC_GET_INT_FLAG(ADC_Desc[ulPin].A,ADC_ADF_INT));
	
	// Clear ADC flag
	ADC_CLR_INT_FLAG(ADC_Desc[ulPin].A,ADC_ADF_INT);
	
	// Read the value
	ulValue = ADC_GET_CONVERSION_DATA(ADC_Desc[ulPin].A,ADC_Desc[ulPin].ch);	
	ulValue = mapResolution(ulValue, 12, _readResolution);
  
  //GPIO_DISABLE_DIGITAL_PATH(GPIO_Desc[ADC_Desc[ulPin-1].pintype.num].P,GPIO_Desc[ADC_Desc[ulPin].pintype.num-1].bit);
  
  // Close ADC
  ADC_Close(ADC_Desc[ulPin].A);
#endif
  return ulValue;	
}


/* PWM clock IP = HCLK = 50Mhz 
=> Freq(Hz) = PWM_clock_IP/((Prescaler+1)*Divider*(CNR+1))

Default: 
Prescaler = 128, Divider = 1, 
ArduWriteResolution = 8 => CNR = 255
=>Freq(Hz) = 1514
______________________________________________________________________
|    Freq    | Prescaler| Divider| Resolution|   F_CPU  | User Input |
|____(Hz)____|__________|________|___(Bit)___|___(Hz)___|_(uint16_t)_|
|1514.050388 |   128    |    1   |     8     | 50000000 |   256      |(Default)
|1514.050388 |   64     |    1   |     9     | 50000000 |   512      |
|1514.050388 |   32     |    1   |     10    | 50000000 |   1024     |
|1514.050388 |   16     |    1   |     11    | 50000000 |   2048     |
|1514.050388 |   8      |    1   |     12    | 50000000 |   4096     |
|____________|__________|________|___________|__________|____________|
*/
void analogWrite(uint32_t ulPin, uint16_t ulValue)
{
  PWM_T *LpPWMGroup;
  uint32_t LulPWMChannel;
  
  LpPWMGroup = ARDU_PINTO_PWMGROUP(ulPin);
    
  LulPWMChannel = ARDU_PINTO_PWMCHANNEL(ulPin);
  
  if(ulValue > (1<<_WriteResolution))
  {
    ulValue = 1<<_WriteResolution;
  }
  

#if defined(__M051__)
	if(ulValue == 0)
	{  
    /* DeInit PWM pin */
    PWM_Stop(LpPWMGroup, (1<<LulPWMChannel));
    
    Ardu_PinConfigAsGPIO(ulPin);
    pinMode(ulPin, OUTPUT);
    digitalWrite(ulPin, LOW);
    PinPWMEnabled[ulPin] = 1;

		return;
	}
#endif	
	/* Check if this PWM pin is not initialized */
	if (!PinPWMEnabled[ARDU_PINTO_MCUPIN(ulPin)])
  {
		/* Set this pin to PWM pin */
		Ardu_PinConfigAsPWM(ulPin);	
    
    /* PWM mode as AUTO-RELOAD */
    LpPWMGroup->PCR |= (1<< (3 + (LulPWMChannel*8)));
    
    PWM_SET_PRESCALER(LpPWMGroup, LulPWMChannel, (uint8_t)(1<<(15 -_WriteResolution)));
    
    PWM_SET_DIVIDER(LpPWMGroup, LulPWMChannel, PWM_CLK_DIV_1);
    
    /* Set the duty of the selected channel */
    PWM_SET_CMR(LpPWMGroup, LulPWMChannel, ulValue);
    
    /* Set the period of the selected channel */
    PWM_SET_CNR(LpPWMGroup, LulPWMChannel, (1<<_WriteResolution));
		
    /* Set the PWM aligned type */
    PWM_SET_ALIGNED_TYPE(LpPWMGroup, (1<<LulPWMChannel), PWM_EDGE_ALIGNED);
    
    

		/* Enable PWM output */
		PWM_EnableOutput(LpPWMGroup, (1<<LulPWMChannel));
		
		/* Start PWM */
		PWM_Start(LpPWMGroup, (1<<LulPWMChannel));
		
		PinPWMEnabled[ulPin] = 1;
	}
	
	/* Set the duty of the selected channel */		
  PWM_SET_CMR(LpPWMGroup, LulPWMChannel, ulValue);

	
	return;
}

#ifdef __cplusplus
}
#endif
