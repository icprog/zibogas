/******************************************************************************
Copyright 2013 大连易达通电子有限公司
All rights riserved.

文件名	：main.c
模块名称：淄博加油POS机主程序
功能概要：淄博加油POS机主程序 
 

当前版本：0.0.2
修改人	：刘及华
完成日期：2014.2.26
升级说明：
 
当前版本：0.0.1
修改人	：刘及华
完成日期：2013.6.16
升级说明：
 
******************************************************************************/

#include "main.h"

/**************************************************************************/
#define 	MENU_COUNT		4
#define		Error			0
#define     PowerOff		-1

//#define DEBUG
//增加设置卡和司机卡是否刷卡标志
#define     ICON_A			0x40//已刷设置卡
#define		ICON_B			0x20//已刷司机卡
//#define		DEBUG
// *****************************************************************
// 功能：		主菜单界面
// *****************************************************************
//INT8U  main_menu[4*16+1] = {"1.参数设置      2.信息查询      3.数据传输      4.操作员密码修改"};

DEV_STAT    DevStat;				//设备状态
//GRAYRECORD GrayRecord;			//灰记录
CARD_INFO CardInfo;					//卡片信息，交易记录由此变量生成
ERROR_CARD_INFO ErrCardInfo;	    //出错卡状态
const char *VERSION = "0.0.2";     //版本号
char dbuf[100];

extern int socketID;
//INT16U   Apptimer_Delay;
extern struct sockaddr_in s_add;

//句柄结构定义，用于外设控制函数
DevHandle hRS232Handle;    //通用串口
DevHandle hDprinterHandle; //打印机
DevHandle hSIMHandle;     //SIM卡
DevHandle hCpuHandle;      //CPU卡
DevHandle hSam1Handle;      //SAM1卡
DevHandle hSam2Handle ;      //SAM2卡
DevHandle hMifsHandle;     //M1卡
DevHandle hWLMHandle;     //GPRS模块
//DevHandle hApptimerHandle; //定时计数器
//Devhandle hRS232Handle;     //usb接口用做调试作用

extern const char parmFileName[] ;
extern const char blackFileName[] ;
extern const char currecFileName[]; 
extern const char conrecFileName[]; 
extern const char hisrecFileName[]; 
extern const char grayrecFileName[] ;
extern const char clerkFileName[] ;

CLERK_STRUCT clerk_list[MAX_CLERK_NUM];

/*****************************************************************
函数原型：LowBatteryDisplay
功能描述：本函数为电池电量显示
参数描述：无
参数名称：	输入/输出？	类型		描述
 作  者：   刘及华
 日  期：   2012-11-24
 修改历史：
		日期        修改人      修改描述
返 回 值：无
*****************************************************************/
INT8U LowBatteryDisplay(void)
{
	ET_PowerInfo PowerInfo;
//	ET_PowerType ucType;
//	ET_PowerCapLevel ucCapability;

	if( EA_ucGetPowerInfo(&PowerInfo) != EM_SUCCESS )
	{
		lcddisperr("batt check failed.");
		return notok;
	}

	if( PowerInfo.ucType == EM_power_TYPE_BATTERY )
	{
		if( PowerInfo.ucCapLevel == EM_power_CAPACITY_LEVEL_LOW )
		{
			lcddisperr(" 电池电量低请充电 ");
			return notok;
		}
		else if( PowerInfo.ucCapLevel == EM_power_CAPACITY_LEVEL_CRITICAL )
		{
			lcddisp(1, 1, (void *)"电池电量过低系统无法");
			lcddisp(2, 1, (void *)"   启动请立即充电   ");
			(void)EA_uiInkey(1);
			return notok;
		}
	}
	return ok;
}

void Debugprintf(const char *fmt, ...)
{

//  int n;
	//char *arg;

//  va_list arg = (va_list)((char*)(&fmt) + 4);

//  n = vsprintf(sprint_buf, fmt, arg);


	EA_ucWriteDevice(hRS232Handle, strlen(fmt), 0, (char *)fmt);
}


