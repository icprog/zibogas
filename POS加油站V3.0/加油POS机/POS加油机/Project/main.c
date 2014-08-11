/******************************************************************************
Copyright 2013 大连易达通电子有限公司
All rights riserved.

文件名	：main.c
模块名称：淄博加油POS机主程序
功能概要：淄博加油POS机主程序 

当前版本：0.1.5 
    	  1、修改记录的传输方式
    	  2、增加一个参数文件记录形式
 
当前版本：0.1.3 
    	  1.设置油价更新有效时间
    	  2.修改油价时候，手动的需要先输入密码后，才能修改
    	  3.从服务器中下载生效时间设定功能
		  4.手动修改油价后，将日志信息上传	
当前版本：0.1.2  
    	  1.设置油价更新有效时间
    	  2.修改油价时候，手动的需要先输入密码后，才能修改
    	  3.从服务器中下载生效时间设定功能
		  4.手动修改油价后，将日志信息上传
 
当前版本：0.1.0  
当前版本：0.0.9 
    	  1.将所有的float的数据类型，全部转换为double类型
    	  2.修改上班时间保存
    	  3.给不进位的数据加上 0.001
    	  4.修改登录时间无法保存的bug
 
当前版本：0.0.7 
当前版本：0.0.6
修改人	：刘及华
完成日期：2014.3.26
升级说明：1、修改一个bug：加油员登陆没有刷卡按一下取消键就登陆成功 
    	  2、增加计算四舍五入的问题
    	  3、白名单下载完也滴，提示音
    	  4、记录查询中单笔数据查询序号点乱（按照十进制显示）
    	  5、快捷方式中 显示当前油价
    	  6、重打上次小票也放到快捷方式中
    	  7、手动输入车号那个界面，要是想取消，是取消不了的

 
当前版本：0.0.3
修改人	：刘及华
完成日期：2014.3.26
升级说明：新增加一个卡 的流程，员工卡是新发的卡，每次刷卡的时候，要验证输入的密码 
 
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

//#define	 _zb_debug_tran_

DEV_STAT    DevStat;				//设备状态
CARD_INFO 	CardInfo;				//卡片信息，交易记录由此变量生成
ERROR_CARD_INFO ErrCardInfo;	    //出错卡状态
const char *VERSION = "VER:0.1.5";  //版本号
GPRS gprs;

//gprs通讯相关
extern int socketID;
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
//INT16U   Apptimer_Delay;
DevHandle hUSBDHandle;     //usb接口用做调试作用
ET_DISK_INFO ptDiskInfo;
ET_PART_INFO ptPartInfo;

extern const char parmFileName[] ;
extern const char blackFileName[] ;
extern const char currecFileName[]; 
extern const char conrecFileName[]; 
extern const char hisrecFileName[]; 
extern const char grayrecFileName[] ;
extern const char clerkFileName[] ;

//LERK_STRUCT clerk_list[MAX_CLERK_NUM];
extern INT8U rec_query[2*20 +2];
extern INT8U  kuaijie_set[5*20 +2];


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
//  ET_PowerType ucType;
//  ET_PowerCapLevel ucCapability;

	if( EA_ucGetPowerInfo(&PowerInfo) != EM_SUCCESS )
	{
		lcddisperr("电池损坏请检测");
		return notok;
	}

	if( PowerInfo.ucType == EM_power_TYPE_BATTERY )
	{
		if( PowerInfo.ucCapLevel == EM_power_CAPACITY_LEVEL_LOW )
		{
			lcddisperr("电池电量低请充电");
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
/*****************************************************************
函数原型：Debugprintf
功能描述：本函数为调试信息输出
参数描述：无
参数名称：	输入/输出？	类型		描述
 作  者：   刘及华
 日  期：   2012-11-24
 修改历史：
		日期        修改人      修改描述
返 回 值：无
*****************************************************************/
void Debugprintf(const char *fmt, ...)
{
//  int n;
//  char *arg;
//  va_list arg = (va_list)((char*)(&fmt) + 4);
//  n = vsprintf(sprint_buf, fmt, arg);
	EA_ucWriteDevice(hRS232Handle, strlen(fmt), 0, (char *)fmt);
}


