/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2010 Bryan Newbold.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *****************************************************************************/

#include "HardwareTimer.h"


#if defined(__M051__)
  #if(MAX_TIMER_HW_UNIT>0)
		HardwareTimer Timer0(0,TMR0_MODULE,CLK_CLKSEL1_TMR0_S_HCLK);
	#endif
	#if(MAX_TIMER_HW_UNIT>1)
		HardwareTimer Timer1(1,TMR1_MODULE,CLK_CLKSEL1_TMR1_S_HCLK);
	#endif
	#if(MAX_TIMER_HW_UNIT>2)
		HardwareTimer Timer2(2,TMR2_MODULE,CLK_CLKSEL1_TMR2_S_HCLK);
	#endif
	#if(MAX_TIMER_HW_UNIT>3)
		HardwareTimer Timer3(3,TMR3_MODULE,CLK_CLKSEL1_TMR3_S_HCLK);
	#endif

#elif defined(__NUC240__)
	#if(MAX_TIMER_HW_UNIT>0)
		HardwareTimer Timer0(0,TMR0_MODULE,CLK_CLKSEL1_TMR0_S_HXT);
	#endif
	#if(MAX_TIMER_HW_UNIT>1)
		HardwareTimer Timer1(1,TMR1_MODULE,CLK_CLKSEL1_TMR1_S_HXT);
	#endif
	#if(MAX_TIMER_HW_UNIT>2)
		HardwareTimer Timer2(2,TMR2_MODULE,CLK_CLKSEL1_TMR2_S_HXT);
	#endif
	#if(MAX_TIMER_HW_UNIT>3)
		HardwareTimer Timer3(3,TMR3_MODULE,CLK_CLKSEL1_TMR3_S_HXT);
	#endif
#elif defined(__NUC131__)
	#if(MAX_TIMER_HW_UNIT>0)
		HardwareTimer Timer0(0,TMR0_MODULE,CLK_CLKSEL1_TMR0_S_HXT);
	#endif
	#if(MAX_TIMER_HW_UNIT>1)
		HardwareTimer Timer1(1,TMR1_MODULE,CLK_CLKSEL1_TMR1_S_HXT);
	#endif
	#if(MAX_TIMER_HW_UNIT>2)
		HardwareTimer Timer2(2,TMR2_MODULE,CLK_CLKSEL1_TMR2_S_HXT);
	#endif
	#if(MAX_TIMER_HW_UNIT>3)
		HardwareTimer Timer3(3,TMR3_MODULE,CLK_CLKSEL1_TMR3_S_HXT);
	#endif
#endif

/** Timer u8TimerUnit numbers */
static voidTimerFuncPtr TimerFuncPtr[MAX_TIMER_HW_UNIT];

#ifdef __cplusplus
extern "C"
{
#endif
	#if(MAX_TIMER_HW_UNIT>0)
	void TMR0_IRQHandler(void) 
  { 
		if(TimerFuncPtr[0] != NULL)	TimerFuncPtr[0](); 
		TIMER_ClearIntFlag(TIMER0);
	}
	#endif
	
	#if(MAX_TIMER_HW_UNIT>1)
	void TMR1_IRQHandler(void) 
  { 
		if(TimerFuncPtr[1] != NULL)	TimerFuncPtr[1](); 
		TIMER_ClearIntFlag(TIMER1);
	}
	#endif
	
	#if(MAX_TIMER_HW_UNIT>2)
	void TMR2_IRQHandler(void) 
  { 
		if(TimerFuncPtr[2] != NULL)	TimerFuncPtr[2](); 
		TIMER_ClearIntFlag(TIMER2);
	}	
	#endif
	
	#if(MAX_TIMER_HW_UNIT>3)
	void TMR3_IRQHandler(void) 
  { 
		if(TimerFuncPtr[3] != NULL)	TimerFuncPtr[3](); 
		TIMER_ClearIntFlag(TIMER3);
	}	
	#endif
#ifdef __cplusplus
}
#endif