/*****************************************************************
 函数原型：Menu_Sure_or_Cancel
 功能描述：选择确认或者是取消界面，只有两个选项，
 参数描述：

 返回值：  -1或者是0

 作  者：   刘及华
 日  期：   2012-11-24
 修改历史：
		日期        修改人      修改描述
		------      ---------   -------------
*****************************************************************/
//INT8U Menu_Sure_or_Cancel(const char * menu_line )
//{
//    INT16U key = 0;
//    int  disp_pos = 0;
//    int  menu_pos = 0;
//    int  m = 0, h = 0, i = 0;
//    INT8U  menu_text[2][22 * 2] =
//    {
//        "确定                ",
//        "取消                "
//    };
//    EA_vCls();
//    EA_vTextOut(0, 0, EM_key_FONT8X16, 0, 1, 1, (char *)menu_line);
//
//    for (;;)
//    {
//        key = EA_uiInkey(0);
//        switch ( key )
//        {
//           case EM_key_INTERPUNCTION:           //上
//               disp_pos--;
//               if ( disp_pos < 0 )
//                   disp_pos = 0;
//               menu_pos--;
//               if ( menu_pos < 0 )
//                   menu_pos = 0;
//
//               EA_vCls();   //清除屏幕
//               EA_ucSetInverse(EM_lcd_INVON);
//               lcddisp(disp_pos + 1, 1, &menu_text[menu_pos][0]);       //显示当前菜单，反显
//               EA_ucSetInverse(EM_lcd_INVOFF);
//
//               m = 1;
//               for ( i = disp_pos + 1; i <= 3; i++ )
//               {
//                   lcddisp(i + 1, 1, &menu_text[menu_pos + m][0]);
//                   m++;
//               }
//
//               break;
//
//           case EM_key_LETTER:                  //下
//               disp_pos++;
//               if ( disp_pos > 2 )
//                   disp_pos = 2;
//               menu_pos++;
//               if ( menu_pos > 2 )
//                   menu_pos = 2;
//
//
//               EA_vCls();   //清除屏幕
//               EA_ucSetInverse(EM_lcd_INVON);
//               lcddisp(disp_pos + 1, 1, &menu_text[menu_pos][0]);       //显示当前菜单，反显
//               EA_ucSetInverse(EM_lcd_INVOFF);
//
//               m = 1;
//               for ( i = disp_pos + 1; i <= 3; i++ )
//               {
//                   lcddisp(i + 1, 1, &menu_text[menu_pos + m][0]);
//                   m++;
//               }
//               m = 1;
//               for ( h = disp_pos - 1; h >= 0; h-- )
//               {
//                   lcddisp(h + 1, 1, &menu_text[menu_pos - m][0]);
//                   m++;
//               }
//
//               break;
//
//           case EM_key_ENTER:
//           if( menu_pos == 1 )
//               return -1;
//           else                   //确认
//               return menu_pos;
//               break;
//
//           case EM_key_CANCEL:
//                   return -1;
//                   break;
//
//           default:
//               break;
//        }
//    }
//}
/*****************************************************************
 函数原型：Debug_string(char *str)
 功能描述：串口调试输出函数
 参数描述：str打印的字符串

 返回值：   无

 作  者：   刘及华
 日  期：   2013-06-24
 修改历史：
		日期        修改人      修改描述
		------      ---------   -------------
*****************************************************************/
//void Debug_string(char *str)
//{
//	EA_ucWriteDevice(hUSBDHandle, sizeof(str), 0, (void *)str);
//}

/*****************************************************************
 函数原型：main()
 功能描述：主函数
 参数描述：

 返回值：   无

 作  者：   许岩/刘及华
 日  期：   2012-11-24
 修改历史：
		日期        修改人      修改描述
		------      ---------   -------------
*****************************************************************/
int main(void)
{	
	INT8U   ret = 0;
	char    mainflag = 1;
	INT8S   buf[30];
	uchar   uckey;
	uint 	uikey;
	BUS_TIME ltime;

	(void)EA_ucSetStopFlag( EM_DISABLE_STOP );  //不允许系统进入睡眠状态；

reboot:
	ret = System_Init();     //系统初始化,包括PSAM，参数表，GPRS初始化
	if( ret != ok )		 
	{
		lcddisperr("系统初始化失败!");
		goto reboot;
	}

	Beep(1000);

	for ( ;; )
	{
		Get_Time(&ltime);
		sprintf((void *)buf, "  %02X%02X-%02X-%02X %02X:%02X",
				ltime.century, ltime.year, ltime.month, ltime.day, ltime.hour, ltime.minute);
//  	EA_vTextOut(0, 0, EM_key_FONT8X16, 0, 1, 1, (char *)buf);
		EA_vDisplay(3, (void *)buf);
		if( mainflag == 1 )
		{
			DisplayWelcomeMenu();
			mainflag = 0;
		}

		uckey = EA_ucKBHit();
		if(uckey == EM_key_HIT)       //有按键
		{
			uikey = EA_uiInkey(0);    //读取按键键值
			switch ( uikey )
			{
				case EM_key_EXIT:     //关机
				{
					mainflag = 1;
					goto shutdown;
					break;
				}

				case EM_key_F3:
				{
					Login();                  //登录界面                    	
					mainflag = 1;
					break;
				}
				case EM_key_F1:	
				{
					Parm_Setting();          //参数配置
					mainflag = 1;
					break;
				}
				case EM_key_1:
				{
					GPRS_TEST();             //无线连接测试
					mainflag = 1;
					break;
				}
				case EM_key_F2:
				{
//  				QUERY_NOTE_RECORD();
					Gprs_Upload_data();      //数据上传
					mainflag = 1;
					break;
				}
				default:
					break;
			}
		}
	}
shutdown:
	Power_Down();
}

