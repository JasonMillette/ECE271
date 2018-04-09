//Jason Millette
//3/25/2018
//ECE 271
#include "stm32l476xx.h"
#include "InputCapture.h"
#include "LCD.h"


	
void TIM4_Init() 
{
  // Enable the clock to GPIO Port E and B and timer 4
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOEEN;
	
	// MODE: 00: Input mode,              01: General purpose output mode
  //       10: Alternate function mode, 11: Analog mode (reset state) 
	GPIOB->MODER &= ~(GPIO_MODER_MODE2); //red LED
	GPIOB->MODER |= GPIO_MODER_MODE2_0;
	GPIOB->MODER &= ~(GPIO_MODER_MODE6); //input capture (echo)
 	GPIOB->MODER |= GPIO_MODER_MODE6_1;
	
	GPIOB->PUPDR &= GPIO_PUPDR_PUPD6;  // no pull up or pull down
	
	//AF mode, AF value = 2 (TIM1_CH1), high-speed, push-pull
	GPIOB->AFR[0] &= ~(GPIO_AFRL_AFSEL6);
	GPIOB->AFR[0] |= GPIO_AFRL_AFSEL6_1;


	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM4EN;   //added 1 might be incorrect

	TIM4->CR1 &= ~TIM_CR1_UDIS;
	
	//set up an appropriate prescalar to slow down the timers input clock
	TIM4->PSC = 159;
	
	//set Auto-reload value to maximum value
	TIM4->ARR = 0xFFFF; //maximum 16-bit value
	
	//set the direction of channel 1 as input, and select the active input
	TIM4->CCMR1 &= ~TIM_CCMR1_CC1S; //clear capture/compare 1 selection bits
	TIM4->CCMR1 |= TIM_CCMR1_CC1S_0; //CCIS[1:0] for channel 1:
																		//00 = output
																		//01 = input, CC1 is mapped on timer input 1
																		//10 = input, CC1 is mapped on timer input 2
																		//11 = input, CC1 is mapped on slave timer
								
	//program the input filter duration: disable digital filtering by clearing
	//IC1F[3:0] bits because we want to capture every event
	TIM4->CCMR1 &= ~TIM_CCMR1_IC1F;  //no filtering
	
	//set the active transition as both rising and falling edges
	//CC1NP:CC1P bits: 00 = rising edge   01 = falling edge
	//                 10 = reserved      11 = both edges
	TIM4->CCER |= (TIM_CCER_CC1P | TIM_CCER_CC1NP); // both edges generate interrupts
	
	//program the input prescalar: clear prescaler to capture each transition
	TIM4->CCMR1 &= ~TIM_CCMR1_IC1PSC;
	
	//enable capture for channel 1
	//CC1E: 0 = disabled, 1 = enabled
	TIM4->CCER |= TIM_CCER_CC1E;
	
	
	//allow channel 1 of timer 4 to generate interrupts
	TIM4->DIER |= TIM_DIER_CC1IE;
	
	//Allow channel 1 of 4 to generate DMA requests
	TIM4->DIER |= TIM_DIER_CC1DE; //optional.; required if DMA is used
	
	//enable the timer counter
	TIM4->CR1 |= TIM_CR1_CEN;
	
	//set priority of timer 4 interrupt to 0 (highest urgency)
	NVIC_SetPriority(TIM4_IRQn, 0);
	
	//enable timer 4 interrupt in the interrupt controller (NVIC)
	NVIC_EnableIRQ(TIM4_IRQn);
	
	LCD_DisplayString("TIM4_I");
	return;
}

char * itoa(int inches)
{
	sprintf(str, "%d", inches);
	return str;
}
