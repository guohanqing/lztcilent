#ifndef __DRV_POWER_H__
#define __DRV_POWER_H__

/* 包含头文件 ----------------------------------------------------------------*/
#include <rthw.h>
#include <rtdevice.h>
#include <board.h>
#include <drv_gpio.h>
#include <drivers/pin.h>

//#include "rtc/bsp_calendar.h"
//#include "time.h"

/* 类型定义 ------------------------------------------------------------------*/
/* 宏定义 --------------------------------------------------------------------*/
/* 扩展变量 ------------------------------------------------------------------*/

#define DEVICE_POWER_PIN  91 /*PB5*/
#define SYS_POWER_PIN     63 /*PC6*/
#define SWITCH_POWER_PIN_IN  24 /*PA1*/
#define BEEP_PIN  	83 /*PD2*/

#define DEVICE_POWER_ON  rt_pin_write(DEVICE_POWER_PIN,PIN_HIGH)
#define DEVICE_POWER_OFF  rt_pin_write(DEVICE_POWER_PIN,PIN_LOW)

#define SYS_POWER_ON  rt_pin_write(SYS_POWER_PIN,PIN_HIGH)
#define SYS_POWER_OFF  rt_pin_write(SYS_POWER_PIN,PIN_LOW)

#define BEEP_ON  rt_pin_write(BEEP_PIN,PIN_HIGH)
#define BEEP_OFF  rt_pin_write(BEEP_PIN,PIN_LOW)

#define POWER_SWITCH_PIN_READ rt_pin_read(SWITCH_POWER_PIN_IN)
extern enum device_stae   device_status;

enum device_stae
{
    SYS_STATUS_ON,
	SYS_STATUS_SLEEP,
    SYS_STATUS_OFF
};

void power_switch_init(void);
    

/* 函数声明 ------------------------------------------------------------------*/



#endif  /* __BSP_RTC_H__ */

/******************* (C) COPYRIGHT 2015-2020 硬石嵌入式开发团队 *****END OF FILE****/
