/************************************************* 
  Copyright (C), 2012, NJUT
  File name:      gprsio.c
  Author:  sundm     Version:  1.0      Date: 2013.1.3 
  Description:    sundm GPRS模块驱动接口函数 
  Others:        采用底层驱动，填充函数  
  Function List:  
*************************************************/ 

#ifndef _ESP8266_H_
#define _ESP8266_H_

#include <rtthread.h>
#include "stm32f1xx.h"

//#define ESP8266_RCC_GPIO_CLK_ENABLE()  	__HAL_RCC_GPIOB_CLK_ENABLE()
//#define ESP8266_CS_PORT              	GPIOB
//#define ESP8266_CS_PIN               	GPIO_PIN_4

#define PB4 					  90 // PB4 Pin is NO. 90 @STM32F103VET6

#define	ESP8266_CS_Init() 		rt_pin_mode(PB4, PIN_MODE_OUTPUT)
#define ESP8266_CS_LOW()		rt_pin_write(PB4,PIN_LOW)
#define ESP8266_CS_HIGH()		rt_pin_write(PB4,PIN_HIGH)



//定义AT指令

#define WIFI_LIST_AP_MAX 				5

#define WIFI_LIST   				0x20
#define WIFI_JAP    				0x21
#define WIFI_QAP    				0x22
#define WIFI_SEND_DATA   		    0x23
#define WIFI_JAP_STATUS 		    0x24
#define WIFI_CONNECT_SEVER 	        0x25


//typedef int LINK_STATUS;


//#define JAP_FAL 1
//#define JAP_OK  2
//#define CONNECT_SEVER_FAL  3
//#define CONNECT_SEVER_OK  4

#define CMD_SEND_FP 					0x03	//閲囬泦鎸囩汗淇℃伅
#define CMD_SEND_USER_PHONE_NUM 		0x04	//鍙戦�鎵嬫満鍙
#define CMD_SEND_USER_PHONE_CHECK		0x07	//鍙戦�楠岃瘉鐮
#define CMD_PAY_NUM 					0x08	//璁惧鍙戦�娑堣垂閲戦
#define CMD_SEND_UDID 					0x09	//鍙戦�韬唤璇嗗埆鐮
#define CMD_SEND_TICK 					0x0A	//心跳
#define CMD_SYSNC_TIME          		0x0E	////鏃堕棿鏍″
#define CMD_SEND_OWNER_PHONE_NUM  		0xE8	//鍙戦�佺粦瀹氭墜鏈哄彿
#define CMD_SEND_OWNER_PHONE_CHECK 		0xEB	//鍙戦�佺粦瀹氶獙璇佺爜
#define CMD_UPDATA 						0x0F	//杩滅▼鍗囩骇
#define CMD_DEV_WIFI                    0x10
#define CMD_DEV_FP                      0x11
	
#if 1
#define ESP8266_ATCMD "AT\x0D\x0A"               // AT查询
#define ESP8266_RESET "AT+RST\x0D\x0A"           // 模块复位

#define ESP8266_CWMODE_STA "AT+CWMODE=1\x0D\x0A"      // 选择WiFi应用模式  Station模式
#define ESP8266_CWMODE_AP "AT+CWMODE=2\x0D\x0A"      // 选择WiFi应用模式  AP模式
#define ESP8266_CWMODE_APSTA "AT+CWMODE=3\x0D\x0A"      // 选择WiFi应用模式  Station+AP模式

#define ESP8266_CWLAP "AT+CWLAP\x0D\x0A"      // 列出当前接入点
#define ESP8266_CWQAP "AT+CWQAP\x0D\x0A"      // 退出
#define ESP8266_CIFSR "AT+CIFSR\x0D\x0A"      // 获取本地IP地址
//#define ESP8266_CWJAP "AT+CWJAP=\"lzt02\",\"lzt123456\"\x0D\x0A"

