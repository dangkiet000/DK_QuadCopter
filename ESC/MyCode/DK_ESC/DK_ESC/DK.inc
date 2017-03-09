;********************************************
;* Birdie 70A with all FETs on PORTD        *
;* All FETs is N-Type                       *
;* Use 1 INT0                               *
;* Original fuses are lfuse:0xbf hfuse:0xc1 *
;********************************************

.equ	F_CPU		= 16000000
.equ	USE_INT0	= 1
.equ	USE_I2C		= 0
.equ	USE_UART	= 0
.equ	USE_ICP		= 0

;.equ    DEAD_LOW_NS     = 450	; May work on some boards, but be careful!
;.equ    DEAD_HIGH_NS    = 8000
;.equ	   CHECK_HARDWARE	 = 1

;******************************************
; PORT D definitions                      *
;******************************************
.equ	ApFET		= 0	;o High-side A
.equ	AnFET		= 1	;o Low-side A

.equ	BpFET		= 4	;o High-side B
.equ	BnFET		= 3	;o Low-side B

.equ	CpFET		= 5	;o High-side C
.equ	CnFET		= 7	;o Low-side C

.equ	c_comp		= 6	;i common comparator input (AIN0)
.equ	rcp_in		= 2	;i r/c pulse input

.equ	INIT_PD		= 0
.equ	DIR_PD		= (1<<ApFET)+(1<<BpFET)+(1<<CpFET)+(1<<AnFET)+(1<<BnFET)+(1<<CnFET)

.equ	ApFET_port	= PORTD
.equ	BpFET_port	= PORTD
.equ	CpFET_port	= PORTD
.equ	AnFET_port	= PORTD
.equ	BnFET_port	= PORTD
.equ	CnFET_port	= PORTD

;******************************************
; PORT C definitions                      *
;******************************************
.equ	mux_voltage	= 7	; ADC7 voltage input (47k from Vbat, 4.7k to gnd, 10.10V in -> .918V at ADC7)
;.equ			= 6	; ADC6
;.equ			= 5	; ADC5
.equ	mux_c		= 4	; ADC4 phase input
.equ	mux_b		= 3	; ADC3 phase input
.equ	mux_a		= 2	; ADC2 phase input
;.equ			= 1	; ADC1
;.equ			= 0	; ADC0

.equ	O_POWER		= 470
.equ	O_GROUND	= 47

.equ	INIT_PC		= 0
.equ	DIR_PC		= 0

;******************************************
; PORT B definitions                      *
;******************************************
;.equ			= 7
;.equ			= 6
;.equ			= 5	(sck stk200 interface)
;.equ			= 4	(miso stk200 interface)
;.equ			= 3	(mosi stk200 interface)
;.equ			= 2
;.equ			= 1
;.equ			= 0

.equ	INIT_PB		= 0
.equ	DIR_PB		= 0

;******************************************
; LED PORT D definitions                  *
;******************************************

;.equ	red_led		= 2
;.equ	green_led	= 7
;.MACRO RED_on
;	sbi	PORTD, red_led
;.ENDMACRO
;.MACRO RED_off
;	cbi	PORTD, red_led
;.ENDMACRO
;.MACRO GRN_on
;	cbi	PORTD, green_led
;.ENDMACRO
;.MACRO GRN_off
;	sbi	PORTD, green_led
;.ENDMACRO
