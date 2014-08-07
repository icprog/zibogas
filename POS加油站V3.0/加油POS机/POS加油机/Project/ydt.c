/******************************************************************************
Copyright 2005 大连易达通电子有限公司
All rights riserved.

文件名	：ydt.c
模块名称：
功能概要：

取代版本：0.0.1
修改人	：许岩
完成日期：2005.04.06
升级说明：create

******************************************************************************/
#include "main.h"

extern DEV_STAT DevStat;
extern CARD_INFO CardInfo;
//extern char dbuf[];

INT32U uiblackLen = sizeof(BlackCardInfo);
INT32U uiparamclen = ( &DevStat.end - &DevStat.start );
INT32U uicurrecLen = sizeof(RECORD);
//INT32U uiconrecLen = sizeof(CONSUMERECORD);
INT32U uihisrecLen = sizeof(RECORD);
//INT32U uigrayrecLen = sizeof(GRAYRECORD);
INT32U clerkLen = sizeof(CLERK_STRUCT);


extern CLERK_STRUCT clerk_list[MAX_CLERK_NUM];


const char parmFileName[] = "param";
const char blackFileName[] = "black";
const char currecFileName[] = "currec";
const char conrecFileName[] = "conrec";
const char hisrecFileName[] = "hisrec";
const char grayrecFileName[] = "grayrec";
const char clerkFileName[] = "clerk";
/*****************************************************************
函数原型：Get_Time
功能描述：读取时间，读取两次，为了防止偶尔一次不成功
参数描述：
参数名称：	输入/输出？	类型		描述
*time_data	输出   _TIME	7个字节长，数据从头到尾顺序是世纪 年 月 日 时 分 秒
返  回  值：ok(0)-成功 
		notok(0xFF)－失败
作      者：许岩
日      期：2005-04-12
修改历史：
日期		修改人		修改描述
------		---------	-------------
*****************************************************************/
INT8U Get_Time(BUS_TIME *time_data)
{
	ET_DATETIME time1;

	(void)EA_ucGetDateTime(&time1);

	time_data->century = 0x20;
	time_data->year = hextobcd((INT8U)(time1.usYear % 2000));
	time_data->month = hextobcd(time1.ucMonth);
	time_data->day = hextobcd(time1.ucDay);
	time_data->hour = hextobcd(time1.ucHour);
	time_data->minute = hextobcd(time1.ucMinute);
	time_data->second = hextobcd(time1.ucSecond);

	CardInfo.card_begin_dt[0] = 0x20;
	CardInfo.card_begin_dt[1] = time_data->year;
	CardInfo.card_begin_dt[2] = time_data->month;
	CardInfo.card_begin_dt[3] = time_data->day;

	CardInfo.card_useful_dt[0] = 0x20;
	CardInfo.card_useful_dt[1] = (time_data->year + 50)%100;
	CardInfo.card_useful_dt[2] = time_data->month;
	CardInfo.card_useful_dt[3] = time_data->day;
	
	CardInfo.card_add_dt[0] = time_data->year;
	CardInfo.card_add_dt[1] = time_data->month;
	CardInfo.card_add_dt[2] = time_data->day;
	CardInfo.card_add_dt[3] = time_data->hour;
	return ok;
}

/*****************************************************************
函数原型：Modify_Time
功能描述：修改时间，修改两次，为了防止偶尔一次不成功
参数描述：
参数名称：	输入/输出？	类型		描述
*time_data	输入	_TIME	7个字节长，数据从头到尾顺序是世纪 年 月 日 时 分 秒
返  回  值：ok(0)-成功 
		notok(0xFF)－失败
作      者	：许岩
日      期：2005-04-12
修改历史：
日期		修改人		修改描述
------		---------	-------------
*****************************************************************/
INT8U Modify_Time(BUS_TIME const * const time_data)
{
	ET_DATETIME time1;

	time1.usYear = 2000 + (INT16U)bcdtohex(time_data->year);
	time1.ucMonth = bcdtohex(time_data->month);
	time1.ucDay = bcdtohex(time_data->day);
	time1.ucHour = bcdtohex(time_data->hour);
	time1.ucMinute = bcdtohex(time_data->minute);
	time1.ucSecond = bcdtohex(time_data->second);

	EA_ucSetDateTime(&time1, EM_SYSTEM_DATE | EM_SYSTEM_TIME);

	return ok;
}

uchar pf_exist(uchar *pucFileName, uchar *pucFileID)
{
	uchar ucret;
	ucret = EA_ucPFOpen((uchar *)pucFileName, (uchar *)pucFileID);
	if(ucret == EM_ffs_NOTEXIST)
		return FALSE;
	else if(ucret == EM_SUCCESS)
	{
		EA_ucPFClose((uchar)*pucFileID);
		return TRUE;
	}
	return FALSE;
}

uchar pf_open(uchar *pucFileName, uchar *pucFileID)
{
	uchar ucret;
	ucret = EA_ucPFOpen((uchar *)pucFileName, (uchar *)pucFileID);
	return ucret;
}

uchar pf_creat(uchar *pucFileName, uint uiRecLen)
{
	uchar ucret;
	ucret = EA_ucPFCreate((uchar *)pucFileName, uiRecLen);
	return ucret;
}

