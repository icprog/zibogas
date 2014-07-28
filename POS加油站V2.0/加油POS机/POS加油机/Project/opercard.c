/******************************************************************************
Copyright 2006 	大连易达通电子有限公司
All rights riserved.
文 件 名	：cardprocess.c
模块名称：EH2108 刷卡子程序
功能概要：主程序 

******************************************************************************/
#include "main.h" 
/******************************************************************************
															全局变量部分
******************************************************************************/
//处理CPU卡使用的变量
extern  INT8U const    SELECT_USER_ADF2[7];
extern INT8U const    VERIFY_PIN[11];//验证口令
extern INT8U const    READ_15_FILE[5];//读取15文件内容
extern  INT8U const    READ_16_FILE[5];//读取16文件内容

////向PSAM卡发命令用到的一些code变量
extern INT8U   SELECT_SAM_ADF1[7];		//选1001目录
extern INT8U   CAL_TAC_M1[5];							//M1卡交易过程中，计算TAC码
extern INT8U const    TAC_INIT[5];
extern INT8U const    TAC_JDINIT[5];

extern DEV_STAT       DevStat;					//设备状态
extern CARD_INFO      CardInfo;					//卡片信息，交易记录由此变量生成
extern ERROR_CARD_INFO ErrCardInfo;				//出错卡状态
extern int socketID;
extern CLERK_STRUCT clerk_list[MAX_CLERK_NUM];

extern DevHandle hSIMHandle;     //SIM卡
extern DevHandle hCpuHandle;      //CPU卡
extern DevHandle hSam1Handle;      //SAM1卡
extern DevHandle hMifsHandle;      //SAM1卡
//extern DevHandle hApptimerHandle;

extern const char parmFileName[];
extern const char blackFileName[];
extern const char currecFileName[];
extern const char conrecFileName[];
extern const char hisrecFileName[];
extern const char grayrecFileName[];

//#define Debug_M1card   //by frank

INT8U  KeySector[14][6];								//M1卡个区密钥，每个区密钥为6字节

// *****************************************************************
// 功能：		printf_debug
// 入口参数：	buf    显示输出字符串
//				
// 出口参数：	Null
// 作者： 刘及华
// 函数出处：
// *****************************************************************
void printf_debug(char * buf)
{
	EA_vCls();
	EA_vDisp(3, 1, (void *)buf);
	Beep(1000);
	(void)EA_uiInkey(2);

}

// *****************************************************************
// 功能：		蜂鸣
// 入口参数：	i=0:蜂鸣"嘀"一声
//				i=1:蜂鸣"嘀嘀嘀"三声
// 出口参数：	Null
// 作者： 刘及华
// 函数出处：EH0218---COMMON.C例子程序
// *****************************************************************
void Beep(int i)
{
	(void)EA_vBeepMs(i);
}
void Beeperr(void)
{
	EA_vBeepMs(300);
	SleepMs(500);
	EA_vBeepMs(300);
}
/*****************************************************************
函数原型：CalConsumeTAC
 功能描述：计算TAC码，M1卡交易流程专用
 参数描述：
 参数名称：	输入/输出？	类型		描述
				
 返  回  值：无
 作      者	：许岩
 日      期：2004-10-08
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
*****************************************************************/
void CalConsumeTAC(void)
{
	INT8U LOCAL_V_TYPE  i = 0;
	INT16U LOCAL_V_TYPE k = 0;
	INT8U  temp_snd_buf[50];
	INT8U  temp_rcv_buf[50];

	unsigned int LOCAL_V_TYPE  rcv_len = 0;

	for ( i=0; i<8; i++ )			  //如果计算不成功，则最多循环计算8次
	{
		////////////////////////////////////////
		//TAC码计算初始化
		////////////////////////////////////////
		memcpy(temp_snd_buf, TAC_INIT, 5);		//TAC计算初始化
		memcpy(&temp_snd_buf[5], &CardInfo._CARD_NUMBER.card_number[0], 8);	// 2字节城市代码 + 2字节行业代码 + 4字节发行流水号
		memcpy(&temp_snd_buf[13], &CardInfo._CARD_NUMBER.detail_card_number.city_code[0], 2);			//行业代码
		memcpy(&temp_snd_buf[15], "\xFF\x00\x00\x00\x00\x00", 6);			//尾
		k = CmdPsam(hSam1Handle, temp_snd_buf, 21, temp_rcv_buf, &rcv_len);		//TAC计算初始化
		if ( k != ok )
			continue;

		////////////////////////////////////////
		//TAC码计算//test:00 00 00 50 06 11 80 00 00 00 05 00 00 00 00 20 05 02 24 11 06 00 80 00
		////////////////////////////////////////

		memcpy(temp_snd_buf, CAL_TAC_M1, 5);//头

		temp_snd_buf[5] = (INT8U)(CardInfo.balance / 0x10000);//3字节交易后余额
		temp_snd_buf[6] = (INT8U)(CardInfo.balance % 0x10000 / 0x100);//3字节交易后余额
		temp_snd_buf[7] = (INT8U)(CardInfo.balance % 0x100);//3字节交易后余额		

		temp_snd_buf[8] = (INT8U)(CardInfo.fare / 0x10000);	//3字节交易金额
		temp_snd_buf[9] = (INT8U)(CardInfo.fare % 0x10000 / 0x100);	//3字节交易金额		
		temp_snd_buf[10]= (INT8U)(CardInfo.fare % 0x100);	//3字节交易金额

				
		memcpy(temp_snd_buf+11, &DevStat.pos_number[0], 6);	//6字节终端机编号
		//memcpy(temp_snd_buf+11, "\x00\x00\x00\x00\x00\x00", 6);	//6字节终端机编号
		memcpy(temp_snd_buf+17, &CardInfo.card_in_time, 7);	//7字节交易日期和时间
		memcpy(temp_snd_buf+24, &CardInfo._CARD_NUMBER.card_number[4], 4);//4字节卡片流水号
		memcpy(temp_snd_buf+28, "\x80", 1);     

		k = CmdPsam(hSam1Handle, temp_snd_buf, 29, temp_rcv_buf, &rcv_len);		//TAC码计算

		if ( k != ok )
			continue;								//如果不成功则循环计算8次
		else
			break;									//直至计算成功才退出
	}

	if ( i >= 8 )
	{	//TAC码计算失败，全写0
		memset(&CardInfo.tac[0], 0x00, 4);
	}
	else
	{	//TAC码计算成功
		memcpy(&CardInfo.tac[0], &temp_rcv_buf[0], 4);		//保存TAC码

	}
}
/*****************************************************************
函数原型：  query_rec_num(void)
 功能描述：查询记录条数
 参数描述：
 参数名称：	输入/输出？	类型		描述
				
 返  回  值：无
 作      者	：刘及华
 日      期：2004-10-08
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
*****************************************************************/
uint query_rec_num(void)
{
	uchar i;
	uchar ucOpenID;
	uint uiReadCnt;

	i = EA_ucPFOpen((uchar *)currecFileName, &ucOpenID);

	if ( i == EM_ffs_NOTEXIST )
	{
		 EA_ucPFCreateLoop((uchar *)currecFileName, sizeof(RECORD), 2000);  //2000条循环记录;
		 EA_ucPFOpen((uchar *)currecFileName, &ucOpenID);
	}
	else if( i != EM_SUCCESS )
	{
		lcddisperr("打开记录文件失败!");
		return notok;
	}

	(void)EA_ucPFInfo(ucOpenID, EM_ffs_VALIDREC, &uiReadCnt);

	(void)EA_ucPFClose(ucOpenID);

	return uiReadCnt;
}


