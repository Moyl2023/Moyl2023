
#include "rtc.h"
#include "stm32f10x_bkp.h"
#include "stm32f10x_rtc.h"

#include "gpio.h"

/*******************************************************************************
* 本文件实现基于RTC的日期功能，提供年月日的读写。（基于ANSI-C的time.h）
* ANSI-C的标准库中，提供了两种表示时间的数据  型：
* time_t:   	UNIX时间戳（从1970-1-1起到某时间经过的秒数）
* 	typedef unsigned int time_t;
* 
* struct tm:	Calendar格式（年月日形式）
*   tm结构如下：
*   struct tm {
*   	int tm_sec;   // 秒 seconds after the minute, 0 to 60
*   					 (0 - 60 allows for the occasional leap second)
*   	int tm_min;   // 分 minutes after the hour, 0 to 59
*		int tm_hour;  // 时 hours since midnight, 0 to 23
*		int tm_mday;  // 日 day of the month, 1 to 31
*		int tm_mon;   // 月 months since January, 0 to 11
*		int tm_year;  // 年 years since 1900
*		int tm_wday;  // 星期 days since Sunday, 0 to 6
*		int tm_yday;  // 从元旦起的天数 days since January 1, 0 to 365
* 		int tm_isdst; // 夏令时？？Daylight Savings Time flag
* 		...
* 	}
* 	其中wday，yday可以自动产生，软件直接读取
* 	mon的取值为0-11
*	***注意***：
*	tm_year:在time.h库中定义为1900年起的年份，即2008年应表示为2008-1900=108
* 	这种表示方法对用户来说不是十分友好，与现实有较大差异。
* 	所以在本文件中，屏蔽了这种差异。
* 	即外部调用本文件的函数时，tm结构体类型的日期，tm_year即为2008
* 	注意：若要调用系统库time.c中的函数，需要自行将tm_year-=1900
*******************************************************************************/

/*******************************************************************************
* Function Name  : Time_ConvUnixToCalendar(time_t t)
* Description    : RTC时间 -> 日历时间
* Input 		 : u32 t  当前RTC时间
* Output		 : None
* Return		 : struct tm
*******************************************************************************/
struct tm Time_ConvUnixToCalendar(time_t t)
{
	struct tm *t_tm;
	t_tm = localtime(&t);
	t_tm->tm_year += 1900;	//localtime转换结果的tm_year是相对值，需要转成绝对值
	return *t_tm;
}

/*******************************************************************************
* Function Name  : Time_ConvCalendarToUnix(struct tm t)
* Description    : 转化calendar时钟到RTC时钟 
* Input 		 : struct tm t
* Output		 : None
* Return		 : time_t
*******************************************************************************/
time_t Time_ConvCalendarToUnix(struct tm t)
{
	t.tm_year -= 1900; 
	return mktime(&t);
}

/*******************************************************************************
* Function Name  : Time_GetUnixTime()
* Description    : 获取RTC时间
* Input 		 : None
* Output		 : None
* Return		 : time_t t
*******************************************************************************/
time_t Time_GetUnixTime(void)
{
	return (time_t)RTC_GetCounter();
}
/*******************************************************************************
* Function Name  : Time_GetCalendarTime()
* Description    : 获取日历时间（struct tm）
* Input 		 : None
* Output		 : None
* Return		 : time_t t
*******************************************************************************/
struct tm Time_GetCalendarTime(void)
{
	time_t t_t;
	struct tm t_tm;

	t_t = (time_t)RTC_GetCounter();
	t_tm = Time_ConvUnixToCalendar(t_t);
	t_tm.tm_mon += 1; 		
	return t_tm;
}

/*******************************************************************************
* Function Name  : Time_SetUnixTime()
* Description    : 写入RTC时间
* Input 		 : time_t t
* Output		 : None
* Return		 : None
*******************************************************************************/
void Time_SetUnixTime(time_t t)
{
	RTC_WaitForLastTask();
	RTC_SetCounter((u32)t);
	RTC_WaitForLastTask();
	return;
}

