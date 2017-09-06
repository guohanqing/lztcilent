#include <rtthread.h>
#include "GUI.h"
#include "WM.h"
#include "app.h"
#include "time.h"
#include <drivers/hwadc.h>
#include <rthw.h>
const int lzt_dev_id = 0xe00008;
//extern char wmsg_pool[256];
char fp_status_buf[8]={0};
extern struct rt_thread emwin_thread;
extern char emWin_thread_stack[RT_APP_EMWIN_MEMORY];
BUTTON_SKINFLEX_PROPS default_Props;
const link_ip_t default_sever_ip ={"121","40","110","95","7878"};
link_ip_t using_sever_ip ={0};

win_msg_t wmsg;
static WM_HWIN HwinCurrent;
static WM_HWIN HwinCurr_Keyboard;
static WM_HWIN HwinCurr_msgbox;

int PROCESS_STATUS = TASK_MAIN;
static int Keyboard_open_flag = 0;
static int IS_WIN_MAIN_FLAG = 1;
int timer_2s_flag = 0;
date_time_t runing_time = {00,0,0,0,0,0};
static int tick_recv_faild = 0;
static rt_uint8_t tick_tosend_count = 4;
static rt_size_t windows_time_out_sec_count = 0;


static rt_err_t wifi_operate(rt_uint8_t cmd,rt_uint8_t *buf,rt_size_t len);
static rt_size_t read_adc(void);
static void app_read_time(void);
static void end_process_show_message(const char *arg);
static rt_err_t connect_server(void);
static int the_runing_window_id = ID_TASK_MAIN;
void emWin_thread_entry(void *parameter)
{
	
    rt_uint8_t second_count = 0;
    rt_uint8_t fp_status = 0;
    //gui_msgbox("指纹付款");
	//WM_Exec();
	//HAL_GPIO_EXTI_Callback(GPIO_PIN_4);
	while(1)
	{	
        /*if((the_runing_window_id == ID_TASK_USER_REGISTER_FP1)||(the_runing_window_id == ID_TASK_USER_REGISTER_FP2)||(the_runing_window_id == ID_TASK_USER_REGISTER_FP3))
        {
            if(GPIO_PIN_SET == FP_OUT_PIN_READ)
            {
                if(0 == fp_status)
                {
                    strcpy(fp_status_buf,"输入中");
                    WM_SendMessageNoPara(HwinCurrent,WM_USER);
                    fp_status = 1;
                }
            }
            else
            {
                rt_memcpy(fp_status_buf,0,8);
                WM_SendMessageNoPara(HwinCurrent,WM_USER);
                fp_status = 0;
            }
        }*/
        
        
        if(1 == timer_2s_flag)
         { 
            //rt_uint8_t send_tick[4];
            tick_tosend_count++;
            #if 1 
             
                /*更新wifi状态*/            
            if(wifi_link_status != WIFI_LINK_CONNECT_SEVER_OK)
            {
                if(1 == IS_WIN_MAIN_FLAG)
                {
                    
                    if(2 == second_count)
                        wifi_jap_status();
                    else if(10 < second_count)
                    {
                        //wifi_operate(WIFI_LIST,RT_NULL,RT_NULL);
                        second_count = 0;
                    }
                    
                    
                    //wifi_ap_status();
                    //list_wifi();
                }
            } 
            #endif
            /*发心跳数据包*/
            if((PROCESS_STATUS == TASK_OWNER_REGIST)||(PROCESS_STATUS == TASK_USER_REGIST)||(PROCESS_STATUS == TASK_PAY))
            {
                if(tick_tosend_count > 7)
                { 
                    send_frame_package(CMD_SEND_TICK,0,RT_NULL,RT_NULL);
                    tick_tosend_count = 0;
                }
            }
            /*更新窗口*/
            if(1 == IS_WIN_MAIN_FLAG)
            {
               battery_capity_bar = read_adc();
                app_read_time();                
               WM_SendMessageNoPara(HwinCurrent,WM_USER);
               second_count++;
               if(0 == runing_time.year)
               {
                    send_frame_package(CMD_SYSNC_TIME,0,RT_NULL,RT_NULL);
                    //run_to_win(ID_TASK_MAIN);
               }
            }
            else
            {
                second_count = 0;
                /*if(1 == windows_time_out_sec_count)
                    end_process_show_message("超时");*/
            }
            timer_2s_flag = 0;
            
        } 	    
        WM_Exec();
        //GUI_Delay(RT_TICK_PER_SECOND/20);
		rt_thread_delay(RT_TICK_PER_SECOND/20);
	}
}
//int count = 0;
void app_timeout_callbak(void* parameter)
{
    
    timer_2s_flag = 1;
    tick_recv_faild++;
    if(0 < windows_time_out_sec_count)
        windows_time_out_sec_count--;
    if(5 < tick_recv_faild)
        wifi_link_status  =  WIFI_LINK_CONNECT_SEVER_FAILD;
    /*发心跳数据*/   
    /*读取电量*/
    
    /******************/
    /*读取时间*/
    /***更新窗口***/
    //count++;
}
extern struct rt_thread background_thread;
extern char background_thread_stack[RT_APP_BACKGROUND_MEMORY];

