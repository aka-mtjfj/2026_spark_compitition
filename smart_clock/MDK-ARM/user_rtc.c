#include "stm32f1xx.h"  
#include "Menu.h"
#include "usart.h"
#include "buzz.h"
    RTC_TimeTypeDef Time;// 定义全局时间/日期结构体
    RTC_DateTypeDef Date;
		uint8_t rtc_second_update_flag=0;
		uint8_t rtc_tamper_flag=0;
		//，
/**
  * @brief ：函数描述:秒中断更新时间并更新标志位
  * @param ：参数描述:无，每一秒自动回调
  * @return ：返回描述:无
  * @refer ：说明:主函数可以读取flay并读取全局变量更新，防止一秒内一直更新写入
  */
	uint8_t test[3];
void HAL_RTCEx_RTCEventCallback(RTC_HandleTypeDef *hrtc)
{
	uint32_t tim_cnt=0;//阈值时间
	uint32_t current_cnt=0;
	static uint8_t buzz_cnt1=0,buzz_cnt2=0,buzz_cnt3=0;
	static uint8_t buzz_flag1=0,buzz_flag2=0,buzz_flag3=0;
	if(hrtc->Instance == RTC)
  {
		
    HAL_RTC_GetTime(hrtc, &Time, RTC_FORMAT_BIN);//硬件要求先读时间再度日期否则数据无法刷新，更新数据到全局变量
    HAL_RTC_GetDate(hrtc, &Date, RTC_FORMAT_BIN);

    //置位更新标志,主循环中显示后需要清除标志位
		tim_cnt=Time.Hours*3600+Time.Minutes*60+Time.Seconds;
		if(clock_yn[0]==1&&tim_cnt==clock1_cnt)
		{//闹钟响铃
		buzz_flag1=1;
			}
			else if(clock_yn[1]==1&&tim_cnt==clock2_cnt)
		{//闹钟响铃
		buzz_flag2=1;
			}
			else if(clock_yn[2]==1&&tim_cnt==clock3_cnt)
		{//闹钟响铃
		buzz_flag3=1;
			}
		if(buzz_cnt1==10)//闹钟一响铃10s
		{
			Buzz_Off();
			buzz_cnt1=0;
			buzz_flag1=0;
		}
		else if(buzz_flag1==1&&buzz_cnt1<10)
		{
			Buzz_On();
			buzz_cnt1++;
		}
				if(buzz_cnt2==10)//闹钟2响铃10s
		{
			Buzz_Off();
			buzz_cnt2=0;
			buzz_flag2=0;
		}
		else if(buzz_flag2==1&&buzz_cnt2<10)//这里有个小bug，如果闹钟2比闹钟1设定的时间早五秒，那么闹钟一会在第五秒不响（使用同一个振动器），可以再加一个总体震荡标志位
		{
			
			buzz_cnt2++;
			Buzz_On();
		}
				if(buzz_cnt3==10)//闹钟3响铃10s
		{
			Buzz_Off();
			buzz_cnt3=0;
			buzz_flag3=0;
		}
		else if(buzz_flag3==1&&buzz_cnt3<10)
		{
			Buzz_On();
			buzz_cnt3++;
		}
		rtc_second_update_flag = 1;
	}
	
}

