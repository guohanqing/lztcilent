/***************************2012-2016, NJUT, Edu.******************************* 

*******************************************************************************/ 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include  <rtthread.h>
#include "drivers/pin.h"
#include "drivers/serial.h"
#include "drv_led.h"
//#include "usart_wifi_esp8266.h"
#include "cvt.h"
#include "app.h"

#ifdef RT_USING_UART_WIFI_ESP8266


/***************************WIFI模块串口接收數據事件******************************/

#define EVENT_WIFI_RECV_DATA      	0x01
#define EVENT_WIFI_START_WATCH      0x02
#define EVENT_WIFI_STOP_WATCH      	0x04
#define EVENT_WIFI_JOIN_AP      		0x08


//#define WIFI_DEBUG_ON

#ifdef WIFI_DEBUG_ON
#define WIFI_DEBUG         rt_kprintf("[ESP-12F] ");rt_kprintf
//#define USART_DEBUG         rt_kprintf("[USART] ");rt_kprintf
#define USART_DEBUG(...)
#else
#define WIFI_DEBUG(...)
#define WSART_DEBUG(...)
#endif /* #ifdef WIFI_DEBUG_ON */

wifi_list_t  wifi_list_AP[WIFI_LIST_AP_MAX];
wifi_list_t  connected_ap;	

static struct rt_mailbox wifi_mail;
static uint32_t wifi_msg_pool[4];
static struct rt_event rev_event;
//rt_bool_t wifi_send(char * str);
//rt_bool_t wifi_jap(void);
rt_bool_t quit_wifi_connect(void);
static rt_uart_extent_device_t _wifi_dev;
enum LINK_STATUS wifi_link_status = WIFI_LINK_JAP_FAILD;
//static char wifi_status[1] = {0};
extern link_ip_t using_sever_ip;
extern const link_ip_t default_sever_ip;
extern struct rt_messagequeue wmq;


rt_uint8_t send_package_check( char *buf,rt_uint16_t len);
void wifiwatch_entry(void* parameter)
{
	rt_err_t result = RT_EOK;
	rt_uint32_t event;
    rt_uint32_t recv_id;
    rt_uint16_t recv_head;
    rt_uint16_t recv_len;
	rt_uint8_t wifi_rx_buffer[512]={0x00};
	rt_size_t  readnum = 0;
    rt_size_t  temp_offset = 0;
    static rt_uint8_t *recv_buf; 
	while(1)
	{
        //wifi_rx_buffer[0] =  0;
        //wifi_rx_buffer[1] = RECV_SERVE;
		result = rt_event_recv(&rev_event,	  
													EVENT_WIFI_RECV_DATA
													|EVENT_WIFI_START_WATCH
													|EVENT_WIFI_STOP_WATCH,
													RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR, 
													5*RT_TICK_PER_SECOND, &event);
		if (result == RT_EOK)
		{
			if (event & EVENT_WIFI_RECV_DATA)
			{
                
                //sever_frame_package_t recv_to_send;
				//rt_thread_delay(RT_TICK_PER_SECOND/20);
				readnum = rt_device_read(_wifi_dev.uart_dev, 0, wifi_rx_buffer+temp_offset+2, 510-temp_offset);
                for(int i = 2;i<readnum+temp_offset+2;i++)
                {
                    if((0xaa == *(wifi_rx_buffer+i))&&(0x55 == *(wifi_rx_buffer+i+1)))
                       recv_buf =  wifi_rx_buffer+i-2;
                }
                if(0xAA == (rt_uint8_t)recv_buf[2])
                {
                    
                    temp_offset+= readnum;
                    recv_buf[0] =  0;
                    recv_buf[1] = RECV_SERVE;
                    recv_head   = ((rt_uint16_t)recv_buf[2]<<8)+(rt_uint16_t)recv_buf[3];
                    recv_len    = ((rt_uint16_t)recv_buf[4]<<8)+(rt_uint16_t)recv_buf[5];
                    recv_id     = ((rt_uint16_t)recv_buf[6]<<16)+((rt_uint16_t)recv_buf[7]<<8)+(rt_uint16_t)recv_buf[8];                    
                   // recv_id = ((rt_uint16_t)wifi_rx_buffer[6]<<16)+((rt_uint16_t)wifi_rx_buffer[7]<<8)+(rt_uint16_t)wifi_rx_buffer[8];
                }       
                if((0xAA55 == recv_head)&&(recv_id == lzt_dev_id))
                {
                   if(temp_offset >= recv_len+4)
                   {
                        rt_mq_send(&wmq,recv_buf,temp_offset+2);
                        rt_memset(wifi_rx_buffer,0x00,sizeof(wifi_rx_buffer));
                        temp_offset = 0;
                        return;
                   }
                }
				
				//WIFI_DEBUG(wifi_rx_buffer);
			}
            if(temp_offset >= 500)
            {
                rt_mq_send(&wmq,recv_buf,503);
                rt_memset(wifi_rx_buffer,0x00,sizeof(wifi_rx_buffer));
                temp_offset = 0;
                return;
        
            }
		}
        else
        {
            temp_offset = 0;
            rt_memset(wifi_rx_buffer,0x00,sizeof(wifi_rx_buffer));
            return;
        }
		rt_thread_delay( RT_TICK_PER_SECOND/2);
	}
}

