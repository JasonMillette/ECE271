	INCLUDE core_cm4_constants.s		; Load Constant Definitions
	INCLUDE stm32l476xx_constants.s 
		
	AREA itoafile, CODE
	EXPORT itoa
	ALIGN
	ENTRY
	
itoa			PROC
				PUSH {R4-R6, LR}
				MOV R2, R0							;R2 = n
				MOV R3, R1							;R3 = s
				
				;build the string backwards
				MOV R6, #10
loop1			CBZ R2, done						;done if n = 0
				UDIV R5, R2, R6						;R5 = n/10
				MLS R4, R6, R5, R2					;R4 = R2 -10*R5
				ADD R4, R4, #0x30					;n%10 + '0'
				STRB R4, [R3], #1					;*p = n%10+'0'
				UDIV R2, R2, R6						;n /= 10
				B	loop1
				
done			MOV R4, #0
				STRB R4, [R3]						;*p = '\0'
				SUB R3, R3, #1						;skip NULL
				
				;reverse the string
loop2			CMP R3, r1							;compare p & s
				BLE exit1							;exit if p <= s
				LDRB R4, [R3]						;swap
				LDRB R5, [R1]
				STRB R4, [R1]
				STRB R5, [R3]
				ADD R1, R1, #1						; s++
				SUB R3, R3, #1						; p--
				B 	loop2
exit1			POP {R4-R6, PC}
				ENDP
				END