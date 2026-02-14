#include "stm32f1xx.h"                  // Device header

#include "Delay.h"
/*
brief:Initializate buttons(B1,B111)，配合定时器使用时需要先开启定时器中断
param:none
back:none
*/
void Key_Init(){
	__HAL_RCC_GPIOA_CLK_ENABLE();
	
	GPIO_InitTypeDef GPIO_InitStructure;
 	GPIO_InitStructure.Mode=GPIO_MODE_INPUT  ;
	GPIO_InitStructure.Pull=GPIO_PULLDOWN;
	GPIO_InitStructure.Speed=GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStructure.Pin=GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3;
 	HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	
}
uint8_t keynum=0;
uint8_t Key_GetNum()
{
	uint8_t tmp=0;
	if(keynum)
	{
		tmp=keynum;
	keynum=0;
	return tmp;
	}
	return 0;
}

uint8_t Key_GetState()
{
	if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_1)==1)
		return 1;
	else if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_2)==1)
		return 2;
	else if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_3)==1)
		return 3;
	else if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_4)==1)
		return 4;
	else if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_5)==1)
		return 5;
	else 
		return 0;
}
void Key_Kick()
{
	static uint8_t PreviousState=0;
	static uint8_t CurrentState=0;
	static uint8_t Count;
	Count ++;
	if (Count >= 20)
	{
		Count = 0;
	PreviousState=CurrentState;
	CurrentState=Key_GetState();
	
	if(CurrentState!=0&&PreviousState==0)
		keynum=CurrentState;
	}
	
	
}
