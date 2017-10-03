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
static uint8_t PinADCEnabled[ADC_MAX_PIN]={0};

static int _ReadResolution = 10;
static uint8_t _WriteResolution = 8;

/* M051 series only have 12-bit ADC resolution */
void analogReadResolution(uint8_t res)
{
	_ReadResolution = res;
}

void analogWriteResolution(uint8_t res)
{
  PWM_T *LpPWMGroup;
  uint32_t LulPWMChannel;
  uint32_t LulArduPin;
  uint32_t i;
  
  if((res != _WriteResolution) && (res > 7) && (res < 13))
  {
    _WriteResolution = res;
    
    for(i=0; i<PWM_MAX_PIN; i++)
    {
      /* Check if this PWM pin is initialized */
      if (PinPWMEnabled[i] != 0)
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
}


/* 
  M051 Series can't set reference voltage 
  => this function is empty 
*/
void analogReference(eAnalogReference ulMode)
{
  if(ulMode == EXTERNAL)
  {
   // ADC_CONFIG_CH7(ADC, ADC_ADCHER_PRESEL_EXT_INPUT_SIGNAL);
  }
  else if(ulMode == INTERNAL)
  {
   // ADC_CONFIG_CH7(ADC, ADC_ADCHER_PRESEL_INT_BANDGAP);
  }
  else if(ulMode == INTERNAL_TEMP)
  {
    //ADC_CONFIG_CH7(ADC, ADC_ADCHER_PRESEL_INT_TEMPERATURE_SENSOR);
  }
  else
  {
    /* Nothing to do */
  }
}

static inline uint32_t mapResolution(uint32_t value, uint32_t from, uint32_t to)
{
	if (from == to)
		return value;
	if (from > to)
		return (value >> (from-to));
	else
		return (value << (to-from));
}

uint32_t analogRead(uint8_t u8Pin)
{
  uint32_t LulAdcValue = 0;
  uint32_t LulADCChannel;

  if(PIN_IS_TYPE(u8Pin, ADC_TYPE) == FALSE)
  {
    /* DET error reported */
    return 0;
  }
  
  LulADCChannel = ARDU_PINTO_ADCCHANNEL(u8Pin);
  
#if defined(__M051__)
  
  /* Check if this PWM pin is not initialized */
	if (PinADCEnabled[LulADCChannel] == 0)
  {
    Ardu_PinConfigAsADC(u8Pin);
    /* Disable the digital input path to avoid the leakage current */
    GPIO_DISABLE_DIGITAL_PATH(ARDU_PINTO_PORT(u8Pin), (1 << LulADCChannel));
    
    /* Set the ADC operation mode as single conversion, input mode as single-end 
       and enable the analog input channel */
    ADC_Open(ADC, ADC_ADCR_DIFFEN_SINGLE_END, ADC_ADCR_ADMD_SINGLE, \
                                                      (1<<LulADCChannel));
    /* ADC convert complete interrupt: Disable */
    ADC_DisableInt(ADC, ADC_ADF_INT);

    /* Power on ADC module */
    ADC_POWER_ON(ADC);

    /* Clear the A/D interrupt flag for safe */
    ADC_CLR_INT_FLAG(ADC, ADC_ADF_INT);
    
    PinADCEnabled[LulADCChannel] = 1;
  }

  /* Start for conversion	*/
  ADC_START_CONV(ADC);
      
  /* Wait for end of conversion */
  while(!ADC_GET_INT_FLAG(ADC, ADC_ADF_INT));
  
  /* Clear ADC flag */
  ADC_CLR_INT_FLAG(ADC, ADC_ADF_INT);
  
  /* Read the value */
  LulAdcValue = ADC_GET_CONVERSION_DATA(ADC, LulADCChannel);	

#endif
  
  LulAdcValue = mapResolution(LulAdcValue, ADC_HARDWARE_RESOLUTION, \
                                                         _ReadResolution);
  
  return LulAdcValue;	
}

/* 
              PWM clock IP = HCLK = 50Mhz 
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
void analogWrite(uint8_t u8Pin, uint16_t u16Value)
{
  PWM_T *LpPWMGroup;
  uint32_t LulPWMChannel;
  
  if(PIN_IS_TYPE(u8Pin, PWM_TYPE) == FALSE)
  {
    /* DET error reported */
    return;
  }
  
  LpPWMGroup = ARDU_PINTO_PWMGROUP(u8Pin);
  LulPWMChannel = ARDU_PINTO_PWMCHANNEL(u8Pin);

  if(u16Value > (1<<_WriteResolution))
  {
    u16Value = 1<<_WriteResolution;
  }
  

#if defined(__M051__)
	if(u16Value == 0)
	{  
    /* DeInit PWM pin */
    PWM_Stop(LpPWMGroup, (1<<LulPWMChannel));
    
    Ardu_PinConfigAsGPIO(u8Pin);
    pinMode(u8Pin, OUTPUT);
    digitalWrite(u8Pin, LOW);
    PinPWMEnabled[ARDPIN_TO_PWMPIN_IDX(u8Pin)] = FALSE;

		return;
	}
#endif	
	/* Check if this PWM pin is not initialized */
  if (PinPWMEnabled[ARDPIN_TO_PWMPIN_IDX(u8Pin)] == 0)
  {
		/* Set this pin to PWM pin */
		Ardu_PinConfigAsPWM(u8Pin);	
    
    PWM_SET_PRESCALER(LpPWMGroup, LulPWMChannel, 
                            (uint8_t)(1<<(15 -_WriteResolution)));
    
    PWM_SET_DIVIDER(LpPWMGroup, LulPWMChannel, PWM_CLK_DIV_1);
    
    /* PWM mode as AUTO-RELOAD */
    LpPWMGroup->PCR |= (1<< (3 + (LulPWMChannel*8)));
    
    PWM_DisableOutput(LpPWMGroup, (1<<LulPWMChannel));
    
    
    /* Set the duty of the selected channel */
    PWM_SET_CMR(LpPWMGroup, LulPWMChannel, u16Value);
    
    /* Set the period of the selected channel */
    PWM_SET_CNR(LpPWMGroup, LulPWMChannel, (1<<_WriteResolution));
		
    /* Set the PWM aligned type */
    PWM_SET_ALIGNED_TYPE(LpPWMGroup, (1<<LulPWMChannel), PWM_EDGE_ALIGNED);
    
    PWM_DisableDutyInt(LpPWMGroup, LulPWMChannel);

		/* Enable PWM output */
		PWM_EnableOutput(LpPWMGroup, (1<<LulPWMChannel));
		
		/* Start PWM */
		PWM_Start(LpPWMGroup, (1<<LulPWMChannel));
		
		PinPWMEnabled[ARDPIN_TO_PWMPIN_IDX(u8Pin)] = TRUE;
    return;
	}
	
	/* Set the duty of the selected channel */		
  PWM_SET_CMR(LpPWMGroup, LulPWMChannel, u16Value);

	
	return;
}

