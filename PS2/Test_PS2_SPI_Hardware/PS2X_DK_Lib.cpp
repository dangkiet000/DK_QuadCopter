/*******************************************************************************
 * @version  V1.00
 * $Revision: 1
 * $Date: 09/04/2017
 * @brief: This is library which control Play Station 2 of Sony.
 * @Author: DangKiet

 @Limitation:
 - Only support Play Station 2 Dual Shock
 - Not support Analog Button Pressure mode.

BASE ON:
Super amazing PS2 controller Arduino Library v1.8
http://www.billporter.info/?p=240

List PS2 command:
http://store.curiousinventor.com/guides/PS2/

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or(at your option) any later
version.
This program is distributed in the hope that it will be useful,but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.
See the GNU General Public License for more details.
<http://www.gnu.org/licenses/>
*******************************************************************************/

#include "PS2X_DK_lib.h"
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>

#include "SPI.h"
#include "Arduino.h"

/*******************************************************************************
**                      Global Datas                                          **
*******************************************************************************/
uint8_t GaaPS2Data[21];
PS2_Type PS2x;

/*******************************************************************************
**                      Low Level Function Prototypes                         **
*******************************************************************************/
inline void PS2_CHIPSELECT_LOW(void)
{
  digitalWrite(10, LOW);
  //delayMicroseconds(10);
}

inline void PS2_CHIPSELECT_HIGH(void)
{
  digitalWrite(10, HIGH);
  //delay(10);
}

uint8_t PS2_Transfer(uint8_t data)
{
  return SPI.transfer(data);
}

void PS2_PortInit_as_SPI(void)
{
  SPI.begin();
  pinMode(10, OUTPUT);
}

void PS2_SPIInit(void)
{
  SPI.begin();

  /* PS2 Max speed = 500Kb => SPI Clock < 500.000 Hz */
  SPI.setClockDivider(SPI_CLOCK_DIV128);

  /* SPI must be LSB */
  SPI.setBitOrder(LSBFIRST);

  /* 1. PS2 Clock Avtive High RISING
  => CPOL = 1
  => SPI_MODE2 or SPI_MODE3
     2.Data are captured on clock's rising edge and data
       is output on a falling edge
  =>  CPHA = 1
  */
  SPI.setDataMode(SPI_MODE3);

}

/*******************************************************************************
**                      Private Function Prototypes                           **
*******************************************************************************/
void PS2_DET_ErrorReport(PS2ErrorIdType enErrorId, uint8_t LucInfo)
{
  switch(enErrorId)
  {
    //PS2_LEDERROR_ON();
    case WRONG_HEADER_DATA_RESPOND:
      #ifdef PS2X_DEBUG
      Serial.println("Byte1 of header command respond is incorrect!");
      #endif
      break;
    case PS2_MODE_IS_UNDEFINED:
      #ifdef PS2X_DEBUG
      Serial.print("Device Mode is wrong: ");
      Serial.print(LucInfo);
      Serial.println(".It should be 0x41 or 0x73 or 0x79 ro 0xF3.");
      #endif
      break;
    case PS2_LACK_PULLUP_RESISTOR:
      #ifdef PS2X_DEBUG
      Serial.print("Lacking pull-up resistor PS2_Data pin. ");
      Serial.println("Please connect PS2_Data pin with 4.7k pull-up resistor.");
      Serial.print("Device Mode is wrong: ");
      Serial.println(LucInfo);
      #endif
      break;
    default: break;
  }

}