/*****************************************************************
 函数原型：main()
 功能描述：主函数
 参数描述：
 返回值：   无
 作  者：   刘及华
 日  期：   2012-11-24
 修改历史：
		日期        修改人      修改描述
		------      ---------   -------------
*****************************************************************/
int main(int argc, char **argv)
{	
	INT8U   ret = 0;
	char    mainflag = 1;
	uchar   uckey;
	uint 	uikey;

	(void)EA_ucSetStopFlag( EM_DISABLE_STOP );  //不允许系统进入睡眠状态；

reboot:
	ret = System_Init();      //系统初始化,包括PSAM，参数表，GPRS初始化
	if( ret != ok )		 
	{
		lcddisperr("系统初始化失败!");
		goto reboot;
	}

	Beep(1000);

	//(void)EA_ucSetStopFlag( EM_ENABLE_STOP );  //允许系统进入睡眠状态；
	for ( ;; )
	{

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
					Login();          //登录界面                    	
					mainflag = 1;
					break;
				}
				case EM_key_F1:	
				{
					Parm_Setting();   //参数设置
					mainflag = 1;
					break;
				}
				case EM_key_1:
				{
					gprs_test();     //无线连接测试
					mainflag = 1;
					break;
				}
				case EM_key_F2:
				{
					kuaijiefangshi(); //快捷方式
//  				Gprs_Upload_data();      
//  				GPRS_Close();
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


void kuaijiefangshi(void)
{
	INT8S ret = 0;
	while(1)
	{

		ret = browse_menu(0, (void *)"", kuaijie_set, TRUE );
		switch ( ret )
		{
		   case 0://白名单下载
				download_whiltelist();	
				break;
		   case 1: //数据上传
			    Gprs_Upload_data();     
				break;	
		   case 2: //当前油价查询
			    query_oil_price();     
			    break;	
		   case 3: //重打上次小票
			    Reprint_sale_record();     
				break;	
		   case 4: //手动输入车号
			    man_set_busnum();     
				break;	
		   case -1:
			   return;
		   default:
				break;
		}
	}
	return ;
}

// *****************************************************************
// 功能：		query_oil_price
// 说明:		查询当前油价
// 入口参数：	
// 出口参数：	Null
// 作者：	frank
// 返回值：		Null		
// *****************************************************************
void query_oil_price(void)
{
	//当前油价显示
	EA_vCls();
	EA_vDisplay(2, "天然气 :%2.2f元/立方", DevStat.price[0]/100.0);
	EA_vDisplay(3, "汽油93#:%2.2f元/升 ", DevStat.price[1]/100.0);
	EA_vDisplay(4, "汽油97#:%2.2f元/升 ", DevStat.price[2]/100.0);
	EA_vDisplay(5, "柴  油 :%2.2f元/升 ", DevStat.price[3]/100.0);

	//按任意键退出
	(void)EA_uiInkey(0);
	return;
}
// *****************************************************************
// 功能：		man_set_busnum
// 说明:		手动输入车号
// 入口参数：	
// 出口参数：	Null
// 作者：
// 返回值：		Null		
// *****************************************************************
void man_set_busnum(void)
{
	INT8U i = 0;
	INT8U input1[20];
	uchar  uckey;
	uint   uikey;
	uchar ret;
	uchar     temp_uc[5];
//  if ( strcmp((void *)&DevStat.bus_number[0], "12345") == 0 )
//  {
//  	goto set_bus_number;
//  }

	EA_vCls();
	lcddisp(2, 1, (void *)"输入车号:           ");
	strcpy((void *)input1, "");

	for ( ;; )
	{
		i = EA_ucGetInputStr(2, 10, 20, EM_BIG_FONT | EM_MODE_CHAR | EM_ALIGN_LEFT | EM_SHOW_ORIGINALLY
							 , 5, 5, 0, (void *)input1); //刘及华，修改只能输入数字和字母
		if ( i == EM_SUCCESS )
			break;
		if ( i == EM_ABOLISH )
			return;      //do nothing
	}

	memcpy((void *)&CardInfo.bus_number[0], (void *)input1, 5);

	WriteParam();

	EA_ucSetInverse(EM_lcd_INVON);
	lcddisp(3, 1, (void *)"      输入成功      ");
	EA_ucSetInverse(EM_lcd_INVOFF);
	SleepMs(1500);

RESELECT:
	

	memcpy(temp_uc, &CardInfo.bus_number[0], 5);

	i = BlackName_Find((char *)temp_uc);	//白名单判断
	if ( i != ok )	 						//不是白名单
	{
		lcddisperr("该卡不在白名单中");
		Beeperr();
		return;
	}

	EA_vCls();
	EA_vDisplay(1, "车  号 :鲁C-%s", CardInfo.bus_number);

	lcddisp(4, 1, (void *)"     请刷员工卡     ");

	while(1)
	{
		if( CardInit() == ok )
			break;

		uckey = EA_ucKBHit();
		if ( uckey ==  EM_key_HIT )		 //按下取消键
		{
			uikey = EA_uiInkey(0);
			if ( uikey == EM_key_CANCEL )		 //按下取消键
			{
				return ;
			}
		}
	}

	ret = M1CardProcess();   //再检测员工卡
	if(ret == ok)
	{
		;
	}
	else
	{
		goto RESELECT;
	}

	return;
}
// *****************************************************************
// 功能：		query_note_record
// 说明:		查询日结小票
// 入口参数：	
// 出口参数：	Null
// 作者：
// 返回值：	-1 关机				
// *****************************************************************
int query_note_and_record(void)
{
	uchar ret ;
	ret = browse_menu(0, (void *)"", rec_query, TRUE );
	switch ( ret )
	{
	   case 0://日结单查询
			Note_Query();				
			break;
	   case 1://单笔记录查询
			Record_Query();
			break;
	   default:
		    break;
	}
	return ok;
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
void print_work_note_his(INT8U * rec)
{
	uchar       ucRet;
	INT8U		RecBuff[70];
	INT8U		strbuff[50];
	INT8U		time[7+2];


	_BANLANCE_IN_HAND * prec = (_BANLANCE_IN_HAND *)rec;

	EA_vCls();
	
	EA_vDisplay(1, "  历史日结帐单打印  ");//(int qx, int qy, char * ceStr, int xyFlag)
	EA_vDisplay(2, "       请等待...    ");//(int qx, int qy, char * ceStr, int xyFlag)
	SleepMs(1500);

	memset(RecBuff, 0, sizeof(RecBuff));

	ucRet = EA_ucGetPrinterStatus(hDprinterHandle);      //打印前必须判断打印机的状态
	if( ucRet == EM_prn_PAPERENDED )
	{
		EA_vCls();
		EA_vDisplay(1, "   缺纸,打印失败!  ");
		EA_vDisplay(2, "   请放入纸张后    ");
		EA_vDisplay(3, "   按确认任意键    ");
		EA_uiInkey(0);
	}
	if( ucRet == EM_prn_OVERHEAT )
	{
		EA_vDisplay(2, "    机芯温度过高    ");
		EA_vDisplay(3, "    请稍后再打印     ");
		EA_uiInkeyMs(500);
		return;
	}
	if( ucRet == EM_prn_LOWVOL )
	{
		EA_vDisplay(2, "    电池电量过低    ");
		EA_vDisplay(3, "   请充电后再打印   ");
		EA_uiInkeyMs(500);
		return;
	}

	(void)EA_ucPrinterAutoFeeding(hDprinterHandle);
///打印头
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char *)"淄博市公共汽车公司\r\n");
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char *)"日结帐单历史记录\r\n");
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char *)"------------------------------\r\n");
//消费次数
	sprintf((void *)strbuff, "天然气加气次数 ：%6d次\r\n", prec->consume_cnt[0]);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char *)strbuff);
	sprintf((void *)strbuff, "汽油93#加油次数：%6d次\r\n", prec->consume_cnt[1]);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char *)strbuff);
	sprintf((void *)strbuff, "汽油97#加油次数：%6d次\r\n", prec->consume_cnt[2]);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char *)strbuff);
	sprintf((void *)strbuff, "柴油加油次数   ：%6d次\r\n", prec->consume_cnt[3]);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char *)strbuff);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char *)"------------------------------\r\n");