// *****************************************************************
// 功能：		QUERY_NOTE_RECORD
// 说明:		查询日结小票
// 入口参数：	
// 出口参数：	Null
// 作者：
// 返回值：	-1 关机				
// *****************************************************************
int QUERY_NOTE_RECORD(void)
{
	int         count;
	INT32U      RecordCount = 0;
	uchar       uckey; 
	uint        uikey; 
	uchar 		ucOpenID=0;
	INT8U 		rec[60];

	memset(rec, 0, sizeof(rec));

	if ( EA_ucPFOpen( (uchar *)hisrecFileName, &ucOpenID) != EM_ffs_SUCCESS )
	{
		lcddisperr("打开日结文件失败");
		return 0;
	}
	// 获取有效信息条数
	(void)EA_ucPFInfo(ucOpenID, EM_ffs_VALIDREC, &RecordCount);
	if( RecordCount == 0)
	{
		lcddisperr("没有日结信息");
		(void)EA_ucPFClose(ucOpenID);
		return 0;//表示没有记录
	}
	count = RecordCount;//最后一条		

	if(EA_ucPFReadRec(ucOpenID, count, 1, &RecordCount, rec) != EM_SUCCESS)
	{
		(void)EA_ucPFClose(ucOpenID);
		lcddisperr((void *)"读取不符合");
		return 0;
	}

	AnalyAndDisplayNote(rec);

	while ( 1 )
	{
		uckey = EA_ucKBHit();
		if(uckey == EM_key_HIT)    //有按键
		{
			uikey = EA_uiInkey(0);    //读取按键键值
			if ( (uikey == EM_key_EXIT) || (uikey == EM_key_CANCEL) )			//退出
			{
				(void)EA_ucPFClose(ucOpenID);
				return 1;
			}
			switch ( uikey )
			{
				case EM_key_UP:	//	按[2]键或往上拨动拨轮则屏幕往上滚动一行
					count --;
					if ( count < 0 )//记录超限
					{
						lcddisperr("最后一条");
						count ++;
					}
					break;

				case EM_key_DOWN:	//	按[8]键或往下拨动拨轮则屏幕往下滚动一行
					count ++;
					if ( count >= RecordCount )
					{
						lcddisperr("最后一条");
						count --;
					}
					break;
				case EM_key_ENTER:
					break;
				default:
					break;
			}

			if ( (uikey==EM_key_UP) || (uikey==EM_key_DOWN)  )
			{
				if(EA_ucPFReadRec(ucOpenID, count, 1, &RecordCount, rec) != EM_SUCCESS)
				{
					(void)EA_ucPFClose(ucOpenID);
					lcddisperr((void *)"读取不符合");
					return 0;
				}
				AnalyAndDisplayNote(rec);
			}

			if(uikey == EM_key_ENTER)
			{
				print_work_note_his(rec);
				(void)EA_ucPFClose(ucOpenID);
				return 1;
			}			
		}
	}
}

/******************************************************************************
 函数名称：print_work_note_history
 功能描述：打印日结算历史票据
 参数名称：输入/输出？	类型		描述
 输入		
 
 返  回  值：
 
 作      者	：于福勇
 日      期：2004-09-02
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
******************************************************************************/
void print_work_note_his (INT8U * rec)
{
	uchar       ucRet;
	INT8U		RecBuff[70];
	INT8U		strbuff[50];
	INT8U		time[7+2];


	_BANLANCE_IN_HAND * prec = (_BANLANCE_IN_HAND *)rec;

	EA_vCls();
	
	EA_vDisplay(1, "    日结帐单打印    ");//(int qx, int qy, char * ceStr, int xyFlag)
	EA_vDisplay(2, "       请等待...    ");//(int qx, int qy, char * ceStr, int xyFlag)
	SleepMs(1500);

	memset(RecBuff, 0, sizeof(RecBuff));
///打印头
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char *)"日结帐单历史记录\r\n");
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char *)"------------------------------\r\n");
//消费次数
	sprintf((void *)strbuff, "加油次数：%6d次\r\n", prec->consume_cnt);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char *)strbuff);

//消费金额
	sprintf((void *)strbuff, "加油体积：%6.2f元\r\n", (float)(prec->consume_amt)/100);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char *)strbuff);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char *)"------------------------------\r\n");