bool PS2_TransferHeaderCommand(uint8_t LucCommand)
{
  /*
  ______________________________________________________________________________
  |Byte #  |Source |Example|  Explanation                                      |
  |        |type   |value  |                                                   |
  |________|_______|_______|___________________________________________________|
  |1st byte|Command|0x01   |New packets always start with 0x01 ... 0x81 for    |
  |        |       |       |memory card?                                       |
  |        |_______|_______|___________________________________________________|
  |        |Data   |0xFF   |Always 0xFF                                        |
  |________|_______|_______|___________________________________________________|
  |2nd byte|Command|0x42   |Main command: can either poll controller or        |
  |        |       |       |configure it.                                      |
  |        |_______|_______|___________________________________________________|
  |        |Data   |0x41   |Device Mode: the high nibble (4) indicates the mode|
  |        |       |       |(0x4 is digital, 0x7 is analog, 0xF is config mode,|
  |        |       |       | (lynxmotion calls 0xF 'DS Native Mode').          |
  |        |       |       |Lower nibble (1) is how many 16 bit words follow   |
  |        |       |       |the header, although the playstation doesn't always|
  |        |       |       |wait for all these bytes.                          |
  |________|_______|_______|___________________________________________________|
  |3rd byte|Command|0x00   |Always 0x00.                                       |
  |        |_______|_______|___________________________________________________|
  |        |Data   |0x5A   |Always 0x5A, this value appears in several         |
  |        |       |       |non-functional places.                             |
  |________|_______|_______|___________________________________________________|
  */
  /* 1. Send header command */
  GaaPS2Data[0] = PS2_Transfer(PS2_START_HEADER_CMD);

  if(GaaPS2Data[0] != 0xFF)
  {
    /* DET error detected */
    PS2_DET_ErrorReport(WRONG_HEADER_DATA_RESPOND, 0);
    PS2x.enPS2Mode = PS2_UNDEFINED_MODE;
    return false;
  }

  GaaPS2Data[1] = PS2_Transfer(LucCommand);

  if(GaaPS2Data[1] == 0x41)
  {
    PS2x.enPS2Mode = PS2_DIGITAL_MODE;
    PS2x.ucNoOfData = 5;
  }
  else if(GaaPS2Data[1] == 0x73)
  {
    PS2x.enPS2Mode = PS2_ANALOG_JOYSTICK_MODE;
    PS2x.ucNoOfData = 6;
  }
  else if(GaaPS2Data[1] == 0x79)
  {
    PS2x.enPS2Mode = PS2_ANALOG_PRESSURE_MODE;
    PS2x.ucNoOfData = 18;
  }
  else if(GaaPS2Data[1] == 0xF3)
  {
    PS2x.enPS2Mode = PS2_CONFIG_MODE;
    PS2x.ucNoOfData = 6;
  }
  else if(GaaPS2Data[1] == 0x63)
  {
    PS2x.enPS2Mode = PS2_UNDEFINED_MODE;
    /* DET error detected */
    PS2_DET_ErrorReport(PS2_LACK_PULLUP_RESISTOR, GaaPS2Data[1]);
    return false;
  }
  else
  {
    PS2x.enPS2Mode = PS2_UNDEFINED_MODE;
    /* DET error detected */
    PS2_DET_ErrorReport(PS2_MODE_IS_UNDEFINED, GaaPS2Data[1]);
    return false;
  }

  GaaPS2Data[2] = PS2_Transfer(PS2_END_HEADER_CMD);
  return true;
}