//消费体积
	sprintf((void *)strbuff, "天然气体积 ：%6.2lf元\r\n", (double)(prec->consume_cap[0])/100);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char *)strbuff);
	sprintf((void *)strbuff, "汽油93#体积：%6.2lf元\r\n", (double)(prec->consume_cap[1])/100);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char *)strbuff);
	sprintf((void *)strbuff, "汽油97#体积：%6.2lf元\r\n", (double)(prec->consume_cap[2])/100);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char *)strbuff);
	sprintf((void *)strbuff, "柴 油 体 积：%6.2lf元\r\n", (double)(prec->consume_cap[3])/100);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char *)strbuff);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char *)"------------------------------\r\n");
//消费金额
	sprintf((void *)strbuff, "天然气金额 ：%6.2lf元\r\n", (double)(prec->consume_amt[0])/100);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char *)strbuff);
	sprintf((void *)strbuff, "汽油93#金额：%6.2lf元\r\n", (double)(prec->consume_amt[1])/100);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char *)strbuff);
	sprintf((void *)strbuff, "汽油97#金额：%6.2lf元\r\n", (double)(prec->consume_amt[2])/100);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char *)strbuff);
	sprintf((void *)strbuff, "柴 油 金 额：%6.2lf元\r\n", (double)(prec->consume_amt[3])/100);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char *)strbuff);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char *)"------------------------------\r\n");
	sprintf((void *)strbuff, "总  金  额 ：%6.2lf元\r\n", (double)(prec->consume_amt[0]+prec->consume_amt[1]+prec->consume_amt[2]
															+prec->consume_amt[3])/100);

	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char *)strbuff);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char *)"------------------------------\r\n");

