;
;Jason Millette
;Lab09 Input capture in assembly
;4/5/2018
;
;detects 1Hz signal
;and measures inches with ultrasonic sensor
;

	INCLUDE core_cm4_constants.s		; Load Constant Definitions
	INCLUDE stm32l476xx_constants.s 

	AREA    main, CODE, READONLY
	EXPORT	__main		; make __main visible to linker
	EXPORT overflow
	EXPORT lastCounter
	EXPORT timespan
	
	IMPORT LCD_PIN_Init
	IMPORT LCD_DisplayString
	IMPORT LCD_Initialization
	IMPORT TIM4_Init
	;IMPORT TIM4_IRQHandler
	IMPORT itoa
	IMPORT SysTick_Initialize
	IMPORT trigger
	IMPORT Delay
	ENTRY			

__main	PROC
		BL HSI_init
		BL LCD_Initialization
		BL LCD_PIN_Init
		BL TIM4_Init
		MOV R10, #148			;148 for inch convertion, 100 for ms
		MOV R0, #1999			;value for systick
		BL SysTick_Initialize	;Initialize systick because Im lazy
		
		;infinite loop to display distance
loop	BL trigger				;trigger
		LDR R1, =timespan 		;load timespan
		LDR R2, [R1]			;load to R0 for conversion to string
		UDIV R2, R2, R10		;convert to inches
		MOV R0, R2			;input for itoa
		BL itoa
		BL LCD_DisplayString	;display string on LCD
		B loop
		
	
stop 	B 		stop     		; dead loop & program hangs here 
	ENDP

	; Slect HSI (16MHz, 1% accuracy) as the system clock
HSI_init PROC 
		EXPORT HSI_init
	
		;turn on HSI oscillator
		LDR R0, =RCC_BASE
		LDR R1, [R0, #RCC_CR]
		ORR R1, R1, #RCC_CR_HSION
		STR R1, [R0, #RCC_CR]
	
		;select HSI as system clock
		LDR R1, [R0, #RCC_CFGR]
		BIC R1, R1, #RCC_CFGR_SW
		ORR R1, R1, #RCC_CFGR_SW_HSI
		STR R1, [R0, #RCC_CFGR]
	
		;wait for HSI stable
waitHSI LDR R1, [R0, #RCC_CR]
		AND R1, R1, #RCC_CR_HSIRDY
		CMP R1, #0
		BEQ waitHSI
		BX LR
		ENDP

	AREA    myData, DATA, READWRITE
	ALIGN
array		DCD   1, 2, 3, 4
timespan	DCD 	0 	;pulse wdith
lastCounter	DCD		0	;timer counter value of last capture event
overflow	DCD		0	;counter for number of overflows
test		DCD		420 ;meme for testing
		END
