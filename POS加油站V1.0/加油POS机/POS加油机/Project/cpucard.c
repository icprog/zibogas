/******************************************************************************
Copyright 2006 	大连金德姆电子有限公司
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
//extern GRAYRECORD  GrayRecord;
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

INT8U  main_menu[] = "1.参数设置          2.信息查询          3.数据传输          4.操作员密码修改    ";
INT8U  KeySector[14][6];								//M1卡个区密钥，每个区密钥为6字节

INT8U  app_menu[8*20+2] = { "F1--售卡            F2--充值            F3--冲帐            04--签退            05--查询            06--打印            07--消费            08--系统管理        " };
//INT8U  PersonInfo[5*20+2]= { "1--持卡人姓名       2--持卡人性别       3--持卡人证件       4--有效证件号       5--保存参数         "};
//#define _debug_m1card_
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
 函数名称：CheckCardCredit
 功能描述：检查卡是否符合冲账
 参数描述：
 参数名称：	输入/输出？	类型		描述
 
 返  回  值：ok(0)-成功
				 notok(0xFF)-失败
				 0xF0-重新刷卡
				   
 作      者	：许岩/刘及华
 日      期：2004-09-02
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
******************************************************************************/
INT8U CheckCardCredit(void)
{
	uint	  uiRetCnt;
	INT8U     p[60];
	uchar     ucOpenID = 0;
	uchar	  ucRet;
	uint   	  uiReadCnt;
	uint      count;
	RECORD    *prec = (RECORD *)p;
	uchar     i = 0;

	memset(p, 0x00, sizeof(RECORD));
	EA_vCls();
	i = EA_ucPFOpen( (uchar *)currecFileName, &ucOpenID);
	if ( i != EM_ffs_SUCCESS )
	{
		lcddisperr("打开记录文件失败");
		return notok;
	}
	// 获取有效信息条数
	(void)EA_ucPFInfo(ucOpenID, EM_ffs_VALIDREC, &uiReadCnt);
	if( uiReadCnt == 0)
	{
		lcddisperr("没有记录信息");
		(void)EA_ucPFClose(ucOpenID);
		return notok;//表示没有记录
	}
	count = uiReadCnt;//最后一条		
	ucRet = EA_ucPFReadRec(ucOpenID, count, 1, &uiRetCnt, p);
	if(ucRet != EM_SUCCESS)
	{
		(void)EA_ucPFClose(ucOpenID);
		lcddisperr((void *)"读取不符合");
		return notok;
	}
	(void)EA_ucPFClose(ucOpenID);
	memcpy(prec, p, sizeof(RECORD));
	if(memcmp(&CardInfo._CARD_NUMBER.detail_card_number.serial_number[0], &prec->card_serial_number[4], 4) !=0)
	{
		lcddisperr((void *)"冲帐卡号不符合");
		return notok;
	}

	if(prec->card_purchase_type != TT_CREDIT_MONEY)
	{
		printf_debug((void *)"前笔交易非充值");
		return notok;
	}

	CardInfo.fare = (float)(prec->fare[0] * 65536 + prec->fare[1] * 256 + prec->fare[2]);
	return ok;
}