//上班时间	
	memcpy(time, (INT8U*)&prec->work_datetime, 7);
	sprintf((void *)strbuff, "上班时间：\r\n");
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	sprintf((void *)strbuff, "%02X%02X-%02X-%02X %02X:%02X:%02X\r\n", time[0],time[1],time[2],time[3],time[4],time[5],time[6]);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);	
//下班时间
	memcpy(time, (INT8U*)&prec->down_datetime,7);
	sprintf((void *)strbuff, "下班时间：\r\n");
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	sprintf((void *)strbuff, "%02X%02X-%02X-%02X %02X:%02X:%02X\r\n", time[0],time[1],time[2],time[3],time[4],time[5],time[6]);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);	

//分行符号
	strcpy((void *)strbuff, "---------------------------------- \r\r\r");
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char *)strbuff);
	
//设备编号
	htoa(RecBuff, &prec->equ_id[0], 3);
	sprintf((void *)strbuff, "设备编号  ：%s\r\n", RecBuff);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char *)strbuff);
//商户号
	htoa(RecBuff, &prec->acnt_id[0], 2);
	sprintf((void *)strbuff, "站  点  号：%s\r\n", RecBuff);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char *)strbuff);
//操作员号
	htoa(RecBuff, &prec->oper_id[0], 1);
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

	_BANLANCE_IN_HAND * prec = (_BANLANCE_IN_HAND *)strTradeInfo;

	EA_vCls();


	sprintf(strbuff, "加油次数:%7d次", (prec->consume_cnt[0]+prec->consume_cnt[1]+prec->consume_cnt[2]+prec->consume_cnt[3]));
	EA_vDisplay(1, strbuff);

//消费金额
	sprintf(strbuff, "加油金额:%6.2lf元", (double)(prec->consume_amt[0]+prec->consume_amt[1]+prec->consume_amt[2]+prec->consume_amt[3])/100.0);
	EA_vDisplay(2, strbuff);

//上班时间	
	memcpy(time, (INT8U*)&prec->work_datetime, 6);
	htoa(temp, time, 6);
	sprintf(strbuff, "上班时间:%s", temp);
	EA_vDisplay(4, strbuff);
	