uchar pf_read(uchar ucFileID, uint uiStartRec, uint uiRecCntR, uint *puiRecCntW, uchar *pucString)
{
	return(EA_ucPFReadRec(ucFileID, uiStartRec, uiRecCntR, (uint *)puiRecCntW, (uchar *)pucString));
}

uchar pf_close(uchar ucFileID)
{
	return( EA_ucPFClose((uchar)ucFileID) );
}

uchar pf_write(uchar ucFileID, uint uiRecID, uchar *pucString)
{
	return( EA_ucPFWriteRec((uchar)ucFileID, (uint)uiRecID, (uchar *)pucString) );
}
/*****************************************************************
函数原型：CheckTimeFormat
功能描述：检查时间格式是否合法
参数描述：
参数名称：	输入/输出？	类型		描述
time		输入  			const _TIME *	7个字节长，数据从头到尾顺序是世纪 年 月 日 时 分 秒

返  回  值：ok(0)-成功 
		notok(0xFF)－失败
作      者	：许岩
日      期：2003-05-12
修改历史：
日期		修改人		修改描述
------		---------	-------------
*****************************************************************/
INT8U CheckTimeFormat(const BUS_TIME *time)
{
	INT8U temp;

	temp = time->month;
	if ( temp >= 0x0a )       //月份应在1－12之间
	{
		if ( temp >= 0x10 && temp <= 0x12 )
		{
			;
		}
		else
		{
			return notok;
		}
	}

	temp = time->day;
	if ( temp > 0x31 )                   //天应在1-31之间
		return notok;
	temp = temp & 0x0F;
	if ( temp > 0x09 )
		return notok;

	temp = time->hour;
	if ( temp > 0x23 )                                   //小时应在0-23之间
		return notok;
	temp = temp & 0x0F;
	if ( temp > 0x09 )
		return notok;

	temp = time->minute;
	if ( temp > 0x59 )                                   //分钟应在0-59之间
		return notok;
	temp = temp & 0x0F;
	if ( temp > 0x09 )
		return notok;

	temp = time->second;
	if ( temp > 0x59 )                                    //秒应在0-59之间
		return notok;
	temp = temp & 0x0F;
	if ( temp > 0x09 )
		return notok;

	return ok;
}

/******************************************************************************
 函数名称：ascii_to_hex
 功能描述：把一个0-9的ascii代码，转化为16进制数
 参数名称： 输入/输出？ 类型        描述
 num        输入        INT8U           要转化为ascii代码的数，要求范围0-9 a-f A-F

 返  回  值：转化后的16进制数

 作      者 ：许岩
 日      期：2004-09-02
 修改历史：
	日期        修改人      修改描述
	------      ---------   -------------
******************************************************************************/
INT8U ascii_to_hex(INT8U num)
{
	//  ToggleWD();

	// 	return (num - 0x30);
	if ( num <= '9' )
		return ( num - 0x30 );
	else if ( num == 'a' || num == 'A' )
		return 0x0A;
	else if ( num == 'b' || num == 'B' )
		return 0x0B;
	else if ( num == 'c' || num == 'C' )
		return 0x0C;
	else if ( num == 'd' || num == 'D' )
		return 0x0D;
	else if ( num == 'e' || num == 'E' )
		return 0x0E;
	else
		return 0x0F;
}

/*****************************************************************
函数原型：bcdtohex
功能描述：bcd码转换为hex码
参数描述：
参数名称：	输入/输出？	类型		描述
-----------		-----------	------	   	-------
bcd				输入			INT8U		输入的bcd码

返  回  值：转换后的hex码
作      者	：许岩
日      期：2006-01-09
修改历史：
日期		修改人		修改描述
------		---------	-------------
*****************************************************************/
INT8U bcdtohex(INT8U bcd)
{
	return ( (bcd >> 4) * 10 + (bcd & 0x0F) );
}

/******************************************************************************
 函数名称：hextobcd
 功能描述：把一个16进制数，转化为bcd码
 参数名称： 输入/输出？ 类型        描述
 num        输入        INT8U           要转化为bcd码的数

 返  回  值：转化后的bcd码

 作      者 ：许岩
 日      期：2004-09-02
 修改历史：
	日期        修改人      修改描述
	------      ---------   -------------
******************************************************************************/
INT8U hextobcd(INT8U num)
{
	return ( ((num / 10) << 4) | (num % 10) );
}

/******************************************************************************
 函数名称：atoh
 功能描述：ascii转换为hex码
 参数描述：
 参数名称： 输入/输出？ 类型		描述
		
 返  回  值：无
		   
 作 	 者 ：许岩
 日 	 期：2005-04-07
 修改历史：
	日期		修改人		修改描述
	------		---------	-------------
******************************************************************************/
void atoh(void *des, INT8U *sour, int ascii_len)
{
	int i = 0;
	char *dest = (char *)des;

	//  ToggleWD();

	if ( ascii_len <= 0 )
		return;

	for ( i = 0; i < ascii_len; i++ )
	{
		if ( i % 2 == 0 )
			continue;

		dest[i / 2] = (ascii_to_hex(sour[i - 1]) << 4) | ascii_to_hex(sour[i]);
	}
}