void wifi_jap_status(void)
{
    wifi_operate(WIFI_JAP_STATUS,RT_NULL,RT_NULL);
}
static void app_read_time(void)
{
    struct tm  rt_time;
     
     rt_device_t rtc_device = rt_device_find("rtc"); 
     /*更新时间*/
    if(RT_NULL != rtc_device)
    {
        rtc_device->control(rtc_device,RT_DEVICE_CTRL_RTC_GET_TIME,&rt_time);
        runing_time.year  	=  rt_time.tm_year;
        runing_time.month 	=  rt_time.tm_mon;
        runing_time.day  	=  rt_time.tm_mday;
        runing_time.hour 	=  rt_time.tm_hour;
        runing_time.minute 	=  rt_time.tm_min;
        runing_time.second 	=  rt_time.tm_sec;
    }
}
static rt_size_t read_adc(void)
{ 
    rt_size_t battery_capity_bar_value = battery_capity_bar;
    rt_device_t adc_dev = RT_NULL;   
    //uint16_t Current_Temperature; 
    __IO uint32_t recv_adc; 
    //rt_device_hwadc_t *devadc;
    float adc; 
    float read_Vbat;
    float adc_0 = 3.7;
    float dispalay_value;
    #define HWADC   "hwadc"
       
    //rt_pin_mode(20, PIN_MODE_OUTPUT);// the port PF8
   
    if ((adc_dev = rt_device_find(HWADC)) == RT_NULL)
    {
        goto END;  
        
    }
    //devadc = (rt_device_hwadc_t *)adc_dev;
    rt_device_open(adc_dev, RT_DEVICE_FLAG_RDONLY);
    rt_device_control(adc_dev, HWTADC_CTRL_START, RT_NULL);      
    rt_device_read(adc_dev, 0, (void*)&recv_adc, 1);    
    adc = ((float)(recv_adc&0x0FFF)*3.3/4096);
    read_Vbat = adc+adc/2;
    
    dispalay_value=100*(read_Vbat-adc_0)/(4.2-adc_0);
    //dispalay_value=1.25*dispalay_value ;  //修正
	
	 if(dispalay_value> 100)
		     dispalay_value=100;
	 if(dispalay_value<0)
		   dispalay_value=0;
     battery_capity_bar_value = (int)dispalay_value;     
     //battery_capity_bar_value = 15;
    rt_device_close(adc_dev);
    END:
    return battery_capity_bar_value;
}
static void get_finger_fp(void)
{
    rt_device_t _fp_device = rt_device_find("finger_dev"); 
    if(RT_NULL != _fp_device)
    {
        _fp_device->open(_fp_device,0);
        _fp_device->control(_fp_device,FP_REG,RT_NULL);
    } 
}