//操作员
	memcpy(strbuff, (INT8U*)&prec->oper_id, 3);
	htoa((void *)temp, (void *)strbuff, 3);
	sprintf(strbuff, "加 油 员:%s",temp);
	EA_vDisplay(3, strbuff);
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
	int ret = 0;

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

	EA_vCls();
	EA_vDrawIcons();

	EA_ucSetInverse(EM_lcd_INVOFF);
	EA_vDisp(1, 1, " 淄博市公共汽车公司 ");
	EA_vDisp(2, 1, "    手持POS加油机   ");
	EA_vDisp(3, 6, (void *)VERSION);


	if ( DevStat.record_number >= MAX_REC_NUM )
	{
		EA_vDisp(3, 1, "  记录已满, 请上传  ");
	}

	EA_vDisp(4, 1,     "设置    快捷    登陆");
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
INT8U SystemCheck(void)
{
	INT8U   ret = 0;

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

	EA_iSetMode(0);    //显示模式设置

	EA_vCls();
	EA_ucSetInverse(EM_lcd_INVOFF);
	EA_vDisp(2, 1, "   系统初始化中...  ");
	key = EA_uiInkeyMs(1500);

	if ( key == EM_key_F3 )
	{
		format_process();    		  //可以进行系统格式化操作
	}

	memset((void *)&DevStat, 0x00, sizeof(DevStat));

	i = ReadParam();          
	if ( i != ok )
	{
		param_factory_default(0);     //格式化重要参数DEV.STATE
	}

	DevStat.POS_type = POS_T_NORMAL;  //盒类型为正常采集

	//重要参数部分初始化函数
	appfile_Init();

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
	INT8U ret;
	//操作员登陆
	//zbClerkFileInit();
	//更新白名单
	ret = download_whiltelist();
	if(ret == notok)
	{
		EA_vDisplay(2, "更新白名单失败!");
		EA_vDisplay(3, "请登录后手动更新");
		Beeperr();
	}
	//更新标价表
	ret = download_price_table();
	if(ret == notok)
	{
		EA_vDisplay(2, "更新油价失败!");
		EA_vDisplay(3, "请登录后手动更新");
		Beeperr();
	}
	GPRS_Close();   //启动为更新程序之后，要立刻关闭GPRS模块
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
	uint   cap_sd;

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
	if ( EA_ucOpenDevice("USBD", EM_io_REALMODE, &hUSBDHandle) != EM_SUCCESS )
	{
		lcddisperr("获得打印操作句柄失败!");
		return notok;
	}
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
		lcddisperr("打印字符间距设置失败");
		return notok;
	}
	ucRet =  EA_ucSetPrinterFormat(hDprinterHandle, EM_prn_YSPACE, 8);
	if ( ucRet != EM_SUCCESS )
	{
		lcddisperr("打印行距设置失败");
		return notok;
	}

	//  ucRet = EA_ucSetPrinterFormat(hDprinterHandle, EM_prn_PAGESPACE, 578);
	ucRet = EA_ucSetPrinterFormat(hDprinterHandle, EM_prn_PAGESPACE , 180);
	if ( ucRet != EM_SUCCESS )
	{
		lcddisperr("打印页长度设置失败");
		return notok;
	}

	ucRet = EA_ucSetPrinterFormat(hDprinterHandle, EM_prn_ASCII_DOTTYPE, EM_prn_ASCIIDOT24X12); 
	if ( ucRet != EM_SUCCESS )
	{
		lcddisperr("打印ASCII字符设置失败");
		return notok;
	}
	ucRet = EA_ucSetPrinterFormat(hDprinterHandle, EM_prn_ASCII_PRNTYPE, EM_prn_ASCIIPRN1X1);
	if ( ucRet != EM_SUCCESS )
	{
		lcddisperr("打印ASCII字符大小失败");
		return notok;
	}
	ucRet = EA_ucSetPrinterFormat(hDprinterHandle, EM_prn_HZ_DOTTYPE, EM_prn_HZDOT24X24);
	if ( ucRet != EM_SUCCESS )
	{
		lcddisperr("打印汉字设置失败!");
		return notok;
	}

	ucRet = EA_ucSetPrinterFormat(hDprinterHandle, EM_prn_HZ_PRNTYPE, EM_prn_HZPRN1X1);
	if ( ucRet != EM_SUCCESS )
	{
		lcddisperr("打印汉字大小设置失败");
		return notok;
	}	

	ucRet = EA_ucSetPrinterFormat(hDprinterHandle, EM_prn_PRNGRAY, 4);
	if ( ucRet != EM_SUCCESS )
	{
		lcddisperr("打印灰度设置失败");
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
		lcddisperr("COM口初始化失败!");
		return notok;
	}
	ucRet = EA_ucInitRS232(hRS232Handle, EM_232_115200BPS, EM_232_NOPAR, EM_232_8DBS);
	if ( ucRet != EM_SUCCESS )
	{
		(void)EA_ucCloseDevice(&hRS232Handle);
		lcddisperr("COM口设置失败!");
		return notok;
	}