/******************************************************************************
 函数名称：ascii_comm
 功能描述：把一个0-9的数转化为ascii代码，此函数也可扩展用于通讯中，通讯中涉及到一些A-F的非法输入，认为合法
 参数名称：	输入/输出？	类型		描述
 num		输入		INT8U			要转化为ascii代码的数，要求范围0-9
 
 返  回  值：转化后的ascii代码
 
 作      者	：许岩
 日      期：2004-09-02
 修改历史：
	日期		修改人		修改描述
	------		---------	-------------
******************************************************************************/
INT8U ascii_comm(INT8U num)
{
	return ( num + 0x30 );
}

/******************************************************************************
 函数名称：SleepMs
 功能描述：延时函数，11.0592M*4晶振专用，开mem加速
 参数名称：	输入/输出？	类型		描述
 _ms	输入	INT16U				要延时的倍数 1ms*?
 
 返  回  值：无
 
 作      者：许岩
 日      期：2004-09-02
 修改历史：
	日期		修改人		修改描述
	------		---------	-------------
******************************************************************************/
void SleepMs(INT16U _ms)
{
	EA_uiInkeyMs(_ms);
	EA_ucClrKeyBuf();
	return;
}

/******************************************************************************
 函数名称：lcddisp
 功能描述：显示函数
 参数名称：	输入/输出？	类型		描述
 line		输入		INT16U		行 1~4
 col		输入		INT16U		列 1~20
 str		输入		INT8U *		要显示的内容
 
 返  回  值：无
 
 作      者：许岩
 日      期：2012-10-29
 修改历史：
	日期		修改人		修改描述
	------		---------	-------------
******************************************************************************/
void lcddisp(INT16U line, INT16U col, const INT8U *str)
{
	EA_vDisp((int)line, (int)col, (char *)str);
}

/******************************************************************************
 函数名称：browse_menu
 功能描述：菜单函数
 参数名称：	输入/输出？	类型		描述
 startline	输入		INT8U		菜单起始显示行 1~4
 title		输入		INT8U *		菜单提示, 不超过8个汉字, 在第一行显示
 p_menu		输入		INT8U *		菜单，每行10个汉字，不够补空格  
 allow_exit	输入		INT8U		是否允许按取消键退出TRUE=允许 FALSE=不允许 
 
 返  回  值：选择的菜单号  0为第一条
		-1 = 取消键 
 
 作      者：许岩
 日      期：2012-10-29
 修改历史：
	日期		修改人		修改描述
	------		---------	-------------
******************************************************************************/
INT8S browse_menu(INT8U startline, INT8U *title, INT8U *p_menu, INT8U allow_exit)
{
	INT16U len = 0;
	INT16U menu_lines = 0;
	INT16U i = 0;
#define MAX_MENU_LINES 100
	INT8U menu_text[MAX_MENU_LINES][22];
	//  INT16U j = 0;
	INT16U key = EM_key_NOHIT;
	struct _MENU_PARAM
	{
		INT16S disp_pos;
		INT16S menu_pos;
	}lmenu;
	INT16S h = 0;
	INT16U disp_pos_max = 0;
	INT16S m = 0;
	//  INT8S buf[30];


	startline = startline;              //avoid warning

	EA_vCls();
	EA_ucSetInverse(EM_lcd_INVOFF);

	//显示title
	EA_vTextOut(0, 0, EM_key_FONT8X16, 0, 1, 1, (char *)title);

	//菜单文字初始化
	len = strlen((char *)p_menu);
	menu_lines = len / 20;
	if ( menu_lines > MAX_MENU_LINES )
		menu_lines = MAX_MENU_LINES;
	//  if ( menu_lines <= 1 )
	//  	return 0;
	if ( menu_lines == 0 )
		return -1;
	memset((void *)&menu_text[0][0], 0x00, sizeof(menu_text));
	for ( i = 0; i < menu_lines; i++ )
	{
		memcpy((void *)&menu_text[i][0], (void *)&p_menu[i * 20], 20);
	}


	if ( menu_lines < 4 )
		disp_pos_max = menu_lines - 1;
	else
		disp_pos_max = 3;

	lmenu.disp_pos = 0;
	lmenu.menu_pos = 0;

	EA_vCls();   //清除屏幕
	EA_ucSetInverse(EM_lcd_INVON);
	lcddisp(lmenu.disp_pos + 1, 1, &menu_text[lmenu.menu_pos][0]);       //显示当前菜单，反显
	EA_ucSetInverse(EM_lcd_INVOFF);

	m = 1;
	for ( i = lmenu.disp_pos + 1; i <= 3; i++ )
	{
		lcddisp(i + 1, 1, &menu_text[lmenu.menu_pos + m][0]);
		m++;
	}
	m = 1;
	for ( h = lmenu.disp_pos - 1; h >= 0; h-- )
	{
		lcddisp(h + 1, 1, &menu_text[lmenu.menu_pos - m][0]);
		m++;
	}

	for (;;)
	{
		key = EA_uiInkey(0);
		switch ( key )
		{
		   case EM_key_INTERPUNCTION:           //上
			   lmenu.disp_pos--;
			   if ( lmenu.disp_pos < 0 )
				   lmenu.disp_pos = 0;
			   lmenu.menu_pos--;
			   if ( lmenu.menu_pos < 0 )
				   lmenu.menu_pos = 0;


			   EA_vCls();   //清除屏幕
			   EA_ucSetInverse(EM_lcd_INVON);
			   lcddisp(lmenu.disp_pos + 1, 1, &menu_text[lmenu.menu_pos][0]);       //显示当前菜单，反显
			   EA_ucSetInverse(EM_lcd_INVOFF);

			   m = 1;
			   for ( i = lmenu.disp_pos + 1; i <= 3; i++ )
			   {
				   lcddisp(i + 1, 1, &menu_text[lmenu.menu_pos + m][0]);
				   m++;
			   }
			   m = 1;
			   for ( h = lmenu.disp_pos - 1; h >= 0; h-- )
			   {
				   lcddisp(h + 1, 1, &menu_text[lmenu.menu_pos - m][0]);
				   m++;
			   }

			   break;

		   case EM_key_LETTER:                  //下
			   lmenu.disp_pos++;
			   if ( lmenu.disp_pos > disp_pos_max )
				   lmenu.disp_pos = disp_pos_max;
			   lmenu.menu_pos++;
			   if ( lmenu.menu_pos > menu_lines - 1 )
				   lmenu.menu_pos = menu_lines - 1;


			   EA_vCls();   //清除屏幕
			   EA_ucSetInverse(EM_lcd_INVON);
			   lcddisp(lmenu.disp_pos + 1, 1, &menu_text[lmenu.menu_pos][0]);       //显示当前菜单，反显
			   EA_ucSetInverse(EM_lcd_INVOFF);

			   m = 1;
			   for ( i = lmenu.disp_pos + 1; i <= 3; i++ )
			   {
				   lcddisp(i + 1, 1, &menu_text[lmenu.menu_pos + m][0]);
				   m++;
			   }
			   m = 1;
			   for ( h = lmenu.disp_pos - 1; h >= 0; h-- )
			   {
				   lcddisp(h + 1, 1, &menu_text[lmenu.menu_pos - m][0]);
				   m++;
			   }

			   break;

		   case EM_key_ENTER:                   //确认
			   return lmenu.menu_pos;

		   case EM_key_1:
			   if ( menu_lines >= 1 )
				   return 0;
			   else
				   break;

		   case EM_key_2:
			   if ( menu_lines >= 2 )
				   return 1;
			   else
				   break;

		   case EM_key_3:
			   if ( menu_lines >= 3 )
				   return 2;
			   else
				   break;

		   case EM_key_4:
			   if ( menu_lines >= 4 )
				   return 3;
			   else
				   break;

		   case EM_key_5:
			   if ( menu_lines >= 5 )
				   return 4;
			   else
				   break;

		   case EM_key_6:
			   if ( menu_lines >= 6 )
				   return 5;
			   else
				   break;

		   case EM_key_7:
			   if ( menu_lines >= 7 )
				   return 6;
			   else
				   break;

		   case EM_key_8:
			   if ( menu_lines >= 8 )
				   return 7;
			   else
				   break;

		   case EM_key_9:
			   if ( menu_lines >= 9 )
				   return 8;
			   else
				   break;


		   case EM_key_CANCEL:
			   if ( allow_exit == TRUE )
				   return -1;
			   else
				   break;

		   default:
			   break;
		}

	}
	return 0;
}


