#include "stm32f1xx.h"                  // Device header
#include "OLED.h"
#include "Key.h"
#include "Delay.h"
#include "tim.h"
#include "user_rtc.h"
#include "rtc.h"
#include "usart.h"
uint8_t flag=1;
uint8_t DirectFlag=2;//用于指定动画向上还是向下
#define MENU1_LAST_CHOOSE   3
#define Timer_LAST_CHOOSE   4
#define Clock_LAST_CHOOSE   4
#define SETTING_LAST_CHOOSE   4      //这里指四行
uint8_t timer1_arr[3]={0,15,0};
uint8_t timer2_arr[3]={0,30,0};
uint8_t timer3_arr[3]={1,0,0};//参数，具体闹钟函数传参更改
uint8_t timer_yn[3]={0};//每个闹钟是否开启的标志，全局变量，定时器到时间中断更改这里的状态即可
uint8_t clock1_arr[3]={7,20,0};
uint8_t clock2_arr[3]={13,40,0};
uint8_t clock3_arr[3]={19,25,0};//参数，具体闹钟函数传参更改
uint8_t clock_yn[3]={2,2,2};//每个闹钟是否开启的标志，全局变量，定时器到时间中断更改这里的状态即可,开始默认关闭
uint32_t timer1_cnt=0,timer2_cnt=0,timer3_cnt=0;
uint32_t clock1_cnt=0,clock2_cnt=0,clock3_cnt=0;
/**
  * @brief ：函数描述:菜单一，用于第一级模式选择
  * @param ：参数描述:无
  * @return ：返回描述:返回进入的二级菜单序号
  * @refer ：说明:主循环中使用该函数，会进入选择，直到按键5选择返回。
  */
uint16_t Menu1()
{
	uint8_t Keynum;
	while(1)
	{
		Keynum=Key_GetNum();//获取按键
		if(Keynum==1)
		{
			DirectFlag=1;
			flag--;//按键1标志减一
			if(flag==0)
			{
				flag=MENU1_LAST_CHOOSE;
			}
			OLED_AnimUpdate();//需要先更新一下动画函数，为了下面的动画实施
		}
		else if(Keynum==3)
		{
			DirectFlag=2;
			flag++;//按键1标志加一
			if(flag==MENU1_LAST_CHOOSE+1)//如果超过最大可选择项
			{
				flag=1;
			}
			OLED_AnimUpdate();//滑动动画
		}
		else if(Keynum==5)//按键5确认进入
		{
			OLED_Clear();
			OLED_Update();//先刷新一下
			OLED_AnimUpdate();
			return flag;
		}
		switch(flag)//设定选择不同的选项时，对应显示的一级菜单，这里设置为超过4时翻页
		{
			case  1:
			{
				OLED_ShowString(0,0,"定时            ",OLED_8X16);
				OLED_ShowString(0,16,"闹钟设定        ",OLED_8X16);
				OLED_ShowString(0,32,"设置            ",OLED_8X16);
				if(DirectFlag==1)
				{
					OLED_Animation(0,16,128,16,0,0,128,16);//实施动画移动,1是减
				}
				else if(DirectFlag==2)
				{
					OLED_Animation(0,0,0,0,0,0,128,16);
				}
				break;
			}
					case  2:
			{
				OLED_ShowString(0,0,"定时            ",OLED_8X16);
				OLED_ShowString(0,16,"闹钟设定        ",OLED_8X16);
				OLED_ShowString(0,32,"设置            ",OLED_8X16);
				if(DirectFlag==1)
				{
					OLED_Animation(0,32,128,16,0,16,128,16);
				}
				else if(DirectFlag==2)
				{
					OLED_Animation(0,0,128,16,0,16,128,16);
				}
				break;				
			}
					case  3:
			{
				OLED_ShowString(0,0,"定时            ",OLED_8X16);
				OLED_ShowString(0,16,"闹钟设定        ",OLED_8X16);
				OLED_ShowString(0,32,"设置            ",OLED_8X16);
				if(DirectFlag==1)
				{
					OLED_Animation(0,48,128,0,0,32,128,16);
				}
				if(DirectFlag==2)
				{
					OLED_Animation(0,16,128,16,0,32,128,16);
				}
				break;
			}
		}
	}
}
/**
  * @brief ：函数描述:三级菜单的闹钟设定
  * @param ：参数描述:二级菜单中的时间参数数组,是否确认的数组
* @return ：返回描述:直接数组修改，0表示直接返回，1表示确认的返回，2表示取消的返回
  * @refer ：说明:传入的数组直接进行修改，确认后退出进行更改，每次刷新的时候读取数组，clock_yn是确认数组,取消是取消闹钟
  */
