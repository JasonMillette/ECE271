;******************** (C) Yifeng ZHU *******************************************
; @file    main.s
; @author  Yifeng Zhu
; @date    May-17-2015
; @note
;           This code is for the book "Embedded Systems with ARM Cortex-M 
;           Microcontrollers in Assembly Language and C, Yifeng Zhu, 
;           ISBN-13: 978-0982692639, ISBN-10: 0982692633
; @attension
;           This code is provided for education purpose. The author shall not be 
;           held liable for any direct, indirect or consequential damages, for any 
;           reason whatever. More information can be found from book website: 
;           http:;www.eece.maine.edu/~zhu/book
;*******************************************************************************




;
;Jason Millette
;Lab05 stepper motor
;2/23/2018
;
;Rotates a stepper motor
;360 by full step
;


	INCLUDE core_cm4_constants.s		; Load Constant Definitions
	INCLUDE stm32l476xx_constants.s      

	AREA    main, CODE, READONLY
	EXPORT	__main		; make __main visible to linker
	IMPORT something
	ENTRY			
				
__main	PROC
	
    ; Enable the clock to GPIO Port B	
	LDR r0, =RCC_BASE
	LDR r1, [r0, #RCC_AHB2ENR]
	ORR r1, r1, #RCC_AHB2ENR_GPIOBEN
	STR r1, [r0, #RCC_AHB2ENR]

	; MODE: 00: Input mode, 01: General purpose output mode
    ;       10: Alternate function mode, 11: Analog mode (reset state)
	LDR r0, =GPIOB_BASE
	LDR r1, [r0, #GPIO_MODER]
	AND r1, #0
	ORR r1, r1,  #(GPIO_MODER_MODER2_0)	; sets mode to output
	ORR r1, r1,  #(GPIO_MODER_MODER3_0)
	ORR r1, r1,  #(GPIO_MODER_MODER6_0)
	ORR r1, r1,  #(GPIO_MODER_MODER7_0)
	STR r1, [r0, #GPIO_MODER]
	
	MOV r1, #0
	MOV r2, #0
	MOV r4, #0    ;initializes all registers
	MOV r6, #0
	LDR r7, =GPIOB_BASE
	
	BL something
	
	B check1                 ;outer most loop for all steps
loop1 ADD r1, r1, #1

	B check2
loop2 ADD r2, r2, #1    ;second loop for the different posistions

	B check3
loop3 ADD r4, r4, #1
check3 CMP r4, #2000     ;delay loop
	BLT loop3
endloop3

	LDR r6, [r7, #GPIO_ODR]  ;clears output
	AND r6, r6, #0
	STR r6, [r7, #GPIO_ODR]

	CMP r2, #1    ;first position
	BGT next1

		LDR r6, [r7, #GPIO_ODR]
		ORR r6, r6, #GPIO_ODR_ODR_2
		ORR r6, r6, #GPIO_ODR_ODR_7
		STR r6, [r7, #GPIO_ODR]
	B end1
	

next1 CMP r2, #2     ;second posisition
	BGT next2

		LDR r6, [r7, #GPIO_ODR]
		ORR r6, r6, #GPIO_ODR_ODR_3
		ORR r6, r6, #GPIO_ODR_ODR_7
		STR r6, [r7, #GPIO_ODR]
	B end1
	
next2 CMP r2, #3	;third position
	BGT next3
		LDR r6, [r7, #GPIO_ODR]
		ORR r6, r6, #GPIO_ODR_ODR_3
		ORR r6, r6, #GPIO_ODR_ODR_6
		STR r6, [r7, #GPIO_ODR]
	B end1
	
next3 CMP r2, #4	 ;fourth position

		LDR r6, [r7, #GPIO_ODR]
		ORR r6, r6, #GPIO_ODR_ODR_2
		ORR r6, r6, #GPIO_ODR_ODR_6
		STR r6, [r7, #GPIO_ODR]
	B end1
end1	

	MOV r4, #0    ;reset for delay
	
check2 CMP r2, #4
	BLT loop2

	MOV r2, #0  ;reset for position
	
check1 CMP r1, #512
	BLT loop1
	
endloop2
endloop1

  
stop 	B 		stop     		; dead loop & program hangs here

	ENDP
					
	ALIGN			

	AREA    myData, DATA, READWRITE
	ALIGN
array	DCD   1, 2, 3, 4
	END
