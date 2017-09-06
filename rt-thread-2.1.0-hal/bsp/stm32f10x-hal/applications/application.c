/*
 * File      : application.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      the first version
 * 2013-07-12     aozima       update for auto initial.
 */

/**
 * @addtogroup STM32
 */
/*@{*/

#include <board.h>
#include <rtthread.h>

#ifdef  RT_USING_COMPONENTS_INIT
#include <components.h>
#endif  /* RT_USING_COMPONENTS_INIT */

#ifdef RT_USING_DFS
/* dfs filesystem:ELM filesystem init */
#include <dfs_elm.h>
/* dfs Filesystem APIs */
#include <dfs_fs.h>
#endif

#ifdef RT_USING_RTGUI
#include <rtgui/rtgui.h>
#include <rtgui/rtgui_server.h>
#include <rtgui/rtgui_system.h>
#include <rtgui/driver.h>
#include <rtgui/calibration.h>
#endif

#include "drv_led.h"
#include "drv_gpio.h"
//#include "drv_hwbutton.h"
//#include "light_wave_curer.h"
#include "drv_lcd_ili9341.h"
#include "drv_iic_touch_ft5216.h"
#include "drv_stm32f10x_spi.h"
#include "spi_flash_w25qxx.h"
#include "drv_uart_finger.h"
#include "usart_wifi_esp8266.h"
#include "drv_power.h"
#include "GUI.h"
#include "app.h"
//#include "bsp_EEPROM.h"

ALIGN(RT_ALIGN_SIZE)
static rt_uint8_t led_stack[1024*2];
static struct rt_thread led_thread;
//extern GUI_CONST_STORAGE GUI_BITMAP bmlogo;
extern int rt_hw_lcd_init(void);
//extern uint8_t ft5216_rcv[];
uint16_t lcdid;
uint16_t ft5216id;
static void led_thread_entry(void* parameter)
{
	//uint8_t i = 1;
    //unsigned int count=0;
	
    rt_led_hw_init();
    
    //rt_pin_mode(PB1, PIN_MODE_OUTPUT);
	  //rt_pin_mode(PC8,PIN_MODE_OUTPUT);//pc8
		//rt_pin_mode(PC9,PIN_MODE_OUTPUT);//pc9
		//rt_pin_mode(PC7,PIN_MODE_INPUT);//pc7
		//rt_pin_write(PC9,PIN_HIGH);
    //rt_pin_mode(PC14, PIN_MODE_OUTPUT);
    //rt_pin_mode(PC15, PIN_MODE_OUTPUT);
    //rt_pin_mode(PB3, PIN_MODE_OUTPUT);
    //rt_pin_mode(PB4, PIN_MODE_OUTPUT);
    //hwadc();
#if 0
	#ifdef RT_USING_LCD_ILI9341
	rt_hw_lcd_init();
	lcdid = ili9341_read_id();
	//if(0x9341 == lcdid)
	//rt_hw_lcd_init();
	ili9341_screen_clear(RED);
	//ili9341_DrawBitmap(0,0 ,bmwelcome.pData);
	//ft5216id = ft5216_ReadID();
	//CTP_IO_Read(0,ft5216_rcv,8); 
	//GUI_Init();
	//GUI_DrawBitmap(&bmlogo,70,30);
	//LCD_Clear();
	 //ili9341_DrawLine(0,8,100,108,GREEN);
	//LCD_Fill(0,0,100,50,RED);
	//Draw_Circle(100,200,50,BRED);	
	
#endif
	#endif	

    while (1)
    { 
        
        #if 0
        //WIFI_LINK_CONNECT_SEVER_OK != wifi_link_status
       // send_frame_package(CMD_SEND_TICK,1,"22");
			//char buf1[6] = {0};
			//char buf1[6] = {0};
			//char buf1[6] = {0};
		//ili9341_screen_clear(RED);
				{ 
					
                    if(1 == i)
                        {
                            rt_device_t _wifi_device = rt_device_find("wifi_dev"); 
                            if(RT_NULL != _wifi_device)
                            {
                                union link_sever_ip ipx;
                                memset(ipx.ip_buf,0,sizeof(ipx.ip_buf));
                                strcpy(ipx.ip.IP1,"192");
                                strcpy(ipx.ip.IP2,"168");
                                strcpy(ipx.ip.IP3,"2");
                                strcpy(ipx.ip.IP4,"102");
                                strcpy(ipx.ip.PORT,"8080");
                                _wifi_device->open(_wifi_device,0);
                                _wifi_device->control(_wifi_device,WIFI_JAP,"lzt02,lzt123456");
                                _wifi_device->control(_wifi_device,WIFI_JAP_STATUS,RT_NULL);
                                _wifi_device->control(_wifi_device,WIFI_CONNECT_SEVER,(void *)ipx.ip_buf);
                                //_wifi_device->control(_wifi_device,WIFI_CONNECT_SEVER,(void *)ipx.ip_buf);
                                
                                i++;
                            }
                             rt_device_t fp_device = rt_device_find("finger_dev"); 
                            if(RT_NULL != fp_device)
                            {
       
                                fp_device->control(fp_device,FP_REG,RT_NULL);
                               // _wifi_device->control(_wifi_device,WIFI_CONNECT_SEVER,(void *)ipx.ip_buf);
                                //_wifi_device->control(_wifi_device,WIFI_CONNECT_SEVER,(void *)ipx.ip_buf);
                                
                                i++;
                            }
                      }
                        
				}
                #endif
        /* led1 on */
#ifndef RT_USING_FINSH
        rt_kprintf("led on, count : %d\r\n",count);
#endif
        //count++;
        //HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET);
        rt_led_on();			
        rt_thread_delay( RT_TICK_PER_SECOND/2 ); /* sleep 0.5 second and switch to other thread */

		    //rt_pin_toggle(PB1);
				
        //rt_pin_toggle(PC14);
        //rt_pin_toggle(PC15);
        //rt_pin_toggle(PB3);
        //rt_pin_toggle(PB4);
        
        /* led1 off */
#ifndef RT_USING_FINSH
        rt_kprintf("led off\r\n");
#endif
        
        //HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);
        rt_led_off();
        rt_thread_delay( RT_TICK_PER_SECOND*2 );       
    }
}