/******************************************************************************
 函数名称：CardQuery
 功能描述：卡信息查询
 参数描述：
 参数名称：	输入/输出？	类型		描述
 
 返  回  值：ok(0)-成功
				 notok(0xFF)-失败
				   
 作      者	：许岩、刘及华
 日      期：2004-09-02
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
******************************************************************************/
INT8U CardQuery(void)
{
//    INT8U   LOCAL_V_TYPE  temp_buf[50];
//    ulong   ulSnr;
//    uint    LOCAL_V_TYPE  rcv_length;
//    INT8U   LOCAL_V_TYPE  k = 0;
//    uchar   ucResult;
//    uchar   i;
//    uint    uiSize;
//    ushort  usICType;
//    //取得卡种类
//    ucResult = EA_ucMIFRequest (hMifsHandle, EM_mifs_IDLE, &usICType);
//    if (ucResult != EM_SUCCESS)
//    {
////  	lcddisperr("M1卡就绪失败");
//        return notok;
//    }
//    ucResult = EA_ucMIFAntiColl (hMifsHandle, &ulSnr);
//    if (ucResult != EM_SUCCESS)
//    {
////  	lcddisperr("M1卡获取卡号失败");
//        return notok;
//    }
//
//    ucResult = EA_ucMIFSelect (hMifsHandle, ulSnr, &uiSize);
//    if (ucResult != EM_SUCCESS)
//    {
////  	lcddisperr("M1卡获取卡类型失败");
//        return notok;
//    }
//
//    CardInfo.csn[3] = (INT8U)(ulSnr / 0x1000000);
//    CardInfo.csn[2] = (INT8U)((ulSnr % 0x1000000) / 0x10000);
//    CardInfo.csn[1] = (INT8U)((ulSnr % 0x10000) / 0x100);
//    CardInfo.csn[0] = (INT8U)(ulSnr % 0x100);
//    ////////////////////////////////////////
//    //保存时间
//    ////////////////////////////////////////
//    i = SaveCardInTime(&CardInfo);
//    if ( i != ok )
//    {
//        lcddisperr("保存时间失败");
//        return notok;
//    }
//
//    ////////////////////////////////////////
//    //PSAM卡复位
//    ////////////////////////////////////////
//    i = PsamRst();
//    if ( i != ok )
//    {
//        lcddisperr("PSAM卡复位失败");
//        return notok;
//    }
//    ////////////////////////////////////////
//    //PSAM卡选1001目录
//    ////////////////////////////////////////
//    k = CmdPsam(hSam1Handle, (void *)SELECT_SAM_ADF1, sizeof(SELECT_SAM_ADF1),
//                temp_buf, &rcv_length);     //选1001目录
//    if ( k != ok )
//    {
//        lcddisperr("chose menu fail");
//        return notok;
//    }
//    k = M1CardQuery();
//    if( k != ok )
//    {
//        EA_vCls();
//        EA_vDisplay(3, "     请重新刷卡     ");
//        Beep(1000);
//        EA_vDelayMs(500);
//        Beep(1000);
//        return notok;
//    }
	return ok;
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
 函数名称：QuitWorkProcess
 功能描述：下班操作
 参数描述：
 参数名称：	输入/输出？	类型		描述

 返  回  值：无

 作      者	：许岩
 日      期：2004-09-20
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
******************************************************************************/
void QuitWorkProcess(void)
{
	memset(&DevStat.cur_driver_number[0], 0x00, sizeof(DevStat.cur_driver_number));					//司机号清0
	DevStat.driver_is_working = FALSE;
	//保存司机卡参数
	WriteParam();
	CardInfo.card_purchase_type = TT_DRIVER_REST;
//	CardInfo.quit_work = TRUE;//让后面的语音提示"司机卡下班"
}

/******************************************************************************
 函数名称：BeginWorkProcess
 功能描述：上班操作
 参数描述：
 参数名称：	输入/输出？	类型		描述
 _buf	输入	INT8U *	司机编号的内容

 返  回  值：无

 作      者	：许岩
 日      期：2004-09-20
 修改历史：
		日期		修改人		修改描述
		2005.2.21		myron				in function
******************************************************************************/
void BeginWorkProcess(INT8U const * const driver_number_buf)
{

	//modified by myron for dandong flow 2005.2.21
	memcpy(&DevStat.cur_driver_number[0], driver_number_buf+2, 4);//保存司机号
	memcpy(&CardInfo.driver_number[0], driver_number_buf+2, 4);//保存司机号
	DevStat.driver_is_working = TRUE;
	//保存司机卡参数
	WriteParam();

	CardInfo.card_purchase_type = TT_DRIVER_WORKING;
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
//  return ok;
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
INT8U Sec6_Block_Is_Legal(const INT8U * data_to_verify)
{
	//ToggleWD();

	//判断最后4字节是否为18E718E7
	if ( (*(data_to_verify + 12) == 0x18) 
		 && (*(data_to_verify + 13) == 0xE7) 
		 && (*(data_to_verify + 14) == 0x18) 
		 && (*(data_to_verify + 15) == 0xE7) )
	{
		return ok;
	}
	else
	{
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
函数原型：BlockToCash
 功能描述：判断钱包区的余额->INT32U的数据
 参数描述：
 参数名称：	输入/输出？	类型		描述
 buf			输入		const INT8U *		要验证的数组的起始指针
				
 返  回  值：INT32U的钱
 作      者	：许岩
 日      期：2004-09-29
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
*****************************************************************/
INT32U BlockToCash(const INT8U *buf)
{
	return( ((INT32U)buf[3] * 0x1000000 ) + ((INT32U)buf[2] * 0x10000 ) + ((INT32U)buf[1] * 0x100 ) + (INT32U)buf[0] );
}


/*****************************************************************
函数原型：CharToCash
 功能描述：判断钱包区的余额->INT32U的数据
 参数描述：
 参数名称：	输入/输出？	类型		描述
 buf			输入		const INT8U *		要验证的数组的起始指针
				
 返  回  值：INT32U的钱
 作      者	：许岩
 日      期：2004-09-29
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
*****************************************************************/
INT32U CharToCash(const INT8U *buf)
{
//  return( ((INT32U)buf[0]<<16) + ((INT32U)buf[1]<<8) + (INT32U)buf[2] );

	return( ((INT32U)buf[0] * 0x10000 ) + ((INT32U)buf[1] * 0x100 ) + (INT32U)buf[2] );
}

/*****************************************************************
函数原型：BlockToCash
 功能描述：判断钱包区的余额->INT32U的数据
 参数描述：
 参数名称：	输入/输出？	类型		描述
 cash		输入		INT32U		要扣的款
				
 返  回  值：指向扣款block格式的指针
 作      者	：许岩
 日      期：2004-09-29
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
*****************************************************************/
unsigned long  CashToBlock(INT32U cash)
{

	INT8U block[4];
	unsigned long temp;
	block[3] = (INT8U)(cash / 0x1000000);
	block[2] = (INT8U)(cash % 0x1000000 / 0x10000 );
	block[1] = (INT8U)(cash % 0x10000 / 0x100 );
	block[0] = (INT8U)(cash % 0x100) ;
	memcpy(&temp,block,4);
	return temp;
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
	
//    INT8U LOCAL_V_TYPE  i = 0;
//    INT16U LOCAL_V_TYPE k = 0;
//    INT8U  temp_snd_buf[50];
//    INT8U  temp_rcv_buf[50];
//
//    unsigned int   LOCAL_V_TYPE  rcv_len = 0;
//
//    ////////////////////////////////////////
//    //ISAM卡选1001目录
//    ////////////////////////////////////////
////	PsamRst(,, INT8U * rcv_length)
//    k = PsamRst();//PSAM卡复位
//    if ( k != ok )
//    {
////  	printf_debug("PSAM卡故障");
//        return ;
//    }
//
//    k = CmdPsam(hSam1Handle, (void *)SELECT_SAM_ADF1, sizeof(SELECT_SAM_ADF1),
//                    temp_rcv_buf, &rcv_len);    //选1001目录
//    if ( k != ok )
//    {
////  	printf_debug("PSAM卡故障");
//        return ;
//    }
//
//    for ( i=0; i<8; i++ )             //如果计算不成功，则最多循环计算8次
//    {
//        ////////////////////////////////////////
//        //TAC码计算初始化
//        ////////////////////////////////////////
//        memcpy(temp_snd_buf, TAC_INIT, 5);      //TAC计算初始化
//
////		temp_snd_buf[5] = 0;								//8字节用户卡应用序列号
//
//        //modified by myron for dadong flow 2005.2.28
//        memcpy(&temp_snd_buf[5], &CardInfo._CARD_NUMBER.card_number[0], 8); // 2字节城市代码 + 2字节行业代码 + 4字节发行流水号
//        memcpy(&temp_snd_buf[13], &CardInfo._CARD_NUMBER.detail_card_number.city_code[0], 2);           //行业代码
//        memcpy(&temp_snd_buf[15], "\xFF\x00\x00\x00\x00\x00", 6);           //尾
//        k = CmdPsam(hSam1Handle, temp_snd_buf, 21, temp_rcv_buf, &rcv_len);     //TAC计算初始化
////		k = CmdPsam(DevStat.cur_psam_channel, temp_snd_buf, 13, temp_rcv_buf, &rcv_len);		//TAC计算初始化
//
//        if ( k != ok )
//        {
//            lcddisperr("计算TAC码失败");
//            continue;
//        }
//        ////////////////////////////////////////
//        //TAC码计算//test:00 00 00 50 06 11 80 00 00 00 05 00 00 00 00 20 05 02 24 11 06 00 80 00
//        ////////////////////////////////////////
//
//        memcpy(temp_snd_buf, CAL_TAC_M1, 5);//头
//
////		if(CardInfo.card_purchase_type == TT_NORMAL_CONSUME_MONEY)//冲帐交易
////		{
////			temp_snd_buf[5] = (INT8U)(CardInfo.balance >>16);//3字节交易后余额
////			temp_snd_buf[6] = (INT8U)(CardInfo.balance >>8);//3字节交易后余额
////			temp_snd_buf[7] = (INT8U)(CardInfo.balance);//3字节交易后余额
//
////			temp_snd_buf[8] = (INT8U)(CardInfo.fare >>16);	//3字节交易金额
////			temp_snd_buf[9] = (INT8U)(CardInfo.fare >>8);	//3字节交易金额
////			temp_snd_buf[10]= (INT8U)(CardInfo.fare );	//3字节交易金额
//
////			//if(DevStat.employee_a_free == TRUE && CardInfo.card_type == CARDT_FREE)//新增的免费卡票价
////			//{
////			//	temp_snd_buf[8] = (INT8U)(DevStat.free_price>>16);	//3字节交易金额
////			//	temp_snd_buf[9] = (INT8U)(DevStat.free_price >>8);	//3字节交易金额
////			//	temp_snd_buf[10]= (INT8U)(DevStat.free_price );	//3字节交易金额
////			//}
////			memcpy(temp_snd_buf+11, &DevStat.pos_number[0], 6);	//6字节终端机编号
////			//memcpy(temp_snd_buf+11, "\x00\x00\x00\x00\x00\x00", 6);	//6字节终端机编号
////			memcpy(temp_snd_buf+17, &CardInfo.card_in_time, 7);	//7字节交易日期和时间
////			memcpy(temp_snd_buf+24, &CardInfo._CARD_NUMBER.card_number[4], 4);//4字节卡片流水号
////			memcpy(temp_snd_buf+28, "\x80", 1);
////			k = CmdPsam(DevStat.cur_psam_channel, temp_snd_buf, 29, temp_rcv_buf, &rcv_len);		//TAC码计算
////		}
////		else//充值交易
////  	{
//            //80FA010018 + 3字节交易后余额 + 2字节联机交易序号 + 3字节交易金额 +
//            //1字节交易类型 + 6字节终端机编号 + 4字节交易日期 + 3字节交易时间 + 8000
//            temp_snd_buf[5] = (INT8U)(CardInfo.balance / 0x10000);//3字节交易后余额
//            temp_snd_buf[6] = (INT8U)(CardInfo.balance % 0x10000 / 0x100);//3字节交易后余额
//            temp_snd_buf[7] = (INT8U)(CardInfo.balance % 0x100);//3字节交易后余额
//
//            memcpy(&temp_snd_buf[8], &CardInfo.purchase_serial_num[0], 2);//交易序号
//
//            temp_snd_buf[10] = (INT8U)(CardInfo.fare / 0x10000);    //3字节交易金额
//            temp_snd_buf[11] = (INT8U)(CardInfo.fare % 0x10000 / 0x100);    //3字节交易金额
//            temp_snd_buf[12]= (INT8U)(CardInfo.fare % 0x100);   //3字节交易金额
//
//            temp_snd_buf[13] = CardInfo.card_purchase_type ;//交易类型
//
//            memcpy(temp_snd_buf+14, &DevStat.pos_number[0], 6); //6字节终端机编号
//
//            memcpy(temp_snd_buf+20, &CardInfo.card_in_time, 7); //7字节交易日期和时间
//
//            memcpy(temp_snd_buf+27, "\x80\x00", 2);
//            k = CmdPsam(hSam1Handle, temp_snd_buf, 29, temp_rcv_buf, &rcv_len);     //TAC码计算
////  	}
//
//        if ( k != ok )
//            continue;                               //如果不成功则循环计算8次
//        else
//            break;                                  //直至计算成功才退出
//    }
//
//    if ( i >= 8 )
//    {   //TAC码计算失败，全写0
//        memset(&CardInfo.tac[0], 0x00, 4);
////  	printf_debug("TAC码计算失败");
//        return ;
//    }
//    else
//    {   //TAC码计算成功
//        memcpy(&CardInfo.tac[0], &temp_rcv_buf[0], 4);      //保存TAC码
////  	printf_debug("TAC码计算成功");
////  	EA_vDisplay(4, "%02x%02x%02x%02x", CardInfo.tac[0],CardInfo.tac[1],CardInfo.tac[2],CardInfo.tac[3]);
////  	EA_vDelayMs(2000);
//        return;
//    }
	INT8U LOCAL_V_TYPE  i = 0;

	INT8U tac1[4];
	INT8U tac2[4];

	memset(tac1, 0x00, 4);
	memset(tac2,0x11,4);
	
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
函数原型：Get_Record
功能描述：取指定地址的记录

参数描述：
参数名称：	输入/输出？	类型		描述
-----------		-----------	------	   	-------
 rec_buf		输出	INT8U *	取回记录存放在此
 start_position	输入	INT16U		p0+2，即NewPtr，即DevStat.record_base_ptr
 rec_pointer	输入	INT16U		要取记录的指针，即DevStat.record_number

返  回  值：无
作      者	：黄立国、许岩
日      期：2004-10-10
修改历史：
日期		修改人		修改描述
------		---------	-------------
*****************************************************************/
//void Get_Record(INT8U *l_rec_buf, INT16U start_position, INT16U rec_pointer)
//{
//    INT16U LOCAL_V_TYPE rec_ptr = 0;
//    INT16U LOCAL_V_TYPE page = 0;
//    INT8U LOCAL_V_TYPE ptr_in_page = 0;
//
//
//    rec_ptr = rec_pointer + start_position;
//    if ( rec_ptr >= (INT16U)MAX_REC_NUM )
//        rec_ptr = rec_ptr % (INT16U)MAX_REC_NUM;
//
//    page = rec_ptr / 3 + RECORDSTART;               //算出要存储的记录的页号
//    ptr_in_page = rec_ptr % 3;                          //算出页内条号
//
//}

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
//	INT8U   temp[50];
	char    strbuf[50];
	float   fBuf=0;

	EA_vCls();
	EA_ucClrKeyBuf();
//  Beep(2000);
	sprintf(strbuf, "车  号: 鲁C-%s", &CardInfo.bus_number[0]);
	EA_vDisp(1, 1, strbuf);

	if( DevStat.fill_mode == 0 )
	{
		sprintf(strbuf, "油  品: 天然气");
	}
	else if( DevStat.fill_mode == 1 )
	{
		sprintf(strbuf, "油  品: 柴油");
	}	
		else if( DevStat.fill_mode == 2 )
	{
		sprintf(strbuf, "油  品: 汽油");
	}	
	else
	{
		return notok;
	}
	EA_vDisp(2, 1, strbuf);
	fBuf = ((float)(cardinfo->fare))/100;
	sprintf(strbuf, "升  数: %6.2f升", fBuf);
	EA_vDisp(3, 1, strbuf);
	EA_vDisp(4, 1, "加油员: %02d", DevStat.cur_driver_number[0]);
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

// *****************************************************************
// 功能：检索卡片类型信息
// 入口参数：	lkeyvalue->按键值
// 出口参数：	Null
// 作者：
// 函数出处：EH0218---COMMON.C例子程序
// *****************************************************************
void GetCardType(INT8U  cardtype, INT8U * temp)
{

	switch ( cardtype )
	{
		case CARDT_NORMAL:
			strcpy((void *)temp, (void *)"  普通卡");
			break;

		case CARDT_OLDMAN:
			strcpy((void *)temp, (void *)"  老年卡");
			break;

		case CARDT_FREE:
			strcpy((void *)temp, (void *)"  免费卡");
			break;

		case CARDT_MEMORY:
			strcpy((void *)temp, (void *)"  纪念卡");
			break;

		case CARDT_DRIVER:
			strcpy((void *)temp, (void *)"  员工卡");
			break;
		case CARDT_STUDENT:
			strcpy((void *)temp, (void *)"  学生卡");
			break;

		case CARDT_GATHER:
			strcpy((void *)temp, (void *)"  采集卡");
			break;

		case CARDT_SET:
			strcpy((void *)temp, (void *)"  设置卡");
			break;

		case CARDT_SIGNED:
			strcpy((void *)temp, (void *)"  签点卡");
			break;

		case CARDT_CHECKED:
			strcpy((void *)temp, (void *)"   检测卡");
			break;

		case CARDT_INSPECTOR:
			strcpy((void *)temp, (void *)"  稽查卡");
			break;

		case 0xF0:
			strcpy((void *)temp, (void *)"    追加");
			break;

		default :
			strcpy((void *)temp, (void *)" 就 餐 卡");
			break;
	}
}

/******************************************************************************
 函数名称：CPUCardQuery
 功能描述：CPU卡信息查询
 参数描述：
 参数名称：	输入/输出？	类型		描述
 
 返  回  值：ok(0)-成功
				 notok(0xFF)-失败
				   
 作      者	：许岩/刘及华
 日      期：2012-12-12
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
******************************************************************************/
INT8U QueryNormal(void)
{
	
	INT8U   LOCAL_V_TYPE  i = 0;
	INT8U   LOCAL_V_TYPE  j = 0;
	INT8U   LOCAL_V_TYPE    block9[16];
	INT8U   LOCAL_V_TYPE    block10[16];
	INT8U   LOCAL_V_TYPE  temp_buf[50];
	float   fbuf = 0.0;
	INT8U   temp[20];
	INT8U   strbuf[50];
	INT32U  LOCAL_V_TYPE  temp_int32u = 0;
	INT16U  key = EM_key_NOHIT;

	////////////////////////////////////////
	//验证Sector 2 密钥
	////////////////////////////////////////
	i = Get_Sector_PermissionA(2, &KeySector[2][0]);
	if ( i != ok )
	{
		return notok;
	}

	i = EA_ucMIFRead(hMifsHandle, 9, block9);
	if ( i != ok )
	{
		return notok;
	}

	i = EA_ucMIFRead(hMifsHandle, 10, block10);
	if ( i != ok )
		return notok;

	i = Purse_Block_Is_Right(block9);
	j = Purse_Block_Is_Right(block10);

	if ( i == ok )
	{
		temp_int32u = BlockToCash(block9);			//9区合法
		fbuf = (float)(temp_int32u) / 100.0;
	}

	else if ( j == ok )
	{
		temp_int32u = BlockToCash(block10);			 //10区合法
		fbuf = (float)(temp_int32u) / 100.0;
	}

	else
	{
		return notok;		//两区都不合法，退出
	}

	//显示卡片信息
	EA_vCls();
	GetCardType(CardInfo.card_type, temp);

	htoa(temp_buf, &CardInfo._CARD_NUMBER.detail_card_number.serial_number[0], 4);
	strcpy((void *)strbuf, "卡号:   ");
	strcat((void *)strbuf, (void *)temp_buf);
	EA_vDisp(1, 1, (void *)strbuf);

	sprintf((void *)strbuf, "类型: %s", temp);
	EA_vDisp(2, 1, (void *)strbuf);

	sprintf((void *)strbuf, "余额: %8.2f元", fbuf);
	EA_vDisp(3, 1, (void *)strbuf);

	EA_vDisp(4, 1, "    按任意键退出    ");
	Beep(1000);
	key = EA_uiInkey(5);
	return ok;
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
	INT8U  cardtype = 0;

#ifndef Debug_M1card
	ret = CardInit();

	if( ret != ok )
	{
		EA_ucMIFHalt(hMifsHandle);	//挂卡
		return notok;
	}
#endif
    //lcddisperr("请放好卡片别移动");

	ret = M1CardProcess();

	EA_ucMIFHalt(hMifsHandle);	//挂卡

	return ret;//选择退出
}


/******************************************************************************
 函数名称：CardSale
 功能描述：售卡交易流程
 参数描述：
 参数名称：输入/输出？	类型		描述
 
 返  回  值：    ok(0)-成功
				 notok(0xFF)-失败
				 0xFE-没找到卡
				 0xF0-重新刷卡
				   
 作      者	：于福勇
 日      期：2007-01-30
 修改历史：
		日期		修改人		修改描述
******************************************************************************/
INT8U CardSale(void)
{
//    INT8U   LOCAL_V_TYPE  i = 0;
//    INT8U   LOCAL_V_TYPE  j = 0;
//    INT32U  LOCAL_V_TYPE  temp_int32u = 0;
//    INT8S   LOCAL_V_TYPE  h = 0;
////	ulong	ulSnr;
////	uchar   ucResult;
////	uint 	uiSize;
////	ushort  usICType;
//
//    INT8U   LOCAL_V_TYPE  block_buffer1[16] = {0};          //M1卡一个block4的数据缓冲
//    INT8U   LOCAL_V_TYPE  block_buffer2[16] = {0};          //M1卡一个block5的数据缓冲
//    INT8U   LOCAL_V_TYPE  block_buffer3[16] = {0};          //M1卡一个block6的数据缓冲
//    ////////////////////////////////////////
//    //卡片初始化
//    ////////////////////////////////////////
//    if( CardInit() != ok )
//    {
//        //rrCardInfo.sale_error_occured = TRUE;
//        EA_ucMIFHalt(hMifsHandle);  //挂卡
//        return notok;
//    }
//
////	ucResult = EA_ucMIFRequest (hMifsHandle, EM_mifs_IDLE, &usICType);
////	if (ucResult != EM_SUCCESS)
////	{
//////  	lcddisperr("M1卡复位失败");
////		return notok;
////	}
////	ucResult = EA_ucMIFAntiColl (hMifsHandle, &ulSnr);
////	if (ucResult != EM_SUCCESS)
////	{
//////  	lcddisperr("M1卡获取卡号失败");
////		return notok;
////	}
////	CardInfo.csn[3] = (INT8U)(ulSnr / 0x1000000);
////	CardInfo.csn[2] = (INT8U)((ulSnr % 0x1000000) / 0x10000);
////	CardInfo.csn[1] = (INT8U)((ulSnr % 0x10000) / 0x100);
////	CardInfo.csn[0] = (INT8U)(ulSnr % 0x100);
////
////	ucResult = EA_ucMIFSelect( hMifsHandle, ulSnr, &uiSize );
////	if (ucResult != EM_SUCCESS)
////	{
//////  	lcddisperr("M1卡选卡失败");
////		return notok;
////	}
//    ////////////////////////////////////////
//    //读Sector 1 中4 区信息
//    ////////////////////////////////////////
//    memcpy(&KeySector[1][0], &CardInfo.csn[0], 4);
//    KeySector[1][4] = ~KeySector[1][0];
//    KeySector[1][5] = ~KeySector[1][1];
//
//    i = Get_Sector_PermissionA(1, &KeySector[1][0]);
//    if ( i != ok )
//        return notok;
//
//    i = EA_ucMIFRead(hMifsHandle, 4, block_buffer1);
//    if ( i != ok )
//        return notok;
//
//    i = EA_ucMIFRead(hMifsHandle, 5, block_buffer2);
//    if ( i != ok )
//        return notok;
//
//    i = EA_ucMIFRead(hMifsHandle, 6, block_buffer3);
//    if ( i != ok )
//        return notok;
//
//    if ( block_buffer1[12] == 0x01 )      //判断启用标志
//    {
//        if((ErrCardInfo.sale_error_occured == FALSE) && (ErrCardInfo.error_occured == FALSE))
//        {
//            //卡启用处理
//            EA_vCls();
//            EA_vDisplay(2,"     开片已启用     ");
//            EA_uiInkeyMs(500);
//            Beep(1000);
//            (void)EA_ucMIFHalt (hMifsHandle);
//            return ok;
//        }
//    }
//
//    //保存卡的应用系列号
//    memcpy(&CardInfo._CARD_NUMBER.detail_card_number.city_code[0], block_buffer1, 2);   //城市代码
//    memcpy(&CardInfo._CARD_NUMBER.detail_card_number.calling_code[0], block_buffer1+2, 2);  //城市代码
//    memcpy(&CardInfo._CARD_NUMBER.detail_card_number.serial_number[0], &block_buffer1[4], 4);//发行流水号
//    memcpy(&CardInfo.authentication_code[0], &block_buffer1[8], 4);//保存卡认证码
//    CardInfo.card_type = block_buffer1[13]; //卡类型
//
////输入充值金额和人员信息
//    if((ErrCardInfo.error_occured == FALSE)&&(ErrCardInfo.sale_error_occured == FALSE))
//    {
//        i = SaveCardInTime(&CardInfo);//保存时间
//        if ( i != ok )
//        {
//            lcddisperr("时钟错误无法交易");
//            (void)EA_ucMIFHalt (hMifsHandle);
//            return notok;
//        }
//        if( GetTradeInfo() != ok )
//        {
//            lcddisperr("请重新交易");
//            (void)EA_ucMIFHalt (hMifsHandle);
//            return ok;
//        }
//
/////////////////////////////////
////判断操作员卡内金额是否够本次售卡充值金额
//        if( DevStat.oper_card_amt < ( CardInfo.fare + DevStat.deposit_amt[0]*256 + DevStat.deposit_amt[1] ))
//        {
//            lcddisperr("大卡金额不足");
//            (void)EA_ucMIFHalt (hMifsHandle);
//            return ok;
//        }
//
//        /*
//        //扣除操作员卡内金额
//        Cpucard_Init(ICC_SOCK_MAIN);
//        temp_int32u = (INT32U)(CardInfo.deposit_amt[0]<<8) | (INT32U)(CardInfo.deposit_amt[1]);
//        if(OperTradeProcess(0, temp_int32u) != 0)
//        {
//            Cpucard_power_off();
//            return notok;
//        }
//        Cpucard_power_off();
//        */
//    ////////////////////////////////////////
//    //卡片有效期设置
//    //01    普通卡
//    //02    老年(半费)卡
//    //03    长寿卡
//    //05    纪念卡
//    //06    员工卡
//    //07    学生卡
//    //08    职工卡
//    ////////////////////////////////////////
//    //  if(CardInfo.card_type==CARDT_STUDENT || CardInfo.card_type==CARDT_OLDMAN || CardInfo.card_type==CARDT_FREE)//学生卡需要设置时间
//    //  {
//    //      if(SetCardUseDT() != ok)
//    //          return notok;
//    //  }
//
//        if(CardInfo.card_type != CARDT_NORMAL )//只有普通卡可以售卡
//        {
//            lcddisperr("非普通卡不能售卡");
//            (void)EA_ucMIFHalt (hMifsHandle);
//            return ok;
//        }
//    }
//
//
//////////////////////////////////////////
////出错卡处理
//////////////////////////////////////////
//    if (( ErrCardInfo.error_occured == TRUE )||(ErrCardInfo.sale_error_occured == TRUE))
//    {//出错卡重入，PSAM卡不能复位
//        h = memcmp(&CardInfo.csn[0], &ErrCardInfo.csn[0], 4);//出错卡硬件卡号比较
//        if ( h != ok )
//        {
//            lcddisperr("请刷出错卡");
//            (void)Beep(1000);
//            (void)EA_ucMIFHalt (hMifsHandle);
//            return notok;//卡交易出错，但回的卡片不是出错卡
//        }
//    }
//    else
//    {
//        i = PsamRst();//PSAM卡复位
//        if ( i != ok )
//        {
//            lcddisperr("PSAM卡故障");
//            (void)EA_ucMIFHalt (hMifsHandle);
//            return notok;
//        }
//    }
//
//
//////////////////////////////////////////
////后台计算密钥
//////////////////////////////////////////
//
//    if ((ErrCardInfo.error_occured == FALSE)&&(ErrCardInfo.sale_error_occured == FALSE))
//    {
//        DevStat.key_query_flag=TRUE;
//        i = GPRS_CAL_KEY();
//        DevStat.key_query_flag=FALSE;
//    }
//
//    if( i != ok )//操作被取消或者网络故障
//    {
////  	lcddisperr("故障,请与中心联系");
////  	EA_ucMIFHalt();	//挂卡
//        (void)EA_ucMIFHalt (hMifsHandle);
//        return ok;
//    }
//
//////////////////////////////////////////
////提示界面
//////////////////////////////////////////
//    EA_vCls();
//    EA_vDisplay(1,"      卡片售卡      ");
//    EA_vDisplay(2,"  请放好卡片别移动  ");
//    EA_vDisplay(3,"  处理中,请等待...  ");
//    EA_uiInkeyMs(500);
//
//    CardInfo.card_purchase_type = TT_CARD_SALE;
//
//    if (ErrCardInfo.sale_error_occured == FALSE)
//    {
//        i = M1_Card_Credit_Process();
//        if(i == 0xF0)
//        {
//            (void)EA_ucMIFHalt (hMifsHandle);
//            return 0xF0;
//        }
//
//        if(i != ok)
//        {
//            (void)EA_ucMIFHalt (hMifsHandle);
//            return notok;
//        }
//    }
//
////2008-03-14将售卡信息放到充值交易前
////改写1区040506块内容
//    ////////////////////////////////////////
//    //验证Sector 1 密钥
//    ////////////////////////////////////////
////  if( CardInit() != ok )
////  {
////  	ErrCardInfo.sale_error_occured = TRUE;
////  	return 0xF0;
////  }
//
//    i = Get_Sector_PermissionB(1, &KeySector[1][0]);
//    if ( i != ok )
//    {
//        ErrCardInfo.sale_error_occured = TRUE;
//        return 0xF0;
//    }
//
//    //改写4，5，6块信息
//    block_buffer1[12] = 0x01;//启用标志
//    memcpy(&block_buffer1[14], &DevStat.deposit_amt[0],2);
//
//    memcpy(&block_buffer2[0], &CardInfo.card_begin_dt[0], 4);
//    memcpy(&block_buffer2[4], &CardInfo.card_useful_dt[0],4);
//    memcpy(&block_buffer2[13], &DevStat.equ_id[0], 3);
//
//    memcpy(&block_buffer3[0], &CardInfo.card_add_dt[0], 4);
//    block_buffer3[7] = (CardInfo.fare / 0x10000);
//    block_buffer3[8] = (CardInfo.fare % 0x10000 / 0x100);
//    block_buffer3[9] = (CardInfo.fare % 0x100);
//
//    block_buffer3[10] = block_buffer3[7];//充值金额
//    block_buffer3[11] = block_buffer3[8];
//    block_buffer3[12] = block_buffer3[9];
//
//    memcpy(&block_buffer3[13], &DevStat.cur_driver_number[1], 3);//操作员号        //by liujihua
//
//
//    //扣除操作员卡内金额押金
//
//    //取出操作员卡内押金
//
//    Cpucard_Init();
//
//    temp_int32u = DevStat.deposit_amt[0] * 0x100 + DevStat.deposit_amt[1];
//    if(OperTradeProcess(0, temp_int32u) != ok)
//    {
//        Cpucard_power_off();
//        ErrCardInfo.sale_error_occured = TRUE;
//        lcddisperr("大卡故障不能操作");
//        EA_ucMIFHalt(hMifsHandle);  //挂卡
//        return 0xF0;
//    }
//    Cpucard_power_off();
//
//    i = EA_ucMIFWrite(hMifsHandle, 6, block_buffer3);
//    if ( i != ok )
//    {
//        //ErrCardInfo.error_occured = TRUE;
//        ErrCardInfo.sale_error_occured = TRUE;
//        return 0xF0;
//    }
//
//    i = EA_ucMIFWrite(hMifsHandle, 5, block_buffer2);
//    if ( i != ok )
//    {
//        //ErrCardInfo.error_occured = TRUE;
//        ErrCardInfo.sale_error_occured = TRUE;
//        return 0xF0;
//    }
//
//    i = EA_ucMIFWrite(hMifsHandle, 4, block_buffer1);
//    if ( i != ok )
//    {
//       // ErrCardInfo.error_occured = TRUE;
//        ErrCardInfo.sale_error_occured = TRUE;
//        return 0xF0;
//    }
//
////保存售卡记录
//    CardInfo.card_purchase_type = TT_CARD_SALE;
//    (void)StoreRecord(&CardInfo);           //储存售卡记录
//
////保存充值记录
//    CardInfo.card_purchase_type = TT_CREDIT_MONEY;
//    CalTAC();
//    //EA_ucMIFHalt();   //挂卡
//    EA_ucMIFHalt(hMifsHandle);
//    (void)StoreRecord(&CardInfo);           //储存充值记录
//
////显示结果
//    CardInfo.card_purchase_type = TT_CARD_SALE;
//    DisplayPurchaseResult(&CardInfo);   //显示交易结果
//
//////保存日结押金
//    DevStat.binhand.sale_cnt++;
//    DevStat.binhand.deposit_amt += (DevStat.deposit_amt[0] *256 + DevStat.deposit_amt[1])/100;
//    DevStat.last_trade_type = TT_CARD_SALE;
//    DevStat.gray_flag = FALSE;
//    WriteParam();
//
//    if(DevStat.mode == 0x00)
//    {
////  	EA_ucSetStopFlag(EM_DISABLE_STOP);
//        print_sale_record();
////  	EA_ucSetStopFlag(EM_ENABLE_STOP);
//    }
//
////增加数据上传
//    //数据上传
//    i = GPRS_SEND_RECORD();
//    if(i == ok)
//    {
//        DevStat.have_data_not_send_flag = FALSE;
//    }
//
////保存充值密钥
//    for(j=1;j<7;j++)
//    memcpy(DevStat.KEYVALUE[j-1], KeySector[j], 6);
//    WriteParam();
//
//
//    //恢复状态
//    memset(&CardInfo, 0x00, sizeof(CardInfo));      //清卡片交易信息
//    memset(&ErrCardInfo, 0x00, sizeof(ErrCardInfo));    //清出错卡信息
//    //DispPurchaseResultDelay = DISP_DELAY;
//    DevStat.purchase_again_req = FALSE;                 //清重新刷卡语音标志
//    DevStat.voice_purchase_again_freq = FALSE;
//    (void)EA_ucMIFHalt (hMifsHandle);
//    return i;
return ok;
}
//
///******************************************************************************
// 函数名称：CardCreditAmt
// 功能描述：充值交易流程
// 参数描述：
// 参数名称：输入/输出？  类型        描述
//
// 返  回  值：    ok(0)-成功
//                 notok(0xFF)-失败
//                 0xFE-没找到卡
//                 0xF0-重新刷卡
//
// 作      者 ：于福勇
// 日      期：2007-01-30
// 修改历史：
//        日期        修改人      修改描述
//******************************************************************************/
//INT8U CardCreditAmt(void)
//{
//    INT8U   LOCAL_V_TYPE  i = 0;
//    INT8U   LOCAL_V_TYPE  j = 0;
//    INT32U  LOCAL_V_TYPE  start_using_date = 0;             //启用日期
//    INT32U  LOCAL_V_TYPE  end_using_date = 0;               //有效截至日期
//    INT32U  LOCAL_V_TYPE  cur_date = 0;                         //当前时间
//    INT8S   LOCAL_V_TYPE  h = 0;
//    INT8U   LOCAL_V_TYPE  block_buffer1[16] = {0};          //M1卡一个block4的数据缓冲
//    INT8U   LOCAL_V_TYPE  block_buffer2[16] = {0};          //M1卡一个block5的数据缓冲
//    INT8U   LOCAL_V_TYPE  block_buffer3[16] = {0};          //M1卡一个block6的数据缓冲
//    INT16U  key;
//    INT8U   buffer[16] = {0};           //M1卡一个block4的数据缓冲
//////////////////////////////////////////
////卡片初始化
//////////////////////////////////////////
//    if( CardInit() != ok )
//    {
//        EA_ucMIFHalt(hMifsHandle);  //挂卡
//        return notok;
//    }
//
////输入充值金额和人员信息
//    if( ErrCardInfo.error_occured == FALSE )
//    {
//        if( GetTradeAmt() != ok )
//        {
//            EA_ucMIFHalt(hMifsHandle);
//            return ok;
//        }
//
/////////////////////////////////
////判断操作员卡内金额是否够本次售卡充值金额
//        if( DevStat.oper_card_amt < CardInfo.fare )
//        {
//            lcddisperr("大卡金额不足");
//            EA_ucMIFHalt(hMifsHandle);
//            return ok;
//        }
///*
//        //扣除操作员卡内金额
//        Cpucard_Init(ICC_SOCK_MAIN);
//        if(OperTradeProcess(0, CardInfo.fare) != 0)
//        {
//            Cpucard_power_off();
//            EA_vCls();
//            //printf_debug("1");
//            MC530Off();
//            InitMC530(TYPE_A_ISO14443);
//            return notok;
//        }
//        Cpucard_power_off();
//*/
//    }
//
////  MC530Off();
////  if( InitMC530(TYPE_A_ISO14443) != MI_OK)
////  	return notok;
//
////  if(CardInit() != ok)
////  	return notok;
//////////////////////////////////////////
////出错卡处理
//////////////////////////////////////////
//    if ( ErrCardInfo.error_occured == TRUE )
//    {//出错卡重入，PSAM卡不能复位
//        h = memcmp(&CardInfo.csn[0], &ErrCardInfo.csn[0], 4);//出错卡硬件卡号比较
//        if ( h != ok )
//        {
//            lcddisperr("请刷出错卡");
//            EA_ucMIFHalt(hMifsHandle);  //挂卡
//            Beep(1000);
//            return notok;//卡交易出错，但回的卡片不是出错卡
//        }
//    }
//    else
//    {
//        i = SaveCardInTime(&CardInfo);//保存时间
//        if ( i != ok )
//            return notok;
///*		//判断是否是当日工作
//        if(memcmp(DevStat.binhand.end_date, (INT8U *)&CardInfo.card_in_time, 4) != 0)//需要做日结
//        {
//            printf_debug("请先做日结");
//            return ok;
//        }
//*/
//    }
//
//
//    i = PsamRst();//PSAM卡复位
//    if ( i != ok )
//    {
//        lcddisperr("PSAM卡故障");
//        //j =EA_ucMIFHalt();    //挂卡
//        EA_ucMIFHalt(hMifsHandle);
//        return notok;
//    }
//
//////////////////////////////////////////
////读Sector 1 中4 区信息
//////////////////////////////////////////
//    memcpy(&KeySector[1][0], &CardInfo.csn[0], 4);
//    KeySector[1][4] = ~KeySector[1][0];
//    KeySector[1][5] = ~KeySector[1][1];
//
//    i = Get_Sector_PermissionA(1, &KeySector[1][0]);
//    if ( i != ok )
//        return notok;
//
//    i = EA_ucMIFRead(hMifsHandle, 4, block_buffer1);
//    if ( i != ok )
//        return notok;
//
//    i = EA_ucMIFRead(hMifsHandle, 5, block_buffer2);
//    if ( i != ok )
//        return notok;
//
//    i = EA_ucMIFRead(hMifsHandle, 6, block_buffer3);
//    if ( i != ok )
//        return notok;
//
//    if ( block_buffer1[12] != 0x01 )      //判断启用标志
//    {
//        //卡启用处理
//        lcddisperr("卡片未启用");
//        Beep(1000);
//        EA_uiInkey(2);
//        EA_ucMIFHalt(hMifsHandle);
//        return ok;
//    }
//
//    memcpy(&start_using_date, block_buffer2, 4);//启用日期
//    memcpy(&end_using_date, block_buffer2+4, 4);//有效截至日期
////  memcpy(&buffer, block_buffer2, 4);//启用日期
////  over_turn(4, buffer);
////  start_using_date = unpackInt(buffer);
////
////  memcpy(&buffer, block_buffer2+4, 4);//有效截至日期
////  over_turn(4, buffer);
////  end_using_date = unpackInt(buffer);
//
//    #ifdef DEBUG
//        lcddisperr("%04d", start_using_date);
//        lcddisperr("%04d", end_using_date);
//    #endif
//
//    //保存卡的应用系列号
//    memcpy(&CardInfo._CARD_NUMBER.detail_card_number.city_code[0], block_buffer1, 2);   //城市代码
//    memcpy(&CardInfo._CARD_NUMBER.detail_card_number.calling_code[0], block_buffer1+2, 2);  //城市代码
//    memcpy(&CardInfo._CARD_NUMBER.detail_card_number.serial_number[0], &block_buffer1[4], 4);//发行流水号
//    memcpy(&CardInfo.authentication_code[0], &block_buffer1[8], 4);//保存卡认证码
//    CardInfo.card_type = block_buffer1[13]; //卡类型
//
///*
//    if ( DevStat.record_number >= (MAX_REC_NUM - 30) )
//    {
//        (void)EA_vCls();
//        DispStr_CE(0, 3, "记录区已满!", DISP_POSITION);
//        Beep(1000);
//        (void)delay_and_wait_key(3, 0, 0);
//        ////////////////////////////////////////
//        //M1卡挂卡
//        ////////////////////////////////////////
//        j =EA_ucMIFHalt();  //挂卡
//        return ok;                                      //容量已满
//    }
//*/
//    if(CardInfo.card_type > 0x10)
//    {
//        lcddisperr((void *)"管理卡不能充值!");
//        EA_ucMIFHalt(hMifsHandle);
//        return notok;
//    }
//
//    if(CardInfo.card_type == CARDT_FREE)
//    {
//        lcddisperr((void *)"免费卡不能充值!");
//        EA_ucMIFHalt(hMifsHandle);
//        return notok;
//    }
//
//    ////////////////////////////////////////
//    //判断有效期
//    ////////////////////////////////////////
//
////  memcpy(&cur_date, &CardInfo.card_in_time, 4);//取当前日avoid warning
//    memcpy(&buffer, &CardInfo.card_in_time, 4);//取当前日avoid warning
//    over_turn(4, buffer);
//    cur_date = unpackInt(buffer);
//    i = CheckPeriodOfValidity(start_using_date, cur_date, end_using_date);  //有效期判断刘及华，调试用
//    if ( i != ok )
//    {//过期处理，不允许交易
//        EA_vCls();
//        EA_vDisp(4, 1, "  此卡过期,继续吗?  ");
//        key = EA_uiInkey(0);
//        switch(key)
//        {
//           case EM_key_ENTER:
//               break;
//           case EM_key_CANCEL:
//               EA_ucMIFHalt(hMifsHandle);
//               return notok;
//        }
//
//        //if(select_affirm(4, (void *)"此卡过期,继续吗?") != 0)
////  	{
////  		j = EA_ucMIFHalt();	//挂卡
////  		EA_ucMIFHalt(hMifsHandle);
////  		return notok;
////  	}
//    }
///*
//
//    switch ( CardInfo.card_type )
//    {
//        case CARDT_NORMAL:  //普通卡
//        case CARDT_MEMORY:  //纪念卡
//            //做有效期判断
//            memcpy(&cur_date, &CardInfo.card_in_time, 4);//取当前日avoid warning
//            i = CheckPeriodOfValidity(start_using_date, cur_date, end_using_date);  //有效期判断
//            if ( i != ok )
//            {//过期处理，不允许交易
//                printf_debug("过有效期");
//                j = EA_ucMIFHalt(); //挂卡
//                return notok;
//            }
//            break;  //v0.1.1 纪念卡加有效期判断
//
//        default:
//            //其他管理卡统一做有效期判断，免费卡如果过期也提示错误
//            memcpy(&cur_date, &CardInfo.card_in_time, 4);//取当前日avoid warning
//            i = CheckPeriodOfValidity(start_using_date, cur_date, end_using_date);  //有效期判断
//            if ( i != ok )
//            {//过期处理，不允许交易
//                printf_debug("过有效期");
//                j = EA_ucMIFHalt(); //挂卡
//                return notok;
//            }
//            break;  //v0.1.1 纪念卡加有效期判断
//    }
//*/
//////////////////////////////////////////
////后台计算密钥
//////////////////////////////////////////
////  MC530Off();
//
//    DevStat.key_query_flag = TRUE;
////	i = GPRSProcess();
//    i = GPRS_CAL_KEY();
//    DevStat.key_query_flag = FALSE;
//
//    if(i != ok)//操作被取消或者网络故障
//    {
//        EA_ucMIFHalt(hMifsHandle);
//        return ok;
//    }
//
//
//////////////////////////////////////////
////提示界面
//////////////////////////////////////////
//    EA_vCls();
//    EA_vDisplay(1,"  卡片充值即将完成  ");
//    EA_vDisplay(2,"  请放好卡片别移动  ");
//    EA_vDisplay(3,"  处理中,请等待...  ");
//    EA_uiInkeyMs(500);
///*
//////////////////////////////////////////
////ISAM卡计算密钥
//////////////////////////////////////////
//
//    if(DevStat.samcompcode==1)//捷德卡片
//    {
//        ////////////////////////////////////////
//        //ISAM卡选1001目录
//        ////////////////////////////////////////
//        k = CmdPsam(DevStat.cur_psam_channel, (void *)SELECT_JDSAM_ADF1, sizeof(SELECT_JDSAM_ADF1),
//                    temp_buf, &rcv_length);     //选1001目录
//        if ( k != 0x9000 )
//            return notok;
//
//        ////////////////////////////////////////
//        //ISAM卡算各区密钥
//        ////////////////////////////////////////
//
//        //80FC010111'+ astr_readcard.city_code + astr_readcard.card_hardno + right(astr_readcard.card_no, 4) + astr_readcard.card_auth + '0102030405'
//
//        memcpy(temp_snd_buf, GET_M1_KEY, sizeof(GET_M1_KEY));//头
//        memcpy(&temp_snd_buf[5], &CardInfo._CARD_NUMBER.detail_card_number.city_code[0], 2);//城市代码
//        memcpy(&temp_snd_buf[7], &CardInfo.csn[0], 4);//CSN硬件卡号
//        memcpy(&temp_snd_buf[11], &CardInfo._CARD_NUMBER.detail_card_number.serial_number[2], 2);//卡发行流水号后2字节
//        memcpy(&temp_snd_buf[13], &CardInfo.authentication_code[0], 4);
//        memcpy(&temp_snd_buf[17],"\x01\x02\x03\0x04\0x05", 5);
//        k = CmdPsam(DevStat.cur_psam_channel, temp_snd_buf, 22, temp_rcv_buf, &rcv_length);//算MAC1
//        if ( k != 0x9000 )
//            return notok;
//        for(cnt=0;cnt<5;cnt++)
//        {
//            memcpy(&KeySector[cnt+1][0], &temp_rcv_buf[cnt*6], 6);//保存计算出的密钥，2sector
//        }
//        memcpy(&temp_snd_buf[17],"\x06\x0A\x0C\x0B\x0D", 5);
//        k = CmdPsam(DevStat.cur_psam_channel, temp_snd_buf, 13, temp_rcv_buf, &rcv_length);//算MAC1
//        if ( k != 0x9000 )
//            return notok;
//
//        memcpy(&KeySector[6][0], &temp_rcv_buf[0], 6);//保存计算出的密钥，2sector
//    }
//    if(DevStat.samcompcode==0)//握奇卡片
//    {
//        ////////////////////////////////////////
//        //ISAM卡选1001目录
//        ////////////////////////////////////////
//        k = CmdPsam(DevStat.cur_psam_channel, (void *)SELECT_SAM_ADF1, sizeof(SELECT_SAM_ADF1),
//                    temp_buf, &rcv_length);     //选1001目录
//        if ( k != 0x9000 )
//            return notok;
//
//        ////////////////////////////////////////
//        //ISAM卡算各区密钥
//        ////////////////////////////////////////
//        for(cnt=1;cnt<7;cnt++)
//        {
//            if(cnt==7 ||cnt==8 || cnt==9)
//                continue;
//            memcpy(temp_snd_buf, ISAM_CAL_KEY_INIT, sizeof(ISAM_CAL_KEY_INIT));//头
//            memcpy(&temp_snd_buf[5], &CardInfo._CARD_NUMBER.detail_card_number.city_code[0], 2);//城市代码
//            memcpy(&temp_snd_buf[7], &CardInfo.csn[0], 4);//CSN硬件卡号
//            memcpy(&temp_snd_buf[11], &CardInfo._CARD_NUMBER.detail_card_number.serial_number[2], 2);//卡发行流水号后2字节
//
//            k = CmdPsam(DevStat.cur_psam_channel, temp_snd_buf, 13, temp_rcv_buf, &rcv_length);//算MAC1
//
//            if ( k != 0x9000 )
//                return notok;
//
//            memcpy(temp_snd_buf, ISAM_CAL_KEY, sizeof(ISAM_CAL_KEY));//头
//            memcpy(&temp_snd_buf[5], &CardInfo._CARD_NUMBER.detail_card_number.city_code[0], 2);//城市代码
//            memcpy(&temp_snd_buf[7], &CardInfo.csn[0], 4);//CSN硬件卡号
//            temp_snd_buf[11] = cnt;//扇区号
//            temp_snd_buf[12] = 0x00;
//
//            k = CmdPsam(DevStat.cur_psam_channel, temp_snd_buf, 13, temp_rcv_buf, &rcv_length);//算MAC1
//
//            if ( k != 0x9000 )
//                return notok;
//
//            memcpy(&KeySector[cnt][0], &temp_rcv_buf[0], 6);//保存计算出的密钥，2sector
//        }
//    }
//*/
//    ////////////////////////////////////////
//    //查黑名单
//    ////////////////////////////////////////
//
///*
//    memcpy(&temp_int32u, &CardInfo._CARD_NUMBER.detail_card_number.serial_number[0], 4);
//    i = BlackName_Find(temp_int32u);    //黑名单判断
//    if ( i == ok )
//    {   //发现是黑名单卡
//#ifdef DEBUG
//        TRACE232("find black name card!\n\r");
//#endif
//    //  i = Lock_M1_Card_Process();//黑名单卡处理，锁卡
//    //  return i;
//        if(select_affirm(4, "非法卡是否继续?") != 0)
//            return notok;
//    }
//*/
////  MC530Off();
////
////  if( InitMC530(TYPE_A_ISO14443) != MI_OK)
////  	return notok;
////充值交易
//    CardInfo.card_purchase_type = TT_CREDIT_MONEY;  //充值交易类型
//    i = M1_Card_Credit_Process();
//    if(i == 0xF0)
//    {
////  	lcddisperr("i == 0xf0");
//        EA_ucMIFHalt(hMifsHandle);  //挂卡
//        return 0xF0;
//    }
//
//    if(i != ok)
//    {
////  	lcddisperr("i != ok");
//        EA_ucMIFHalt(hMifsHandle);  //挂卡
//        return notok;
//    }
//
////保存充值记录
//    CardInfo.card_purchase_type = TT_CREDIT_MONEY;
//    (void)CalTAC();
//    (void)EA_ucMIFHalt(hMifsHandle);     //挂卡
//
//    (void)StoreRecord(&CardInfo);           //储存充值记录
//    (void)DisplayPurchaseResult(&CardInfo); //显示交易结果
//
//    //记录最后一次的交易类型
//    DevStat.last_trade_type = TT_CREDIT_MONEY;
//    DevStat.gray_flag = FALSE;
//    WriteParam();
//
//    if(DevStat.mode==0x00)
//    {
////  	EA_ucSetStopFlag(EM_DISABLE_STOP);							 //禁止进入省点模式
//        print_record();//打印记录
////  	EA_ucSetStopFlag(EM_ENABLE_STOP);
//    }
//
////数据上传
////  MC530Off();
//    i = GPRS_SEND_RECORD();
//    if(i == ok)
//    {
//        DevStat.have_data_not_send_flag = FALSE;
//    }
//
////保存充值密钥
//    for(j=2;j<7;j++)
//    {
//        memcpy(DevStat.KEYVALUE[j-2], KeySector[j], 6);
//    }
//    WriteParam();
//
////	GPRSProcess();
//    //恢复状态
//    memset(&CardInfo, 0x00, sizeof(CardInfo));      //清卡片交易信息
//    memset(&ErrCardInfo, 0x00, sizeof(ErrCardInfo));    //清出错卡信息
//    //DispPurchaseResultDelay = DISP_DELAY;
//    DevStat.purchase_again_req = FALSE;                 //清重新刷卡语音标志
//    DevStat.voice_purchase_again_freq = FALSE;
//
////记录上传
//    return i;
//}
/*****************************************************************
函数原型：unpackInt(unsigned char *msg)
功能描述：///\x00\x00\x00\x64 -> 100
参数描述：
参数名称：	输入/输出？	类型		描述
-----------		-----------	------	   	-------
返 回 值：无
*****************************************************************/
unsigned int unpackInt(unsigned char *msg)
{
	union
	{
		int i;
		struct 
		{
			unsigned char ch1;
			unsigned char ch2;
			unsigned char ch3;
			unsigned char ch4;
		} ch;
	} ui;
	
	ui.ch.ch4=msg[0];
	ui.ch.ch3=msg[1];
	ui.ch.ch2=msg[2];
	ui.ch.ch1=msg[3];
	
	return ui.i;
}
/*****************************************************************
函数原型：over_turn(unsigned char length,unsigned char *ptr)
功能描述：本函数为字符串高低字节转换功能
参数描述：lengrth = 字符串长度 
		  *ptr    = 输入的字符串 
参数名称：	输入/输出？	类型		描述
-----------		-----------	------	   	-------
返 回 值：无
*****************************************************************/
void over_turn(unsigned char length,unsigned char *ptr)
{
	unsigned char i;
	unsigned char temp[16];
	memcpy(temp,ptr,length);
	for(i=0;i<length;i++)
	{
		ptr[i]=temp[length-i-1];
	}
}
/*****************************************************************
函数原型：Cpucard_Init
功能描述：本函数初始化SAM OR CPU卡通道
参数描述：无
参数名称：	输入/输出？	类型		描述
-----------		-----------	------	   	-------
返 回 值：无
*****************************************************************/
INT8U Cpucard_Init()  
{	
//  INT16U key = 0;
//
//  (void)EA_vCls();
//  while( EA_ucICCardIn(hCpuHandle) != EM_SUCCESS )
//  {
//  	(void)EA_vDisplay(2, "     请插入CPU卡    ");
//  	key = EA_uiInkeyMs(10);
//  	if( key == EM_key_CANCEL )
//  		return notok;
//  }
	return ok;	 
}
	
/*****************************************************************
函数原型：Cpucard_power_off
功能描述：本函数关闭通道
参数描述：无
参数名称：	输入/输出？	类型		描述
-----------		-----------	------	   	-------
返 回 值：无
*****************************************************************/
void Cpucard_power_off()
{
//  uchar result;
//  result = EA_ucICPowerDown(hCpuHandle);
//  if  (result != EM_SUCCESS)
//  {
//  	return;
//  }

}
//
///*****************************************************************
//函数原型：CPU_SET_PROCESS
//功能描述：本函数为CPU设置卡处理过程
//参数描述：无
//参数名称：	输入/输出？	类型		描述
//-----------		-----------	------	   	-------
//返 回 值：无
//*****************************************************************/
//int CPU_SET_PROCESS(INT8U * data)
//{
///*
//    卡类型标识  1
//    充值金额上限    3
//    卡内余额上限    4
//    操作员号    3
//    操作员密码  3
//    允许经营时间    4
//    售卡点中心代码  2
//    售卡点代码  2
//    机具代码    3
//    中心服务器地址  4
//    服务器端口号    2
//    卡片状态    1
//    允许充值次数    1
//*/
//
////	DevStat.max_price_limit = HextoBcd(*(data+1)) * 10000  + HextoBcd(*(data+2))*100 + HextoBcd(*(data+3));
////	DevStat.max_card_limit = HextoBcd(*(data+4)) * 1000000  + HextoBcd(*(data+5))*10000 + HextoBcd(*(data+6))*100 + HextoBcd(*(data+7));
//
//    DevStat.max_price_limit = bcdhex(*(data+1)) * 10000  + bcdhex(*(data+2)) * 100 + bcdhex(*(data+3));
//    DevStat.max_card_limit = bcdhex(*(data+4)) * 1000000  + bcdhex(*(data+5)) * 10000 + bcdhex(*(data+6)) * 100 + bcdhex(*(data+7));
//
//    htoa(&DevStat.card_password[0], data+11, 3);
//    memcpy(&DevStat.acnt_id[0], data+18, 2);
////  memcpy(&DevStat.spot_id[0], data+20, 2);    by liujihua  没有找到原因，错误显示：2600，正确数值为：0032
//    memcpy(&DevStat.spot_id[0], data+18, 2);
//    memcpy(&DevStat.equ_id[0], data+22, 3);
//
//    memcpy(&DevStat.deposit_amt[0], data+34, 2);
//
//    //服务器地址
//    memcpy(&DevStat.ServerIp[0], data+25, 4);
//    //服务器端口号
//    memcpy(&DevStat.ServerPort[0], data+29, 2);
//
//    if(*(data+31)==0x02)//此卡已经停用
//        return -1;
//
//    DevStat.have_been_set = TRUE;
//    DevStat.driver_is_working=FALSE;
//
//    WriteParam();
//
//    return 1;
//
//}

//
///*****************************************************************
//函数原型：CPU_OPER_PROCESS
//功能描述：本函数为CPU操作员卡处理过程
//参数描述：无
//参数名称：	输入/输出？	类型		描述
//-----------		-----------	------	   	-------
//返 回 值：     -1:上一个操作员没有签退
//               -2:操作员密码错误
//               -3:卡片已经过有效期
//               -4:卡片已经停用
//               -5: 没有可用的有效次数
//               -6: 什么都不做，直接退出
//               1:  成功
//*****************************************************************/
//int CPU_OPER_PROCESS(INT8U * data)
//{
///*
//    卡类型标识  1
//    充值金额上限    3
//    卡内余额上限    4
//    操作员号    3
//    操作员密码  3
//    允许经营时间    4
//    售卡点中心代码  2
//    售卡点代码  2
//    机具代码    3
//    中心服务器地址  4
//    服务器端口号    2
//    卡片状态    1
//    允许充值次数    1
//*/
//    BUS_TIME            time_data;
//    INT8U               temp_rcv_buf[50], temp_snd_buf[50];
//    INT8U               i;
//    INT16U              k;
//    uint                rcv_len = 0;
//    int                 ret = 0;
//
////取系统日期
//    Get_Time(&time_data);
//
//    temp_snd_buf[0] = time_data.century;
//    temp_snd_buf[1] = time_data.year;
//    temp_snd_buf[2] = time_data.month;
//    temp_snd_buf[3] = time_data.day;
////卡片状态监测
//
//    if(*(data+31)==0x02)//此卡已经停用
//        return -4;
//
////操作员工号保存
//    if(!DevStat.driver_is_working)
//    {
//        DevStat.cur_driver_number[0] = 0x30;
//        memcpy(&DevStat.cur_driver_number[1], data+8, 3);          //by liujihua
//        memset(&DevStat.binhand, 0x00, sizeof(DevStat.binhand));
//        memcpy(&DevStat.binhand.oper_id[0], data+8, 3);
//    //  memcpy(&DevStat.binhand.end_date[0], temp_snd_buf, 4);
//    }
//    else
//    {
//        if(memcmp(&DevStat.cur_driver_number[1], data+8, 3) != 0)//不是同一个操作员卡  //by liujihua
//        {
//            return -1;
//        }
//    }
//
//    htoa(&DevStat.card_password[0], data+11, 3);
//
//
////操作员卡密码检验
////返回值	== 0  正确输入
////			== 1  未输入字符
////			== -1 按退出键
////			== -2 拔卡退出
////			== -3 按FUNC2退出
////			== -4 按FUNC1退出
//
//    ret = OperPasswordCheck();
//    if(ret == notok)//密码检验错误
//    {
//
//        lcddisperr("密码错误一次");
//        ret = OperPasswordCheck();
//        if(ret != ok)
//        {
//            lcddisperr("密码错误二次");
//
//            ret = OperPasswordCheck();
//            if(ret != ok)
//            {
//                lcddisperr("密码错误三次");
//                *(data+31) = 0x02;
//                for ( i=0; i<8; i++ )             //如果计算不成功，则最多循环计算8次
//                {
//                    ////////////////////////////////////////
//                    //更改卡片使用与否标识
//                    ////////////////////////////////////////
//                    memcpy(temp_snd_buf, WRITE_15_FILE, 5);
//
//                    memcpy(&temp_snd_buf[5], data, 36);
//                    k = CmdPsam(hCpuHandle, temp_snd_buf, 41, temp_rcv_buf, &rcv_len);      //TAC计算初始化
//                    if ( k != ok )
//                        continue;
//                    else
//                        break;
//                }
//                return -2;//密码验证错误，直接退出
//            }
//        }
//    }
//    else if( ret == -6 )
//    {
//        return -6;
//    }
//
////有效期检测
//    memcpy(&DevStat.useful_datetime[0],data+14, 4);
//    if(memcmp(&DevStat.useful_datetime[0], temp_snd_buf, 4) < 0)
//        return -3;
//
//////卡片有效次数
////	DevStat.useful_cnt = *(data+32);
////	if(DevStat.useful_cnt <=0)
////		return -5;
//
//    DevStat.driver_is_working = TRUE;
//
//    //读取16文件内容－－读取卡内金额
//
//    k = CmdPsam(hCpuHandle, (void *)READ_16_FILE, sizeof(READ_16_FILE) ,temp_rcv_buf, &rcv_len);
//    if ( k != ok )
//    {
//        EA_vCls();
//        EA_vDisplay(1,"读取钱包文件错误");
//        EA_vDisplay(2,"按下任意键退出");
//        EA_uiInkey(0);
//        return -5;
//    }
//
//    if( Purse_Block_Is_Right(temp_rcv_buf) != ok )//钱包格式检验
//    {
//        EA_vCls();
//        EA_vDisp(1,1,"操作员卡钱包损坏");
//        EA_vDisp(2,1,"操作无法继续进行");
//        EA_vDisp(3,1,"请持卡到中心处理");
//        EA_vDisp(4,1,"按下任意键退出");
//        EA_uiInkey(0);
//        return -5;
//    }
//
//    DevStat.oper_card_amt = (((INT32U)temp_rcv_buf[0] * 0x1000000 + (INT32U)temp_rcv_buf[1] * 0x10000 + (INT32U)temp_rcv_buf[2] * 0x100 + (INT32U)temp_rcv_buf[3]));
//
//    if( !DevStat.binhand.begin_flag )
//    {
//        memcpy(&DevStat.binhand.work_datetime, &time_data, sizeof(BUS_TIME));//begin work datetime
//        DevStat.binhand.begin_flag = TRUE;
////SEND OPER CARDNO TO BANKSERVICE. BANKSERVICE CHECK if OPER CARD IS BLACK.
//        if(GPRS_OPER_CARD_CHECK(DevStat.oper_cardno) != ok)
//        {
//            DevStat.driver_is_working = FALSE;
//            return -6;
//        }
//
//    }
//
//    //20071019 modify by yfy add oper card_amt_e
//    DevStat.binhand.cpu_card_amt_b = DevStat.oper_card_amt ;
//    DevStat.driver_is_working = TRUE;
//    WriteParam();
//
//    return 1;
//}
//
//
///*****************************************************************
//函数原型：CpuDecrement
//功能描述：本函数为操作员卡扣款
//参数描述：无
//参数名称：	输入tFlag---交易类型:0---扣款交易，1:充值交易
/////输出？	类型		描述
//-----------		-----------	------	   	-------
//返 回 值：	-1:无卡
//              -2:选择目录错误
//              -3:口令验证错误
//              -4:
//
//*****************************************************************/
//int CpuDecrement(INT8U * data, INT32U trade_amt)
//{
//    INT32U          fCpuBalance;
////	unsigned long  lCpuBalance;
//    INT8U           cnt=0;
//    unsigned int    LOCAL_V_TYPE  rcv_len = 0;
//    INT8U           temp_rcv_buf[50], temp_snd_buf[50];
//    uchar           k=0;
//
//    if(trade_amt == 0)
//        return ok;
//
//    fCpuBalance = ( ((INT32U)data[0] * 0x1000000) + ((INT32U)data[1] * 0x10000 ) + ((INT32U)data[2] * 0x100 ) + (INT32U)data[3] ) ;
//
////  fCpuBalance = 1003000;
//
//    if( ErrCardInfo.error_occured == TRUE || ErrCardInfo.sale_error_occured == TRUE)
//    {
//        if( fCpuBalance == ErrCardInfo.init_cpu_card_balance )//没有扣款
//        {
//
//            //lcddisperr("没有减款");
//        }
//        else
//        {
////			DevStat.oper_card_amt -= trade_amt;
////			DevStat.binhand.credit_amt += trade_amt;
////			DevStat.binhand.credit_amt_cnt ++;
////			WriteParam();
//            return ok;     //刘及华，这里的处理是没有扣款的话，把款扣掉
//        }
//    }
//
//    ErrCardInfo.init_cpu_card_balance = fCpuBalance;
//
//    if(fCpuBalance < trade_amt)
//    {
//        EA_vCls();
//        EA_vDisplay(1,"操作员卡金额不足");
//        EA_vDisplay(2,"操作无法继续进行败");
//        EA_vDisplay(3,"请持卡到中心充值");
//        EA_vDisplay(4,"按下任意键退出");
//        EA_uiInkey(5);
//        return notok;
//    }
//
//    fCpuBalance -= trade_amt;
//
//    data[0] = (INT8U)(fCpuBalance / 0x1000000 );
//    data[1] = (INT8U)(fCpuBalance % 0x1000000 / 0x10000 );
//    data[2] = (INT8U)(fCpuBalance % 0x10000 / 0x100 );
//    data[3] = (INT8U)(fCpuBalance % 0x100 );
//
//    data[4] = ~(INT8U)(fCpuBalance / 0x1000000 );
//    data[5] = ~(INT8U)(fCpuBalance % 0x1000000 / 0x10000 );
//    data[6] = ~(INT8U)(fCpuBalance % 0x10000 / 0x100 );
//    data[7] = ~(INT8U)(fCpuBalance % 0x100 );
//
//    data[8] = (INT8U)(fCpuBalance / 0x1000000);
//    data[9] = (INT8U)(fCpuBalance % 0x1000000 / 0x10000);
//    data[10]  = (INT8U)(fCpuBalance % 0x10000 / 0x100);
//    data[11]  = (INT8U)(fCpuBalance % 0x100);
//
//    memcpy(data+12, "\x18\xE7\x18\xE7", 4);
//    for ( cnt = 0; cnt < 8; cnt ++ )              //如果计算不成功，则最多循环计算8次
//    {
//        ////////////////////////////////////////
//        //更改钱包
//        ////////////////////////////////////////
//        memcpy(temp_snd_buf, WRITE_16_FILE, 5);
//
//        memcpy(&temp_snd_buf[5], data, 16);
//        k = CmdPsam(hCpuHandle, temp_snd_buf, 21, temp_rcv_buf, &rcv_len);      //TAC计算初始化
//        if ( k != ok )
//        {
//            continue;
//        }
//        else
//        {
//            DevStat.oper_card_amt -= trade_amt;
//            DevStat.binhand.credit_amt += trade_amt;
//            DevStat.binhand.credit_amt_cnt ++;
//            WriteParam();
//            return ok;
//        }
//    }
//
//    if(cnt == 8)
//    {
//    //  ErrCardInfo.error_occured=TRUE ;
//        EA_vCls();
//        EA_vDisplay(1,"操作员卡操作失败");
//        EA_vDisplay(2,"操作无法继续进行");
//        EA_vDisplay(3,"按下任意键退出");
//        EA_uiInkey(5);
//        return notok;
//    }
//    else
//        return ok;
//
//}
//
///*****************************************************************
//函数原型：CpuIncrement
//功能描述：本函数为操作员卡扣款
//参数描述：无
//参数名称：	输入tFlag---交易类型:0---扣款交易，1:充值交易
/////输出？	类型		描述
//-----------		-----------	------	   	-------
//返 回 值：	-1:无卡
//              -2:选择目录错误
//              -3:口令验证错误
//              -4:
//
//*****************************************************************/
//int CpuIncrement(INT8U * data, INT32U trade_amt)
//{
//    INT32U      fCpuBalance;
////	unsigned long  lCpuBalance;
//    INT8U       cnt=0;
//    INT8U       temp_rcv_buf[50], temp_snd_buf[50];
//    uchar      k=0;
//    uint        LOCAL_V_TYPE  rcv_len = 0;
//
//    if(trade_amt == 0)
//        return ok;
//
////  fCpuBalance = ( ((INT32U)data[0]<<24) + ((INT32U)data[1]<<16) + ((INT32U)data[2]<<8) + (INT32U)data[3] );
//    fCpuBalance = (((INT32U)data[0] * 0x1000000 ) + ((INT32U)data[1] * 0x10000 ) + ((INT32U)data[2] * 0x100 ) + (INT32U)data[3] ) ;
//
//    if( ErrCardInfo.error_occured==TRUE || ErrCardInfo.sale_error_occured==TRUE)
//    {
//        if(fCpuBalance == ErrCardInfo.init_cpu_card_balance)//没有扣款
//        {
//            //lcddisperr("没有扣款");
//        }
//        else
//        {
////			DevStat.oper_card_amt -= trade_amt;
////			DevStat.binhand.credit_amt += trade_amt;
////			DevStat.binhand.credit_amt_cnt ++;
////  		WriteParam();
//            return ok;
//        }
//    }
//
//    ErrCardInfo.init_cpu_card_balance = fCpuBalance;
//
//    fCpuBalance += trade_amt;
//
//    data[0] = (INT8U)(fCpuBalance / 0x1000000 );
//    data[1] = (INT8U)(fCpuBalance % 0x1000000 / 0x10000 );
//    data[2] = (INT8U)(fCpuBalance % 0x10000 / 0x100 );
//    data[3] = (INT8U)(fCpuBalance % 0x100 );
//
//    data[4] = ~(INT8U)(fCpuBalance / 0x1000000 );
//    data[5] = ~(INT8U)(fCpuBalance % 0x1000000 / 0x10000);
//    data[6] = ~(INT8U)(fCpuBalance % 0x10000 / 0x100 );
//    data[7] = ~(INT8U)(fCpuBalance % 0x100 );
//
//    data[8] = (INT8U)(fCpuBalance / 0x1000000 );
//    data[9] = (INT8U)(fCpuBalance % 0x1000000 / 0x10000 );
//    data[10] = (INT8U)(fCpuBalance % 0x10000 / 0x100 );
//    data[11] = (INT8U)(fCpuBalance % 0x100 );
//
//    memcpy(data+12, "\x18\xE7\x18\xE7", 4);
//    for ( cnt = 0; cnt < 8; cnt ++ )              //如果计算不成功，则最多循环计算8次
//    {
//        ////////////////////////////////////////
//        //更改钱包
//        ////////////////////////////////////////
//        memcpy(temp_snd_buf, WRITE_16_FILE, 5);
//
//        memcpy(&temp_snd_buf[5], data, 16);
//        k = CmdPsam(hCpuHandle, temp_snd_buf, 21, temp_rcv_buf, &rcv_len);      //TAC计算初始化
//        if ( k != ok )
//        {
//            //lcddisperr("k == %02x ", k);
//            continue;
//        }
//        else
//        {
//
//            DevStat.oper_card_amt += trade_amt;
//            DevStat.binhand.flush_amt_cnt++;
//            DevStat.binhand.flush_amt += trade_amt;
//            WriteParam();
//            return ok;
//        }
//    }
//
//    if(cnt == 8)
//    {
//        //ErrCardInfo.error_occured = TRUE;
//        EA_vCls();
//        EA_vDisplay(1,"操作员卡操作失败");
//        EA_vDisplay(2,"操作无法继续进行败");
//        EA_vDisplay(3,"按下任意键退出");
//        EA_uiInkey(5);
//        return notok;
//    }
//    else
//        return ok;
//}
//
//
/*****************************************************************
函数原型：OperTradeProcess
功能描述：本函数为操作员卡交易函数
参数描述：无
参数名称：	输入tFlag---交易类型:0---扣款交易，1:充值交易
///输出？	类型		描述
-----------		-----------	------	   	-------
返 回 值： 	  -1:无卡
			  -2:选择目录错误
			  -3:口令验证错误
			  -4:

*****************************************************************/
int OperTradeProcess(int tFlag, INT32U trade_amt)
{
	INT8U rcv_buf[100];
	uint rcv_length;
	INT16U  k;
	int     ret = 0;

	//cpu卡复位
	ret = Cpucard_power_on(rcv_buf, &rcv_length);
	if( ret != ok )
	{
		Cpucard_power_off();
		lcddisperr("CPU卡复位失败");
		return -1;//无卡
	}
		
	memset(rcv_buf, 0x00, sizeof(rcv_buf));
	rcv_length = 0;
	
	//选择1002目录
	k = CmdPsam(hCpuHandle, (void *)SELECT_USER_ADF2, sizeof(SELECT_USER_ADF2) ,rcv_buf, &rcv_length);
	if ( k != ok )
	{
		EA_vCls();
		EA_vDisplay(1,"选择目录错误");
		EA_uiInkeyMs(2000);
		return -2;
	}

	//验证口令
	k = CmdPsam(hCpuHandle, (void *)VERIFY_PIN, sizeof(VERIFY_PIN) ,rcv_buf, &rcv_length);
	if ( k != ok )
	{
		EA_vCls();
		EA_vDisplay(1,"验证口令错误");
		EA_vDisplay(2,"非本系统卡片");
		EA_vDisplay(3,"按下任意键退出");
		EA_uiInkeyMs(2000);
		return -3;
	}

	//读取15文件内容－－判断是什么卡片类型
	k = CmdPsam(hCpuHandle, (void *)READ_15_FILE, sizeof(READ_15_FILE) ,rcv_buf, &rcv_length);
	if ( k != ok )
	{
		EA_vCls();
		EA_vDisplay(1,"读取信息文件错误");
		EA_vDisplay(2,"按下任意键退出");
		EA_uiInkeyMs(2000);
		return -3;
	}

	//判断是否是同一张操作员卡
	if(memcmp(&DevStat.cur_driver_number[1], rcv_buf+8, 3) != 0)//不是同一个操作员卡
		return -6;		

	//读取16文件内容－－读取卡内金额
	//
	k = CmdPsam(hCpuHandle, (void *)READ_16_FILE, sizeof(READ_16_FILE) ,rcv_buf, &rcv_length);
	if ( k != ok )
	{
		EA_vCls();
		EA_vDisplay(1,"读取钱包文件错误");
		EA_vDisplay(2,"按下任意键退出");
		EA_uiInkeyMs(2000);
		return -3;
	}

	if(Purse_Block_Is_Right(rcv_buf) !=ok)//钱包格式检验
	{
		EA_vCls();
		EA_vDisplay(1,"操作员卡钱包损坏");
		EA_vDisplay(2,"操作无法继续进行");
		EA_vDisplay(3,"请持卡到中心处理");
		EA_vDisplay(4,"按下任意键退出");
		EA_uiInkey(5);
		return -4;
	}

	if(tFlag)
	{
		if(CpuIncrement(rcv_buf, trade_amt) == ok)
			return ok;
	}
	else
	{
		if(CpuDecrement(rcv_buf, trade_amt) == ok)
			return ok;
	}
	
	return notok;

}
/******************************************************************************
 函数名称：Cpucard_power_on
 功能描述：复位CPU卡
 参数描述：
 参数名称：	输入/输出？	类型		描述  
   
 返  回  值：ok(0)-成功
			 notok(0xFF)-失败
				   
 作      者	：刘及华
 日      期：2012-12-12
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
******************************************************************************/
INT8U Cpucard_power_on(INT8U *prcvbuf, uint *prcvLen)
{
	uchar result;
	uchar prot;
	// 给卡上电
	result = EA_ucICPowerUp(hCpuHandle, prcvLen, prcvbuf, &prot);
	if  ((result != EM_SUCCESS) || (*prcvbuf != 0x3b ))
	{
		return notok;
	}
	return ok;
}


/*****************************************************************
函数原型：CpuProcess
功能描述：本函数为CPU卡操作
参数描述：无
参数名称：	输入/输出？	类型		描述
-----------		-----------	------	   	-------
返 回 值： 		0:做日结
				1:表示成功		
			   -1:无卡
			  -2:选择目录错误
			  -3:口令验证错误
    		  -4:卡片类型不符合
    		  -5:卡片已经停用
    		  -6:什么都不做，直接退出

*****************************************************************/
int CpuProcess()
{
//    INT8U  rcv_buf[20];
//    uint   rcv_length = 0;
//    INT8U  k;
//    INT16U  key;
//    INT8S  ret = 0;
//
//    //cpu卡复位
//    memset(rcv_buf,0,sizeof(rcv_buf));
//    ret = Cpucard_power_on( rcv_buf, &rcv_length );
//    if( ret != ok)
//    {
//        lcddisperr("        无卡        ");
//        EA_vDisplay(4, "   按下任意键退出   ");
//        (void)EA_uiInkey(0);
//        return -1;
//    }
//    //选择1002目录 ,读CPU卡文件
//    rcv_length = 0;
//    k = CmdPsam(hCpuHandle, (void *)SELECT_USER_ADF2, sizeof(SELECT_USER_ADF2) ,rcv_buf, &rcv_length);
//    if ( k != ok )
//    {
//        lcddisperr("选择目录错误");
////  	EA_vDisplay(3,"按下任意键退出");
////  	(void)EA_uiInkey(0);
//        return -2;
//    }
//    //验证口令
//    k = CmdPsam(hCpuHandle, (void *)VERIFY_PIN, sizeof(VERIFY_PIN) ,rcv_buf, &rcv_length);
//    if ( k != ok )
//    {
//        EA_vCls();
//        EA_vDisplay(1,"验证口令错误");
//        EA_vDisplay(2,"非本系统卡片");
//        EA_vDisplay(3,"按下任意键退出");
//        (void)EA_uiInkey(0);
//        return -3;
//    }
//    //读取15文件内容－－判断是什么卡片类型
//    k = CmdPsam(hCpuHandle, (void *)READ_15_FILE, sizeof(READ_15_FILE) ,rcv_buf, &rcv_length);
//    if ( k != ok )
//    {
//        EA_vCls();
//        EA_vDisplay(1,"读取信息文件错误");
//        EA_vDisplay(2,"按下任意键退出");
//        (void)EA_uiInkey(0);
//        return -3;
//    }
//    //add gather_type card --to gather data when oper_type card damage
//    if(rcv_buf[0] == 0x82)
//    {
//        //DataGather();
//        return 0;
//    }
//     //add worker_down card --- permit  oper down
//    if(rcv_buf[0] == 0x83)
//    {
//        EA_vCls();
//        EA_vDisp(4, 1, "   是否做当天日结?  ");
//        key = EA_uiInkey(0);
//        switch(key)
//        {
//           case EM_key_ENTER:
//                if(Gprs_Upload_data() != ok)
//                {
//                    lcddisperr("     签退不成功!    ");
//                }
//                else
//                {
//                    EA_vCls();
//                    EA_vDisplay(1, "   欢迎使用本设备   ");
//                    EA_vDisplay(2, "      签退成功      ");
//                    EA_vDisplay(3, "   按下任意键退出   ");
//                    (void)EA_uiInkey(5);
//                }
//                break;
//            default :
//                break;
//        }
//        return 0;
//    }
//    if((rcv_buf[0] != 0x80) && (rcv_buf[0] != 0x81))//卡类型不符合，退出
//    {
//        (void)EA_vCls();
//        (void)EA_vDisplay(1,"   卡片类型不符合   ");
//        (void)EA_vDisplay(2,"   按下任意键退出   ");
//        (void)EA_vDisplay(3,"   卡类型：%x", rcv_buf[0]);
//        (void)EA_uiInkey(0);
//        return -4;
//    }
//    if( !DevStat.have_been_set )//未刷设置卡         测试用  刘及华
////  if(0)
//    {
//        if((rcv_buf[0] != 0x81) )//卡类型不符合，退出
//        {
//            EA_vCls();
//            EA_vDisplay(1,"    请插入设置卡    ");
//            EA_vDisplay(2,"   按下任意键退出   ");
//            EA_uiInkeyMs(500);
//            return -4;
//        }
//        else
//        {
//            if(CPU_SET_PROCESS(rcv_buf) < 0)
//            {
//                EA_vCls();
//                EA_vDisplay(1,"    卡片已经停用    ");
//                EA_vDisplay(2,"   按下任意键退出   ");
//                EA_uiInkey(10);
//                return -5;
//            }
//            else
//            {
//                EA_vCls();
//                EA_vDisplay(1,"   设置卡设置成功   ");
//                EA_vDisplay(2,"   按下任意键退出   ");
//                EA_uiInkey(10);
//                return 1;
//            }
//
//        }
//    }
//    else//刷过设置卡，判断是否是操作员卡
//    {
//        if(rcv_buf[0] == 0x80)//操作员卡
//        {
//            ret = CPU_OPER_PROCESS(rcv_buf);
//            if( ret == -1 )//非当班操作员
//            {
//                EA_vCls();
//                EA_vDisplay(1,"    非当班操作员    ");
//                EA_vDisplay(2,"   按下任意键退出   ");
//                EA_uiInkey(5);
//                return -5;
//            }
//            else if( ret == -2 )
//            {
//                EA_vCls();
//                EA_vDisplay(1,"   操作员密码错误   ");
//                EA_vDisplay(2,"   卡片已经被锁住   ");
//                EA_vDisplay(3,"   持卡到中心解锁   ");
//                EA_vDisplay(4,"   按下任意键退出   ");
//                EA_uiInkey(5);
//                return -5;
//            }
//            else if( ret == -3 )
//            {
//                EA_vCls();
//                EA_vDisplay(1,"   卡片已过有效期   ");
//                EA_vDisplay(2,"   持卡到中心恢复   ");
//                EA_vDisplay(3,"   按下任意键退出   ");
//                EA_uiInkey(5);
//                return -5;
//            }
//            else if( ret == -4 )
//            {
//                EA_vCls();
//                EA_vDisplay(1,"    卡片已经停用    ");
//                EA_vDisplay(2,"   按下任意键退出   ");
//                EA_uiInkey(10);
//                return -5;
//            }
//            else if( ret == 1 )
//            {
//                EA_vCls();
//                EA_vDisplay(1,"   操作员登陆成功   ");
//                EA_vDisplay(2,"   按下任意键退出   ");
//                EA_uiInkey(5);
//                return 1;
//            }
//            else if( ret == -6 )
//            {
//                return -6;
//            }
//            else
//            {
//                return -5;
//            }
//        }
//        if((rcv_buf[0] == 0x81) )//设置卡
//        {
//            if( CPU_SET_PROCESS(rcv_buf ) < 0)
//            {
//                EA_vCls();
//                EA_vDisplay(1,"    卡片已经停用    ");
//                EA_vDisplay(2,"   按下任意键退出   ");
//                EA_uiInkey(10);
//                return -5;
//            }
//            else
//            {
//                EA_vCls();
//                EA_vDisplay(1,"   设置卡设置成功   ");
//                EA_vDisplay(2,"   按下任意键退出   ");
//                EA_uiInkey(10);
//                return 1;
//            }
//        }
//        return -6;
//    }
	return 1;
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
	char input[22];
	uint i = 0;
	uchar k = 0;
//  EA_vCls();
//  EA_vDisplay(1, "     正在登陆...    ");
//  EA_vDisplay(2, "      请等待...     ");
//  SleepMs(2000);
	
//  if( Open_GPRS_Module() != ok )
//  {
//  	lcddisperr("PPP拨号连接失败");
//  	return notok;
//  }
//  if( GPRS_TEST() != ok )          //调试2012.12.18
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
	EA_vCls();
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

//  if( GPRS_TEST() != ok )          //调试2012.12.18
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
	uchar   ret = 0;
	int     package_num=0,package_index=0, package_left=0;
	INT8U	ch=0;
//查询白名单版本号
	EA_vCls();
	EA_vDisplay(1, "     同步白名单     ");
	EA_vDisplay(2, "版本:%02x%02x%02x%02x%02x%02x%02x", DevStat.black_version[0],
		 DevStat.black_version[1],DevStat.black_version[2],DevStat.black_version[3],
		 DevStat.black_version[4],DevStat.black_version[5],DevStat.black_version[6]);
	EA_vDisplay(3, "白名单数量:%04d", DevStat.black_name_number);
	SleepMs(2000);

	if( Open_GPRS_Module() != ok )
	{
		lcddisperr("PPP拨号连接失败");
		return notok;
	}

	DevStat.black_flag = 0;
	
	memset(sendbuf, 0x00, sizeof(sendbuf));

	sprintf((void *)sendbuf, "@START%02x%02x%02xWHTV022%02x%02x%02x%02x%02x%02x%02x%02x%02x%04d",
		 DevStat.equ_id[0],DevStat.equ_id[1],DevStat.equ_id[2],
		 DevStat.acnt_id[0],DevStat.acnt_id[1],
		 DevStat.black_version[0],DevStat.black_version[1],DevStat.black_version[2],
		 DevStat.black_version[3],DevStat.black_version[4],DevStat.black_version[5],
		 DevStat.black_version[6],DevStat.black_name_number);
	
	ret = GPRS_SEND_Receive(sendbuf,41);

	if( ret != ok )
	{
		EA_vCls();
		EA_vDisplay(1, "    GPRS数据传输    ");
		EA_vDisplay(2, "  网络故障通讯失败  ");	
		EA_vDisplay(3, "  网络正常后再上传  ");
		SleepMs(2000);
		GPRS_Close();
		return notok;
	}

//需要更新白名单
	if( DevStat.black_flag == 1 )
	{
		//清空白名单列表
		EA_ucBlkClean ((char *)blackFileName);
		DevStat.current_record_index = 0;
		sprintf((void *)str, "共有%04d条白名单", DevStat.black_name_number);
		EA_vDisplay(2, (void *)str);	
		SleepMs(2000);

		package_num = DevStat.black_name_number  / 90;
		package_left = DevStat.black_name_number % 90;

		//申请白名单
		for(package_index=0;package_index<package_num;package_index++)
		{
			memset(sendbuf,0x00,sizeof(sendbuf));
			ch = (int)((package_index*100)/(package_num+1));
			sprintf((void *)str, "下载进度%03d%%", ch);
			EA_vDisplay(3, (void *)str);
			//SleepMs(500);
			sprintf((void *)sendbuf, "@START%02x%02x%02xWHTD012%02x%02x%04d0090", 
					DevStat.equ_id[0], DevStat.equ_id[1], DevStat.equ_id[2],
					DevStat.acnt_id[0], DevStat.acnt_id[1], package_index * 90);
			ret = GPRS_SEND_Receive((void *)sendbuf,31);
			if( ret != ok )
			{
				EA_vCls();
				EA_vDisplay(4, "白名单下载失败!");
				memset(DevStat.black_version, 0, 7);
				DevStat.black_name_number = 0;
				EA_ucBlkClean ((char *)blackFileName);
				DevStat.current_record_index = 0;
				return notok;
			}
		}

		if( ret == ok )
		{
			if( package_left>0 )
			{
				memset(sendbuf,0x00,sizeof(sendbuf));
				sprintf((void *)sendbuf, "@START000000WHTD012%02x%02x%04d%04d", DevStat.acnt_id[0],DevStat.acnt_id[1],
					(package_num*80), package_left);
				ret = GPRS_SEND_Receive(sendbuf,31);
				if( ret == ok )
				{
					EA_vDisplay(3, "下载完成");			
				}
				else
				{
					EA_vDisplay(4, (void *)"白名单下载失败!");
					memset(DevStat.black_version,0,7);
					DevStat.black_name_number = 0;
//  				DB_erase_filesys(DB_BLACK);
					EA_ucBlkClean((char *)blackFileName);
					DevStat.current_record_index = 0;
					return notok;
				}
			}
			else
			{
				EA_vDisplay(3, "下载完成");
			}
			DevStat.driver_is_working = TRUE;
			WriteParam();
			SleepMs(1000);
		}
		if( ret != ok )
		{
			EA_vCls();
			EA_vDisplay(1, "GPRS数据传输");
			EA_vDisplay(2, "网络故障通讯失败");	
			EA_vDisplay(3, "网络正常后再使用");
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

	return ok;
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
	INT8U   strbuf[40];
//  INT8U   iflag = 0;
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
			return ;
		}
		else
		{
			Get_Time(&ltime); 

			memcpy(&DevStat.binhand.work_datetime, &ltime, sizeof(BUS_TIME));
			memcpy(DevStat.binhand.acnt_id, DevStat.acnt_id, 2);
			memcpy(DevStat.binhand.equ_id, DevStat.equ_id, 3);
			memcpy(&DevStat.binhand.oper_id[1], DevStat.cur_driver_number, 2);
			DevStat.binhand.consume_cnt = 0;
			DevStat.binhand.consume_amt = 0;
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
//			EA_vCls();
			EA_vDisplay(1, "请扫加油卡...");
			EA_vDisplay(4, "签退            返回");
			sprintf((void *)strbuf, "本班加油:%3d次", DevStat.binhand.consume_cnt);		
			EA_vDisplay(2, (void *)strbuf);
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
						EA_vDisp(4, 1, "F1-确定    其他-取消");	
						uikey = EA_uiInkey(0);
						switch(uikey)
						{
						   case EM_key_F1:
							    if( Logout() == ok )
								{
									EA_vCls();
									EA_vDisplay(1, "   欢迎使用本设备   ");
									EA_vDisplay(2, "      签退成功      ");
									EA_vDisplay(3, "   按下任意键退出   ");
									(void)EA_uiInkey(5);
									return;
								}
								else
								{
									EA_vCls();
									EA_vDisplay(1, "     签退不成功     ");
									EA_vDisplay(2, "     请重新签退     ");
									EA_vDisplay(3, "   按下任意键退出   ");
									(void)EA_uiInkey(5);
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
/*****************************************************************
函数原型：FuncMenu
功能描述：本函数为功能菜单显示
参数描述：无
参数名称：	输入/输出？	类型		描述
-----------		-----------	------	   	-------
返 回 值：无
*****************************************************************/
void  FuncMenu()
{
	int             ret;
	long            lSubKeyRet=255;
	char            mainflag=0;

	for( ;; )//主菜单循环
	{
		lSubKeyRet = browse_menu(0, (unsigned char*)"" , main_menu, TRUE );
		switch ( lSubKeyRet )
		{
//			case 0://功能设置
//				ret = Func_Setting();
//				if ( ret == -1 )
//				{
//					//Halt_EH0218(POWER_DELAY);			//	关机
//					Halt_HD6305 ();//按 电源键则休眠手持机
					
//				}
//				break;

			case 0://参数设置
				ret = Parm_Setting();
				if ( ret == -1 )
				{
					//Halt_EH0218(POWER_DELAY);			//	关机
//  				Halt_HD6305 ();
				}
				break;

			case 1://信息查询
				ret = Information_Query();
				if ( ret == -1 )
				{
					//Halt_EH0218(POWER_DELAY);			//	关机
//  				Halt_HD6305 ();
					
				}
				break;

			case 2://通讯
				//ret = DataTransport();
				ret = 1;
				if ( ret == -1 )
				{
					//Halt_EH0218(POWER_DELAY);			//	关机
					//Halt_HD6305 ();
					
				}
				break;
			case 3://操作员密码修改
				ret = OperPasswordModify();        
				if ( ret !=ok)
				{
					//Halt_EH0218(POWER_DELAY);			//	关机
					printf_debug((void *)"密码更新失败");
					
				}
				break;
			case -1://KEY_POWER,AUTO_QUIT
				return;
			case -5:
				mainflag = 1;
				break;

			default:
				break;
		}
		if ( mainflag )
		{
			break;
		}
	}
}


/******************************************************************************
 函数名称：SetCardUseDT
 功能描述：设置卡片有效期
 参数描述：
 参数名称：输入/输出？	类型		描述
 
 返  回  值：    ok(0)-成功
				 notok(1)-失败
				 				   
 作      者	：于福勇
 日      期：2007-01-30
 修改历史：
		日期		修改人		修改描述
******************************************************************************/
INT8U SetCardUseDT()

{

//	INT8U  temp_buf[20];
	BUS_TIME ltimer;
//	typ_RTC_date_rec rdr;
	
//	EA_ucClrKeyBuf();			
//	init_console(CONSOLE_CHINESE);
	EA_ucClrKeyBuf();		
	EA_vCls();

	EA_vDisp(1,1, "卡片有效期设置");
	Get_Time(&ltimer);
	if (Get_Time(&ltimer) == ok)
	{
//		temp_buf[4] = temp_buf[7] = 0;
//		rdr.fields.year = atol(temp_buf);
//		rdr.fields.month = atol(temp_buf+5);
//		rdr.fields.day = atol(temp_buf+8);
		CardInfo.card_useful_dt[0] = ltimer.century;
		CardInfo.card_useful_dt[1] = ltimer.year;
		CardInfo.card_useful_dt[2] = ltimer.month;
		CardInfo.card_useful_dt[3] = ltimer.day;

//		init_console(CONSOLE_CHINESE);	
//		KEY_init(KEY_BEEP_KEY|KEY_AUTO_EL_ON|KEY_FIFO_MODE);//开启按键声音和背光
//		(void)sm_setting(MSK_KEY_UP|MSK_KEY_DOWN); //禁止键按下或抬起来时做响应
//		KEY_beep_mask_set(0xFFFfffff);
//		KEY_EL_mask_set(0xFFFfffff);
//		EA_ucClrKeyBuf();		
		return ok;
	}	
	else
	{
//		init_console(CONSOLE_CHINESE);	
//		KEY_init(KEY_BEEP_KEY|KEY_AUTO_EL_ON|KEY_FIFO_MODE);//开启按键声音和背光
//		(void)sm_setting(MSK_KEY_UP|MSK_KEY_DOWN); //禁止键按下或抬起来时做响应
//		KEY_beep_mask_set(0xFFFfffff);
//		KEY_EL_mask_set(0xFFFfffff);
		EA_ucClrKeyBuf();

		return notok;
	}
	
	return ok;
}



/******************************************************************************
 函数名称：GetTradeAmt
 功能描述：获得交易金额
 参数描述：
 参数名称：输入/输出？  类型        描述

 返  回  值：    ok(0)-成功
				 notok(1)-失败

 作      者 ：于福勇
 日      期：2007-01-30
 修改历史：
		日期        修改人      修改描述
******************************************************************************/
INT8U GetTradeAmt(void)
{
	INT8U flag = FALSE;
	FP32   temp_int32u = 0;
	INT8U input[50];
	INT8U buf[50];
	uchar i;
////////////////////////////////////////
//输入充值金额
////////////////////////////////////////

	EA_vCls();

	flag = FALSE;
	EA_ucSetInverse(EM_lcd_INVOFF);
	lcddisp(1, 1, (void *)"输入交易金额单位(元/升)");
	lcddisp(2, 1, (void *)"金额:       元      ");
	strcpy((void *)input, "");

	for ( ;; )
	{
		i = get_input_str(2, 6, 1, 7, (void *)input);
		if ( i == EM_SUCCESS )
			break;
		if ( i == EM_ABOLISH )
			return notok;
	}
	sscanf((void *)input, "%f", &temp_int32u);
	sprintf((void *)buf, "金额:%4.2f  元", temp_int32u);
	lcddisp(2, 1, (void *)buf);
	EA_ucSetInverse(EM_lcd_INVOFF);

	if(temp_int32u > 0)
		temp_int32u *=100 ;
	else
		temp_int32u = 0;

	if(temp_int32u > 10000)//credit amt must >30 and <max_price_limit
	{
//  	Beep(500);
		lcddisperr("交易定额无效");
		Beeperr();
		return notok;
	}
	else
	{
		CardInfo.fare = temp_int32u;
		lcddisperr("交易定额输入完成");
	}

	return ok;
}


/******************************************************************************
 函数名称：GetConsumeTradeAmt
 功能描述：获得交易金额
 参数描述：
 参数名称：输入/输出？	类型		描述
 返  回  值：    ok(0)-成功
				 notok(1)-失败				 				   
 作      者	：于福勇
 日      期：2007-01-30
 修改历史：
		日期		修改人		修改描述
******************************************************************************/
INT8U GetConsumeTradeAmt(void)
{
	INT8U   flag = 0;
	INT8U   input[50],buf[50];
	FP32    LOCAL_V_TYPE  temp_int32u = 0;
	uchar 	i;
////////////////////////////////////////
//输入充值金额
////////////////////////////////////////
	
//	init_console(CONSOLE_CHINESE);				

	EA_vCls();
	

//  EA_vDisp(1, 1, (unsigned char*)"请输入交易金额");
//  EA_vDisp(2, 1, "单位( 元 )");
	input[0] = '\0';
	EA_ucSetInverse(EM_lcd_INVON);
	lcddisp(1, 1, (void *)"请输入交易金额      ");
	strcpy((void *)input, "金额:               ");
	for ( ;; )
	{
		i = get_input_str(2, 7, 1, 7, (void *)input);
		if ( i == EM_SUCCESS )
			break;
		if ( i == EM_ABOLISH )
			return notok;
	}
	sscanf((void *)input, "%f", &temp_int32u);
	sprintf((void *)buf, "金额: %4.2f元", temp_int32u);
	lcddisp(2, 1, (void *)buf);
	EA_ucSetInverse(EM_lcd_INVOFF);

//  temp_int32u = get_numeric(4,4,4,0);
//	if(temp_int32u > 0)
//  	temp_int32u *=100 ;
//		;
//	else
//		temp_int32u = 0;
	
	while (!flag)
	{
		if( temp_int32u==0 )//credit amt must >30 and <max_price_limit
		{
			//EA_vCls();
			//EA_vCls();
//  		Beep(500);
			EA_vDisp(3, 1, "交易金额输入无效");  
			Beeperr();
		}
		else
		{
		//	EA_vCls();
		//	EA_ucClrKeyBuf();				
			flag = 1;
			CardInfo.fare = temp_int32u;
			break;
		//	sprintf(strbuf, "金额:%6.2f元", CardInfo.fare/100);
		//	if(select_affirm(4, strbuf)==0)
		//		break;
				
		}
		
//判断有无退出键按下
	/*	ret = KEY_read();
		if(ret ==EXIT_KEY_COMM)
		{
			Beep(2000);
			EA_uiInkeyMs(200);
			init_console(CONSOLE_CHINESE);	
			KEY_init(KEY_BEEP_KEY|KEY_AUTO_EL_ON|KEY_FIFO_MODE);//开启按键声音和背光
			(void)sm_setting(MSK_KEY_UP|MSK_KEY_DOWN); //禁止键按下或抬起来时做响应
			KEY_beep_mask_set(0xFFFfffff);
			KEY_EL_mask_set(0xFFFfffff);
			EA_ucClrKeyBuf();	
			return notok;	
		}
		if(ret == EXIT_KEY_POWER)
		{
			Halt_HD6305();
		}
//		else
//		{
//			flag = FALSE;
//		}
*/
//重新输入		
//		EA_vCls();
//  	EA_vDelay(1, 1, "                   ");
//  	EA_vDelay(2, 1, "请输入交易金额     ");
		temp_int32u = 0;
//  	temp_int32u = get_numeric(4,4,4,0);
		strcpy((void *)input,"");
		EA_ucSetInverse(EM_lcd_INVON);
		lcddisp(1, 1, (void *)"请输入交易金额      ");
		strcpy((void *)input, "金额:               ");
		for ( ;; )
		{
			i = get_input_str(2, 7, 1, 7, (void *)input);
			if ( i == EM_SUCCESS )
				break;
			if ( i == EM_ABOLISH )
				return notok;
		}
		sscanf((void *)input, "%f", &temp_int32u);
		sprintf((void *)buf, "金额: %4.2f元", temp_int32u);
		lcddisp(2, 1, (void *)buf);
		EA_ucSetInverse(EM_lcd_INVOFF);
//		if(temp_int32u > 0)
//  		temp_int32u *=100 ;
//			;
//		else
//			temp_int32u = 0;
	}
//  init_console(CONSOLE_CHINESE);
//  KEY_init(KEY_BEEP_KEY|KEY_AUTO_EL_ON|KEY_FIFO_MODE);//开启按键声音和背光
//  (void)sm_setting(MSK_KEY_UP|MSK_KEY_DOWN); //禁止键按下或抬起来时做响应
//  KEY_beep_mask_set(0xFFFfffff);
//  KEY_EL_mask_set(0xFFFfffff);
    EA_ucClrKeyBuf();
	

//判断输入金额是否合法
	if(flag == 0)//没有输入充值金额
	{
		Beeperr();
		return notok;	
	}

	EA_vCls();
	EA_vDisp(1,1,"      卡片交易      ");
	EA_vDisp(2,1,"  请放好卡片别移动  ");
	EA_vDisp(3,1,"   处理中，请等待   ");
	return ok;

}


/******************************************************************************
 函数名称：GetTradeInfo
 功能描述：获得持卡人交易信息
 参数描述：
 参数名称：输入/输出？	类型		描述
 
 返  回  值：    ok(0)-成功
				 notok(1)-失败
				 				   
 作      者	：于福勇
 日      期：2007-01-30
 修改历史：
		日期		修改人		修改描述
******************************************************************************/
INT8U GetTradeInfo(void)
{
//	INT32U  LOCAL_V_TYPE  temp_int32u = 0;
//	INT8U   LOCAL_V_TYPE  temp_buf[50];
//	INT8U   			  strbuf[50];
//	int   			  	  key;
////////////////////////////////////////
//输入充值金额
////////////////////////////////////////
	
	if( GetTradeAmt() != ok )
		return notok;
/*	
////////////////////////////////////////
//输入持卡人证件信息
////////////////////////////////////////
	if(GetPersonInfo()!=ok)
	{
		(void)EA_vCls();
		DispStr_CE(0, 2, "人员信息输入错误", DISP_POSITION);     
		DispStr_CE(0, 6, "售卡失败!", DISP_CENTER);   
		Beep(1000);
		return notok;	
	}
	
	EA_vCls();
	init_console(CONSOLE_CHINESE);	
	KEY_init(KEY_BEEP_KEY|KEY_AUTO_EL_ON|KEY_FIFO_MODE);//开启按键声音和背光
	(void)sm_setting(MSK_KEY_UP|MSK_KEY_DOWN); //禁止键按下或抬起来时做响应
	KEY_beep_mask_set(0xFFFfffff);
	KEY_EL_mask_set(0xFFFfffff);
	EA_ucClrKeyBuf();	
*/	
//  EA_vDisp(1,1,"卡片售卡");
//  EA_vDisp(2,1,"请放好卡片别移动");
//  EA_vDisp(3,1,"处理中,请等待...");

	return ok;

}

///******************************************************************************
// 函数名称：GetPersonInfo
// 功能描述：获得持卡人信息
// 参数描述：
// 参数名称：输入/输出？  类型        描述
//
// 返  回  值：    ok(0)-成功
//                 notok(0xFF)-失败
//
// 作      者 ：于福勇
// 日      期：2007-01-30
// 修改历史：
//        日期        修改人      修改描述
//******************************************************************************/
//INT8U GetPersonInfo(void)
//{
//    unsigned char *fp_data,SendData[100]={0};
////	INT8U	Lflag=0;//登陆成功与否的标志
//    int     ret;
//    INT8U   flag=0;
//
//    EA_vCls();
//
//    for(;;)
//    {
//        ret = browse_menu(0, (unsigned char *)"持卡人信息", PersonInfo,TRUE );
//        switch(ret)
//        {
//            case 0://输入姓名
//            case EM_key_1:
//            {
//                EA_uiInkeyMs(200);
//                EA_vCls();
//            //  EA_ucClrKeyBuf();
//            //  init_console(CONSOLE_CHINESE);
//                EA_ucClrKeyBuf();
//                memset(SendData, 0, sizeof(SendData));
//                fp_data = SendData;
//                if (1)   //gets_dialog_ch((char *)fp_data, "输入姓名", GETS_HINT_INVERSED) == GETS_DIALG_CONFIRM
//                {
//                    //strcat(fp_data, "\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20");
//                    if(strlen((char *)fp_data) == 0)
//                    {
//                        printf_debug((void *)"姓名必须输入");
//                    }
//                    else
//                    {
//                        //sprintf(strbuf, "1:%s", fp_data);
//                        //printf_debug(strbuf);
//                        EA_uiInkeyMs(10);
//                        strcat((void *)fp_data, "\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20");
//                        //memcpy(strbuf, fp_data, 8);
//                        //strbuf[8] = 0;
//                        //sprintf(strbuf, "%s", fp_data);
//                        //printf_debug(strbuf);
//                        memcpy(CardInfo.name, fp_data, 8);
//                        //memcpy(strbuf, CardInfo.name, 8);
//                        //strbuf[8] = 0;
//                        //printf_debug(strbuf);
//                        flag = 1;
//                    }
//                }
//                else
//                    flag=0;
//
//                EA_uiInkeyMs(200);
////  			init_console(CONSOLE_CHINESE);
////  			KEY_init(KEY_BEEP_KEY|KEY_AUTO_EL_ON|KEY_FIFO_MODE);//开启按键声音和背光
////  			(void)sm_setting(MSK_KEY_UP|MSK_KEY_DOWN); //禁止键按下或抬起来时做响应
////  			KEY_beep_mask_set(0xFFFfffff);
////  			KEY_EL_mask_set(0xFFFfffff);
//                EA_ucClrKeyBuf();
//                *fp_data = 0;
//                break;
//            }
//            case 1://输入性别
//            case EM_key_2:
//            {
////  			Sex_Select();
//                break;
//            }
//            case 2://输入证件类型
//            case EM_key_3:
//            {
//                Id_Select();
//                break;
//            }
//            case 3://输入证件号码
//            case EM_key_4:
//            {
//                EA_ucClrKeyBuf();
//                EA_uiInkeyMs(200);
////  			init_console(CONSOLE_CHINESE);
//                EA_ucClrKeyBuf();
//                memset(SendData, 0, sizeof(SendData));
//                fp_data=SendData;
//                if (1)    //gets_dialog_ch((char *)fp_data, "输入证件号码", GETS_HINT_INVERSED) == GETS_DIALG_CONFIRM
//                {
//                    if(strlen((char *)fp_data) == 0)
//                    {
//                        printf_debug((char *)"证件号码必须输入");
//                    }
//                    else
//                    {
//                        EA_uiInkeyMs(10);
//                        strcat((char *)fp_data, "\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20");
//                        //memcpy(strbuf, fp_data, 18);
//                        //strbuf[18] = 0;
//                    //  printf_debug(strbuf);
//                        memcpy(CardInfo.id_no, fp_data, 18);
//                        flag += 2;
//                    }
//                }
//
//
//                EA_uiInkeyMs(200);
////  			init_console(CONSOLE_CHINESE);
//
////  			KEY_init(KEY_BEEP_KEY|KEY_AUTO_EL_ON|KEY_FIFO_MODE);//开启按键声音和背光
////  			(void)sm_setting(MSK_KEY_UP|MSK_KEY_DOWN); //禁止键按下或抬起来时做响应
////  			KEY_beep_mask_set(0xFFFfffff);
////  			KEY_EL_mask_set(0xFFFfffff);
//                EA_ucClrKeyBuf();
//                *fp_data = 0;
//                break;
//            }
//            case 4://是否保存参数
//            case EM_key_5:
//            {
//                if(!(flag & 0x01))
//                {
//                    printf_debug((void *)"姓名必须输入");
//                    continue;
//                }
//                if(!(flag & 0x02))
//                {
//                    printf_debug((void *)"证件号码必须输入");
//                    continue;
//                }
//
////  			EA_ucClrKeyBuf();
////  			EA_uiInkeyMs(200);
////  			init_console(CONSOLE_CHINESE);
////  			EA_ucClrKeyBuf();
////  			ret = select_affirm(4,"是否保存参数?");
//                EA_uiInkeyMs(200);
////  			init_console(CONSOLE_CHINESE);
//
////  			KEY_init(KEY_BEEP_KEY|KEY_AUTO_EL_ON|KEY_FIFO_MODE);//开启按键声音和背光
////  			(void)sm_setting(MSK_KEY_UP|MSK_KEY_DOWN); //禁止键按下或抬起来时做响应
////  			KEY_beep_mask_set(0xFFFfffff);
////  			KEY_EL_mask_set(0xFFFfffff);
//                EA_ucClrKeyBuf();
//                if(ret==0)
//                    return ok;
//                break;
//            }
//            case -5:
//            {
//                //EA_uiInkeyMs(200);
////  			init_console(CONSOLE_CHINESE);
////
////  			KEY_init(KEY_BEEP_KEY|KEY_AUTO_EL_ON|KEY_FIFO_MODE);//开启按键声音和背光
////  			(void)sm_setting(MSK_KEY_UP|MSK_KEY_DOWN); //禁止键按下或抬起来时做响应
////  			KEY_beep_mask_set(0xFFFfffff);
////  			KEY_EL_mask_set(0xFFFfffff);
//                EA_ucClrKeyBuf();
//
//                return notok;
//                break;
//            }
//            case -1:
//            {
////  			Halt_HD6305();
//                break;
//            }
//            default:
//                break;
//        }
//    }
//
//
////  KEY_init(KEY_BEEP_KEY|KEY_AUTO_EL_ON|KEY_FIFO_MODE);//开启按键声音和背光
////  (void)sm_setting(MSK_KEY_UP|MSK_KEY_DOWN); //禁止键按下或抬起来时做响应
////  KEY_beep_mask_set(0xFFFfffff);
////  KEY_EL_mask_set(0xFFFfffff);
//    EA_ucClrKeyBuf();
//
//    return ok;
//}

/*****************************************************************
函数原型：OperCardCheck
功能描述：本函数为检测是否有操作员卡在，
				//并判断是否为同一张卡
参数描述：无
参数名称：	输入/输出？	类型		描述
-----------		-----------	------	   	-------
返 回 值：	  -1:无卡
			  -2:选择目录错误
			  -3:口令验证错误
			  -4:

*****************************************************************/
int OperCardCheck()
{
//    INT8U rcv_buf[100];
//    uint  rcv_length;
//    INT16U  k;
//    int     ret = 0;
//
//    //cpu卡复位
//    ret = Cpucard_power_on(rcv_buf, &rcv_length);
//    if(ret == notok)
//        return -1;//无卡
//
//    memset(rcv_buf,0,sizeof(rcv_buf));
//    rcv_length = 0;
//
//    //选择1002目录
//    k = CmdPsam(hCpuHandle, (void *)SELECT_USER_ADF2, sizeof(SELECT_USER_ADF2) ,rcv_buf, &rcv_length);
//    if ( k != ok )
//    {
//        EA_vCls();
//        lcddisperr("选择目录错误");
//        return -2;
//    }
//
//    //验证口令
//    k = CmdPsam(hCpuHandle, (void *)VERIFY_PIN, sizeof(VERIFY_PIN) ,rcv_buf, &rcv_length);
//    if ( k != ok )
//    {
//        EA_vCls();
//        EA_vDisplay(1,"选择目录错误");
//        EA_vDisplay(2,"非本系统卡片");
//        EA_vDisplay(3,"按下任意键退出");
//        EA_uiInkeyMs(500);
//        return -3;
//    }
//
//    //读取15文件内容－－判断是什么卡片类型
//    k = CmdPsam(hCpuHandle, (void *)READ_15_FILE, sizeof(READ_15_FILE) ,rcv_buf, &rcv_length);
//    if ( k != ok )
//    {
//        EA_vCls();
//        EA_vDisplay(1,"读取信息文件错误");
//        EA_vDisplay(2,"按下任意键退出");
//        EA_uiInkeyMs(500);
//        return -4;
//    }
//
//    if((rcv_buf[0] != 0x80))//卡类型不符合，退出
//    {
//        EA_vCls();
//        EA_vDisplay(1,"卡片类型不符合");
//        EA_vDisplay(2,"按下任意键退出");
//        EA_uiInkeyMs(500);
//        return -5;
//    }
//    //卡片状态监测
//    if(*(rcv_buf+31) == 0x02)//此卡已经停用
//        return -6;
////操作员工号保存
//    if(memcmp(&DevStat.cur_driver_number[1], rcv_buf+8, 3) != 0)//不是同一个操作员卡
//        return -6;
//
//    //读取16文件内容－－读取卡内金额
//    k = CmdPsam(hCpuHandle, (void *)READ_16_FILE, sizeof(READ_16_FILE) ,rcv_buf, &rcv_length);
//    if ( k != ok )
//    {
//        EA_vCls();
//        EA_vDisplay(1,"读取钱包文件错误");
//        EA_vDisplay(2,"按下任意键退出");
//        EA_uiInkeyMs(500);
//        return -5;
//    }
//    if(Purse_Block_Is_Right(rcv_buf) != ok)//钱包格式检验
//    {
//        EA_vCls();
//        EA_vDisplay(1,"操作员卡钱包损坏");
//        EA_vDisplay(2,"操作无法继续进行");
//        EA_vDisplay(3,"请持卡到中心处理");
//        EA_vDisplay(4,"按下任意键退出");
//        EA_uiInkey(5);
//        return -5;
//    }
//    //((INT32U)rcv_buf[3]<<24) + ((INT32U)rcv_buf[2]<<16) + ((INT32U)rcv_buf[1]<<8) + (INT32U)rcv_buf[0]
//    DevStat.oper_card_amt = (((INT32U)rcv_buf[0] * 0x1000000 + (INT32U)rcv_buf[1] * 0x10000 + (INT32U)rcv_buf[2] * 0x100 + (INT32U)rcv_buf[3]) )  ;
	return ok;
}
/*****************************************************************
函数原型：CPU_OPER_DOWN
功能描述：本函数为CPU操作员卡下班处理
参数描述：无
参数名称：	输入/输出？	类型		描述
-----------		-----------	------	   	-------
返 回 值：   无
*****************************************************************/
void CPU_OPER_DOWN()
{
	DevStat.driver_is_working = FALSE;
	memset(DevStat.cur_driver_number, 0x00, sizeof(DevStat.cur_driver_number));
	memset(&DevStat.binhand, 0x00, sizeof(DevStat.binhand));
	WriteParam();
}


/******************************************************************************
 函数名称：GrayCardDeal
 功能描述：卡片补偿操作
 参数描述：
 参数名称：输入/输出？	类型		描述
 
 返  回  值：    ok(0)-成功
				 notok(0xFF)-失败
				 0xFE-没找到卡
				 0xF0-重新刷卡
				   
 作      者	：于福勇
 日      期：2007-01-30
 修改历史：
		日期		修改人		修改描述
******************************************************************************/
//INT8U GrayCardDeal(void)
//{
//
//
//    INT8U   LOCAL_V_TYPE  i = 0;
//    INT8U   LOCAL_V_TYPE  j = 0;
////	INT32U  LOCAL_V_TYPE  start_using_date = 0;				//启用日期
////	INT32U  LOCAL_V_TYPE  end_using_date = 0;				//有效截至日期
////	INT32U  LOCAL_V_TYPE  cur_date = 0;							//当前时间
//    INT32U  LOCAL_V_TYPE  temp_int32u = 0;
//    INT8S   LOCAL_V_TYPE  h = 0;
//
////	INT16U  LOCAL_V_TYPE  k = 0;
//
////	INT8U   LOCAL_V_TYPE  temp_rcv_buf[50];
////	INT8U   LOCAL_V_TYPE  temp_snd_buf[50];
//    INT8U   LOCAL_V_TYPE  block_buffer1[16] = {0};          //M1卡一个block4的数据缓冲
//    INT8U   LOCAL_V_TYPE  block_buffer2[16] = {0};          //M1卡一个block5的数据缓冲
//    INT8U   LOCAL_V_TYPE  block_buffer3[16] = {0};          //M1卡一个block6的数据缓冲
////	INT8U   			  strbuf[50];
////	int		key;
//    INT8U   LOCAL_V_TYPE    m = 0;
////	INT16U  LOCAL_V_TYPE    temp_int16u;
//    INT8S   LOCAL_V_TYPE    g = 0;
//    INT8U   LOCAL_V_TYPE    block9[16];
//    INT8U   LOCAL_V_TYPE    block10[16];
//
//    INT32U  LOCAL_V_TYPE    temp_balance;
//
//////////////////////////////////////////
////卡片初始化
//////////////////////////////////////////
//    if(CardInit() != ok)
//        return notok;
//
//////////////////////////////////////////
////判断是否是出错卡
//////////////////////////////////////////
//    h = memcmp(&CardInfo.csn[0], &GrayRecord.csn[0], 4);//出错卡硬件卡号比较
//    if ( h != ok )
//    {
////  	(void)EA_vCls();
//        lcddisperr("请刷出错卡");
//        Beep(1000);
//        return notok;//卡交易出错，但回的卡片不是出错卡
//    }
//
//    i = PsamRst();//PSAM卡复位
//    if ( i != ok )
//    {
//        lcddisperr((void *)"PSAM卡故障");
//        j = EA_ucMIFHalt(hMifsHandle);  //挂卡
//        return notok;
//    }
//
//////////////////////////////////////////
////恢复保存的充值密钥
//////////////////////////////////////////
//    for(i=1;i<=6;i++)
//    {
//        memcpy(&KeySector[i][0], &DevStat.KEYVALUE[i][0], 6);
//    }
//
//
//////////////////////////////////////////
////提示界面
//////////////////////////////////////////
//    EA_vCls();
//    EA_vDisp(1,1,"卡片补偿");
//    EA_vDisp(2,1,"请放好卡片别移动");
//    EA_vDisp(3,1,"处理中，请等待");
//
//////////////////////////////////////////
////充值交易
//////////////////////////////////////////
//
//
//////////////////////////////////////////
////验证Sector 6 密钥
//////////////////////////////////////////
//    i = Get_Sector_PermissionB(6, &KeySector[6][0]);
//    if ( i != ok )
//    {
//        return notok;
//    }
//    CardInfo.credit_rec_pos = GrayRecord.block24_buf_to_restore[8];
//
//    if(CardInfo.credit_rec_pos == 0x00 ||CardInfo.credit_rec_pos == 0xFF)
//        CardInfo.credit_rec_pos = 0x0C;
//    else
//        CardInfo.credit_rec_pos ++;
//
//    if(CardInfo.credit_rec_pos==0x0f ||CardInfo.credit_rec_pos==0x13 )
//        CardInfo.credit_rec_pos ++;
//    if(CardInfo.credit_rec_pos>=0x17)
//        CardInfo.credit_rec_pos=0x0C;
//
//////////////////////////////////////////
////write  sector 6 info
//////////////////////////////////////////
//    i = EA_ucMIFWrite(hMifsHandle,24, &GrayRecord.block24_buf_to_restore[0]);
//    if ( i != ok )
//        return notok;
//
//    i = EA_ucMIFWrite(hMifsHandle,25, &GrayRecord.block24_buf_to_restore[0]);
//    if ( i != ok )
//        return notok;
//
////保存设备交易序号
//    memcpy(&CardInfo.dev_trade_no[0],&DevStat.dev_trade_no[0], 2);                                      //PSAM卡交易序号清零
//
//    CardInfo.psam_purchase_serial_num = 0;
//
//
//////////////////////////////////////////
////验证Sector 2 密钥
//////////////////////////////////////////
//    i = Get_Sector_PermissionB(2, &KeySector[2][0]);
//    if ( i != ok )
//    {
//        return notok;
//    }
//
//    i = EA_ucMIFRead(hMifsHandle, 9, block9);
//    if ( i != ok )
//        return notok;
//
//
//    i = EA_ucMIFRead(hMifsHandle, 10, block10);
//    if ( i != ok )
//        return notok;
//
//    i = Purse_Block_Is_Right(block9);
//    j = Purse_Block_Is_Right(block10);
//    if ( (i == ok) && (j == ok) )
//    {   //两区都合法，看两区数据是否一致
//        g = memcmp(block9, block10, 16);
//        if ( g == ok )
//        {
//            ;           //两区一致，什么也不做
//        }
//        else
//        {
//            //两区不一致，以9块为主
//            m = EA_ucMIFRestore(hMifsHandle,9);
//            m = EA_ucMIFTransfer(hMifsHandle,10);
////  		m = CardMFCValueWithTransfer(PICC_INCREMENT, 9, 10,zero);	  //两区不一致，以9块为主
//            if ( m != ok )
//                return notok;
//        }
//    }
//    else if ( i == ok )
//    {
////         m = Write(10, block9);              //9区合法，9->10
//        m = EA_ucMIFRestore(hMifsHandle, 9);
//        m = EA_ucMIFTransfer(hMifsHandle, 10);     //9区合法，9->10
//        if ( m != ok )
//            return notok;
//    }
//    else if ( j == ok )
//    {
////         m = Write(9, block10);          //10区合法，10->9
//        m = EA_ucMIFRestore(hMifsHandle, 10);
//        m = EA_ucMIFTransfer(hMifsHandle, 9);     //10区合法，10->9
//        if ( m != ok )
//            return notok;
//        memcpy(block9, block10, 16);        //好的数据传给block9下边处理数据用block9
//    }
//    else
//    {
//        return notok;       //两区都不合法，退出
//    }
//
//
////////////////////
////trans card balance
//
//    temp_balance = BlockToCash(block9);
//////////////////////////////////////////
////judge card_purchase_type
////0x77---sale; 0x88---credit; 0x99---flush
//////////////////////////////////////////
//    if((GrayRecord.card_purchase_type==TT_GRAY_SALE)||(GrayRecord.card_purchase_type==TT_GRAY_CREDIT))
//    {
//        if((CharToCash(GrayRecord.balance) + CharToCash(GrayRecord.fare))==temp_balance)//increment ok
//        {
//            CardInfo.balance = temp_balance;
////			CardInfo.init_balance = GrayRecord.balance;
//        }
//        else if(CharToCash(GrayRecord.balance) == temp_balance)
//        {
//            if(DevStat.oper_card_amt == ( ((CharToCash(GrayRecord.oper_card_amt)) * 0x100 ) + GrayRecord.oper_card_amt[3] ))
//            {
//                //扣除操作员卡内金额
//                Cpucard_Init();
//                if(OperTradeProcess(0, CharToCash(GrayRecord.fare)) != 0)
//                {
//                    Cpucard_power_off();
//                    return notok;
//                }
//                Cpucard_power_off();
//
//                //CardMFCValueWithTransfer(int flag, int s_block, int d_block, int num);
//
//                i = EA_ucMIFIncrement(hMifsHandle, 9, CharToCash(GrayRecord.fare));
//                i = EA_ucMIFTransfer(hMifsHandle, 9);
//                if ( i != ok )
//                {
//                    return notok;
//                }
//                i = EA_ucMIFRestore(hMifsHandle, 9);
//                i = EA_ucMIFTransfer(hMifsHandle, 10);
//                if ( i != ok )
//                {
//                    return notok;
//                }
//                CardInfo.balance = CharToCash(GrayRecord.balance) + CharToCash(GrayRecord.fare);//储存交易后余额
////				CardInfo.init_balance = GrayRecord.balance;
//            }
//            else if((DevStat.oper_card_amt + CharToCash(GrayRecord.fare)) == (((CharToCash(GrayRecord.oper_card_amt)) * 0x100 )+ GrayRecord.oper_card_amt[3]))
//            {
//                i = EA_ucMIFIncrement(hMifsHandle,9, CharToCash(GrayRecord.fare));
//                i = EA_ucMIFTransfer(hMifsHandle, 9);
////  			i = CardMFCValueWithTransfer(PICC_INCREMENT, 9, 9,CharToCash(GrayRecord.fare));
//                if ( i != ok )
//                {
//                    return notok;
//                }
//                i = EA_ucMIFIncrement(hMifsHandle,9, 0);
//                i = EA_ucMIFTransfer(hMifsHandle, 10);
////  			i = CardMFCValueWithTransfer(PICC_INCREMENT, 9, 10, 0);
//                if ( i != ok )
//                {
//                    return notok;
//                }
//                CardInfo.balance = CharToCash(GrayRecord.balance) + CharToCash(GrayRecord.fare);//储存交易后余额
////				CardInfo.init_balance = GrayRecord.balance;
//            }
//            else
//                return notok;//error
//        }
//        else
//        {
//            return notok;//error
//        }
//    }
//    else//flush
//    {
//        if((CharToCash(GrayRecord.balance) - CharToCash( GrayRecord.fare)) == temp_balance)//increment ok
//        {
//            CardInfo.balance = temp_balance ;//储存交易后余额;
////			CardInfo.init_balance = GrayRecord.balance;
//        }
//        else if(CharToCash(GrayRecord.balance) == temp_balance)
//        {
//            if (  DevStat.oper_card_amt == (( CharToCash(GrayRecord.oper_card_amt) * 0x100 ) + GrayRecord.oper_card_amt[3] ) )
//            {
//                //扣除操作员卡内金额
//                Cpucard_Init();
//                if(OperTradeProcess(1, CharToCash(GrayRecord.fare)) != 0)
//                {
//                    Cpucard_power_off();
////  				MC530Off();
////  				InitMC530(TYPE_A_ISO14443);
//                    return notok;
//                }
//                Cpucard_power_off();
//                i = EA_ucMIFDecrement(hMifsHandle, 9, CharToCash(GrayRecord.fare));
//                i = EA_ucMIFTransfer(hMifsHandle, 9);
//                if ( i != ok )
//                {
//                    return notok;
//                }
//                i = EA_ucMIFDecrement(hMifsHandle, 9, 0);
//                i = EA_ucMIFTransfer(hMifsHandle, 10);
//                if ( i != ok )
//                {
//                    return notok;
//                }
//                CardInfo.balance = CharToCash(GrayRecord.balance) - CharToCash(GrayRecord.fare);//储存交易后余额
//            }
//            else if ((DevStat.oper_card_amt - CharToCash(GrayRecord.fare)) == ((CharToCash(GrayRecord.oper_card_amt) * 0x100 ) + GrayRecord.oper_card_amt[3]))
//            {
//                i = EA_ucMIFDecrement(hMifsHandle, 9, CharToCash(GrayRecord.fare));
//                i = EA_ucMIFTransfer(hMifsHandle, 9);
////  			i = CardMFCValueWithTransfer(PICC_DECREMENT, 9, 9,CharToCash(GrayRecord.fare));
//                if ( i != ok )
//                {
//                    return notok;
//                }
//                i = EA_ucMIFDecrement(hMifsHandle,9, 0);
//                i = EA_ucMIFTransfer(hMifsHandle, 10);
////  			i = CardMFCValueWithTransfer(PICC_DECREMENT, 9, 10, 0);
//                if ( i != ok )
//                {
//                    return notok;
//                }
//                CardInfo.balance = CharToCash(GrayRecord.balance) - CharToCash(GrayRecord.fare);//储存交易后余额
//            }
//            else
//                return notok;//error
//        }
//        else
//        {
//            return notok;//error
//
//        }
//
//    }
//
//////////////////////////////////////////
////验证Sector 3.4.5 密钥改写交易记录
//////////////////////////////////////////
//
//    m = (CardInfo.credit_rec_pos -12 )/4  +3;//扇区号
//    i=Get_Sector_PermissionB(m, &KeySector[m][0]);
//    if ( i != ok )
//        return ok;
//    memcpy(&CardInfo.blockcreditrec[0], &CardInfo.card_in_time.year, 5);//yymmddhhmi
//    CardInfo.blockcreditrec[5] = GrayRecord.balance[0];//卡片金额
//    CardInfo.blockcreditrec[6] = GrayRecord.balance[1];//卡片金额
//    CardInfo.blockcreditrec[7] = GrayRecord.balance[2];//卡片金额
//
//    CardInfo.blockcreditrec[8] = GrayRecord.fare[0];//交易金额
//    CardInfo.blockcreditrec[9] = GrayRecord.fare[1];//交易金额
//    CardInfo.blockcreditrec[10] =  GrayRecord.fare[2];//交易金额
//
//    memcpy(&CardInfo.blockcreditrec[11],&DevStat.equ_id[1],2);//设备号
//    memcpy(&CardInfo.blockcreditrec[13], &DevStat.cur_driver_number[1],3);//操作员号
//
////  i = CardMFCWrite16Bytes(CardInfo.credit_rec_pos, &CardInfo.blockcreditrec[0]);
//    i = EA_ucMIFWrite(hSam1Handle, CardInfo.credit_rec_pos, &CardInfo.blockcreditrec[0]);
//
//////////////////////////////////////////
/////判断灰记录类型
/////card_purchase_type : 0x77--sale;  0x88--credit; 0x99---flush
/////////////////////////////////////////
//
//////write  sector one info
//    if(GrayRecord.card_purchase_type==TT_GRAY_SALE)
//    {
//        //改写1区信息
//        ////////////////////////////////////////
//        //验证Sector 1 密钥
//        ///////////////////////////////////////
//        i = Get_Sector_PermissionB(1, &KeySector[1][0]);
//        if ( i != ok )
//        {
//            return notok;
//        }
//
//        i = EA_ucMIFRead(hMifsHandle, 4, block_buffer1);
//        if ( i != ok )
//            return notok;
//
//        i = EA_ucMIFRead(hMifsHandle, 5, block_buffer2);
//        if ( i != ok )
//            return notok;
//
//        i = EA_ucMIFRead(hMifsHandle, 6, block_buffer3);
//        if ( i != ok )
//            return notok;
//
//        //改写4，5，6块信息
//        block_buffer1[12] = 0x01;//启用标志
//        memcpy(&block_buffer1[14], &DevStat.deposit_amt[0],2);
//
//        memcpy(&block_buffer2[0], &CardInfo.card_begin_dt[0], 4);
//        memcpy(&block_buffer2[4], &CardInfo.card_useful_dt[0],4);
//        memcpy(&block_buffer2[13], &DevStat.equ_id[0], 3);
//
//        memcpy(&block_buffer3[0], &CardInfo.card_add_dt[0], 4);
//        block_buffer3[7] = (CardInfo.fare / 0x10000 );
//        block_buffer3[8] = (CardInfo.fare % 0x10000 / 0x100 );
//        block_buffer3[9] = (CardInfo.fare % 0x100 );
//
//        block_buffer3[10] = block_buffer3[7];//充值金额
//        block_buffer3[11] = block_buffer3[8];
//        block_buffer3[12] = block_buffer3[9];
//
//        memcpy(&block_buffer3[13], &DevStat.cur_driver_number[1], 3);//操作员号
//
//        //扣除操作员卡内金额押金
//        if ((DevStat.oper_card_amt == ((CharToCash(GrayRecord.oper_card_amt) * 0x100) + GrayRecord.oper_card_amt[3] )) ||
//            (DevStat.oper_card_amt == ((CharToCash(GrayRecord.oper_card_amt) * 0x100) + GrayRecord.oper_card_amt[3]) - CharToCash(GrayRecord.fare)))
//        {
//            Cpucard_Init();
//            temp_int32u = DevStat.deposit_amt[0]*256 + DevStat.deposit_amt[1];
//            if(OperTradeProcess(0, temp_int32u) != 0)
//            {
//                Cpucard_power_off();
//                return notok;
//            }
//            Cpucard_power_off();
//        }
//
//        i = EA_ucMIFWrite(hSam1Handle, 6, block_buffer3);
//        if ( i != ok )
//        {
//            return notok;
//        }
//
//        i = EA_ucMIFWrite(hSam1Handle, 5, block_buffer2);
//        if ( i != ok )
//        {
//            return notok;
//        }
//
//        i = EA_ucMIFWrite(hSam1Handle,4, block_buffer1);
//        if ( i != ok )
//        {
//            return notok;
//        }
//    }
//
////save record
//    if(GrayRecord.card_purchase_type == TT_GRAY_SALE)
//    {
//        //保存售卡记录
//        CardInfo.card_purchase_type = TT_CARD_SALE;
//        (void)StoreRecord(&CardInfo);           //储存售卡记录
//    }
////保存记录
//    if((GrayRecord.card_purchase_type == TT_GRAY_SALE)||(GrayRecord.card_purchase_type == TT_GRAY_CREDIT))
//        CardInfo.card_purchase_type = TT_CREDIT_MONEY;
//    if(GrayRecord.card_purchase_type == TT_GRAY_FLUSH)
//        CardInfo.card_purchase_type = TT_NORMAL_CONSUME_MONEY;
//    CalTAC();
//    EA_ucMIFHalt(hMifsHandle);  //挂卡
//    (void)StoreRecord(&CardInfo);           //储存充值记录
//
////显示结果
//    if(GrayRecord.card_purchase_type == TT_GRAY_SALE)
//        CardInfo.card_purchase_type = TT_CARD_SALE;
//    DisplayPurchaseResult(&CardInfo);   //显示交易结果
//
//
//    if(GrayRecord.card_purchase_type == TT_GRAY_SALE)
//    {
//    //保存日结押金
//        DevStat.binhand.sale_cnt++;
//        DevStat.binhand.deposit_amt += (DevStat.deposit_amt[0] *256 + DevStat.deposit_amt[1])/100;
//        DevStat.last_trade_type = TT_CARD_SALE;
//        WriteParam();
//        if(DevStat.mode==0x00)
//        {
////  		EA_ucSetStopFlag(EM_DISABLE_STOP);
//            print_sale_record();
////  		EA_ucSetStopFlag(EM_ENABLE_STOP);
//        }
//
//    }
//    else
//    {
//        if(DevStat.mode==0x00)
//        {
////  		EA_ucSetStopFlag(EM_DISABLE_STOP);
//            print_record();
////  		EA_ucSetStopFlag(EM_ENABLE_STOP);
//        }
//    }
//
////增加数据上传
//    //数据上传
////  MC530Off();
//    i = GPRS_SEND_RECORD();
//    if(i == ok)
//    {
//        DevStat.have_data_not_send_flag = FALSE;
//    }
//
//    //恢复状态
//    memset(&CardInfo, 0x00, sizeof(CardInfo));      //清卡片交易信息
//    memset(&ErrCardInfo, 0x00, sizeof(ErrCardInfo));    //清出错卡信息
//    //DispPurchaseResultDelay = DISP_DELAY;
//    DevStat.purchase_again_req = FALSE;                 //清重新刷卡语音标志
//    DevStat.voice_purchase_again_freq=FALSE;
//
//    return ok;
//}