void drv_sync_time(date_time_t *set_time)
{
    rt_device_t rtc_device = rt_device_find("rtc"); 
             /*更新时间*/
    if(RT_NULL != rtc_device)
    {
        struct tm rt_time;
        
        rt_time.tm_year =   set_time->year;
        rt_time.tm_mon  =   set_time->month;
        rt_time.tm_mday =   set_time->day;
        rt_time.tm_hour =   set_time->hour;
        rt_time.tm_min  =   set_time->minute;
        rt_time.tm_sec  =   set_time->second;
        rtc_device->control(rtc_device,RT_DEVICE_CTRL_RTC_SET_TIME,&rt_time);
    }
}
static rt_err_t fp_recv_data_do(rt_uint8_t *buf)
{
    if(ACK_SUCCESS == (int)buf[4])
        return RT_EOK;
    else 
        return RT_ERROR;
    
}
static void unpack_frame(char *buf,sever_frame_package_t* frame)
{
     /*/////////帧数据长度///////////// */
    //frame_package_t temp_frame;
    //char temp_buf[30] = {0};
    int data_len = 0;
    
    //memcpy(temp_frame.buf,buf,10);
    //frame = temp_frame.frame;
    //rt_strncpy(temp_buf,buf+2,2);
    frame->frame_data_len = ((rt_uint16_t)buf[2] <<8)+(rt_uint16_t)buf[3];
    frame->id  = ((rt_uint16_t)buf[4] <<16)+((rt_uint16_t)buf[5] <<8)+(rt_uint16_t)buf[6];
    frame->SEQ  = buf[7];
    frame->DESC  =  buf[8]; 
    frame->CMD  =  buf[9];
    frame->frame_head = (((rt_uint16_t)buf[0])<<8) + (rt_uint16_t)buf[1];
    frame->check = buf[data_len+4];
    frame->pdata = buf+10;   
}
static rt_uint8_t is_list_wifi_win = 0;
void run_to_win(rt_uint8_t  WIN_ID)
{
    //GUI_MEMDEV_Delete(HwinCurrent);
    //rt_uint8_t wifi_list_status = 0;
    // rt_base_t leaval;
    //leaval = rt_hw_interrupt_disable();    
    GUI_EndDialog(HwinCurrent,0);
    WM_DeleteWindow(HwinCurrent);
    IS_WIN_MAIN_FLAG = 0;
    is_list_wifi_win = 0;
    the_runing_window_id = WIN_ID;
    windows_time_out_sec_count = 5;
    if(1 == Keyboard_open_flag)
    {
        WM_DeleteWindow(HwinCurr_Keyboard);
        Keyboard_open_flag = 0;
    }
    switch(WIN_ID)
    {
        case ID_TASK_MAIN:
            HwinCurrent = Createmain();
            IS_WIN_MAIN_FLAG = 1;
            //wifi_list_status = 0;
            PROCESS_STATUS = TASK_MAIN;
            break;
        case ID_TASK_PAY_NUM:
            HwinCurrent = Creat_Pay_input();
            Keyboard_open_flag = 1;
            HwinCurr_Keyboard = keyboard_num(HwinCurrent);
            PROCESS_STATUS = TASK_PAY; 
            connect_server();        
			break;
        case ID_TASK_PAY_UDID  :
            HwinCurrent = Creat_Pay_UDID();
            Keyboard_open_flag = 1;
            HwinCurr_Keyboard = keyboard_num(HwinCurrent);
            PROCESS_STATUS = TASK_PAY;
            break;
        case ID_TASK_USER_REGISTER_PHONE:
            connect_server();
            HwinCurrent = Creat_user_phone_input();
            Keyboard_open_flag = 1;
            HwinCurr_Keyboard = keyboard_num(HwinCurrent);
            PROCESS_STATUS = TASK_USER_REGIST;
            break;
        case ID_TASK_USER_REGISTER_CHECKNUM:
            HwinCurrent = Creat_user_validation_input();
            Keyboard_open_flag = 1;
            HwinCurr_Keyboard = keyboard_num(HwinCurrent);
            PROCESS_STATUS = TASK_USER_REGIST;
            break;
        case ID_TASK_USER_REGISTER_FP1:
            HwinCurrent = CreateFp_regist1();
            PROCESS_STATUS = TASK_USER_REGIST;
            break;
        case ID_TASK_USER_REGISTER_FP2:
            HwinCurrent = CreateFp_regist2();
            PROCESS_STATUS = TASK_USER_REGIST;
            break;                  
        case ID_TASK_USER_REGISTER_FP3:
            HwinCurrent = CreateFp_regist3();
            PROCESS_STATUS = TASK_USER_REGIST;
            break;                    
        case ID_TASK_OWNER_REGISTER_PHONE:
            connect_server();
            HwinCurrent = Creat_owner_phone_input();
            Keyboard_open_flag = 1;
            HwinCurr_Keyboard = keyboard_num(HwinCurrent);
            PROCESS_STATUS = TASK_OWNER_REGIST;
            break;
        case ID_TASK_OWNER_REGISTER_CHECKNUM :
            HwinCurrent = Creat_owner_validation_input();
            Keyboard_open_flag = 1;
            HwinCurr_Keyboard = keyboard_num(HwinCurrent);
            PROCESS_STATUS = TASK_OWNER_REGIST;
            break;
        case ID_TASK_LIST_WIFI:
            is_list_wifi_win = 1;
            HwinCurrent = Createwifi_list();
            WM_Exec();
            //wifi_list_status = 1;
            wifi_operate(WIFI_LIST,RT_NULL,RT_NULL);
            if(1 == is_list_wifi_win)
            {    
                GUI_EndDialog(HwinCurrent,0);
                WM_DeleteWindow(HwinCurrent);
                HwinCurrent = Createwifi_list();
                WM_Exec();
                //wifi_list_status = 0;
            }
            PROCESS_STATUS = TASK_MAIN;
            break;
        default:
            break;
    }
    //rt_hw_interrupt_enable(leaval);
    //WM_Exec();    
}

void run_to_jap_wifi(char *buffer)
{
   
    
    is_list_wifi_win = 0;
    the_runing_window_id = ID_TASK_JOAP_WIFI;
    windows_time_out_sec_count = 10;
    WM_DeleteWindow(HwinCurrent);
    HwinCurrent = Createwifi_jap(buffer);
    Keyboard_open_flag = 1;
    HwinCurr_Keyboard = keyboard_a(HwinCurrent);
    PROCESS_STATUS = TASK_MAIN;    
}    