/*******************************************************************************
* Function Name  : Time_SetCalendarTime()
* Description    : 将给定的Calendar格式时间写入RTC
* Input 		 : struct tm t
* Output		 : None
* Return		 : None
*******************************************************************************/
void Time_SetCalendarTime(struct tm t)
{
	t.tm_mon -= 1; 
	Time_SetUnixTime(Time_ConvCalendarToUnix(t));
	return;
}

// ================================================================================================================
// void RTC_Configuration(void) RTC配置 
// ================================================================================================================

void RTC_Configuration(void)
{
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	 // 打开电源和后备接口时钟
  PWR_BackupAccessCmd(ENABLE);	                       // 使能对后备寄存器和RTC的访问 
  BKP_DeInit();		                                     // 复位后备寄存器
  RCC_LSEConfig(RCC_LSE_ON);	                         // 32.768K Hz 低速外部谐振器 为实时时钟提供一个低功耗且精准的时钟源 RCC_BDCR-LSEON 
  
  while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)	 // 等待 LSE稳定 
  {ToggleGPIORev(&WDT);}

  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);	             // 设置LSE作为RTC时钟源 
  RCC_RTCCLKCmd(ENABLE);	                             // 使能 RTC时钟 
  RTC_WaitForSynchro();			                           // 时钟寄存器同步完成  
  RTC_WaitForLastTask();			                         // 等待对RTC寄存器写操作完成  
  RTC_WaitForLastTask();		                           // 等待对RTC寄存器写操作完成
  RTC_SetPrescaler(32767);                             // RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) 预分频器 设置为1S的周期
  RTC_WaitForLastTask();		                           // 等待对RTC寄存器写操作完成
}


// ================================================================================
//  void Time_Adjust(void)
//  时钟校准 
//  ==============================================================================
void Time_Adjust(struct tm CalTime)
{
	RTC_Configuration();	                // 配置RTC 
	PWR_BackupAccessCmd(ENABLE);	        // 使能对后备寄存器和RTC的访问 
  RTC_WaitForLastTask();				        // 等待上次写寄存器操作完成
	Time_SetCalendarTime(CalTime);
  RTC_WaitForLastTask();			          // 等待写寄存器操作完成
	PWR_BackupAccessCmd(DISABLE);	        // 禁能对后备寄存器和RTC的访问 

	PWR_BackupAccessCmd(ENABLE);	        // 使能对后备寄存器和RTC的访问 
  BKP_WriteBackupRegister(BKP_DR1, BKP_RTC_CFG_FLAG);	        // 写入操作标志
	PWR_BackupAccessCmd(DISABLE);	        // 使能对后备寄存器和RTC的访问 

}

#include"appstart.h"
void sysRtcHwInit(void)
{	 
	struct tm CalTime;
	#if OS_CRITICAL_METHOD == 3
	OS_CPU_SR  cpu_sr;
	#endif  
 BKP_ReadBackupRegister(BKP_DR1);
	// 读取备份寄存器偏移量为4的位置 - 备份数据寄存器不正确或者没有被用过
	if (BKP_ReadBackupRegister(BKP_DR1) != BKP_RTC_CFG_FLAG)			
	{		
		RTC_Configuration();	           // 配置RTC 

		CalTime.tm_year = 2017;
		CalTime.tm_mon  = 2;
		CalTime.tm_mday = 14;
		CalTime.tm_hour = 11;
		CalTime.tm_min  = 11;
		CalTime.tm_sec  = 11;
		OS_ENTER_CRITICAL();
		Time_Adjust(CalTime);	
		OS_EXIT_CRITICAL();
											  // 配置时分秒 -  初始化一个默认时间 
		PWR_BackupAccessCmd(ENABLE);	        // 使能对后备寄存器和RTC的访问 
		BKP_WriteBackupRegister(BKP_DR1, BKP_RTC_CFG_FLAG);	        // 写入操作标志
		PWR_BackupAccessCmd(DISABLE);	        // 使能对后备寄存器和RTC的访问 
	}
	else		                                                      // 备份寄存器已经被操作过 
	{		
		RTC_WaitForSynchro();			                                  // 等待RTC寄存器同步 
		RTC_WaitForLastTask();		                                  // 等待寄存器写操作完成 
	}
	
	//printf("flag = %d ,,,temp = %x !!!\n\r",flag,temp);
}
