//Jason Millette
//3/25/2018
//ECE 271
#include "stm32l476xx.h"
#include "InputCapture.h"
#include "LCD.h"

volatile char str[6] = {' ', ' ', ' ', ' ', ' ', ' '};

char * itoa(int inches)
{
	sprintf(str, "%d", inches);
	return str;
}
