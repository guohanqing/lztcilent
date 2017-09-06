
#ifndef APP_H
#define APP_H
#include "GUI.h"
#include "DIALOG.h"
#include "IMAGE.h"
#include "usart_wifi_esp8266.h"
#include "drv_uart_finger.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
//#include "png.h"

// color define
#define COLOR_BACK0            0xFF3333
#define COLOR_BACK1            0x550000
#define COLOR_BORDER           0x444444
#define COLOR_KEYPAD0          0xAAAAAA
#define COLOR_KEYPAD1          0x555555

#define BUTTON_COLOR0          0xEEEEEE
#define BUTTON_COLOR1          0xCCCCCC
#define BUTTON_COLOR2          0xCCCCCC
#define BUTTON_COLOR3          0xAAAAAA

#define RT_APP_BACKGROUND_MEMORY 			1024*5
//#define RT_APP_UPDATER_MEMORY 			1024*3
#define RT_APP_EMWIN_MEMORY 			1024*4

#define ID_TEXT_DATE            (GUI_ID_USER + 0x03)
#define ID_TEXT_TIME            (GUI_ID_USER + 0x04)

//#define ID_WINDOW_BAR     		(GUI_ID_USER + 0x16)
//#define ID_PROGBAR_BAT     		(GUI_ID_USER + 0x18)

//#define ID_BUTTON_WIFI     		(GUI_ID_USER + 0x20)
//#define ID_BUTTON_REGISTER1		(GUI_ID_USER + 0x21)
//#define ID_BUTTON_REGISTER2		(GUI_ID_USER + 0x22)
//#define ID_BUTTON_PAY_MONEY1	(GUI_ID_USER + 0x23)
//#define ID_BUTTON_PAY_MONEY2	(GUI_ID_USER + 0x24)
//#define ID_BUTTON_OWNER_BIND	(GUI_ID_USER + 0x25)

// define the state machine to schedule the job event
//#define 	ID_WIFI_LIST		0x01
//#define 	ID_WIFI_LINK		0x02
//#define		ID_REGESTER1		0x03
//#define		ID_REGESTER2		0x04
//#define		ID_PAY_MONEY1		0x05
//#define		ID_PAY_MONEY2		0x06
//#define		ID_OWNER_BIND		0x07


#define		ID_EVENT_BUTTON_WIFI_LIST		(uint32_t)(1<<0)
#define		ID_EVENT_BUTTON_WIFI_LINK		(uint32_t)(1<<1)
#define		ID_EVENT_BUTTON_REGESTER1		(uint32_t)(1<<2)
#define		ID_EVENT_BUTTON_REGESTER2		(uint32_t)(1<<3)
#define		ID_EVENT_BUTTON_PAY_MONEY1		(uint32_t)(1<<4)
#define		ID_EVENT_BUTTON_PAY_MONEY2		(uint32_t)(1<<5)
#define		ID_EVENT_BUTTON_OWNER_BIND		(uint32_t)(1<<6)


#define     ID_TASK_MAIN                     0x01
#define     ID_TASK_PAY_NUM                  0x02
#define     ID_TASK_PAY_FP                   0x03
#define     ID_TASK_PAY_UDID                 0x04
#define     ID_TASK_USER_REGISTER_PHONE      0x05
#define     ID_TASK_USER_REGISTER_CHECKNUM   0x06
#define     ID_TASK_USER_REGISTER_FP1        0x07
#define     ID_TASK_USER_REGISTER_FP2        0x08                   
#define     ID_TASK_USER_REGISTER_FP3        0x09                    
#define     ID_TASK_OWNER_REGISTER_PHONE     0x0A
#define     ID_TASK_OWNER_REGISTER_CHECKNUM  0x0B
#define     ID_TASK_OWNER_REGISTER_FP1       0x0C
#define     ID_TASK_OWNER_REGISTER_FP2       0x0D                 
#define     ID_TASK_OWNER_REGISTER_FP3       0x0E
#define     ID_TASK_LIST_WIFI                0x0F
#define     ID_TASK_JOAP_WIFI                0x10
#define     ID_TASK_NUM_KEYBOARD             0x15
#define     ID_TASK_ABC_KEYBOARD             0x16
#define     ID_TASK_SYSNC_TIME               0x17
#define     ID_TASK_SEND_FP                  0x18
#define     ID_TASK_SEND_TICK                0x19
#define     ID_MSG_SWITCH_KEYBOARD           0x22

