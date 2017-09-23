/******************************************************************************
 * @file     Arduino.h
 * @version  V2.00
 * $Date: 13/10/07 3:57p $ 
 * @brief    MINI51 Series Global Control for Arduino
 *
 * @note
 * Copyright (C) 2012 Nuvoton Technology Corp. All rights reserved.
 *;
 ******************************************************************************/
 

char x;

void Blinking(void)
{
//  P00 ^= 1;
//  digitalWrite(8, HIGH);
  x ^= 1;
}

void Blinking(long a)
{
  x = a;
  x--;
}