/* Enter/Exit PS2 configuration mode */
bool PS2_ConfigMode(bool blConfigMode)
{
  /*
  This can poll the controller like 0x42, but if the first command byte is 1,
  it has the effect of entering config mode (0xF3), in which the packet response
  can be configured. If the current mode is 0x41, this command only needs to be
  5 bytes long. Once in config / escape mode, 0x43 does not return button states
  anymore, but 0x42 still does (except for pressures). Also, all packets have
  will 6 bytes of command / data after the header.
  _______________________________________________________________________________
  Byte #       | 1  2  3 |4    5 |6  7  8  9 |10 11 12 13 14 15 16 17 18 19 20 21
  _____________|_________|____ __|___________|___________________________________
  Command (hex)| 01 43 00|0x01|00|00 00 00 00|00 00 00 00 00 00 00 00 00 00 00 00
  Data (hex)   | FF 79 5A|FF  |FF|7F 7F 7F 7F|00 00 00 00 00 00 00 00 00 00 00 00
  _____________|_________|____|__|___________|___________________________________
  Section      | Header  |Digital|Analog joy |Button pressures (0xFF = fully pressed)
  Analog map   |                 |RX RY LX LY R  L  U  D  Tri O X Sqr L1 R1 L2 R2

  -Byte4: command = 0x01 enters config mode.
          command = 0x00 exit config mode. If 0x00 and already out of config
          mode, it behaves just like 0x42, but without vibration motor control.
  */
  uint8_t i;
  #ifdef PS2X_DEBUG
  uint8_t LaaDataOut[9];
  #endif
  /* Set attention pin as LOW to start communication */
  PS2_CHIPSELECT_LOW();

  /* 1. Send header command */
  PS2_TransferHeaderCommand(PS2_ENTER_EXIT_CONFIG_CMD);

  /* 2. Send data config */
  GaaPS2Data[3] = PS2_Transfer(blConfigMode);

  /* 3. Send Dummy data */
  for (i=0; i<PS2x.ucNoOfData; i++)
  {
    GaaPS2Data[3+1+i] = PS2_Transfer(PS2_DUMMY_DATA);
  }

  /* Set attention pin as HIGH to end communication */
  PS2_CHIPSELECT_HIGH();

  #ifdef PS2X_DEBUG
  Serial.print("PS2_ConfigMode: ");
  if(blConfigMode == ENTER_CONFIG_MODE)
  {
    Serial.println("ENTER");
  }
  else
  {
    Serial.println("EXIT");
  }
  LaaDataOut[0] = PS2_START_HEADER_CMD;
  LaaDataOut[1] = PS2_ENTER_EXIT_CONFIG_CMD;
  LaaDataOut[2] = PS2_END_HEADER_CMD;
  LaaDataOut[3] = blConfigMode;
  for (i=0; i<PS2x.ucNoOfData-1; i++)
  {
    LaaDataOut[3+1+i] = PS2_DUMMY_DATA;
  }
  PS2_PrintData(LaaDataOut, 3+PS2x.ucNoOfData);
  #endif

  /* Set attention pin as LOW to start communication */
  PS2_CHIPSELECT_LOW();
  /*************************************************************************
   *  At this time, we have to send one more sequence to update and verify
   *  PS2 Mode
   ************************************************************************/
  PS2_GetMode();

  if((PS2x.enPS2Mode == PS2_CONFIG_MODE) && (blConfigMode == ENTER_CONFIG_MODE))
  {
    return true;
  }
  else if((PS2x.enPS2Mode != PS2_CONFIG_MODE) && (blConfigMode == EXIT_CONFIG_MODE))
  {
    return true;
  }

  return false;
}

#ifdef PS2X_DEBUG
void PS2_PrintData(uint8_t *LpCommandList, uint8_t LucLen)
{
  uint8_t i;
  Serial.println("OUT:IN");
  for (i=0; i< LucLen; i++)
  {
    Serial.print(LpCommandList[i], HEX);
    Serial.print(":");
    Serial.print(GaaPS2Data[i], HEX);
    Serial.print(" ");
  }
  Serial.println("");
}
#endif

/*******************************************************************************
**                      API Functions                                         **
*******************************************************************************/
bool PS2_Init(void)
{
  bool LddReturn;

  LddReturn = true;

  PS2_PortInit_as_SPI();

  PS2_SPIInit();

  //PS2_LEDERROR_OFF();

  PS2x.enPS2Mode = PS2_DIGITAL_MODE;

  PS2x.ucNoOfData = 2;

  PS2_CHIPSELECT_HIGH();

  LddReturn = PS2_SetMode(PS2_SET_ANALOG_MODE);
  LddReturn = PS2_EnableVibration();

  #ifdef PS2X_DEBUG
  if(LddReturn != true) Serial.println("PS2_SET_ANALOG_MODE: FALSE");
  else Serial.println("PS2_SET_ANALOG_MODE: TRUE");
  #endif

  return LddReturn;
}