HardwareTimer::HardwareTimer(uint8_t timerNum, uint32_t moduleIdx, \
                                                           uint32_t clksel) 
{
  /* Decode timer number to base address of timer */
  if (timerNum >= MAX_TIMER_HW_UNIT) return;            
  TIMER_T * aaTimerHWUnit[] = 
  {
    #if(MAX_TIMER_HW_UNIT>0)
    TIMER0,
    #endif
    #if(MAX_TIMER_HW_UNIT>1)
    TIMER1,
    #endif
    #if(MAX_TIMER_HW_UNIT>2)
    TIMER2,
    #endif
    #if(MAX_TIMER_HW_UNIT>3)
    TIMER3,
    #endif
  };
  pTimerHWUnit = aaTimerHWUnit[timerNum];
  u8TimerUnit = timerNum;      
  TimerFuncPtr[u8TimerUnit] = NULL; 
  CLK_EnableModuleClock(moduleIdx);
  CLK_SetModuleClock(moduleIdx, clksel, NULL); 
}

/*
  ExpectFreq = F_Timer/((PreScaler + 1)*TCMP)
  
F_Timer = HCLK = 50 MHz
The best precision of ExpectFreq is achieve if TCMP is maximum.

=> TCMP = F_Timer/(ExpectFreq*(PreScaler + 1))

TCMP is max when preScaler is min.

**
  * @brief      Open Timer with Operate Mode and Frequency
  *
  * @param[in]  timer       The pointer of the specified Timer module. It could be TIMER0, TIMER1, TIMER2, TIMER3.
  * @param[in]  u32Mode     Operation mode. Possible options are
  *                         - \ref TIMER_ONESHOT_MODE
  *                         - \ref TIMER_PERIODIC_MODE
  *                         - \ref TIMER_TOGGLE_MODE
  *                         - \ref TIMER_CONTINUOUS_MODE
  * @param[in]  u32Freq     Target working frequency
  *
  * @return     Real timer working frequency
  *
  * @details    This API is used to configure timer to operate in specified mode and frequency.
  *             If timer cannot work in target frequency, a closest frequency will be chose and returned.
  * @note       After calling this API, Timer is \b NOT running yet. But could start timer running be calling
  *             \ref TIMER_Start macro or program registers directly.
  */
uint32_t HardwareTimer::open(uint32_t mode, uint32_t freq) 
{ 
  return TIMER_Open(pTimerHWUnit, mode, freq);
  
}

void HardwareTimer::close() 
{ 
    TIMER_Close(pTimerHWUnit);		
}

void HardwareTimer::start() 
{
		TIMER_Start(pTimerHWUnit);
}

void HardwareTimer::setPrescaleFactor(uint32_t factor) 
{    
    TIMER_SET_PRESCALE_VALUE(pTimerHWUnit, factor);
}

void HardwareTimer::setCompare(uint32_t val) 
{
     TIMER_SET_CMP_VALUE(pTimerHWUnit, val);
}

void HardwareTimer::attachInterrupt(void (*callback)(void)) 
{
   TimerFuncPtr[u8TimerUnit] = callback;
   TIMER_EnableInt(pTimerHWUnit);
   NVIC_EnableIRQ((IRQn_Type)((int)TMR0_IRQn + u8TimerUnit));
}

void HardwareTimer::detachInterrupt() 
{
   TimerFuncPtr[u8TimerUnit] = NULL;
   TIMER_DisableInt(pTimerHWUnit);
   NVIC_DisableIRQ((IRQn_Type)((int)TMR0_IRQn + u8TimerUnit));
}

uint32_t HardwareTimer::getModuleClock()
{
	return TIMER_GetModuleClock(pTimerHWUnit);
}

void HardwareTimer::clearIntFlag()
{
	TIMER_ClearIntFlag(pTimerHWUnit);
}