/******************************************************************************
 函数名称：SaveCardInTime
 功能描述：入卡时保存时间
 参数描述：
 参数名称：	输入/输出？	类型		描述
 card_info	输入		CARD_INFO *		准备要保存时间的变量的指针
 
 返  回  值：ok(0)-成功
				 notok(0xFF)-失败
				   
 作      者	：许岩
 日      期：2004-09-02
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
******************************************************************************/
INT8U SaveCardInTime(CARD_INFO *card_info)
{
	INT8U LOCAL_V_TYPE i = 0;
	BUS_TIME LOCAL_V_TYPE time;

	i = Get_Time(&time);
	if ( i != ok )
		return notok;

	i = CheckTimeFormat(&time);								//检测时间格式
	if ( i != ok )
		return notok;

	memcpy(&card_info->card_in_time, &time, sizeof(BUS_TIME));
	return ok;
}

/******************************************************************************
 函数名称：CheckPeriodOfValidity
 功能描述：有效期判断
 参数描述：
 参数名称：	输入/输出？	类型		描述
 start			输入		INT32U		启用日期
 current		输入		INT32U		当前日期
 end			输入		INT32U		有效截止日期
 
 返  回  值：ok(0)-成功
				 notok(0xFF)-失败(过期)
				   
 作      者	：许岩
 日      期：2004-09-20
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
******************************************************************************/
INT8U CheckPeriodOfValidity(INT32U start, INT32U current, INT32U end)
{
	if ( (current >= start) && (current <= end) )
		return ok;
	else
		return notok;

}

/*****************************************************************
函数原型：Sec6_Block_Is_Legal
 功能描述：判断一个区(16个字节)的数据是否正确，判断最后4字节是否为18E718E7
 参数描述：
 参数名称：	输入/输出？	类型		描述
 data_to_verify	输入	const INT8U *		要验证的数组的起始指针
				
 返  回  值：ok(0)-验证成功		
				notok(0xff)-验证失败
 作      者	：许岩
 日      期：2003-10-14
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
*****************************************************************/
INT8U Sec0_Block_Is_Legal(const INT8U * data_to_verify)
{
	//ToggleWD();

	//判断最后4字节是否为18E718E7,09F609F6
	if (    (*(data_to_verify + 12) == 0x09) 
		 && (*(data_to_verify + 13) == 0xF6) 
		 && (*(data_to_verify + 14) == 0x09) 
		 && (*(data_to_verify + 15) == 0xF6) )
	{
		return ok;
	}
	else
	{
		lcddisperr("0块字节校验失败");
		return notok;
	}
}

