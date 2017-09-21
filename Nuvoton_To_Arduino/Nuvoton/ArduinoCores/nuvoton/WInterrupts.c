/*
  Copyright (c) 2011-2012 Arduino.  All right reserved.

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

#include "WInterrupts.h"

#if defined(__M051__)

#define GP_NUM 8
typedef void (*interruptCB)(void);

static interruptCB callbacksP0[GP_NUM];
static interruptCB callbacksP1[GP_NUM];
static interruptCB callbacksP2[GP_NUM];
static interruptCB callbacksP3[GP_NUM];
static interruptCB callbacksP4[GP_NUM];

/* Configure PIO interrupt sources */
static void __initialize() 
{
	uint8_t i;
	for (i=0; i<GP_NUM; i++) 
  {
		callbacksP0[i] = NULL;
		callbacksP1[i] = NULL;
		callbacksP2[i] = NULL;
		callbacksP3[i] = NULL;
		callbacksP4[i] = NULL;
	}
  GPIO_SET_DEBOUNCE_TIME(GPIO_DBCLKSRC_LIRC, GPIO_DBCLKSEL_4);
	NVIC_EnableIRQ(GPIO_P0P1_IRQn);
	NVIC_EnableIRQ(GPIO_P2P3P4_IRQn);
}

void attachInterrupt(uint32_t pin, void (*callback)(void), uint32_t mode)
{
	static bool Lblenabled = 0;
  GPIO_T * LpPort;
  uint32_t LulPin;
  
	if (!Lblenabled)
  {
		__initialize();
		Lblenabled = 1;
	}

  LulPin = pin%8;
	LpPort = (GPIO_T *)(GPIO_BASE | ((pin/8)*0x40));
  
	if (LpPort == P0)
		callbacksP0[LulPin] = callback;
	else if (LpPort == P1)
		callbacksP1[LulPin] = callback;
	else if (LpPort == P2)
		callbacksP2[LulPin] = callback;
	else if (LpPort == P3)
		callbacksP3[LulPin] = callback;
	else if (LpPort == P4)
		callbacksP4[LulPin] = callback;		
  
  /* Set pin as input */
  pinMode(pin, INPUT_PULLUP);

	/* Enable interrupt */
	if(mode == FALLING)
		GPIO_EnableInt(LpPort, LulPin, GPIO_INT_FALLING);
	else if(mode == RISING)		
		GPIO_EnableInt(LpPort, LulPin, GPIO_INT_RISING);	
	else if(mode == CHANGE)
		GPIO_EnableInt(LpPort, LulPin, GPIO_INT_BOTH_EDGE);		
}

void detachInterrupt(uint32_t pin)
{
  GPIO_T * LpPort;
  uint32_t LulPin;
  
  LulPin = pin%8;
	LpPort = (GPIO_T *)(GPIO_BASE | ((pin/8)*0x40));
  
	// Disable interrupt
	GPIO_DisableInt(LpPort, LulPin);
	
	if (LpPort == P0)
		callbacksP0[LulPin] = NULL;
	else if (LpPort == P1)
		callbacksP1[LulPin] = NULL;
	else if (LpPort == P2)
		callbacksP2[LulPin] = NULL;
	else if (LpPort == P3)
		callbacksP3[LulPin] = NULL;
	else if (LpPort == P4)
		callbacksP4[LulPin] = NULL;		
}

#ifdef __cplusplus
extern "C" {
#endif

void GPIOP0P1_IRQHandler(void)
{
  uint32_t i;		
  for (i=0; i<GP_NUM; i++)
  {
    if(P0->ISRC & (1<<i))
    { 
      if (callbacksP0[i]) callbacksP0[i]();	  
      /* Clear interrupt flag */
      P0->ISRC = (1<<i);
    }
    if(P1->ISRC & (1<<i))
    {
      if (callbacksP1[i]) callbacksP1[i]();
      /* Clear interrupt flag */
      P1->ISRC = (1<<i);
    }
  }  
}
void GPIOP2P3P4_IRQHandler(void)
{
  uint32_t i;	
  //P00 ^= 1;
  for (i=0; i<GP_NUM; i++) 
  {
    if(P2->ISRC & (1<<i))
    {
      if (callbacksP2[i]) callbacksP2[i]();
      /* Clear interrupt flag */
      P2->ISRC = (1<<i);	  			
    }
    if(P3->ISRC & (1<<i)) 
    {
      if (callbacksP3[i] != NULL) 
      {
        callbacksP3[i]();
      }
      /* Clear interrupt flag */
      P3->ISRC = (1<<i);	
    }
    if(P4->ISRC & (1<<i)) 
    {
      if (callbacksP4[i]) callbacksP4[i]();
      /* Clear interrupt flag */ 			
      P4->ISRC = (1<<i);	
    } 	
  }     
}

#ifdef __cplusplus
}
#endif

#endif