/******************************************************************************
 函数名称：lcddisperr
 功能描述：液晶屏提示错误
 参数名称：	输入/输出？	类型		描述
 str		输入		char *		要显示的内容
 
 返  回  值：无
 
 作      者：许岩
 日      期：2012-10-29
 修改历史：
	日期		修改人		修改描述
	------		---------	-------------
******************************************************************************/
void lcddisperr(const char *str)
{
	INT16U key = EM_key_NOHIT;
	uchar  len=0;

	len = strlen((void *)str);
	if( len <= 20 )
	{
		EA_vCls();
		EA_vDisp(3, (20 -len)/2 + 1, (char *)str);
	}
	else if( len > 20 )
	{
		EA_vCls();
		EA_vDisp(3, 1, (char *)str);
	}

	key = EA_uiInkeyMs(1200);
//  (void)EA_uiInkeyMs(500);
	EA_vDisp(3, 1, "                      ");
}

/*****************************************************************
 函数原型：param_factory_default()
 功能描述：参数恢复出厂设置
 参数描述：	
 参数名称：	输入/输出？	类型		描述
 level		输入		INT8U		初始化的等级0-不初始化记录和参数文件 1-记录和参数文件都初始化
				 
 返回值：	无
 
 作  者：	许岩
 日  期：	2004-09-17
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
*****************************************************************/
void param_factory_default(INT8U level)
{
	//建立5个记录文件包含，参数表，白名单，交易记录、
	//历史记录、员工名单
	INT8U i = 0;
	//重新参数表
	 i = EA_ucPFCreate((uchar *)parmFileName, uiparamclen);  
	 if ( i == EM_ffs_FILEEXIST )          //文件已经存在
	 {
		 if ( EA_ucPFDel((uchar *)parmFileName) != EM_ffs_SUCCESS )
		 {
			 lcddisperr("删除参数表失败");
		 }
		 if ( EA_ucPFCreate((uchar *)parmFileName, uiparamclen) == EM_ffs_SUCCESS )
		 {
			 lcddisperr("重新创建参数表");
		 }
	 }
	 else if ( i == EM_ffs_FILEFULL )
	 {
		 lcddisperr("创建参数表数满");
	 }
	 else if ( i == EM_ffs_SUCCESS )
	 {
		 lcddisperr("创建参数表成功");
	 }
	 else
	 {
		 lcddisperr("创建参数表失败");
	 }
//
//   //员工登陆文件
//   i = EA_ucPFCreate((uchar *)clerkFileName, sizeof(clerk_list));
//   if ( i == EM_ffs_FILEEXIST )          //文件已经存在
//   {
//  	 if ( EA_ucPFDel((uchar *)clerkFileName) != EM_ffs_SUCCESS )
//  	 {
//  		 lcddisperr("删除员工表失败");
//  	 }
//  	 if ( EA_ucPFCreate((uchar *)clerkFileName, sizeof(clerk_list)) == EM_ffs_SUCCESS )
//  	 {
//  		 lcddisperr("重新创建员工表");
//  	 }
//   }
//   else if ( i == EM_ffs_FILEFULL )
//   {
//  	 lcddisperr("创建员工表数满");
//   }
//   else if ( i == EM_ffs_SUCCESS )
//   {
//  	 lcddisperr("创建员工表成功");
//   }
//   else
//   {
//  	 lcddisperr("创建员工表失败");
//   }
//
//  for (i=0; i<MAX_CLERK_NUM; i++)
//  {
//  	clerk_list[i].id[0] = '0';
//  	clerk_list[i].id[1] = '0'+i;
//  	clerk_list[i].right = ID_OPERATOR;
//  	memset(clerk_list[i].pw,'0'+i,4);
//  }
//
//  zbClerkSaveFile();

	memset((void *)&DevStat, 0x00, sizeof(DevStat));
	strcpy((void *)&DevStat.user_passwd[0], "000000");
 	
	DevStat.driver_is_working = FALSE;
	memset(&DevStat.dev_trade_no[0], 0, 2);
	DevStat.mode = 0x00;
	DevStat.samcompcode = 1;
	DevStat.Gprs_On_Flag = 0;
	DevStat.effect_time.hour = 0x07;
	memset(&DevStat.binhand, 0x00, sizeof(DevStat.binhand));

	WriteParam();

	if ( level == 0 )
	{
		return;
	}
	else if ( level == 1 )
	{
		lcddisperr("磁盘格式化中...");
		i = EA_ucPFFormat();       //磁盘格式化
		if( i != EM_ffs_SUCCESS)
		{
			lcddisperr("磁盘格式化失败");
		}
		//重新创建白名单文件
		i = EA_ucBlkCreate((char *)blackFileName, uiblackLen, 2000);
		if ( i == EM_bfs_FILE_EXIST )          //文件已经存在
		{
			if (  EA_ucBlkDelete((char *)blackFileName) != EM_SUCCESS )
			{
				lcddisperr("删除白名单文件失败");
			}
			if (EA_ucBlkCreate((char *)blackFileName, uiblackLen, 2000) == EM_SUCCESS )
			{
				lcddisperr("重新创建白名单");
			}
		}
		else if ( i == EM_bfs_NO_SPACE )
		{
			lcddisperr("创建白名单数满");
		}
		else if ( i == EM_ffs_SUCCESS )
		{
			lcddisperr("创建白名单成功");
		}
		else
		{
			lcddisperr("创建白名单失败");
		}
		//重新创建记录文件
		 i = EA_ucPFCreateLoop((uchar *)currecFileName, uicurrecLen, 2000);  //1000条循环记录;
		 if ( i == EM_ffs_FILEEXIST )          //文件已经存在
		 {
			 if ( EA_ucPFDel((uchar *)currecFileName) != EM_ffs_SUCCESS )
			 {
				 lcddisperr("删除消费记录失败");
			 }
			 if ( EA_ucPFCreateLoop((uchar *)currecFileName, uicurrecLen, 2000) == EM_ffs_SUCCESS )
			 {
				 lcddisperr("重建消费记录");
			 }
		 }
		 else if ( i == EM_ffs_FILEFULL )
		 {
			 lcddisperr("创建消费记录数已满");
		 }
		 else if ( i == EM_ffs_SUCCESS )
		 {
			 lcddisperr("创建消费记录成功");         
		 }
		 else
		 {
			 lcddisperr("创建消费记录失败");
		 }
		 //重新创建历史记录文件
		 i = EA_ucPFCreateLoop((uchar *)hisrecFileName, uihisrecLen, 10000);
		 if ( i == EM_ffs_FILEEXIST )          //文件已经存在
		 {
			 if ( EA_ucPFDel((uchar *)hisrecFileName) != EM_ffs_SUCCESS )
			 {
				 lcddisperr("删除历史记录文件失败");
			 }
			 if ( EA_ucPFCreateLoop((uchar *)hisrecFileName, uihisrecLen, 10000) == EM_ffs_SUCCESS )
			 {
				 lcddisperr("重建历史记录");
			 }
		 }
		 else if ( i == EM_ffs_FILEFULL )
		 {
			 lcddisperr("创建历史记录数已满");
		 }
		 else if ( i == EM_ffs_SUCCESS )
		 {
			 lcddisperr("创建历史记录成功");
		 }
		 else
		 {
			 lcddisperr("创建历史记录失败");
		 }
		 //重新日结文件
		 i = EA_ucPFCreateLoop((uchar *)conrecFileName, uicurrecLen, 800);  //1000条循环记录;
		 if ( i == EM_ffs_FILEEXIST )          //文件已经存在
		 {
			 if ( EA_ucPFDel((uchar *)conrecFileName) != EM_ffs_SUCCESS )
			 {
				 lcddisperr("删除日结文件失败");
			 }
			 if ( EA_ucPFCreateLoop((uchar *)conrecFileName, uicurrecLen, 800) == EM_ffs_SUCCESS )
			 {
				 lcddisperr("重建日结文件记录");
			 }
		 }
		 else if ( i == EM_ffs_FILEFULL )
		 {
			 lcddisperr("创建日结数已满");
		 }
		 else if ( i == EM_ffs_SUCCESS )
		 {
			 lcddisperr("创建日结成功");         
		 }
		 else
		 {
			 lcddisperr("创建日结失败");
		 }
	}

	return;
}