/*****************************************************************
函数原型：Purse_Block_Is_Right
 功能描述：判断钱包区(16个字节)的数据是否正确
 参数描述：
 参数名称：	输入/输出？	类型		描述
 data_to_verify	输入	const INT8U *		要验证的数组的起始指针
				
 返  回  值：ok(0)-验证成功		notok(0xff)-验证失败
 作      者	：黄立国、许岩
 日      期：2003-10-14
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
*****************************************************************/
INT8U Purse_Block_Is_Right(INT8U * data_to_verify)
{
//	ToggleWD();

	// 1. 判断后4个字节是否255
	// 2. 判断0-3是否等于8-11
	// 3. 判断0-3是否是4-7反码
	if ( (*(data_to_verify + 12) + *(data_to_verify + 13) == 255)
		 && (*(data_to_verify + 12) == *(data_to_verify + 14))
		 && (*(data_to_verify + 13) == *(data_to_verify + 15)) )
	{

		if ( (*(data_to_verify + 0) == *(data_to_verify + 8))
			 && (*(data_to_verify + 1) == *(data_to_verify + 9))
			 && (*(data_to_verify + 2) == *(data_to_verify + 10))
			 && (*(data_to_verify + 3) == *(data_to_verify + 11)) )
		{

			if ( (*(data_to_verify + 0) == (~(*(data_to_verify + 4))    &0x000000ff))
				 && (*(data_to_verify + 1) == (~(*(data_to_verify + 5)) &0x000000ff))
				 && (*(data_to_verify + 2) == (~(*(data_to_verify + 6)) &0x000000ff))
				 && (*(data_to_verify + 3) == (~(*(data_to_verify + 7)) &0x000000ff)) )
			{
				return ok;
			}
			else
			{
				// step 3 fail
				return notok;
			}
		}
		else
		{
			// step 2 fail
			return notok;
		}
	}
	else
	{
		// step 1 fail
		return notok;
	}
}

/*****************************************************************
函数原型：CalTAC
 功能描述：计算TAC码，M1卡交易流程专用
 参数描述：
 参数名称：	输入/输出？	类型		描述
				
 返  回  值：无
 作      者	：许岩
 日      期：2004-10-08
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
*****************************************************************/
void CalTAC(void)
{
	INT8U  i = 0;

	INT8U tac1[4];
	INT8U tac2[4];

	memset(tac1, 0x00, 4);
	memset(tac2, 0x11, 4);
	
	for(i=0; i<10; i++)
	{
		CalTAC2(tac1);
		CalTAC2(tac2);

		if (memcmp(tac1, tac2, 4) == 0)		//算2遍，一样
		{
			memcpy(&CardInfo.tac[0], tac1, 4);
			return;
		}
	}

	//算了10遍，都不一样
	memcpy(&CardInfo.tac[0], tac1, 4);
}

/*****************************************************************
 函数原型：	SingleFlashBoot
 功能描述：单FLASH启动，用于一块FLASH失效，采集另一块FLASH的数据
 参数描述：	参数名称		输入/输出？		类型			描述
 pri_slv		输入		INT8U const		由主FLASH还是从FLASH启动，＝PRI 或 ＝SLV
 
 返回值：	无
 
 作  者：	大连现代/许岩
 日  期：	2005-01-31
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
*****************************************************************/
uchar  Sector6_Process(void)
{

	INT8U LOCAL_V_TYPE  i = 0;
	INT8U LOCAL_V_TYPE  j = 0;
	INT8U LOCAL_V_TYPE  m = 0;
	INT8S LOCAL_V_TYPE g = 0;
	INT8U  block_buffer1[16] = {0};
	INT8U  block_buffer2[16] = {0};
	////////////////////////////////////////
	//验证Sector 6 密钥
	////////////////////////////////////////
	i = Get_Sector_PermissionA(6, &KeySector[6][0]);
	if ( i != ok )
		return notok;

	i = EA_ucMIFRead(hMifsHandle, 24, block_buffer1);
	if ( i != ok )
		return notok;

	i = EA_ucMIFRead(hMifsHandle, 25, block_buffer2);
	if ( i != ok )
		return notok;

	i = Sec6_Block_Is_Legal(block_buffer1);
	j = Sec6_Block_Is_Legal(block_buffer2);
	if ( (i == ok) && (j == ok) )
	{
		//两区都合法，看两区数据是否一致
		g = memcmp(block_buffer1, block_buffer2, 16);
		if ( g == ok )
		{
			//两区一致，什么也不做
			;
		}
		else
		{
			//两区不一致，以24区为主
			m = EA_ucMIFWrite(hSam1Handle,25,block_buffer1);
			if ( m != ok )
				return notok;
		}

	}
	else if ( i == ok )
	{
		//24区合法，24->25
		m = EA_ucMIFWrite(hSam1Handle,25,block_buffer1);
		if ( m != ok )
			return notok;
	}
	else if ( j == ok )
	{
		//25区合法，25->24
		m = EA_ucMIFWrite(hSam1Handle,24,block_buffer2);
		if ( m != ok )
			return notok;
		memcpy(block_buffer1, block_buffer2, 16);//好的数据传给block_buffer1，下边处理数据用block_buffer1
	}
	else
	{
		//两区都不合法，退出
		return notok;
	}

	////////////////////////////////////////
	//判断此卡是否已经置黑名单标志
	////////////////////////////////////////
	if ( block_buffer1[7] == 0x04 )
	{
		printf_debug((void *)"非法卡");	
		return notok;
	}
	return ok;
}
/*****************************************************************
函数原型：DisplayPurchaseResult
功能描述：显示交易结果(扣款成功后)
参数描述：
参数名称：	输入/输出？	类型		描述
cardinfo	输入			const CARD_INFO*	交易完毕后的卡片的所有信息

返  回  值：无
				
作      者	：许岩
日      期：2004-09-23
修改历史：
日期		修改人		修改描述
2005.2.21		myron	                   in function 
*****************************************************************/
int DisplayPurchaseResult(const CARD_INFO *cardinfo)
{	
	float   temp_fare;
	char    strbuf[50];
	float   fBuf = 0.0;

	EA_vCls();
	EA_ucClrKeyBuf();

	EA_vDisplay(1, "车  号 :鲁C-%s", cardinfo->bus_number);

	fBuf = ((float)(cardinfo->capacity)) / 100.0;
	sprintf(strbuf, "升  数: %5.2f", fBuf);
	EA_vDisp(3, 1, strbuf);
	//EA_vDisp(4, 1, "加油员: %02d", DevStat.cur_driver_number[0]);
	temp_fare = cardinfo->fare / 100.0;

	if( DevStat.fill_mode == 0 )
	{
		sprintf(strbuf, "油  品: 天然气");	
	}
	else if( DevStat.fill_mode == 1 )
	{
		sprintf(strbuf, "油  品: 汽油93#");
	}	
	else if( DevStat.fill_mode == 2 )
	{
		sprintf(strbuf, "油  品: 汽油97#");
	}
	else if( DevStat.fill_mode == 3 )
	{
		sprintf(strbuf, "油  品: 柴油");
	}	
	else
	{
		return notok;
	}
	EA_vDisp(2, 1, strbuf);
	EA_vDisp(4, 1, "金  额: %4.2f元", temp_fare);

	Beep(1000);

	if( EA_uiInkey(0) == EM_key_ENTER )
	{
		return ok;
	}
	return notok;
}

