/*
 *Jason Millette
 *3/1/2018
 *Clock speed 4MHZ
 *LAB 7 PWM LAB
 */

#include "stm32l476xx.h"

// Prototypes
void TIM1_Init(void);

volatile uint32_t TimeDelay;
int main(void){
	//Uses PWM to Brightne then dim the LED
	//Then rotates the servo from -90, 0, 90
	//Then moves servo on user input

	TIM1_Init();

  // Enable the clock to GPIO Port E
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOEEN;
			
	// MODE: 00: Input mode,              01: General purpose output mode
  //       10: Alternate function mode, 11: Analog mode (reset state)
	GPIOE->MODER &= ~(GPIO_MODER_MODER11);   // for green LED and motor
	GPIOE->MODER |= (GPIO_MODER_MODER11_1); 

	//AF mode, AF value = 1 (TIM1_CH1N), high-speed, push-pull
	GPIOE->AFR[1] &= ~(GPIO_AFRH_AFSEL11);
	GPIOE->AFR[1] |= GPIO_AFRH_AFSEL11_0;
	GPIOE->AFR[1] &= ~(GPIO_AFRH_AFSEL10);
	GPIOE->AFR[1] |= GPIO_AFRH_AFSEL10_0;
}

void TIM1_Init () 
{
	//enable Timer 1 clock
	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
	
	//counting direction: 0 = Up-counting, 1 = Down-counting
	TIM1->CR1 &= ~TIM_CR1_DIR; //Up counting
	
	// prescaler, slow down the input clock by a factor of (1 + prescalar)
	TIM1->PSC = .6; //16 MHZ / (1 + .6) = 10MHz
	
	// Auto-reload
	TIM1->ARR = 99;  //PWM period = (99 + 1) * 1/10MHz = 10us
	
	//clear output compare mode bits for channel 1
	TIM1->CCMR1 &= ~TIM_CCMR1_OC1M;
	
	//select PWM Mode 1 output on channel 1 (OC1M = 110)
	TIM1->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2;
	
	//output 1 preload enable
	TIM1->CCMR1 |= TIM_CCMR1_OC1PE;
	
	//select output polarity: 0 = Active high, 1 = Active low
	TIM1->CCMR1 &= ~TIM_CCER_CC1NP; //OC1N = OCREF + CC1NP
	
	//Enable complementary output of channel 1 (CH1N)
	TIM1->CCER |= TIM_CCER_CC1NE;
	
	//Main output enable (MOE): 0 = disable, 1 = Enable
	TIM1->BDTR |= TIM_BDTR_MOE;
	
	// Output compare register for channel 1
	TIM1->CCR1 = 50; //for 20% duty cycle
	
	//enable counter
	TIM1->CR1 |= TIM_CR1_CEN;
	
	return;
}