void start_image_thread_entry(void* parameter)
{
	
	#ifdef RT_USING_PIN    
	   stm32_hw_pin_init();
		#if 1
		rt_pin_mode(SYS_POWER_PIN,PIN_MODE_OUTPUT); 
		rt_pin_mode(DEVICE_POWER_PIN,PIN_MODE_OUTPUT);
		rt_pin_mode(SWITCH_POWER_PIN_IN,PIN_MODE_INPUT);
        rt_pin_mode(BEEP_PIN,PIN_MODE_OUTPUT);
		DEVICE_POWER_ON;
        SYS_POWER_ON;
        device_status = SYS_STATUS_ON;
        BEEP_ON;
        rt_thread_delay( RT_TICK_PER_SECOND/2);
        BEEP_OFF;
		#endif
	#endif
    
	rt_hw_lcd_init();
    //rt_thread_delay( RT_TICK_PER_SECOND/10);
    GUI_Init();
    GUI_SetBkColor(GUI_WHITE);
    GUI_Clear();
    GUI_UC_SetEncodeUTF8();
    GUI_SetFont(&GUI_Font_kai24);
    GUI_SetColor(GUI_BLUE);
    GUI_DispStringAt("恋指时代",72,180);
	GUI_SetFont(&GUI_Font_song16);
    GUI_SetColor(GUI_BLACK);
	GUI_DispStringAt("河南恋指团科技有限公司",38,230);
    GUI_SetFont(GUI_FONT_20_ASCII);
    GUI_SetColor(GUI_BLACK);
    GUI_DispStringAt("www.51lzt.com",60,260);
    GUI_DrawBitmap(&bmwelcome,0,0); 
    while (1)
    { 
        rt_thread_delay( RT_TICK_PER_SECOND*3);
       
        return;
    }
}