void wifiwatch(void)
{
	/* 创建wifi watch线程*/
    rt_thread_t thread;	
  
	thread = rt_thread_find("wifiwatch");
	if(thread == RT_NULL)
	{
		thread = rt_thread_create("wifiwatch",
										wifiwatch_entry, RT_NULL,
										1536, 25, 10);
      
        if (thread != RT_NULL)
        {
            rt_thread_startup(thread);
        }
    }
}
    
void wifistopwatch(void)
{
	rt_event_send(&rev_event, EVENT_WIFI_STOP_WATCH);
}

/* 数据到达回调函数,发送事件到wifi_send_data_package */
static rt_err_t wifi_uart_input(rt_device_t dev, rt_size_t size)
{
	rt_event_send(&rev_event, EVENT_WIFI_RECV_DATA);
	return RT_EOK;
}

/*WIFI串口发送和接收*/
rt_bool_t wifi_send_data_package(char *cmd,char *ack,uint16_t waittime, uint8_t retrytime,char *ret)
{
	rt_bool_t res = RT_FALSE; 
	rt_err_t result = RT_EOK;
	rt_uint32_t event;
	rt_uint32_t temp_offset = 0;;
	rt_uint16_t count;
	char wifi_rx_buffer[512]={0x00};
	rt_thread_t thread;	
    rt_size_t read_num;
    //rt_mutex_take(&send_page_mutex,2*RT_TICK_PER_SECOND);
	thread = rt_thread_find("wifiwatch");
	if( thread != RT_NULL)
	{
		rt_thread_delete(thread);
	}
  
	do 
	{
		rt_device_write(_wifi_dev.uart_dev, 0, cmd, rt_strlen(cmd));   
		//count = 3;
		temp_offset = 0;
        rt_memset(wifi_rx_buffer,0x00,sizeof(wifi_rx_buffer));
		for(count = 500;count > 0;)
		{
			result = rt_event_recv(&rev_event, 
														EVENT_WIFI_RECV_DATA
														|EVENT_WIFI_START_WATCH
														|EVENT_WIFI_STOP_WATCH, 														
														RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR, 
														waittime*RT_TICK_PER_SECOND, &event);
			if(result == RT_EOK)
			{
				if (event & EVENT_WIFI_RECV_DATA)
				{
					read_num = rt_device_read(_wifi_dev.uart_dev, 0, wifi_rx_buffer+temp_offset, 512-temp_offset);
					//WIFI_DEBUG(wifi_rx_buffer);
					if((rt_strstr(wifi_rx_buffer,ack))||(rt_strstr(wifi_rx_buffer,"OK")))
					{
                        temp_offset += read_num;
                        rt_device_read(_wifi_dev.uart_dev, 0, wifi_rx_buffer+temp_offset, 512-temp_offset);
						res = RT_TRUE;
						if(RT_NULL != ret)
                            rt_strncpy(ret,wifi_rx_buffer,strlen(wifi_rx_buffer)); 
						count = 0;
					}
					else if(rt_strstr(wifi_rx_buffer,"ERROR"))
					{
                        temp_offset += read_num;
                        rt_device_read(_wifi_dev.uart_dev, 0, wifi_rx_buffer+temp_offset, 512-temp_offset);
						res = RT_FALSE;
						count = 0;
						rt_thread_delay( RT_TICK_PER_SECOND );
					}
					else
					{
						count--;
						temp_offset += read_num;
						if(temp_offset >500)
							temp_offset = 0;
					}
					
				}
			}
			else
			{
				count = 0;
                rt_thread_delay(RT_TICK_PER_SECOND);
			}
			 
		}
		retrytime--;
	}while((RT_FALSE == res)&&(retrytime >= 1));
	//wifiwatch();
    //rt_mutex_release(&send_page_mutex);
	return res;
} 
static rt_err_t rt_esp8266_init(rt_device_t dev)
{
  return RT_EOK;
}

