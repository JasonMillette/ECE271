	INCLUDE core_cm4_constants.s		; Load Constant Definitions
	INCLUDE stm32l476xx_constants.s 

	IMPORT overflow
	IMPORT lastCounter
	IMPORT timespan
	
	AREA TIM4Init, CODE
	EXPORT TIM4_Init
 	;EXPORT TIM4_IRQHandler
	ALIGN
	ENTRY
	
TIM4_Init PROC
	LDR R1, =RCC_BASE					;enable the clock for TIM4
	LDR R2, [R1, #RCC_APB1ENR1]
	ORR R2, R2, #RCC_APB1ENR1_TIM4EN
	STR R2, [R1, #RCC_APB1ENR1]
	
	LDR R1, =RCC_BASE					;Enable the clock to gpio port E
	LDR R2, [R1, #RCC_AHB2ENR]			
	ORR R2, R2, #RCC_AHB2ENR_GPIOEEN
	str R2, [R1, #RCC_AHB2ENR]
		
	LDR R1, =RCC_BASE					;Enable the clock to gpio port B
	LDR R2, [R1, #RCC_AHB2ENR]
	ORR R2, R2, #RCC_AHB2ENR_GPIOBEN
	str R2, [R1, #RCC_AHB2ENR]
	
	LDR R1, =GPIOB_BASE					;Setting mode as AF
	LDR R2, [R1, #GPIO_MODER]
	BIC R2, R2, #GPIO_MODER_MODER6
	ORR R2, R2, #GPIO_MODER_MODER6_1
	str R2, [R1, #GPIO_MODER]	
	
	LDR R1, =GPIOB_BASE				;Setting NO PULL UP PULL DOWN
	LDR R2, [R1, #GPIO_PUPDR]
	BIC R2, R2, #GPIO_PUPDR_PUPDR6
	str R2, [R1, #GPIO_PUPDR]	
	
	LDR R1, =GPIOB_BASE					;Setting AF MODE
	LDR R2, [R1, #GPIO_AFR0]
	BIC R2, R2, #GPIO_AFRL_AFRL6
	ORR R2, R2, #GPIO_BSRR_BR_9
	str R2, [R1, #GPIO_AFR0]	
	
	LDR R1, =TIM4_BASE					;Setting ARR to 0xFFFF
	LDR R2, [R1, #TIM_ARR]
	MOV R2, #0xFFFF
	str R2, [R1, #TIM_ARR]
	
	LDR R1, =TIM4_BASE					;Setting PSC to 159
	LDR R2, [R1, #TIM_PSC]
	MOV R2, #15
	str R2, [R1, #TIM_PSC]
	
	LDR R1, =TIM4_BASE					;Setting direction of channel 1 as input
	LDR R2, [R1, #TIM_CCMR1]
	BIC R2, R2, #TIM_CCMR1_CC1S
	ORR R2, R2, #TIM_CCMR1_CC1S_0
	str R2, [R1, #TIM_CCMR1]
	
	LDR R1, =TIM4_BASE					;Disable filtering
	LDR R2, [R1, #TIM_CCMR1]
	BIC R2, R2, #TIM_CCMR1_IC1F
	str R2, [R1, #TIM_CCMR1]
	
	LDR R1, =TIM4_BASE					;Set rising falling edge
	LDR R2, [R1, #TIM_CCER]
	ORR R2, R2, #TIM_CCER_CC1P
	ORR R2, #TIM_CCER_CC1NP
	str R2, [R1, #TIM_CCER]
	
	LDR R1, =TIM4_BASE					;clear prescalar
	LDR R2, [R1, #TIM_CCMR1]
	BIC R2, R2, #TIM_CCMR1_IC1PSC
	str R2, [R1, #TIM_CCMR1]
	
	LDR R1, =TIM4_BASE					;channel 1 timer 4 input
	LDR R2, [R1, #TIM_CCER]
	ORR R2, R2, #TIM_CCER_CC1E
	str R2, [R1, #TIM_CCER]
	
	LDR R1, =TIM4_BASE					;DMA requests not needed
	LDR R2, [R1, #TIM_DIER]
	ORR R2, R2, #TIM_DIER_CC1IE
	str R2, [R1, #TIM_DIER]
	
	LDR R1, =TIM4_BASE					;DMA requests not needed
	LDR R2, [R1, #TIM_DIER]
	ORR R2, R2, #TIM_DIER_CC1DE
	str R2, [R1, #TIM_DIER]
	
	LDR R1, =TIM4_BASE					;enable timer counter
	LDR R2, [R1, #TIM_CR1]
	ORR R2, R2, #TIM_CR1_CEN
	str R2, [R1, #TIM_CR1]
	
	
	;Set Interrupt priority
	MOV R0, #30
	MOV R1, #1
	LSL R2, R1, #4						;R2 = priority << 4
	LDR R3, =NVIC_BASE					;NVIC base address
	LDR R4, =NVIC_IP0					;Interrupt priority register
	ADD R3, R3, R4					
	STRB R2, [R3, R0]					;save priority; dont use STR
	
	;enable interrupt
	AND R2, R0, #0x1F					;bit offset in a word
	MOV R3, #1
	LSL R3, R3, R2						;R3 = 1 << (IRQn & 0x1F)
	LDR R4, =NVIC_BASE
	
	CMP R1, #0
	LDRNE R1, =NVIC_ISER0
	LDREQ R1, =NVIC_ICER0
	
	ADD R1, R4, R1						;R1 = addr, of NVIC->ISER0
	LSR R2, R0, #5						;Memory offser (in words): IRQn >> 5
	STR R3, [R1, R2]					;Enable/disable interrupt

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
	ALIGN
	END
