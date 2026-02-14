#include "stm32f1xx.h" 
#ifndef __MyRTC_H
#define __MyRTC_H
void MyRTC_SetTime(void);
void MyRTC_ReadTime(void);
extern uint16_t MyRTC_Time[];
#endif
/**
MyRTC_SetTime需要在hal的RTC初始化后使用
更改数据在.c的时间数组里更改
需要time.h头文件
读取数据时调用即可得到现在时间
需要对RTC.c中初始化的 
if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
		}后到末尾注释掉或使用if(0),if(0)不会在更新时被消除
RTC_WriteTimeCounter（），RTC_ReadTimeCounter（），是库中static修饰的两个私有函数，
需要跳转到.c函数和声明处去掉static，后会有警告，忽视即可。
**/