static rt_err_t rt_esp8266_open(rt_device_t dev,rt_uint16_t oflag)
{
	
	   ESP8266_CS_HIGH();
    return RT_EOK;
}

static rt_err_t rt_esp8266_close(rt_device_t dev)
{
     ESP8266_CS_LOW();
    return RT_EOK;
}
#if 0
static rt_size_t rt_esp8266_read(rt_device_t dev,rt_off_t pos, const void *buffer, rt_size_t size)
{
	rt_err_t result = RT_EOK;
	return 0;
}
#endif

rt_uint8_t send_package_check( char *buf,rt_uint16_t len)
{
    rt_uint8_t check = 0;
    rt_uint16_t i = len;
    for(;i > 0;i--)
    {
      rt_uint8_t data = *buf;
      check = check^data;
      buf++;       
    }
    return check;
}

rt_err_t get_frame_buf(sever_frame_package_t *send,char *buf,rt_size_t len)
{
    //union package_send  package1;
    
    send->frame_head =0xAA55;
    if(RT_NULL == len)
        send->frame_data_len = 6+strlen(send->pdata);
    else
        send->frame_data_len = 6+len;
    send->id = lzt_dev_id;
    send->SEQ = 0x01;
    
    //package1.frame =(char) *send;
    //rt_strncpy(buf,package1.buf,4);
    //rt_strncpy(buf+4,package1.buf+5,6);
    buf[0] = 0xAA;
    buf[1] = 0x55;
    buf[2] = (char)(send->frame_data_len>>8);
    buf[3] = (char)(send->frame_data_len&0x00ff);
    buf[4] = (char)((send->id>>16)&0xff);
    buf[5] = (char)((send->id>>8)&0xff);
    buf[6] = (char)((send->id)&0xff);
    buf[7] = send->SEQ;
    buf[8] = send->DESC;
    buf[9] = send->CMD;
    rt_memcpy(buf+10,send->pdata,(send->frame_data_len-6));
    send->check = send_package_check(buf+4,send->frame_data_len);
    buf[send->frame_data_len+4] = send->check;
    buf[send->frame_data_len+5] = 0x0d;
    buf[send->frame_data_len+6] = 0x0a;
    buf[send->frame_data_len+7] = 0xaa;
    buf[send->frame_data_len+8] = 0xaa;
    
    
   return RT_EOK; 
}
rt_size_t wifi_send(const void *args,rt_uint16_t len)
{
    wifiwatch();
    return(rt_device_write(_wifi_dev.uart_dev, 0, args, len));	
}