#ifdef RT_USING_RTGUI
rt_bool_t cali_setup(void)
{
    rt_kprintf("cali setup entered\n");
    return RT_FALSE;
}

void cali_store(struct calibration_data *data)
{
    rt_kprintf("cali finished (%d, %d), (%d, %d)\n",
               data->min_x,
               data->max_x,
               data->min_y,
               data->max_y);
}
#endif /* RT_USING_RTGUI */

void rt_init_thread_entry(void* parameter)
{ 

#ifdef RT_USING_COMPONENTS_INIT
    /* initialization RT-Thread Components */
    rt_components_init();
#endif

//#endif /* RT_USING_PIN */ 

	
 #ifdef RT_USING_SPI_FLASH
	  w25qxx_init("sd0", "spiflash");
	
#endif  
    
    /* Filesystem Initialization */
#if defined(RT_USING_DFS) && defined(RT_USING_DFS_ELMFAT)
	/* initialize the device file system */
		dfs_init();

		/* initialize the elm chan FatFS file system*/
		elm_init();
		
    /* mount sd card fat partition 1 as root directory */
    //dfs_mkfs("elm","sd0");
    if (dfs_mount("sd0", "/", "elm", 0, 0) == 0)
    {
        rt_kprintf("File System initialized!\n");
    }
    else
	{
		rt_kprintf("File System initialzation failed!\n");
	}
		
#endif  /* RT_USING_DFS */
	//emwin_system_init();
    power_switch_init();

#ifdef RT_USING_RTGUI
    {
        extern void rt_hw_lcd_init();
        extern void rtgui_touch_hw_init(void);

        rt_device_t lcd;

        /* init lcd */
        rt_hw_lcd_init();

        /* init touch panel */
        rtgui_touch_hw_init();

        /* find lcd device */
        lcd = rt_device_find("lcd");

        /* set lcd device as rtgui graphic driver */
        rtgui_graphic_set_device(lcd);

#ifndef RT_USING_COMPONENTS_INIT
        /* init rtgui system server */
        rtgui_system_server_init();
#endif

        calibration_set_restore(cali_setup);
        calibration_set_after(cali_store);
        calibration_init();
    }
#endif /* #ifdef RT_USING_RTGUI */
}

int rt_application_init(void)
{
    rt_thread_t init_thread;
    rt_thread_t start_thread;
    rt_err_t result;
    /* init led thread */
    result = rt_thread_init(&led_thread,
                            "led",
                            led_thread_entry,
                            RT_NULL,
                            (rt_uint8_t*)&led_stack[0],
                            sizeof(led_stack),
                            20,
                            5);
    if (result == RT_EOK)
    {
        rt_thread_startup(&led_thread);
    } 
    start_thread =  rt_thread_create("begin_image",
                             start_image_thread_entry,RT_NULL,1536, 8, 21);
    if (result == RT_EOK)
    {
        rt_thread_startup(start_thread);
    }
#ifdef RT_USING_HWADC    
    /* init lwc process thread */
    /*result = rt_thread_init(&lwc_adc_thread,
                            "lwcadc",
                            lwc_adc_thread_entry,
                            RT_NULL,
                            (rt_uint8_t*)&lwc_adc_stack[0],
                            sizeof(lwc_adc_stack),
                            25,
                            5);
    if (result == RT_EOK)
    {
        rt_thread_startup(&lwc_adc_thread);
    }*/  
#endif   

#if (RT_THREAD_PRIORITY_MAX == 32)
    init_thread = rt_thread_create("init",
                                   rt_init_thread_entry, RT_NULL,
                                   1024, 8, 20);
#else
    init_thread = rt_thread_create("init",
                                   rt_init_thread_entry, RT_NULL,
                                   2048, 80, 20);
#endif

    if (init_thread != RT_NULL)
        rt_thread_startup(init_thread);

    return 0;
}

/*@}*/