static void end_process_show_message(const char *arg)
{
    //WM_HWIN hmsg_box;
    HwinCurr_msgbox = gui_msgbox(arg,HwinCurrent);
    rt_thread_delay(RT_TICK_PER_SECOND*2);
    GUI_EndDialog(HwinCurr_msgbox,0);
    WM_DeleteWindow(HwinCurr_msgbox);
    run_to_win(ID_TASK_MAIN);
}

static void show_message_bytime(const char *arg,rt_tick_t tick)
{
    //WM_HWIN hmsg_box;
    HwinCurr_msgbox = gui_msgbox(arg,HwinCurrent);
    if(RT_NULL != tick)
    {
        rt_thread_delay(tick);
        GUI_EndDialog(HwinCurr_msgbox,0);
        WM_DeleteWindow(HwinCurr_msgbox);
    }
    //run_to_win(ID_TASK_MAIN);
}
static void end_msg_box(void)
{
    GUI_EndDialog(HwinCurr_msgbox,0);
    WM_DeleteWindow(HwinCurr_msgbox);
}
static rt_err_t connect_server(void)
{
    rt_err_t result = RT_ERROR;
    if(WIFI_LINK_CONNECT_SEVER_OK != wifi_link_status)
    {
        union link_sever_ip ipx;
        rt_uint8_t count= 3; 
        if(0 == using_sever_ip.IP1[0])
            ipx.ip = default_sever_ip;
        else
            ipx.ip = using_sever_ip;
        do{
            result = wifi_operate(WIFI_CONNECT_SEVER,(void *)ipx.ip_buf,RT_NULL);
            count--;
          }while((result == RT_ERROR)&&(count > 1));
      if(RT_EOK != result)
          end_process_show_message("连接服务器失败");
    } 
    return result;   
}
rt_err_t send_frame_package(rt_uint8_t cmd,rt_uint8_t desc,char *args,rt_size_t data_len)
{
    sever_frame_package_t   send_frame;
    char buf[522] = {0};
    tick_tosend_count = 0;
    //rt_base_t leval;
    rt_err_t resault;
    //leval = rt_hw_interrupt_disable();
    if(WIFI_LINK_JAP_FAILD != wifi_link_status)
    {
      
        /*if(WIFI_LINK_CONNECT_SEVER_OK != wifi_link_status)
        {
                union link_sever_ip ipx;
                if(0 == using_sever_ip.IP1[0])
                    ipx.ip = default_sever_ip;
                else
                    ipx.ip = using_sever_ip;
               if(RT_EOK != wifi_operate(WIFI_CONNECT_SEVER,(void *)ipx.ip_buf,RT_NULL)) 
               {
                   if(cmd != CMD_SEND_TICK)
                        end_process_show_message("连接失败");
                   goto SEND_END;
               }
        }*/
        connect_server();
        send_frame.CMD = cmd;
        send_frame.DESC = desc;
        switch(cmd)
        {
            case CMD_SEND_FP:
                send_frame.pdata = args;
                 break;//
            case CMD_SEND_USER_PHONE_NUM://04	//
                    send_frame.pdata = args;
                    send_frame.DESC =  0;
                 break;
            case CMD_SEND_USER_PHONE_CHECK://07	//
                send_frame.pdata = args;
                send_frame.DESC =  0;
                 break;
            case CMD_PAY_NUM ://08	//
                send_frame.pdata = args;
                send_frame.DESC =  0;
                 break;
            case CMD_SEND_UDID ://09	
                send_frame.pdata = args;
                send_frame.DESC =  0;
                 break;
            case CMD_SEND_TICK ://0A	
                {
                    char temp_buf[2] = {0xFF,0xFF};
                    send_frame.pdata = temp_buf;
                    send_frame.DESC =  0;
                }
                 break;
            case CMD_SYSNC_TIME:	////
                {
                    char temp_buf[2] = {0xff,0xF4};
                    send_frame.pdata = temp_buf;
                    send_frame.DESC =  0;
                }
                 break;
            case CMD_SEND_OWNER_PHONE_NUM://E8	//
                    send_frame.pdata = args;
                    send_frame.DESC =  0;
                 break;
            case CMD_SEND_OWNER_PHONE_CHECK://EB	
                send_frame.pdata = args;
                send_frame.DESC =  0;
                 break;
            case CMD_UPDATA:
                break;
            default:
                break;
        }
        get_frame_buf(&send_frame,buf,data_len);
        {         
            resault =  wifi_operate(WIFI_SEND_DATA,(rt_uint8_t*)buf,send_frame.frame_data_len+9);
        }
    } 
    else
       resault = RT_ERROR;

    //rt_hw_interrupt_enable(leval);
    return resault;
}

