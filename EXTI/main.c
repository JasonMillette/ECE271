/*
 *Jason Millette
 *3/1/2018
 *Clock speed 4MHZ
 *LAB 7 PWM LAB
 */

#include "stm32l476xx.h"
#include "LCD.h"
#include "SYSTICK.h"

// Prototypes
void MSI_Init(void);
void EXTI_Init(void);
void EXTI0_IRQHandler (void);
void EXTI1_IRQHandler (void);
void EXTI2_IRQHandler (void);


int main(void){
	//Toggles red LED every 1/4 second
	//uses interrupt to display button
	//on LCD and toggle green LED
	
	MSI_Init();
	

  // Enable the clock to GPIO Port A and E and B
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOEEN;
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
			
	// MODE: 00: Input mode,              01: General purpose output mode
  //       10: Alternate function mode, 11: Analog mode (reset state)
	GPIOE->MODER &= ~(GPIO_MODER_MODER8);   // for green LED and motor
	GPIOE->MODER |= (GPIO_MODER_MODER8_0); 
	GPIOA->MODER &= ~(0x0CFF); // for joystick
	GPIOB->MODER &= ~(GPIO_MODER_MODE2);
	GPIOB->MODER |= GPIO_MODER_MODE2_0;
	
	// PUPDR:00: NO PUPD (reset state),   01: Pullup
  //       10: Pulldown,                11: Reserved
	GPIOA->PUPDR &= ~(0x00000CFF);
	GPIOA->PUPDR |= 0x000008AA;
	
	LCD_Clock_Init();
	LCD_Initialization();
	LCD_Clear();
	SysTick_Initialize(999); //initialize SysTick

	EXTI_Init();   //initialize external interrupt
	while (1) {
		GPIOB->ODR ^= GPIO_ODR_OD2;  //toggle red led every 1/4 second
		Delay(250);
	}
	
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

void EXTI_Init()
{
	//enable SYSCFG clock
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	
	//select PA.0 as the trigger source of EXTI 0
	SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR1_EXTI0;
	SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PA;
	SYSCFG->EXTICR[0] &= ~(0x000F);

	//select PA.1 as the trigger source of EXTI 1
	SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR1_EXTI1;
	SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI1_PA;
	SYSCFG->EXTICR[0] &= ~(0x000F);
	
	//select PA.2 as the trigger source of EXTI 2
	SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR1_EXTI2;
	SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI2_PA;
	SYSCFG->EXTICR[0] &= ~(0x000F);
	
	//Enable rising edge trigger for EXTI 0
	//Rising trigger selection register (RSTR)
	//0 = disabled, 1 = enabled
	EXTI->RTSR1 |= EXTI_RTSR1_RT0;
	
	//Enable rising edge trigger for EXTI 1
	//Rising trigger selection register (RSTR)
	//0 = disabled, 1 = enabled
	EXTI->RTSR1 |= EXTI_RTSR1_RT1;
	
	//Enable rising edge trigger for EXTI 2
	//Rising trigger selection register (RSTR)
	//0 = disabled, 1 = enabled
	EXTI->RTSR1 |= EXTI_RTSR1_RT2;
	
	//Disable falling edge trigger for EXTI 0
	//Falling trigger selection register (FSTR)
	//0 = disabled, 1 = enabled
	EXTI->FTSR2 &= ~EXTI_FTSR1_FT0;
	
	//Disable falling edge trigger for EXTI 1
	//Falling trigger selection register (FSTR)
	//0 = disabled, 1 = enabled
	EXTI->FTSR2 &= ~EXTI_FTSR1_FT1;
	
	//Disable falling edge trigger for EXTI 2
	//Falling trigger selection register (FSTR)
	//0 = disabled, 1 = enabled
	EXTI->FTSR2 &= ~EXTI_FTSR1_FT2;
	
	//Enable EXTI 0 interrupt
	//Interrupt mask register: 0 = masked, 1 = unmasked
	// "Masked" means that processor ignores the corresponding interrupt
	EXTI->IMR1 |= EXTI_IMR1_IM0;  //enable EXTI line 0
	
	//Enable EXTI 1 interrupt
	//Interrupt mask register: 0 = masked, 1 = unmasked
	// "Masked" means that processor ignores the corresponding interrupt
	EXTI->IMR1 |= EXTI_IMR1_IM1;  //enable EXTI line 1
	
	//Enable EXTI 2 interrupt
	//Interrupt mask register: 0 = masked, 1 = unmasked
	// "Masked" means that processor ignores the corresponding interrupt
	EXTI->IMR1 |= EXTI_IMR1_IM2;  //enable EXTI line 2
	
	//set EXTI 0 priority to 1
	NVIC_SetPriority(EXTI0_IRQn, 1);
	
	//set EXTI 1 priority to 1
	NVIC_SetPriority(EXTI1_IRQn, 1);
	
	//set EXTI 2 priority to 1
	NVIC_SetPriority(EXTI2_IRQn, 1);
	
	//Enable EXTI 0 interrupt
	NVIC_EnableIRQ(EXTI0_IRQn);
	
	//Enable EXTI 1 interrupt
	NVIC_EnableIRQ(EXTI1_IRQn);
	
	//Enable EXTI 2 interrupt
	NVIC_EnableIRQ(EXTI2_IRQn);
}

void EXTI0_IRQHandler ()
{
	if ((EXTI->PR1 & EXTI_PR1_PIF0) == EXTI_PR1_PIF0) {
		//toggle LED
		GPIOE->ODR ^= 1<<8;     //toggle PE 8 output
		LCD_DisplayString((uint8_t*)"CENTER");
		//clear interrupt pending request
		EXTI->PR1 |= EXTI_PR1_PIF0;  //write 1 to clear
	}
}

void EXTI1_IRQHandler ()
{
	if ((EXTI->PR1 & EXTI_PR1_PIF1) == EXTI_PR1_PIF1) {
		LCD_DisplayString((uint8_t*)"LEFT");
		//clear interrupt pending request
		EXTI->PR1 |= EXTI_PR1_PIF1;  //write 1 to clear
	}
}

void EXTI2_IRQHandler ()
{
	if ((EXTI->PR1 & EXTI_PR1_PIF2) == EXTI_PR1_PIF2) {
		LCD_DisplayString((uint8_t*)"RIGHT");
		//clear interrupt pending request
		EXTI->PR1 |= EXTI_PR1_PIF2;  //write 1 to clear
	}
}
