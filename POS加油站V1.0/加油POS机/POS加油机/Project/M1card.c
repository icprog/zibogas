/******************************************************************************
Copyright 2012 	大连易达通电子技术有限公司
All rights riserved.
文 件 名： m1card.c
模块名称： E550 刷卡子程序
功能概要： 主程序
*****************************************************************/
#include "main.h"
#define  Debug_Com

extern DevHandle hCpuHandle;      //CPU卡
extern DevHandle hMifsHandle;
extern DevHandle hSam1Handle;
extern DEV_STAT    DevStat;
ERROR_CARD_INFO ErrCardInfo;
CARD_INFO       CardInfo;
//GRAYRECORD  GrayRecord;
char send_buffer[1024];

#define _debug_m1card_      //调试信息，by frank ,正式出厂时要去掉

#define MAX_REC_NUM			2000
extern INT8U     			KeySector[14][6];

//处理CPU卡使用的变量
INT8U const    RATS[2]=                  {0xE0,0x00};
INT8U const    DESELECT[1]=              {0xC2};
INT8U const    SELECT_USER_ADF1[7]=      {0x00,0xA4,0x00,0x00,0x02,0x10,0x01};
INT8U const    SELECT_USER_ADF2[7]=      {0x00,0xA4,0x00,0x00,0x02,0x10,0x02};
INT8U const    GET_CHALLENGE[5]=         {0x00,0x84,0x00,0x00,0x04};
INT8U const    APPEND_RECORD[4]=         {0x04,0xE2,0x00,0xC8};		//C8可能标识0019文件	2005.02.23	hwr
INT8U const    PURCHASE_INIT[5]=         {0x80,0x50,0x01,0x02,0x0B};						//交易初始化
INT8U const    PURCHASE[5]=              {0x80,0x54,0x01,0x00,0x0F};						//交易
INT8U const    GET_PROVE_CODE[5]=        {0x80,0x5A,0x00,0x06,0x02};						//取交易认证码
INT8U const    GET_BALANCE[5]=           {0x80,0x5C,0x00,0x02,0x04};						//取余额
INT8U const    APPLICATION_LOCK[5]=      {0x84,0x1E,0x00,0x00,0x04};							//锁卡
INT8U const    ADDRECORD[5]=             {0x04,0xd6,0x97,0x00,0x0f};						//写0017文件
INT8U const    VERIFY_PIN[11]=			 {0x00,0x20,0x00,0x00,0x06,0x31,0x32,0x33,0x34,0x35,0x36};//验证口令
INT8U const    READ_15_FILE[5]=		 	 {0x00,0xB0,0x95,0x00,0x24};//读取15文件内容
INT8U const    WRITE_15_FILE[5]=		 {0x00,0xD6,0x95,0x00,0x24};//读取15文件内容

INT8U const    READ_16_FILE[5]=		 	 {0x00,0xB0,0x96,0x00,0x10};//读取16文件内容
INT8U const    WRITE_16_FILE[5]=		 {0x00,0xD6,0x96,0x00,0x10};//读取16文件内容

//处理PSAM卡使用的变量
extern INT8U const    SELECT_SAM_ADF1[7];
extern INT8U const    SELECT_JDSAM_ADF1[7];

extern INT8U const    SELECT_SAM_ADF2[7];
extern INT8U const    SELECT_SAM_ADF3[7];
extern INT8U const    DES_INIT[5];
extern INT8U const    DES_CRYPT[4];
extern INT8U const    GET_MAC1[5];
extern INT8U const    VERIFY_MAC2[5];
extern INT8U const    GET_M1_KEY[5];
extern INT8U const    TAC_INIT[5];
extern INT8U const    TAC_JDINIT[5];

extern INT8U const    CAL_TAC_M1[5];
extern INT8U const    ReadPOSNumber[5];
extern INT8U const    DESCOMMUNICATION1[5];
extern INT8U const    DESCOMMUNICATION[4];
extern INT8U const    ISAM_CAL_KEY_INIT[5];
extern INT8U const    ISAM_CAL_KEY[5];

extern INT8U const   SELECT_SAM_3F01[7];		//选1001目录
extern INT8U const   GET_M1_FUZA_KEY[5];							//计算M1卡个区密钥
extern INT8U const   TAC_FUZA_INIT[5];							//M1卡交易流程中，TAC码计算初始化
extern INT8U const   CAL_TAC_M1_FUZA[5];							//M1卡交易过程中，计算TAC码
//INT8U const   code    GET_MI_FUZA_KEY_YINZI[8]= {0xD1,0xCC,0xCC,0xA8,0x00,0x00,0x00,0x00};                          //M1卡交易过程中，计算TAC码
extern INT8U const   GET_MI_FUZA_KEY_YINZI[8];                          //M1卡交易过程中，计算TAC码
extern INT8U const   FUZHA_TAC_INIT[5];							//M1卡交易流程中，TAC码计算初始化
extern INT8U const   FUZHA_CAL_TAC_M1[5];	

extern const char 	parmFileName[];
extern const char 	blackFileName[];
extern const char 	currecFileName[];
extern const char 	conrecFileName[];
extern const char 	hisrecFileName[];
extern const char 	grayrecFileName[];

#define    PICC_DECREMENT   0
#define    PICC_INCREMENT   1
/*****************************************************************
 函数原型：Get_Sector_PermissionA
 功能描述：取得第N区的读写权限
 参数描述：
 参数名称：	输入/输出？	类型		描述
 sector		输入			INT8U		要取得权限的区
 key		输入			INT8U *		要取得权限区的密钥
				
 返  回  值：ok(0)-成功
				 notok(0xFF)-失败
				   
 作      者	：刘及华
 日      期：2012-1-10
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
		20070207    yufuyong	增加充值密钥验证
*****************************************************************/
INT8U Get_Sector_PermissionA(INT8U sector, INT8U *key)
{
	uchar ucResult;

	ucResult = EA_ucMIFAuthToRam (hMifsHandle, sector, EM_mifs_KEYA, key);
	if (ucResult != EM_SUCCESS)
	{
		   /*  错误处理*/
		return notok;
	}
	else
	{
		// 通过认证
		return ok;
	}

}

/*****************************************************************
 函数原型：Get_Sector_PermissionB
 功能描述：取得第N区的读写权限
 参数描述：
 参数名称：	输入/输出？	类型		描述
 sector		输入			INT8U		要取得权限的区
 key		输入			INT8U *		要取得权限区的密钥
				
 返  回  值：ok(0)-成功
				 notok(0xFF)-失败
				   
 作      者	：刘及华
 日      期：2012-12-10
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
		20070207    yufuyong	增加充值密钥验证
*****************************************************************/
INT8U Get_Sector_PermissionB(INT8U sector, INT8U *key)
{
	uchar ucResult;
	ucResult = EA_ucMIFAuthToRam(hMifsHandle, sector, EM_mifs_KEYB, key);
	if (ucResult != EM_SUCCESS)
	{
		/*  错误处理*/
		lcddisperr("密钥验证错误!");
		return notok;
	}
	else
	{
		return ok;
	}
}
///******************************************************************************
// 函数名称：M1CardConsumeProcess
// 功能描述：M1卡消费交易流程
// 参数描述：
// 参数名称：	输入/输出？	类型		描述
// 
// 返  回  值：ok(0)-成功
//				 notok(0xFF)-失败
//				 0xF0-重新刷卡
//				   
// 作      者	：许岩
// 日      期：2004-09-02
// 修改历史：
//		日期		修改人		修改描述
//		------		---------	-------------
//******************************************************************************/
//INT8U M1CardConsumeProcess(void)
//{
//	INT8U   LOCAL_V_TYPE  i = 0;
//	INT8U   LOCAL_V_TYPE  j = 0;
//	INT32U  LOCAL_V_TYPE  start_using_date = 0;				//启用日期
//	INT32U  LOCAL_V_TYPE  end_using_date = 0;				//有效截至日期
//	INT32U  LOCAL_V_TYPE  cur_date = 0;							//当前时间
//	INT32U  LOCAL_V_TYPE  temp_int32u = 0;
//	INT8S   LOCAL_V_TYPE  h = 0;
//	uint    LOCAL_V_TYPE  rcv_length;
//	INT16U  LOCAL_V_TYPE  k = 0;
//	INT8U   LOCAL_V_TYPE  temp_buf[50];
//	INT8U   LOCAL_V_TYPE  temp_rcv_buf[50];
//	INT8U   LOCAL_V_TYPE  temp_snd_buf[50];
//	INT8U   LOCAL_V_TYPE  block_buffer1[16] = {0};			//M1卡一个block的数据缓冲
//	INT16U  key ;
//
//	if(!ErrCardInfo.error_occured)
//	{
//		if(GetConsumeTradeAmt() != ok)
//		{
//			//EA_vCls();
////  		MC530Off();
////  		InitMC530(TYPE_A_ISO14443);
//			EA_ucMIFHalt(hMifsHandle);
//			return ok;
//		}
//	}	
//
////  MC530Off();
////  if( InitMC530(TYPE_A_ISO14443) != MI_OK)
////  	return notok;
//	
//	if(CardInit() != ok)
//		return notok;
//
//	
//
//	////////////////////////////////////////
//	//出错卡处理
//	////////////////////////////////////////
//	if ( ErrCardInfo.error_occured == TRUE )
//	{//出错卡重入，PSAM卡不能复位
//		h = memcmp(&CardInfo.csn[0], &ErrCardInfo.csn[0], 4);//出错卡硬件卡号比较
//		if ( h != ok )
//		{
//// 			cardinfomation.return_card=1;
//// 			printf_debug("请刷出错卡");
////  		(void)EA_vCls();
//			lcddisperr("请刷出错卡");
//			Beep(1000);
//
//			return notok;//卡交易出错，但回的卡片不是出错卡
//		}
//	}
//	else
//	{
//		////////////////////////////////////////
//		//保存时间
//		////////////////////////////////////////
//		i = SaveCardInTime(&CardInfo);
//		if ( i != ok )
//			return notok;
//		////////////////////////////////////////
//		//PSAM卡复位
//		////////////////////////////////////////
//		i = PsamRst();
//		if ( i != ok )
//		{
//			return notok;
//		}
//
//	}
//
//	////////////////////////////////////////
//	//读Sector 1 中4 区信息
//	////////////////////////////////////////
//	memcpy(&KeySector[1][0], &CardInfo.csn[0], 4);
//	KeySector[1][4] = ~KeySector[1][0];
//	KeySector[1][5] = ~KeySector[1][1];
//
//	i = Get_Sector_PermissionA(1, &KeySector[1][0]);
//	if ( i != ok )
//		return notok;
//
//	i = EA_ucMIFRead(hMifsHandle, 4, block_buffer1);
//
//	if ( i != ok )
//		return notok;
//
//	if ( block_buffer1[12] != 0x01 )	  //判断启用标志
//	{
//		//卡未启用处理
//		(void)EA_vCls();
//		EA_vDisp(3, 1, "     卡片未启用!    ");         
//		Beep(1000);
////  	(void)delay_and_wait_key(3, 0, 0);
//		EA_uiInkey(3);
//		////////////////////////////////////////
//		//M1卡挂卡
//		////////////////////////////////////////
////  	j =EA_ucMIFHalt(hMifsHandle)();	//挂卡
//		EA_ucMIFHalt(hMifsHandle);
//		return ok;
//	}
//
//	//保存卡的应用系列号
//	memcpy(&CardInfo._CARD_NUMBER.detail_card_number.city_code[0], block_buffer1, 2);	//城市代码
//	memcpy(&CardInfo._CARD_NUMBER.detail_card_number.calling_code[0], block_buffer1+2, 2);	//城市代码
//	memcpy(&CardInfo._CARD_NUMBER.detail_card_number.serial_number[0], &block_buffer1[4], 4);//发行流水号
//	memcpy(&CardInfo.authentication_code[0], &block_buffer1[8], 4);//保存卡认证码
//	CardInfo.card_type = block_buffer1[13];	//卡类型
//
//#ifdef DEBUG
//	TRACE232("read block 4 ok.\n\r");
//#endif
//
//
//	////////////////////////////////////////
//	//计算刷卡间隔时间
//	////////////////////////////////////////
////	i=Cal_Interval();
////	if(i!=ok)
////		return notok;
//
//	////////////////////////////////////////
//	//出错卡处理
//	////////////////////////////////////////
//	if ( ErrCardInfo.error_occured == TRUE )
//	{//出错卡重入，PSAM卡不能复位
//		h = memcmp(&CardInfo._CARD_NUMBER.detail_card_number.serial_number[0], &ErrCardInfo.m1_card_number[0], 4);	//出错卡卡号比较
//		if ( h != ok )
//		{
//			return notok;//卡交易出错，但回的卡片不是出错卡
//		}
//	}
//
//// #ifdef USE_TEST_PSAM
//	////////////////////////////////////////
//	//PSAM卡选1001目录
//	////////////////////////////////////////
//	k = CmdPsam(hSam1Handle, (void *)SELECT_SAM_ADF1, sizeof(SELECT_SAM_ADF1), 
//				temp_buf, &rcv_length);		//选1001目录
//	if ( k != ok )
//		return notok;
//// #else
//// 	////////////////////////////////////////
//// 	//PSAM卡选1003目录
//// 	////////////////////////////////////////
//// 	k = CmdPsam(DevStat.cur_psam_channel, (void *)SELECT_SAM_ADF3, sizeof(SELECT_SAM_ADF3),
//// 				temp_buf, &rcv_length);		//选1001目录
//// 	if ( k != 0x9000 )
//// 		return notok;
//// #endif
//
//	////////////////////////////////////////
//	//PSAM卡算各区密钥
//	////////////////////////////////////////
//	memcpy(temp_snd_buf, GET_M1_KEY, sizeof(GET_M1_KEY));//头
//	memcpy(&temp_snd_buf[5], &CardInfo._CARD_NUMBER.detail_card_number.city_code[0], 2);//城市代码
//	memcpy(&temp_snd_buf[7], &CardInfo.csn[0], 4);//CSN硬件卡号
//	memcpy(&temp_snd_buf[11], &CardInfo._CARD_NUMBER.detail_card_number.serial_number[2], 2);//卡发行流水号后2字节
//	memcpy(&temp_snd_buf[13], &CardInfo.authentication_code[0], 4);	//卡认证码
//	temp_snd_buf[17] = 0x02;
//	temp_snd_buf[18] = 0x06;
//	temp_snd_buf[19] = 0x0a;
//	temp_snd_buf[20] = 0x0b;
//	temp_snd_buf[21] = 0x0c;//计算sector2和6,10,11,12的密钥
//	k = CmdPsam(hSam1Handle, temp_snd_buf, 22, temp_rcv_buf, &rcv_length);//算MAC1
//
//	if ( k != ok )
//		return notok;
//	memcpy(&KeySector[2][0], &temp_rcv_buf[0], 6);	//保存计算出的密钥，2sector
//	memcpy(&KeySector[6][0], &temp_rcv_buf[6], 6);//保存计算出的密钥，6sector
//	memcpy(&KeySector[10][0], &temp_rcv_buf[12], 6);//保存计算出的密钥，10sector
//	memcpy(&KeySector[11][0], &temp_rcv_buf[18], 6);//保存计算出的密钥，11sector
//	memcpy(&KeySector[12][0], &temp_rcv_buf[24], 6);//保存计算出的密钥，12sector#ifdef DEBUG
////#ifdef DEBUG
////    TRACE232("psam cal key ok.\n\r");
////#endif
//
//
//	////////////////////////////////////////
//	//查黑名单
//	////////////////////////////////////////
//	memcpy(&temp_int32u, &CardInfo._CARD_NUMBER.detail_card_number.serial_number[0], 4);
//	i = BlackName_Find(temp_int32u);	//黑名单判断
//	if ( i == ok )
//	{	//发现是黑名单卡
////#ifdef DEBUG
////        TRACE232("find black name card!\n\r");
////#endif
//		i = Lock_M1_Card_Process();//黑名单卡处理，锁卡
//		return i;
//	}
//
//	////////////////////////////////////////
//	//读Sector 1 中5 区信息
//	////////////////////////////////////////
//	i = EA_ucMIFRead(hMifsHandle, 5, block_buffer1);
//	if ( i != ok )
//		return notok;
//
//	memcpy(&start_using_date, block_buffer1, 4);//启用日期
//	memcpy(&end_using_date, block_buffer1+4, 4);//有效截至日期
//
//	//做有效期判断
//	memcpy((void *)&cur_date, (void *)&CardInfo.card_in_time.century, 4);//取当前日avoid warning
//	i = CheckPeriodOfValidity(start_using_date, cur_date, end_using_date);	//有效期判断
//	if ( i != ok )
//	{//过期处理，不允许交易
//		//printf_debug("过有效期");
//		EA_vDisp(4, 1, "  此卡过期,继续吗?  ");
////  	if(select_affirm(4,"此卡过期,继续吗?")!=0)
//		key = EA_uiInkey(0);
//		switch(key)
//		{
//		   case EM_key_ENTER:
//			   		Beep(500);
////  		j = EA_ucMIFHalt(hMifsHandle)();	//挂卡
////  				EA_ucMIFHalt(hMifsHandle);
//		//DispPurchaseResultDelay = DISP_DELAY; 			
//					break;
//		   case EM_key_CANCEL:
//				EA_ucMIFHalt(hMifsHandle);
//				return ok;
//		}
//	}
//
//
//	CardInfo.card_purchase_type = TT_NORMAL_CONSUME;//普通卡正常交易
//	
//	////////////////////////////////////////
//	//各种卡的子处理流程
//	////////////////////////////////////////
//
//	switch ( CardInfo.card_type )
//	{
//		case CARDT_NORMAL:		//普通卡
//		case CARDT_STUDENT:	//小学生卡
//// 		case CARDT_STUDENT_B:	//中学生卡
//// 		case CARDT_MONTH:		//月票卡
//		case CARDT_MEMORY:		//纪念卡
//
//		case CARDT_OLDMAN:		//老年卡
//		case CARDT_FREE:	//家属卡
//			i = M1_Card_Consume_ProcessA();
//			if ( i == ok )		  //扣款成功
//			{
//				CardInfo.card_purchase_type = TT_NORMAL_CONSUME;//普通卡正常交易
//				
//				CalConsumeTAC();		//计算TAC码
//				EA_ucMIFHalt(hMifsHandle);
//				
//				(void)StoreConsumeRecord(&CardInfo);			//储存消费记录
//
//				DisplayPurchaseResult(&CardInfo);	//显示交易结果
//
//			//记录最后一次的交易类型
//			//	DevStat.last_trade_type = TT_NORMAL_CONSUME;
//				DevStat.gray_flag = FALSE;
//				DevStat.binhand.consume_cnt++;
//				DevStat.binhand.consume_amt += ((float)(CardInfo.fare))/100;
//				WriteParam();
//				
//				if(DevStat.mode==0x00)
//					print_consume_record();
//
//				//恢复状态
//				memset(&CardInfo, 0x00, sizeof(CardInfo));		//清卡片交易信息
//				memset(&ErrCardInfo, 0x00, sizeof(ErrCardInfo));	//清出错卡信息
//				//DispPurchaseResultDelay = DISP_DELAY;
//				DevStat.purchase_again_req = FALSE; 				//清重新刷卡语音标志
//				DevStat.voice_purchase_again_freq=FALSE;
//			//	   halt_req = TRUE;
//
//#ifdef  DEBUG
//				TRACE232("ICCardProcess over.\n\r");
//#endif
////			//数据上传		
////				MC530Off();
////				i = GPRS_SEND_RECORD();
////				if(i == ok)
////				{
////					DevStat.have_data_not_send_flag = FALSE;
////					WriteParam();
////				}
//				
//			//		MC530Off();
//			//		GPRSProcess();
//				return ok;
//			}
//			break;
//
//		default:
//			printf_debug((void *)"管理卡不能消费");
//			(void)EA_ucMIFHalt(hMifsHandle);	//挂卡
//			return ok;
//	}
//
//	j = EA_ucMIFHalt(hMifsHandle);	//挂卡
//
//	if ( i != ok )
//		return i;
//
////     if((CardInfo.card_purchase_type==COMMON_CONSUME_TYPE)
////     ||(CardInfo.card_purchase_type==FAVOURABLE_CONSUME_TYPE)
////     ||(CardInfo.card_purchase_type==ZERO_CONSUME_TYPE))
////     {
////         //这3种情况计算tac  modified by myron for dandong flow 2005.2.18
////         CalTAC();       //计算TAC码
////     }
////     else
////     {
////     }
//
//	return ok;
//}
//