static int wifi_status = 0; 
static rt_err_t wifi_operate(rt_uint8_t cmd,rt_uint8_t *buf,rt_size_t len)
{
    rt_uint8_t send_count;
    rt_err_t result = RT_ERROR;
    rt_mutex_take(&wifi_send_mutex,3*RT_TICK_PER_SECOND);
    if(WIFI_SEND_DATA == cmd)
        send_count = 3;
    else if(WIFI_CONNECT_SEVER == cmd)
        send_count = 2;
    else
        send_count = 1;
    do{
        if(1 != wifi_status)
        {
            wifi_status = 1;
             rt_device_t wifi = rt_device_find("wifi_dev"); 
            if(RT_NULL != wifi)
            {
                switch(cmd)
                {
                    case WIFI_LIST:
                         result = wifi->control(wifi,WIFI_LIST,RT_NULL);
                    break;
                    case WIFI_JAP_STATUS:
                        result = wifi->control(wifi,WIFI_JAP_STATUS,RT_NULL);
                    break;
                    case WIFI_CONNECT_SEVER:
                        result = wifi->control(wifi,WIFI_CONNECT_SEVER,buf);
                    break;
                    case WIFI_SEND_DATA:
                        if(len == wifi->write(wifi,0,buf,len))
                            result = RT_EOK;
                        else
                            result = RT_ERROR;
                    break;
                    case WIFI_JAP:
                        result = wifi->control(wifi,WIFI_JAP,buf);
                    break;    
                    default:
                    break;
                }
            }
            wifi_status = 0;
            send_count = 0;
        }
        else
        {
            result = RT_ERROR;
            send_count -- ;
            rt_thread_delay(RT_TICK_PER_SECOND);
        }
    }while(send_count > 0);
    rt_mutex_release(&wifi_send_mutex);
    return result;
}

static void task_pay(win_msg_t *wmsgx)
{
    sever_frame_package_t frame;
    unpack_frame(wmsgx->buf,&frame);
    switch(wmsgx->id_operate)
    {
        case RECV_SERVE:
        {
            switch((rt_uint8_t)frame.CMD)
            {
                case CMD_SEND_FP:
                {
                    if(0xff == (rt_uint8_t)wmsgx->buf[10])
                    {    
                        switch((rt_uint8_t)wmsgx->buf[11])
                        {   
                            case 0x03:
                                if(0x00 == frame.DESC)
                                {
                                   end_msg_box();
                                   run_to_win(ID_TASK_PAY_UDID); 
                                }
                            break;
                            case 0x04:
                                end_process_show_message("验证失败");
                            break;
                            default:
                            break;
                        }      
                    }

                }
                break;
                case CMD_PAY_NUM:
                {
                    if(0xff == (rt_uint8_t)wmsgx->buf[10])
                    {
                        switch((rt_uint8_t)wmsgx->buf[11])
                        {                    
                            case 0x03:
                                get_finger_fp();
                                show_message_bytime("请输入您的指纹",RT_NULL);
                            break;
                            case 0x04:
                                end_process_show_message("验证失败");                
                            break;
                            default:
                            break;
                        }  
                    }
                  
                }
                break; 					
                case CMD_SEND_UDID :
                 {
                      if(0xff == (rt_uint8_t)wmsgx->buf[10])
                      {
                        switch((rt_uint8_t)wmsgx->buf[11])
                        {                    
                            case 0x03:
                                //gui_msgbox("付款成功");
                               // Createmain();
                            break;
                            case 0x04:
                                //gui_msgbox("付款失败");
                                //Createmain();
                            break;
                            case 0x05:
                                end_process_show_message("付款成功");
                            break;
                            case 0x06:
                                end_process_show_message("指纹匹配失败");
                            break;
                            default:
                            break;
                        }                 
                      }
                      
                 }   
                break;
                case CMD_SEND_TICK :
                 {
                      if(0xff == (rt_uint8_t)wmsgx->buf[10])
                      {
                        switch((rt_uint8_t)wmsgx->buf[11])
                        {                    
                            case 0x03:
                                wifi_link_status  =  WIFI_LINK_CONNECT_SEVER_OK;
                                tick_recv_faild = 0;                            
                                //gui_msgbox("付款成功");
                               // Createmain();
                            break;
                            default:
                            break;
                        }                 
                      }
                      
                 }   
                break;
                case CMD_SYSNC_TIME:
                {
                    union_date_time_t recv_time;
                    rt_memcpy(recv_time.buf,frame.pdata,6);
                    runing_time = recv_time.time;
                    drv_sync_time(&recv_time.time);
                }
                break;   
                default:
                break;
            }               
        }
        break;
        case RCV_DEVICE:
        switch(wmsgx->id_win)
        {
            case ID_TASK_SEND_FP:
                if((0xF5 ==  (rt_uint8_t)wmsgx->buf[0])&&(0x00 ==  (rt_uint8_t)wmsgx->buf[4]))
                {
                    send_frame_package(CMD_SEND_FP,0,wmsgx->buf+9,497);//494
                }
                else if(0x08 == (rt_uint8_t)wmsgx->buf[0])
                    end_process_show_message("指纹读取失败");
                else 
                {
                    show_message_bytime("读取失败，请输入指纹",RT_TICK_PER_SECOND/2);
                    get_finger_fp();
                }
            break;
            default:
            break;
        }
        break;
        case SEND_TO_SERVE:
        {
             switch(wmsgx->id_win)
             {
                 case ID_TASK_PAY_NUM:
                     send_frame_package(CMD_PAY_NUM,0,wmsgx->buf,RT_NULL);
                 break;
                 case ID_TASK_PAY_UDID:
                     send_frame_package(CMD_SEND_UDID,0,wmsgx->buf,RT_NULL);
                 break;
                 case ID_TASK_PAY_FP:
                 break;
                 default:    
                 break;
             }
        }
        break;
        case RETURN_FROM_WIN:
        switch(wmsgx->id_win)
         {
             case ID_TASK_PAY_NUM:
                 run_to_win(ID_TASK_MAIN);
             break;
             case ID_TASK_PAY_UDID:
                 run_to_win(ID_TASK_PAY_NUM);
             break;
             case ID_TASK_PAY_FP:
             break;
             default:    
             break;
         }
        break;
        default:
        break;
    }
    
}

