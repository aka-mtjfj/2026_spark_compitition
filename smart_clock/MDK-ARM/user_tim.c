#include "stm32f1xx.h"                // Device header
#include "tim.h"
#include "key.h"
#include "Menu.h"
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	static uint32_t cnt=0;
	cnt++;
	if(htim->Instance==TIM2)//定时器2用于管理按键的非阻塞检测
	{
			Key_Kick();
	}
	else if(htim->Instance==TIM1)//定时器1用于三个定时的公共定时器
	{
		if(timer_yn[0]==1&&cnt>=timer1_cnt)
		{
			timer_yn[0]=2;
			//到达时间，蜂鸣器响一段时间
		}
		else if(timer_yn[1]==1&&cnt>=timer2_cnt)
		{
			timer_yn[1]=2;
			//到达时间，蜂鸣器响一段时间
		}
		else if(timer_yn[2]==1&&cnt>=timer3_cnt)
			timer_yn[2]=2;
		//到达时间，蜂鸣器响一段时间
	}
	
	
}