uint8_t LED_BLINK_BITS;
uint16_t LED_COUNTER;

void Led_Service(void)    // this is called by Timer0 Overflow
{
 uint8_t bit_mask;

 if(!(LED_COUNTER--))
  {
   LED_COUNTER = LED_BLINK_DELAY;
   bit_mask = LED_BITS & LED_BLINK_BITS;
   LED_PORT ^= bit_mask;
  }//end of if(!(LED...)

}// end of Led_Service()

void led_set(uint8_t led, uint8_t mode)
{
 uint8_t bit_mask;

 bit_mask = 1 << led;
 bit_mask <<= LED_START;
 switch(mode)
 {
  case LED_OFF: LED_PORT |= bit_mask;
                LED_BLINK_BITS &= (~ bit_mask);
  				  	 break;
  					
  case LED_BLINK:
                LED_BLINK_BITS |=   bit_mask;
  case LED_ON:  LED_PORT &= (~bit_mask);
                break;
	
 }//end of switch(mode)

}//end of led_set(...)

void init_led(void)
{
 LED_DDR = LED_BITS;
 LED_BLINK_BITS =0;
 LED_COUNTER = LED_BLINK_DELAY;
 LED_PORT |= LED_BITS;      // turn all leds off
}// end of init_led()