static int fp_register_count = 0;
static void task_user_register(win_msg_t *wmsgx)
{
    sever_frame_package_t frame;
    unpack_frame(wmsgx->buf,&frame);
    switch(wmsgx->id_operate)
    {
        case RECV_SERVE:
        {
            switch((rt_uint8_t)frame.CMD)
            {
                case CMD_SEND_FP:
                {
                    if(0xff == (rt_uint8_t)wmsgx->buf[10])
                    {    
                        switch((rt_uint8_t)wmsgx->buf[11])
                        {   
                            case 0x03:
                                if(0x01 == frame.DESC)
                                {
                                    fp_register_count = 2;
                                    get_finger_fp();
                                    run_to_win(ID_TASK_USER_REGISTER_FP2);
                                    //WM_DeleteWindow(HwinCurrent);
                                    //HwinCurrent = CreateFp_regist2();
                                }
                                else if(0x02 == frame.DESC)
                                {
                                     fp_register_count = 3;
                                     get_finger_fp();
                                     run_to_win(ID_TASK_USER_REGISTER_FP3);
                                     //WM_DeleteWindow(HwinCurrent);
                                     //HwinCurrent = CreateFp_regist3();
                                }
                                else
                                {
                                   end_process_show_message("注册失败"); 
                                }
                                    
                            break;
                            case 0x04:
                                show_message_bytime("验证失败",RT_TICK_PER_SECOND/2);
                                //Createmain();
                            break;
                            case 0x05:
                                //gui_msgbox("注册成功");
                                end_process_show_message("注册成功");
                                //Createmain();
                            break;
                            case 0x06:
                                end_process_show_message("注册失败");
                                //Createmain();
                            break;
                            default:
                            break;
                        }      
                    }

                }
                break;
                case CMD_SEND_USER_PHONE_NUM :
                {
                        switch((rt_uint8_t)wmsgx->buf[11])
                        {                    
                            case 0x03:
                            {
                                run_to_win(ID_TASK_USER_REGISTER_CHECKNUM);
                                //WM_DeleteWindow(HwinCurrent);
                                //HwinCurrent = Creat_user_validation_input();
                            }
                            break;
                            case 0x04:
                                //end_process_show_message("手机号码");
                                end_process_show_message("手机号码错误");
                                //Createmain();
                            break;
                            case 0x05:
                                end_process_show_message("已绑定指纹");
                                //Createmain();
                            break;
                            default:
                            break;
                        }  

                }
                break;		
                case CMD_SEND_USER_PHONE_CHECK:
                {
                         switch((rt_uint8_t)wmsgx->buf[11])
                        {                    
                            case 0x03:
                            {
                                fp_register_count = 1;
                                get_finger_fp();
                                run_to_win(ID_TASK_USER_REGISTER_FP1);
                            }
                            break;
                            case 0x04:
                                end_process_show_message("验证失败");
                                //Createmain();
                            break;
                            default:
                            break;
                        }  
                  
                }
                break;
                case CMD_SEND_TICK :
                 {
                      if(0xff == (rt_uint8_t)wmsgx->buf[10])
                      {
                        switch((rt_uint8_t)wmsgx->buf[11])
                        {                    
                            case 0x03:
                                wifi_link_status  =  WIFI_LINK_CONNECT_SEVER_OK;
                                tick_recv_faild = 0;                            
                                //gui_msgbox("付款成功");
                               // Createmain();
                            break;
                            default:
                            break;
                        }                 
                      }
                      
                 }   
                break;
                case CMD_SYSNC_TIME:
                {
                    union_date_time_t recv_time;
                    rt_memcpy(recv_time.buf,frame.pdata,6);
                    runing_time = recv_time.time;
                    drv_sync_time(&recv_time.time);
                }
                break;                 
                default:
                break;
            }
        }
        break;
        case RCV_DEVICE:
        switch(wmsgx->id_win)
        {
            case ID_TASK_SEND_FP:
                if(0< fp_register_count < 4)
                {
                    if(RT_EOK==fp_recv_data_do((rt_uint8_t*)wmsgx->buf))
                        send_frame_package(CMD_SEND_FP,fp_register_count,wmsgx->buf+9,497);
                    else
                    {
                       show_message_bytime("读取失败，请输入指纹",RT_TICK_PER_SECOND/2);
                       get_finger_fp(); 
                    }
                }
            break;
            default:
            break;
        }
         break;
        default:
        break;
    }
}

