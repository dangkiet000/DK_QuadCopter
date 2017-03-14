/*
  the rc_impuls is measured in the range of 1.024 to 2.048ms with the center at 1.5 ms
  the result is scaled to 1 - 129 - 255
*/

#define INT1_RISING  0x08			// rising edge triggers INT1
#define INT1_FALLING 0x0C			// falling edge triggers INT1
#define INT1_MASK    0xF3        // masks out bit3,2 in MCUCR

//global variables
volatile uint8_t RC_IMP_OVERFLOW;


ISR(INT1_vect, ISR_NOBLOCK)	  //since it's a lot of time the next INT1 should occur, the isr is not blocking other irq
									     //keep in mind that this will only work as long as the ir is edge-triggered
									     //and there are no glitches
{
 uint8_t temp=TCNT0;				  //read timer0, for accuracy the value is read always at first

 if((MCUCR & ~INT1_MASK) == INT1_RISING )
  {
   TCNT0=191;   				 	 // reset timer0
   Rc_Err_Cnt = 0;
   temp = MCUCR;               // enable INT1 at falling edge
   temp &=INT1_MASK;           // temp is needed to prevent level-triggering (MCUCR=xxxx00xx)
   MCUCR = temp | INT1_FALLING;
  }
 else
 {
  if(Rc_Err_Cnt > 1)
   Rc_Imp_Valid = RC_UNVALID;   // there is always 1 overflow
  else
   {
   Rc_Imp_Valid=RC_VALID;

   if (temp < 64) temp = 64;
    else if (temp > 191) temp = 191;

   temp -= 64;
   temp <<=1;
   temp++;
   Rc_Impuls = temp;
   }//end if(Rc_Err_Count...)

  temp  = MCUCR;             //enable INT1 at rising edge
  temp &= INT1_MASK;
  MCUCR = INT1_RISING | temp;

 }//endif(MCUCR...)
}//end of ISR(INT1...)

ISR(TIMER0_OVF_vect, ISR_NOBLOCK)
{
 if(Rc_Err_Cnt++ > MAX_RC_ERR_CNT) Rc_Imp_Valid=RC_UNVALID;

#ifdef LED_SERVICE
 Led_Service();
#endif

 wdt_reset();
}//end of ISR(TIMER1...)

void start_ppm_measure(void)
{
 Rc_Impuls=0;
 Rc_Err_Cnt=0;
 Rc_Imp_Valid=RC_UNVALID;

 MCUCR &= INT1_MASK;			//enable INT1 at rising edge
 MCUCR |= INT1_RISING;

 TCNT0=0;
 TCCR0=0x03;					//enable timer0 counting, prescaler = clkio/64

 TIMSK|=0x01;					//enable timer0 overflow irq
 GICR |=0x80;					//enable INT1
}//end  of start_ppm_measure()