// *****************************************************************
// 功能：在屏幕上输出字符
// 入口参数：	lb->显示输出字符
// 出口参数：	Null
// 作者：
// 函数出处：EH0218---COMMON.C例子程序
// *****************************************************************
char HextoBcd(char sour)
{
	return(((sour/10)<<4) |(sour%10));
}

/******************************************************************************
 函数名称：CardFlush
 功能描述：加油卡交易流程
 参数描述：
 参数名称：输入/输出？	类型		描述
 
 返  回  值：    ok(0)-成功
				 notok(0xFF)-失败
				 0xFE-没找到卡
				 0xF0-重新刷卡
				   
 作      者	：刘及华
 日      期：2014-01-30
 修改历史：
		日期		修改人		修改描述
******************************************************************************/
INT8U CardFlush(void)
{
	INT8U  ret = 0;
	uchar  uckey = NO_KEY;
	uint   uikey = NO_KEY;

	ret = ICCardProcess();   //先进行检测加油卡

	if( ret == 0)
	{
RESELECT:
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
					return ok;
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
	}

	//EA_ucMIFHalt(hMifsHandle);	//挂卡

	return ret;//选择退出
}

/******************************************************************************
 函数名称：GPRS_OPER_CARD_CHECK
 功能描述：GPRS监测操作员卡是否为黑名单
 参数名称：输入/输出？	类型		描述
输入		    opercardno: 操作员卡号
				
 返  回  值：无
 
 作      者	：YFY
 日      期：2007-05-2
 修改历史：
		日期		修改人		修改描述
******************************************************************************/
INT8U oper_login_check(void)
{	
//  char input[22];
//  uint i = 0;
//  uchar k = 0;
//  EA_vCls();
//  EA_vDisplay(1, "     正在登陆...    ");
//  EA_vDisplay(2, "      请等待...     ");
//  SleepMs(2000);

//  if( open_gprs_module() != ok )
//  {
//  	lcddisperr("PPP拨号连接失败");
//  	return notok;
//  }
//  if( gprs_test() != ok )          //调试2012.12.18
//  {
//  	lcddisperr("同步服务器时间失败");
//  	return notok;
//  }
	//设置心跳频率100S
//  ret = EA_uiCreateAppTimer("AppTime", (void *)AppTimerISR, 0, 10000, 10000, EM_rtos_ENABLE_TIMER, &hApptimerHandle); //心跳时间为一分钟
//  if (ret != EM_SUCCESS)
//  {
//  	lcddisperr("定时器设定失败");
//  	return notok;
//  }
/*	EA_vCls();
	EA_ucClrKeyBuf();

	EA_vDisplay(1, "请输入操作员ID:     ");
	memset((void *)input, 0, sizeof(input));

	for ( ;; )
	{
		i = EA_ucGetInputStr(1, 16, 20, EM_BIG_FONT | EM_MODE_NUMBER | EM_ALIGN_LEFT | EM_SHOW_ORIGINALLY
							 , 2, 2, 0, (void *)input);
		if ( i == EM_SUCCESS )
			break;
		if ( i == EM_ABOLISH )
			return notok;
	}

	for(k=0; k<MAX_CLERK_NUM; k++)
	{
		if ( memcmp(&input[0], &clerk_list[k].id[0], 2) == 0 )
		{
//  		memcpy(DevStat.cur_driver_number, clerk_list[k].id, 2);
			DevStat.cur_driver_number[0] = (ascii_to_hex(input[0]) << 4) | ascii_to_hex(input[1]);
			break;
		}
	}

	if( k >= MAX_CLERK_NUM )
	{
		lcddisp(4, 1, (void *)"     用户不存在     ");
		(void)EA_uiInkey(2);
		return notok;
	}


	EA_vDisplay(2, "请输入密码:         ");

	memset((void *)input, 0, sizeof(input));

	for ( ;; )
	{
		i = EA_ucGetInputStr(2, 12, 20, EM_BIG_FONT | EM_MODE_NUMBER | EM_ALIGN_LEFT | EM_SHOW_PASSWORD
							 , 4, 4, 0, (void *)input);
		if ( i == EM_SUCCESS )
			break;
		if ( i == EM_ABOLISH )
			return notok;
	}

	if ( memcmp((void *)input, &clerk_list[k].pw[0], 4) == 0 )
	{
		;
	}
	else
	{
		lcddisp(4, 1, (void *)"     密码不正确     ");
		(void)EA_uiInkey(2);
		return notok;
	}

//  if( gprs_test() != ok )          //调试2012.12.18
//  {
//  	lcddisperr("同步服务器时间失败");
//  	return notok;
//  }
//
//  if( download_whiltelist() != ok )
//  {
//  	lcddisperr("白名单下载失败");
//  	return notok;
//  }

    return ok;
*/
	uchar  uckey=0;
	uint   uikey=0;
	INT8U  block_buffer1[16] = {0};
	INT8U  tmp_buffer[16] = {0};
	INT8U  input[22];
	int    i = 0;

	EA_vCls();


	while(1)
	{
		EA_vDisplay(2, (void *)"    请刷加油员卡    ");
		if( CardInit() == ok )
			break;

		uckey = EA_ucKBHit();
		if ( uckey ==  EM_key_HIT )		 //按下取消键
		{
			uikey = EA_uiInkey(0);
			if ( uikey == EM_key_CANCEL )		 //按下取消键
			{
				return ok;
			}
		}
	}

	////////////////////////////////////////
	//读Sector 1 中4 区信息
	////////////////////////////////////////
	memcpy(&KeySector[1][0], &CardInfo.csn[0], 4);
	KeySector[1][4] = ~KeySector[1][0];
	KeySector[1][5] = ~KeySector[1][1];

	i = Get_Sector_PermissionA(1, &KeySector[1][0]);
	if ( i != ok )
	{
		lcddisperr((char *)"请重新刷卡");
		return notok;
	}

	i = EA_ucMIFRead(hMifsHandle, 4, block_buffer1);
	if ( i != ok )
	{
		lcddisperr((char *)"请重新刷卡");
		return notok;
	}

	if ( block_buffer1[10] != 0x01 )	  //判断启用标志
	{
		//卡未启用处理
		lcddisperr((char *)"卡片未启用");
		Beeperr();
		return notok;
	}

	memcpy(&DevStat.oper_cardno[0], &block_buffer1[1], 3);							 //加油员卡号																				 //
	memcpy(&CardInfo.card_random_num[0], &block_buffer1[4], 4);						 //加油员密码
	CardInfo.card_type = block_buffer1[11];											     //卡类

	if (CardInfo.card_type != 0xBB)                 //只允许普通卡消费debug  by frank
	{
		lcddisperr((void *)"不是加油员卡!");
		return notok;
	}

	EA_vCls();
	EA_vDisplay(1, "加 油 员:%02X%02X%02X", DevStat.oper_cardno[0], DevStat.oper_cardno[1], DevStat.oper_cardno[2]);
	EA_vDisplay(2, "输入密码:            ");

	memset((void *)input, 0, sizeof(input));

	for ( ;; )
	{
		i = EA_ucGetInputStr(2, 10, 20, EM_BIG_FONT | EM_MODE_NUMBER | EM_ALIGN_LEFT | EM_SHOW_PASSWORD
							 , 8, 8, 0, (void *)input);
		if ( i == EM_SUCCESS )
			break;
		if ( i == EM_ABOLISH )
			return notok;
	}

	var_asc2bcd(&tmp_buffer[0], &input[0], 8);

	if ( memcmp((void *)tmp_buffer, &CardInfo.card_random_num[0], 4) == 0 )
	{
		;
	}
	else
	{
		lcddisp(4, 1, (void *)"     密码不正确     ");
		(void)EA_uiInkey(2);
		return notok;
	}
	WriteParam();
	return ok;
}

