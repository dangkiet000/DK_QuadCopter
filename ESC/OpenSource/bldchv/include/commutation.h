// this is the output data for High_Low_fet in sequence (PortB2:0)
// a '1' means the high-fet is turned on and v.v.


// pwm both sides   (clamping bemf to H,L,H,L,....)
uint8_t PWM_BYTE_DATA[2][6]={
								     {0x31, //-011-001  H        // this is the "FREE_RUN" - data
								      0x23, //-010-011  L
								   	0x62, //-110-010  H
								   	0x46, //-100-110  L
								   	0x54, //-101-100  H
								   	0x15, //-001-101  L
								  	  },
								  	  {
									   0x75, //-111-101           // this the start up part
								  	 	0x01, //-000-001
								   	0x73, //-111-011
								   	0x02, //-000-010
								   	0x76, //-111-110
								   	0x04, //-000-100
								  	  }
								 	 };





// this is the output data for sda in sequence (PortD2:0)
// a '1' means fet "on" a '0" fet #off'

uint8_t ACTIVE_FET[6]={
							  0x03, // 0 1 1         			// this is the "FREE_RUN" - data
				 			  0x05, // 1 0 1
							  0x06, // 1 1 0
							  0x03, // 0 1 1
							  0x05, // 1 0 1
							  0x06  // 1 1 0				
							 };

ISR(TIMER1_COMPA_vect) // this is the main commutation isr, a new commutation is started here
{
 uint8_t active_fet;	  //PortD2_0 Data ->  fet is on/off

 stop_pwm();
 if(ACSR & 0x08)       //comparator irq still enabled
  {
   ACSR &=0XF7;        //disable bemf-irq
   Bemf_Zc_Missed++;
  }//end of if(ACSR..)

 if(Com_Mode != COM_STOP)
  {
   OCR1B=(Deg30_Time>>1); 								// half of Deg30_Time --> 15DEG_TIME
   OCR1A=(Deg30_Time<<1);  							// 2 times of Deg30_Time

   if(Com_Mode == COM_FREERUN)                                  	
    {
     Pwm_Byte=PWM_BYTE_DATA[0][Com_Index]; 	 	// prepare next output for "FREE_RUN"
     active_fet=ACTIVE_FET    [Com_Index];
     TIMSK |= 0x08;										// enabling irq: OCIE1B  (BEMF_measuring)
    }
   else
    {
     active_fet=0x07;									// all !! fets are always on
     Pwm_Byte=PWM_BYTE_DATA[1][Com_Index]; 	 	// prepare next output for startup
    }

   if(++Com_Index >5) Com_Index=0;

   PORTD=active_fet;										// turn fets on
   start_pwm();											// starting the pwm "in sync" with the next cycle

  }//endif(MODE != ...)

}// end of ISR(TIMER1_COMPA_vect)



void init_commutation(void)
{
 TCCR1B=0x00;				//stop timer1
 TCNT1=0x0000; 			//set timer1 to 0
 TCCR1A=0x00;				//set to ctc-mode , ocr1A
 TCCR1B=0x08;

 Com_Index=0;  			//start with the first cycle
 Com_Mode=COM_STOP;	   //no commutation yet
}//end of init_commutation

void start_commutation(void)
{
 TIMSK |= 0x10;			//enabling irq: OCIE1A
 TCCR1B |= 0x01;			//start timer1; prescaler 1 --> no prescaling

// since the first commutation starts AFTER the first TIMER1_COMPA_IRQ
// there will be a gap of 2*DEG30_TIME at the very first cycle
// to overcome this the irq is called directly

  asm volatile("cli\n\t"
 				   "rcall __vector_6\n\t"		// TIMER1_COMPA_vect
					::);

}//end of start_commutation(void)


void stop_commutation(void)
{
 TCCR1B &= 0xFE;	//stop timer1
 TIMSK  &= 0xE7;  //disable timer1_irqs
 stop_pwm();
 PORTD=0x00;	// turn off the FETs immediately
 PORTB=0x00;
}



