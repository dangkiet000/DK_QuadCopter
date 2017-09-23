/******************************************************************************
 * The MIT License
 *
 * Copyright (c) 2010 Perry Hung.
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
 * @file HardwareSerial.cpp
 * @brief Wirish serial port implementation.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "Arduino.h"

#if defined(__M051__)

  #if(UART_MAX_COUNT>0)
  ring_buffer rx_buffer = { { 0 }, 0, 0};
  HardwareSerial Serial(UART0,
                        UART0_MODULE,
                        CLK_CLKSEL1_UART_S_PLL,
                        1,
                        UART0_IRQn,
                        &rx_buffer);
  #endif

  #if(UART_MAX_COUNT>1)
  ring_buffer rx_buffer1 = { { 0 }, 0, 0};
  HardwareSerial Serial1(UART1,
                         UART1_MODULE,
                         CLK_CLKSEL1_UART_S_PLL,
                         1,
                         UART1_IRQn,
                         &rx_buffer1);
  #endif

#ifdef __cplusplus
extern "C" {
#endif

#if(UART_MAX_COUNT>0)
void UART0_IRQHandler(void)
{	 
	while(UART_GET_INT_FLAG(UART0, UART_IER_RDA_IEN_Msk))
	{
		int i = (unsigned int)(rx_buffer.head + 1) % SERIAL_BUFFER_SIZE;
  	unsigned char u8RxData;

    if (i != rx_buffer.tail) 
    {
      u8RxData = UART0->RBR;
    	rx_buffer.buffer[rx_buffer.head] = u8RxData;
    	rx_buffer.head = i;
  	}
	}
		
}
#endif

#if(UART_MAX_COUNT>1)
void UART1_IRQHandler(void)
{
	while(UART_GET_INT_FLAG(UART1, UART_IER_RDA_IEN_Msk))
	{
		int i = (unsigned int)(rx_buffer1.head + 1) % SERIAL_BUFFER_SIZE;
  	unsigned char u8RxData;
    
    if (i != rx_buffer1.tail) 
    {
      u8RxData = UART1->RBR;
    	rx_buffer1.buffer[rx_buffer1.head] = u8RxData;
    	rx_buffer1.head = i;
  	}
	}
		
}
#endif

#ifdef __cplusplus
}
#endif

#endif




#if(UART_MAX_COUNT>0)
void serialEvent() __attribute__((weak));
#endif

#if(UART_MAX_COUNT>1)
void serial1Event() __attribute__((weak));
#endif

void serialEventRun(void)
{
		#if(UART_MAX_COUNT>0)
    if (Serial.available()) serialEvent();
    #endif
    	
    #if(UART_MAX_COUNT>1)
    if (Serial1.available()) serial1Event();
    #endif	
}

HardwareSerial::HardwareSerial(UART_T *uart_device,
                               uint32_t u32ModuleClockIndex,															                              
                               uint32_t u32ClkSrc,
                               uint32_t u32ClkDiv,
                               IRQn_Type u32IrqId,
                               ring_buffer *rx_buffer)
{
    this->uart_device = uart_device;
    this->u32ModuleClockIdx = u32ModuleClockIndex;
    this->u32ClkSrc = u32ClkSrc;
    this->u32ClkDiv = u32ClkDiv;
    this->u32IrqId = u32IrqId;
    this->_rx_buffer = rx_buffer;
}

/*
 * Set up/tear down
 */

void HardwareSerial::begin(uint32_t baud)
{
  Ardu_PinConfigAsUART(uart_device);

#if defined(__NUC240__) | defined(__NUC131__) | defined(__M051__)
  /* Enable IP clock */       
  CLK_EnableModuleClock(u32ModuleClockIdx);    	
  
  /* Select IP clock source and clock divider */
  CLK_SetModuleClock(u32ModuleClockIdx, u32ClkSrc, CLK_CLKDIV_UART(u32ClkDiv));

  /* Reset IP */
  //SYS_ResetModule(UART_Desc[u32Idx].module);
  
  /* Enable Interrupt */
  UART_ENABLE_INT(uart_device, UART_IER_RDA_IEN_Msk);
  NVIC_EnableIRQ(u32IrqId);
  
  /* Configure UART and set UART Baudrate */
  UART_Open(uart_device, baud);
  
#endif
    
}

void HardwareSerial::end(void) 
{
  UART_Close(uart_device);
}

/*
 * I/O
 */

int HardwareSerial::read(void)
{  
  unsigned int u32Tail;
  
  u32Tail = _rx_buffer->tail;
  
  // if the head isn't ahead of the tail, we don't have any characters
  if (_rx_buffer->head == u32Tail)
  {
    return -1;
  }
  else
  {
    unsigned char c = _rx_buffer->buffer[u32Tail];
    u32Tail = (unsigned int)(u32Tail + 1) % SERIAL_BUFFER_SIZE;
    return c;
  }
}

int HardwareSerial::available(void)
{    
  unsigned int u32Head;
  
  u32Head = _rx_buffer->head;
    
  return (unsigned int)(SERIAL_BUFFER_SIZE + u32Head - _rx_buffer->tail) % SERIAL_BUFFER_SIZE;
}

size_t HardwareSerial::write(const uint8_t ch) 
{
#if defined(__NUC240__) | defined(__NUC131__) | (__M051__)
	while(uart_device->FSR & UART_FSR_TX_FULL_Msk);
	uart_device->THR = ch;
	return 1;
#endif
}

void HardwareSerial::flush(void) 
{
#if defined(__M451__)
  while( !(uart_device->FIFOSTS & UART_FIFOSTS_TXEMPTY_Msk) );
  
#elif defined(__M051__)
  /* Waiting for transmitting is complete in FIFO mode */
  while( !(uart_device->FSR & UART_FSR_TX_EMPTY_Msk) );
#endif
}


int HardwareSerial::peek( void ) 
{
  unsigned int u32Tail;
  
  u32Tail = _rx_buffer->tail;
  if (_rx_buffer->head == u32Tail)
  {
    return -1;
  } 
  else
  {
    return _rx_buffer->buffer[u32Tail];
  }
}
