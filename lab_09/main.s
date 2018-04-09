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
	IMPORT LCD_PIN_Init
	IMPORT LCD_DisplayString
	IMPORT LCD_Initialization
	IMPORT TIM4_Init
	IMPORT itoa
	ENTRY			

__main	PROC
		BL HSI_init
		BL LCD_Initialization
		BL LCD_PIN_Init
		;LDR R0, =string
		;BL LCD_DisplayString
		BL TIM4_Init
		MOV R10, #148
	
loop	LDR R0, =timespan
		MUL R0, R0, R10
		BL itoa
		BL LCD_DisplayString
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
			
TIM4_IRQHandler PROC
				EXPORT 	TIM4_IRQHandler
				
				PUSH {R4, R6, R10, lr}
				
				LDR R0, = TIM4_BASE  				;Pseudo instruction
				LDR R2, [R0, #TIM_SR] 				;read status register
				AND R3, R2, #TIM_SR_UIF 			;check update event flag
				CBZ R3, check_CCFlag 				;Compare and brance on zero
				
				LDR R3, =overflow
				LDR R1, [R3] 						;read overflow from memory
				ADD R1, R1, #1 						;Increment overflow counter
				STR R1, [R3] 						;save overflow memory
				
				BIC R2, R2, #TIM_SR_UIF 			;clear update event flag
				STR R2, [R0, #TIM_SR] 				;update event flag
				
check_CCFlag	AND R2, R2, #TIM_SR_CC1IF 	;check capture event flag
				CBZ R2, exit 						;compare and branch on zero
				
				LDR R0, =TIM4_BASE 					;Load base memory address
				LDR R1, [R0, #TIM_CCR1] 			;read the new capture value
				
				LDR R2, =lastCounter
				LDR R0, [R2] 						;load the last counter value
				STR R1, [R2] 						;save the new counter value
				CBZ R0, clearOverflow 				;compare and branch on zero
				
				LDR R3, =overflow
				LDR R4, [R3] 						;load the overflow value
				LSL R4, R4, #16 					; multiply by 2^16
				ADD R6, R1, R4
				SUB R10, R6, R0 					;R10 = timer counter difference
				LDR R2, =timespan
				STR R10, [R2] 						;update timespan in memory
		
clearOverflow	MOV R0, #0
				LDR R3, =overflow
				STR R0, [R3] 						;clear overflow counter
exit			POP {R4, R6, R10, pc}
				ENDP

	AREA    myData, DATA, READWRITE
	ALIGN
array		DCD   1, 2, 3, 4
timespan	DCD 	0 	;pulse wdith
lastCounter	DCD		0	;timer counter value of last capture event
overflow	DCD		0	;counter for number of overflows
string		DCB		"LAB 09" ;string for testing
	
	END
		