/******************************************************************************
 函数名称：M1CardProcess
 功能描述：M1卡交易流程
 参数描述：
 参数名称：	输入/输出？	类型		描述
 
 返  回  值：ok(0)-成功
				 notok(0xFF)-失败
				 0xF0-重新刷卡
				   
 作      者	：许岩
 日      期：2004-09-02
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
******************************************************************************/
INT8U M1CardProcess(void)
{
	INT8U     i = 0;
	INT32U    start_using_date = 0;				//启用日期
	INT32U    end_using_date = 0;				//有效截至日期
	INT32U    cur_date = 0;						//当前时间
	INT32U    temp_int32u = 0;
	INT8S     h = 0;
	uint      rcv_length;
	INT16U    k = 0;
	INT8U     temp_rcv_buf[50];
	INT8U     temp_snd_buf[50];
	INT8U     block_buffer1[16] = {0};			//M1卡一个block的数据缓冲

#ifndef Debug_M1card
	////////////////////////////////////////
	//出错卡处理
	////////////////////////////////////////
	if ( ErrCardInfo.error_occured == TRUE )
	{//出错卡重入，PSAM卡不能复位
		h = memcmp(&CardInfo.csn[0], &ErrCardInfo.csn[0], 4);//出错卡硬件卡号比较
		if ( h != ok )
		{
			lcddisperr("请刷出错卡");
			Beeperr();
			return notok;//卡交易出错，但回的卡片不是出错卡
		}
	}
	else
	{
		////////////////////////////////////////
		//保存时间
		////////////////////////////////////////
		i = SaveCardInTime(&CardInfo);
		if ( i != ok )
		{
			lcddisperr("时间出错");
			return notok;
		}
			
	}

	 i = PsamRst();
	 if ( i != ok )
	 {
		 lcddisperr((char *)"PSAM卡故障");
		 return notok;
	 }
	
	////////////////////////////////////////
	//读Sector 1 中4 区信息
	////////////////////////////////////////
	memcpy(&KeySector[1][0], &CardInfo.csn[0], 4);
	KeySector[1][4] = ~KeySector[1][0];
	KeySector[1][5] = ~KeySector[1][1];
	memset(&KeySector[1][0],0xFF,6);

	i = Get_Sector_PermissionA(1, &KeySector[1][0]);
	if ( i != ok )
	{
		lcddisperr((char *)"密钥1区计算失败");
		return notok;
	}
		
	i = EA_ucMIFRead(hMifsHandle, 4, block_buffer1);
	if ( i != ok )
	{
		lcddisperr((char *)"密钥2区计算失败");
		return notok;
	}
		

	if ( block_buffer1[12] != 0x02 )	  //判断启用标志
	{
		//卡未启用处理
		lcddisperr((char *)"卡片未启用");
		Beeperr();
		return ok;
	}

	//保存卡的应用系列号
	memcpy(&CardInfo._CARD_NUMBER.detail_card_number.city_code[0], block_buffer1, 2);	//城市代码
	memcpy(&CardInfo._CARD_NUMBER.detail_card_number.calling_code[0], block_buffer1+2, 2);	//城市代码
//  memcpy(&CardInfo._CARD_NUMBER.detail_card_number.serial_number[0], &block_buffer1[4], 4);//发行流水号
	memcpy(&CardInfo.authentication_code[0], &block_buffer1[8], 4);//保存卡认证码
	CardInfo.card_type = block_buffer1[13];	//卡类型
#ifdef _debug_m1card_
	EA_vDisplay(3, "加油卡类型:%02x", CardInfo.card_type);
	SleepMs(2000);
#endif
//  if (CardInfo.card_type != 0x00)                 //只允许普通卡消费debug  by frank
//  {
//  	lcddisperr((void *)"不是普通加油卡!");
//  	return notok;
//  }

		////////////////////////////////////////
	//读Sector 1 中5 区信息时间等信息
	////////////////////////////////////////
	i = EA_ucMIFRead(hMifsHandle, 5, block_buffer1);
	if ( i != ok )
	{
//  	lcddisperr((char *)"密钥3区计算失败");
		return notok;
	}

	memcpy(&start_using_date, block_buffer1+8, 4);//启用日期
	memcpy(&end_using_date, block_buffer1+4, 4);//有效截至日期
	memcpy(&cur_date, &CardInfo.card_in_time, 4);//取当前日期

	if ( cur_date < start_using_date )
	{
		//卡未启用处理
		lcddisperr("卡片未启用!");         
		Beeperr();
		return notok;
	}
	////////////////////////////////////////
	//PSAM卡选3F01目录
	////////////////////////////////////////
	k = CmdPsam(hSam1Handle, (INT8U *)SELECT_SAM_3F01, sizeof(SELECT_SAM_3F01), 
				temp_rcv_buf, &rcv_length);		//选3F01目录
	if ( k != ok )
	{
//  	lcddisperr("PSAM卡选3F01目录错误");
		return notok;
	}
	////////////////////////////////////////
	//PSAM卡算各区密钥
	////////////////////////////////////////
	memcpy(temp_snd_buf, (INT8U *)GET_M1_FUZA_KEY, sizeof(GET_M1_FUZA_KEY));//头
	memcpy(&temp_snd_buf[5], &CardInfo._CARD_NUMBER.detail_card_number.city_code[0], 2);//城市代码
	memcpy(&temp_snd_buf[7], &CardInfo.csn[0], 4);//CSN硬件卡号
	temp_snd_buf[11]=0x00;
	temp_snd_buf[12]=0x00;
	memcpy(&temp_snd_buf[13], &CardInfo.authentication_code[0], 4);	//卡认证码
	temp_snd_buf[17] = 0x02;
	temp_snd_buf[18] = 0x03;
	temp_snd_buf[19] = 0x04;
	temp_snd_buf[20] = 0x05;
	temp_snd_buf[21] = 0x06;//计算sector2和6,8,11,12的密钥
	memcpy(&temp_snd_buf[22], (INT8U *)GET_MI_FUZA_KEY_YINZI, sizeof(GET_MI_FUZA_KEY_YINZI));//头
	k = CmdPsam(hSam1Handle, temp_snd_buf, 30, temp_rcv_buf, &rcv_length);//算MAC1
	if ( k != ok )
	{
//  	lcddisperr("PSAM卡算MAC1错误!");
		return notok;
	}
	
	memcpy(&KeySector[2][0], &temp_rcv_buf[0], 6);	//保存计算出的密钥，2sector
	memcpy(&KeySector[3][0], &temp_rcv_buf[6], 6);  //保存计算出的密钥，6sector
	memcpy(&KeySector[4][0], &temp_rcv_buf[12], 6); //保存计算出的密钥，10sector
	memcpy(&KeySector[5][0], &temp_rcv_buf[18], 6); //保存计算出的密钥，11sector
	memcpy(&KeySector[6][0], &temp_rcv_buf[24], 6); //保存计算出的密钥，12sector

	memcpy(temp_snd_buf, (INT8U *)GET_M1_FUZA_KEY, sizeof(GET_M1_FUZA_KEY));//头
	temp_snd_buf[4]=0x15;
	memcpy(&temp_snd_buf[5], &CardInfo._CARD_NUMBER.detail_card_number.city_code[0], 2);//城市代码
	memcpy(&temp_snd_buf[7], &CardInfo.csn[0], 4);//CSN硬件卡号
	temp_snd_buf[11]=0x00;
	temp_snd_buf[12]=0x00;
	memcpy(&temp_snd_buf[13], &CardInfo.authentication_code[0], 4);	//卡认证码
	temp_snd_buf[17] = 0x08;
	memcpy(&temp_snd_buf[18], (INT8U *)GET_MI_FUZA_KEY_YINZI, sizeof(GET_MI_FUZA_KEY_YINZI));//头
	k = CmdPsam(hSam1Handle, temp_snd_buf, 26, temp_rcv_buf, &rcv_length);//算MAC1
	if ( k != ok )
	{
//  	lcddisperr("PSAM卡算MAC2错误!");
		return notok;
	}
	memcpy(&KeySector[8][0], &temp_rcv_buf[0], 6);	//保存计算出的密钥，2sector
	//密钥计算完毕

	//读卡号
	i = Get_Sector_PermissionA(8, &KeySector[8][0]);
	if ( i != ok )
	{
//  	lcddisperr((char *)"密钥4区计算失败");
		return notok;
	}
	i = EA_ucMIFRead(hMifsHandle, 33, block_buffer1);
	if ( i != ok )
	{
//  	lcddisperr((char *)"密钥5区计算失败");
		return notok;
	}
	memcpy(&CardInfo._CARD_NUMBER.detail_card_number.calling_code[0], block_buffer1+4, 2);//行业代码
	CardInfo._CARD_NUMBER.detail_card_number.calling_code[0]=0x31;
	memcpy(&CardInfo._CARD_NUMBER.detail_card_number.serial_number[0], &block_buffer1[6], 4);//发行流水号


	////////////////////////////////////////
	//出错卡处理
	////////////////////////////////////////
	if ( ErrCardInfo.error_occured == TRUE )//出错卡重入
	{
		h = memcmp(&CardInfo._CARD_NUMBER.detail_card_number.serial_number[0], &ErrCardInfo.m1_card_number[0], 4);	//出错卡卡号比较
		if ( h != ok )
		{
			lcddisperr("请刷出错卡!");
			return notok;//卡交易出错，但回的卡片不是出错卡
		}
	}

	//////////////////////////////////////////////////////////////
	//查黑名单此卡还没有被锁，从库里得到黑名单
	//////////////////////////////////////////////////////////////
#ifdef Debug_Com
	sprintf(send_buffer, "\r\n卡的硬件号:%02X%02X%02X%02X\r\n",
			CardInfo.csn[0], CardInfo.csn[1], CardInfo.csn[2], CardInfo.csn[3]);
	Debugprintf(send_buffer);
	sprintf(send_buffer, "\r\n卡的序列号:%02X%02X%02X%02X\r\n",
			CardInfo._CARD_NUMBER.detail_card_number.serial_number[0], CardInfo._CARD_NUMBER.detail_card_number.serial_number[1],
				CardInfo._CARD_NUMBER.detail_card_number.serial_number[2], CardInfo._CARD_NUMBER.detail_card_number.serial_number[3]);
	Debugprintf(send_buffer);
#endif
	memcpy(&temp_int32u, &CardInfo._CARD_NUMBER.detail_card_number.serial_number[0], 4);
	i = BlackName_Find(temp_int32u);	//白名单判断
	if ( i != ok )	 					//不是白名单
	{
		lcddisperr("无效卡!");
		Beeperr();
		return i;
	}

	lcddisperr("有效卡!");
	
	//////////////////////////////////////////////////////////////
	//查看此卡当日是否已刷过
	//////////////////////////////////////////////////////////////
//  i = Cal_Interval();
//  if( i !=ok )
//  {
//  	lcddisperr("此卡当日已加过油!");
//  	Beeperr();
//  	return i;
//  }

	CardInfo.card_purchase_type = TT_NORMAL_CONSUME;    		//普通卡正常交易

#endif

	i = M1_Card_Consume_Process();
	if ( i == ok )		    //扣款成功
	{    
		if ( DisplayPurchaseResult(&CardInfo) == ok )  			//显示交易结果
		{
			CalTAC();		  //计算TAC码
			EA_ucMIFHalt(hMifsHandle);
			(void)StoreRecord(&CardInfo);			   			//储存消费记录
			//打印本次交易数据
			print_sale_record();
			//恢复状态
			memset(&CardInfo, 0x00, sizeof(CardInfo));			//清卡片交易信息
			memset(&ErrCardInfo, 0x00, sizeof(ErrCardInfo));	//清出错卡信息
			//DispPurchaseResultDelay = DISP_DELAY;
			DevStat.purchase_again_req = FALSE;					//清重新刷卡语音标志
			DevStat.voice_purchase_again_freq = FALSE;
			return ok;
		}
	}

	lcddisperr("交易未完成!");

	return i;
}