/******************************************************************************
 函数名称：WriteParam
 功能描述：保存参数
 参数描述：
 参数名称： 输入/输出？ 类型        描述

 返  回  值：无

 作      者 ：许岩/刘及华
 日      期：2012-11-24
 修改历史：
		日期        修改人      修改描述
		------      ---------   -------------
******************************************************************************/
void WriteParam(void)
{
	INT8U ucOpenID = 0;
	//打开参数文件
	if ( EA_ucPFOpen((uchar *)parmFileName, &ucOpenID) != EM_SUCCESS )
	{
		lcddisperr("打开参数表失败");
		return;
	}	
	if ( EA_ucPFWriteRec(ucOpenID, 0, &DevStat.start) != EM_SUCCESS )
	{
		lcddisperr("添加参数表失败");
		return;
	}

	if ( EA_ucPFClose(ucOpenID) != EM_SUCCESS )
	{
		lcddisperr("关闭参数表失败");
		return;
	}
	
}

/******************************************************************************
 函数名称：ReadParam
 功能描述：读取参数
 参数描述：
 参数名称： 输入/输出？ 类型        描述

 返  回  值：ok ? notok

 作      者 ：许岩/刘及华
 日      期：2012-11-24
 修改历史：
		日期        修改人      修改描述
		------      ---------   -------------
******************************************************************************/
INT8U ReadParam(void)
{
	uchar ucOpenID = 0;
	uint  uiReadCnt = 0;
	uchar i;
	uint  uiRetNum;
	// 打开参数表文件
	if ( EA_ucPFOpen((INT8U *)parmFileName, &ucOpenID) != EM_SUCCESS )
	{
		(void)EA_ucPFClose(ucOpenID);
		return notok;
	}
	//读取数据，最后一条有效参数表信息
	i = EA_ucPFInfo(ucOpenID, EM_ffs_VALIDREC, &uiReadCnt);
	if ( i != EM_ffs_SUCCESS )
	{
		(void)EA_ucPFClose(ucOpenID);
		return notok;
	}
	if( uiReadCnt == 0)
	{
		lcddisperr("参数表为空");
		(void)EA_ucPFClose(ucOpenID);
		return notok;
	}
	if ( EA_ucPFReadRec(ucOpenID, uiReadCnt, 1, &uiRetNum, &DevStat.start ) != EM_SUCCESS )
	{
		(void)EA_ucPFClose(ucOpenID);
		return notok;
	}
	//关闭文件
	(void)EA_ucPFClose(ucOpenID);
	return ok;
}

