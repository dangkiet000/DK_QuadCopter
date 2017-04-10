/*******************************************************************************
 * @version  V1.00
 * $Revision: 1
 * $Date: 09/04/2017
 * @brief: This is library which control Play Station 2 of Sony.
 * @Author: DangKiet

 @Limitation:
 - Only support Play Station 2 Dual Shock
 - Not support Analog Button Pressure Mode.

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

/********************         CONNECTION          ******************************
 *    PS2              |         MCU
 *    Data         <------>   MISO
 *    Attention    <------>   Chip Select
 *    Clock        <------>   SCK
 *    Power        <------>   3.3V
 *    GND          <------>   GND
 *    Acknowledge  <------>   Interrupt (Option)
 *
 * Notes:
 * 1. DATA and ACK pin is open-collector => Connect this pin to 4.7k pull-up
 * resistor. If this pin is not connect pull-up resistor, DATA is wrong and
 * byte1 will return 0x63 instead 0x73(analog mode).
*******************************************************************************/

/* To debug ps2 controller, uncomment these lines to print out debug to uart */
#define PS2X_DEBUG

#ifndef PS2X_DK_Lib_h
  #define PS2X_DK_Lib_h
#include "Arduino.h"
#endif

#include <stdio.h>
#include <stdint.h>

#include <avr/io.h>
#include "SPI.h"


/*******************************************************************************
**                      Define macro                                          **
*******************************************************************************/
/* Button mapping of byte5 */
#define PS2_L2          0xFE // BIT0 Need to confirm with new PS2 controller
#define PS2_R2          0xFD // BIT1
#define PS2_L1          0xFB // BIT2 Need to confirm with new PS2 controller
#define PS2_R1          0xF7 // BIT3
#define PS2_TRIANGLE    0xEF // BIT4
#define PS2_CIRCLE      0xBF // BIT6
#define PS2_CROSS       0xDF // BIT5
#define PS2_SQUARE      0x7F // BIT7

/* Button mapping of byte4 */
#define PS2_SELECT      0xFE // BIT0
#define PS2_L3          0xFD // BIT1
#define PS2_R3          0xFB // BIT2
#define PS2_START       0xF7 // BIT3
#define PS2_PAD_UP      0xEF // BIT4
#define PS2_PAD_RIGHT   0xDF // BIT5
#define PS2_PAD_DOWN    0xBF // BIT6
#define PS2_PAD_LEFT    0x7F // BIT7

/* Define index of ucAnalogJoystick[] */
#define PS2_RX      0x00
#define PS2_RY      0x01
#define PS2_LX      0x02
#define PS2_LY      0x03

/* Bit define for byte4 of command 0x43: Enter/Exit Config Mode */
#define ENTER_CONFIG_MODE    0x01
#define EXIT_CONFIG_MODE     0x00

/* Define Command(byte2 of header command) */
#define PS2_POLLING_CMD               0x42
#define PS2_ENTER_EXIT_CONFIG_CMD     0x43
/* These command only work in config mode(0xF3) */
#define PS2_SHOW_BUTTON_IN_POLL_CMD   0x41
#define PS2_SWITCH_MODE_CMD           0x44
#define PS2_STATUS_INFO_CMD           0x45
#define PS2_ENABLE_VIBRATION_CMD      0x4D
#define PS2_ENABLE_PRESSURE_CMD       0x4F

#define PS2_START_HEADER_CMD          0x01
#define PS2_END_HEADER_CMD            0x00

#define PS2_DUMMY_DATA                0x00

/* Define data of command 0x44: Switch modes between digital and analog */
#define PS2_SET_DIGITAL_MODE          0x00
#define PS2_SET_ANALOG_MODE           0x01
#define SWITCH_MODE_LOCK              0x03
#define SWITCH_MODE_USER              0x00

/* Define data of command 0x4D: Map bytes in the 0x42 command to actuate
   the vibration motors */
#define PS2_SMALL_MOTOR          0x00
#define PS2_LARGE_MOTOR          0x01
/*******************************************************************************
**                      Data Types                                            **
*******************************************************************************/
/* Definition of PS2 error id */
typedef enum ETag_PS2ErrorIdType
{
  WRONG_HEADER_DATA_RESPOND = 0,
  PS2_MODE_IS_UNDEFINED,
  PS2_LACK_PULLUP_RESISTOR
} PS2ErrorIdType;

/* Definition of PS2 mode */
typedef enum ETag_PS2ModeType
{
  PS2_DIGITAL_MODE = 0x41,
  PS2_ANALOG_JOYSTICK_MODE = 0x73,
  PS2_ANALOG_PRESSURE_MODE = 0x79,
  PS2_CONFIG_MODE = 0xF3,
  PS2_UNDEFINED_MODE = 0xFF
} PS2ModeType;

/* Data Structure for PS2 initialization */
typedef struct STag_PS2_Type
{
  /* PS2 controller mode */
  PS2ModeType enPS2Mode;

  /* Number of data Play Station 2 respond(depending on mode) */
  uint8_t ucNoOfData;

  /* This is byte 4th and 5th which contain: Digital Button State Mapping */
  uint8_t ucDigitalButton[2];

  /* This is byte 6th -> 9th which contain: Analog Joystick State Mapping */
  uint8_t ucAnalogJoystick[4];
} PS2_Type;
/*******************************************************************************
**                      Global Data                                           **
*******************************************************************************/
extern PS2_Type PS2x;
/*******************************************************************************
**                      Low Level Function Prototypes                         **
*******************************************************************************/
inline void PS2_CHIPSELECT_LOW(void);
inline void PS2_CHIPSELECT_HIGH(void);
uint8_t PS2_Transfer(uint8_t data);
void PS2_PortInit_as_SPI(void);
void PS2_SPIInit(void);
/*******************************************************************************
**                      Private Function Prototypes                           **
*******************************************************************************/
void PS2_DET_ErrorReport(PS2ErrorIdType enErrorId, uint8_t LucInfo);
bool PS2_TransferHeaderCommand(uint8_t LucCommand);
bool PS2_ConfigMode(bool blConfigMode);

#ifdef PS2X_DEBUG
void PS2_PrintData(uint8_t *LpCommandList, uint8_t LucLen);
#endif
/*******************************************************************************
**                      API Functions Prototypes                              **
*******************************************************************************/
bool PS2_Init(void);
bool PS2_SetMode(bool enMode);
void PS2_GetMode(void);
bool PS2_EnableVibration(void);

