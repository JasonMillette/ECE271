/*
 *Jason Millette
 *3/1/2018
 *Clock speed 8MHZ
 *LAB 6 SYSTICK  *external clock delay function
 *with second counter
 */

#include "stm32l476xx.h"
#include "LCD.h"
volatile uint32_t TimeDelay;
// Prototypes
void SysTick_Initialize (uint32_t ticks);
void Delay(uint32_t nTime);
void SysTick_Handler(void);
void LCD_PIN_Init(void);
void LCD_Configure(void);
void LCD_Display_Name(void);
void LCD_Display_String(void);
void LCD_Clear(void);
void Scrolling_text(uint8_t* ptr);


int main(void){
	int count = 0;
	char str[6] = {' ',' ',' ',' ',' ',' '}; //initialize string for output
		// Enable Medium Speed Internal Clock (MSI = 8 MHz)
	RCC->CR &= ~(RCC_CR_MSION); //Turn off MSI so it can be modified
	RCC->CR |= (RCC_CR_MSIRANGE_7);
	RCC->CR |= RCC_CR_MSIRGSEL;
	RCC->CR |= (RCC_CR_MSION);
  // wait until MSI is ready
  while ( (RCC->CR & (uint32_t) RCC_CR_MSIRDY) == 0 ) {;}
	
  
  // Enable the clock to GPIO Port B	
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOEEN;
			

	// MODE: 00: Input mode,              01: General purpose output mode
  //       10: Alternate function mode, 11: Analog mode (reset state)
  GPIOB->MODER &= ~(GPIO_MODER_MODE13 | GPIO_MODER_MODE12) ;   // Clear bit 13 and bit 12
  GPIOB->MODER |= (GPIO_MODER_MODE13 | GPIO_MODER_MODE12);

	GPIOB->OTYPER &= ~(GPIO_OTYPER_OT2);

	GPIOE->MODER &= ~(GPIO_MODER_MODER8);   // for green LED
	GPIOE->MODER |= (1<<16); 
	  
	RTC->WPR |= (0xCA | 0x53);
	LCD_Clock_Init();
	LCD_Initialization();
	LCD_Clear();
	SysTick_Initialize(999); //initialize SysTick
	
	while(1) {
		GPIOE->ODR ^= GPIO_ODR_OD8;    //toggle LED
		count++;
		sprintf(str, "%d", count);   //convert count to string for output
		LCD_DisplayString(str);
		Delay(1000);           //delay function
	}
	LCD_bar();
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

