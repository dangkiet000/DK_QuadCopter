// global definitions
// all global defines are written in capital letter

// motor control
#define COM_STOP 			0
#define COM_SYNC_RUN 	1
#define COM_FREERUN 		2
#define MAX_DEG30_TIME  15000    // the min rpm in FREERUN MODE is determed by this ( ~380 rpm for a 14pol motor)
#define MIN_DEG30_TIME  400      // the max rpm in FREERUN MODE is determed by this ( ~10000 rpm for a 14pol motor)
#define MAX_ZC_MISSED 	12			// there should no more than MAX_ZC_MISSED in FREERUN MODE
#define P_DELAY 		   140 	   //(~17,5us) delay caused by calculation
#define ADVANCE_ANGLE   266      // angle of 'early commutation'  (256 =0°,266 = 5° later, 288 = 15° later)
											// = (30-ADVANCE_ANGLE/4)/ 30 * 256;
				 							// subject to 'fine-tuning'	
                                 // strangely my actual motor needs a little bit of late timing
				 							
#define PWM_TOP    		250      // pwm is always "on" int the range PWM _TOP <= pwm <= 0xFF;
											
/* these are the values for 20-40Volt */											
#define PWM_REF_VOLT    0x130  	// this specifies for which batterie voltage the ramp table is precalculated
											// -> (Ubatt * 0.0485) * 1024/2.56   (0x184)
#define UBATT_LOW       0x5740   // lowest Ubatt (18V)   calculate as PWM_REF_VOLT *64 !!!


/* these are the values for <=20Volt, works only if hardware is adapted
							
#define PWM_REF_VOLT    0x9B  	// this specifies for which batterie voltage the ramp table is precalculated
											// -> (Ubatt * 0.0485) * 1024/2.56   (~8Volt)
#define UBATT_LOW       0x3559   // lowest Ubatt (11V)   calculate as PWM_REF_VOLT *64 !!!
*/



#define RAMP_LENGTH 12        	// guess it, defines the length of te starting ramp
#define RAMP_DELAY  10  	  		//this specifies how often a commutation change must occur before the ramp_values are changed
											//this leads directly to the acceleration speed
											
// rc impuls measurement											
#define MAX_RC_IMP_WIDTH  255		// width of a 2.048ms impuls
#define MIN_RC_IMP_WIDTH  1		// width of a 1.024ms impuls		
#define MAX_RC_ERR_CNT 		15		// defines the maximum rc error count before an error is assumed
#define RC_UNVALID 		 	 0		// the measured puls is wrong
#define RC_VALID 1

// miscellaneous
#define LED_SERVICE   		      //use T0OV-ISR for blinking
#define LED_PORT		 	PORTC
#define LED_DDR   	 	DDRC
#define LED_BITS  	 	0x38		//output bits at LED_PORT
#define LED_START 		0x03    	//bit of first LED
#define LED_OFF		 	0
#define LED_ON        	1
#define LED_BLINK     	2
#define LED_BLINK_DELAY 50



//global variables declarations
//all global variables start with a first capital letter


// motor control
volatile uint8_t  Pwm_Width;     // stores the width of the actual pwm_signal
volatile uint8_t  Pwm_Byte;      // holds the "Fet-Pattern"

volatile uint8_t  Com_Index;		// stores the actual index of the commutation cycle
volatile uint8_t  Com_Mode;		// signals the controller mode STOP,SYNC_MODE,FREERUN
volatile uint16_t Deg30_Time;  	// TIME for 30 degrees of commutation

volatile uint8_t  Bemf_Zc_Missed;  // is > 0 if zero crossings are missed, is counted up in commutation irq

volatile uint8_t  Pwm_Min;       // matches the last pwm-value in ramp_table

volatile uint16_t Batt_Voltage;  // ADC-Reading of the battery voltage


// rc impuls measurement											
volatile uint8_t Rc_Err_Cnt;
volatile uint8_t Rc_Impuls;
volatile uint8_t Rc_Imp_Valid;



























































