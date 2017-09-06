
//#include "includes.h"
#include "keyboard.h"
#include "GUI.h"
#include "string.h"
#include "app.h"
//#include "pysearch.h"


/*
*********************************************************************************************************
*	                         句柄和字体变量
*********************************************************************************************************
*/
//TEXT_Handle ahText[2];
//MULTIEDIT_HANDLE hMulti;    
//GUI_FONT SIF_Font;  /* 使用SIF字体方式 */

/*
*********************************************************************************************************
*	                         静态变量
*********************************************************************************************************
*/
static unsigned char s_SelectSymbol = 1;	/* 0表示t特殊字符，1表英文 */
static unsigned char s_CapsLock = 0;      /* 0表示小写，1表示大写 */


static unsigned char s_Key;
char s_Textbuf[10]={0};
//extern int Keyboard_switch_flag;
//unsigned char s_tempbuf[256]={0};
//char s_Hzbuf[512]={0};
//char s_Bufview[128]={0};

//static unsigned int   s_Offset = 0;
//static unsigned int  s_Hznum;          
//static unsigned int  s_Hzoff;

/*
*********************************************************************************************************
*	                         宏定义和结构体
*********************************************************************************************************
*/

#define BUTTON_SKINFLEX_RADIUS 4
#define ID_BUTTON              (GUI_ID_USER + 0x30)
#define APP_INIT_LOWERCASE     (WM_USER + 0)
static void _cbKeyPad(WM_MESSAGE * pMsg);
typedef struct {
  int          xPos;
  int          yPos;
  int          xSize;
  int          ySize;
  const char * acLabel;   /* 按钮对应的小写字符 */
  const char * acLabel1;  /* 按钮对应的大写字符 */
	const char * acLabel2;   /*按钮对应的特殊字符*/
  char         Control;   /* 按钮对应的控制位，1表示功能按钮，0表示普通按钮 */
} BUTTON_DATA;

typedef struct {
  int          xPos;
  int          yPos;
  int          xSize;
  int          ySize;
  const char * acLabel;   /* 按钮对应字符 */
  char         Control;   /* 按钮对应的控制位，1表示功能按钮，0表示普通按钮 */
}NUM_BUTTON_DATA;

/*
*********************************************************************************************************
*	                         宏定义和结构体
*********************************************************************************************************
*/
#define BUTTON_WHITH  24
#define BUTTON_HIGHT  43
#define BUTTON_StartX 23
#define BUTTON_HIGHTX 43