uint8_t Menu3_Clock_Setting(uint8_t* clock_arr)
{
	uint8_t tmp_arr[3];
	uint8_t Clock_setting_flag=1;
	uint8_t Clock_setting_direct=2;//动画滑动方向，1为向上，2为向下
	uint8_t Clock_setting_OK=0;//确定按下的是哪一个
	for(uint8_t i=0;i<3;i++)
	{
		tmp_arr[i]=clock_arr[i];//复制下数组，确认更改后才写入
	}
	OLED_Clear();
	OLED_ShowString(0,0,"<-               ",OLED_8X16);
	OLED_Printf(0,16,OLED_8X16,"  %2d时%2d分%2d秒         ",tmp_arr[0],tmp_arr[1],tmp_arr[2]);
	OLED_ShowString(0,48,"  确定    取消  ",OLED_8X16);
	OLED_Update();//初始化一下时间设置页面
	OLED_AnimUpdate();
	while(1)
	{
	OLED_ShowString(0,0,"<-               ",OLED_8X16);
	OLED_Printf(0,16,OLED_8X16,"  %2d时%2d分%2d秒         ",tmp_arr[0],tmp_arr[1],tmp_arr[2]);
	OLED_ShowString(0,48,"  确定    取消  ",OLED_8X16);
	uint8_t Keynum=Key_GetNum();
	if(Keynum==1)
		{
			Clock_setting_direct=1;
			Clock_setting_flag--;
			if(Clock_setting_flag==0)
				Clock_setting_flag=6;//6是最后一个可按的位置（取消）
			OLED_AnimUpdate();
		}
			else	if(Keynum==3)
		{
			Clock_setting_direct=2;
			Clock_setting_flag++;
			if(Clock_setting_flag==6+1)
				Clock_setting_flag=1;
			OLED_AnimUpdate();
		}
		else	if(Keynum==5)//上下调节更改timerflag，此时还没有选择menu3_flag
		{
			Clock_setting_OK=Clock_setting_flag;//确认选择
			//确定不需要移动动画
		}
		switch(Clock_setting_flag)//Timer_setting_flag在哪个位置的显示设置，界面不翻页，显示一次即可
		{
			case 1:{//返回
				if(Clock_setting_direct==1)
				{
					OLED_Animation(16,16,16,16,0,0,16,16);//实施动画移动,1是减
				}
				else if(Clock_setting_direct==2)
				{
					OLED_Animation(0,0,0,0,0,0,16,16);
				}
				break;
		}
			case 2:{//时
				if(Clock_setting_direct==1)
				{
					OLED_Animation(48,16,16,16,16,16,16,16);//实施动画移动,1是减
				}
				else if(Clock_setting_direct==2)
				{
					OLED_Animation(0,0,16,16,16,16,16,16);
				}
				break;
		}
					case 3:{//分
				if(Clock_setting_direct==1)
				{
					OLED_Animation(80,16,16,16,48,16,16,16);//实施动画移动,1是减
				}
				else if(Clock_setting_direct==2)
				{
					OLED_Animation(16,16,16,16,48,16,16,16);
				}
				break;
		}
					case 4:{//秒
				if(Clock_setting_direct==1)
				{
					OLED_Animation(16,48,32,16,80,16,16,16);//实施动画移动,1是减
				}
				else if(Clock_setting_direct==2)
				{
					OLED_Animation(48,16,16,16,80,16,16,16);
				}
				break;
		}
							case 5:{//确认
				if(Clock_setting_direct==1)
				{
					OLED_Animation(80,48,32,16,16,48,32,16);//实施动画移动,1是减
				}
				else if(Clock_setting_direct==2)
				{
					OLED_Animation(80,16,16,16,16,48,32,16);
				}
				break;
		}
						case 6:{//取消
				if(Clock_setting_direct==1)
				{
					OLED_Animation(112,64,0,0,80,48,32,16);//实施动画移动,1是减
				}
				else if(Clock_setting_direct==2)
				{
					OLED_Animation(16,48,32,16,80,48,32,16);
				}
				break;
		}
	}
	if(Clock_setting_OK==1)
		return 0;
	else if(Clock_setting_OK==2)
	{
		OLED_ReverseArea(16,16,16,16);
		OLED_DrawLine(16,31,31,31);
		OLED_UpdateArea(16,16,16,16);//按下确定后块反转变换为底部横线表示可更改，部分更改并更新
		while(1)
		{
			Keynum=Key_GetNum();
			if(Keynum==1)//小时数字减一
			{
				if(tmp_arr[0]!=0)
					tmp_arr[0]-=1;
				else 
					tmp_arr[0]=23;
				OLED_Printf(0,16,OLED_8X16,"  %2d时%2d分%2d秒         ",tmp_arr[0],tmp_arr[1],tmp_arr[2]);
				OLED_DrawLine(16,31,31,31);
				OLED_UpdateArea(0,16,128,16);//更新显示
			}
			else if(Keynum==3)
			{
				if(tmp_arr[0]!=23)
					tmp_arr[0]+=1;
				else
					tmp_arr[0]=0;
				OLED_Printf(0,16,OLED_8X16,"  %2d时%2d分%2d秒         ",tmp_arr[0],tmp_arr[1],tmp_arr[2]);
				OLED_DrawLine(16,31,31,31);
				OLED_UpdateArea(0,16,128,16);//更新显示
			}
			else if(Keynum==5)
			{
				OLED_Printf(0,16,OLED_8X16,"  %2d时%2d分%2d秒         ",tmp_arr[0],tmp_arr[1],tmp_arr[2]);
				OLED_ReverseArea(16,16,16,16);
				OLED_UpdateArea(0,16,128,16);//更新显示
				break;
			}
		}
	}
		else if(Clock_setting_OK==3)//调节分
	{
		OLED_ReverseArea(48,16,16,16);
		OLED_DrawLine(48,31,63,31);
		OLED_UpdateArea(0,16,128,16);//按下确定后块反转变换为底部横线表示可更改，部分更改并更新
		while(1)
		{
			Keynum=Key_GetNum();
			if(Keynum==1)//分钟数字减一
			{
				if(tmp_arr[1]!=0)
					tmp_arr[1]-=1;
				else 
					tmp_arr[1]=59;
				OLED_Printf(0,16,OLED_8X16,"  %2d时%2d分%2d秒         ",tmp_arr[0],tmp_arr[1],tmp_arr[2]);
				OLED_DrawLine(48,31,63,31);
				OLED_UpdateArea(0,16,128,16);//更新显示
			}
			else if(Keynum==3)
			{
				if(tmp_arr[1]!=59)
					tmp_arr[1]+=1;
				else
					tmp_arr[1]=0;
				OLED_Printf(0,16,OLED_8X16,"  %2d时%2d分%2d秒         ",tmp_arr[0],tmp_arr[1],tmp_arr[2]);
				OLED_DrawLine(48,31,63,31);
				OLED_UpdateArea(0,16,128,16);//更新显示
			}
			else if(Keynum==5)
			{
				OLED_Printf(0,16,OLED_8X16,"  %2d时%2d分%2d秒         ",tmp_arr[0],tmp_arr[1],tmp_arr[2]);
				OLED_ReverseArea(48,16,16,16);
				OLED_UpdateArea(0,16,128,16);//更新显示
				break;
			}
		}
	}
			else if(Clock_setting_OK==4)//调节秒
	{
		OLED_ReverseArea(80,16,16,16);
		OLED_DrawLine(80,31,95,31);
		OLED_UpdateArea(0,16,128,16);//按下确定后块反转变换为底部横线表示可更改，部分更改并更新
		while(1)
		{
			Keynum=Key_GetNum();
			if(Keynum==1)//分钟数字减一
			{
				if(tmp_arr[1]!=0)
					tmp_arr[1]-=1;
				else 
					tmp_arr[1]=59;
				OLED_Printf(0,16,OLED_8X16,"  %2d时%2d分%2d秒         ",tmp_arr[0],tmp_arr[1],tmp_arr[2]);
				OLED_DrawLine(80,31,95,31);
				OLED_UpdateArea(0,16,128,16);//更新显示
			}
			else if(Keynum==3)
			{
				if(tmp_arr[1]!=59)
					tmp_arr[1]+=1;
				else
					tmp_arr[1]=0;
				OLED_Printf(0,16,OLED_8X16,"  %2d时%2d分%2d秒         ",tmp_arr[0],tmp_arr[1],tmp_arr[2]);
				OLED_DrawLine(80,31,95,31);
				OLED_UpdateArea(0,16,128,16);//更新显示
			}
			else if(Keynum==5)
			{
				OLED_Printf(0,16,OLED_8X16,"  %2d时%2d分%2d秒         ",tmp_arr[0],tmp_arr[1],tmp_arr[2]);
				OLED_ReverseArea(80,16,16,16);
				OLED_UpdateArea(0,16,128,16);//更新显示
				break;
			}
		}
	}
	else if(Clock_setting_OK==5)
	{
		//确认并退出
				for(uint8_t i=0;i<3;i++)//设定闹钟
		{
			clock_arr[i]=tmp_arr[i];
		}
		return 1;
	}
		else if(Clock_setting_OK==6)
	{
		//取消并退出
		
		return 2;
	}
}
	
}
/**
  * @brief ：函数描述:定时，设定定时时间
  * @param ：参数描述:无
  * @return ：返回描述:无
	* @refer ：说明:具体定时设置在函数中设置，而不依靠这个二级菜单
  */