rt_err_t wifi_connect_sever(void *args)
{
	link_ip_t *ip = (link_ip_t*) args;
	char connect_buf[45]={0};
	char sympol1[1] = {'"'};
	char sympol2[1] = {'.'};
	char sympol3[1] = {','};
	//strcpy(temp_buf,"+++");
    wifi_send_data_package(ESP8266_CIPMODE_CLOSED,"+++",1,2,RT_NULL);
	
    if(wifi_send_data_package(ESP8266_CIPSEND,">",2,1,RT_NULL))
	{
		goto END;
	}
	//strcpy(temp_buf,"AT+CWMODE=1\x0D\x0A");
	/*if(!wifi_send_data_package(ESP8266_CWMODE_STA,"OK",2,3,RT_NULL))//station 模式
	{
		return RT_ERROR;
	}*/
    rt_thread_delay( RT_TICK_PER_SECOND/5 );
	//strcpy(temp_buf,"AT+CIPMUX=0\x0D\x0A");
	if(!wifi_send_data_package(ESP8266_CIPMUX,"OK",2,3,RT_NULL))//榨t迵模式
	{
		return RT_ERROR;
	}
    rt_thread_delay( RT_TICK_PER_SECOND/5 );
	//strcpy(temp_buf,"AT+CIPMODE=1\x0D\x0A");
	if(!wifi_send_data_package(ESP8266_CIPMODE,"OK",1,3,RT_NULL))//透垣
	{
		return RT_ERROR;
	}
	strcpy(connect_buf,"AT+CIPSTART=\"TCP\",\"");
	strcat(connect_buf,ip->IP1);
	strcat(connect_buf,sympol2);/*192.*/
	strcat(connect_buf,ip->IP2);
	strcat(connect_buf,sympol2);/*168.*/
	strcat(connect_buf,ip->IP3);
	strcat(connect_buf,sympol2);/*2.*/
	strcat(connect_buf,ip->IP4);/*108*/
	strcat(connect_buf,sympol1);/*"*/
	strcat(connect_buf,sympol3);
	strcat(connect_buf,ip->PORT);
	strcat(connect_buf,"\x0D\x0A");
    rt_thread_delay( RT_TICK_PER_SECOND/4 );
    wifi_link_status = WIFI_LINK_CONNECT_SEVER_FAILD;
	if(!wifi_send_data_package(connect_buf,"OK",2,3,RT_NULL))//l迵远詫鼐媳欠
	{
        
		return RT_ERROR;
        
	}
    rt_thread_delay( RT_TICK_PER_SECOND/2 );
	if(!wifi_send_data_package(ESP8266_CIPSEND,">",1,3,RT_NULL))
	{
		return RT_ERROR;
	}
    END:
    wifi_link_status = WIFI_LINK_CONNECT_SEVER_OK;
	return RT_EOK;
}
rt_err_t wifi_list(char *buf)
{
	rt_size_t  temp_offset = 0;
	memset(wifi_list_AP,0,sizeof(wifi_list_AP));
	char *list_begin = rt_strstr(buf+temp_offset,"+CWLAP:(");
    for(uint8_t i = 0; list_begin&&(i < WIFI_LIST_AP_MAX); i++)
	{
		char temp_rssi[3];
		int int_rssi = -90;
		//rt_size_t temp_offset2 = list_begin - buf + sizeof("+CWLAP:(");
		//char *list_begin = rt_strstr(buf+temp_offset,"+CWLAP:(");
		char *ssid_end = rt_strstr(list_begin+sizeof("+CWLAP:(")+3,",");
		char *rssi_end = rt_strstr(list_begin+sizeof("+CWLAP:(")+3,")");
		rt_size_t ssid_len = (rt_size_t)(ssid_end - (list_begin+sizeof("+CWLAP:(")+3));
		rt_size_t rssi_len = (rt_size_t)(rssi_end - (ssid_end+1));
		rt_strncpy(wifi_list_AP[i].ecn,list_begin+sizeof("+CWLAP:(")-1,1);
		rt_strncpy(wifi_list_AP[i].ssid,list_begin+sizeof("+CWLAP:(")+2,ssid_len);
		rt_strncpy(temp_rssi,ssid_end+1,rssi_len);
		temp_offset = rssi_end - buf;
		int_rssi = atoi(temp_rssi);

		if(int_rssi > -55)
			wifi_list_AP[i].rssi = 4;
		else if(int_rssi > -65)
			wifi_list_AP[i].rssi = 3;
		else if(int_rssi > -75)
			wifi_list_AP[i].rssi = 2;
		else if(int_rssi > -90)
			wifi_list_AP[i].rssi = 1;
		else
			wifi_list_AP[i].rssi = 0;
		list_begin = rt_strstr(buf+temp_offset,"+CWLAP:(");
	}	
	if( 0 == temp_offset)
		return RT_ERROR;
	else
		return RT_EOK;
}	
	
