/*
 *Jason Millette
 *3/23/2018
 *Clock speed 16MHZ
 *LAB 9 
 */

#include "stm32l476xx.h"
#include "LCD.h"
#include "ADC.h"
#include "GPIOInit.h"

volatile uint32_t Result, Result_1, Result_2, Result_3, Result_4;

// Prototypes
void HSI_Init(void);
void ADC1_2_IRQHandler(void);

volatile uint32_t Result, Result_1, Result_2, Result_3, Result_4;
char str[6] = {' ', ' ', ' ', ' ', ' ', ' '};

int main(void){
	//
	
	HSI_Init();
	LCD_Clock_Init();
	LCD_Initialization();
	LCD_Clear();
	GPIOInit();
	ADC1_Init();
	
	
	LCD_DisplayString((uint8_t*)"LAB 10");

	while(1) {
		ADC1->CR |= ADC_CR_ADSTART; //one conversion
		while((ADC123_COMMON->CSR & ADC_CSR_EOC_MST) != ADC_CSR_EOC_MST);
		sprintf(str,"%d",(ADC1->DR)/13);  //display voltage on LCD
		LCD_DisplayString(str);
		if (ADC1->DR/13 > 200)					//toggle RED LED if Voltage above 2V
			GPIOB->ODR |= GPIO_ODR_OD2;
		else if (ADC1->DR/13 < 100)
			GPIOB->ODR &= ~GPIO_ODR_OD2;
	}
	
}

void HSI_Init()
{
	// Enable High Speed Internal Clock (HSI = 16 MHz)
  RCC->CR |= ((uint32_t)RCC_CR_HSION);
	
  // wait until HSI is ready
  while ( (RCC->CR & (uint32_t) RCC_CR_HSIRDY) == 0 ) {;}
	
  // Select HSI as system clock source 
  RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
  RCC->CFGR |= (uint32_t)RCC_CFGR_SW_HSI;     // 01: HSI16 oscillator used as system clock

  // Wait till HSI is used as system clock source 
  while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) == 0 ) {;}

	return;
}

void ADC1_2_IRQHandler (void)
{
	if ((ADC1->ISR & ADC_ISR_EOC) == ADC_ISR_EOC) {
		//For a regular channel, check End of conversion (EOC) flag
		//Reading ADC data register (DR) clears the EOC flag
		Result = ADC1->DR;
} else if ((ADC1->ISR & ADC_ISR_JEOS) == ADC_ISR_JEOS) {
	//For injected channels, check End of sequence (JEOS) flag
	//Reading injected data registers does not clear the JEOS flag
	//Each injected channel has a dedicated data register
	Result_1 = ADC1->JDR1;		//INjected channel 1
	Result_2 = ADC1->JDR2;		//INjected channel 2
	Result_3 = ADC1->JDR3;		//INjected channel 3
	Result_4 = ADC1->JDR4;		//INjected channel 4
	ADC1->ISR |= ADC_ISR_EOS; //Clear the flag by writing 1 to it
	}
}
