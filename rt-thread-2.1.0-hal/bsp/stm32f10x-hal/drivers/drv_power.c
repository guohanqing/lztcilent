
//#include "usart/bsp_debug_usart.h"
#include "drv_power.h"
//#include "stm32f1xx_hal.h"
#include <rtthread.h>
#include "usart_wifi_esp8266.h"
//#include "app.h"
#include "app.h"
#define SW_EVENT_SLEEP      0x01
#define SW_EVENT_DEL_TIME   0x02
#define SW_EVENT_DEV_OFF    0x04

#define SW_EVENT SW_EVENT_SLEEP|SW_EVENT_DEL_TIME|SW_EVENT_DEV_OFF

//extern enum LINK_STATUS wifi_link_status;
enum device_stae   device_status;
static rt_size_t  switch_time_out_count =0;
void power_switch_init(void)
{
/*Configure GPIO pin : CTP_INT_Pin */
    GPIO_InitTypeDef GPIO_InitStruct;
    
	GPIO_InitStruct.Pin = GPIO_PIN_1;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
		//GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/* EXTI interrupt init*/
	HAL_NVIC_SetPriority(EXTI1_IRQn, 5, 1);
    HAL_NVIC_EnableIRQ(EXTI1_IRQn);
}
void EXTI1_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
}

static rt_timer_t switch_time_out = RT_NULL;
static rt_event_t switch_event;



//void beep_on(rt_uint8_t count,rt_size_t time)

void power_switch_timeout_callbak(void* parameter)
{
    if(PIN_LOW == POWER_SWITCH_PIN_READ)
        switch_time_out_count++;
    else
    {
        
		if(1 < switch_time_out_count <11)
			rt_event_send(switch_event,SW_EVENT_SLEEP);
		else
			rt_event_send(switch_event,SW_EVENT_DEL_TIME);
        if(RT_NULL != switch_time_out)
        {
            //rt_timer_delete(switch_time_out);
            //rt_free(switch_time_out);
        }
        switch_time_out_count = 0;
    }
    if(30 < switch_time_out_count)
    {
        device_status = SYS_STATUS_OFF;
        SYS_POWER_OFF;
    }
}
void switch_watch_entry(void* parameter)
{
    rt_err_t result;
    rt_uint32_t event;
    while(1) 
    {
        result = rt_event_recv(switch_event, SW_EVENT,RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR, 2*RT_TICK_PER_SECOND, &event);
        if(RT_EOK == result)
        {
            if(event&SW_EVENT_SLEEP)
            {
                #if 0
                if(device_status == SYS_STATUS_SLEEP)
				{
					rt_device_t wifi,fp,lcd;
					//wifi = rt_device_find("wifi_dev");
					fp = rt_device_find("finger_dev"); 
					lcd = rt_device_find("lcd"); 					
					/*if(RT_NULL != wifi)
					{
						wifi->open(wifi,RT_NULL);
						wifi_jap_status();
					}*/
					if(RT_NULL != lcd)
					{
						lcd->open(lcd,RT_NULL);
					}
					
					if(RT_NULL != fp)
					{
						fp->open(fp,RT_NULL);
					}
					device_status = SYS_STATUS_ON;
				}
				else if(device_status == SYS_STATUS_ON)
				{
					rt_device_t wifi,fp,lcd;
					//wifi = rt_device_find("wifi_dev");
					fp = rt_device_find("finger_dev"); 
					lcd = rt_device_find("lcd");
					run_to_win(ID_TASK_MAIN); 					
					/*if(RT_NULL != wifi)
					{
						wifi->close(wifi);
					}*/
					//wifi_link_status = WIFI_LINK_JAP_FAILD;
					if(RT_NULL != lcd)
					{
						lcd->close(lcd);
					}
					if(RT_NULL != fp)
					{
						fp->close(fp);
					}
					device_status = SYS_STATUS_SLEEP;
				}
                if(RT_NULL != switch_time_out)
                {
                    rt_timer_delete(switch_time_out);
                    switch_time_out = RT_NULL;
                    return;
                } 
                #endif
            }
            if(event&SW_EVENT_DEL_TIME)
            {
                if(RT_NULL != switch_time_out)
                {
                    rt_timer_delete(switch_time_out);
                    switch_time_out = RT_NULL;
                    return;
                } 
            }
            if(event&SW_EVENT_DEV_OFF)
            {
                ;
            }
            
        }
    }      
}

void switch_watch(void)
{
	/* 创建wifi watch线程*/
    rt_thread_t thread;	
  
	thread = rt_thread_find("switch");
	if(thread == RT_NULL)
	{
		thread = rt_thread_create("switch",
										switch_watch_entry, RT_NULL,
										512, 21, 15);
      
        if (thread != RT_NULL)
        {
            rt_thread_startup(thread);
        }
        
    }
}


void EXTI1_INTERPUT_cb(void)
{
    if(SYS_STATUS_ON == device_status)
    {
       
        switch_time_out_count = 0;
        switch_event = rt_event_create("rev_ev", RT_IPC_FLAG_FIFO);
        switch_time_out = rt_timer_create("switch_timer",power_switch_timeout_callbak,RT_NULL,RT_TICK_PER_SECOND/10,RT_TIMER_FLAG_PERIODIC);
        if(RT_NULL != switch_time_out)
            rt_timer_start(switch_time_out);
        switch_watch();
    }
}
/******************* (C) COPYRIGHT 2015-2020 硬石嵌入式开发团队 *****END OF FILE****/