static void task_owner_register(win_msg_t *wmsgx)
{
     sever_frame_package_t frame; 
     unpack_frame(wmsgx->buf,&frame);
     switch(wmsgx->id_operate)
     {
         case RECV_SERVE:
            switch((rt_uint8_t)frame.CMD)
            {
                case CMD_SEND_OWNER_PHONE_NUM :
                {
                        switch((rt_uint8_t)wmsgx->buf[11])
                        {                    
                            case 0x03:
                                run_to_win(ID_TASK_OWNER_REGISTER_CHECKNUM);
                                //WM_DeleteWindow(HwinCurrent);
                                //HwinCurrent = Creat_owner_validation_input();
                            break;
                            case 0x04:
                                end_process_show_message("手机号码错误");
                                //Createmain();
                            break;
                            case 0x05:
                                end_process_show_message("已绑定设备");
                                //Createmain();
                            break;
                            case 0x06:
                                end_process_show_message("请先注册");
                                //Createmain();
                            break;
                            default:
                            break;
                        }  

                }
                break;		
                case CMD_SEND_OWNER_PHONE_CHECK:
                {
                         switch((rt_uint8_t)wmsgx->buf[11])
                        {                    
                            case 0x03:
                                end_process_show_message("绑定成功");
                            break;
                            case 0x04:
                                end_process_show_message("绑定失败");
                                //Createmain();
                            break;
                            default:
                            break;
                        }  
                  
                }
                break;
                case CMD_SEND_TICK :
                 {
                      if(0xff == (rt_uint8_t)wmsgx->buf[10])
                      {
                        switch((rt_uint8_t)wmsgx->buf[11])
                        {                    
                            case 0x03:
                                wifi_link_status  =  WIFI_LINK_CONNECT_SEVER_OK;
                                tick_recv_faild = 0;                            
                                //gui_msgbox("付款成功");
                               // Createmain();
                            break;
                            default:
                            break;
                        }                 
                      }
                      
                 }   
                break;
                case CMD_SYSNC_TIME:
                {
                    
                    union_date_time_t recv_time;
                    rt_memcpy(recv_time.buf,frame.pdata,6);
                    runing_time = recv_time.time;
                    drv_sync_time(&recv_time.time);
                }
                break;                 
                default:
                break;
            }
         break;
         case RCV_DEVICE:
         break;
         case SEND_TO_SERVE:
         {
             switch(wmsgx->id_win)
             {
                 case ID_TASK_OWNER_REGISTER_PHONE:
                     if(RT_EOK != send_frame_package(CMD_SEND_OWNER_PHONE_NUM,0,wmsgx->buf,RT_NULL))
                         end_process_show_message("连接失败");
                 break;
                 case ID_TASK_OWNER_REGISTER_CHECKNUM:
                     if(RT_EOK !=send_frame_package(CMD_SEND_OWNER_PHONE_CHECK,0,wmsgx->buf,RT_NULL))
                         end_process_show_message("发送失败");
                 break;
                 default:
                 break;
             }
         }
         break;

        case RETURN_FROM_WIN:
            switch(wmsgx->id_win)
            {
                 case ID_TASK_OWNER_REGISTER_PHONE:
                     run_to_win(ID_TASK_MAIN);
                 break;
                 case ID_TASK_OWNER_REGISTER_CHECKNUM:
                     run_to_win(ID_TASK_OWNER_REGISTER_PHONE);
                 break;
                 case ID_TASK_PAY_FP:
                 break;
                 default:    
                 break;
            }
        break;
        default:
        break;                     
        
    
    }
}

