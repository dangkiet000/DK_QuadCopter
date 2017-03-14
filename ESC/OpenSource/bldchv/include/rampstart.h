#define ROTOR_ALIGN_PATT 0x04 // the rotor must be aligned before the first commutatin step could be applied
#define ROTOR_ALIGN_FET  0x06	// this is done by switching the fets on with a pattern leading step 0
										// since there is no information for the current flowing, we will start with the lowest
								 		// PWM_value from the ramp_table								

	
	
#define ROTOR_ALIGN_DELAY 25000 //at abaout 50ms
	
			 		
// this is the ramp data for a 14-pol motor  ~260kv @20V
// don't forget to set PWM_REF_VOLT in <globals.h> accordingly

uint16_t DEG30_VALUE[32]={30303, 26348, 23307, 20895,
								  18935, 17312, 15945, 14778,
								  13770, 12891, 12117, 11431,
								  10819, 10268,  9771,  9320,
								   8909,  8533,  8187,  7868,
								   7573,  7299,  7044,  6807,
								   6585,  6377,  6181,  5998,
								   5825,  5661,  5507,  5361
								  };


uint8_t PWM_VALUE[32]={0x32, 0x33, 0x34, 0x35, 0x36, 0x38, 0x39, 0x3A,
							  0x3B, 0x3D, 0x3E, 0x3F, 0x40, 0x42, 0x43, 0x44,
							  0x45, 0x47, 0x48, 0x49, 0x4A, 0x4C, 0x4D, 0x4E,
							  0x4F, 0x50, 0x52, 0x53, 0x54, 0x55, 0x57, 0x58
							  };

// the values in PWM_VALUE must be adjusted to the actual supply-voltage
// this is done in init_ramp()
// if the rotor stalls during rampstart try first to adjust PWM_REF_VOLT in globals.h
// greater value gives more starting torque but draws also more current during startup

void init_ramp(void)
{
 volatile uint16_t i;
 uint16_t adc_read;
 uint16_t c_factor;

 //measure batterie voltage at first
 ADMUX   = _BV(REFS1) | _BV(REFS0) | _BV(MUX2) | _BV(MUX1)| _BV(MUX0) | _BV(ADLAR); // 2.5Vref Channel7 left adjusted
 ADCSRA  = _BV(ADEN)  | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);  // enable ADC, Prescaler = 128

 ADCSRA |= _BV(ADSC); 		  // start a single conversion
 while(ADCSRA & _BV(ADSC));  // wait for conversation to complete

 adc_read = ADC;

 while(ADC < UBATT_LOW) // if Ubatt lower than reference voltage stay here forever
  {
   led_set(0,LED_ON);led_set(1,LED_ON);led_set(2,LED_ON);
   i=40000;
   while(i) i--;
   led_set(0,LED_OFF);led_set(1,LED_OFF);led_set(2,LED_OFF);
   i=40000;
   while(i) i--;
  }//end of while(ADC < UBATT...)

  c_factor=adc_read/PWM_REF_VOLT;

  for(i=0;i< RAMP_LENGTH; i++)
   {
    PWM_VALUE[i] = (uint8_t)(((uint16_t)PWM_VALUE[i] << 6) / c_factor);
   }

}//end if init_ramp(...)

void rampstart(void)
{
  uint8_t cycle, com_index, com_nr;
  volatile uint16_t temp;  //volatile is is needed since the compiler optimze it otherwise to junk

 //align rotor
 Pwm_Width = PWM_VALUE[0];
 Pwm_Byte  = ROTOR_ALIGN_PATT;
 PORTD = ROTOR_ALIGN_FET;

 start_pwm();                     //force rotor to ALIGN_ROTOR_PATT position
 temp=ROTOR_ALIGN_DELAY;
 while (temp) temp--;    // delay 7 + temp*17 cycle

 stop_pwm();
 PORTD=0x00;

 Deg30_Time = DEG30_VALUE[0];
 Pwm_Width  = PWM_VALUE[0];
 Pwm_Min		= PWM_VALUE[RAMP_LENGTH-1];
 Com_Mode   = COM_SYNC_RUN;
 com_index  = Com_Index;


 start_commutation();


 for(cycle=0;cycle < RAMP_LENGTH;cycle++)
 {
  com_nr=RAMP_DELAY;			//specifies how often an index change must occur before advancing the table

  while(com_nr--)
  {
   while(com_index == Com_Index) temp--;   // wait for next commutation step
   com_index=Com_Index;                    // temp is junk, the gcc otherwise optimze the "empty loop" away
  }//end of while(com_nr...)

  Deg30_Time = DEG30_VALUE[cycle];
  Pwm_Width = PWM_VALUE[cycle];

 }//end of for(cycle..)

}//end of rampstart()






























