#define NUM_BUTTON_WHITH  50
#define NUM_ABC_BUTTON_WHITH  65
#define NUM_BUTTON_HIGHT  43
static const BUTTON_DATA _aButtonData[] = 
{
	/* 第1排按钮 */
	//{   5,                   BUTTON_HIGHTX,  BUTTON_WHITH, BUTTON_HIGHT, "Tab", "Tab", 1 },
	{ 5,                  	0,  BUTTON_WHITH, BUTTON_HIGHT, "q", "Q", "#",0},
	{ 5+BUTTON_StartX,   	  0,  BUTTON_WHITH, BUTTON_HIGHT, "w", "W", ".",0 },
	{ 5+BUTTON_StartX*2,    0,  BUTTON_WHITH, BUTTON_HIGHT, "e", "E", "?",0 },
	{ 5+BUTTON_StartX*3,   	0,  BUTTON_WHITH, BUTTON_HIGHT, "r", "R", "!",0 },
	{ 5+BUTTON_StartX*4,   	0,  BUTTON_WHITH, BUTTON_HIGHT, "t", "T", ":",0 },
	{ 5+BUTTON_StartX*5,   	0,  BUTTON_WHITH, BUTTON_HIGHT, "y", "Y", "/",0 },
	{ 5+BUTTON_StartX*6,   	0,  BUTTON_WHITH, BUTTON_HIGHT, "u", "U", "@",0 },
	{ 5+BUTTON_StartX*7,   	0,  BUTTON_WHITH, BUTTON_HIGHT, "i", "I", "}",0 },
	{ 5+BUTTON_StartX*8,   	0,  BUTTON_WHITH, BUTTON_HIGHT, "o", "O", ";",0 },
	{ 5+BUTTON_StartX*9,  	0,  BUTTON_WHITH, BUTTON_HIGHT, "p", "P", "[",0 },

	/* 第2排按钮 */
	{ 16,                  3+BUTTON_HIGHTX,  BUTTON_WHITH, BUTTON_HIGHT, "a", "A","~", 0 },
	{ 16+BUTTON_StartX,    3+BUTTON_HIGHTX,  BUTTON_WHITH, BUTTON_HIGHT, "s", "S", "(",0 },
	{ 16+BUTTON_StartX*2,  3+BUTTON_HIGHTX,  BUTTON_WHITH, BUTTON_HIGHT, "d", "D", ")",0 },
	{ 16+BUTTON_StartX*3,  3+BUTTON_HIGHTX,  BUTTON_WHITH, BUTTON_HIGHT, "f", "F", "<",0 },
	{ 16+BUTTON_StartX*4,  3+BUTTON_HIGHTX,  BUTTON_WHITH, BUTTON_HIGHT, "g", "G", ">",0 },
	{ 16+BUTTON_StartX*5,  3+BUTTON_HIGHTX,  BUTTON_WHITH, BUTTON_HIGHT, "h", "H","*", 0 },
	{ 16+BUTTON_StartX*6,  3+BUTTON_HIGHTX,  BUTTON_WHITH, BUTTON_HIGHT, "j", "J","&", 0 },
	{ 16+BUTTON_StartX*7,  3+BUTTON_HIGHTX,  BUTTON_WHITH, BUTTON_HIGHT, "k", "K","]", 0 },
	{ 16+BUTTON_StartX*8,  3+BUTTON_HIGHTX,  BUTTON_WHITH, BUTTON_HIGHT, "l", "L","{", 0 },
	//{ 16+BUTTON_StartX*9,  33+BUTTON_HIGHTX,  BUTTON_WHITH,  BUTTON_HIGHT, "Enter","Enter", 1},

	/* 第3排按钮 */
	{ 6,                    6+BUTTON_HIGHTX*2,  BUTTON_WHITH+10, BUTTON_HIGHT, "ABC", "abc","abc", 1},
	{ 16+BUTTON_StartX*1,   6+BUTTON_HIGHTX*2,  BUTTON_WHITH, BUTTON_HIGHT, "z", "Z", "$", 0 },
	{ 16+BUTTON_StartX*2,   6+BUTTON_HIGHTX*2,  BUTTON_WHITH, BUTTON_HIGHT, "x", "X", "%", 0 },
	{ 16+BUTTON_StartX*3,   6+BUTTON_HIGHTX*2,  BUTTON_WHITH, BUTTON_HIGHT, "c", "C", "^",0 },
	{ 16+BUTTON_StartX*4,   6+BUTTON_HIGHTX*2,  BUTTON_WHITH, BUTTON_HIGHT, "v", "V", "_",0 },
	{ 16+BUTTON_StartX*5,   6+BUTTON_HIGHTX*2,  BUTTON_WHITH, BUTTON_HIGHT, "b", "B", "+", 0 },
	{ 16+BUTTON_StartX*6,   6+BUTTON_HIGHTX*2,  BUTTON_WHITH, BUTTON_HIGHT, "n", "N", "-",0 },
	{ 16+BUTTON_StartX*7,   6+BUTTON_HIGHTX*2,  BUTTON_WHITH, BUTTON_HIGHT, "m", "M", "=",0 },
	{ 16+BUTTON_StartX*8,   6+BUTTON_HIGHTX*2,  BUTTON_WHITH+10, BUTTON_HIGHT, "Del", "Del","Del", 0 },
	//{ 6+BUTTON_StartX*9,   67+BUTTON_HIGHTX,  BUTTON_WHITH, BUTTON_HIGHT, ".", ".", 0 },
	//{ 6+BUTTON_StartX*10,  67+BUTTON_HIGHTX,  BUTTON_WHITH, BUTTON_HIGHT, "?", "?", 0 },

	/* 第4排按钮 */
	 { 4,     9+BUTTON_HIGHTX*3,  58,  BUTTON_HIGHT, "Symbol", "Symbol","Symbol", 1},
	 { 67,    9+BUTTON_HIGHTX*3,  50,  BUTTON_HIGHT, "123",   "123",  "123", 1},
	 { 123,   9+BUTTON_HIGHTX*3,  50,  BUTTON_HIGHT, " ",     " ",    " ",   0},
   { 178,   9+BUTTON_HIGHTX*3,  58,  BUTTON_HIGHT, "Enter",  "Enter","Enter",1},
};

