/*
This software is part of my bldchv-project.
For details see http://rmmx.gmxhome.de/bldc .
Please feel free to question me at rmmx (1234) gmx.de  (1234) == @

You the user are free to use the entire source-code (main and header files)
for your own education. Feel free to modify the code to your own needs.
If you redistribute this files or a part of this, it would be nice,
if you could mention me as the original-author.



DISCLAIMER OF WARRANTY

THIS SOURCE CODE IS PROVIDED "AS IS" AND WITHOUT ANY WARRANTIES AS TO PERFORMANCE OR MERCHANTABILITY.
THIS SOURCE CODE IS PROVIDED WITHOUT ANY EXPRESS OR IMPLIED WARRANTIES WHATSOEVER.
NO WARRANTY OF FITNESS FOR A PARTICULAR OR GENERAL PURPOSE IS OFFERED.
THE USER IS ADVISED TO TEST THE SOURCE CODE THOROUGHLY BEFORE RELYING ON IT.
THE USER MUST ASSUME THE ENTIRE RISK OF USING THE SOURCE CODE OR PARTS OF IT.

*/


#include <avr/io.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>




#include "./include/globals.h"
#include "./include/pwm32k.h"
#include "./include/commutation.h"
#include "./include/led.h"
#include "./include/rampstart.h"
#include "./include/rcimp.h"
#include "./include/bemf.h"

volatile int32_t Pwm_Width32;	

uint8_t calc_pwm(void)
{
 uint16_t rc_pwm;
 uint16_t pwm_width;

 uint8_t ret;
 int16_t error;


 pwm_width =(uint32_t)(PWM_TOP+1-Pwm_Min);
 pwm_width <<=7;
 rc_pwm = pwm_width/(MAX_RC_IMP_WIDTH  - MIN_RC_IMP_WIDTH);
 rc_pwm *= Rc_Impuls;
 rc_pwm >>= 7;
 rc_pwm += Pwm_Min;

 error=(int16_t)rc_pwm-(int16_t)Pwm_Width;
 Pwm_Width32 +=(int32_t)error;
 ret=((uint8_t)(Pwm_Width32>>9));
 if(ret < Pwm_Min) ret=Pwm_Min;
 return(ret);
}//end of calc_pwm



int main(void)
{
 volatile uint16_t i,j;					  // volatile is needed for empty delay loop,
 uint8_t  error_condition;

                                  	  // the compiler would otherwise optimize it away
 init_led(); 						
 init_ramp();								  // calculate the pwm values accordingly to the batterie voltage													

 do{  									     // start of forever loop
 init_pwm();
 init_commutation();
 init_bemf();
 start_ppm_measure();

 wdt_enable(WDTO_15MS);					 // enable Watchdog timer

 sei();

 led_set(0,LED_BLINK); led_set(1,LED_OFF); led_set(2,LED_OFF);

 while(Rc_Imp_Valid == RC_UNVALID || Rc_Impuls > 7);  // wait for rc impuls become to min

 led_set(0,LED_OFF); led_set(0,LED_ON);     // become solid green
 led_set(1,LED_BLINK);

 while(Rc_Impuls  <  10);

 led_set(1,LED_OFF); led_set(1,LED_ON);     // LED2 become solid green ;

 init_pwm(); 										  //called again to recharge "FET-C"
 rampstart();

 Pwm_Width32=(int32_t)(Pwm_Min)<<9;
 Com_Mode=COM_FREERUN;
 error_condition = 0;

 led_set(2,LED_BLINK);

 do
  {
   if(Bemf_Zc_Missed > MAX_ZC_MISSED) 										 error_condition = 1;
   if(Rc_Imp_Valid == RC_UNVALID)      									 error_condition |=2;
   if(Rc_Impuls < 10)  						 									 error_condition |=4;
   if(Deg30_Time > MAX_DEG30_TIME || Deg30_Time < MIN_DEG30_TIME)  error_condition = 7;
   if(Com_Index==0) Pwm_Width=calc_pwm();
  }
 while(!error_condition);  // watch error condition

 stop_commutation();
 Com_Mode=COM_STOP;


 led_set(0,LED_OFF); led_set(1,LED_OFF); led_set(2,LED_OFF);


 switch (error_condition)
 {
  case 1: led_set(0,LED_ON); break;
  case 2: led_set(1,LED_ON); break;
  case 3: led_set(0,LED_ON);led_set(1,LED_ON); break;
  case 4: led_set(2,LED_ON); break;
  case 5: led_set(0,LED_ON);led_set(2,LED_ON); break;
  case 6: led_set(1,LED_ON);led_set(2,LED_ON); break;
  case 7: led_set(0,LED_ON);led_set(1,LED_ON);led_set(2,LED_ON); break;
 }// end of switch

 i=25;
 while (i--) { j=40000;  while (j--);} // give time to settle down, than start over

 } while(1); // forever
}//end of main()










