uint16_t Menu2_Clock()
{
	uint8_t Menu3_Flag=0; 
	uint8_t Keynum=0;
	uint8_t Clock_Direct_Flag=2; //定时器菜单动画上下
	uint8_t Clock_flag=1;//设定初始指向第一行

	uint8_t ret=0;
	OLED_ShowString(0,0,"<-               ",OLED_8X16);//第一行是返回键
	if(clock_yn[0]==1)
		OLED_Printf(0,16,OLED_8X16,"%2d时%2d分%2d秒   *       ",clock1_arr[0],clock1_arr[1],clock1_arr[2]);//设定第一页的显示
	else
		OLED_Printf(0,16,OLED_8X16,"%2d时%2d分%2d秒           ",clock1_arr[0],clock1_arr[1],clock1_arr[2]);
if(clock_yn[1]==1)
		OLED_Printf(0,32,OLED_8X16,"%2d时%2d分%2d秒   *       ",clock2_arr[0],clock2_arr[1],clock2_arr[2]);//设定第一页的显示
	else
		OLED_Printf(0,32,OLED_8X16,"%2d时%2d分%2d秒           ",clock2_arr[0],clock2_arr[1],clock2_arr[2]);
if(clock_yn[2]==1)
		OLED_Printf(0,48,OLED_8X16,"%2d时%2d分%2d秒   *       ",clock3_arr[0],clock3_arr[1],clock3_arr[2]);//设定第一页的显示
	else
		OLED_Printf(0,48,OLED_8X16,"%2d时%2d分%2d秒           ",clock3_arr[0],clock3_arr[1],clock3_arr[2]);
	OLED_Update();//更新显示内容
	OLED_AnimUpdate();
	while(1)
	{
			OLED_ShowString(0,0,"<-               ",OLED_8X16);//第一行是返回键
	if(clock_yn[0]==1)
		OLED_Printf(0,16,OLED_8X16,"%2d时%2d分%2d秒   *       ",clock1_arr[0],clock1_arr[1],clock1_arr[2]);//设定第一页的显示
	else
		OLED_Printf(0,16,OLED_8X16,"%2d时%2d分%2d秒           ",clock1_arr[0],clock1_arr[1],clock1_arr[2]);
if(clock_yn[1]==1)
		OLED_Printf(0,32,OLED_8X16,"%2d时%2d分%2d秒   *       ",clock2_arr[0],clock2_arr[1],clock2_arr[2]);//设定第一页的显示
	else
		OLED_Printf(0,32,OLED_8X16,"%2d时%2d分%2d秒           ",clock2_arr[0],clock2_arr[1],clock2_arr[2]);
if(clock_yn[2]==1)
		OLED_Printf(0,48,OLED_8X16,"%2d时%2d分%2d秒   *       ",clock3_arr[0],clock3_arr[1],clock3_arr[2]);//设定第一页的显示
	else
		OLED_Printf(0,48,OLED_8X16,"%2d时%2d分%2d秒           ",clock3_arr[0],clock3_arr[1],clock3_arr[2]);
		Keynum=Key_GetNum();//没有按下的时候返回的是0
		if(Keynum==1)
		{
			Clock_Direct_Flag=1;//1表示向下
			Clock_flag--;
			if(Clock_flag==0)
				Clock_flag=Clock_LAST_CHOOSE;
			OLED_AnimUpdate();
		}
			else	if(Keynum==3)
		{
			Clock_Direct_Flag=2;//1表示向上
			Clock_flag++;
			if(Clock_flag==Clock_LAST_CHOOSE+1)
				Clock_flag=1;
			OLED_AnimUpdate();
		}
		else	if(Keynum==5)//上下调节更改clockflag，此时还没有选择menu3_flag
		{
			OLED_Clear();
			OLED_Update();
			Menu3_Flag=Clock_flag;//确认选择
			
		}
	//这里不需要update，因为下面的动画函数中会update一次
	switch(Clock_flag)//上面已经做好了选择，这里给个显示
		{
			case 1:{

				if(Clock_Direct_Flag==2)
				{
					OLED_Animation(0,0,0,0,0,0,128,16);
				}
				else if(Clock_Direct_Flag==1)//动画为减的动画
				{
					OLED_Animation(0,16,128,16,0,0,128,16);
				}
			break;
			}
			case 2:{

				if(Clock_Direct_Flag==2)
				{
					OLED_Animation(0,0,128,16,0,16,128,16);
				}
				else if(Clock_Direct_Flag==1)//动画为减的动画
				{
					OLED_Animation(0,32,128,16,0,16,128,16);
				}
			break;
			}
			case 3:{

				if(Clock_Direct_Flag==2)
				{
					OLED_Animation(0,16,128,16,0,32,128,16);
				}
				else if(Clock_Direct_Flag==1)//动画为减的动画
				{
					OLED_Animation(0,48,128,16,0,32,128,16);
				}
			break;
			}
			case 4:{

				if(Clock_Direct_Flag==2)
				{
					OLED_Animation(0,32,128,16,0,48,128,16);
				}
				else if(Clock_Direct_Flag==1)//动画为减的动画
				{
					OLED_Animation(0,64,128,0,0,48,128,16);
				}
			break;
			}
		}
		
		if(Menu3_Flag==1)//和上面的显示其实可以交换，只有menu3_flag选择后才会进入
		{
			OLED_AnimUpdate();
			return 0;//第一行返回
		}
			else	if(Menu3_Flag==2)
		{
			//设定时间并返回一个确认或取消的返回值，根据这个值继续操作
			ret=Menu3_Clock_Setting(clock1_arr);//固定传三个定时参数并更改
			if(ret==1)
			{
				clock_yn[0]=1;
				clock1_cnt=clock1_arr[0]*3600+clock1_arr[1]*60+clock1_arr[2];
				//设定时间为clock1_arr并启动，此处更改yn数组上面的循环显示会自动标上 * 标识符
				//这里直接将确认数组置一，此时只匹配时间，不匹配日期
			}
			else if(ret==2)
			{
				clock_yn[0]=2;//第一行返回的话是0，但不对确认数组做处理

			}
		}
			else	if(Menu3_Flag==3)
		{
			ret=Menu3_Clock_Setting(clock2_arr);
						if(ret==1)
			{
				clock_yn[0]=1;
				clock2_cnt=clock2_arr[0]*3600+clock2_arr[1]*60+clock2_arr[2];
			}
			else if(ret==2)
			{
				clock_yn[0]=2;//第一行返回的话是0，但不对确认数组做处理

			}
		}
			else	if(Menu3_Flag==4)
		{
			ret=Menu3_Clock_Setting(clock3_arr);
						if(ret==1)
			{
				clock_yn[0]=1;
				clock3_cnt=clock3_arr[0]*3600+clock3_arr[1]*60+clock3_arr[2];
			}
			else if(ret==2)
			{
				clock_yn[0]=2;
			}
		}
	}
}