/*****************************************************************
 函数原型：Cal_Crc
 功能描述：计算一串数据的CRC值，采用CCITT算法
 参数描述：
 参数名称：	输入/输出？	类型		描述
 data_to_cal_crc	输入	INT8U const *		要进行CRC计算的数组的起始指针
 len				 		输入	INT16U				要进行CRC计算的数组的长度

 返  回  值：uint型的CRC值(2字节)
 作      者	：许岩
 日      期：2004-02-04
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
*****************************************************************/
INT16U Cal_Crc(INT8U const *data_to_cal_crc, INT16U len)
{
	INT16U crc;
	INT8U i;

	crc = 0xFFFF;           //CRC初始值

	while ( len-- != 0 )
	{
		crc = crc ^ ((INT16U) * data_to_cal_crc++ << 8);

		for ( i = 0; i < 8; ++i )
			if ( crc & 0x8000 )
				crc = (crc << 1) ^ 0x1021;
			else
				crc	= crc << 1;
	}

	return crc;
}


/******************************************************************************
 函数名称：browse_menu_time
 功能描述：菜单函数，带显示时间的，主菜单专用
 参数名称：	输入/输出？	类型		描述
 startline	输入		INT8U		菜单起始显示行 1~4
 title		输入		INT8U *		菜单提示, 不超过8个汉字, 在第一行显示
 p_menu		输入		INT8U *		菜单，每行10个汉字，不够补空格  
 allow_exit	输入		INT8U		是否允许按取消键退出TRUE=允许 FALSE=不允许 
 
 返  回  值：选择的菜单号  0为第一条
		-1 = 取消键 
 
 作      者：许岩
 日      期：2012-10-29
 修改历史：
	日期		修改人		修改描述
	------		---------	-------------
******************************************************************************/
INT8S browse_menu_time(INT8U startline, INT8U *title, INT8U *p_menu, INT8U allow_exit)
{
	INT16U len = 0;
	INT16U menu_lines = 0;
	INT16U i = 0;
#define MAX_MENU_LINES 100
	INT8U menu_text[MAX_MENU_LINES][22];
	//  INT16U j = 0;
	INT16U key = EM_key_NOHIT;
	struct _MENU_PARAM
	{
		INT16S disp_pos;
		INT16S menu_pos;
	}lmenu;
	INT16S h = 0;
	INT16U disp_pos_max = 0;
	INT16S m = 0;
	INT8S buf[30];
	BUS_TIME ltime;


	startline = startline;              //avoid warning

	EA_vCls();
	EA_ucSetInverse(EM_lcd_INVOFF);

	//  //显示title
	//  EA_vTextOut(0, 0, EM_key_FONT8X16, 0, 1, 1, (char *)title);
	//2012-11-16 14:25
	Get_Time(&ltime);
	sprintf((void *)buf, "%02X%02X-%02X-%02X %02X:%02X",
			ltime.century, ltime.year, ltime.month, ltime.day, ltime.hour, ltime.minute);
	EA_vTextOut(0, 0, EM_key_FONT8X16, 0, 1, 1, (char *)buf);

	//菜单文字初始化
	len = strlen((char *)p_menu);
	menu_lines = len / 20;
	if ( menu_lines > MAX_MENU_LINES )
		menu_lines = MAX_MENU_LINES;
	//  if ( menu_lines <= 1 )
	//  	return 0;
	if ( menu_lines == 0 )
		return -1;
	memset((void *)&menu_text[0][0], 0x00, sizeof(menu_text));
	for ( i = 0; i < menu_lines; i++ )
	{
		memcpy((void *)&menu_text[i][0], (void *)&p_menu[i * 20], 20);
	}


	if ( menu_lines < 4 )
		disp_pos_max = menu_lines - 1;
	else
		disp_pos_max = 3;
	//  EA_ucSetInverse(EM_lcd_INVON);
	//  lcddisp(1, 1, &menu_text[0][0]);
	//  EA_ucSetInverse(EM_lcd_INVOFF);
	//  for ( i = 1; i < disp_pos_max; i++ )
	//  {
	//  	lcddisp(i + 1, 1, &menu_text[i][0]);
	//  }

	lmenu.disp_pos = 0;
	lmenu.menu_pos = 0;

	EA_vCls();   //清除屏幕
	EA_ucSetInverse(EM_lcd_INVON);
	lcddisp(lmenu.disp_pos + 1, 1, &menu_text[lmenu.menu_pos][0]);       //显示当前菜单，反显
	EA_ucSetInverse(EM_lcd_INVOFF);

	m = 1;
	for ( i = lmenu.disp_pos + 1; i <= 3; i++ )
	{
		lcddisp(i + 1, 1, &menu_text[lmenu.menu_pos + m][0]);
		m++;
	}
	m = 1;
	for ( h = lmenu.disp_pos - 1; h >= 0; h-- )
	{
		lcddisp(h + 1, 1, &menu_text[lmenu.menu_pos - m][0]);
		m++;
	}


	for (;;)
	{
		Get_Time(&ltime);
		sprintf((void *)buf, "%02X%02X-%02X-%02X %02X:%02X",
				ltime.century, ltime.year, ltime.month, ltime.day, ltime.hour, ltime.minute);
		EA_vTextOut(0, 0, EM_key_FONT8X16, 0, 1, 1, (char *)buf);

		key = EA_uiInkeyMs(2000);
		switch ( key )
		{
		   case EM_key_INTERPUNCTION:           //上
			   lmenu.disp_pos--;
			   if ( lmenu.disp_pos < 0 )
				   lmenu.disp_pos = 0;
			   lmenu.menu_pos--;
			   if ( lmenu.menu_pos < 0 )
				   lmenu.menu_pos = 0;


			   EA_vCls();   //清除屏幕
			   EA_ucSetInverse(EM_lcd_INVON);
			   lcddisp(lmenu.disp_pos + 1, 1, &menu_text[lmenu.menu_pos][0]);       //显示当前菜单，反显
			   EA_ucSetInverse(EM_lcd_INVOFF);

			   m = 1;
			   for ( i = lmenu.disp_pos + 1; i <= 3; i++ )
			   {
				   lcddisp(i + 1, 1, &menu_text[lmenu.menu_pos + m][0]);
				   m++;
			   }
			   m = 1;
			   for ( h = lmenu.disp_pos - 1; h >= 0; h-- )
			   {
				   lcddisp(h + 1, 1, &menu_text[lmenu.menu_pos - m][0]);
				   m++;
			   }

			   break;

		   case EM_key_LETTER:                  //下
			   lmenu.disp_pos++;
			   if ( lmenu.disp_pos > disp_pos_max )
				   lmenu.disp_pos = disp_pos_max;
			   lmenu.menu_pos++;
			   if ( lmenu.menu_pos > menu_lines - 1 )
				   lmenu.menu_pos = menu_lines - 1;


			   EA_vCls();   //清除屏幕
			   EA_ucSetInverse(EM_lcd_INVON);
			   lcddisp(lmenu.disp_pos + 1, 1, &menu_text[lmenu.menu_pos][0]);       //显示当前菜单，反显
			   EA_ucSetInverse(EM_lcd_INVOFF);

			   m = 1;
			   for ( i = lmenu.disp_pos + 1; i <= 3; i++ )
			   {
				   lcddisp(i + 1, 1, &menu_text[lmenu.menu_pos + m][0]);
				   m++;
			   }
			   m = 1;
			   for ( h = lmenu.disp_pos - 1; h >= 0; h-- )
			   {
				   lcddisp(h + 1, 1, &menu_text[lmenu.menu_pos - m][0]);
				   m++;
			   }
			   //  		   for ( h = lmenu.disp_pos - 1; h >= 0; h-- )
			   //  		   {
			   //  			   lcddisp(h + 1, 1, &menu_text[lmenu.menu_pos - (lmenu.disp_pos - h)][0]);
			   //  		   }


			   break;

		   case EM_key_ENTER:                   //确认
			   return lmenu.menu_pos;
			   //  		   //debug
			   //  		   sprintf((void *)buf, "%d", lmenu.menu_pos + 1);
			   //  		   EA_vTextOut(0, 0, EM_key_FONT8X16, 0, 1, 1, (void *)buf);
			   //  		   break;
			   //  		   //debug end


		   case EM_key_1:
			   if ( menu_lines >= 1 )
				   return 0;
			   else
				   break;

		   case EM_key_2:
			   if ( menu_lines >= 2 )
				   return 1;
			   else
				   break;

		   case EM_key_3:
			   if ( menu_lines >= 3 )
				   return 2;
			   else
				   break;

		   case EM_key_4:
			   if ( menu_lines >= 4 )
				   return 3;
			   else
				   break;

		   case EM_key_5:
			   if ( menu_lines >= 5 )
				   return 4;
			   else
				   break;

		   case EM_key_6:
			   if ( menu_lines >= 6 )
				   return 5;
			   else
				   break;

		   case EM_key_7:
			   if ( menu_lines >= 7 )
				   return 6;
			   else
				   break;

		   case EM_key_8:
			   if ( menu_lines >= 8 )
				   return 7;
			   else
				   break;

		   case EM_key_9:
			   if ( menu_lines >= 9 )
				   return 8;
			   else
				   break;


		   case EM_key_CANCEL:
			   if ( allow_exit == TRUE )
				   return -1;
			   else
				   break;

		   default:
			   break;
		}

	}

	return 0;
}

