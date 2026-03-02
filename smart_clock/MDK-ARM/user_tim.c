#include "stm32f1xx.h"                  // Device header
#include "tim.h"
#include "key.h"
#include "Menu.h"
#include "buzz.h"
#include "usart.h"
uint8_t buzzoff_flag=0;
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	static uint32_t cnt1=0,cnt2=0,cnt3=0;
		static uint8_t buzz_cnt1=0,buzz_cnt2=0,buzz_cnt3=0;
	static uint8_t buzz_flag1=0,buzz_flag2=0,buzz_flag3=0;
	if(htim->Instance==TIM2)//定时器2用于管理按键的非阻塞检测
	{
			Key_Kick();
	}
	else if(htim->Instance==TIM1)//定时器1用于三个定时的公共定时器
	{
		if(timer_yn[0]==1)
		{
			cnt1++;
			if(cnt1==timer1_cnt)
			{
				cnt1=0;
			buzzoff_flag=0;
			timer_yn[0]=2;
			buzz_flag1=1;
			//到达时间，蜂鸣器响一段时间
			}
		}
		else if(timer_yn[1]==1)
		{
			cnt2++;
			if(cnt2==timer2_cnt)
			{
				cnt2=0;
			buzzoff_flag=0;
			timer_yn[1]=2;
			buzz_flag2=1;
			//到达时间，蜂鸣器响一段时间
			}

		}
		else if(timer_yn[2]==1)
		{
			cnt3++;
			if(cnt3==timer3_cnt)
			{
				cnt3=0;
				buzzoff_flag=0;
				timer_yn[2]=2;
		//到达时间，蜂鸣器响一段时间
		buzz_flag3=1;
			}
		}
		if(buzz_cnt1==5)//闹钟一响铃5s
		{
			Buzz_Off();
			buzzoff_flag=1;
			buzz_cnt1=0;
			buzz_flag1=0;
		}
		else if(buzz_flag1==1&&buzz_cnt1<10)
		{
			Buzz_On();
			buzz_cnt1++;
		}
				if(buzz_cnt2==5)//闹钟2响铃5s
		{
			Buzz_Off();
			buzzoff_flag=1;
			buzz_cnt2=0;
			buzz_flag2=0;
		}
		else if(buzz_flag2==1&&buzz_cnt2<10)
		{
			
			buzz_cnt2++;
			Buzz_On();
		}
				if(buzz_cnt3==5)//闹钟3响铃5s
		{
			Buzz_Off();
			buzzoff_flag=1;
			buzz_cnt3=0;
			buzz_flag3=0;
		}
		else if(buzz_flag3==1&&buzz_cnt3<10)
		{
			Buzz_On();
			buzz_cnt3++;
		}
		
	}
	
	
}