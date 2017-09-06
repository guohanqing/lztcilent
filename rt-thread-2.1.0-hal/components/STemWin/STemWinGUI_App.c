#include <rthw.h>
#include "GUI.h"
#include "WM.h"
#include "app.h"

//#include "GUIDEMO.h"
//#include "MainTask.h"
//#include "drv_lcd_st7735.h"
struct rt_thread emwin_thread;
char emWin_thread_stack[RT_APP_EMWIN_MEMORY];
struct rt_thread background_thread;
//struct rt_thread updater_thread;
//char updater_thread_stack[RT_APP_UPDATER_MEMORY];
char background_thread_stack[RT_APP_BACKGROUND_MEMORY];
struct rt_messagequeue wmq;
static char wmsg_pool[RT_APP_MSG_LEN];
struct rt_event wevent;
struct rt_timer app_time;
struct rt_mutex wifi_send_mutex;
void emWin_thread_entry(void *parameter);
void background_thread_entry(void *parameter);
//void updater_thread_entry(void *parameter);


int emwin_system_init(void)
{
	rt_err_t result;
	//rt_thread_t emwin_thread1;
	
	rt_device_t device = rt_device_find("lcd");
	if(device == RT_NULL)
	{
		rt_kprintf("no graphic device in the system. \n");
		
	}
	else
	{
		rt_mq_init(&wmq, "mqt",
					&wmsg_pool[0], /* 内存池指向msg_pool */
					sizeof(win_msg_t), /* 每个消息的大小 */
					sizeof(wmsg_pool), /* 内存池的大小是msg_pool的大小*/
					RT_IPC_FLAG_FIFO); /* 如果有多个线程等待，按照FIFO的方法分配消息*/
		rt_event_init(&wevent, "wevent", RT_IPC_FLAG_FIFO);			
			
		result = rt_thread_init(&emwin_thread,
								"emwin",
								emWin_thread_entry, RT_NULL,
								&emWin_thread_stack[0], 
								sizeof(emWin_thread_stack),
								13, 100);
				 
		if (result == RT_EOK)
			rt_thread_startup(&emwin_thread);
		
		result = rt_thread_init(&background_thread,
								"background",
								background_thread_entry, RT_NULL,
								&background_thread_stack[0], 
								sizeof(background_thread_stack),
								14, 100);
		if (result == RT_EOK)
			rt_thread_startup(&background_thread);
        /*result = rt_thread_init(&updater_thread,
								"updater",
								updater_thread_entry, RT_NULL,
								&updater_thread_stack[0], 
								sizeof(updater_thread_stack),
								17, 20);
		if (result == RT_EOK)
			rt_thread_startup(&updater_thread);*/
        rt_mutex_init(&wifi_send_mutex,"wifi_mutex",RT_IPC_FLAG_FIFO);
        rt_timer_init(&app_time,"2stimer",app_timeout_callbak,RT_NULL,2*RT_TICK_PER_SECOND,RT_TIMER_FLAG_PERIODIC);
        rt_timer_start(&app_time);
	}
    return 0;
}
INIT_APP_EXPORT(emwin_system_init);