/*******************************************************************************
                           EXTERNAL ARDUINO API(M051)
*******************************************************************************/
/*
  Analog Input Channel 7 Source Selection 
  00 = External analog input. 
  01 = Internal band-gap voltage. 
  10 = Internal temperature sensor. 
  11 = Reserved. 
  Note: When the band-gap voltage is selected as the analog input source of ADC 
  channel 7, ADC peripheral clock rate needs to be limited to lower than 
  300 kHz.
*/

/*
  The band-gap voltage reference (VBG) is an  internal fixed reference voltage
  regardless of power supply variations. The VBG output is internally connected
  to ADC channel 7 source multiplexer and Analog Comparators’s (ACMP) negative 
  input side. 
  For battery power detection application, user can use the VBG as ADC input 
  channel such that user can convert the A/D conversion result to calculate 
  AVDD with following formula. 

AVDD = ((2 ^ N) / R) * VBG 
N: ADC resolution 
R: A/D conversion result 

M051 series: N = 12, VBG = 1.25V
=> AVDD = (4096 / R) * 1.25 
   AVDD(V) = 6144 / R

=> AVDD(mV) = 6144000 / R
*/
/**
  * @brief Read voltage(mV) power source Vcc.
  * @param[in] None
  * @return Vcc(mV)
  */
uint32_t analogReadVcc(void)
{
  uint32_t LulVcc;
  uint32_t LulAdcValue;
  
  
  /* Check if this PWM pin is not initialized */
	if (!PinADCEnabled[INTERNAL_BANDGAP_CHANNEL])
  {
    //Ardu_PinConfigAsADC(ulArduPin);
 
    /* Disable the digital input path to avoid the leakage current */
    //GPIO_DISABLE_DIGITAL_PATH(ARDU_PINTO_PORT(ulArduPin), (1 << LulADCChannel));
    
    /* Set the ADC operation mode as single conversion, input mode as single-end 
       and enable the analog input channel */
    ADC_Open(ADC, ADC_ADCR_DIFFEN_SINGLE_END, ADC_ADCR_ADMD_SINGLE, \
                                             (1<<INTERNAL_BANDGAP_CHANNEL));
    
    ADC_CONFIG_CH7(ADC, ADC_ADCHER_PRESEL_INT_BANDGAP);
    
    /* ADC convert complete interrupt: Disable */
    ADC_DisableInt(ADC, ADC_ADF_INT);

    /* Power on ADC module */
    ADC_POWER_ON(ADC);

    /* Clear the A/D interrupt flag for safe */
    ADC_CLR_INT_FLAG(ADC, ADC_ADF_INT);
    
    PinADCEnabled[INTERNAL_BANDGAP_CHANNEL] = 1;
  }

  /* Start for conversion	*/
  ADC_START_CONV(ADC);
      
  /* Wait for end of conversion */
  while(!ADC_GET_INT_FLAG(ADC, ADC_ADF_INT));
  
  /* Clear ADC flag */
  ADC_CLR_INT_FLAG(ADC, ADC_ADF_INT);
  
  /* Read the value */
  LulAdcValue = ADC_GET_CONVERSION_DATA(ADC, INTERNAL_BANDGAP_CHANNEL);	
  
  LulVcc = 6144000 / LulAdcValue;
    
  return LulVcc;
}

void analogReadClose(void)
{
  ADC_Close(ADC);
}

#ifdef __cplusplus
}
#endif
