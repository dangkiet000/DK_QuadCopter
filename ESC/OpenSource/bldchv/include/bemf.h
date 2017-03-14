//the bemf_mask[] holds for every commutation step the adc-mux and edge values
//ACSR1:0 = 1,0 AC-irq on falling edge of ac-output -> rising  bemf
//ACSR1:0 = 1,1 AC-irq on rising  edge of ac-output -> falling bemf

static uint8_t BEMF_MASK[6]= { 0x02, // adc-mux=0000 = A; ac-mode 0010, rising  at Phase A    (5)
                               0x23, // adc-mux=0010 = C; ac-mode 0011, falling at Phase C    (0)
                               0x12, // adc-mux=0001 = B; ac-mode 0010, rising  at Phase B    (1)
                               0x03, // adc-mux=0000 = A; ac-mode 0011, falling at Phase A    (2)
                               0x22, // adc-mux=0010 = C; ac-mode 0010, rising  at Phase C    (3)
                               0x13, // adc-mux=0001 = B; ac-mode 0011, falling at Phase B    (4)
                             };


volatile uint16_t BEMF_T0, BEMF_T1;   // holds the time from commutation start to zero crossing point

ISR(TIMER1_COMPB_vect, ISR_NOBLOCK)    // only called in Com_Mode == FREERUN
{
 uint8_t  temp;

 TIMSK &= 0xF7;								// disable further OCR1B_Irqs
 temp  = BEMF_MASK[Com_Index];
 ACSR = (temp & 0x03); 						// set ac-irq mode
 ADMUX= (temp >> 4); 	 					// set ADMUX Eingang
 ACSR |= 0x10;  		 		   			// clear AC-irq flag
 ACSR |= 0x08;   		 						// enable bemf-irq

}// end of ISR(TIMER1_COMPB_vect)



ISR(ANA_COMP_vect)
{
 uint32_t long_temp;
 uint16_t deg30;

 BEMF_T0	= TCNT1;		 	  			 		 // read timer1

 TCNT1=0x0000;						    		 // reset Timer1 to zero
 ACSR &= 0xF7;				  			 		 // disable bemf-irq

 sei();						  					 // enable all irq

 PORTD |= 0x80;								 //set zero crossing test signal at PD7

 // calculate the new commutation point
 deg30 = (BEMF_T0 + (3* Deg30_Time)) >> 2;   // new time is an average of the 3 times the last Deg30_Time and actual crossing time

 Deg30_Time = deg30;       					   // save the calculated deg30 time

 //calculate the phase advance angle corrected times
 long_temp = (uint32_t)deg30 * (uint32_t)ADVANCE_ANGLE;  // multiply by scaled angle
 long_temp >>=8;                                         // division through 256
 deg30 = long_temp & 0xFFFF;


 //deg30 -= P_DELAY;   					  // substract the time needed to calculate all output

 Deg30_Time = (Deg30_Time+deg30)>>1;

 OCR1A = deg30;							       	// set commutation point

 Bemf_Zc_Missed = 0;

}//end of ISR(ANA_COMP_vect)


void init_bemf(void)
{
 ADCSRA = 0x00;		  // disable adc-converter
 SFIOR |= 0x08;    	  // enable ADC_MUX as Input for AIN1
 ACSR  &= 0xF7;		  // disable AC-IRQ
 ACSR  &= 0x7F;        // turn AC-power on

 Bemf_Zc_Missed = 1;   //starting values
}//end of init_bemf()








