/**
  * @brief ：函数描述:系统设置
  * @param ：参数描述:无
  * @return ：返回描述:无
  * @refer ：说明:包括省电模式，wifi拓展和系统信息
  */
uint16_t Menu2_Setting()
{
	uint8_t Menu3_Flag=0; 
	uint8_t Setting_flag=1;
	uint8_t Keynum=0;
	uint8_t Setting_Direct_flag=2;
	OLED_ShowString(0,0,"<-              ",OLED_8X16);
	OLED_ShowString(0,16,"省电模式              ",OLED_8X16);
	OLED_ShowString(0,32,"wifi拓展          ",OLED_8X16);
	OLED_ShowString(0,48,"关于闹钟             ",OLED_8X16);                                                                                                                                                                                                                                     
	OLED_Update();
	OLED_AnimUpdate();
	while(1)
	{
			OLED_ShowString(0,0,"<-              ",OLED_8X16);
	OLED_ShowString(0,16,"省电模式              ",OLED_8X16);
	OLED_ShowString(0,32,"wifi拓展          ",OLED_8X16);
	OLED_ShowString(0,48,"关于闹钟             ",OLED_8X16);//这里的循环每次必须要重写入一次，否则会混乱
		 
		Keynum=Key_GetNum();
		if(Keynum==1)
		{
			Setting_Direct_flag=1;
			Setting_flag--;
			if(Setting_flag==0)
				Setting_flag=SETTING_LAST_CHOOSE;
			OLED_AnimUpdate();
		}
			else	if(Keynum==3)
		{
			Setting_flag++;
			if(Setting_flag==SETTING_LAST_CHOOSE+1)
				Setting_flag=1;
			Setting_Direct_flag=2;
			OLED_AnimUpdate();
			
		}
			else	if(Keynum==5)
		{
			OLED_Clear();
			OLED_Update();
			Menu3_Flag=Setting_flag;
			//确定时不需要再做动画显示
		}
		
		switch(Setting_flag)//根据选择做出动画显示
		{
			case  1:
			{
				if(Setting_Direct_flag==1)
				{
					OLED_Animation(0,16,128,16,0,0,128,16);//实施动画移动,1是减
				}
				else if(Setting_Direct_flag==2)
				{
					OLED_Animation(0,0,128,0,0,0,128,16);
				}
				break;
			}
					case  2:
			{
				if(Setting_Direct_flag==1)
				{
					OLED_Animation(0,32,128,16,0,16,128,16);
				}
				else if(Setting_Direct_flag==2)
				{
					OLED_Animation(0,0,128,16,0,16,128,16);
				}
				break;				
			}
					case  3:
			{
				if(Setting_Direct_flag==1)
				{
					OLED_Animation(0,48,128,16,0,32,128,16);
				}
				if(Setting_Direct_flag==2)
				{
					OLED_Animation(0,16,128,16,0,32,128,16);
				}
				break;
			}
			case  4:
			{
				if(Setting_Direct_flag==1)
				{
					OLED_Animation(0,64,128,0,0,48,128,16);
				}
				if(Setting_Direct_flag==2)
				{
					OLED_Animation(0,32,128,16,0,48,128,16);
				}
				break;
			}
		}
		
		if(Menu3_Flag==1)
		{
			OLED_AnimUpdate();
			return 0;
		}
			else	if(Menu3_Flag==2)
		{
			//省电功能
			//
			//
			//
		}
			else	if(Menu3_Flag==3)
		{
			//wifi拓展
			//
			//
			//
		}
			else	if(Menu3_Flag==4)
		{
			//说明网址
			//
			//
			//
		}

	}
}

