//Jason Millette
//3/25/2018
//ECE 271
#include "SYSTICK.h"
#include "stm32l476xx.h"

volatile uint32_t TimeDelay;

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
	
	GPIOE->MODER &= ~GPIO_MODER_MODE11; //trigger
	GPIOE->MODER |= GPIO_MODER_MODE11_0;

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

char *itoa(int num)
{
	sprintf(str, "%d", num);
	return str;
}

void trigger(void)
{
		GPIOE->ODR |= GPIO_ODR_OD11;  //toggle trigger
		Delay(2);
		GPIOE->ODR &= ~GPIO_ODR_OD11;
		Delay(100);
		return;
}
