//Jason Millette
//3/25/2018
//ECE 271
#include "stm32l476xx.h"
#include "InputCapture.h"
#include "LCD.h"

volatile uint32_t pulse_width = 0;
volatile double timeSpan = 0;
volatile uint8_t OC = 0;
volatile uint32_t last_captured = 0;
volatile uint32_t signal_polarity = 0; //assume inout is low initially

void TIM4_IRQHandler()
//Interrupt for PB6
{
	uint32_t current_captured;
	char str[6] = {' ',' ',' ',' ',' ',' '}; //initialize string for output
	
	if((TIM4->SR & TIM_SR_CC1IF) != 0) { //check if interrupt flag is set
		
		//reading CCR1 clears CC1IF interrupt flag
		current_captured = TIM4->CCR1;
		
		//toggle the polarity flag
		signal_polarity = 1 - signal_polarity;
		
		if (signal_polarity == 0) { //calculate only when the current input is low
			pulse_width = (current_captured - last_captured);			//assume up-counting
			timeSpan = (pulse_width + ((1 + 65535) * OC))/100;
			if (pulse_width < (148 * 120)) {
			//	sprintf(str, "%d", (int)(((double)pulse_width)/148));   // calculate and create string for distance
				//LCD_DisplayString((uint8_t*)str);   //dispay distance in inches
			}
			
			sprintf(str, "%d", (int)(timeSpan)); //prints frequency
			LCD_DisplayString((uint8_t*)str);
			TIM4->CNT = 0;
			OC = 0;
		}
		
		last_captured = current_captured;
		
	}
	
	if ((TIM4->SR & TIM_SR_UIF) != 0) { //check if overflow flag has taken place
		TIM4->SR &= ~TIM_SR_UIF; //clear UIF flag to prefvent re-entering
		OC++;
	}
}