//获取SD卡信息
	ucRet = EA_ucGetDiskInfo(EM_DISK_SDCARD, &ptDiskInfo);
	if ( ucRet != EM_SUCCESS )
	{
		lcddisperr("请插入SD卡!");
		return notok;
	}
	cap_sd = ptDiskInfo.ullSize / 0x100000;
	EA_vDisplay(3, "  SD卡可用容量%dM", cap_sd);
	SleepMs(2000);
	if(cap_sd < 1)
	{
		lcddisperr("请及时清理SD中数据!");
		Beeperr();
	}
//  // 分配空间来存储磁盘的分区列表
//  ptPartList = EA_pvMalloc(ptDiskInfo.uiPartNum * sizeof(ptPartList));
//  if (!ptPartList)
//  {
//  	lcddisperr("分配SD卡缓存区失败!");
//  	return notok;
//  }
//  // 获取磁盘的分区列表
//  ucRet = EA_ucGetPartList(EM_DISK_SDCARD, ptDiskInfo.uiPartNum, &uiRealPartNum, ptPartList);
//  if (ucRet != EM_SUCCESS)
//  {
//  	EA_vFree(ptPartList);
//  	lcddisperr("获取SD卡列表失败!");
//  	return notok;
//  }
//
//  EA_vDisplay(1, "分区数目:%d", ptDiskInfo.uiPartNum);
//  SleepMs(3000);

	ucRet = EA_ucGetPartInfo(EM_DISK_SDCARD, 0, &ptPartInfo);
	if (ucRet != EM_SUCCESS)
	{
		lcddisperr("获取SD卡分区信息失败!");
		return notok;
	}

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


	for ( ;; )
	{
		EA_vCls();
//  	Get_Time(&ltime);
		EA_vDisp(1, 1, "当前生效时间:");
		sprintf((void *)buf, "       %02X:%02X        ", DevStat.effect_time.hour, DevStat.effect_time.minute);
		EA_vDisp(2, 1, (void *)buf);
		EA_vDisp(3, 1, "请输入新的时间:");
		(void)EA_ucClrKeyBuf();
		strcpy((void *)input, "");

		i = EA_ucGetInputStr(4, 8, 20, EM_BIG_FONT | EM_MODE_NUMBER | EM_ALIGN_LEFT | EM_SHOW_ORIGINALLY
		, 2, 2, 0, (void *)input);
		if ( i == EM_ABOLISH )
			return;
		if ( i != EM_SUCCESS )
			continue;
		
		i = EA_ucGetInputStr(4, 11, 20, EM_BIG_FONT | EM_MODE_NUMBER | EM_ALIGN_LEFT | EM_SHOW_ORIGINALLY
		, 2, 2, 0, (void *)input);
		if ( i == EM_ABOLISH )
			return;
		if ( i != EM_SUCCESS )
			continue;
		//  	sprintf(dbuf, "%s", input);
		//  	EA_vDisp(3, 1, dbuf);
		//  	sprintf(dbuf, "result:%02X", i);
		//  	EA_vDisp(4, 1, dbuf);

		ltime.hour = (ascii_to_hex(input[8]) << 4) | ascii_to_hex(input[9]);
		ltime.minute = (ascii_to_hex(input[11]) << 4) | ascii_to_hex(input[12]);

		if ( ltime.hour > 0x23 )       //小时应在0-23之间
			return ;


		
		if ( ltime.minute > 0x59 )                                   //分钟应在0-59之间
			return ;


		if ( (ltime.hour > 0x23) || (ltime.minute > 0x59) )
		{
			EA_vCls();
			EA_vDisp(4, 1, "时间格式错误");
			SleepMs(1500);
			continue;
		}

		DevStat.effect_time.hour = ltime.hour;
		DevStat.effect_time.minute = ltime.minute;
		WriteParam();

		EA_vCls();
		EA_vDisp(4, 1, "生效时间修改成功");
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
	INT8U input[10];
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
		pf_format();
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
	close_gprs_module();
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
	close_gprs_module();
//  EA_vSetSaveResTime(uisleep_delay);
	EA_vSystemManage (EM_RESET);
	
	//(void)EA_uiInkey(0);
}