/******************************************************************************
 函数名称：get_input_str
 功能描述：获取字符串
 参数名称：	输入/输出？	类型		描述
 iStartLine	输入		int			输入起始行
 iStartCol	输入		int			输入起始列
 uiMinText	输入		uint		菜单，每行10个汉字，不够补空格  
 uiMaxText	输入		uint		是否允许按取消键退出TRUE=允许 FALSE=不允许 
 
 返  回  值：选择的菜单号  0为第一条
		-1 = 取消键 
 
 作      者：许岩
 日      期：2012-10-29
 修改历史：
	日期		修改人		修改描述
	------		---------	-------------
******************************************************************************/
INT8U get_input_str(int iStartLine, int iStartCol, uint uiMinText, uint uiMaxText, char *pcString)
{
	INT16U key = EM_key_NOHIT;
	char buf[100];
	char clr_buf[100];
	INT32U i = 0;

	(void)EA_ucClrKeyBuf();
	strcpy(clr_buf, "");
	for ( i = 0; i < uiMaxText; i++ )
	{
		strcat(clr_buf, " ");
	}

	strcpy(buf, "");

	for (;;)
	{
		lcddisp(iStartLine, iStartCol, (void *)buf);
		key = EA_uiInkey(0);
		switch ( key )
		{
		   case EM_key_0:
			   if ( strlen(buf) < uiMaxText )
				   strcat(buf, "0");
			   break;

		   case EM_key_1:
			   if ( strlen(buf) < uiMaxText )
				   strcat(buf, "1");
			   break;

		   case EM_key_2:
			   if ( strlen(buf) < uiMaxText )
				   strcat(buf, "2");
			   break;

		   case EM_key_3:
			   if ( strlen(buf) < uiMaxText )
				   strcat(buf, "3");
			   break;

		   case EM_key_4:
			   if ( strlen(buf) < uiMaxText )
				   strcat(buf, "4");
			   break;

		   case EM_key_5:
			   if ( strlen(buf) < uiMaxText )
				   strcat(buf, "5");
			   break;

		   case EM_key_6:
			   if ( strlen(buf) < uiMaxText )
				   strcat(buf, "6");
			   break;

		   case EM_key_7:
			   if ( strlen(buf) < uiMaxText )
				   strcat(buf, "7");
			   break;

		   case EM_key_8:
			   if ( strlen(buf) < uiMaxText )
				   strcat(buf, "8");
			   break;

		   case EM_key_9:
			   if ( strlen(buf) < uiMaxText )
				   strcat(buf, "9");
			   break;

		   case EM_key_INTERPUNCTION:           //上
			   if ( strlen(buf) < uiMaxText )
			   {
				   if ( strstr(buf, ".") == NULL )
					   strcat(buf, ".");
			   }
			   break;

		   case EM_key_CLEAR:
			   if ( strlen(buf) > 0 )
			   {
				   buf[strlen(buf) - 1] = '\0';
				   lcddisp(iStartLine, iStartCol, (void *)clr_buf);
			   }
			   break;

		   case EM_key_CANCEL:
			   return EM_ABOLISH;

		   case EM_key_ENTER:                   //确认
			   if ( strlen(buf) < uiMinText )
			   {
				   break;
			   }
			   else
			   {
				   strcpy(pcString, buf);
				   return EM_SUCCESS;
			   }

		   case EM_key_F1:
			   return EM_key_F1;

		   case EM_key_F2:
			   return EM_key_F2;

		   default:
			   break;
		}

	}
}