bool PS2_EnableVibration(void)
{
  /*
  Only works after the controller is in config mode (0xF3).
  _______________________________________________________
  |Byte #       | 1   2   3 | 4     5   | 6   7   8   9 |
  |_____________|___________|___________|_______________|
  |Command (hex)| 01  4D  00|0x00  0x01 | FF  FF  FF  FF|
  |Data (hex)   | FF  F3  5A|0x00  0x01 | FF  FF  FF  FF|
  |_____________|___________|___________|_______________|
  |Section      |  Header   | Config parameters         |
  |_____________|___________|___________________________|
  -Byte4: 0x00 maps the corresponding byte in 0x42 to control the small motor.
          0xFF in the 0x42 command will turn it on, all other values turn it
          off.
  -Byte5: 0x01 maps the corresponding byte in 0x42 to control the large motor.
          The power delivered to the large motor is then set from 0x00 to 0xFF
          in 0x42. 0x40 was the smallest value that would actually make the
          motor spin for us.
  -0xFF:  Disables, and is the default value when the controller is first
          connected. The data bytes just report the current mapping.
  -Things don't always work if more than one command byte is mapped to a motor.
  */

  #ifdef PS2X_DEBUG
  uint8_t LaaDataOut[9];
  #endif
  uint8_t i;
  uint8_t LddReturn;

  LddReturn = PS2_ConfigMode(ENTER_CONFIG_MODE);

  /* Set attention pin as LOW to start communication */
  PS2_CHIPSELECT_LOW();

  /* 1. Send header command */
  PS2_TransferHeaderCommand(PS2_ENABLE_VIBRATION_CMD);

  /* 2. Send data config */
  GaaPS2Data[3] = PS2_Transfer(PS2_SMALL_MOTOR);
  GaaPS2Data[4] = PS2_Transfer(PS2_LARGE_MOTOR);

  /* 3. Send Dummy data */
  for (i=0; i<PS2x.ucNoOfData-2; i++)
  {
    GaaPS2Data[4+1+i] = 0xFF;
  }
  /* Set attention pin as HIGH to start communication */
  PS2_CHIPSELECT_HIGH();

  #ifdef PS2X_DEBUG
  Serial.println("PS2_EnableVibration");
  LaaDataOut[0] = PS2_START_HEADER_CMD;
  LaaDataOut[1] = PS2_ENABLE_VIBRATION_CMD;
  LaaDataOut[2] = PS2_END_HEADER_CMD;
  LaaDataOut[3] = PS2_SMALL_MOTOR;
  LaaDataOut[4] = PS2_LARGE_MOTOR;
  for (i=0; i<PS2x.ucNoOfData-2; i++)
  {
    LaaDataOut[3+2+i] = PS2_DUMMY_DATA;
  }
  PS2_PrintData(LaaDataOut, 3+PS2x.ucNoOfData);
  #endif

  /* Verify motor vibration ??? */
  LddReturn = PS2_ConfigMode(EXIT_CONFIG_MODE);

  return LddReturn;
}