//上班时间	
	memcpy(time, (INT8U*)&prec->work_datetime, 7);
	htoa(RecBuff, time, 7);
	sprintf((void *)strbuff, "上班时间：%s\r\n", RecBuff);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char *)strbuff);
	
//下班时间
	memcpy(time, (INT8U*)&prec->down_datetime,7);
	htoa(RecBuff, time, 7);

	sprintf((void *)strbuff, "下班时间：%s\r\n",RecBuff);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char *)strbuff);

//分行符号
	strcpy((void *)strbuff, "---------------------------------- \r\r\r");
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char *)strbuff);
	
//设备编号
	htoa(RecBuff+4, &prec->equ_id[0], 3);
	sprintf((void *)strbuff, "设备编号：%s\r\n", RecBuff);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char *)strbuff);
//商户号
	htoa(RecBuff, &prec->acnt_id[0], 2);
	sprintf((void *)strbuff, "站点号：%s\r\n", RecBuff);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char *)strbuff);
//操作员号
	htoa(RecBuff, &prec->oper_id[0], 2);
	sprintf((void *)strbuff, "加油员工号：%s\r\n", RecBuff);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char *)strbuff);
	// 打印最后调用换页命令
	ucRet = EA_ucLinePrinter(hDprinterHandle, 0, "\f");
	if ( ucRet == EM_SUCCESS )
	{
		EA_vDisplay(2, "本次打印结束!");
		Beep(1000);
		EA_uiInkeyMs(2000);
	}
		
}


// *****************************************************************
// 功能：		AnalyAndDisplayNote
// 说明:		查询日结表，并显示日结信息
// 入口参数：	
// 出口参数：	Null
// 作者：
// 返回值：							
// *****************************************************************
void AnalyAndDisplayNote(INT8U * strTradeInfo)
{
	INT8U	time[7+2]; 
	char    strbuff[30];
	INT8U   temp[30];
//	float   fBuf = 0;
//	INT8U   LOCAL_V_TYPE  temp_buf[100];
	_BANLANCE_IN_HAND * prec = (_BANLANCE_IN_HAND *)strTradeInfo;

	EA_vCls();


	sprintf(strbuff, "加油次数:%7d次", prec->consume_cnt);
	EA_vDisplay(1, strbuff);

//消费金额
	sprintf(strbuff, "加油体积:%6.2f元", (float)(prec->consume_amt)/100);
	EA_vDisplay(2, strbuff);

//上班时间	
	memcpy(time, (INT8U*)&prec->work_datetime,7);
	htoa(temp, time, 7);
	sprintf(strbuff, "上班：%s", temp);
	EA_vDisplay(3, strbuff);
	
//下班时间
	memcpy(time, (INT8U*)&prec->down_datetime,7);
	htoa(temp, time, 7);
	sprintf(strbuff, "下班：%s",temp);
	EA_vDisplay(4, strbuff);
}

