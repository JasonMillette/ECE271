/*
 *Jason Millette
 *Lab 04 Stepper motor Control
 *2/9/2018
 *clock speed 16MHZ
 */

#include "stm32l476xx.h"
#include "LCD.h"

// Prototypes
void LCD_PIN_Init(void);
void LCD_Configure(void);
void LCD_Display_String(void);
void LCD_Clear(void);
void Fullstep(double degree);
void Halfstep(double degree);
unsigned char keypad_scan(void);
int atoi(char *);

char set_mask(char x);
int i, j, k, l; 
double degree, steps;
unsigned char key_map [4][4] = {
		{'1','2','3','A'},
		{'4','5','6','B'},
		{'7','8','9','C'},
		{'*','0','#','D'},  //# is used as a whitespace
};
unsigned char Full[4] = {0x9, 0xA, 0x6, 0x5};

unsigned char key;   //declares all variables needed for this program
unsigned char str_len = 0;
char str[50];

int main(void){
	RTC->WPR |= (0xCA | 0x53);
	LCD_Clock_Init();
	LCD_Initialization();
	LCD_Clear();
	
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOEEN; //enables the clock for port E

	
	GPIOE->MODER &= ~(GPIO_MODER_MODER10 | GPIO_MODER_MODER11 | GPIO_MODER_MODER12 | GPIO_MODER_MODER13);       //sets the mode for the ports
	GPIOE->MODER |= (GPIO_MODER_MODER10_0 | GPIO_MODER_MODER11_0 | GPIO_MODER_MODER12_0 | GPIO_MODER_MODER13_0);
	GPIOA->MODER &= ~(GPIO_MODER_MODER1 | GPIO_MODER_MODER2 | GPIO_MODER_MODER3 | GPIO_MODER_MODER5);
	GPIOB->MODER &= ~(GPIO_MODER_MODER2 | GPIO_MODER_MODER3 | GPIO_MODER_MODER6 | GPIO_MODER_MODER7);
	GPIOB->MODER |= (GPIO_MODER_MODER2_0 | GPIO_MODER_MODER3_0 | GPIO_MODER_MODER6_0 | GPIO_MODER_MODER7_0);
	
	GPIOE->PUPDR &= ~(GPIO_PUPDR_PUPD10 | GPIO_PUPDR_PUPD11 | GPIO_PUPDR_PUPD12 | GPIO_PUPDR_PUPD13); //sets no pull up or pull down

	while(1) {
 
		key = keypad_scan();   //gets the character has been pressed
		degree = atoi(str);
		switch (key) {
			case '*':               //backspaces on *
				str[str_len] = ' ';
				str_len --;
				break;
			case 'B':            //rotates full step on input angle
				Fullstep(degree);
				break;
			case 'A':           //rotates half step on input angle
				Halfstep(degree);
				break;
			case '#':                  //rotates the 360
				Fullstep(360);
				break;
			default:				//adds the most recent character to the string
				str[str_len] = key;
				str[str_len + 1] = 0;
				str_len++;
				if (str_len >= 48) str_len = 0;	
		}
		if ((str[0] == '4') && (str[1] == '2') && (str[2] == '0')) {
			LCD_DisplayString((uint8_t*)str);   //something cool
			for (i = 0; i < 125000; i++);
			LCD_DisplayString((uint8_t*)"WWEED!");
		}
		else LCD_DisplayString((uint8_t*)str);
	}
	
}

void Halfstep (double degree) 
{
	/*
	 *rotates the 
	 *given degree
	 *then quits back
	 *to main
	 */		
			steps = (degree * 1.42222222222);
			for (k = 0; k < steps; k++) {
				for (l = 0; l < 8; l++) {
					for (j = 0; j < 2500; j++);
					GPIOB->ODR &= ~(GPIO_ODR_OD2 | GPIO_ODR_OD3 | GPIO_ODR_OD6 | GPIO_ODR_OD7);
					switch (l) {
						case 0:
							GPIOB->ODR |= (GPIO_ODR_OD2 | GPIO_ODR_OD7);
							break;
						case 1:
							GPIOB->ODR |= GPIO_ODR_OD7;
							break;
						
						case 2:
							GPIOB->ODR |= (GPIO_ODR_OD7 | GPIO_ODR_OD3);
							break;
						case 3:
							GPIOB->ODR |= GPIO_ODR_OD3;
							break;
						case 4:
							GPIOB->ODR |= (GPIO_ODR_OD6 | GPIO_ODR_OD3);
							break;
						case 5:
							GPIOB->ODR |= GPIO_ODR_OD6;
							break;
						
						case 6:
							GPIOB->ODR |= (GPIO_ODR_OD6 | GPIO_ODR_OD2);
							break;
						case 7:
							GPIOB->ODR |= GPIO_ODR_OD2;
							break;
					}
						
				}
					
			}
}