static const NUM_BUTTON_DATA _num_ButtonData[] = 
{
	/* 第1排按钮 */
	{ 5,                          0,          			 NUM_BUTTON_WHITH, NUM_BUTTON_HIGHT, "1", 0},
	{ 5*2+NUM_BUTTON_WHITH,   	  0,  							 NUM_BUTTON_WHITH, NUM_BUTTON_HIGHT, "2", 0},
	{ 5*3+NUM_BUTTON_WHITH*2,     0,  							 NUM_BUTTON_WHITH, NUM_BUTTON_HIGHT, "3", 0},
	{ 5*4+NUM_BUTTON_WHITH*3,   	0,  							 NUM_ABC_BUTTON_WHITH, NUM_BUTTON_HIGHT, "回删",1},
	/* 第2排按钮 */
	{ 5,                          5+NUM_BUTTON_HIGHT*1,  NUM_BUTTON_WHITH, NUM_BUTTON_HIGHT, "4", 0},
	{ 5*2+NUM_BUTTON_WHITH,   	  5+NUM_BUTTON_HIGHT*1,  NUM_BUTTON_WHITH, NUM_BUTTON_HIGHT, "5", 0},
	{ 5*3+NUM_BUTTON_WHITH*2,     5+NUM_BUTTON_HIGHT*1,  NUM_BUTTON_WHITH, NUM_BUTTON_HIGHT, "6", 0},
	{ 5*4+NUM_BUTTON_WHITH*3,   	5+NUM_BUTTON_HIGHT*1,NUM_ABC_BUTTON_WHITH, NUM_BUTTON_HIGHT, "abc", 1},
	//{ 16+BUTTON_StartX*9,  33+BUTTON_HIGHTX,  BUTTON_WHITH,  BUTTON_HIGHT, "Enter","Enter", 1},

	/* 第3排按钮 */
	{ 5,                          10+NUM_BUTTON_HIGHT*2,  NUM_BUTTON_WHITH, NUM_BUTTON_HIGHT, "7", 0},
	{ 5*2+NUM_BUTTON_WHITH,   	  10+NUM_BUTTON_HIGHT*2,  NUM_BUTTON_WHITH, NUM_BUTTON_HIGHT, "8", 0},
	{ 5*3+NUM_BUTTON_WHITH*2,     10+NUM_BUTTON_HIGHT*2,  NUM_BUTTON_WHITH, NUM_BUTTON_HIGHT, "9", 0},
	{ 5*4+NUM_BUTTON_WHITH*3,   	10+NUM_BUTTON_HIGHT*2, NUM_ABC_BUTTON_WHITH, NUM_BUTTON_HIGHT*2+5, "确认",1},
	/* 第4排按钮 */
	{ 5,                          15+NUM_BUTTON_HIGHT*3,  NUM_BUTTON_WHITH, NUM_BUTTON_HIGHT, "00", 0},
	{ 5*2+NUM_BUTTON_WHITH,   	  15+NUM_BUTTON_HIGHT*3,  NUM_BUTTON_WHITH, NUM_BUTTON_HIGHT, "0", 0},
	{ 5*3+NUM_BUTTON_WHITH*2,     15+NUM_BUTTON_HIGHT*3,  NUM_BUTTON_WHITH, NUM_BUTTON_HIGHT, ".", 0},
	//{ 8*4+NUM_BUTTON_WHITH*3,   	15+NUM_BUTTON_HIGHT*1,  NUM_BUTTON_WHITH, NUM_BUTTON_HIGHT, "abc", 0},
};