/*****************************************************************
函数原型：AppTimerISR
功能描述：定时器函数
参数描述：设置网络心跳时间
参数名称：	输入/输出？	类型		描述
-----------		-----------	------	   	------- 
作	  者： 刘及华 
返 回 值：无
*****************************************************************/
void AppTimerISR()
{
	//lcddisperr("定时显示");
//  GPRS_SEND_Receive((void *)&"0000", 4);
	int ret = 0;
//  if()
	if((socketID = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
//  	lcddisperr("SOCK建立失败");
		return;
	}

	ret = connect(socketID,(struct sockaddr_in *)(&s_add), sizeof(struct sockaddr_in));
	if( ret == -1 )
	{
		return;
	}
	if( write(socketID, (void *)&"0000", 4) == -1)
	{
//  	lcddisperr("发送失败");
//  	goto Err_sockoff;
	}
	close(socketID);
	return;
}
/*****************************************************************
函数原型：DisplayWelcomeMenu
功能描述：本函数为显示欢迎界面
参数描述：无
参数名称：	输入/输出？	类型		描述
-----------		-----------	------	   	-------
返 回 值：无
*****************************************************************/
void DisplayWelcomeMenu(void)
{
	BUS_TIME  ltime;
	INT8U buf[22];

	EA_vCls();
	EA_ucSetInverse(EM_lcd_INVOFF);
	EA_vDisp(1, 1, " 淄博市公共汽车公司 ");
	EA_vDisp(2, 1, "    手持POS加油机   ");

	Get_Time(&ltime);
	sprintf((void *)buf, "  %02X%02X-%02X-%02X %02X:%02X",
			ltime.century, ltime.year, ltime.month, ltime.day, ltime.hour, ltime.minute);
	EA_vDisplay(3, (void *)buf);

	if ( DevStat.record_number >= MAX_REC_NUM )
	{
		EA_vDisp(3, 1, "  记录已满, 请上传  ");
	}

	EA_vDisp(4, 1,     "F1设置 F2上传 F3登陆");
	return ;
}

/*****************************************************************
函数原型：SystemCheck
功能描述：本函数为系统检测
参数描述：无
参数名称：	输入/输出？	类型		描述
-----------		-----------	------	   	------- 
作    者： 刘及华 
日	  期： 2012-11-28
返 回 值：有
			 1:成功
			 0:失败
*****************************************************************/
INT8U SystemCheck()
{
	INT8U   ret = 0;
	//SAM卡检测
	EA_vCls();
//  EA_vDisp(2, 1, "  PSAM卡检测中...   ");
//  (void)EA_uiInkeyMs(2000);
	ret = SamCheck();
	if ( ret != ok )
	{
		EA_vCls();
		lcddisp(1, 1, (void *)"   PSAM卡检测错误   ");
		lcddisp(2, 1, (void *)"     请检查插槽     ");
		(void)EA_uiInkeyMs(2000);
		return notok;
	}
//  lcddisperr(" PSAM卡检测成功!");
	return ok;
}
/*****************************************************************
 函数原型：System_Init()
 功能描述：参数等初始化
 参数描述：

 返回值：   OK ?  NOTOK

 作  者：   许岩/刘及华
 日  期：   2012-11-24
 修改历史：
		日期        修改人      修改描述
		------      ---------   -------------
*****************************************************************/
INT8U System_Init(void)
{
	INT8U   i = 0;
	INT16U  key = EM_key_NOHIT;

	EA_vCls();
	EA_ucSetInverse(EM_lcd_INVOFF);
	EA_vDisp(1, 1, "    版本号: ");
	EA_vDisp(1, 13, VERSION);
	SleepMs(2000);
	EA_vCls();
	EA_vDisp(2, 1, "   系统初始化中...  ");
	key = EA_uiInkeyMs(2000);

	if ( key == EM_key_F3 )
	{
		format_process();    		  //可以进行系统格式化操作
	}
	//重要参数部分初始化函数
	appfile_Init();

	memset((void *)&DevStat, 0x00, sizeof(DevStat));

	i = ReadParam();  
	        
	if ( i != ok )
	{
		param_factory_default(0);     //格式化重要参数DEV.STATE
	}

	DevStat.POS_type = POS_T_NORMAL;  //盒类型为正常采集


	DevStat.record_number = query_rec_num();

	memset((void *)&CardInfo, 0x00, sizeof(CardInfo));        //清除卡的信息
	memset((void *)&ErrCardInfo, 0x00, sizeof(ErrCardInfo));  //清除错误卡的信息

	//这里应该添加一个函数，负责管理所有的设备的句柄结构!!!!
	i = Device_Handle_Init();
	if( i != ok)
	{
		return notok;
	}

	if( LowBatteryDisplay() != ok )     //电池电量检测
	{	
		lcddisperr("电池电量检测失败");
		return notok;
	}
	if( SystemCheck() != ok )          //PSAM系统检测
	{
		lcddisperr("PSAM卡检测失败");
		return notok;
	}
	return ok;
}
/*****************************************************************
 函数原型：appfile_Init(void)
 功能描述：用户文件系统初始化
 参数描述：	
				 
 返回值：	无
 
 作  者： 刘及华
 日  期：	2012-11-24
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
*****************************************************************/
void appfile_Init(void)
{
	//操作员登陆
	zbClerkFileInit();
	//更新白名单
//  download_whiltelist();

}


/*****************************************************************
 函数原型：zbClerkFileInit(void)
 功能描述：操作员文件初始化
 参数描述：	
				 
 返回值：	OK ? NOTOK
 
 作  者： 刘及华
 日  期：	2012-11-24
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
*****************************************************************/
char zbClerkFileInit(void)
{
	char upd=1;
	uchar i;
	uchar ucOpenID;
	uchar ucRet;

	if (pf_exist((void *)clerkFileName, &ucOpenID)==FALSE)
	{
		ucRet = pf_creat((void *)clerkFileName, sizeof(CLERK_STRUCT));
		upd=1;
	}
	else
	{
		ucRet = pf_close(ucOpenID);
		upd=0;
	}

//  if (ucRet!=EM_SUCCESS)
//  {
//  	lcddisperr((void *)"员工名单初始化失败");
//  	return notok;
//  }
	
	memset(&clerk_list, 0, sizeof(clerk_list));

	if (upd) 
	{
		for (i=0; i<MAX_CLERK_NUM; i++)
		{
  			clerk_list[i].id[0] = '0';
	  		clerk_list[i].id[1] = '0'+i;
//			memset(clerk_list[i].id, '0'+i, 2);
			clerk_list[i].right = ID_OPERATOR;
			memset(clerk_list[i].pw,'0'+i,4);
		}

		//set the default
//		memset(clerk_list[9].id,'8',2);
//		memcpy(clerk_list[9].pw,"22446868",8);
//		clerk_list[9].right=ID_SUPERVISOR;

		//////////////////////////////////////////////////////////////////////////
//		memset(clerk_list[0].id,'0',2);
//		memset(clerk_list[0].pw,'0',6);
//		clerk_list[0].right=ID_MANAGER;

		//////////////////////////////////////////////////////////////////////////
//		strcpy((void *)&clerk_list[1].id[0], "01");
//		memset(clerk_list[1].pw,'1',4);
//		clerk_list[1].right=ID_OPERATOR;

		zbClerkSaveFile();
	}
	
	zbClerkReadFile();

	return ok;
}

/*****************************************************************
 函数原型：zbClerkSaveFile(void)
 功能描述：操作员文件初始化
 参数描述：	
				 
 返回值：	OK ? NOTOK
 
 作  者： 刘及华
 日  期：	2012-11-24
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
*****************************************************************/
char zbClerkSaveFile(void)
{
//	uchar ucret;
	uchar ucOpenID;

	pf_open((void *)clerkFileName, &ucOpenID);

	pf_write(ucOpenID, 0, (uchar *)clerk_list);

	pf_close(ucOpenID);

	return TRUE;
}

/*****************************************************************
 函数原型：zbClerkReadFile(void)
 功能描述：操作员文件初始化
 参数描述：	
				 
 返回值：	OK ? NOTOK
 
 作  者： 刘及华
 日  期：	2012-11-24
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
*****************************************************************/
char zbClerkReadFile(void)
{
	uchar ucOpenID;
	uint  uiRecnum;

	pf_open((void *)clerkFileName, &ucOpenID);

	pf_read(ucOpenID, 1, 1, &uiRecnum, (uchar *)clerk_list);
	
	pf_close(ucOpenID);

	return TRUE;
}
/*****************************************************************
 函数原型：Device_Handle_Init()
 功能描述：设备所有句柄初始化
 参数描述：	
				 
 返回值：	OK ? NOTOK
 
 作  者： 刘及华
 日  期：	2012-11-24
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
*****************************************************************/
INT8U Device_Handle_Init(void)
{
	uchar  ucRet;

	if (EA_ucOpenDevice("RFCARD", EM_mifs_TYPEA, &hMifsHandle) != EM_SUCCESS)
	{
		lcddisperr("读卡器初始化失败");
		return notok;
	}
	if ( EA_ucOpenDevice("PRINT", EM_io_REALMODE, &hDprinterHandle) != EM_SUCCESS )
	{
		lcddisperr("获得打印操作句柄失败!");
		return notok;
	}
//  if ( EA_ucOpenDevice("USBD", EM_io_REALMODE, &hUSBDHandle) != EM_SUCCESS )
//  {
//  	lcddisperr("获得打印操作句柄失败!");
//  	return notok;
//  }
//  if ( EA_ucOpenDevice("WLM", 0, &hWLMHandle) != EM_SUCCESS )
//  {
//  	lcddisperr("获得GPRS句柄失败!");
//  	return notok;
//  }
///////////////////////////////////////////////打印机设置参数
	ucRet =  EA_ucInitPrinter(hDprinterHandle);
	if ( ucRet != EM_SUCCESS )
	{
		EA_vDisplay(2, "初始化打印机芯失败!");
		return notok;
	}
	ucRet = EA_ucSetPrinterFormat(hDprinterHandle, EM_prn_XSPACE, 0);
	if ( ucRet != EM_SUCCESS )
	{
		//  	(void)EA_ucCloseDevice(&hDprinterHandle);
		lcddisperr("打印机设置失败!");
		return notok;
	}
	ucRet =  EA_ucSetPrinterFormat(hDprinterHandle, EM_prn_YSPACE, 6);
	if ( ucRet != EM_SUCCESS )
	{
		//  	(void)EA_ucCloseDevice(&hDprinterHandle);
		lcddisperr("打印机设置失败!");
		return notok;
	}

	//  ucRet = EA_ucSetPrinterFormat(hDprinterHandle, EM_prn_PAGESPACE, 578);
	ucRet = EA_ucSetPrinterFormat(hDprinterHandle, EM_prn_PAGESPACE , 220);
	if ( ucRet != EM_SUCCESS )
	{
		//  	(void)EA_ucCloseDevice(&hDprinterHandle);
		lcddisperr("打印机设置失败!");
		return notok;
	}

	ucRet = EA_ucSetPrinterFormat(hDprinterHandle, EM_prn_ASCII_DOTTYPE, EM_prn_ASCIIDOT24X12);
	if ( ucRet != EM_SUCCESS )
	{
		//  	(void)EA_ucCloseDevice(&hDprinterHandle);
		lcddisperr("打印机设置失败!");
		return notok;
	}
	ucRet = EA_ucSetPrinterFormat(hDprinterHandle, EM_prn_ASCII_PRNTYPE, EM_prn_ASCIIPRN1X1);
	if ( ucRet != EM_SUCCESS )
	{
		//  	(void)EA_ucCloseDevice(&hDprinterHandle);
		lcddisperr("打印机设置失败!");
		return notok;
	}
	ucRet = EA_ucSetPrinterFormat(hDprinterHandle, EM_prn_HZ_DOTTYPE, EM_prn_HZDOT24X24);
	if ( ucRet != EM_SUCCESS )
	{
		//  	(void)EA_ucCloseDevice(&hDprinterHandle);
		lcddisperr("打印机设置失败!");
		return notok;
	}

	ucRet = EA_ucSetPrinterFormat(hDprinterHandle, EM_prn_HZ_PRNTYPE, EM_prn_HZPRN1X1);
	if ( ucRet != EM_SUCCESS )
	{
		//  	(void)EA_ucCloseDevice(&hDprinterHandle);
		lcddisperr("打印机设置失败!");
		return notok;
	}	
	ucRet = EA_ucSetPrinterFormat(hDprinterHandle, EM_prn_PRNGRAY, 4);
	if ( ucRet != EM_SUCCESS )
	{
		//  	(void)EA_ucCloseDevice(&hDprinterHandle);
		lcddisperr("打印机设置失败!");
		return notok;
	}
/////////////////////////////////////////////////////////////////////////////初始化时候只设置一次
	if (EA_ucOpenDevice("SAM1CARD", 0, &hSam1Handle) != EM_SUCCESS)
	{
		lcddisperr("PSAM卡初始化失败");
		return notok;
	}
//  if (EA_ucOpenDevice("USERCARD", 0, &hCpuHandle) != EM_SUCCESS)
//  {
//  	lcddisperr("CPU卡初始化失败");
//  	return notok;
//  }
//  if (EA_ucICInitModule(hCpuHandle, EM_ic_VOLDEFAULT, EM_ic_TCRMODE) != EM_SUCCESS)
//  {
//      lcddisperr("设置CPU规范失败");
//   	return notok;
//  }
    ucRet = EA_ucOpenDevice("USBD", 0, &hRS232Handle);
	if ( ucRet != EM_SUCCESS )
	{
		(void)EA_ucCloseDevice(&hRS232Handle);
		lcddisperr("COM口设置失败!");
		return notok;
	}
	EA_ucInitRS232(hRS232Handle, EM_232_115200BPS, EM_232_NOPAR, EM_232_8DBS);

	return ok;
}
/******************************************************************************
 函数名称：TimeFormatChk
 功能描述：读时钟，看时钟格式是否正确
 参数描述：
 参数名称：	输入/输出？	类型		描述
				
 返  回  值：ok(0)-自检成功
				 notok(0xFF)-自检失败
				   
 作      者	：许岩
 日      期：2004-09-02
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
******************************************************************************/
INT8U TimeFormatChk(void)
{
	INT8U i = 0;
	BUS_TIME ltime;

	i = Get_Time(&ltime);
	if ( i != ok )
	{
		return notok;
	}

	i = CheckTimeFormat(&ltime);
	return i;
}

/******************************************************************************
 函数名称：TimeModify
 功能描述：要求输入时间，进行修改
 参数描述：
 参数名称：	输入/输出？	类型		描述
				
 返  回  值：无
				   
 作      者	：许岩
 日      期：2004-09-02
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
******************************************************************************/
void TimeModify(void)
{
	INT8U i = 0;
	INT8U input[50];
	INT8U buf[50];
	BUS_TIME ltime;

	for (;;)
	{
		EA_vCls();
		Get_Time(&ltime);
		EA_vDisp(1, 1, "当前时间:");
		sprintf((void *)buf, "%02X%02X-%02X-%02X-%02X:%02X:%02X", ltime.century, ltime.year, ltime.month,
				ltime.day, ltime.hour, ltime.minute, ltime.second);
		EA_vDisp(2, 1, (void *)buf);
		EA_vDisp(3, 1, "请输入新的时间:");
		(void)EA_ucClrKeyBuf();
		strcpy((void *)input, "");

		i = EA_ucGetInputStr(4, 1, 20, EM_BIG_FONT | EM_MODE_NUMBER | EM_ALIGN_LEFT | EM_SHOW_ORIGINALLY
							 , 14, 14, 0, (void *)input);
		//  	sprintf(dbuf, "%s", input);
		//  	EA_vDisp(3, 1, dbuf);
		//  	sprintf(dbuf, "result:%02X", i);
		//  	EA_vDisp(4, 1, dbuf);
		if ( i == EM_ABOLISH )
			return;
		if ( i != EM_SUCCESS )
			continue;
		ltime.century = (ascii_to_hex(input[0]) << 4) | ascii_to_hex(input[1]);
		ltime.year = (ascii_to_hex(input[2]) << 4) | ascii_to_hex(input[3]);
		ltime.month = (ascii_to_hex(input[4]) << 4) | ascii_to_hex(input[5]);
		ltime.day = (ascii_to_hex(input[6]) << 4) | ascii_to_hex(input[7]);
		ltime.hour = (ascii_to_hex(input[8]) << 4) | ascii_to_hex(input[9]);
		ltime.minute = (ascii_to_hex(input[10]) << 4) | ascii_to_hex(input[11]);
		ltime.second = (ascii_to_hex(input[12]) << 4) | ascii_to_hex(input[13]);

		i = CheckTimeFormat(&ltime);
		if ( i != ok )
		{
			EA_vCls();
			EA_vDisp(4, 1, "时间格式错误");
			SleepMs(1500);
			//  		EA_vCls();
			continue;
		}

		Modify_Time(&ltime);
		EA_vCls();
		EA_vDisp(4, 1, "时间修改成功");
		SleepMs(1500);
		break;
	}
}


/******************************************************************************
 函数名称：format_process
 功能描述：格式化处理
 参数描述：
 参数名称：	输入/输出？	类型		描述
				
 返  回  值：无
				   
 作      者	：许岩/刘及华
 日      期：2012-11-24
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
******************************************************************************/
void format_process(void)
{
	INT8U i = 0;
	INT8U input[50];
	INT8U passwd[10] = "22884646";

	EA_vCls();
	(void)EA_vDisp(1, 1, (void *)"     系统格式化     ");
	(void)EA_vDisp(2, 1, (void *)"输入密码:");
	(void)EA_ucClrKeyBuf();
	strcpy((void *)input, "");
	for ( ;; )
	{
		i = EA_ucGetInputStr(2, 10, 20, EM_BIG_FONT | EM_MODE_NUMBER | EM_ALIGN_LEFT | EM_SHOW_PASSWORD
							 , 8, 8, 0, (void *)input);
		if ( i == EM_SUCCESS )
			break;
		if ( i == EM_ABOLISH )
			return;
	}

	if ( strcmp((void *)input, (void *)passwd) == 0 )
	{
		lcddisperr("系统正在格式化...");
	}
	else
	{
		EA_ucSetInverse(EM_lcd_INVON);
		lcddisp(4, 1, (void *)"密码错误!");
		EA_ucSetInverse(EM_lcd_INVOFF);
		return;
	}
	//初始化操作
	param_factory_default(1);

}
/******************************************************************************
 函数名称：Power_Down
 功能描述：休眠手持机
 参数名称：	输入/输出？	类型		描述
 输入		
 
 返  回  值：
 
 作      者	：于福勇/刘及华
 日      期：2012-11-27

 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
******************************************************************************/
void Power_Down(void)
{
//  uint uisleep_delay;   //可提升为全局变量
//  EA_ucSetStopFlag(EM_ENABLE_STOP);  //两秒后进入睡眠状态；
	lcddisperr("系统正在关机...");
//  uisleep_delay = 2;
	Close_GPRS_Module();
//  EA_vSetSaveResTime(uisleep_delay);
	EA_vSystemManage (EM_SHUTDOWN);
	
//  (void)EA_uiInkey(0);
}
/******************************************************************************
 函数名称：Power_Down
 功能描述：休眠手持机
 参数名称：	输入/输出？	类型		描述
 输入		
 
 返  回  值：
 
 作      者	：于福勇/刘及华
 日      期：2012-11-27

 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
******************************************************************************/
void System_reset(void)
{
//  uint uisleep_delay;   //可提升为全局变量
//  EA_ucSetStopFlag(EM_ENABLE_STOP);  //两秒后进入睡眠状态；
	lcddisperr("系统正在重启...");
//  uisleep_delay = 2;
	Close_GPRS_Module();
//  EA_vSetSaveResTime(uisleep_delay);
	EA_vSystemManage (EM_RESET);
	
	//(void)EA_uiInkey(0);
}