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

/**
 *  @brief Wirish timer class.
 */

#ifndef _HARDWARETIMER_H_
#define _HARDWARETIMER_H_

#include "Arduino.h"

typedef void (*voidTimerFuncPtr)(void);

/** Timer mode. */
#define ONESHOT 		TIMER_ONESHOT_MODE
#define PERIODIC 		TIMER_PERIODIC_MODE
#define TOGGLE 			TIMER_TOGGLE_MODE
#define CONTINUOUS	TIMER_CONTINUOUS_MODE 

/** @brief Deprecated; use TIMER_OUTPUT_COMPARE instead. */
#define TIMER_OUTPUTCOMPARE TIMER_OUTPUT_COMPARE

/**
 * @brief Interface to one of the 16-bit timer peripherals.
 */
class HardwareTimer {
//private:
public:
    TIMER_T *pTimerHWUnit;
		uint8_t u8TimerUnit;
public:
    /**
     * @brief Construct a new HardwareTimer instance.
     * @param timerNum number of the timer to control.
     * @param moduleIdx is including TMR0_MODULE,TMR1_MODULE,TMR2_MODULE and TMR3_MODULE
     * @param clkse is including CLK_CLKSEL1_TMRx_S_XTAL       
  	 *													 CLK_CLKSEL1_TMRx_S_IRC10K       
  	 *													 CLK_CLKSEL1_TMRx_S_HCLK         
  	 *													 CLK_CLKSEL1_TMRx_S_IRC22M 
     */
    HardwareTimer(uint8_t timerNum, uint32_t moduleIdx, uint32_t clksel);
 /**
  * @brief This API is used to configure timer to operate in specified mode
  *        and frequency. If timer cannot work in target frequency, a closest
  *        frequency will be chose and returned.  
  * @param[in] u32Mode Operation mode. Possible options are
  *                 - \ref ONESHOT
  *                 - \ref PERIODIC
  *                 - \ref TOGGLE
  *                 - \ref CONTINUOUS
  * @param[in] u32Freq Target working frequency
  * @return Real Timer working frequency
  * @note After calling this API, Timer is \b NOT running yet. But could start timer running be calling
  *       \ref TIMER_Start macro or program registers directly
  */
		uint32_t open(uint32_t mode, uint32_t freq);
		
		void start(void);
	/**
  	* @brief This API stops Timer counting and disable the Timer interrupt function  
  	* @return None
  	*/		
    void close(void);
    
		void setPrescaleFactor(uint32_t factor);
		void setCompare(uint32_t compare);

    /**
     * @brief Attach an interrupt handler to the given u8TimerUnit.
     *
     * This interrupt handler will be called when the timer's counter
     * reaches the given u8TimerUnit compare value.
     *
     * @param u8TimerUnit the u8TimerUnit to attach the ISR to, from 1 to 4.
     * @param handler The ISR to attach to the given u8TimerUnit.
     * @see voidTimerFuncPtr
     */
    void attachInterrupt(voidTimerFuncPtr handler);

    /**
     * @brief Remove the interrupt handler attached to the given
     *        u8TimerUnit, if any.
     *
     * The handler will no longer be called by this timer.
     *
     * @param u8TimerUnit the u8TimerUnit whose interrupt to detach, from 1 to 4.
     * @see HardwareTimer::attachInterrupt()
     */
    void detachInterrupt();
    
		/**
	   * @brief This API is used to get the clock frequency of Timer	  
	   * @return Timer clock frequency
	   * @note This API cannot return correct clock rate if timer source is external clock input.
	   */   
    uint32_t getModuleClock();
    
		/**
		  * @brief This function clears the Timer time-out interrupt flag.		  
		  * @return None
		  */
    void clearIntFlag();

};

/* -- The rest of this file is deprecated. --------------------------------- */
#define MAX_TIMER_HW_UNIT 4

#if(MAX_TIMER_HW_UNIT>0)
	extern HardwareTimer Timer1;
#endif
#if(MAX_TIMER_HW_UNIT>1)
	extern HardwareTimer Timer2;
#endif
#if(MAX_TIMER_HW_UNIT>2)
	extern HardwareTimer Timer3;
#endif
#if(MAX_TIMER_HW_UNIT>3)
	extern HardwareTimer Timer4;
#endif

#endif
