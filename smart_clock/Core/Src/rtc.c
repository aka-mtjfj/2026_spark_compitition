/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    rtc.c
  * @brief   This file provides code for the configuration
  *          of the RTC instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "rtc.h"

/* USER CODE BEGIN 0 */
#include "Menu.h"
/* USER CODE END 0 */

RTC_HandleTypeDef hrtc;

/* RTC init function */
void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */
		RTC_TimeTypeDef time;   //时间结构体参shu
		RTC_DateTypeDef datebuff;   //日期结构体参shu
  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef DateToUpdate = {0};

  /* USER CODE BEGIN RTC_Init 1 */
	__HAL_RCC_BKP_CLK_ENABLE();       //kai启后备区域时jian
	__HAL_RCC_PWR_CLK_ENABLE();		  //kai启电源时jian
	HAL_PWR_EnableBkUpAccess();       //使能备份寄存器访
  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
  hrtc.Init.OutPut = RTC_OUTPUTSOURCE_ALARM;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */
if(HAL_RTCEx_BKUPRead(&hrtc,RTC_BKP_DR1)!= 0x5051)
{
  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 0x10;
  sTime.Minutes = 0x27;
  sTime.Seconds = 0x33;

  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  DateToUpdate.WeekDay = RTC_WEEKDAY_FRIDAY;
  DateToUpdate.Month = RTC_MONTH_FEBRUARY;
  DateToUpdate.Date = 0x13;
  DateToUpdate.Year = 0x25;

  if (HAL_RTC_SetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */
	__HAL_RTC_SECOND_ENABLE_IT(&hrtc,RTC_IT_SEC);	 //kai启RTC时钟秒中duan
	datebuff = DateToUpdate;  //把日期数据拷贝到自己定义的data
	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, 0x5051);//向指定的后备区域寄存器写入数ju
//	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR2, (uint16_t)datebuff.Year);
//	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR3, (uint16_t)datebuff.Month);
//	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR4, (uint16_t)datebuff.Date);
	//	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR5, (uint16_t)datebuff.WeekDay);暂时保存三个闹钟的时分秒
	clock1_arr[0]=7;
	clock1_arr[1]=30;
	clock1_arr[2]=0;
	clock2_arr[0]=13;
	clock2_arr[1]=40;
	clock2_arr[2]=0;
	clock3_arr[0]=19;
	clock3_arr[1]=30;
	clock3_arr[2]=0;
  }
	else
	{
		//datebuff.WeekDay = HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR5);
		clock1_arr[0]= HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR2);
		clock1_arr[1]= HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR3);
		clock1_arr[2]= HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR4);
		clock2_arr[0]= HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR5);
		clock2_arr[1]= HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR6);
		clock2_arr[2]= HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR7);
		clock3_arr[0]= HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR8);
		clock3_arr[1]= HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR9);
		clock3_arr[2]= HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR10);
		DateToUpdate = datebuff;
		if (HAL_RTC_SetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BCD) != HAL_OK)
		{
			Error_Handler();
		}
			__HAL_RTC_SECOND_ENABLE_IT(&hrtc,RTC_IT_SEC);//kai启RTC时钟秒中duan
	}

  /* USER CODE END RTC_Init 2 */

}

void HAL_RTC_MspInit(RTC_HandleTypeDef* rtcHandle)
{

  if(rtcHandle->Instance==RTC)
  {
  /* USER CODE BEGIN RTC_MspInit 0 */

  /* USER CODE END RTC_MspInit 0 */
    HAL_PWR_EnableBkUpAccess();
    /* Enable BKP CLK enable for backup registers */
    __HAL_RCC_BKP_CLK_ENABLE();
    /* RTC clock enable */
    __HAL_RCC_RTC_ENABLE();

    /* RTC interrupt Init */
    HAL_NVIC_SetPriority(RTC_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(RTC_IRQn);
  /* USER CODE BEGIN RTC_MspInit 1 */

  /* USER CODE END RTC_MspInit 1 */
  }
}

void HAL_RTC_MspDeInit(RTC_HandleTypeDef* rtcHandle)
{

  if(rtcHandle->Instance==RTC)
  {
  /* USER CODE BEGIN RTC_MspDeInit 0 */

  /* USER CODE END RTC_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_RTC_DISABLE();

    /* RTC interrupt Deinit */
    HAL_NVIC_DisableIRQ(RTC_IRQn);
  /* USER CODE BEGIN RTC_MspDeInit 1 */

  /* USER CODE END RTC_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
