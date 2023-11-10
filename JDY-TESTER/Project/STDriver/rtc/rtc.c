
#include "rtc.h"
#include "stm32f10x_bkp.h"
#include "stm32f10x_rtc.h"

#include "gpio.h"

/*******************************************************************************
* ���ļ�ʵ�ֻ���RTC�����ڹ��ܣ��ṩ�����յĶ�д��������ANSI-C��time.h��
* ANSI-C�ı�׼���У��ṩ�����ֱ�ʾʱ�������  �ͣ�
* time_t:   	UNIXʱ�������1970-1-1��ĳʱ�侭����������
* 	typedef unsigned int time_t;
* 
* struct tm:	Calendar��ʽ����������ʽ��
*   tm�ṹ���£�
*   struct tm {
*   	int tm_sec;   // �� seconds after the minute, 0 to 60
*   					 (0 - 60 allows for the occasional leap second)
*   	int tm_min;   // �� minutes after the hour, 0 to 59
*		int tm_hour;  // ʱ hours since midnight, 0 to 23
*		int tm_mday;  // �� day of the month, 1 to 31
*		int tm_mon;   // �� months since January, 0 to 11
*		int tm_year;  // �� years since 1900
*		int tm_wday;  // ���� days since Sunday, 0 to 6
*		int tm_yday;  // ��Ԫ��������� days since January 1, 0 to 365
* 		int tm_isdst; // ����ʱ����Daylight Savings Time flag
* 		...
* 	}
* 	����wday��yday�����Զ����������ֱ�Ӷ�ȡ
* 	mon��ȡֵΪ0-11
*	***ע��***��
*	tm_year:��time.h���ж���Ϊ1900�������ݣ���2008��Ӧ��ʾΪ2008-1900=108
* 	���ֱ�ʾ�������û���˵����ʮ���Ѻã�����ʵ�нϴ���졣
* 	�����ڱ��ļ��У����������ֲ��졣
* 	���ⲿ���ñ��ļ��ĺ���ʱ��tm�ṹ�����͵����ڣ�tm_year��Ϊ2008
* 	ע�⣺��Ҫ����ϵͳ��time.c�еĺ�������Ҫ���н�tm_year-=1900
*******************************************************************************/

/*******************************************************************************
* Function Name  : Time_ConvUnixToCalendar(time_t t)
* Description    : RTCʱ�� -> ����ʱ��
* Input 		 : u32 t  ��ǰRTCʱ��
* Output		 : None
* Return		 : struct tm
*******************************************************************************/
struct tm Time_ConvUnixToCalendar(time_t t)
{
	struct tm *t_tm;
	t_tm = localtime(&t);
	t_tm->tm_year += 1900;	//localtimeת�������tm_year�����ֵ����Ҫת�ɾ���ֵ
	return *t_tm;
}

/*******************************************************************************
* Function Name  : Time_ConvCalendarToUnix(struct tm t)
* Description    : ת��calendarʱ�ӵ�RTCʱ�� 
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
* Description    : ��ȡRTCʱ��
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
* Description    : ��ȡ����ʱ�䣨struct tm��
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
* Description    : д��RTCʱ��
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
* Description    : ��������Calendar��ʽʱ��д��RTC
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
// void RTC_Configuration(void) RTC���� 
// ================================================================================================================

void RTC_Configuration(void)
{
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	 // �򿪵�Դ�ͺ󱸽ӿ�ʱ��
  PWR_BackupAccessCmd(ENABLE);	                       // ʹ�ܶԺ󱸼Ĵ�����RTC�ķ��� 
  BKP_DeInit();		                                     // ��λ�󱸼Ĵ���
  RCC_LSEConfig(RCC_LSE_ON);	                         // 32.768K Hz �����ⲿг���� Ϊʵʱʱ���ṩһ���͹����Ҿ�׼��ʱ��Դ RCC_BDCR-LSEON 
  
  while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)	 // �ȴ� LSE�ȶ� 
  {ToggleGPIORev(&WDT);}

  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);	             // ����LSE��ΪRTCʱ��Դ 
  RCC_RTCCLKCmd(ENABLE);	                             // ʹ�� RTCʱ�� 
  RTC_WaitForSynchro();			                           // ʱ�ӼĴ���ͬ�����  
  RTC_WaitForLastTask();			                         // �ȴ���RTC�Ĵ���д�������  
  RTC_WaitForLastTask();		                           // �ȴ���RTC�Ĵ���д�������
  RTC_SetPrescaler(32767);                             // RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) Ԥ��Ƶ�� ����Ϊ1S������
  RTC_WaitForLastTask();		                           // �ȴ���RTC�Ĵ���д�������
}


// ================================================================================
//  void Time_Adjust(void)
//  ʱ��У׼ 
//  ==============================================================================
void Time_Adjust(struct tm CalTime)
{
	RTC_Configuration();	                // ����RTC 
	PWR_BackupAccessCmd(ENABLE);	        // ʹ�ܶԺ󱸼Ĵ�����RTC�ķ��� 
  RTC_WaitForLastTask();				        // �ȴ��ϴ�д�Ĵ����������
	Time_SetCalendarTime(CalTime);
  RTC_WaitForLastTask();			          // �ȴ�д�Ĵ����������
	PWR_BackupAccessCmd(DISABLE);	        // ���ܶԺ󱸼Ĵ�����RTC�ķ��� 

	PWR_BackupAccessCmd(ENABLE);	        // ʹ�ܶԺ󱸼Ĵ�����RTC�ķ��� 
  BKP_WriteBackupRegister(BKP_DR1, BKP_RTC_CFG_FLAG);	        // д�������־
	PWR_BackupAccessCmd(DISABLE);	        // ʹ�ܶԺ󱸼Ĵ�����RTC�ķ��� 

}

#include"appstart.h"
void sysRtcHwInit(void)
{	 
	struct tm CalTime;
	#if OS_CRITICAL_METHOD == 3
	OS_CPU_SR  cpu_sr;
	#endif  
 BKP_ReadBackupRegister(BKP_DR1);
	// ��ȡ���ݼĴ���ƫ����Ϊ4��λ�� - �������ݼĴ�������ȷ����û�б��ù�
	if (BKP_ReadBackupRegister(BKP_DR1) != BKP_RTC_CFG_FLAG)			
	{		
		RTC_Configuration();	           // ����RTC 

		CalTime.tm_year = 2017;
		CalTime.tm_mon  = 2;
		CalTime.tm_mday = 14;
		CalTime.tm_hour = 11;
		CalTime.tm_min  = 11;
		CalTime.tm_sec  = 11;
		OS_ENTER_CRITICAL();
		Time_Adjust(CalTime);	
		OS_EXIT_CRITICAL();
											  // ����ʱ���� -  ��ʼ��һ��Ĭ��ʱ�� 
		PWR_BackupAccessCmd(ENABLE);	        // ʹ�ܶԺ󱸼Ĵ�����RTC�ķ��� 
		BKP_WriteBackupRegister(BKP_DR1, BKP_RTC_CFG_FLAG);	        // д�������־
		PWR_BackupAccessCmd(DISABLE);	        // ʹ�ܶԺ󱸼Ĵ�����RTC�ķ��� 
	}
	else		                                                      // ���ݼĴ����Ѿ��������� 
	{		
		RTC_WaitForSynchro();			                                  // �ȴ�RTC�Ĵ���ͬ�� 
		RTC_WaitForLastTask();		                                  // �ȴ��Ĵ���д������� 
	}
	
	//printf("flag = %d ,,,temp = %x !!!\n\r",flag,temp);
}
