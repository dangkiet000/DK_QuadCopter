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

#ifndef _WIRING_ANALOG_
#define _WIRING_ANALOG_

#ifdef __cplusplus
extern "C" {
#endif

#define ADC_HARDWARE_RESOLUTION 12
  
typedef enum EtagAnalogReference
{
  _DEFAULT = 0,
  INTERNAL,
  INTERNAL1V1,
  INTERNAL2V56,
  _EXTERNAL,
  INTERNAL_TEMP
} eAnalogReference;
/*
 * \brief Configures the reference voltage used for analog input (i.e. the value used as the top of the input range).
 * This function is kept only for compatibility with existing AVR based API.
 *
 * \param ulMmode Should be set to AR_DEFAULT.
 */
extern void analogReference( eAnalogReference ulMode );

/*
 * \brief Writes an analog value (PWM wave) to a pin.
 *
 * \param ulPin
 * \param ulValue
 */
extern void analogWrite(uint8_t u8Pin, uint16_t u16Value);

static inline uint32_t mapResolution(uint32_t value, uint32_t from, uint32_t to);

/*
 * \brief Reads the value from the specified analog pin.
 *
 * \param ulPin
 *
 * \return Read value from selected pin, if no error.
 */
extern uint32_t analogRead(uint8_t u8Pin);


/*
 * \brief Set the resolution of analogRead return values. Default is 10 bits (range from 0 to 1023).
 *
 * \param res
 */
extern void analogReadResolution(uint8_t res);

/*
 * \brief Set the resolution of analogWrite parameters. Default is 8 bits (range from 0 to 255).
 *
 * \param res
 */
extern void analogWriteResolution(uint8_t res);

/*******************************************************************************
                           EXTERNAL ARDUINO API
*******************************************************************************/


#define INTERNAL_BANDGAP_CHANNEL 7
/**
  * @brief Disable ADC module
  * @param[in] adc The pointer of the specified ADC module
  * @return None
  */
extern void analogReadClose(void);

/**
  * @brief Read value of power voltage 
  * @param[in] None
  * @return Vcc in milli Voltage
  */
extern uint32_t analogReadVcc(void);

#ifdef __cplusplus
}
#endif

#endif /* _WIRING_ANALOG_ */