/******************************************************************************
 函数名称：StoreRecord
 功能描述：交易记录存储函数
 参数名称：	输入/输出？	类型		描述
cardinfo	输入			CARD_INFO*	交易完毕后的卡片的所有信息
				
 返  回  值：无
 
 作      者	：许岩
 日      期：2004-09-23
 修改历史：
		日期		修改人		修改描述
		2005.2.18		myron	                     in function 
******************************************************************************/
INT8U StoreRecord(CARD_INFO *cardinfo)
{
	INT8U LOCAL_V_TYPE i = 0;
	INT8U record[60];
	INT32U temp_int32u = 0;
//	INT8S temp[100];
	INT16U crc = 0;

	RECORD * prec = (RECORD*)record;
	
	memset(record, 0x00, sizeof(record));


	//////////////////////////////////////////
	//组织交易记录格式，相同部份
	//////////////////////////////////////////
	prec->card_type = cardinfo->card_type;//卡类型1字节
	prec->card_purchase_type = cardinfo->card_purchase_type;//交易类型 1字节	

	temp_int32u = query_rec_num();
//  temp_int32u = DevStat.record_number;
	prec->pos_purchase_serial_num[0] = (INT8U)(temp_int32u >> 16);	//终端机交易序号
	prec->pos_purchase_serial_num[1] = (INT8U)(temp_int32u >> 8);
	prec->pos_purchase_serial_num[2] = (INT8U)temp_int32u;

//  memcpy(&prec->card_serial_number[0], &cardinfo->_CARD_NUMBER.card_number[0], 8);//城市代码2字节+行业代码2字节+卡发行流水号4字节
    memcpy(&prec->bus_number[0], &cardinfo->bus_number[0], 5);//城市代码2字节+行业代码2字节+卡发行流水号4字节
	memcpy((void *)&prec->card_in_time, &cardinfo->card_in_time, 7);//交易日期4字节和时间 3字节
	memcpy(&prec->tac[0], &cardinfo->tac[0], 4);//TAC码 4字节

//  memcpy(&prec->balance[0], ((INT8U *)(&cardinfo->balance))+1, 3); //交易后余额 3字节
	memcpy(&prec->fare[0], (INT8U *)(&cardinfo->fare), 3);	    //加油体积 3字节
	memcpy(&prec->purchase_serial_num[0], ((INT8U *)(&cardinfo->purchase_serial_num[0])), 2); //用户卡脱机交易序号 2字节

	memcpy(&prec->acnt_id[0], &DevStat.acnt_id[0], 2);
//  memcpy(&prec->spot_id[0], &DevStat.spot_id[0], 2);
	memcpy(&prec->equ_id[0], &DevStat.equ_id[0], 3);
	memcpy(&prec->fuel_type, &DevStat.fill_mode, 1);

	memcpy(&prec->isam_no[0], &DevStat.isam_no[0], 6);
	memcpy(&prec->driver_id[0], &DevStat.cur_driver_number[0],4);

	crc = Cal_CRC(record, 49);  

	record[49] = ((crc >> 8)& 0xff);//CRC1
	record[50] = (crc & 0xff);//CRC2

	i = SaveCardRecord(record);		

	DevStat.binhand.consume_cnt++;
	DevStat.binhand.consume_amt+=cardinfo->fare;

	WriteParam();
	
	return ok;
}
/******************************************************************************
 函数名称：void Cal_Interval()
 功能描述：
 参数描述：
 参数名称：	输入/输出？	类型		描述
 none
 
 返  回  值：ok or notok
   
 作      者	：myron
 日      期：2005.4.21
 修改历史：
		日期		修改人		修改描述

******************************************************************************/
INT8U Cal_Interval(void)
{
     INT16U LOCAL_V_TYPE k = 0;
     INT8U  temp_buf[55];
     INT8S LOCAL_V_TYPE h = 0;

	 DevStat.record_number = query_rec_num();
     ////////////////////////////////////////
     //计算刷卡间隔时间
     ////////////////////////////////////////
     for(k=0; k<DevStat.record_number; k++)
     {
         Get_Record(temp_buf, 0, (DevStat.record_number - k)); //取最近第n条记录
         h=memcmp(&CardInfo.card_in_time, &temp_buf[36], 4);   //比较时间
         if(h == ok)
         {//年 月日 相等
             
             h=memcmp(&CardInfo._CARD_NUMBER.card_number[0], &temp_buf[22], 8);
             if(h==ok)
             {//卡号相等
                 return notok;
             }
             else
             {//不相等则继续查询
                 ;
             }
         }
         else
         {//相差一天以上
             break;
         }
     }
	 return ok;
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
void Get_Record(INT8U *l_rec_buf, INT16U start_position, INT16U rec_pointer)
{
	INT8U p[60];
	uchar ucOpenID = 0;
	uint  uiRetNum = 0;

	memset(p, 0, sizeof(p));

	if ( EA_ucPFOpen((uchar *)currecFileName, &ucOpenID) != EM_ffs_SUCCESS )
	{
		lcddisperr("打开记录文件失败");
		return ;
	}
	if ( EA_ucPFReadRec(ucOpenID, rec_pointer, 1, &uiRetNum, p) != EM_ffs_SUCCESS )
	{
		lcddisperr("读取记录文件失败");
		return ;
	}

	(void)EA_ucPFClose(ucOpenID);

	memcpy(l_rec_buf, p, 49);
}
/******************************************************************************
 函数名称：M1CardQuery
 功能描述：M1卡信息查询
 参数描述：
 参数名称：	输入/输出？	类型		描述
 
 返  回  值：ok(0)-成功
				 notok(0xFF)-失败
				   
 作      者	：许岩
 日      期：2004-09-02
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
******************************************************************************/
INT8U M1CardQuery(void)
{
	INT8U   LOCAL_V_TYPE  i = 0;
	INT32U  LOCAL_V_TYPE  start_using_date = 0;				//启用日期
	INT32U  LOCAL_V_TYPE  end_using_date = 0;				//有效截至日期
	INT32U  LOCAL_V_TYPE  cur_date = 0;							//当前时间
	uint    LOCAL_V_TYPE  rcv_length;
	INT16U  LOCAL_V_TYPE  k = 0;
	INT8U   LOCAL_V_TYPE  temp_rcv_buf[50];
	INT8U   LOCAL_V_TYPE  temp_snd_buf[50];
	INT8U   LOCAL_V_TYPE  block_buffer1[16] = {0};			//M1卡一个block的数据缓冲
	
//  if(CardInit() != ok)
//  {
//  	return notok;
//  }

	////////////////////////////////////////
	//读Sector 1 中4 区信息
	////////////////////////////////////////
	memcpy(&KeySector[1][0], &CardInfo.csn[0], 4);
	KeySector[1][4] = ~KeySector[1][0];
	KeySector[1][5] = ~KeySector[1][1];

	i = Get_Sector_PermissionA(1, &KeySector[1][0]);
	if ( i != ok )
	{
//  	lcddisperr("密钥验证错误!");
		EA_ucMIFHalt (hMifsHandle);	//挂卡
		return notok;
	}
			
	i = EA_ucMIFRead(hMifsHandle, 4, block_buffer1);
	if ( i != ok )
	{
//  	lcddisperr("读4区文件错误!");
		EA_ucMIFHalt (hMifsHandle);	//挂卡
		return notok;
	}

	if ( block_buffer1[12] != 0x01 )	  //判断启用标志
	{
		//卡未启用处理
//  	Beep(500);
		lcddisperr("卡片未启用");
		Beeperr();
		EA_ucMIFHalt(hMifsHandle);
		return ok;
	}
	//保存卡的应用系列号
	memcpy(&CardInfo._CARD_NUMBER.detail_card_number.city_code[0], block_buffer1, 2);	//城市代码
	memcpy(&CardInfo._CARD_NUMBER.detail_card_number.calling_code[0], block_buffer1+2, 2);	//城市代码
// 	DevStat.cur_psam_channel=block_buffer1[2];
	memcpy(&CardInfo._CARD_NUMBER.detail_card_number.serial_number[0], &block_buffer1[4], 4);//发行流水号
	memcpy(&CardInfo.authentication_code[0], &block_buffer1[8], 4);//保存卡认证码
	CardInfo.card_type = block_buffer1[13];	//卡类型

	////////////////////////////////////////
	//读Sector 1 中5 区信息
	////////////////////////////////////////
	i = EA_ucMIFRead(hMifsHandle, 5, block_buffer1);
	if ( i != ok )
	{
//  	lcddisperr("读5区错误");
		EA_ucMIFHalt (hMifsHandle);	//挂卡
		return notok;
	}

	memcpy(&start_using_date, block_buffer1, 4);//启用日期
	memcpy(&end_using_date, block_buffer1+4, 4);//有效截至日期

	////////////////////////////////////////
	//判断有效期
	////////////////////////////////////////

	//做有效期判断
	memcpy((void *)&cur_date, (void *)&CardInfo.card_in_time.century, 4);//取当前日avoid warning
	i = CheckPeriodOfValidity(start_using_date, cur_date, end_using_date);	//有效期判断
	if ( i != ok )
	{	//过期处理，不允许交易
		lcddisperr("此卡已经过期");
		EA_ucMIFHalt (hMifsHandle);	//挂卡
		return ok;
	}

	////////////////////////////////////////
	//PSAM卡算各区密钥
	////////////////////////////////////////
	memcpy(temp_snd_buf, GET_M1_KEY, sizeof(GET_M1_KEY));//头
	memcpy(&temp_snd_buf[5], &CardInfo._CARD_NUMBER.detail_card_number.city_code[0], 2);//城市代码
	memcpy(&temp_snd_buf[7], &CardInfo.csn[0], 4);//CSN硬件卡号
	memcpy(&temp_snd_buf[11], &CardInfo._CARD_NUMBER.detail_card_number.serial_number[2], 2);//卡发行流水号后2字节
	memcpy(&temp_snd_buf[13], &CardInfo.authentication_code[0], 4);	//卡认证码
	temp_snd_buf[17] = 0x02;
	temp_snd_buf[18] = 0x06;
	temp_snd_buf[19] = 0x0a;
	temp_snd_buf[20] = 0x0b;
	temp_snd_buf[21] = 0x0c;//计算sector2和6,10,11,12的密钥

	k = CmdPsam(hSam1Handle, temp_snd_buf, 22, temp_rcv_buf, &rcv_length);//算MAC1
	if ( k != ok )
	{
		lcddisperr("计算密钥失败");
		return notok;
	}

	memcpy(&KeySector[2][0], &temp_rcv_buf[0], 6);	//保存计算出的密钥，2sector
	memcpy(&KeySector[6][0], &temp_rcv_buf[6], 6);//保存计算出的密钥，6sector
	memcpy(&KeySector[10][0], &temp_rcv_buf[12], 6);//保存计算出的密钥，10sector
	memcpy(&KeySector[11][0], &temp_rcv_buf[18], 6);//保存计算出的密钥，11sector
	memcpy(&KeySector[12][0], &temp_rcv_buf[24], 6);//保存计算出的密钥，12sector

	////////////////////////////////////////
	//验证Sector 6 密钥
	////////////////////////////////////////
	i = Get_Sector_PermissionA(6, &KeySector[6][0]);
	if ( i != ok )
	{
		EA_ucMIFHalt (hMifsHandle);	//挂卡
		return notok;
	}
	
	////////////////////////////////////////
	//判断此卡是否已经置黑名单标志
	////////////////////////////////////////
	if ( block_buffer1[7] == 0x04 )
	{
		lcddisperr("非法卡");
		(void)EA_ucMIFHalt(hMifsHandle);	
		return ok;
	}
	switch ( CardInfo.card_type )
	{
		case CARDT_NORMAL:
		case CARDT_STUDENT:
		case CARDT_OLDMAN:
		case CARDT_FREE:
		case CARDT_MEMORY:
		case CARDT_DRIVER:
			i = QueryNormal();
			(void)EA_ucMIFHalt(hMifsHandle);
			return i;
		default:
			EA_vDisp(1,1,"设备不支持此卡!");
			(void)EA_ucMIFHalt(hMifsHandle);
			return ok;
	}

}
/******************************************************************************
 函数名称：M1_Driver_Card_Process
 功能描述：M1司机卡处理流程
 参数描述：
 参数名称： 输入/输出？ 类型        描述
 无输入      输出状态码


 返  回  值：ok(0)-成功
			 notok(0xFF)-失败

 作      者 ：myron
 日      期：2005-02-17
 修改历史：
		日期        修改人      修改描述
		------      ---------   -------------
******************************************************************************/
INT8U M1_Driver_Card_Process(void)
{
	return ok;
}

/* *****************************************************************************
 函数名称：M1_Set_Card_Process
 功能描述：M1票价车号线路设置卡流程
 参数描述：
 参数名称：	输入/输出？	类型		描述
无输入		输出状态码
 
 返  回  值：ok(0)-成功
			 notok(0xFF)-失败
				   
 作      者	：myron
 日      期：2005-02-16
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
***************************************************************************** */
INT8U M1_Set_Card_Process(void)
{
	return ok;
}

/******************************************************************************
 函数名称：M1_Checked_Card_Process
 功能描述：M1检测卡处理流程
 参数描述：
 参数名称：	输入/输出？	类型		描述
无输入		输出状态码
 
 返  回  值：ok(0)-成功
			 notok(0xFF)-失败
				   
 作      者	：myron
 日      期：2005-02-16
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
******************************************************************************/
INT8U M1_Checked_Card_Process()
{
/*	
	INT8U LOCAL_V_TYPE  i = 0;
	INT8U LOCAL_V_TYPE  j = 0;
	INT8U LOCAL_V_TYPE  m = 0;
//	INT16U LOCAL_V_TYPE k = 0;
	INT16U LOCAL_V_TYPE temp_int16u;
	INT8S LOCAL_V_TYPE g = 0;
//	INT8S LOCAL_V_TYPE h = 0;

	INT8U  block_buffer1[16] = {0};
	INT8U  block_buffer2[16] = {0};

	////////////////////////////////////////
	//验证Sector 6 密钥
	////////////////////////////////////////
	i=Sector6_Process();
	if ( i!=ok )
		return notok;

// 	////////////////////////////////////////////////////////
// 	//出错判断
// 	////////////////////////////////////////////////////////
// 	if ( ErrCardInfo.error_occured == TRUE )  //保证了交易序号可断不可重，交易序号为数据库中主键之一
// 	{
// 		//出错卡重入
// 		memcpy(&ErrCardInfo.block24_buf_to_restore[12], "\x18\xE7\x18\xE7", 4);		//强制置为0x18E7
// 		i = CardMFCWrite16Bytes(24, &ErrCardInfo.block24_buf_to_restore[0]);
// 		if ( i != ok )
// 			return notok;
//
// 		i = CardMFCWrite16Bytes(25, &ErrCardInfo.block24_buf_to_restore[0]);
// 		if ( i != ok )
// 			return notok;
// 	}
// 	else
// 	{		//第一次入卡处理
// 		////////////////////////////////////////
// 		//卡内交易计数器处理
// 		////////////////////////////////////////
// 		memcpy(&CardInfo.init_purchase_serial_num[0], block_buffer1+2, 2);				//初始交易序号
// 		memcpy(&temp_int16u, block_buffer1+2, 2);														//初始交易序号
// 		if ( temp_int16u >= 0xFFFF )
// 			temp_int16u = 0;
// 		else
// 			temp_int16u++;
// 		memcpy(&CardInfo.purchase_serial_num[0], &temp_int16u, 2);						//+1后的交易序号
//
// 		memcpy(&ErrCardInfo.block24_buf_to_restore[0], block_buffer1, 16);				//凑24区应该写入的数据
// 		memcpy(&ErrCardInfo.block24_buf_to_restore[2], &CardInfo.purchase_serial_num[0], 2);
// 		memcpy(&ErrCardInfo.block24_buf_to_restore[12], "\x18\xE7\x18\xE7", 4);		//强制置为0x18E7
// 		i = CardMFCWrite16Bytes(24, &ErrCardInfo.block24_buf_to_restore[0]);
// 		if ( i != ok )
// 			return notok;
//
// 		i = CardMFCWrite16Bytes(25, &ErrCardInfo.block24_buf_to_restore[0]);
// 		if ( i != ok )
// 			return notok;
//
// 	}
	CardInfo.psam_purchase_serial_num = 0;//PSAM卡交易序号清零
//	CardInfo.card_purchase_type= 0x10;

	////////////////////////////////////////
	//验证Sector 11 密钥
	////////////////////////////////////////
	i = Get_Sector_PermissionA(11, &KeySector[11][0]);
	if ( i != ok )
		return notok;

	i = CardMFCRead16Bytes(44, block_buffer1);
	if ( i != ok )
		return notok;

	CardInfo.checked_flag= block_buffer1[3];//保存检测标志
	memcpy(&CardInfo.manager_number.checker_number[0],block_buffer1,sizeof(CardInfo.manager_number.checker_number));//保存检测员编号
	CardInfo.card_purchase_type=TT_CHECK;
*/
	return ok;
}

/******************************************************************************
 函数名称：M1_Inspector_Card_Process
 功能描述：M1稽查卡处理流程
 参数描述：
 参数名称：	输入/输出？	类型		描述
无输入		输出状态码
 
 返  回  值：ok(0)-成功
			 notok(0xFF)-失败
				   
 作      者	：myron
 日      期：2005-02-16
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
******************************************************************************/
INT8U M1_Inspector_Card_Process(void)
{
	return ok;
}

/******************************************************************************
 函数名称：M1_Signed_Card_Process
 功能描述：M1签点卡处理流程
 参数描述：
 参数名称： 输入/输出？ 类型		描述
无输入		输出状态码
 
 返  回  值：ok(0)-成功
			 notok(0xFF)-失败
				   
 作 	 者 ：myron
 日 	 期：2005-02-16
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
******************************************************************************/
INT8U M1_Signed_Card_Process()
{
/*	
	INT8U LOCAL_V_TYPE  i = 0;
	INT8U LOCAL_V_TYPE  j = 0;
	INT8U LOCAL_V_TYPE  m = 0;
	INT16U LOCAL_V_TYPE temp_int16u;
	INT8S LOCAL_V_TYPE g = 0;
	INT8U block_buffer1[16] = {0};
	INT8U block_buffer2[16] = {0};

	////////////////////////////////////////
	//验证Sector 6 密钥
	////////////////////////////////////////
	i=Sector6_Process();
	if ( i!=ok )
		return notok;

// 	////////////////////////////////////////////////////////
// 	//出错判断
// 	////////////////////////////////////////////////////////
// 	if ( ErrCardInfo.error_occured == TRUE )  //保证了交易序号可断不可重，交易序号为数据库中主键之一
// 	{
// 		//出错卡重入
// 		memcpy(&ErrCardInfo.block24_buf_to_restore[12], "\x18\xE7\x18\xE7", 4);		//强制置为0x18E7
// 		i = CardMFCWrite16Bytes(24, &ErrCardInfo.block24_buf_to_restore[0]);
// 		if ( i != ok )
// 			return notok;
//
// 		i = CardMFCWrite16Bytes(25, &ErrCardInfo.block24_buf_to_restore[0]);
// 		if ( i != ok )
// 			return notok;
// 	}
// 	else
// 	{		//第一次入卡处理
// 		////////////////////////////////////////
// 		//卡内交易计数器处理
// 		////////////////////////////////////////
// 		memcpy(&CardInfo.init_purchase_serial_num[0], block_buffer1+2, 2);				//初始交易序号
// 		memcpy(&temp_int16u, block_buffer1+2, 2);														//初始交易序号
// 		if ( temp_int16u >= 0xFFFF )
// 			temp_int16u = 0;
// 		else
// 			temp_int16u++;
// 		memcpy(&CardInfo.purchase_serial_num[0], &temp_int16u, 2);						//+1后的交易序号
//
// 		memcpy(&ErrCardInfo.block24_buf_to_restore[0], block_buffer1, 16);				//凑24区应该写入的数据
// 		memcpy(&ErrCardInfo.block24_buf_to_restore[2], &CardInfo.purchase_serial_num[0], 2);
// 		memcpy(&ErrCardInfo.block24_buf_to_restore[12], "\x18\xE7\x18\xE7", 4);		//强制置为0x18E7
// 		i = CardMFCWrite16Bytes(24, &ErrCardInfo.block24_buf_to_restore[0]);
// 		if ( i != ok )
// 			return notok;
//
// 		i = CardMFCWrite16Bytes(25, &ErrCardInfo.block24_buf_to_restore[0]);
// 		if ( i != ok )
// 			return notok;
//
// 	}
	CardInfo.psam_purchase_serial_num = 0;//PSAM卡交易序号清零
//	CardInfo.card_purchase_type= 0x10;

	////////////////////////////////////////
	//验证Sector 11 密钥
	////////////////////////////////////////
	i = Get_Sector_PermissionA(11, &KeySector[11][0]);
	if ( i != ok )
		return notok;

	i = CardMFCRead16Bytes(44, block_buffer1);
	if ( i != ok )
		return notok;

	memcpy(&CardInfo.manager_number.signer_number[0],block_buffer1,sizeof(CardInfo.manager_number.signer_number));//保存签点员编号
	memcpy(&CardInfo.station_number[0],block_buffer1+5,sizeof(CardInfo.station_number));//保存站点编号
	CardInfo.card_purchase_type = TT_SIGN;
*/
	return ok;
}

/******************************************************************************
 函数名称：M1_Gather_Card_Process
 功能描述：M1采集卡处理流程
 参数描述：
 参数名称：	输入/输出？	类型		描述
无输入		输出状态码
 
 返  回  值：ok(0)-成功
			 notok(0xFF)-失败
				   
 作      者	：myron
 日      期：2005-02-16
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
******************************************************************************/
INT8U M1_Gather_Card_Process()
{
/*	
	INT8U LOCAL_V_TYPE  i = 0;
	INT8U LOCAL_V_TYPE  j = 0;
	INT8U LOCAL_V_TYPE  m = 0;
	INT16U LOCAL_V_TYPE temp_int16u;
	INT8S LOCAL_V_TYPE g = 0;
	INT8U block_buffer1[16] = {0};
	INT8U block_buffer2[16] = {0};

	////////////////////////////////////////
	//验证Sector 6 密钥
	////////////////////////////////////////
	i = Get_Sector_PermissionA(6, &KeySector[6][0]);
	if ( i != ok )
		return notok;

	i = CardMFCRead16Bytes(24, block_buffer1);
	if ( i != ok )
		return notok;

	////////////////////////////////////////
	//判断此卡是否已经置黑名单标志
	////////////////////////////////////////
	if ( block_buffer1[6] == 0x04 )
	{
		printf_debug("非法卡");
		return notok;
	}

//     i = CardMFCRead16Bytes(25, block_buffer2);
//     if ( i != ok )
//         return notok;

	if ( DevStat.driver_is_working == TRUE )
	{
// 		DevStat.driver_is_working = FALSE;
// 		CardInfo.card_purchase_type = TT_DRIVER_REST;
// 		WriteParam();
		QuitWorkProcess();
		return ok;
	}

	CardInfo.psam_purchase_serial_num = 0;										//PSAM卡交易序号清零

	////////////////////////////////////////
	//验证Sector 11 密钥
	////////////////////////////////////////
	i = Get_Sector_PermissionA(11, &KeySector[11][0]);
	if ( i != ok )
		return notok;

	i = CardMFCRead16Bytes(44, block_buffer1);
	if ( i != ok )
		return notok;

	DevStat.allow_upload_data = TRUE;//允许采集
	memcpy(&CardInfo.manager_number.gather_number[0],block_buffer1,sizeof(CardInfo.manager_number.gather_number));//保存采集员编号
	CardInfo.card_purchase_type = TT_MANAGE;
*/
	return ok;
}

/******************************************************************************
 函数名称：M1_Favor_Card_Process()
 功能描述：M1采集卡处理流程
 参数描述：
 参数名称：	输入/输出？	类型		描述
无输入		输出状态码
 
 返  回  值：ok(0)-成功
			 notok(0xFF)-失败
				   
 作      者	：myron
 日      期：2005-02-22
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
******************************************************************************/
// INT8U M1_Favor_Card_Process()
// {
//     INT8U LOCAL_V_TYPE  i = 0;
//     INT8U LOCAL_V_TYPE  j = 0;
//     INT8U LOCAL_V_TYPE  m = 0;
//     INT16U LOCAL_V_TYPE temp_int16u;
//     INT8S LOCAL_V_TYPE g = 0;
//     INT8U block_buffer1[16] = {0};
//     INT8U block_buffer2[16] = {0};
//     INT16U LOCAL_V_TYPE temp_balance;
//     ////////////////////////////////////////
//     //验证Sector 6 密钥
//     ////////////////////////////////////////
//     i=Sector6_Process();
//     if ( i!=ok )
//         return notok;
//
//     ////////////////////////////////////////////////////////
//     //出错判断
//     ////////////////////////////////////////////////////////
//     if ( ErrCardInfo.error_occured == TRUE )  //保证了交易序号可断不可重，交易序号为数据库中主键之一
//     {
//         //出错卡重入
//         memcpy(&ErrCardInfo.block24_buf_to_restore[12], "\x18\xE7\x18\xE7", 4);     //强制置为0x18E7
//         i = CardMFCWrite16Bytes(24, &ErrCardInfo.block24_buf_to_restore[0]);
//         if ( i != ok )
//             return notok;
//
//         i = CardMFCWrite16Bytes(25, &ErrCardInfo.block24_buf_to_restore[0]);
//         if ( i != ok )
//             return notok;
//     }
//     else
//     {       //第一次入卡处理
//         ////////////////////////////////////////
//         //卡内交易计数器处理
//         ////////////////////////////////////////
//         memcpy(&CardInfo.init_purchase_serial_num[0], block_buffer1+2, 2);              //初始交易序号
//         memcpy(&temp_int16u, block_buffer1+2, 2);                                                       //初始交易序号
//         if ( temp_int16u >= 0xFFFF )
//             temp_int16u = 0;
//         else
//             temp_int16u++;
//         memcpy(&CardInfo.purchase_serial_num[0], &temp_int16u, 2);                      //+1后的交易序号
//
//         memcpy(&ErrCardInfo.block24_buf_to_restore[0], block_buffer1, 16);              //凑24区应该写入的数据
//         memcpy(&ErrCardInfo.block24_buf_to_restore[2], &CardInfo.purchase_serial_num[0], 2);
//         memcpy(&ErrCardInfo.block24_buf_to_restore[12], "\x18\xE7\x18\xE7", 4);     //强制置为0x18E7
//         i = CardMFCWrite16Bytes(24, &ErrCardInfo.block24_buf_to_restore[0]);
//         if ( i != ok )
//             return notok;
//
//         i = CardMFCWrite16Bytes(25, &ErrCardInfo.block24_buf_to_restore[0]);
//         if ( i != ok )
//             return notok;
//
//     }
//     CardInfo.psam_purchase_serial_num = 0;//PSAM卡交易序号清零
//     //  CardInfo.card_purchase_type= 0x10;
//     //add hu
//     ////////////////////////////////////////
//     //根据卡类型算出消费金额
//     ////////////////////////////////////////
//     CardInfo.fare = GetFare(CardInfo.card_type);
//     //检查钱包金额
//     i=Check_Money();
//     if ( i!=ok )
//         return notok;
//     //金额够消费
//     ////////////////////////////////////////
//     //验证Sector 10 密钥
//     ////////////////////////////////////////
//     i = Get_Sector_PermissionA(10, &KeySector[10][0]);
//     if ( i != ok )
//         return notok;
//
//     i = CardMFCRead16Bytes(41, block_buffer1);
//     if ( i != ok )
//         return notok;
//
//     i = CardMFCRead16Bytes(42, block_buffer2);
//     if ( i != ok )
//         return notok;
//
//     i = Favor_Block_Is_Right(block_buffer1);
//     j = Favor_Block_Is_Right(block_buffer2);
//     if ( (i == ok) && (j == ok) )
//     {
//         //两区都合法，看两区数据是否一致
//         g = memcmp(block_buffer1, block_buffer2, 16);
//         if ( g == ok )
//         {
//             //两区一致，什么也不做
//             ;
//         }
//         else
//         {
//             //两区不一致，以41区为主
//             m = CardMFCWrite16Bytes(42,block_buffer1);
//             if ( m != ok )
//                 return notok;
//         }
//
//     }
//     else if ( i == ok )
//     {
//         // 41区合法，41->42
//         m = CardMFCWrite16Bytes(42,block_buffer1);
//         if ( m != ok )
//             return notok;
//     }
//     else if ( j == ok )
//     {
//         // 42区合法，42->41
//         m = CardMFCWrite16Bytes(41,block_buffer2);
//         if ( m != ok )
//             return notok;
//         memcpy(block_buffer1, block_buffer2, 16);//好的数据传给block_buffer1，下边处理数据用block_buffer1
//     }
//     else
//     {
//         //两区都不合法，退出
//         return notok;
//     }
//     i = memcmp(block_buffer1,&CardInfo.card_in_time.year, 3);       //p0+119是世纪位
//     //时间相等，扣除次数
//     if ( i == 0 )
//     {
//         //相等，扣一次
//         memcpy(&temp_balance, &block_buffer1[3], 2);
//         if ( temp_balance == 0 )
//         {
//             //优惠次数已扣光，当作普通卡消费
//             CardInfo.card_type= CARDT_NORMAL;
//             memcpy(&block_buffer1[5],&CardInfo.card_in_time, 7);    //写入当(上)次交易日期时间
//             memcpy(&block_buffer1[12], "\x29\xD6\x29\xD6", 4);//强制置为0x29d6
//             /////////////////////////////////////////
//             //扣次数处理
//             /////////////////////////////////////////
//             i = CardMFCWrite16Bytes(41,block_buffer1);
//             if ( i != ok )
//             {//扣次数出错处理，记录出错卡卡号和原次数余额
//                 memcpy(&ErrCardInfo.m1_card_number[0], &CardInfo._CARD_NUMBER.detail_card_number.serial_number[0], 4);  //保存出错M1卡卡号
//                 memcpy(&ErrCardInfo.csn[0], &CardInfo.csn[0], 4);               //保存出错M1卡硬件卡号
//                 ErrCardInfo.init_time_balance = CardInfo.init_time_balance;                 //保存出错M1卡原次数余额
//                 ErrCardInfo.error_occured = TRUE;
//                 DevStat.purchase_again_req = TRUE;      //申请重新刷卡语音
//                 DevStat.voice_purchase_again_freq=100;
//                 return notok;
//             }
//             i = CardMFCWrite16Bytes(42,block_buffer1);
//             if ( i != ok )
//             {   //扣款出错处理，记录出错卡卡号和原余额
//                 memcpy(&ErrCardInfo.m1_card_number[0], &CardInfo._CARD_NUMBER.detail_card_number.serial_number[0], 4);  //保存出错M1卡卡号
//                 memcpy(&ErrCardInfo.csn[0], &CardInfo.csn[0], 4);               //保存出错M1卡硬件卡号
//                 ErrCardInfo.init_time_balance = CardInfo.init_time_balance;                 //保存出错M1卡原余额
//                 ErrCardInfo.error_occured = TRUE;
//                 DevStat.purchase_again_req = TRUE;      //申请重新刷卡语音
//                 DevStat.voice_purchase_again_freq=100;
//
//                 return notok;
//             }
//         }
//         else
//         {
//             //还有优惠次数
//             temp_balance--;
//
// //          g_xunPreferential_Price_Num = preferential_times;
//
//             memcpy(&block_buffer1[3], &temp_balance, 2);        //优惠次数--后写回
//
//             memcpy(&block_buffer1[5], &CardInfo.card_in_time, 7);   //写入当(上)次交易日期时间
//             memcpy(&block_buffer1[12], "\x29\xD6\x29\xD6", 4);//强制置为0x29d6
//             /////////////////////////////////////////
//             //扣次数处理
//             /////////////////////////////////////////
//             i = CardMFCWrite16Bytes(41,block_buffer1);
//             if ( i != ok )
//             {//扣次数出错处理，记录出错卡卡号和原次数余额
//                 memcpy(&ErrCardInfo.m1_card_number[0], &CardInfo._CARD_NUMBER.detail_card_number.serial_number[0], 4);  //保存出错M1卡卡号
//                 memcpy(&ErrCardInfo.csn[0], &CardInfo.csn[0], 4);               //保存出错M1卡硬件卡号
//                 ErrCardInfo.init_time_balance = CardInfo.init_time_balance;                 //保存出错M1卡原次数余额
//                 ErrCardInfo.error_occured = TRUE;
//                 DevStat.purchase_again_req = TRUE;      //申请重新刷卡语音
//                 DevStat.voice_purchase_again_freq=100;
//                 return notok;
//             }
//             i = CardMFCWrite16Bytes(42,block_buffer1);
//             if ( i != ok )
//             {   //扣款出错处理，记录出错卡卡号和原余额
//                 memcpy(&ErrCardInfo.m1_card_number[0], &CardInfo._CARD_NUMBER.detail_card_number.serial_number[0], 4);  //保存出错M1卡卡号
//                 memcpy(&ErrCardInfo.csn[0], &CardInfo.csn[0], 4);               //保存出错M1卡硬件卡号
//                 ErrCardInfo.init_time_balance = CardInfo.init_time_balance;                 //保存出错M1卡原余额
//                 ErrCardInfo.error_occured = TRUE;
//                 DevStat.purchase_again_req = TRUE;      //申请重新刷卡语音
//                 DevStat.voice_purchase_again_freq=100;
//
//                 return notok;
//             }
//         }
//     }
//     else
//     {
//         //不相等，换月处理
//         memcpy(&block_buffer1[0], &CardInfo.card_in_time.year, 3);      //更新有效年月
//         switch ( CardInfo.card_type )
//         {
//             case 0x02:      //小学生卡
//                 memcpy(&temp_balance, &DevStat.student_a_card_count, 2);
//                 temp_balance--;
//                 memcpy(&block_buffer1[3], &temp_balance, 2);        //优惠次数--后写回
//                 break;
//
//             case 0x08:      //中学生卡
//                 memcpy(&temp_balance, &DevStat.student_b_card_count, 2);
//                 temp_balance--;
//                 memcpy(&block_buffer1[3], &temp_balance, 2);        //优惠次数--后写回
//                 break;
//
//             case 0x07:      //月票卡
//                 memcpy(&temp_balance, &DevStat.emplory_b, 2);
//                 temp_balance--;
//                 memcpy(&block_buffer1[3], &temp_balance, 2);        //优惠次数--后写回
//                 break;
//
//             default:
//                 return 0xFF;            //此情况应该不存在
//                 break;
//         }
//
//         //g_xunPreferential_Price_Num = preferential_times;
//         memcpy(&block_buffer1[5], &CardInfo.card_in_time, 7);   //写入当(上)次交易日期时间
//         memcpy(&block_buffer1[12], "\x29\xD6\x29\xD6", 4);//强制置为0x29d6
//         /////////////////////////////////////////
//         //扣次数处理
//         /////////////////////////////////////////
//         i = CardMFCWrite16Bytes(41,block_buffer1);
//         if ( i != ok )
//         {//扣次数出错处理，记录出错卡卡号和原次数余额
//             memcpy(&ErrCardInfo.m1_card_number[0], &CardInfo._CARD_NUMBER.detail_card_number.serial_number[0], 4);  //保存出错M1卡卡号
//             memcpy(&ErrCardInfo.csn[0], &CardInfo.csn[0], 4);               //保存出错M1卡硬件卡号
//             ErrCardInfo.init_time_balance = CardInfo.init_time_balance;                 //保存出错M1卡原次数余额
//             ErrCardInfo.error_occured = TRUE;
//             DevStat.purchase_again_req = TRUE;      //申请重新刷卡语音
//             DevStat.voice_purchase_again_freq=100;
//             return notok;
//         }
//         i = CardMFCWrite16Bytes(42,block_buffer1);
//         if ( i != ok )
//         {   //扣款出错处理，记录出错卡卡号和原余额
//             memcpy(&ErrCardInfo.m1_card_number[0], &CardInfo._CARD_NUMBER.detail_card_number.serial_number[0], 4);  //保存出错M1卡卡号
//             memcpy(&ErrCardInfo.csn[0], &CardInfo.csn[0], 4);               //保存出错M1卡硬件卡号
//             ErrCardInfo.init_time_balance = CardInfo.init_time_balance;                 //保存出错M1卡原余额
//             ErrCardInfo.error_occured = TRUE;
//             DevStat.purchase_again_req = TRUE;      //申请重新刷卡语音
//             DevStat.voice_purchase_again_freq=100;
//
//             return notok;
//         }
//         return 0xFD;
//     }
//     //按优惠金额扣钱
//     i=M1_Card_Consume_Process();
//     if ( i!=ok )
//         return notok;
// 	return ok;
// }

INT8U CardMFCValueWithTransfer(int flag, int s_block, int d_block, ulong num)
{
	INT8U ucResult;

	if(flag == 0)//decrement
	{
		ucResult = EA_ucMIFDecrement (hMifsHandle, s_block,num);
		if (ucResult != EM_SUCCESS)
		{
			return notok;
		}
		/* 将递增后第14块内容备份到第13块中，这样，第13块也成为数值块，数值为125*/
		ucResult = EA_ucMIFTransfer (hMifsHandle, d_block);
		if (ucResult != EM_SUCCESS)
		{
			return notok;
		}
	}
	if(flag == 1)//increment
	{
		ucResult = EA_ucMIFIncrement(hMifsHandle, s_block,num);
		if (ucResult != EM_SUCCESS)
		{
			return notok;
		}
		/* 将递增后第14块内容备份到第13块中，这样，第13块也成为数值块，数值为125*/
		ucResult = EA_ucMIFTransfer (hMifsHandle, d_block);
		if (ucResult != EM_SUCCESS)
		{
			return notok;
		}
	}
	return ok;
}
/******************************************************************************
 函数名称：Cal_BCC
 功能描述：对接收到的数据进行BCC校验
 参数描述：
 参数名称：	输入/输出？	类型		描述
 buf		输入		const INT8U*		要进行BCC计算的数据缓冲区
 len		输入		INT16U		长度
				
 返  回  值：计算出来的BCC值
				   
 作      者	：许岩
 日      期：2004-09-02
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
******************************************************************************/
INT8U Cal_BCC_Hex(const INT8U *buf, INT16U len)
{
	INT16U LOCAL_V_TYPE i = 0;
	INT8U LOCAL_V_TYPE bcc = 0;

	for ( i=0; i<len; i++ )
	{
		bcc ^= buf[i];
	}

	return bcc;
}
/*****************************************************************
 函数原型：M1_Card_Consume_Process
 功能描述：M1消费卡扣款流程
 参数描述：
 参数名称：	输入/输出？	类型		描述
				
				
 返  回  值：	ok(0)-消费成功		
				0xFF-消费失败
				//0xFC-不是本系统卡
				//0xFD-有卡出错，但没有回该卡
				0xF0-重新刷卡
 作      者	：许岩
 日      期：2004-09-27
 修改历史：
		日期		修改人		修改描述
		2005.2.21		myron			in function
*****************************************************************/
INT8U M1_Card_Consume_Process(void)
{
	char    input[22], buf[22];
	INT16U  LOCAL_V_TYPE    temp_int16u;
	uchar   i;
	float   temp_fare;

	memset(buf, 0, sizeof(buf));

	//消费记录区处理
	EA_vCls();
	EA_ucClrKeyBuf();
	Beep(2500);
	strcpy((void *)&CardInfo.bus_number[0], "12345");
	EA_vDisplay(1, "车  号:鲁C-%s", &CardInfo.bus_number[0] );

	EA_ucSetInverse(EM_lcd_INVOFF);

	if( DevStat.fill_mode == 0 )
	{
		EA_vDisplay(2, "天然气:        升(L)");
	}
	else if( DevStat.fill_mode == 1 )
	{
		EA_vDisplay(2, "柴油量:        升(L)");
	}
	else if( DevStat.fill_mode == 2 )
	{
		EA_vDisplay(2, "汽油量:        升(L)");
	}
	else
	{
		return notok;
	}
	strcpy((void *)input, "");

	for ( ;; )
	{
		i = get_input_str(2, 8, 1, 6, (void *)input);
		if ( i == EM_SUCCESS )
			break;
		if ( i == EM_ABOLISH )
			return notok;
	}

	sscanf((void *)input, "%f", &temp_fare);
	
	if( DevStat.fill_mode == 0 )
	{
		sprintf((void *)buf, "加油量:%4.2f升(L)", temp_fare);
	}
	else
	{
		sprintf((void *)buf, "加气量:%4.2f升(L)", temp_fare);
	}

	CardInfo.fare = temp_fare * 100;

	lcddisp(2, 1, (void *)buf);

//  EA_ucSetInverse(EM_lcd_INVOFF);

	if ( (CardInfo.fare < 1) || (CardInfo.fare > 50000 ) )
	{
		//余额不足处理
		lcddisperr((char *)"输入的升数错误!");
		memset(&CardInfo, 0x00, sizeof(CardInfo));      	//清卡片交易信息
		memset(&ErrCardInfo, 0x00, sizeof(ErrCardInfo));    //清出错卡信息
		DevStat.purchase_again_req = FALSE;					//清重新刷卡语音标志
		DevStat.voice_purchase_again_freq = 0;
		return notok;
	}
//  CardInfo.balance = CardInfo.init_balance - CardInfo.fare;//储存交易后余额
	CardInfo.card_purchase_type = TT_NORMAL_CONSUME;		 //普通卡正常交易
	//保存设备交易序号	
	temp_int16u = ((INT16U)DevStat.dev_trade_no[0] << 8) | (INT16U)DevStat.dev_trade_no[1]; 	//初始交易序号
	if ( temp_int16u >= 0xFFFF )
		temp_int16u = 0;
	else
		temp_int16u++;
	DevStat.dev_trade_no[0] = (INT8U)(temp_int16u >> 8);						//+1后的交易序号
	DevStat.dev_trade_no[1] = (INT8U)temp_int16u;

	WriteParam();

	return ok;
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
INT8U Fuzha_Sec6_Block_Is_Legal(const INT8U * data_to_verify)
{
	INT8U LOCAL_V_TYPE i = 0;
	INT8U LOCAL_V_TYPE bcc = 0;

//  ToggleWD();

	if (memcmp(data_to_verify, "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF", 16) == 0)
		return notok;

	if (memcmp(data_to_verify, "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 16) == 0)
			return notok;

	for ( i=0; i<15; i++ )
	{
		bcc ^= *(data_to_verify + i);
	}
	if ( bcc==*(data_to_verify + 15) )
	{
		return ok;
	}
	else
	{
		return notok;
	}
}
///*****************************************************************
// 函数原型：M1_Card_Consume_ProcessA
// 功能描述：M1消费卡扣款流程
// 参数描述：
// 参数名称：	输入/输出？	类型		描述
//				
//				
// 返  回  值：	ok(0)-消费成功		
//				0xFF-消费失败
//				//0xFC-不是本系统卡
//				//0xFD-有卡出错，但没有回该卡
//				0xF0-重新刷卡
// 作      者	：许岩
// 日      期：2004-09-27
// 修改历史：
//		日期		修改人		修改描述
//		2005.2.21		myron			in function
//*****************************************************************/
//INT8U M1_Card_Consume_ProcessA(void)
//{
//	
//	INT8U   LOCAL_V_TYPE    i = 0;
//	INT8U   LOCAL_V_TYPE    j = 0;
//	INT8U   LOCAL_V_TYPE    m = 0;
//	INT16U  LOCAL_V_TYPE    temp_int16u;
//	INT8S   LOCAL_V_TYPE    g = 0;
//	INT8U   LOCAL_V_TYPE    block_buffer1[16] = {0};
//	INT8U   LOCAL_V_TYPE    block_buffer2[16] = {0};
//	INT8U   LOCAL_V_TYPE    block9[16];
//	INT8U   LOCAL_V_TYPE    block10[16];
//	float   zero=0;
//	//float temp_data;
//	//INT8U 	const  			zero[4] = {0,0,0,0};
//	INT32U  LOCAL_V_TYPE    temp_balance;
////     INT8U   LOCAL_V_TYPE    zero_balance_counter ;
//
//
//
//	////////////////////////////////////////
//	//验证Sector 6 密钥
//	////////////////////////////////////////
////  if(CardInit() != ok)
////  	return notok;
//	
//	//i = Get_Sector_PermissionA(6, &KeySector[6][0]);
//	i = Get_Sector_PermissionA(6, &KeySector[6][0]);
//	if ( i != ok )
//		return notok;
//
//	i = EA_ucMIFRead(hMifsHandle, 24, block_buffer1);
//	if ( i != ok )
//		return notok;
//
//	i = EA_ucMIFRead(hMifsHandle, 25, block_buffer2);
//	if ( i != ok )
//		return notok;
//
//	i = Sec6_Block_Is_Legal(block_buffer1);
//	j = Sec6_Block_Is_Legal(block_buffer2);
//	if ( (i == ok) && (j == ok) )
//	{
//		//两区都合法，看两区数据是否一致
//		g = memcmp(block_buffer1, block_buffer2, 16);
//		if ( g == ok )
//		{
//			//两区一致，什么也不做
//			;
//		}
//		else
//		{
//			//两区不一致，以24区为主
//			m = EA_ucMIFWrite(hMifsHandle, 25, block_buffer1);
//			if ( m != ok )
//				return notok;
//		}
//
//	}
//	else if ( i == ok )
//	{
//		//24区合法，24->25
//		m = EA_ucMIFWrite(hMifsHandle, 25, block_buffer1);
//		if ( m != ok )
//			return notok;
//	}
//	else if ( j == ok )
//	{
//		//25区合法，25->24
//		m = EA_ucMIFWrite(hMifsHandle, 24, block_buffer2);
//		if ( m != ok )
//			return notok;
//		memcpy(block_buffer1, block_buffer2, 16);//好的数据传给block_buffer1，下边处理数据用block_buffer1
//	}
//	else
//	{
//		//两区都不合法，退出
//		return notok;
//	}
//
//	////////////////////////////////////////
//	//判断此卡是否已经置黑名单标志
//	////////////////////////////////////////
//	if ( block_buffer1[7] == 0x04 )
//	{
//		printf_debug((char*)"非法卡");
//		return notok;
//	}
//
//
//	CardInfo.credit_rec_pos = block_buffer1[8];
//		
//	if(CardInfo.credit_rec_pos == 0x00 ||CardInfo.credit_rec_pos == 0xFF)
//		CardInfo.credit_rec_pos = 0x0C;
//	else
//		CardInfo.credit_rec_pos ++;
//
//	if(CardInfo.credit_rec_pos==0x0f ||CardInfo.credit_rec_pos==0x13 )
//		CardInfo.credit_rec_pos ++;
//	if(CardInfo.credit_rec_pos>=0x17)
//		CardInfo.credit_rec_pos=0x0C;
//
//	////////////////////////////////////////////////////////
//	//出错判断
//	////////////////////////////////////////////////////////
//	if ( ErrCardInfo.error_occured == TRUE )							  //保证了交易序号可断不可重，交易序号为数据库中主键之一
//	{
//		//出错卡重入
//		memcpy(&ErrCardInfo.block24_buf_to_restore[12], "\x18\xE7\x18\xE7", 4);		//强制置为0x18E7
//		i = EA_ucMIFWrite(hMifsHandle, 24, &ErrCardInfo.block24_buf_to_restore[0]);
//		if ( i != ok )
//			return notok;
//
//		i = EA_ucMIFWrite(hMifsHandle, 25, &ErrCardInfo.block24_buf_to_restore[0]);
//		if ( i != ok )
//			return notok;
//	}
//	else
//	{		//第一次入卡处理 
//		////////////////////////////////////////
//		//卡内交易计数器处理
//		////////////////////////////////////////
//		CardInfo.init_purchase_serial_num[0] = block_buffer1[0];				//初始交易序号
//		CardInfo.init_purchase_serial_num[1] = block_buffer1[1];
//		temp_int16u = ((INT16U)block_buffer1[0] << 8) | (INT16U)block_buffer1[1];				//初始交易序号
//		if ( temp_int16u >= 0xFFFF )
//			temp_int16u = 0;
//		else
//			temp_int16u++;
////  	EA_vDisplay(1, "%02x", block_buffer1[0]);
////  	EA_vDisplay(2, "%02x", block_buffer1[1]);
////  	EA_vDisplay(2, "%02d", temp_int16u);
////  	EA_uiInkey(0);
//
//		CardInfo.purchase_serial_num[0] = (INT8U)(temp_int16u / 0x100 );						//+1后的交易序号
//		CardInfo.purchase_serial_num[1] = (INT8U)(temp_int16u % 0x100 );
//
//		memcpy(&ErrCardInfo.block24_buf_to_restore[0], block_buffer1, 16);				//凑24区应该写入的数据
//		memcpy(&ErrCardInfo.block24_buf_to_restore[0], &CardInfo.purchase_serial_num[0], 2);
//		memcpy(&ErrCardInfo.block24_buf_to_restore[12], "\x18\xE7\x18\xE7", 4);		//强制置为0x18E7
//		i = EA_ucMIFWrite(hMifsHandle, 24, &ErrCardInfo.block24_buf_to_restore[0]);
//		if ( i != ok )
//			return notok;
//
//		i = EA_ucMIFWrite(hMifsHandle, 25, &ErrCardInfo.block24_buf_to_restore[0]);
//		if ( i != ok )
//			return notok;
//
//	}
//
//	CardInfo.psam_purchase_serial_num = 0;										//PSAM卡交易序号清零
//
//	////////////////////////////////////////
//	//验证Sector 2 密钥
//	////////////////////////////////////////
//	if(CardInit() !=ok)
//		return notok;
//	
//	//i = Get_Sector_PermissionA(2, &KeySector[2][0]);
//	i = Get_Sector_PermissionA(2, &KeySector[2][0]);
//	if ( i != ok )
//		return notok;
//
//	i =EA_ucMIFRead(hMifsHandle, 9, block9);
//	if ( i != ok )
//		return notok;
//
//	i = EA_ucMIFRead(hMifsHandle, 10, block10);
//	if ( i != ok )
//		return notok;
//
//	i = Purse_Block_Is_Right(block9);
//	j = Purse_Block_Is_Right(block10);
//	if ( (i == ok) && (j == ok) )
//	{	//两区都合法，看两区数据是否一致
//		g = memcmp(block9, block10, 16);
//		if ( g == ok )
//		{
//			;			//两区一致，什么也不做
//		}
//		else
//		{
//			//两区不一致，以9块为主
//			m = CardMFCValueWithTransfer(PICC_DECREMENT, 9, 10,zero);	  //两区不一致，以9块为主
//			if ( m != ok )
//				return notok;
//		}
//	}
//	else if ( i == ok )
//	{
////         m = Write(10, block9);              //9区合法，9->10
//		m = CardMFCValueWithTransfer(PICC_DECREMENT, 9, 10, zero);	   //9区合法，9->10
//		if ( m != ok )
//			return notok;
//	}
//	else if ( j == ok )
//	{
////         m = Write(9, block10);          //10区合法，10->9
//		m = CardMFCValueWithTransfer(PICC_DECREMENT, 10, 9, zero);	  //10区合法，10->9
//		if ( m != ok )
//			return notok;
//		memcpy(block9, block10, 16);		//好的数据传给block9下边处理数据用block9
//	}
//	else
//	{
//		return notok;		//两区都不合法，退出
//	}
//
//
//
//	////////////////////////////////////////
//	//出错卡扣款处理
//	////////////////////////////////////////
//	if ( ErrCardInfo.error_occured == TRUE )
//	{	//出错卡回卡处理，分两种情况，一种是已经扣过款了，令一种是还没扣过款
//		temp_balance = BlockToCash(block9);
//		if ( temp_balance == ErrCardInfo.init_balance )
//		{	//扣款没成功
//			//往下接着走第一次扣款处理
//		}
//		else
//		{	//扣款成功(包括其他情况，也算扣款成功)
//			CardInfo.balance = temp_balance;//储存交易后余额
//
//			//if ( (DevStat.oldman_free == TRUE && CardInfo.card_type == CARDT_OLDMAN)
//			//	 || (DevStat.employee_a_free == TRUE && CardInfo.card_type == CARDT_FREE)
//			//	 || (DevStat.driver_free == TRUE && CardInfo.card_type == CARDT_DRIVER) )
//			//{
//			//	CardInfo.card_purchase_type = TT_FREE;	//免费卡交易
//			//}
//			
//			//else
//			//{
//				CardInfo.card_purchase_type = TT_NORMAL_CONSUME_MONEY;//普通卡正常交易
//			//}
//
//			return ok;
//		}
//	}
//
//	////////////////////////////////////////
//	//第一次入卡扣款处理
//	////////////////////////////////////////
//	if ( ErrCardInfo.error_occured != TRUE )			//只有在第一次入卡时，才保存初始余额，以后雷打不动
//	{
//		CardInfo.init_balance = BlockToCash(block9);
//	}
//
//
//	if ( CardInfo.init_balance < CardInfo.fare )
//	{
//		//余额不足处理
//		printf_debug((char *)"余额不足请充值");
//		memset(&CardInfo, 0x00, sizeof(CardInfo));		//清卡片交易信息		
//		memset(&ErrCardInfo, 0x00, sizeof(ErrCardInfo));	//清出错卡信息
//		DevStat.purchase_again_req = FALSE;//清重新刷卡语音标志
//		DevStat.voice_purchase_again_freq=0;
//		return notok;
//	}
//
//	if( ErrCardInfo.error_occured != TRUE)
//		StoreGrayRecord();
//
//// 	SleepMs(500);
////	i = CardMFCValueWithTransfer(PICC_DECREMENT, 9, 9,CashToBlock(CardInfo.fare));
//	i = CardMFCValueWithTransfer(PICC_DECREMENT, 9, 9,CardInfo.fare);
//
//	if ( i != ok )
//	{	//扣款出错处理，记录出错卡卡号和原余额
//		memcpy(&ErrCardInfo.m1_card_number[0], &CardInfo._CARD_NUMBER.detail_card_number.serial_number[0], 4);	//保存出错M1卡卡号
//		memcpy(&ErrCardInfo.csn[0], &CardInfo.csn[0], 4);				//保存出错M1卡硬件卡号
//		ErrCardInfo.init_balance = CardInfo.init_balance;					//保存出错M1卡原余额
//		ErrCardInfo.error_occured = TRUE;
//		DevStat.purchase_again_req = TRUE;		//申请重新刷卡语音
//		DevStat.voice_purchase_again_freq=100;
//// 		return notok;
//		return 0xF0;
//	}
//	i = CardMFCValueWithTransfer(PICC_DECREMENT, 9, 10, 0);
//	if ( i != ok )
//	{	//扣款出错处理，记录出错卡卡号和原余额
//		memcpy(&ErrCardInfo.m1_card_number[0], &CardInfo._CARD_NUMBER.detail_card_number.serial_number[0], 4);	//保存出错M1卡卡号
//		memcpy(&ErrCardInfo.csn[0], &CardInfo.csn[0], 4);				//保存出错M1卡硬件卡号
//		ErrCardInfo.init_balance = CardInfo.init_balance;					//保存出错M1卡原余额
//		ErrCardInfo.error_occured = TRUE;
//		DevStat.purchase_again_req = TRUE;		//申请重新刷卡语音
//		DevStat.voice_purchase_again_freq=100;
//// 		return notok;
//		return 0xF0;
//	}
//
//	CardInfo.balance = CardInfo.init_balance - CardInfo.fare;//储存交易后余额
////	CardInfo.card_purchase_type = TT_NORMAL_CONSUME_MONEY;	//冲帐交易类型
//	CardInfo.card_purchase_type = TT_NORMAL_CONSUME_MONEY;//普通卡正常交易
//
////保存设备交易序号
//	
//	temp_int16u = ((INT16U)DevStat.dev_trade_no[0] << 8) | (INT16U)DevStat.dev_trade_no[1]; 			//初始交易序号
//	if ( temp_int16u >= 0xFFFF )
//		temp_int16u = 0;
//	else
//		temp_int16u++;
//	DevStat.dev_trade_no[0] = (INT8U)(temp_int16u / 0x100);						//+1后的交易序号
//	DevStat.dev_trade_no[1] = (INT8U)(temp_int16u % 0x100);
//	WriteParam();
//
//	return ok;
//}
/*****************************************************************
 函数原型：Lock_M1_Card_Process
 功能描述：M1卡黑名单在中，锁卡处理流程
 参数描述：
 参数名称：	输入/输出？	类型		描述
				
				
 返  回  值：	ok(0)-消费成功		
				0xFF-消费失败
 作      者	：许岩
 日      期：2003-12-19
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
*****************************************************************/
INT8U Lock_M1_Card_Process(void)
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
	if(CardInit() != ok)
	{
		EA_ucMIFHalt(hMifsHandle);	//挂卡
		return notok;
	}
	
	i = Get_Sector_PermissionB(6, &KeySector[6][0]);
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
			;//两区一致，什么也不做
		}
		else
		{
			//两区不一致，以24区为主
			m = CardMFCWrite16Bytes(25,block_buffer1);
			if ( m != ok )
				return notok;
		}

	}
	else if ( i == ok )
	{
		//24区合法，24->25
		m = CardMFCWrite16Bytes(25,block_buffer1);
		if ( m != ok )
			return notok;
	}
	else if ( j == ok )
	{
		//25区合法，25->24
		m = CardMFCWrite16Bytes(24,block_buffer2);
		if ( m != ok )
			return notok;
		memcpy(block_buffer1, block_buffer2, 16);		//好的数据传给block_buffer1，下边处理数据用block_buffer1
	}
	else
	{
		return notok;		//两区都不合法，退出
	}

	////////////////////////////////////////
	//判断此卡是否已经置黑名单标志
	////////////////////////////////////////
	if ( block_buffer1[7] == 0x04 )
	{
//  	Beep(500);
		printf_debug((char *)"非法卡");
		Beeperr();	
		return notok;
	}

	////////////////////////////////////////
	//将黑名单标志位写为0x04
	////////////////////////////////////////
	block_buffer1[7] = 0x04;
	i = CardMFCWrite16Bytes(24, block_buffer1);
	if ( i != ok )
		return notok;

	i = CardMFCWrite16Bytes(25, block_buffer1);

	
	CardInfo.psam_purchase_serial_num = 0;//PSAM卡交易序号清零
	CardInfo.balance = 0;//交易后余额清零
	CardInfo.fare = 0;//交易金额清零
	memset(&CardInfo.purchase_serial_num[0], 0x00, 2);//交易计数器清零  
	memset(&CardInfo.tac[0], 0x00, 4);		//tac码清零
	CardInfo.card_purchase_type = TT_BLACKNAME_CONSUME_TYPE;	//交易类型为黑名单交易
	(void)StoreRecord(&CardInfo);			//储存售卡记录
	////////////////////////////////////////
	//M1卡挂卡
	////////////////////////////////////////
	j = EA_ucMIFHalt(hMifsHandle);	//挂卡

	return ok;
}

/******************************************************************************
 函数名称：ICCardProcess
 功能描述：卡交易流程
 参数描述：
 参数名称：	输入/输出？	类型		描述
 
 返  回  值：ok(0)-成功
				 notok(0xFF)-失败
				 0xFE-没找到卡
				 0xF0-重新刷卡
				   
 作      者	：许岩
 日      期：2004-09-02
 修改历史：
		日期		修改人		修改描述
		2005.2.18		myron	                   in function 
******************************************************************************/
INT8U ICCardProcess(void)
{
//     INT8U LOCAL_V_TYPE  j =0;
	INT8U LOCAL_V_TYPE  i = 0;  
	unsigned short  ICType;
	ulong ICSnr;
	uint  puiSize;

	i = EA_ucMIFRequest(hMifsHandle, EM_mifs_IDLE, &ICType);
	if ( i != EM_SUCCESS )
	{
		return 0xFE;
	}
	i = EA_ucMIFAntiColl(hMifsHandle,&ICSnr);
	if ( i != EM_SUCCESS )
	{
		return notok;
	}
	EA_ucMIFSelect(hMifsHandle,ICSnr, &puiSize);
	if ( i != EM_SUCCESS )
	{
		return notok;
	}

	i = M1CardProcess();
	if ( i != ok )
		return i;

	memset(&CardInfo, 0x00, sizeof(CardInfo));		//清卡片交易信息
	memset(&ErrCardInfo, 0x00, sizeof(ErrCardInfo));	//清出错卡信息
	DevStat.purchase_again_req = FALSE;					//清重新刷卡语音标志
	DevStat.voice_purchase_again_freq=FALSE;
	return ok;
}
//
///*****************************************************************
// 函数原型：M1_Card_Credit_Process
// 功能描述：M1消费卡充值流程
// 参数描述：
// 参数名称： 输入/输出？ 类型        描述
//
//
// 返  回  值：   ok(0)-充值成功
//                0xFF-充值失败
//                //0xFC-不是本系统卡
//                //0xFD-有卡出错，但没有回该卡
//                0xF0-重新刷卡
// 作      者 ：许岩
// 日      期：2004-09-27
// 修改历史：
//        日期        修改人      修改描述
//        2005.2.21       myron           in function
//*****************************************************************/
//INT8U M1_Card_Credit_Process(void)
//{
//    INT8U   LOCAL_V_TYPE    i = 0;
//    INT8U   LOCAL_V_TYPE    j = 0;
//    INT8U   LOCAL_V_TYPE    m = 0;
//    INT16U  LOCAL_V_TYPE    temp_int16u;
//    INT8S   LOCAL_V_TYPE    g = 0;
//    INT8U   LOCAL_V_TYPE    block_buffer1[16] = {0};
//    INT8U   LOCAL_V_TYPE    block_buffer2[16] = {0};
//    INT8U   LOCAL_V_TYPE    block9[16];
//    INT8U   LOCAL_V_TYPE    block10[16];
//    ulong   zero = 0;
//    INT32U  LOCAL_V_TYPE    temp_balance;
//    int     ret = 0;
//
////	uchar   ucResult;
//
//    ////////////////////////////////////////
//    //验证Sector 6 密钥
//    ////////////////////////////////////////
//
//
////  if( CardInit() != ok )
////  {
////  	EA_ucMIFHalt(hMifsHandle);	//挂卡
////  	return notok;
////  }
//
//    i = Get_Sector_PermissionB( 6, &KeySector[6][0] );
//    if ( i != ok )
//    {
//        lcddisperr("no 1!");
//        return notok;
//    }
//
//    i = EA_ucMIFRead( hMifsHandle, 24, block_buffer1 );
//    if ( i != ok )
//    {
//        lcddisperr("no 2!");
//        return notok;
//    }
//    i = EA_ucMIFRead(hMifsHandle, 25, block_buffer2);
//    if ( i != ok )
//    {
//        lcddisperr("no 3!");
//        return notok;
//    }
//
//    i = Sec6_Block_Is_Legal(block_buffer1);
//    j = Sec6_Block_Is_Legal(block_buffer2);
//
//    if ( ( i == ok ) && ( j == ok ) )
//    {
//        //两区都合法，看两区数据是否一致
//        g = memcmp(block_buffer1, block_buffer2, 16);
//        if ( g == ok )
//        {
//            //两区一致，什么也不做
//            ;
//        }
//        else
//        {
//            //两区不一致，以24区为主
////  		m = CardMFCWrite16Bytes(25,block_buffer1);
//            m = EA_ucMIFWrite( hMifsHandle, 25, block_buffer1 );
//            if ( m != ok )
//                return notok;
//        }
//
//    }
//    else if ( i == ok )
//    {
//        //24区合法，24->25
//        m = EA_ucMIFWrite( hMifsHandle, 25, block_buffer1 );
//        if ( m != ok )
//            return notok;
//    }
//    else if ( j == ok )
//    {
//        //25区合法，25->24
//        m = EA_ucMIFWrite( hMifsHandle, 24, block_buffer2 );
//        if ( m != ok )
//            return notok;
//        memcpy(block_buffer1, block_buffer2, 16);  //好的数据传给block_buffer1，下边处理数据用block_buffer1
//    }
//    else
//    {
//        //两区都不合法，退出
//        EA_ucMIFHalt(hMifsHandle);  //挂卡
//        return notok;
//    }
//
//    ////////////////////////////////////////
//    //判断此卡是否已经置黑名单标志
//    ////////////////////////////////////////
//    if ( block_buffer1[7] == 0x04 )
//    {
//        lcddisperr("非法卡");
////  	EA_ucMIFHalt();	//挂卡
//        (void)EA_ucMIFHalt(hMifsHandle);
//        return notok;
//    }
//
//    CardInfo.credit_rec_pos = block_buffer1[8];
//
//    if(CardInfo.credit_rec_pos == 0x00 ||CardInfo.credit_rec_pos == 0xFF)
//        CardInfo.credit_rec_pos = 0x0C;
//    else
//        CardInfo.credit_rec_pos ++;
//
//    if(CardInfo.credit_rec_pos == 0x0f ||CardInfo.credit_rec_pos == 0x13 )
//        CardInfo.credit_rec_pos ++;
//    if(CardInfo.credit_rec_pos >= 0x17)
//        CardInfo.credit_rec_pos = 0x0C;
//
//
//    CardInfo.card_purchase_type = TT_CREDIT_MONEY;  //充值交易类型
//    CardInfo.balance = CardInfo.init_balance + CardInfo.fare;//储存交易后余额
//
//    ////////////////////////////////////////////////////////
//    //出错判断
//    ////////////////////////////////////////////////////////
//    if ( ErrCardInfo.error_occured == TRUE )                              //保证了交易序号可断不可重，交易序号为数据库中主键之一
//    {
//        //出错卡重入
//        memcpy(&ErrCardInfo.block24_buf_to_restore[12], "\x18\xE7\x18\xE7", 4);     //强制置为0x18E7
//        i = EA_ucMIFWrite(hMifsHandle, 24, &ErrCardInfo.block24_buf_to_restore[0]);
//        if ( i != ok )
//            return notok;
//
//        i = EA_ucMIFWrite(hMifsHandle, 25, &ErrCardInfo.block24_buf_to_restore[0]);
//        if ( i != ok )
//            return notok;
//    }
//    else
//    {
//        //第一次入卡处理
//        ////////////////////////////////////////
//        //卡内交易计数器处理
//        ////////////////////////////////////////
//        CardInfo.init_purchase_serial_num[0] = block_buffer1[0];                //初始交易序号
//        CardInfo.init_purchase_serial_num[1] = block_buffer1[1];
//        temp_int16u = ((INT16U)block_buffer1[0] << 8) | (INT16U)block_buffer1[1];               //初始交易序号
//        if ( temp_int16u >= 0xFFFF )
//            temp_int16u = 0;
//        else
//            temp_int16u ++;
//        CardInfo.purchase_serial_num[0] = (INT8U)(temp_int16u / 0x100);                     //+1后的交易序号
//        CardInfo.purchase_serial_num[1] = (INT8U)(temp_int16u % 0x100);
//
//        memcpy(&ErrCardInfo.block24_buf_to_restore[0], block_buffer1, 16);              //凑24区应该写入的数据
//        memcpy(&ErrCardInfo.block24_buf_to_restore[0], &CardInfo.purchase_serial_num[0], 2);
//        ErrCardInfo.block24_buf_to_restore[8] = CardInfo.credit_rec_pos;
//        memcpy(&ErrCardInfo.block24_buf_to_restore[12], "\x18\xE7\x18\xE7", 4);     //强制置为0x18E7
//        i = EA_ucMIFWrite(hMifsHandle, 24, &ErrCardInfo.block24_buf_to_restore[0]);
//        if ( i != ok )
//            return notok;
//
//        i = EA_ucMIFWrite(hMifsHandle, 25, &ErrCardInfo.block24_buf_to_restore[0]);
//        if ( i != ok )
//            return notok;
//    }
//
////保存设备交易序号
//
//    temp_int16u = ((INT16U)DevStat.dev_trade_no[0] << 8) | (INT16U)DevStat.dev_trade_no[1];             //初始交易序号
//    if ( temp_int16u >= 0xFFFF )
//        temp_int16u = 0;
//    else
//        temp_int16u++;
//    DevStat.dev_trade_no[0] = (INT8U)(temp_int16u / 0x100 );                        //+1后的交易序号
//    DevStat.dev_trade_no[1] = (INT8U)(temp_int16u % 0x100 );
//    WriteParam();
//
//
//    memcpy(&CardInfo.dev_trade_no[0],&DevStat.dev_trade_no[0], 2);              //PSAM卡交易序号清零
//
//    CardInfo.psam_purchase_serial_num = 0;
//
//
//    ////////////////////////////////////////
//    //验证Sector 2 密钥
//    ////////////////////////////////////////
//
////  if( CardInit() != ok )
////  {
////  	return notok;
////  }
//
//    i = Get_Sector_PermissionB(2, &KeySector[2][0]);
//    if ( i != ok )
//    {
//        lcddisperr("no 3");
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
//        g = memcmp( block9, block10, 16 );
//        if ( g == ok )
//        {
//            ;           //两区一致，什么也不做
//        }
//        else
//        {
//            m = CardMFCValueWithTransfer(1, 9, 10, zero);
//            if ( m != ok )
//                return notok;
//        }
//    }
//    else if ( i == ok )
//    {
//        m = CardMFCValueWithTransfer(1, 9, 10, zero);      //9区合法，9->10
//        if ( m != ok )
//            return notok;
//    }
//    else if ( j == ok )
//    {
//        m = CardMFCValueWithTransfer(1, 10, 9, zero);     //10区合法，10->9
//        if ( m != ok )
//            return notok;
//
//        memcpy(block9, block10, 16);        //好的数据传给block9下边处理数据用block9
//    }
//    else
//    {
//        EA_ucMIFHalt(hMifsHandle);  //挂卡
//        return notok;       //两区都不合法，退出
//    }
//
//    ////////////////////////////////////////
//    //出错卡扣款处理
//    ////////////////////////////////////////
//    if ( ErrCardInfo.error_occured == TRUE )
//    {   //出错卡回卡处理，分两种情况，一种是已经扣过款了，令一种是还没扣过款
//        temp_balance = BlockToCash(block9);
//        if ( temp_balance == ErrCardInfo.init_balance )
//        {   //扣款没成功
//            //往下接着走第一次扣款处理
//            //判断操作员大卡金额是否发生变化
//        }
//        else
//        {   //扣款成功(包括其他情况，也算扣款成功)
//            CardInfo.balance = temp_balance;//储存交易后余额
//            CardInfo.card_purchase_type = TT_CREDIT_MONEY;  //免费卡交易
//
////			if ( (DevStat.oldman_free == TRUE && CardInfo.card_type == CARDT_OLDMAN)
////				 || (DevStat.employee_a_free == TRUE && CardInfo.card_type == CARDT_FREE)
////				 || (DevStat.driver_free == TRUE && CardInfo.card_type == CARDT_DRIVER) )
////			{
////				CardInfo.card_purchase_type = TT_FREE;	//免费卡交易
////			}
////			else
////			{
////				CardInfo.card_purchase_type = TT_NORMAL_CONSUME_MONEY;//普通卡正常交易
////			}
//
//            return ok;
//        }
//    }
//
//    ////////////////////////////////////////
//    //第一次入卡扣款处理
//    ////////////////////////////////////////
//    if ( ErrCardInfo.error_occured == FALSE )           //只有在第一次入卡时，才保存初始余额，以后雷打不动
//    {
//        CardInfo.init_balance = BlockToCash(block9);
//    }
//
//    if ( ( CardInfo.init_balance + CardInfo.fare ) > DevStat.max_card_limit)
//    {
//
//        //余额不足处理
//        lcddisperr("余额超过上限");
//        memset(&CardInfo, 0x00, sizeof(CardInfo));      //清卡片交易信息
//        memset(&ErrCardInfo, 0x00, sizeof(ErrCardInfo));    //清出错卡信息
//        DevStat.purchase_again_req = FALSE;//清重新刷卡语音标志
//        DevStat.voice_purchase_again_freq = 0;
//        EA_ucMIFHalt(hMifsHandle);  //挂卡
//        return notok;
//    }
//
//    if ( ErrCardInfo.error_occured == FALSE )           //只有在第一次入卡时，才保存初始余额，以后雷打不动
//    {
//        StoreGrayRecord();
//    }
//    //扣除操作员卡内金额
////  Cpucard_Init();
//    ret = OperTradeProcess(0, CardInfo.fare);
//    if(ret != ok )
//    {
//        Cpucard_power_off();
//        ErrCardInfo.error_occured = TRUE;
//        lcddisperr("大卡错误不能交易");
////  	MC530Off();
////  	InitMC530(TYPE_A_ISO14443);
//        EA_ucMIFHalt(hMifsHandle);  //挂卡
//        return 0xF0;
//    }
////  Cpucard_power_off();
////	}
//
//// 	SleepMs(500);
////	i = EA_ucMIFTransfer(PICC_INCREMENT, 9, 9,CardInfo.fare);
////  i = EA_ucMIFIncrement(hMifsHandle, 9, CardInfo.fare);
////  i = EA_ucMIFTransfer(hMifsHandle, 9);
//    i = CardMFCValueWithTransfer( 1, 9, 9, CardInfo.fare );
//    if ( i != ok )
//    {   //扣款出错处理，记录出错卡卡号和原余额
//        memcpy(&ErrCardInfo.m1_card_number[0], &CardInfo._CARD_NUMBER.detail_card_number.serial_number[0], 4);  //保存出错M1卡卡号
//        memcpy(&ErrCardInfo.csn[0], &CardInfo.csn[0], 4);               //保存出错M1卡硬件卡号
//        ErrCardInfo.init_balance = CardInfo.init_balance;                   //保存出错M1卡原余额
//        ErrCardInfo.error_occured = TRUE;
//        DevStat.purchase_again_req = TRUE;      //申请重新刷卡语音
//        DevStat.voice_purchase_again_freq = 100;
//// 		return notok;
////		printf_debug("222222222222");
//        EA_ucMIFHalt(hMifsHandle);  //挂卡
//        return 0xF0;
//    }
////  i = CardMFCValueWithTransfer(PICC_INCREMENT, 9, 10, 0);
////  i = EA_ucMIFIncrement(hMifsHandle, 9, 0);
////  i = EA_ucMIFTransfer(hMifsHandle, 10);
//    i = CardMFCValueWithTransfer( 1, 9, 10, 0 );
//    if ( i != ok )
//    {   //扣款出错处理，记录出错卡卡号和原余额
//        memcpy(&ErrCardInfo.m1_card_number[0], &CardInfo._CARD_NUMBER.detail_card_number.serial_number[0], 4);  //保存出错M1卡卡号
//        memcpy(&ErrCardInfo.csn[0], &CardInfo.csn[0], 4);               //保存出错M1卡硬件卡号
//        ErrCardInfo.init_balance = CardInfo.init_balance;                   //保存出错M1卡原余额
//        ErrCardInfo.error_occured = TRUE;
//        DevStat.purchase_again_req = TRUE;      //申请重新刷卡语音
//        DevStat.voice_purchase_again_freq = 100;
//// 		return notok;
////		printf_debug("111111");
//        EA_ucMIFHalt(hMifsHandle);  //挂卡
//        return 0xF0;
//    }
//
//    CardInfo.card_purchase_type = TT_CREDIT_MONEY;  //充值交易类型
//    CardInfo.balance = CardInfo.init_balance + CardInfo.fare;//储存交易后余额
//
////	printf_debug("3333333");
//
//    ////////////////////////////////////////
//    //验证Sector 3.4.5 密钥改写交易记录
//    ////////////////////////////////////////
////  if(CardInit() != ok)
////  	return ok;
//
//    m = (CardInfo.credit_rec_pos -12 )/4  + 3;//扇区号
//    i = Get_Sector_PermissionB(m, &KeySector[m][0]);
//    if ( i != ok )
//        return ok;
//    memcpy(&CardInfo.blockcreditrec[0], &CardInfo.card_in_time.year, 5);//yymmddhhmi
//    CardInfo.blockcreditrec[5] = (CardInfo.init_balance / 0x10000);//卡片金额
//    CardInfo.blockcreditrec[6] = (CardInfo.init_balance % 0x10000 / 0x100);//卡片金额
//    CardInfo.blockcreditrec[7] = (CardInfo.init_balance % 0x100);//卡片金额
//
//    CardInfo.blockcreditrec[8] = ( CardInfo.fare / 0x10000 );//交易金额
//    CardInfo.blockcreditrec[9] = ( CardInfo.fare % 0x10000 / 0x100);//交易金额
//    CardInfo.blockcreditrec[10] = ( CardInfo.fare % 0x100);//交易金额
//
//    memcpy(&CardInfo.blockcreditrec[11],&DevStat.equ_id[1],2);//设备号
//    memcpy(&CardInfo.blockcreditrec[13], &DevStat.cur_driver_number[1],3);//操作员号        //by liujihua
//
//    i = EA_ucMIFWrite(hMifsHandle, CardInfo.credit_rec_pos, &CardInfo.blockcreditrec[0]);
//    //充值后余额
//    CardInfo.balance = CardInfo.init_balance + CardInfo.fare;//储存交易后余额
//
////	if ( (DevStat.oldman_free == TRUE && CardInfo.card_type == CARDT_OLDMAN)
////		 || (DevStat.employee_a_free == TRUE && CardInfo.card_type == CARDT_FREE)
////		 || (DevStat.driver_free == TRUE && CardInfo.card_type == CARDT_DRIVER) )
////	{
////		CardInfo.card_purchase_type = TT_FREE;	//免费卡交易
////	}
////	else
////	{
////		CardInfo.card_purchase_type = TT_NORMAL_CONSUME_MONEY;//普通卡正常交易
////	}
//    lcddisperr("充值成功!");
//    return ok;
//}
//
/******************************************************************************
 函数名称：CardInit
 功能描述：卡片初始化,完成找卡工作
 参数描述：
 参数名称：输入/输出？	类型		描述
 
 返  回  值：    ok(0)-成功
				 notok(0xFF)-失败
				 0xFE-没找到卡
				 0xF0-重新刷卡
				   
 作      者	：于福勇/刘及华
 日      期：2012-12-10
 修改历史：
		日期		修改人		修改描述
******************************************************************************/
INT8U CardInit(void)
{
	uchar	ucResult;
	ushort	usICType;
	ulong	ulSnr;
	INT8U   kind_of_card;
	uint	uiSize;

	ucResult = EA_ucMIFRequest(hMifsHandle, EM_mifs_IDLE, &usICType);
	if (ucResult != EM_SUCCESS)
	{
//  	lcddisperr("M1卡获取卡号失败");
		return notok;
	}

	ucResult = EA_ucMIFAntiColl(hMifsHandle, &ulSnr);
	if (ucResult != EM_SUCCESS)
	{
//  	lcddisperr("M1卡获取卡号失败");
		return notok;
	}

	ucResult = EA_ucMIFSelect(hMifsHandle, ulSnr, &uiSize);
	if (ucResult != EM_SUCCESS)
	{
//  	lcddisperr("M1卡获取卡类型失败");
		return notok;
	}

	CardInfo.csn[3] = (INT8U)(ulSnr / 0x1000000);
	CardInfo.csn[2] = (INT8U)((ulSnr % 0x1000000) / 0x10000);
	CardInfo.csn[1] = (INT8U)((ulSnr % 0x10000) / 0x100);
	CardInfo.csn[0] = (INT8U)(ulSnr % 0x100);

	kind_of_card = usICType;

//#ifdef _debug_m1card_
//    EA_vDisplay(3, "卡类型为:%02x", kind_of_card);
//    SleepMs(2000);
//#endif

//  if ( kind_of_card != M1_CARD )
//  {
//  	lcddisperr("不是M1卡");   //frank -- 在出厂前测试用
//  	return notok;
//  }
	//取得硬件卡号
	return ok;
}