/******************************************************************************
 函数名称：download_whiltelist(void)
 功能描述：下载最新的白名单
 参数名称：无
 输入		   无
				
 返  回  值：无
 
 作      者	：刘及华
 日      期：2013-06-2
 修改历史：
		日期		修改人		修改描述
******************************************************************************/
INT8U download_price_table(void)
{
	INT8U   sendbuf[1024];
	uchar   ret = 0;

	lcddisperr("更新油价表");
	EA_vCls();     
	EA_vDisplay(1, "天然气 :%2.2f元/立方", DevStat.price[0]/100.0);
	EA_vDisplay(2, "汽油93#:%2.2f元/升 ", DevStat.price[1]/100.0);
	EA_vDisplay(3, "汽油97#:%2.2f元/升 ", DevStat.price[2]/100.0);
	EA_vDisplay(4, "柴  油 :%2.2f元/升 ", DevStat.price[3]/100.0);
    SleepMs(2000);

	if( open_gprs_module() != ok )
	{
		lcddisperr("PPP拨号连接失败");
		return notok;
	}

	memset(sendbuf, 0x00, sizeof(sendbuf));

	sprintf((void *)sendbuf, "@START%02X%02X%02XUPYJ000",
		 DevStat.equ_id[0], DevStat.equ_id[1], DevStat.equ_id[2]);
	
	ret = GPRS_Send_Receive(sendbuf, 19);
	if( ret != ok )
	{
		EA_vCls();
		EA_vDisplay(1, "    GPRS数据传输    ");
		EA_vDisplay(2, "  网络故障通讯失败  ");	
		EA_vDisplay(3, "  网络正常后再尝试  ");
		SleepMs(2000);
//  	GPRS_Close();
		return notok;
	}

	WriteParam();
	lcddisperr("油价更新成功");
//  GPRS_Close();
	return ok;

}
/******************************************************************************
 函数名称：download_whiltelist(void)
 功能描述：下载最新的白名单
 参数名称：无
 输入		   无
				
 返  回  值：无
 
 作      者	：刘及华
 日      期：2013-06-2
 修改历史：
		日期		修改人		修改描述
******************************************************************************/
uchar download_whiltelist(void)
{
	INT8U   sendbuf[1024], str[50];
	uchar   ret=0;
	uint    package_num=0, package_index=0, package_left=0;
	INT8U	ch=0;

	memset(str, 0, sizeof(str));
//查询白名单版本号
	EA_vCls();
	EA_vDisplay(1, "     同步白名单     ");
	EA_vDisplay(3, "    现有数量:%04d   ", DevStat.black_name_number);
	SleepMs(2000);

	if( open_gprs_module() != ok )
	{
		lcddisperr("PPP拨号连接失败");
		return notok;
	}

	DevStat.black_flag = 0;
	
	memset(sendbuf, 0x00, sizeof(sendbuf));

	sprintf((void *)sendbuf, "@START%02X%02X%02XWHNM000",
		 DevStat.equ_id[0], DevStat.equ_id[1], DevStat.equ_id[2]);
	
	ret = GPRS_Send_Receive(sendbuf, 19);
	if( ret != ok )
	{
		EA_vCls();
		EA_vDisplay(1, "    GPRS数据传输    ");
		EA_vDisplay(2, "  网络故障通讯失败  ");	
		EA_vDisplay(3, "  网络正常后再尝试  ");
		SleepMs(2000);
		return notok;
	}

//需要更新白名单
	if( DevStat.black_flag == 1 )
	{
		EA_ucBlkClean ((char *)blackFileName);
		DevStat.current_record_index = 0;
		sprintf((void *)str, "  下载%04d条白名单  ", DevStat.black_name_number);
		EA_vDisplay(2, (void *)str);	
		SleepMs(2000);

		package_num = DevStat.black_name_number  / 190;
		package_left = DevStat.black_name_number % 190;

		//按照整包下载，前提条数满足190条及以上
		for( package_index=0; package_index<package_num; package_index++ )
		{
			memset(sendbuf, 0, sizeof(sendbuf));
			ch = (int)((package_index*100)/(package_num+1));
			sprintf((void *)str, "下载进度%2d%%", ch);
			EA_vDisplay(3, (void *)str);
			//SleepMs(500);
			sprintf((void *)sendbuf, "@START%02X%02X%02XWHUP006%02d%04d", 
					DevStat.equ_id[0], DevStat.equ_id[1], DevStat.equ_id[2],
					package_index, 190 * 5);

			ret = GPRS_Send_Receive((void *)sendbuf, 25);
			if( ret != ok )
			{
				EA_vCls();
				EA_vDisplay(4, "白名单下载失败!");
				SleepMs(2000);
				//memset(DevStat.black_version, 0, 7);
				DevStat.black_name_number = 0;
				//EA_ucBlkClean ((char *)blackFileName);
				DevStat.current_record_index = 0;
				return notok;
			}
		}

		if( ret == ok )
		{
			if( package_left > 0 )
			{
				memset(sendbuf,0x00,sizeof(sendbuf));
				sprintf((void *)sendbuf, "@START%02X%02X%02XWHUP006%02d%04d", 
						DevStat.equ_id[0], DevStat.equ_id[1], DevStat.equ_id[2],
					package_num, package_left*5);
				ret = GPRS_Send_Receive(sendbuf, 25);
				if( ret == ok )
				{
					EA_vDisplay(3, "      下载完成      ");	
					SleepMs(2000);		
				}
				else
				{
					EA_vDisplay(4, (void *)"   白名单下载失败   ");
					//memset(DevStat.black_version,0,7);
					DevStat.black_name_number = 0;
					//EA_ucBlkClean((char *)blackFileName);
					DevStat.current_record_index = 0;
					GPRS_Close();
					return notok;
				}
			}
			else
			{
				EA_vDisplay(3, "      下载完成      ");
				SleepMs(2000);
			}
			DevStat.driver_is_working = TRUE;
			WriteParam();
		}
		if( ret != ok )
		{
			EA_vCls();
			EA_vDisplay(1, "    GPRS数据传输    ");
			EA_vDisplay(2, "  网络故障通讯失败  ");	
			EA_vDisplay(3, "  网络正常后再使用  ");
			SleepMs(2000);
			GPRS_Close();
			return notok;
		}
	}
	else if(DevStat.black_flag == 0)
	{
		lcddisperr("白名单已最新不用更新");
		return ok;
	}

	GPRS_Close();
	return ok;
/*
		//最后一包
		if( package_left == 0 )
		{
			black_length = (190*package_num);

		}
		else if( package_left > 0 )  		
		{
			memset(sendbuf, 0x00, sizeof(sendbuf));
			sprintf((void *)sendbuf, "@START%02X%02X%02XWHUP006%02d%04d", DevStat.equ_id[0], 
					DevStat.equ_id[1], DevStat.equ_id[2], package_num, package_left * 5);

			memset(END, 0, sizeof(END));
			crc = Cal_CRC((unsigned char*)sendbuf, 25);	
			sprintf((char*)END, "%02X%02XEND\r", ((crc >> 8)& 0xff), (crc & 0xff));
			memcpy(sendbuf + 25, END, 8);
			sendbuf[33] = '\0';

			if( write(socketID, sendbuf, 33) == -1)
			{
				lcddisperr("TCP/IP发送失败");
				goto Err_whilte_list;
			}	

			memset(recbuf, 0x00, sizeof(recbuf));
			retlen = read(socketID, recbuf, sizeof(recbuf));
			if( retlen < 0)
			{
				lcddisperr("TCP/IP接收失败3");
				goto Err_whilte_list;
			}	
			recbuf[retlen] = '\0';

			if( memcmp(recbuf+12, "WHUP", 4) == 0 )      //更新白名单
			{
//  			memcpy(pblk+950*package_num, recbuf+19, package_left * 5);
				Debugprintf((void *)recbuf);
		    }
			black_length = (190*package_num+package_left);
		}

		memset(sendbuf, 0x00, sizeof(sendbuf));
		sprintf((void *)sendbuf, "@START%02X%02X%02XUPDE000", DevStat.equ_id[0], 
				DevStat.equ_id[1], DevStat.equ_id[2]);

		memset(END, 0, sizeof(END));
		crc = Cal_CRC((unsigned char*)sendbuf, 19);	
		sprintf((char*)END, "%02X%02XEND\r", ((crc >> 8)& 0xff), (crc & 0xff));
		memcpy(sendbuf + 19, END, 8);
		sendbuf[27] = '\0';

		if( write(socketID, sendbuf, 27) == -1)
		{
			lcddisperr("TCP/IP发送失败1");
			goto Err_whilte_list;
		}	

		memset(recbuf, 0x00, sizeof(recbuf));
		retlen = read(socketID, recbuf, sizeof(recbuf));
		if( retlen < 0)
		{
			lcddisperr("TCP/IP接收失败2");
			goto Err_whilte_list;
		}	
		recbuf[retlen] = '\0';
		if( (memcmp(recbuf+12, "UPDE", 4) == 0) && (memcmp(recbuf+19, "OK", 2) == 0) )
		{
			lcddisperr("      下载完成!     ");
			DevStat.driver_is_working = TRUE;
			
			SleepMs(1000);

			//清空白名单列表
			EA_ucBlkClean ((char *)blackFileName);
			//写入数据库
			EA_ucBlkOpen((void *)&blackFileName, &uiOpenID);

//  		Debugprintf((void *)pblk);

			ret = EA_ucBlkAppendBase(uiOpenID, pblk, &black_length);
//  		ret = EA_ucBlkUpdate(uiOpenID, NULL, 0, pblk, &black_length);
			if ( ret != EM_bfs_SUCCESS )
			{
				EA_vDisplay(1, "存储白名单失败");
				EA_vDisplay(2, "实际下载条数：%ld", black_length);
				EA_vDisplay(3, "错误代码：0x%02X", ret);
				SleepMs(2000);
				EA_ucBlkClose(uiOpenID);
				DevStat.black_name_number = 0;
	//  		free(pblk);
				return notok;
			}

			WriteParam();
			EA_ucBlkClose(uiOpenID);
			lcddisperr("      下载完成      ");
	//  	free(pblk);
			return ok;
		}

	}
	else if( DevStat.black_flag == 0 )
	{
		lcddisperr("白名单为最新版本");
		return ok;
	}
Err_whilte_list:

	EA_vCls();
	EA_vDisplay(1, "    GPRS数据传输    ");
	EA_vDisplay(2, "  网络故障通讯失败  ");	
	EA_vDisplay(3, "  网络正常后再使用  ");
	SleepMs(2000);
	DevStat.black_name_number = 0;
//  free(pblk);
    return notok;
*/
}