/**
  * @brief ：函数描述:三级菜单的时间设定
  * @param ：参数描述:二级菜单中的时间参数数组,是否确认的数组
* @return ：返回描述:直接数组修改，0表示直接返回，1表示确认的返回，2表示取消的返回
  * @refer ：说明:传入的数组直接进行修改，确认后退出进行更改，每次刷新的时候读取数组，timer_yn是确认数组,取消是取消闹钟
  */
uint8_t Menu3_Timer_Setting(uint8_t* timer_arr)
{
	uint8_t tmp_arr[3];
	uint8_t Keynum=0;
	uint8_t Timer_setting_flag=1;
	uint8_t timer_setting_direct=2;//动画滑动方向，1为向上，2为向下
	uint8_t Timer_setting_OK=0;//确定按下的是哪一个
	uint8_t test[]="test\r\n";
	for(uint8_t i=0;i<3;i++)
	{
		tmp_arr[i]=timer_arr[i];//复制下数组，确认更改后才写入
	}
	OLED_Clear();

	OLED_AnimUpdate();
	while(1)
	{
	OLED_ShowString(0,0,"<-               ",OLED_8X16);
	OLED_Printf(0,16,OLED_8X16,"  %2d时%2d分%2d秒         ",tmp_arr[0],tmp_arr[1],tmp_arr[2]);
	OLED_ShowString(0,48,"  确定    取消  ",OLED_8X16);
	uint8_t keynum=Key_GetNum();
	if(keynum==1)
		{
			timer_setting_direct=1;
			Timer_setting_flag--;
			if(Timer_setting_flag==0)
				Timer_setting_flag=6;//6是最后一个可按的位置（取消）
			OLED_AnimUpdate();
		}
			else	if(keynum==3)
		{
			timer_setting_direct=2;
			Timer_setting_flag++;
			if(Timer_setting_flag==6+1)
				Timer_setting_flag=1;
			OLED_AnimUpdate();
		}
		else	if(keynum==5)//上下调节更改timerflag，此时还没有选择menu3_flag
		{
			Timer_setting_OK=Timer_setting_flag;//确认选择
			OLED_AnimUpdate();
		}
		switch(Timer_setting_flag)//Timer_setting_flag在哪个位置的显示设置，界面不翻页，显示一次即可
		{
			case 1:{//返回
				if(timer_setting_direct==1)
				{
					OLED_Animation(16,16,16,16,0,0,16,16);//实施动画移动,1是减
				}
				else if(timer_setting_direct==2)
				{
					OLED_Animation(0,0,16,0,0,0,16,16);
				}
				break;
		}
			case 2:{//时
				if(timer_setting_direct==1)
				{
					OLED_Animation(48,16,16,16,16,16,16,16);//实施动画移动,1是减
				}
				else if(timer_setting_direct==2)
				{
					OLED_Animation(0,0,16,16,16,16,16,16);
				}
				break;
		}
					case 3:{//分
				if(timer_setting_direct==1)
				{
					OLED_Animation(80,16,16,16,48,16,16,16);//实施动画移动,1是减
				}
				else if(timer_setting_direct==2)
				{
					OLED_Animation(16,16,16,16,48,16,16,16);
				}
				break;
		}
					case 4:{//秒
				if(timer_setting_direct==1)
				{
					OLED_Animation(16,48,32,16,80,16,16,16);//实施动画移动,1是减
				}
				else if(timer_setting_direct==2)
				{
					OLED_Animation(48,16,16,16,80,16,16,16);
				}
				break;
		}
							case 5:{//确认
				if(timer_setting_direct==1)
				{
					OLED_Animation(80,48,32,16,16,48,32,16);//实施动画移动,1是减
				}
				else if(timer_setting_direct==2)
				{
					OLED_Animation(80,16,16,16,16,48,32,16);
				}
				break;
		}
						case 6:{//取消
				if(timer_setting_direct==1)
				{
					OLED_Animation(80,64,0,0,80,48,32,16);//实施动画移动,1是减
				}
				else if(timer_setting_direct==2)
				{
					OLED_Animation(16,48,32,16,80,48,32,16);
				}
				break;
		}
	}
	if(Timer_setting_OK==1)
	{
		OLED_AnimUpdate();
		return 0;
	}
	else if(Timer_setting_OK==2)
	{
		OLED_ReverseArea(16,16,16,16);
		OLED_DrawLine(16,31,31,31);
		OLED_UpdateArea(16,16,16,16);//按下确定后块反转变换为底部横线表示可更改，部分更改并更新
		while(1)
		{
			keynum=Key_GetNum();
			if(keynum==1)//小时数字减一
			{
				if(tmp_arr[0]!=0)
					tmp_arr[0]-=1;
				else 
					tmp_arr[0]=23;
				OLED_Printf(0,16,OLED_8X16,"  %2d时%2d分%2d秒         ",tmp_arr[0],tmp_arr[1],tmp_arr[2]);
				OLED_DrawLine(16,31,31,31);
				OLED_UpdateArea(0,16,128,16);//更新显示
			}
			else if(keynum==3)
			{
				if(tmp_arr[0]!=23)
					tmp_arr[0]+=1;
				else
					tmp_arr[0]=0;
				OLED_Printf(0,16,OLED_8X16,"  %2d时%2d分%2d秒         ",tmp_arr[0],tmp_arr[1],tmp_arr[2]);
				OLED_DrawLine(16,31,31,31);
				OLED_UpdateArea(0,16,128,16);//更新显示
			}
			else if(keynum==5)
			{
				OLED_Printf(0,16,OLED_8X16,"  %2d时%2d分%2d秒         ",tmp_arr[0],tmp_arr[1],tmp_arr[2]);
				OLED_ReverseArea(16,16,16,16);
				OLED_UpdateArea(0,16,128,16);//更新显示
				break;
			}
		}
	}
		else if(Timer_setting_OK==3)//调节分
	{
		OLED_ReverseArea(48,16,16,16);
		OLED_DrawLine(48,31,63,31);
		OLED_UpdateArea(0,16,128,16);//按下确定后块反转变换为底部横线表示可更改，部分更改并更新
		while(1)
		{
			keynum=Key_GetNum();
			if(keynum==1)//分钟数字减一
			{
				if(tmp_arr[1]!=0)
					tmp_arr[1]-=1;
				else 
					tmp_arr[1]=59;
				OLED_Printf(0,16,OLED_8X16,"  %2d时%2d分%2d秒         ",tmp_arr[0],tmp_arr[1],tmp_arr[2]);
				OLED_DrawLine(48,31,63,31);
				OLED_UpdateArea(0,16,128,16);//更新显示
			}
			else if(keynum==3)
			{
				if(tmp_arr[1]!=59)
					tmp_arr[1]+=1;
				else
					tmp_arr[1]=0;
				OLED_Printf(0,16,OLED_8X16,"  %2d时%2d分%2d秒         ",tmp_arr[0],tmp_arr[1],tmp_arr[2]);
				OLED_DrawLine(48,31,63,31);
				OLED_UpdateArea(0,16,128,16);//更新显示
			}
			else if(keynum==5)
			{
				OLED_Printf(0,16,OLED_8X16,"  %2d时%2d分%2d秒         ",tmp_arr[0],tmp_arr[1],tmp_arr[2]);
				OLED_ReverseArea(48,16,16,16);
				OLED_UpdateArea(0,16,128,16);//更新显示
				break;
			}
		}
	}
			else if(Timer_setting_OK==4)//调节秒
	{
		OLED_ReverseArea(80,16,16,16);
		OLED_DrawLine(80,31,95,31);
		OLED_UpdateArea(0,16,128,16);//按下确定后块反转变换为底部横线表示可更改，部分更改并更新
		while(1)
		{
			keynum=Key_GetNum();
			if(keynum==1)//分钟数字减一
			{
				if(tmp_arr[1]!=0)
					tmp_arr[1]-=1;
				else 
					tmp_arr[1]=59;
				OLED_Printf(0,16,OLED_8X16,"  %2d时%2d分%2d秒         ",tmp_arr[0],tmp_arr[1],tmp_arr[2]);
				OLED_DrawLine(80,31,95,31);
				OLED_UpdateArea(0,16,128,16);//更新显示
			}
			else if(keynum==3)
			{
				if(tmp_arr[1]!=59)
					tmp_arr[1]+=1;
				else
					tmp_arr[1]=0;
				OLED_Printf(0,16,OLED_8X16,"  %2d时%2d分%2d秒         ",tmp_arr[0],tmp_arr[1],tmp_arr[2]);
				OLED_DrawLine(80,31,95,31);
				OLED_UpdateArea(0,16,128,16);//更新显示
			}
			else if(keynum==5)
			{
				OLED_Printf(0,16,OLED_8X16,"  %2d时%2d分%2d秒         ",tmp_arr[0],tmp_arr[1],tmp_arr[2]);
				OLED_ReverseArea(80,16,16,16);
				OLED_UpdateArea(0,16,128,16);//更新显示
				break;
			}
		}
	}
	else if(Timer_setting_OK==5)
	{
		//确认并退出,确认时已经写入全局数组
		for(uint8_t i=0;i<3;i++)
		{
			timer_arr[i]=tmp_arr[i];
		}
		return 1;
	}
		else if(Timer_setting_OK==6)
	{
		//取消并退出
		return 2;
	}
}
	
}
/**
  * @brief ：函数描述:定时，设定定时时间
  * @param ：参数描述:无
  * @return ：返回描述:无
	* @refer ：说明:具体定时设置在函数中设置，而不依靠这个二级菜单
  */
