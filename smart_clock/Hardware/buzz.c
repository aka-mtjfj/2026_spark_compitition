#include "stm32f1xx.h"                  // Device header
void Buzz_On()
{
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET);
}
void Buzz_Off()
{
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET);
}