static void task_other(win_msg_t *wmsgx)
{
     //sever_frame_package_t frame;
    //unpack_frame(wmsgx->buf,&frame);
    switch(wmsgx->id_operate)
    {
        case SEND_TO_SERVE:
        switch(wmsgx->id_win)
        {
            case ID_TASK_JOAP_WIFI:
            {
                rt_device_t _wifi_device = rt_device_find("wifi_dev"); 
                if(RT_NULL != _wifi_device)
                {
                    if(RT_EOK == wifi_operate(WIFI_JAP,(rt_uint8_t*)wmsgx->buf,RT_NULL))
                    {
                        wifi_operate(WIFI_JAP_STATUS,RT_NULL,RT_NULL);
                        end_process_show_message("连接成功");
                    }
                    else
                        end_process_show_message("连接失败");
                    //_wifi_device->control(_wifi_device,WIFI_JAP_STATUS,RT_NULL);
                }       
            }    
            break;
            case ID_TASK_LIST_WIFI:
            {
               
                //Keyboard_switch_flag = 1;
            }    
            break;
            case ID_TASK_SYSNC_TIME:
            {
                send_frame_package(CMD_SYSNC_TIME,0,wmsgx->buf,6);
            }    
            break;
            default:
            break;
        }
        break;
        case RECV_SERVE:
        {   
            sever_frame_package_t frame;
            unpack_frame(wmsgx->buf,&frame);
            switch((rt_uint8_t)frame.CMD)
            {
                case CMD_SYSNC_TIME:
                {
                    
                    union_date_time_t recv_time;
                    rt_memcpy(recv_time.buf,frame.pdata,6);
                    runing_time = recv_time.time;
                    drv_sync_time(&recv_time.time);
                }
                break;
                default:
                break;
            }
        }
        break;
        case RCV_DEVICE:
        switch(wmsgx->id_win)
        {
            case ID_TASK_SYSNC_TIME:
            {
                union_date_time_t timex;
                rt_memcpy(timex.buf,wmsgx->buf,6);
				runing_time = timex.time;
			}
            break;
            case ID_MSG_SWITCH_KEYBOARD:
                switch(wmsgx->buf[0])
                {
                    case KEYBOARD_ABC:
                        WM_DeleteWindow(HwinCurr_Keyboard);
                        Keyboard_open_flag = 1;
                        HwinCurr_Keyboard = keyboard_a(HwinCurrent);
                    break;
                    case KEYBOARD_NUM:
                        WM_DeleteWindow(HwinCurr_Keyboard);
                        Keyboard_open_flag = 1;
                        HwinCurr_Keyboard = keyboard_num(HwinCurrent);
                    break;
                }
            break;
            default:    
            break;
        }
        break;
        case RETURN_FROM_WIN:
            switch(wmsgx->id_win)
            {
                 case ID_TASK_LIST_WIFI:
                     run_to_win(ID_TASK_MAIN);
                 break;
                 case ID_TASK_JOAP_WIFI:
                     run_to_win(ID_TASK_LIST_WIFI);
                 break;
                 #if 1
                 case ID_TASK_MAIN:
                   switch(wmsgx->buf[0])
                    {
                        /* 会员注册 */
                        case 0:
                            //Creat_user_phone_input();
                            run_to_win(ID_TASK_USER_REGISTER_PHONE);
                            break;
                        /*指纹付款*/
                        case 1:
                            run_to_win(ID_TASK_PAY_NUM);
                            break;
                        /*商户绑定*/
                        case 2:
                            run_to_win(ID_TASK_OWNER_REGISTER_PHONE);
                            break;
                        /*wifilist列表*/
                        case 55:
                            run_to_win(ID_TASK_LIST_WIFI);
                            break;
                        default:
                            break;									
                    }
                  break;
                    #endif
                 default:    
                 break;
            }

        break;
        default:
        break;
    }
    
  
}

void background_thread_entry(void *parameter)
{
	//char buf[RT_APP_MSG_LEN];
	win_msg_t wmsg;
    GUI_Init();
	GUI_UC_SetEncodeUTF8();
	GUI_SetFont(&GUI_Font_song16);
	BUTTON_GetSkinFlexProps(&default_Props, BUTTON_SKINFLEX_PI_ENABLED);
	HwinCurrent = Createmain();
	while(1)
	{
		if (RT_EOK == rt_mq_recv(&wmq, (win_msg_t *)&wmsg, sizeof(win_msg_t),RT_WAITING_FOREVER))//RT_WAITING_FOREVER
		{
            if(ID_TASK_SEND_TICK == wmsg.id_operate)
            {
                send_frame_package(CMD_SEND_TICK,0,wmsg.buf,2);   
            }
            else
            {
                switch(PROCESS_STATUS)
                {
                    case TASK_MAIN:
                        task_other(&wmsg);
                    break;                    
                    case  TASK_PAY:
                        task_pay(&wmsg);
                    break;             
                    case  TASK_USER_REGIST:
                        task_user_register(&wmsg);
                    break;               
                    case  TASK_OWNER_REGIST:
                        task_owner_register(&wmsg);
                    break;              
                    default:
                    break;                     
                }
            }
            memset(wmsg.buf,0,sizeof(wmsg.buf));
		}	
		rt_thread_delay(RT_TICK_PER_SECOND/2);
	}
}