#define ESP8266_CWJAP "AT+CWJAP=\"lzt02\",\"lzt123456\"\x0D\x0A"      // 加入接入点 TP-LINK_sundm
#define ESP8266_CIPEXITSERVER "AT+CIPSERVER=0\x0D\x0A"      // 退出server
#define ESP8266_CIPMUX "AT+CIPMUX=0\x0D\x0A"      // 设置单连接
#define ESP8266_CIPMODE "AT+CIPMODE=1\x0D\x0A"      // 设置透传模式

#define ESP8266_CIPSTART "AT+CIPSTART=\"TCP\",\"192.168.2.102\",8080\x0D\x0A"      // 建立TCP/UDP连接
#define ESP8266_CIPSTATUS "AT+CIPSTATUS\x0D\x0A"      // 获得TCP/UDP连接状态
#define ESP8266_CIP_JAP_STATUS "AT+CWJAP?\x0D\x0A"
#define ESP8266_CIPSEND "AT+CIPSEND\x0D\x0A"      // 发送数据

#define ESP8266_CIPCLOSE "AT+CIPCLOSE\x0D\x0A"      // 关闭TCP/UDP连接
#define ESP8266_CIPMODE_CLOSED "+++"      // 关闭透传
#define ESP8266_CWLAPOPT "AT+CWLAPOPT=0,7\x0D\x0A"                   //设置cwlap的显示属性 1(0)(不)根据信号强度排序(bit6)(bit5)(bit4)ch(bit3)mac(bit2)信号强度(bit1)ssid(bit0)加密方式
#endif

enum LINK_STATUS
{
	WIFI_LINK_JAP_FAILD = 1,
	WIFI_LINK_JAP_OK,
	WIFI_LINK_CONNECT_SEVER_FAILD,
	WIFI_LINK_CONNECT_SEVER_OK
};

typedef struct link_ip
{
    char IP1[4];
    char IP2[4];
    char IP3[4];
    char IP4[4];
    char PORT[6];
}link_ip_t;	
	
typedef struct sever_frame_package
{
		rt_uint16_t frame_head;
		rt_uint16_t frame_data_len;
	    rt_uint32_t  id;
		char  SEQ;	 /* 搴忓彿榛樿01 */      
		char  DESC;  /* 浠呴噰闆嗘寚绾逛俊鎭娇鐢紙1-绗竴娆￠噰闆嗭紱2-绗簩娆￠噰闆嗘寚绾逛俊鎭紱3-绗笁娆℃寚绾归噰闆嗕俊鎭級榛樿0 */
		char  CMD;   /* 娑堟伅绫诲瀷 */
		char        *pdata;
	    rt_uint8_t   check;// 浠庢暟鎹暱搴﹀紑濮嬪埌鏍￠獙鐮佷箣鍓嶇殑鏁版嵁鐨勫紓鎴栨牎楠/
}sever_frame_package_t;

typedef struct sever_frame_package_char
{
		char frame_head[2];
		char frame_data_len[2];
	    char  id[3];
		char  SEQ;	 /* 搴忓彿榛樿01 */      
		char  DESC;  /* 浠呴噰闆嗘寚绾逛俊鎭娇鐢紙1-绗竴娆￠噰闆嗭紱2-绗簩娆￠噰闆嗘寚绾逛俊鎭紱3-绗笁娆℃寚绾归噰闆嗕俊鎭級榛樿0 */
		char  CMD;   /* 娑堟伅绫诲瀷 */
		char        *pdata;
	    rt_uint8_t   check;// 浠庢暟鎹暱搴﹀紑濮嬪埌鏍￠獙鐮佷箣鍓嶇殑鏁版嵁鐨勫紓鎴栨牎楠/
}sever_frame_package_char_t;

typedef union frame_package
{
      sever_frame_package_char_t   frame;
      char 					       buf[16];
}frame_package_t;
	
union link_sever_ip
{
    link_ip_t ip;
    char      ip_buf[22];  
};

typedef struct wifi_list
{
	char ecn[1];
	char ssid[32];
	int  rssi;
}wifi_list_t;

extern wifi_list_t  wifi_list_AP[WIFI_LIST_AP_MAX];
extern wifi_list_t  connected_ap;

rt_err_t send_frame_package(rt_uint8_t cmd,rt_uint8_t desc,char *args,rt_size_t data_len);

#endif