#define     KEYBOARD_ABC                     0x11
#define     KEYBOARD_NUM                     0x22

#define     SEND_TO_SERVE                    0x01
#define     RCV_DEVICE                       0x04
#define     RECV_SERVE                       0x18
#define     RETURN_FROM_WIN                  0x14

#define  TASK_MAIN              0x01
#define  TASK_PAY               0x02
#define  TASK_USER_REGIST       0x03
#define  TASK_OWNER_REGIST      0x04

typedef struct date_time
{
    rt_uint8_t year;
    rt_uint8_t month;
    rt_uint8_t day;
    rt_uint8_t hour;
    rt_uint8_t minute;
    rt_uint8_t second;
}date_time_t;

typedef union union_date_time
{
    date_time_t time;
    rt_uint8_t  buf[6];
}union_date_time_t;

typedef struct win_msg
{
	char     id_win;
    char     id_operate;	
	char     buf[512];	
}win_msg_t;

typedef union win_msg_union
{   win_msg_t  msg;
    char       buf[514];   
}win_msg_union_t;

#define RT_APP_MSG_LEN 			sizeof(win_msg_t)*3
//extern int IS_WIN_MAIN_FLAG;
extern struct rt_messagequeue wmq;
extern struct rt_event wevent;
extern struct rt_mutex wifi_send_mutex;
extern const int lzt_dev_id;
extern BUTTON_SKINFLEX_PROPS default_Props;
extern GUI_CONST_STORAGE GUI_FONT GUI_Font_song16, GUI_Font_kai20S, GUI_Font_kai24;
extern GUI_CONST_STORAGE GUI_BITMAP bm_loginS, bm_shake,button_zw_pay, bmzwzc,bmid,bmlogo;
extern GUI_CONST_STORAGE GUI_BITMAP bmnowifi,bmwifi1,bmwifi2,bmwifi3,bmwifi4,bmwelcome,bmfingerbitmap,bmzw100;
extern const GUI_BITMAP* WIFI[];
extern wifi_list_t wifi_list_AP[];
extern wifi_list_t  connected_ap;
extern rt_size_t battery_capity_bar;
extern struct rt_timer app_time;
extern enum LINK_STATUS wifi_link_status;
extern date_time_t runing_time;

extern char fp_status_buf[];
//extern struct rt_thread emwin_thread;
//extern struct rt_thread background_thread;
//extern struct rt_thread updater_thread;;

void wifi_jap_status(void);





WM_HWIN keyboard_num(WM_HWIN hwin);
WM_HWIN keyboard_a(WM_HWIN hwin);
void button_set_BKCOLOR(I16 statue,I32 color);
rt_err_t get_frame_buf(sever_frame_package_t *send,char *buf,rt_size_t len);
void run_to_win(rt_uint8_t  WIN_ID);
void run_to_jap_wifi(char *buffer);
rt_err_t send_frame_package(rt_uint8_t cmd,rt_uint8_t desc,char *args,rt_size_t data_len);

void app_timeout_callbak(void* parameter);
WM_HWIN  gui_msgbox(const char *s,WM_HWIN hWin);
WM_HWIN Createmain(void);
WM_HWIN Createwifi_list(void);
WM_HWIN Createdenglu(void);
WM_HWIN CreatenotificationBar(void);
WM_HWIN Createwifi_jap(char *buffer);
WM_HWIN Creat_owner_phone_input(void);
WM_HWIN Creat_user_phone_input(void);
WM_HWIN Creat_owner_validation_input(void);
WM_HWIN Creat_user_validation_input(void);
WM_HWIN Creat_Pay_input(void);
WM_HWIN Creat_Pay_UDID(void);

WM_HWIN CreateFp_regist1(void);
WM_HWIN CreateFp_regist2(void);
WM_HWIN CreateFp_regist3(void);
WM_HWIN Createmain(void);
int emwin_system_init(void);

#endif
