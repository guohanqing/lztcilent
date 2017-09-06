
//#include "usart/bsp_debug_usart.h"
#include "drv_rtc.h"
//#include "stm32f1xx_hal.h"
#include <rtthread.h>
#include "app.h"
RTC_HandleTypeDef hrtc;
extern struct rt_messagequeue wmq;

 void MX_RTC_Init(void)
{

  //RTC_TimeTypeDef sTime;
 // RTC_DateTypeDef DateToUpdate;
  __HAL_RCC_PWR_CLK_ENABLE();
  HAL_PWR_EnableBkUpAccess();

    /**Initialize RTC Only 
    */
  hrtc.Instance = RTC;
  hrtc.Init.AsynchPrediv = RTC_AUTO_1_SECOND;
  hrtc.Init.OutPut = RTC_OUTPUTSOURCE_NONE;
  HAL_RTC_Init(&hrtc) ;
 

    /**Initialize RTC and set the Time and Date 
    */
  if(HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1) != 0x32F2)
  {
      /*sTime.Hours = 0x1;
      sTime.Minutes = 0x0;
      sTime.Seconds = 0x1;

      if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
      {
       ;
      }

      DateToUpdate.WeekDay = RTC_WEEKDAY_WEDNESDAY;
      DateToUpdate.Month = RTC_MONTH_AUGUST;
      DateToUpdate.Date = 0x30;
      DateToUpdate.Year = 0x17;

      if (HAL_RTC_SetDate(&hrtc, &DateToUpdate, RTC_FORMAT_BCD) != HAL_OK)
      {
        ;
      }*/

        HAL_RTCEx_BKUPWrite(&hrtc,RTC_BKP_DR1,0x32F2);
  
  }
  __HAL_RCC_BKP_CLK_ENABLE();
  __HAL_RCC_RTC_ENABLE();
  HAL_RTCEx_SetSecond_IT(&hrtc);

}
void HAL_RTCEx_RTCEventErrorCallback(RTC_HandleTypeDef *hrtcx)
{
    RTC_TimeTypeDef recv_time;
    RTC_DateTypeDef recv_date;
    union_date_time_t sendtime;
    rt_uint8_t buf[20] ={0};
    HAL_RTC_GetTime(hrtcx, &recv_time, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(hrtcx,&recv_date, RTC_FORMAT_BIN);
    
    buf[0] =  ID_TASK_SYSNC_TIME;
    buf[1] = RCV_DEVICE;
    sendtime.time.year   =  recv_date.Year;
    sendtime.time.month  =  recv_date.Month;
    sendtime.time.day    =  recv_date.Date;
    sendtime.time.hour   =  recv_time.Hours;
    sendtime.time.minute =  recv_time.Minutes;
    sendtime.time.second =  recv_time.Seconds;
    rt_memcpy(buf+2,sendtime.buf,6);
    rt_mq_send(&wmq,buf,8);   
}

static rt_err_t drv_rtc_init(rt_device_t dev)
{
    MX_RTC_Init();
    return RT_EOK;
}

static rt_err_t drv_rtc_open(rt_device_t dev, rt_uint16_t oflag)
{
    
    return RT_EOK;
}

static rt_err_t drv_rtc_close(rt_device_t dev)
{
    return RT_EOK;
}

static rt_err_t drv_rtc_control(rt_device_t dev, rt_uint8_t cmd, void *args)
{
    RT_ASSERT(dev != RT_NULL);
    switch(cmd)
    {
        case RT_DEVICE_CTRL_RTC_SET_TIME:
        {
              RTC_TimeTypeDef sTime;
              RTC_DateTypeDef DateToUpdate;
              struct tm *time1 = (struct tm*)args;
            
               
               /*设置日期*/ 
              DateToUpdate.WeekDay =  time1->tm_wday;
              DateToUpdate.Month = time1->tm_mon;
              DateToUpdate.Date = time1->tm_mday;
              DateToUpdate.Year = time1->tm_year;  
              HAL_RTC_SetDate(&hrtc,&DateToUpdate,RTC_FORMAT_BIN);
              
              /*设置时间*/
              sTime.Hours = time1->tm_hour;
              sTime.Minutes = time1->tm_min;
              sTime.Seconds = time1->tm_sec;
              HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN); 
        }
        break;
		case RT_DEVICE_CTRL_RTC_GET_TIME:
		{
              RTC_TimeTypeDef sTime;
              RTC_DateTypeDef DateToUpdate;
              struct tm *time1 = (struct tm*)args;
            
               
               /*获取日期*/ 
			  HAL_RTC_GetDate(&hrtc,&DateToUpdate,RTC_FORMAT_BIN);
              time1->tm_wday  = DateToUpdate.WeekDay;
			  time1->tm_mon   = DateToUpdate.Month;
              time1->tm_mday  = DateToUpdate.Date;
              time1->tm_year  = DateToUpdate.Year;  
              
              
              /*获取时间*/
			  HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
               time1->tm_hour =sTime.Hours;
               time1->tm_min =sTime.Minutes;
               time1->tm_sec =sTime.Seconds;
               
        }	
		break;
        default:
        break;
    }

    return RT_EOK;
}




rt_err_t rt_hw_rtc_register(rt_device_t rtc,
                               const char              *name,
                               rt_uint32_t              flag)
{
    struct rt_device *device;
    RT_ASSERT(rtc != RT_NULL);
    device = rtc;
    device->type        = RT_Device_Class_Char;
    device->rx_indicate = RT_NULL;
    device->tx_complete = RT_NULL;
    device->init        = drv_rtc_init;
    device->open        = drv_rtc_open;
    device->close       = drv_rtc_close;
    device->read        = RT_NULL;
    device->write       = RT_NULL;
    device->control     = drv_rtc_control;
    device->user_data   = RT_NULL;
    /* register a character device */
    return rt_device_register(device, name, flag);
}

static struct rt_device _rtc_clock_device;
int rt_hw_rtc_clock_init(void)
{  
	 //ESP8266_CS_Init();
    MX_RTC_Init();
	rt_hw_rtc_register(&_rtc_clock_device,"rtc",RT_DEVICE_FLAG_RDWR);
	
  return RT_TRUE;
}
INIT_DEVICE_EXPORT(rt_hw_rtc_clock_init);


/******************* (C) COPYRIGHT 2015-2020 ӲʯǶ��ʽ�����Ŷ� *****END OF FILE****/
