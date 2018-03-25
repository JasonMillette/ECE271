/*
 *Jason Millette
 *3/1/2018
 *Clock speed 4MHZ
 *LAB 7 PWM LAB
 */

#include "stm32l476xx.h"

// Prototypes
void SysTick_Initialize (uint32_t ticks);
void Delay(uint32_t nTime);
void SysTick_Handler(void);
void MSI_Init(void);
void TIM1_Init(void);

volatile uint32_t TimeDelay;
int ARR = 99, start = 0;

int main(void){
	//Uses PWM to Brightne then dim the LED
	//Then rotates the servo from -90, 0, 90
	//Then moves servo on user input
	
	int i = 0, brightness = 1, stepSize = 1;
	
	MSI_Init();
	TIM1_Init();

  // Enable the clock to GPIO Port A and E
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOEEN;
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
			
	// MODE: 00: Input mode,              01: General purpose output mode
  //       10: Alternate function mode, 11: Analog mode (reset state)
	GPIOE->MODER &= ~(GPIO_MODER_MODER8);   // for green LED and motor
	GPIOE->MODER |= (GPIO_MODER_MODER8_1); 
	GPIOA->MODER &= ~(0x0CFF); // for joystick
	
	// PUPDR:00: NO PUPD (reset state),   01: Pullup
  //       10: Pulldown,                11: Reserved
	GPIOA->PUPDR &= ~(0x00000CFF);
	GPIOA->PUPDR |= 0x000008AA;
	
	//AF mode, AF value = 1 (TIM1_CH1N), high-speed, push-pull
	GPIOE->AFR[1] &= ~(GPIO_AFRH_AFSEL8);
	GPIOE->AFR[1] |= GPIO_AFRH_AFSEL8_0;
	GPIOE->AFR[1] &= ~(GPIO_AFRH_AFSEL10);
	GPIOE->AFR[1] |= GPIO_AFRH_AFSEL10_0;
	  
	SysTick_Initialize(999); //initialize SysTick
	
	while(i != 2) {
		if ((brightness >= 99) || (brightness <= 0)) {
			stepSize = -stepSize;  //reverse direction
			i++;
		}
			
		brightness += stepSize;
		TIM1->CCR1 = brightness;
		Delay(10);
		
	}
	
	Delay(1000);
	ARR = 1999;
	start = 50; // -90
	TIM1_Init();	
	Delay(500);
	TIM1->CCR1 = 140; // 0
	Delay(500);
	TIM1->CCR1 = 250; //90 x
	
	while (1) {
		if (GPIOA->IDR & 0x02) //left -90
			TIM1->CCR1 = 50;
		if (GPIOA->IDR & 0x01) //center 0
			TIM1->CCR1 = 140;
		if (GPIOA->IDR & 0x04)  //right 90
			TIM1->CCR1 = 250;
		Delay(1); //for debounce
	}
}

void SysTick_Initialize (uint32_t ticks) {
	/*
	 *initializes
	 *the systick
	 */
	SysTick->CTRL = 0;
	SysTick->LOAD = ticks - 1;
	NVIC_SetPriority (SysTick_IRQn, (1<<__NVIC_PRIO_BITS) - 1);
	SysTick->VAL = 0;
	SysTick->CTRL &= ~SysTick_CTRL_CLKSOURCE_Msk;
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}

void SysTick_Handler (void) {
	/*
	 *decrements
	 *time delay 
	 *variable
	 */
	if (TimeDelay > 0)
		TimeDelay--;
}

void Delay(uint32_t nTime) {
	/*
	 *waits for
	 *timedelay to be decremented
	 */
	TimeDelay = nTime;
	while(TimeDelay != 0);
}

void TIM1_Init () 
{
	//enable Timer 1 clock
	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
	
	//counting direction: 0 = Up-counting, 1 = Down-counting
	TIM1->CR1 &= ~TIM_CR1_DIR; //Up counting
	
	// prescaler, slow down the input clock by a factor of (1 + prescalar)
	TIM1->PSC = 39; //4 MHZ / (1 + 39) = 100KHz
	
	// Auto-reload
	TIM1->ARR = ARR;  //PWM period = (999 + 1) * 1/100KHz = 0.001s
	
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
	TIM1->CCR1 = start;
	
	//enable counter
	TIM1->CR1 |= TIM_CR1_CEN;
	
	return;
}

void MSI_Init()
{
	//Enable medium speed insternal clock 4MHz
	RCC->CR &= ~(RCC_CR_MSION); //Turn off MSI so it can be modified
	RCC->CR |= (RCC_CR_MSIRANGE_6);
	RCC->CR |= RCC_CR_MSIRGSEL;
	RCC->CR |= (RCC_CR_MSION);
	while ( (RCC->CR & (uint32_t) RCC_CR_MSIRDY) == 0 ) {;}
	return;
}