bool PS2_SetMode(bool enMode)
{
  /* Only works after the controller is in config mode (0xF3).
  ______________________________________________________
  Byte #       | 1   2   3 | 4     5   | 6   7   8   9  |
  _____________|___________|___________|________________|
  Command (hex)| 01  44  00| 0x01  0x03| 00  00  00  00 |
  Data (hex)   | FF  F3  5A| 00    00  | 00  00  00  00 |
  _____________|___________|___________|________________|
  Section      |  Header   | Config parameters          |


  -Set analog mode : Byte4:command = 0x01
  -Set digital mode: Byte4:command = 0x00
  -If Byte5.command is 0x03, the controller is locked, otherwise the user can
  change from analog to digital mode using the analog button on the controller.
  Note that if the controller is already setup to deliver pressure values,
  toggling the state with the controller button will revert the controller back
  into not sending pressures.
  Some controllers have a watch-dog timer that reverts back into digital mode
  if a command is not received within a second or so.
  */
  #ifdef PS2X_DEBUG
  uint8_t LaaDataOut[9];
  #endif
  uint8_t i;
  uint8_t LddReturn;

  LddReturn = PS2_ConfigMode(ENTER_CONFIG_MODE);

  /* Set attention pin as LOW to start communication */
  PS2_CHIPSELECT_LOW();

  /* 1. Send header command */
  PS2_TransferHeaderCommand(PS2_SWITCH_MODE_CMD);

  /* 2. Send data config */
  GaaPS2Data[3] = PS2_Transfer(enMode);
  GaaPS2Data[4] = PS2_Transfer(SWITCH_MODE_LOCK);

  /* 3. Send Dummy data */
  for (i=0; i<PS2x.ucNoOfData-2; i++)
  {
    GaaPS2Data[3+2+i] = PS2_DUMMY_DATA;
  }
  /* Set attention pin as HIGH to start communication */
  PS2_CHIPSELECT_HIGH();

  LddReturn = PS2_ConfigMode(EXIT_CONFIG_MODE);
  #ifdef PS2X_DEBUG
  Serial.print("PS2_SetMode: ");
  if(enMode == PS2_SET_DIGITAL_MODE)
  {
    Serial.println("DIGITAL_MODE");
  }
  else
  {
    Serial.println("ANALOG_MODE");
  }
  LaaDataOut[0] = PS2_START_HEADER_CMD;
  LaaDataOut[1] = PS2_SWITCH_MODE_CMD;
  LaaDataOut[2] = PS2_END_HEADER_CMD;
  LaaDataOut[3] = enMode;
  LaaDataOut[4] = SWITCH_MODE_LOCK;
  for (i=0; i<PS2x.ucNoOfData-2; i++)
  {
    LaaDataOut[3+2+i] = PS2_DUMMY_DATA;
  }
  PS2_PrintData(LaaDataOut, 3+PS2x.ucNoOfData);
  #endif

  /* Verify PS2 Mode */
  PS2_GetMode();

  /* If the high nibble is 0x7: Analog mode */
  if(((PS2x.enPS2Mode >> 4) == 0x07) && (enMode == PS2_SET_ANALOG_MODE))
  {
    LddReturn = true;
  }
  /* If the high nibble is 0x4: Digital mode */
  else if(((PS2x.enPS2Mode >> 4) == 0x04) && (enMode == PS2_SET_DIGITAL_MODE))
  {
    LddReturn = true;
  }
  else
  {
    LddReturn = false;
  }

  return LddReturn;
}

void PS2_GetMode(void)
{
  #ifdef PS2X_DEBUG
  uint8_t LaaDataOut[9];
  #endif
  uint8_t i;
  /* Set attention pin as LOW to start communication */
  PS2_CHIPSELECT_LOW();

  /* 1. Send header command */
  PS2_TransferHeaderCommand(PS2_POLLING_CMD);

  /* 2. Send Dummy data */
  for (i=0; i<PS2x.ucNoOfData; i++)
  {
    GaaPS2Data[3+i] = PS2_Transfer(PS2_DUMMY_DATA);
  }

  /* Set attention pin as HIGH to start communication */
  PS2_CHIPSELECT_HIGH();

  #ifdef PS2X_DEBUG
  Serial.println("PS2_GetMode");

  LaaDataOut[0] = PS2_START_HEADER_CMD;
  LaaDataOut[1] = PS2_POLLING_CMD;
  LaaDataOut[2] = PS2_END_HEADER_CMD;
  for (i=0; i<PS2x.ucNoOfData; i++)
  {
    LaaDataOut[3+i] = PS2_DUMMY_DATA;
  }
  PS2_PrintData(LaaDataOut, 3+PS2x.ucNoOfData);
  #endif
}