rt_err_t joap_wifi(void *args)
{
	char recv_buf[100];
	wifi_send_data_package(ESP8266_CIPMODE_CLOSED,"+++",1,2,RT_NULL);
	if(wifi_send_data_package(ESP8266_CWQAP,"OK",2,3,RT_NULL))
	{
        //rt_thread_delay( RT_TICK_PER_SECOND/4);
		if(wifi_send_data_package(ESP8266_CWMODE_STA,"OK",2,3,RT_NULL))
		{
			char *ssid_end;
			//char *password; 
			rt_size_t len1,len2,len3;
			//strcat(args,"\"\0x0D\0xA");
			ssid_end = rt_strstr(args,",");
			len1 =  rt_strlen(args);
			len2 =  rt_strlen(ssid_end);
			len3 = len1 - len2;
			//strcpy(args+len1,"\"\0x0D\0xA");
			strcpy(recv_buf,"AT+CWJAP=\"\",\"\"\x0D\x0A");
			strcpy(recv_buf+10,args);
			strcpy(recv_buf+10+len3,"\",\"");
			strcpy(recv_buf+13+len3,ssid_end+1);
			strcpy(recv_buf+12+len1,"\"\x0D\x0A");
			if(wifi_send_data_package(recv_buf,"OK",3,3,RT_NULL))
			{
				wifi_link_status = WIFI_LINK_JAP_OK;
				return RT_EOK;
			}

		}

	}
	wifi_link_status = WIFI_LINK_JAP_FAILD;
	return RT_ERROR; 
}

rt_err_t joap_wifi_status(char *buf)
{
    wifi_send_data_package(ESP8266_CIPMODE_CLOSED,"+++",1,2,RT_NULL);
    //rt_thread_delay( RT_TICK_PER_SECOND/4);
    if(wifi_send_data_package(ESP8266_CIP_JAP_STATUS,"OK",2,1,buf))
    {	
        char *status;
        char *rssi_start;
        rt_size_t ssid_len;
        rt_size_t rssi_len;
        int int_rssi = -97;
        char *split = ",";
        //char split2 = '"';
        char temp_rssi[4] = {0};
        
        status = rt_strstr(buf,"+CWJAP:");
        //strtok(buf+8,&split2);
        if(status)
        {
                strtok(status+8,split);
                strtok(NULL,split);
                strtok(NULL,split);
                rssi_start 		= strtok(NULL,split);
                ssid_len 			= strlen(status+8)-1;
                rssi_len 			= strlen(rssi_start);
                //rt_strncpy(wifi_list_AP[i].ecn,list_begin+sizeof("+CWLAP:(")-1,1);
                rt_strncpy(connected_ap.ssid,status+8,ssid_len);
                rt_strncpy(temp_rssi,rssi_start,rssi_len);
                int_rssi = atoi(temp_rssi);
                if(int_rssi > -55)
                    connected_ap.rssi = 4;
                else if(int_rssi > -65)
                    connected_ap.rssi = 3;
                else if(int_rssi > -75)
                    connected_ap.rssi = 2;
                else if(int_rssi > -90)
                    connected_ap.rssi = 1;
                else
                    connected_ap.rssi = 0;        
        wifi_link_status = WIFI_LINK_JAP_OK;
        //rt_mb_send(&wifi_mail,(rt_uint32_t)wifi_status);
        }
        else
        {
            wifi_link_status = WIFI_LINK_JAP_FAILD;
            rt_memset((void*)connected_ap.ssid,0,sizeof(connected_ap.ssid));
            connected_ap.rssi = 0;
        }
        return RT_EOK;							
    }
    else
        return RT_ERROR;
    
}


static rt_size_t rt_esp8266_write(rt_device_t dev,
                                    rt_off_t pos,
                                    const void* buffer,
                                    rt_size_t size)
{
    //struct rt_device *temp_dev = dev;
    rt_size_t len;
   
    wifiwatch(); 
    len = rt_device_write(_wifi_dev.uart_dev,0,buffer, size);

    return len;
       
}