uint16_t Menu2_Timer()
{
	uint8_t Menu3_Flag=0; 
	uint8_t Timer_Direct_Flag=2; //定时器菜单动画上下
	uint8_t Timer_flag=1;//设定初始指向第一行
	uint8_t Keynum=0;
	uint8_t ret=0;
	OLED_ShowString(0,0,"<-               ",OLED_8X16);//第一行是返回键
	if(timer_yn[0]==1)
		OLED_Printf(0,16,OLED_8X16,"%2d时%2d分%2d秒   *       ",timer1_arr[0],timer1_arr[1],timer1_arr[2]);//设定第一页的显示
	else
		OLED_Printf(0,16,OLED_8X16,"%2d时%2d分%2d秒           ",timer1_arr[0],timer1_arr[1],timer1_arr[2]);
if(timer_yn[1]==1)
		OLED_Printf(0,32,OLED_8X16,"%2d时%2d分%2d秒   *       ",timer2_arr[0],timer2_arr[1],timer2_arr[2]);//设定第一页的显示
	else
		OLED_Printf(0,32,OLED_8X16,"%2d时%2d分%2d秒           ",timer2_arr[0],timer2_arr[1],timer2_arr[2]);
if(timer_yn[2]==1)
		OLED_Printf(0,48,OLED_8X16,"%2d时%2d分%2d秒   *       ",timer3_arr[0],timer3_arr[1],timer3_arr[2]);//设定第一页的显示
	else
		OLED_Printf(0,48,OLED_8X16,"%2d时%2d分%2d秒           ",timer3_arr[0],timer3_arr[1],timer3_arr[2]);
	OLED_Update();//更新显示内容
	OLED_AnimUpdate();
	while(1)
	{
			OLED_ShowString(0,0,"<-               ",OLED_8X16);//第一行是返回键
	if(timer_yn[0]==1)
		OLED_Printf(0,16,OLED_8X16,"%2d时%2d分%2d秒   *       ",timer1_arr[0],timer1_arr[1],timer1_arr[2]);//设定第一页的显示
	else
		OLED_Printf(0,16,OLED_8X16,"%2d时%2d分%2d秒           ",timer1_arr[0],timer1_arr[1],timer1_arr[2]);
if(timer_yn[1]==1)
		OLED_Printf(0,32,OLED_8X16,"%2d时%2d分%2d秒   *       ",timer2_arr[0],timer2_arr[1],timer2_arr[2]);//设定第一页的显示
	else
		OLED_Printf(0,32,OLED_8X16,"%2d时%2d分%2d秒           ",timer2_arr[0],timer2_arr[1],timer2_arr[2]);
if(timer_yn[2]==1)
		OLED_Printf(0,48,OLED_8X16,"%2d时%2d分%2d秒   *       ",timer3_arr[0],timer3_arr[1],timer3_arr[2]);//设定第一页的显示
	else
		OLED_Printf(0,48,OLED_8X16,"%2d时%2d分%2d秒           ",timer3_arr[0],timer3_arr[1],timer3_arr[2]);

		Keynum=Key_GetNum();//没有按下的时候返回的是0
		if(Keynum==1)
		{
			Timer_Direct_Flag=1;//1表示向下
			Timer_flag--;
			if(Timer_flag==0)
				Timer_flag=Timer_LAST_CHOOSE;
			OLED_AnimUpdate();
		}
			else	if(Keynum==3)
		{
			Timer_Direct_Flag=2;//1表示向上
			Timer_flag++;
			if(Timer_flag==Timer_LAST_CHOOSE+1)
				Timer_flag=1;
			OLED_AnimUpdate();
		}
		else	if(Keynum==5)//上下调节更改timerflag，此时还没有选择menu3_flag
		{
			OLED_Clear();
			OLED_Update();
			OLED_AnimUpdate();//进入下一个函数可以初始更新
			Menu3_Flag=Timer_flag;//确认选择
			
		}

	//这里不需要update，因为下面的动画函数中会update一次
		switch(Timer_flag)
		{
			case 1:{

				if(Timer_Direct_Flag==2)
				{
					OLED_Animation(0,0,128,0,0,0,128,16);
				}
				else if(Timer_Direct_Flag==1)//动画为减的动画
				{
					OLED_Animation(0,16,128,16,0,0,128,16);
				}
			break;
			}
			case 2:{

				if(Timer_Direct_Flag==2)
				{
					OLED_Animation(0,0,128,16,0,16,128,16);
				}
				else if(Timer_Direct_Flag==1)//动画为减的动画
				{
					OLED_Animation(0,32,128,16,0,16,128,16);
				}
			break;
			}
			case 3:{

				if(Timer_Direct_Flag==2)
				{
					OLED_Animation(0,16,128,16,0,32,128,16);
				}
				else if(Timer_Direct_Flag==1)//动画为减的动画
				{
					OLED_Animation(0,48,128,16,0,32,128,16);
				}
			break;
			}
			case 4:{

				if(Timer_Direct_Flag==2)
				{
					OLED_Animation(0,32,128,16,0,48,128,16);
				}
				else if(Timer_Direct_Flag==1)//动画为减的动画
				{
					OLED_Animation(0,64,128,0,0,48,128,16);
				}
			break;
			}
		}
		
		if(Menu3_Flag==1)//和上面的显示其实可以交换，只有menu3_flag选择后才会进入
		{
			OLED_AnimUpdate();
			return 0;//第一行返回
		}
			else	if(Menu3_Flag==2)
		{
			//设定时间并返回一个确认或取消的返回值，根据这个值继续操作
			ret=Menu3_Timer_Setting(timer1_arr);//固定传三个定时参数并更改
			if(ret==1)
			{
				timer_yn[0]=1;
				//确认时已经参数已经写入全局数组
				//设定时间为timer1_arr并启动，此处更改yn数组上面的循环显示会自动标上 * 标识符
				timer1_cnt=timer1_arr[0]*3600+timer1_arr[1]*60+timer1_arr[2];//定时器总秒数

			}
			else if(ret==2)
			{
				timer_yn[0]=2;//第一行返回的话是0，但不对确认数组做处理
				//关闭定时器，直接使用三个定时器对应，因为及时会闲置也要保证定时功能
				//关闭即可，不需要管timer1_cnt
			}
			Menu3_Flag=0;//完成时间设置后重置选择序号，防止下次循环再次进入
		}
			else	if(Menu3_Flag==3)
		{
			ret=Menu3_Timer_Setting(timer2_arr);
						if(ret==1)
			{
				timer_yn[1]=1;
				//
				//设定时间为timer1_arr并启动，此处更改yn数组上面的循环显示会自动标上 * 标识符
				timer2_cnt=timer2_arr[0]*3600+timer2_arr[1]*60+timer2_arr[2];
			}
			else if(ret==2)
			{
				timer_yn[1]=2;//第一行返回的话是0，但不对确认数组做处理
				//关闭定时器，直接使用三个定时器对应，因为及时会闲置也要保证定时功能

			}
		}
			else	if(Menu3_Flag==4)
		{
			ret=Menu3_Timer_Setting(timer2_arr);
						if(ret==1)
			{
				timer_yn[2]=1;
				//
				//设定时间为timer1_arr并启动，此处更改yn数组上面的循环显示会自动标上 * 标识符
				timer3_cnt=timer3_arr[0]*3600+timer3_arr[1]*60+timer3_arr[2];
			}
			else if(ret==2)
			{
				timer_yn[2]=2;//第一行返回的话是0，但不对确认数组做处理
				//关闭定时器，直接使用三个定时器对应，因为及时会闲置也要保证定时功能

			}
		}
		//使用一个定时器，如果一个定时都没有设置就关闭，否则一直开启，三个定时共用一个定时器
		if(timer_yn[0]==2&&timer_yn[1]==2&&timer_yn[2]==2)
		{
				HAL_TIM_Base_Stop_IT(&htim1);
				__HAL_TIM_SET_COUNTER(&htim1,0);
		}
		else
			{
				if((htim1.Instance->CR1 & TIM_CR1_CEN)==0)//如果yn不是全是2并且还没打开定时器，则打开
			{
				__HAL_TIM_SET_COUNTER(&htim1,0);
				HAL_TIM_Base_Start_IT(&htim1);
			}
			
		}
	}
}