/*
*********************************************************************************************************
*	函 数 名: _cb_num_KeyPad
*	功能说明: 回调函数
*	形    参：pMsg  指针参数
*	返 回 值: 无
*********************************************************************************************************
*/
static void _cb_num_KeyPad(WM_MESSAGE * pMsg) 
{
	WM_HWIN    hWin;
	WM_HWIN    hButton;
	int        Id;
	int        NCode;
	int        xSize;
	int        ySize;
	unsigned   i;

	hWin = pMsg->hWin;
	switch (pMsg->MsgId) 
	{
		/* 创建所需的按钮 */
		case WM_CREATE:
			for (i = 0; i < GUI_COUNTOF(_num_ButtonData); i++) 
			{
                BUTTON_SetDefaultSkin(BUTTON_SKIN_FLEX);
                BUTTON_SetSkinFlexProps(&default_Props, BUTTON_SKINFLEX_PI_ENABLED); 
				hButton = BUTTON_CreateEx(_num_ButtonData[i].xPos, _num_ButtonData[i].yPos, _num_ButtonData[i].xSize, _num_ButtonData[i].ySize, 
				hWin, WM_CF_SHOW, 0, ID_BUTTON + i);
				if((3 == i)||(11 == i))
                    BUTTON_SetFont(hButton, &GUI_Font_song16);
                else
                    BUTTON_SetFont(hButton, &GUI_Font24_ASCII);
				//BUTTON_SetFont(hButton, &GUI_Font24_ASCII);
				BUTTON_SetText(hButton, _num_ButtonData[i].acLabel);
				BUTTON_SetFocussable(hButton, 0);
			}
			break;
		
		/* 绘制背景 */	
		case WM_PAINT:	
			xSize = WM_GetWindowSizeX(hWin);
			ySize = WM_GetWindowSizeY(hWin);
			GUI_SetColor(COLOR_BORDER);
			GUI_DrawRect(0, 0, xSize - 1, ySize - 1);
			GUI_DrawGradientV(1, 1, xSize - 2, ySize - 2, COLOR_KEYPAD0, COLOR_KEYPAD1);
			break;
		
		/* 用于处理按钮的消息 */
		case WM_NOTIFY_PARENT:
			Id    = WM_GetId(pMsg->hWinSrc);
			NCode = pMsg->Data.v;
			switch (NCode) 
			{
				case WM_NOTIFICATION_RELEASED:
					if (_num_ButtonData[Id - ID_BUTTON].acLabel) 
					{
						/* 第一步：键盘的切换 *******************************************/
						if(strncmp((const char *)_num_ButtonData[Id - ID_BUTTON].acLabel, "abc", sizeof(_num_ButtonData[Id - ID_BUTTON].acLabel)) == 0)
						{
                            //hWinParent = WM_GetParent(hWin);
							                           
							s_CapsLock = 0;
							s_SelectSymbol = 0;
                            rt_uint8_t msg[4]={0};
                            //rt_uint8_t wifi_image_id = 55;                          
                            msg[1] = RCV_DEVICE;
                            msg[0] = ID_MSG_SWITCH_KEYBOARD;
                            msg[2] = KEYBOARD_ABC;
                            rt_mq_send(&wmq,msg,4);
							break;		
						}
						{
							/* 点击回车键 */
							if(strncmp((const char *)_num_ButtonData[Id - ID_BUTTON].acLabel, "确认", sizeof(_num_ButtonData[Id - ID_BUTTON].acLabel)) == 0)
							{
								//EDIT_AddKey(hMulti, GUI_KEY_ENTER);
								GUI_SendKeyMsg(GUI_KEY_ENTER,1);
                                //WM_DeleteWindow(HwinCurr_Keyboard);
								break;
							}
							
							/* 点击回格键 */
							if(strncmp((const char *)_num_ButtonData[Id - ID_BUTTON].acLabel, "回删", sizeof(_num_ButtonData[Id - ID_BUTTON].acLabel)) == 0)
							{
								//EDIT_AddKey(hMulti, GUI_KEY_BACKSPACE);
								GUI_SendKeyMsg(GUI_KEY_BACKSPACE,1);
                                
								break;
							}
							/* 其它按键，但不能是功能键 */
							if(_num_ButtonData[Id - ID_BUTTON].Control == 0)
							{	
								if(0 == strncmp((const char *)_num_ButtonData[Id - ID_BUTTON].acLabel, "00", sizeof(_num_ButtonData[Id - ID_BUTTON].acLabel)))
								{
									s_Textbuf[0] = _num_ButtonData[Id - ID_BUTTON].acLabel[0];
									s_Textbuf[1] = _num_ButtonData[Id - ID_BUTTON].acLabel[1];
									s_Textbuf[2] = '\0';
									GUI_StoreKeyMsg(s_Textbuf[0],1);
									GUI_StoreKeyMsg(s_Textbuf[1],1);
									//EDIT_AddKey(hMulti, s_Textbuf[0]);
									//EDIT_AddKey(hMulti, s_Textbuf[1]);
									//EDIT_AddTxt(hMulti, s_Textbuf);
								}
                                else								
								{
									s_Textbuf[0] = _num_ButtonData[Id - ID_BUTTON].acLabel[0];
									s_Textbuf[1] = '\0';
									//EDIT_AddKey(hMulti, s_Textbuf[0]);
									GUI_StoreKeyMsg(s_Textbuf[0],1);									
								}
							}
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


/*
*********************************************************************************************************
*	函 数 名: _cbKeyPad
*	功能说明: 回调函数
*	形    参：pMsg  指针参数
*	返 回 值: 无
*********************************************************************************************************
*/
static void _cbKeyPad(WM_MESSAGE * pMsg) 
{
	WM_HWIN    hWin;
	WM_HWIN    hButton;
	int        Id;
	int        NCode;
	int        xSize;
	int        ySize;
	unsigned   i;

	hWin = pMsg->hWin;
	switch (pMsg->MsgId) 
	{
		/* 创建所需的按钮 */
		case WM_CREATE:
			for (i = 0; i < GUI_COUNTOF(_aButtonData); i++) 
			{
                BUTTON_SetSkinFlexProps(&default_Props, BUTTON_SKINFLEX_PI_ENABLED); 
                BUTTON_SetDefaultSkin(BUTTON_SKIN_FLEX);
				hButton = BUTTON_CreateEx(_aButtonData[i].xPos, _aButtonData[i].yPos, _aButtonData[i].xSize, _aButtonData[i].ySize, 
				hWin, WM_CF_SHOW, 0, ID_BUTTON + i);
                if(11 == i)
                    BUTTON_SetFont(hButton, &GUI_Font16B_ASCII);
                else
                    BUTTON_SetFont(hButton, &GUI_Font16B_ASCII);
				BUTTON_SetText(hButton, _aButtonData[i].acLabel);
				BUTTON_SetFocussable(hButton, 0);
			}
			break;
		
		/* 绘制背景 */	
		case WM_PAINT:	
			xSize = WM_GetWindowSizeX(hWin);
			ySize = WM_GetWindowSizeY(hWin);
			GUI_SetColor(COLOR_BORDER);
			GUI_DrawRect(0, 0, xSize - 1, ySize - 1);
			GUI_DrawGradientV(1, 1, xSize - 2, ySize - 2, COLOR_KEYPAD0, COLOR_KEYPAD1);
			break;
		
		/* 用于处理按钮的消息 */
		case WM_NOTIFY_PARENT:
			Id    = WM_GetId(pMsg->hWinSrc);
			NCode = pMsg->Data.v;
			switch (NCode) 
			{
				case WM_NOTIFICATION_RELEASED:
					if (_aButtonData[Id - ID_BUTTON].acLabel) 
					{
						/* 第一步：实现字母大小写的切换 *******************************************/
						if(strncmp((const char *)_aButtonData[Id - ID_BUTTON].acLabel, "ABC", sizeof(_aButtonData[Id - ID_BUTTON].acLabel)) == 0)
						{
							/* 当前是小写，切换到大写，必须在英文模式下实现大小写切换*/							
                            if(s_CapsLock == 0)
                            {
                                for (i = 0; i < GUI_COUNTOF(_aButtonData); i++) 
                                {
                                    BUTTON_SetText(WM_GetDialogItem(hWin, ID_BUTTON + i),  _aButtonData[i].acLabel1);
                                }
                                s_CapsLock = 1;
                                s_SelectSymbol = 0;
                            }
                            else
                            {
                                for (i = 0; i < GUI_COUNTOF(_aButtonData); i++) 
                                {
                                    BUTTON_SetText(WM_GetDialogItem(hWin, ID_BUTTON + i), _aButtonData[i].acLabel);
                                }
                                s_CapsLock = 0;
                                s_SelectSymbol = 0;
									}
						/* 功能键处理完要退出 */
							break;			
						}	
                        else if(strncmp((const char *)_aButtonData[Id - ID_BUTTON].acLabel, "Symbol", sizeof(_aButtonData[Id - ID_BUTTON].acLabel)) == 0)
						{
							/*1特殊符号，0英文*/
							if(0 == s_SelectSymbol)
							{
								for (i = 0; i < GUI_COUNTOF(_aButtonData); i++) 
								{
									BUTTON_SetText(WM_GetDialogItem(hWin, ID_BUTTON + i), _aButtonData[i].acLabel2);
								}
								s_CapsLock = 0;
								s_SelectSymbol = 1;
							}
							else
							{
								for (i = 0; i < GUI_COUNTOF(_aButtonData); i++) 
								{
									BUTTON_SetText(WM_GetDialogItem(hWin, ID_BUTTON + i), _aButtonData[i].acLabel);
								}
								s_CapsLock = 0;
								s_SelectSymbol = 0;
							}
							/* 功能键处理完要退出 */
							break;
						}
						
						/* 第三步：字母大小写的识别 ********************************************/
						if(1 == s_SelectSymbol)
						{
							if(s_CapsLock == 0)
							{
								s_Key = _aButtonData[Id - ID_BUTTON].acLabel[0];
							}
							/* 大写字母 */
							else
							{
								s_Key = _aButtonData[Id - ID_BUTTON].acLabel1[0];
							}
                        }
                        else
                        {
                            s_Key = _aButtonData[Id - ID_BUTTON].acLabel2[0];
                        }        
                        {
                            /* 点击回车键 */
                            if(strncmp((const char *)_aButtonData[Id - ID_BUTTON].acLabel, "Enter", sizeof(_aButtonData[Id - ID_BUTTON].acLabel)) == 0)
                            {
                                //GUI_SendKeyMsg(Key, Pressed);
                                //EDIT_AddKey(hMulti, GUI_KEY_ENTER);
                                GUI_SendKeyMsg(GUI_KEY_ENTER,1);
                                //WM_DeleteWindow(HwinCurr_Keyboard);
                                //WM_SendMessage();
                                break;
                            }
                            
                            /* 点击回格键 */
                            if(strncmp((const char *)_aButtonData[Id - ID_BUTTON].acLabel, "Del", sizeof(_aButtonData[Id - ID_BUTTON].acLabel)) == 0)
                            {
                                //EDIT_AddKey(hMulti, GUI_KEY_BACKSPACE);
                                GUI_SendKeyMsg(GUI_KEY_BACKSPACE,1);
                                break;
                            }
                            if(strncmp((const char *)_aButtonData[Id - ID_BUTTON].acLabel, "123", sizeof(_aButtonData[Id - ID_BUTTON].acLabel)) == 0)
                            {
                                rt_uint8_t msg[4]={0};
                                //rt_uint8_t wifi_image_id = 55;                          
                                msg[1] = RCV_DEVICE;
                                msg[0] = ID_MSG_SWITCH_KEYBOARD;
                                msg[2] = KEYBOARD_NUM;
                                rt_mq_send(&wmq,msg,4);
                                //WM_CreateWindowAsChild(0, 130, 240, 190, hWinParent, WM_CF_SHOW | WM_CF_STAYONTOP, _cb_num_KeyPad, 0);
                                //MULTIEDIT_AddKey(hMulti, GUI_KEY_BACKSPACE);
                                break;
                            }


                            /* 其它按键，但不能是功能键 */
                            if(_aButtonData[Id - ID_BUTTON].Control == 0)
                            {
                                s_Textbuf[0] = s_Key;
                                s_Textbuf[1] = '\0';
                                //EDIT_AddKey(hMulti, s_Textbuf[0]);
                                GUI_StoreKeyMsg(s_Key,1);
                                //WM_SendMessage(hMulti,s_Textbuf);
                            }
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
/*
*********************************************************************************************************
*	函 数 名: MainTask
*	功能说明: 检索拼音的函数,使用很简单，只需输入拼音就行返回这个拼音对应的汉字个数。
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/

WM_HWIN keyboard_num(WM_HWIN hwin)
{
    //BUTTON_SetSkinFlexProps(&default_Props, BUTTON_SKINFLEX_PI_ENABLED);
    WM_HWIN  Hwin_keyboard ;  
	Hwin_keyboard =  WM_CreateWindowAsChild(0, 130, 240, 190, hwin,WM_CF_SHOW , _cb_num_KeyPad, 0);//| WM_CF_STAYONTOP
    WM_DisableMemdev(Hwin_keyboard);
    //WM_EnableMemdev(Hwin_keyboard);
    return Hwin_keyboard;
}
WM_HWIN keyboard_a(WM_HWIN hwin)
{
    //BUTTON_SetSkinFlexProps(&default_Props, BUTTON_SKINFLEX_PI_ENABLED);  
	WM_HWIN  Hwin_keyboard ;  
	Hwin_keyboard =  WM_CreateWindowAsChild(0, 130, 240, 190, hwin, WM_CF_SHOW | WM_CF_STAYONTOP, _cbKeyPad, 0);
    s_SelectSymbol = 1;
    s_CapsLock = 0;
    WM_DisableMemdev(Hwin_keyboard);
   // WM_EnableMemdev(Hwin_keyboard);
    return Hwin_keyboard;
}

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