static rt_err_t rt_esp8266_control(rt_device_t dev, rt_uint8_t cmd, void *args)
{
    rt_err_t result = RT_EOK;
    RT_ASSERT(dev != RT_NULL);
    char buf[512]={0};
    #if 1
    switch(cmd)
    {
        case WIFI_LIST:
            wifi_send_data_package(ESP8266_CIPMODE_CLOSED,"+++",1,2,RT_NULL);
            if(wifi_send_data_package(ESP8266_CWLAPOPT,"OK",2,6,RT_NULL))//
            {	
                //strcpy(buf,"AT+CWLAP\x0D\x0A");
                if(wifi_send_data_package(ESP8266_CWLAP,"OK",3,3,buf))
                {
                    wifi_list(buf);
                    result = RT_EOK;
                }		
            }
            result =  RT_ERROR;
        break;
        case WIFI_JAP:
            result = joap_wifi(args);
        break;
        case WIFI_QAP:
            wifi_send_data_package(ESP8266_CIPMODE_CLOSED,"+++",1,2,RT_NULL);
            if(wifi_send_data_package(ESP8266_CWQAP,"OK",2,3,RT_NULL))
                result =RT_EOK;
            else
                result =RT_ERROR;
        //case WIFI_SEND_DATA:
        break;
        case WIFI_CONNECT_SEVER:
            if(wifi_link_status != WIFI_LINK_JAP_FAILD)
                result = wifi_connect_sever(args);
            else
                result =  RT_ERROR;
        break;
        case WIFI_JAP_STATUS:
            //strcpy(buf,"AT+CIPSTATUS\x0D\x0A");
            result = joap_wifi_status(buf);
        break;
        default:
         break;
    }
     #endif
    return result;
}
rt_err_t rt_hw_wifi_register(struct rt_uart_extent_device *finger_dev,
                               const char              *name,
                               rt_uint32_t              flag)
{
    struct rt_device *device;
    RT_ASSERT(finger_dev != RT_NULL);
    device = &(finger_dev->parent);
    device->type        = RT_Device_Class_Char;
    device->rx_indicate = RT_NULL;
    device->tx_complete = RT_NULL;
    device->init        = rt_esp8266_init;
    device->open        = rt_esp8266_open;
    device->close       = rt_esp8266_close;
    device->read        = RT_NULL;
    device->write       = rt_esp8266_write;
    device->control     = rt_esp8266_control;
    device->user_data   = RT_NULL;
    /* register a character device */
    return rt_device_register(device, name, flag);
}

int rt_hw_wifi_init(void)
{  
	 ESP8266_CS_Init();
    _wifi_dev.uart_dev = RT_NULL;
	_wifi_dev.uart_dev = rt_device_find(ESP8266_USARTx);
	//wifi_device =  _wifi_dev.uart_dev = RT_NULL;
	//_wifi_dev.uart_dev = rt_device_find(ESP8266_USARTx);
	//wifi_device = _wifi_dev.uart_dev;;
    if (_wifi_dev.uart_dev != RT_NULL)    
    {
        rt_device_set_rx_indicate(_wifi_dev.uart_dev, wifi_uart_input);
        rt_device_open(_wifi_dev.uart_dev, RT_DEVICE_OFLAG_RDWR| RT_DEVICE_FLAG_STREAM | RT_DEVICE_FLAG_INT_RX); 	
    }
    else
    {
        return RT_FALSE;
    }
	rt_event_init(&rev_event, "rev_ev", RT_IPC_FLAG_FIFO);
	rt_mb_init(&wifi_mail,"wifi_mail",&wifi_msg_pool,(sizeof(wifi_msg_pool))/4,RT_IPC_FLAG_FIFO);
	rt_hw_wifi_register(&_wifi_dev,"wifi_dev",RT_DEVICE_FLAG_RDWR);
    joap_wifi_status(RT_NULL);
	
  return RT_TRUE;
}
INIT_COMPONENT_EXPORT(rt_hw_wifi_init);

#ifdef RT_USING_FINSH
#include <finsh.h>
//FINSH_FUNCTION_EXPORT(wifi_config, connect to usart and AT test.);
//FINSH_FUNCTION_EXPORT(wifi_init, connect to usart.);
//FINSH_FUNCTION_EXPORT(wifi_scan, SACN and list AP.);
//FINSH_FUNCTION_EXPORT(wifi_jap, ESP12F join to AP.);
//FINSH_FUNCTION_EXPORT(wifi_rssi, get ESP12F current AP rssi.);
#endif /* RT_USING_FINSH */

#endif /* RT_USING_UART_WIFI_ESP8266 */