/*****************************************************************
函数原型：Login
功能描述：本函数为操作员登陆窗口
参数描述：无
参数名称：	输入/输出？	类型		描述
-----------		-----------	------	   	-------
返 回 值：无
*****************************************************************/
void  Login(void)
{

	INT8U   ret;
	uchar   uckey;
	uint    uikey;
	INT8U   record[60];
	BUS_TIME    ltime;

	EA_vCls();

	memset(record, 0x00, sizeof(record));

	if( DevStat.driver_is_working == FALSE )  //未上班
	{
		if( oper_login_check() != ok )  	  //debug by frank
		{
			DevStat.driver_is_working = FALSE;
			lcddisperr("登录失败");
			return;
		}
		else
		{
			Get_Time(&ltime); 
			memcpy(&DevStat.binhand.work_datetime, &ltime, sizeof(BUS_TIME));

			memcpy(DevStat.binhand.acnt_id, DevStat.acnt_id, 2);
			memcpy(DevStat.binhand.equ_id, DevStat.equ_id, 3);
			memcpy(&DevStat.binhand.oper_id[0], DevStat.oper_cardno, 3);

			memset(DevStat.binhand.consume_cnt, 0, 16);
			memset(DevStat.binhand.consume_cap, 0, 16);
			memset(DevStat.binhand.consume_amt, 0, 16);

			DevStat.driver_is_working = TRUE;
			WriteParam();		
			lcddisperr("登录成功");
		}
	}

#ifdef _debug_m1card_
	EA_vDisplay(1, "%02X%02X%02X%02X%02X%02X%02X", ltime.century, DevStat.binhand.work_datetime.year, 
				DevStat.binhand.work_datetime.month, DevStat.binhand.work_datetime.day, DevStat.binhand.work_datetime.hour,
				DevStat.binhand.work_datetime.minute, DevStat.binhand.work_datetime.second);
	SleepMs(3000);
#endif

	if( DevStat.driver_is_working == TRUE )   //已经上班
	{		
		for ( ;; )
		{
			TextHeadDisplayTimer();

			EA_vDisplay(1, "请扫加油卡...");
			EA_vDisplay(4, "签退            返回");
//			sprintf((void *)strbuf, "本班加油:%3d次", DevStat.binhand.consume_cnt);		
			EA_vDisplay(2, " ");
			EA_vDisplay(3, " ");
			if( LowBatteryDisplay() == notok )
			{
				Power_Down();
				continue;
			}
			
			if( DevStat.record_number >= MAX_REC_NUM )
			{
				lcddisperr((void *)"记录已满必须签退");
				Gprs_Upload_data();	 	//数据上传
				break;
			}
			
			ret = CardFlush();

			if ( ret == 0xF0 )		    //重新刷卡
			{
				for ( ;; )
				{
					EA_vCls();
					EA_vDisplay(3,(void *)"请重新刷卡");
					Beeperr();
					ret = CardFlush();
					if ( ret == ok )
					{
						break;
					}
					else
					{
						uckey = EA_ucKBHit();
						if ( uckey ==  EM_key_HIT )		 //按下取消键
						{
							uikey = EA_uiInkey(0);
							if ( uikey == EM_key_CANCEL )		 //按下取消键
							{
								break;
							}
						}
					}
				}
			}
			else
			{
				uckey = EA_ucKBHit();
				if ( uckey ==  EM_key_HIT )		 
				{		
					uikey = EA_uiInkey(0);
					if(uikey == EM_key_F1 )
					{
						EA_vCls();
						EA_vDisp(2, 1, "   是否做当天日结?  ");
						EA_vDisp(4, 1, "确定            取消");	
						uikey = EA_uiInkey(0);
						switch(uikey)
						{
						   case EM_key_F1:
						   case EM_key_ENTER:
							    if( Logout() == ok )
								{
									EA_vCls();
									EA_vDisplay(1, "   欢迎使用本设备   ");
									EA_vDisplay(2, "      签退成功      ");
									EA_vDisplay(3, "   按下任意键退出   ");
									(void)EA_uiInkey(3);
									GPRS_Close();
									return;
								}
								else
								{
									EA_vCls();
									EA_vDisplay(1, "     签退不成功     ");
									EA_vDisplay(2, "     请重新签退     ");
									EA_vDisplay(3, "   按下任意键退出   ");
									(void)EA_uiInkey(3);
									GPRS_Close();
									break;
								}
						   default :
								break;
						}
					}
					else
						break;
				}
			}
		}               
	}
}
/*****************************************************************
函数原型：TextHeadDisplayTimer(void)
功能描述：本函数为在开头的位置显示时间
参数描述：无
参数名称：	输入/输出？	类型		描述
-----------		-----------	------	   	-------
返 回 值：ok  ?  notok
*****************************************************************/
void TextHeadDisplayTimer(void)
{
	BUS_TIME ltime;
	char buf[22];

	Get_Time(&ltime);
	sprintf((void *)buf, "%02X%02X-%02X-%02X %02X:%02X",
			ltime.century, ltime.year, ltime.month, ltime.day, ltime.hour, ltime.minute);
	EA_vTextOut(0, 0, EM_key_FONT8X16, 0, 1, 1, (char *)buf);
}
/*****************************************************************
函数原型：INT8U Logout(void)
功能描述：本函数为签退功能
参数描述：无
参数名称：	输入/输出？	类型		描述
-----------		-----------	------	   	-------
返 回 值：ok  ?  notok
*****************************************************************/
INT8U Logout(void)
{
	BUS_TIME	time_data;
	INT8U	    record[100];

	if( Gprs_Upload_data() == ok ) 
	{
		Get_Time(&time_data);
		memcpy(&DevStat.binhand.down_datetime, &time_data, sizeof(BUS_TIME));
		//打印日结数据
		print_work_note();
		memcpy(record, (INT8U*)&DevStat.binhand, sizeof(DevStat.binhand));
		SaveNoteRecord(record);
		DevStat.driver_is_working = FALSE;
		memset(&DevStat.binhand, 0x00, sizeof(DevStat.binhand));
		WriteParam();

		return ok;
	}

	return notok;
}




