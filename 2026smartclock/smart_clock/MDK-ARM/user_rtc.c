#include "stm32f1xx.h"  
#include "Menu.h"
    RTC_TimeTypeDef sTime;// 定义全局时间/日期结构体
    RTC_DateTypeDef sDate;
		uint8_t rtc_second_update_flag=0;
		uint8_t rtc_tamper_flag=0;
		//，
/**
  * @brief ：函数描述:秒中断更新时间并更新标志位
  * @param ：参数描述:无，每一秒自动回调
  * @return ：返回描述:无
  * @refer ：说明:主函数可以读取flay并读取全局变量更新，防止一秒内一直更新写入
  */
void HAL_RTCEx_RTCEventCallback(RTC_HandleTypeDef *hrtc)
{
	uint32_t tim_cnt=0;//阈值时间
	uint32_t current_cnt=0;
	if(hrtc->Instance == RTC)
  {
    HAL_RTC_GetTime(hrtc, &sTime, RTC_FORMAT_BCD);//硬件要求先读时间再度日期否则数据无法刷新，更新数据到全局变量
    HAL_RTC_GetDate(hrtc, &sDate, RTC_FORMAT_BCD);
    //置位更新标志,主循环中显示后需要清除标志位
		tim_cnt=sTime.Hours*3600+sTime.Minutes*60+sTime.Seconds;
    
		if(clock_yn[0]==1&&tim_cnt==clock1_cnt)
		{//闹钟响铃，
		//
		//
			}
			else if(clock_yn[1]==1&&tim_cnt==clock2_cnt)
		{//闹钟响铃，
		//
		//
			}
			else if(clock_yn[2]==1&&tim_cnt==clock3_cnt)
		{//闹钟响铃，
		//
		//
			}
		rtc_second_update_flag = 1;
	}
	
}