void Fullstep (double degree) 
{
	/*
	 *rotates the 
	 *given degree
	 *then quits back
	 *to main
	 */
			steps = (degree * 1.42222222);
			for (k = 0; k < steps; k++) {
				for (l = 0; l < 4; l++) {
					for (j = 0; j < 2500; j++);
					GPIOB->ODR &= ~(GPIO_ODR_OD2 | GPIO_ODR_OD3 | GPIO_ODR_OD6 | GPIO_ODR_OD7);
					switch (l) {
						
						case 0:
							GPIOB->ODR |= (GPIO_ODR_OD2 | GPIO_ODR_OD7);
							break;
						
						case 1:
							GPIOB->ODR |= (GPIO_ODR_OD7 | GPIO_ODR_OD3);
							break;
						
						case 2:
							GPIOB->ODR |= (GPIO_ODR_OD6 | GPIO_ODR_OD3);
							break;
						
						case 3:
							GPIOB->ODR |= (GPIO_ODR_OD6 | GPIO_ODR_OD2);
							break;
					}
				}
					
			}
}

unsigned char keypad_scan(void) 
{
	
	/*
	 *does a polling scan
	 *and determines the 
	 *key pressed then
	 *outputs the char
	 */
	
	
	char row, col;
	char mask = 0xF;
	int i;
	
	GPIOE->ODR &= ~(GPIO_ODR_OD10| GPIO_ODR_OD11 | GPIO_ODR_OD12 | GPIO_ODR_OD13);  //clears the outputs
	
	for(i = 0; i < 250; i++);   //delay for hardware to set outputs
	
	while ((GPIOA->IDR & (GPIO_IDR_ID1 | GPIO_IDR_ID2 | GPIO_IDR_ID3 | GPIO_IDR_ID5)) == (GPIO_IDR_ID1 | GPIO_IDR_ID2 | GPIO_IDR_ID3 | GPIO_IDR_ID5)); //waits for a key press
	
	for(i = 0; i < 250; i++);  //delay for hardware to set outputs
	
	if ((GPIOA->IDR & (GPIO_IDR_ID2 | GPIO_IDR_ID3 | GPIO_IDR_ID5)) == (GPIO_IDR_ID2 | GPIO_IDR_ID3 | GPIO_IDR_ID5)) {   //determines the colum
		col = 0;
	}
	if ((GPIOA->IDR & (GPIO_IDR_ID1 | GPIO_IDR_ID3 | GPIO_IDR_ID5)) == (GPIO_IDR_ID1 | GPIO_IDR_ID3 | GPIO_IDR_ID5)) {
		col = 1;
	}
	if ((GPIOA->IDR & (GPIO_IDR_ID2 | GPIO_IDR_ID1 | GPIO_IDR_ID5)) == (GPIO_IDR_ID2 | GPIO_IDR_ID1 | GPIO_IDR_ID5)) {
		col = 2;
	}
	if ((GPIOA->IDR & (GPIO_IDR_ID2 | GPIO_IDR_ID3 | GPIO_IDR_ID1)) == (GPIO_IDR_ID2 | GPIO_IDR_ID3 | GPIO_IDR_ID1)) {
		col = 3;
	}
	
	for (row = 0; row < 4; row++) {
			
		GPIOE->ODR &= ~(GPIO_ODR_OD11 | GPIO_ODR_OD12 | GPIO_ODR_OD13 | GPIO_ODR_OD10);   //sets the output bits
		GPIOE->ODR |= ((mask - set_mask(row)) << 10);
		
		for(i = 0; i < 2500; i++);  //delay for hardware to set outputs
		
		if(!((GPIOA->IDR & (GPIO_IDR_ID1 | GPIO_IDR_ID2 | GPIO_IDR_ID3 | GPIO_IDR_ID5)) == (GPIO_IDR_ID1 | GPIO_IDR_ID2 | GPIO_IDR_ID3 | GPIO_IDR_ID5))) {      // determins row then outputs mapped char
			while (!((GPIOA->IDR & (GPIO_IDR_ID1 | GPIO_IDR_ID2 | GPIO_IDR_ID3 | GPIO_IDR_ID5)) == (GPIO_IDR_ID1 | GPIO_IDR_ID2 | GPIO_IDR_ID3 | GPIO_IDR_ID5)));   
			return key_map[row][col];
		}
	}
	;
}

int myAtoi(char *str)     //taken from https://www.geeksforgeeks.org/write-your-own-atoi/
{
    int res = 0, i; // Initialize result
  
    // Iterate through all characters of input string and
    // update result
    for (i = 0; str[i] != '\0'; ++i)
        res = res*10 + str[i] - '0';
  
    // return result.
    return res;
}
char set_mask(char x)
{
	/*
	 *Takes the row
	 *and return needed
	 *char for mask
	 */
	
	if (x == 0) return 1;
	if (x == 1) return 2;
	if (x == 2) return 4;
	if (x == 3) return 8;
}	

void System_Clock_Init(void){
	// Enable High Speed Internal Clock (HSI = 16 MHz)
  RCC->CR |= ((uint32_t)RCC_CR_HSION);
  // wait until HSI is ready
  while ( (RCC->CR & (uint32_t) RCC_CR_HSIRDY) == 0 );
  // Select HSI as system clock source 
  RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
  RCC->CFGR |= (uint32_t)RCC_CFGR_SW_HSI;  //01: HSI16 oscillator used as system clock
  // Wait till HSI is used as system clock source 
  while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) == 0 );
		
}
