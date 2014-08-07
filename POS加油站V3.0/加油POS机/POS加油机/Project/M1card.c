/******************************************************************************
Copyright 2012 	大连易达通电子技术有限公司
All rights riserved.
文 件 名： m1card.c
模块名称： E550 刷卡子程序
功能概要： 主程序
*****************************************************************/
#include "main.h"
//#define  Debug_Com
//#define _debug_m1card_      //调试信息，by frank ,正式出厂时要去掉
//#define  _zb_debug_sd_
//#define  _zb_debug_tran_

extern DevHandle hCpuHandle;      //CPU卡
extern DevHandle hMifsHandle;
extern DevHandle hSam1Handle;
extern ET_PART_INFO ptPartInfo;
extern DEV_STAT    DevStat;
ERROR_CARD_INFO ErrCardInfo;
extern CARD_INFO   CardInfo;
extern ET_DISK_INFO ptDiskInfo;
char send_buffer[1024];



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
extern INT8U const   GET_M1_FUZA_KEY[5];		//计算M1卡个区密钥
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
 sector		输入		INT8U		要取得权限的区
 key		输入		INT8U *		要取得权限区的密钥
				
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
		//lcddisperr("密钥验证错误!");
		return notok;
	}
	else
	{
		return ok;
	}
}

/******************************************************************************
 函数名称：M1CardProcess
 功能描述：M1卡交易流程
 参数描述：
 参数名称：	输入/输出？	类型		描述
 
 返  回  值：ok(0)-成功
				 notok(0xFF)-失败
				 0xF0-重新刷卡
				   
 作      者	：刘及华
 日      期：2014-02-22
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
******************************************************************************/
INT8U M1CardProcess(void)
{
	INT8U     i = 0;					
	uint      rcv_length;
	INT16U    k = 0;
	INT8U     temp_rcv_buf[50];
	INT8U     temp_snd_buf[50];
	INT8U     block_buffer1[16] = {0};			//M1卡一个block的数据缓冲


#ifndef Debug_M1card

	 i = PsamRst();
	 if ( i != ok )
	 {
		 lcddisperr((char *)"PSAM卡故障");
		 return notok;
	 }
	
	////////////////////////////////////////
	//保存时间
	////////////////////////////////////////
	i = SaveCardInTime(&CardInfo);
	if ( i != ok )
	{
		lcddisperr("时间出错");
		return notok;
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
		lcddisperr((char *)"请重新刷卡1");
		return notok;
	}
		
	i = EA_ucMIFRead(hMifsHandle, 4, block_buffer1);
	if ( i != ok )
	{
		lcddisperr((char *)"请重新刷卡2");
		return notok;
	}
		
	if ( block_buffer1[12] != 0x01 )	  //判断启用标志
	{
		//卡未启用处理
		lcddisperr((char *)"卡片未启用");
		Beeperr();
		return notok;
	}

	//保存卡的应用系列号
	memcpy(&CardInfo._CARD_NUMBER.detail_card_number.city_code[0], block_buffer1, 2);   //城市代码
	memcpy(&CardInfo._CARD_NUMBER.detail_card_number.calling_code[0], block_buffer1 + 2, 2);  //行业代码
	memcpy(&CardInfo._CARD_NUMBER.detail_card_number.serial_number[0], &block_buffer1[4], 4); //发行流水号
	memcpy(&CardInfo.authentication_code[0], &block_buffer1[8], 4); //保存卡认证码
	CardInfo.card_type = block_buffer1[13]; //卡类型

#ifdef _debug_m1card_
	EA_vDisplay(3, "加油卡类型:%02x", block_buffer1[12]);
	SleepMs(2000);
#endif									    


	////////////////////////////////////////
	//PSAM卡选1003目录
	////////////////////////////////////////
	k = CmdPsam(hSam1Handle, (INT8U *)SELECT_SAM_ADF1, sizeof(SELECT_SAM_ADF1),
				temp_rcv_buf, &rcv_length);     //选1001目录
	if ( k != ok )
	{
		lcddisperr("PSAM卡选3F01目录错误");
		return notok;
	}

	////////////////////////////////////////
	//PSAM卡算各区密钥
	////////////////////////////////////////
	memcpy(temp_snd_buf, (INT8U *)GET_M1_KEY, sizeof(GET_M1_KEY)); //头
	memcpy(&temp_snd_buf[5], &CardInfo._CARD_NUMBER.detail_card_number.city_code[0], 2); //城市代码
	memcpy(&temp_snd_buf[7], &CardInfo.csn[0], 4); //CSN硬件卡号
	memcpy(&temp_snd_buf[11], &CardInfo._CARD_NUMBER.detail_card_number.serial_number[2], 2); //卡发行流水号后2字节
	memcpy(&temp_snd_buf[13], &CardInfo.authentication_code[0], 4); //卡认证码
	temp_snd_buf[17] = 0x02;
	temp_snd_buf[18] = 0x06;
	temp_snd_buf[19] = 0x0a;
	temp_snd_buf[20] = 0x0b;
	temp_snd_buf[21] = 0x0c; //计算sector2和6,10,11,12的密钥
	k = CmdPsam(hSam1Handle, temp_snd_buf, 22, temp_rcv_buf, &rcv_length); //算MAC1
	if ( k != ok )
	{
		lcddisperr("算MAC1错误");
		return notok;
	}
	memcpy(&KeySector[2][0], &temp_rcv_buf[0], 6);   //保存计算出的密钥，2sector
	memcpy(&KeySector[6][0], &temp_rcv_buf[6], 6);   //保存计算出的密钥，6sector
	memcpy(&KeySector[10][0], &temp_rcv_buf[12], 6); //保存计算出的密钥，10sector
	memcpy(&KeySector[11][0], &temp_rcv_buf[18], 6); //保存计算出的密钥，11sector
	memcpy(&KeySector[12][0], &temp_rcv_buf[24], 6); //保存计算出的密钥，12sector
	

    ////////////////////////////////////////
	//读Sector 11 中44 区信息时间等信息
	//////////////////////////////////////
	i = Get_Sector_PermissionA(11, &KeySector[11][0]);
	if ( i != ok )
	{
		lcddisperr((char *)"请重新刷卡3");
		return notok;
	}

	i = EA_ucMIFRead(hMifsHandle, 44, block_buffer1);
	if ( i != ok )
	{
		lcddisperr((char *)"请重新刷卡4");
		return notok;
	}

	memcpy(&CardInfo.driver_number[0], &block_buffer1[3], 3);//员工卡 卡号

//  memcpy(&CardInfo.authentication_code[0], &block_buffer1[8], 4);						//保存卡认证码

	//////////////////////////////////////////////////////////////
	//查黑名单此卡还没有被锁，从库里得到黑名单
	//////////////////////////////////////////////////////////////
#ifdef Debug_Com
	sprintf(send_buffer, "\r\n CSN:%02X%02X%02X%02X\r\n",
			CardInfo.csn[0], CardInfo.csn[1], CardInfo.csn[2], CardInfo.csn[3]);
	Debugprintf(send_buffer);
	sprintf(send_buffer, "\r\n code:%02X%02X%02X%02X\r\n",
			CardInfo.authentication_code[0], CardInfo.authentication_code[1],
			CardInfo.authentication_code[2], CardInfo.authentication_code[3]);
	Debugprintf(send_buffer);
	sprintf(send_buffer, "\r\n DRIVER:%02X%02X%02X\r\n",
			CardInfo.driver_number[0], CardInfo.driver_number[1], 
			CardInfo.driver_number[2]);
	Debugprintf(send_buffer);

	sprintf(send_buffer, "\r\n bus_num:%s\r\n",
			(char *)CardInfo.bus_number);
	Debugprintf(send_buffer);
#endif

//  //////////////////////////////////////////////////////////////
//  //查看此卡 密钥是否正确？
//  //////////////////////////////////////////////////////////////
//  i = Cal_Keyval();
//  if( i !=ok )
//  {
//  	lcddisperr("交易未完成!");
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
			//CalTAC();		  //计算TAC码
			EA_ucMIFHalt(hMifsHandle);                          //挂卡
			StoreRecord(&CardInfo);			   			        //储存消费记录	
			print_sale_record();                                //打印本次交易数据
			//恢复状态
			memset(&CardInfo, 0x00, sizeof(CardInfo));			//清卡片交易信息
			memset(&ErrCardInfo, 0x00, sizeof(ErrCardInfo));	//清出错卡信息
			DevStat.purchase_again_req = FALSE;					//清重新刷卡语音标志
			DevStat.voice_purchase_again_freq = FALSE;
			return ok;
		}
	}

	lcddisperr("交易未完成!");
	EA_ucMIFHalt(hMifsHandle);                          //挂卡
	return i;
}
/*****************************************************************
函数原型：trans_to_int32u
功能描述：将4字节数组转换为INT32U的数
参数描述：
参数名称：	输入/输出？ 类型		描述
buf			输入		INT8U *		需要进行转换的字符串
			
返	回	 值：转换结果 
 
作		 者：许岩
日		 期：2003-11-27
修改历史：
		日期		修改人		修改描述
		------		---------	-------------
*****************************************************************/
INT32U trans_to_int32u(INT8U *buf)
{
	INT32U i = 0;

	i = (INT32U)buf[0] * 0x1000000 + (INT32U)buf[1] * 0x10000 + (INT32U)buf[2] * 0x100 + (INT32U)buf[3];
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
	INT8U  i = 0;
	INT8U  record[57];
	INT32U temp_int32u = 0;
	uchar  buf[100];
	int    buf_len = 0;

	RECORD * prec = (RECORD*)record;
	
	memset(record, 0x00, sizeof(record));
	memset(buf, 0, sizeof(buf));

	//////////////////////////////////////////
	//组织交易记录格式，相同部份
	//////////////////////////////////////////

	temp_int32u = query_rec_num();
	if(temp_int32u >= 2000)
	{
		lcddisperr("请尽快将记录上传");
		temp_int32u = 2000;
	}

	memcpy((INT8U *)&prec->pos_purchase_serial_num[0], (INT8U *)&cardinfo->purchase_serial_num[0], 3);
	var_bcd2asc(buf+buf_len, prec->pos_purchase_serial_num, 3);
	buf_len+=6;
#ifdef _zb_debug_tran_
	scrShowMsgInfo((char *)"num", buf+buf_len-6, 6, ORG_ASCII);
#endif
	//车牌号
    memcpy((INT8U *)&prec->bus_number[0], (INT8U *)&cardinfo->bus_number[0], 5);				
//  var_bcd2asc(buf+buf_len, &cardinfo->bus_number[0], 5);
	memcpy(buf+buf_len, &prec->bus_number[0], 5);
	buf_len+=5;
#ifdef _zb_debug_tran_
		scrShowMsgInfo((char *)"bus_num", buf+buf_len-5, 5, ORG_ASCII);
#endif
	//交易日期4字节和时间 3字节
	memcpy((BUS_TIME *)&prec->card_in_time, (BUS_TIME *)&cardinfo->card_in_time, 7);		
	var_bcd2asc(buf+buf_len, (uchar *)&prec->card_in_time, 7);
	buf_len+=14;
#ifdef _zb_debug_tran_
		scrShowMsgInfo((char *)"date", buf+buf_len-14, 14, ORG_ASCII);
#endif
	//加油金额
//  memset(tmpbuf, 0, 10);                                                  
//  packInt(tmpbuf, cardinfo->fare);
	memcpy(&prec->fare, &cardinfo->fare, 4);
//  var_bcd2asc(buf+buf_len, (uchar *)&cardinfo->fare, 4);
//  buf_len+=8;
	sprintf((char *)(buf+buf_len), (char *)"%08d", cardinfo->fare);
	buf_len+=8;
#ifdef _zb_debug_tran_
		scrShowMsgInfo((char *)"fare", buf+buf_len-8, 8, ORG_ASCII);
#endif	
//  memset(tmpbuf, 0, 10);	                                                //加油升数
//  packInt(tmpbuf, cardinfo->capacity);
	memcpy(&prec->capacity, &cardinfo->capacity, 4);
//  var_bcd2asc(buf+buf_len, (uchar *)&prec->capacity, 4);
	sprintf((char *)(buf+buf_len), (char *)"%08d", prec->capacity);
	buf_len+=8;
#ifdef _zb_debug_tran_
		scrShowMsgInfo((char *)"cap", buf+buf_len-8, 8, ORG_ASCII);
#endif	
																		
	memcpy(&prec->driver_no[0], (INT8U *)(&cardinfo->driver_number[0]), 3); //员工卡卡号
	var_bcd2asc(buf+buf_len, &prec->driver_no[0], 3);
	buf_len+=6;
#ifdef _zb_debug_tran_
		scrShowMsgInfo((char *)"driver_no", buf+buf_len-6, 6, ORG_ASCII);
#endif

	memcpy(&prec->acnt_id[0], &DevStat.acnt_id[0], 2);                     //站点号
	var_bcd2asc(buf+buf_len, prec->acnt_id, 2);
	buf_len+=4;
#ifdef _zb_debug_tran_
		scrShowMsgInfo((char *)"acnt_id", buf+buf_len-4, 4, ORG_ASCII);
#endif

	memcpy(&prec->equ_id[0], &DevStat.equ_id[0], 3);                      //设备编号
	var_bcd2asc(buf+buf_len, prec->equ_id, 3);
	buf_len+=6;
#ifdef _zb_debug_tran_
		scrShowMsgInfo((char *)"equ_id", buf+buf_len-6, 6, ORG_ASCII);
#endif

	memcpy(&prec->fuel_type, &DevStat.fill_mode, 1);                        //油品
	var_bcd2asc(buf+buf_len, &prec->fuel_type, 1);
	buf_len+=2;

#ifdef _zb_debug_tran_
		scrShowMsgInfo((char *)"fuel_type", buf+buf_len-2, 2, ORG_ASCII);
#endif	
																		
	memcpy((uchar *)&prec->price, (uchar *)&DevStat.price[DevStat.fill_mode], 4); //单价
	var_bcd2asc(buf+buf_len, (uchar *)&prec->price, 4);
	sprintf((void *)(buf+buf_len), "%08d", prec->price);
	buf_len+=8;
#ifdef _zb_debug_tran_
		scrShowMsgInfo((char *)"fuel_price", buf+buf_len-8, 8, ORG_ASCII);
#endif
																		
	memcpy(&prec->oper_id[0], &DevStat.oper_cardno[0], 3);            //操作员号
	var_bcd2asc(buf+buf_len, &prec->oper_id[0], 3);
	buf_len+=6;
#ifdef _zb_debug_tran_
		scrShowMsgInfo((char *)"oper_id", buf+buf_len-6, 6, ORG_ASCII);
#endif

//  crc = Cal_CRC(record, sizeof(record));
//	Debugprintf((char *)buf);

	i = StoreRectoSD(prec);
	if( i == notok )
	{
		lcddisperr("本次交易未备份");
		Beeperr();
		return notok;
	}

	i = SaveCardRecord(prec);	
	if( i == notok)
	{
		lcddisperr("本次交易不成功");
		Beeperr();
		return notok;
	}
	
	i = prec->fuel_type;

	if( i <= 3 )
	{
		DevStat.binhand.consume_cnt[i] ++;
		DevStat.binhand.consume_cap[i] += prec->capacity;
		DevStat.binhand.consume_amt[i] += prec->fare;
	}

	WriteParam();
	
	return ok;
}
/////////////////////////////////////////////////
//
/////////////////////////////////////////////////
INT8U StoreRectoSD(RECORD * prec)
{
	FILE *fp = NULL; 
	uchar buf[100];
	char  path[50];
	int   buf_len = 0;
	long  length;

	memset(buf, 0, sizeof(buf));
	memset(path, 0, sizeof(path));

//  frank -- 2014,4,8好，这里去测试的时候，要修改回来，按日期来存储！
//  htoa(buf, (void *)&prec->card_in_time, 6);
//  sprintf(path, "%s/%s", ptPartInfo.acHome, buf);
	sprintf(path, "%s/%s", ptPartInfo.acHome, "backup.txt");

	if (pfexist(path) == FALSE) {
		fp = fopen(path, "w+");
	} else {
		fp = fopen(path, "r+");
	}

	fseek(fp, 0L, SEEK_END);
	length=ftell(fp);

	if(length >= ptDiskInfo.ullSize - 100)
	{
		lcddisperr("SD卡已经存储满!");
		Beeperr();
		return notok;
	}

#ifdef _zb_debug_sd_
		Debugprintf((char *)"<<<<<<<<<<<<<_debug_sd_<<<<<<<<<<<<\n");
#endif
	//4B 商户号
	var_bcd2asc(buf+buf_len, prec->acnt_id, 2);
	buf_len+=4;
#ifdef _zb_debug_sd_
		scrShowMsgInfo((char *)"acnt_id", buf+buf_len-4, 4, ORG_ASCII);
#endif
	//6B 设备编号
	var_bcd2asc(buf+buf_len, prec->equ_id, 3);
	buf_len+=6;
#ifdef _zb_debug_sd_
		scrShowMsgInfo((char *)"equ_id", buf+buf_len-6, 6, ORG_ASCII);
#endif
	//6B 操作员号
	var_bcd2asc(buf+buf_len, &prec->oper_id[0], 3);
	buf_len+=6;
#ifdef _zb_debug_sd_
		scrShowMsgInfo((char *)"oper_id", buf+buf_len-6, 6, ORG_ASCII);
#endif
	//6B 交易序号号
	var_bcd2asc(buf+buf_len, prec->pos_purchase_serial_num, 3);
	buf_len+=6;
#ifdef _zb_debug_sd_
	scrShowMsgInfo((char *)"num", buf+buf_len-6, 6, ORG_ASCII);
#endif
//5B 车号
	memcpy(buf+buf_len, &prec->bus_number[0], 5);
//  var_bcd2asc(buf+buf_len, &prec->bus_number[0], 5);
	buf_len+=5;
#ifdef _zb_debug_sd_
		scrShowMsgInfo((char *)"bus_num", buf+buf_len-5, 5, ORG_ASCII);
#endif
	//6B 加油员号
	var_bcd2asc(buf+buf_len, &prec->driver_no[0], 3);
 buf_len+=6;
#ifdef _zb_debug_sd_
	 scrShowMsgInfo((char *)"driver_no", buf+buf_len-6, 6, ORG_ASCII);
#endif
	//1B 加油种类
	var_bcd2asc(buf+buf_len, &prec->fuel_type, 1);
	buf_len+=2;
#ifdef _zb_debug_sd_
		scrShowMsgInfo((char *)"fuel_type", buf+buf_len-2, 2, ORG_ASCII);
#endif	
	//8B 加油单价
	sprintf((char *)(buf+buf_len), "%08d", prec->price);
	buf_len+=8;
#ifdef _zb_debug_sd_
		scrShowMsgInfo((char *)"fuel_price", buf+buf_len-8, 8, ORG_ASCII);
#endif
		//8B 加油金额
//  var_bcd2asc(buf+buf_len, (uchar *)&prec->fare, 4);
	sprintf((char *)(buf+buf_len), "%08d", prec->fare);
	buf_len+=8;
#ifdef _zb_debug_sd_
		scrShowMsgInfo((char *)"fare", buf+buf_len-8, 8, ORG_ASCII);
#endif	

//  var_bcd2asc(buf+buf_len, (uchar *)&prec->capacity, 4);
		//8B 加油升数
	sprintf((char *)(buf+buf_len), "%08d", prec->capacity);
	buf_len+=8;
#ifdef _zb_debug_sd_
		scrShowMsgInfo((char *)"cap", buf+buf_len-8, 8, ORG_ASCII);
#endif	
//14B 加油时间
	var_bcd2asc(buf+buf_len, (uchar *)&prec->card_in_time, 7);
	buf_len+=14;
#ifdef _zb_debug_sd_
		scrShowMsgInfo((char *)"date", buf+buf_len-14, 14, ORG_ASCII);
#endif

	strcat((char *)buf, "\r\n");
	if (fwrite(buf, strlen((void *)buf), 1, fp) == FALSE)	
	{
		pfclose(fp);
		lcddisperr("写SD文件失败");
		return notok;
	}

//  memset(buf, 0, sizeof(buf));
//  if(pfread(fp, (char *)buf, sizeof(RECORD)) == FALSE)
//  {
//  	pfclose(fp);
//  	lcddisperr("读SD文件失败");
//  	return notok;
//  }

//#ifdef _debug_sd_
//    Debugprintf((char *)buf);
//#endif

	pfclose(fp);
	return ok;
}


FILE * pfopen(char *filepath, char *mode)
{
	return (fopen(filepath, mode));
}

size_t pfread(FILE *fh, char *dest, long num)
{
	return (fread((char *)dest, sizeof(char), num, fh));
}

//////////////////////////////////////////////////////////////////////////
//
size_t pfwrite(FILE *fh, char *src, long num)
{
	return (fwrite((char *)src, sizeof(char), num, fh));
}

void pfseek(FILE *fh, long seek, int mode)
{
	fseek(fh, seek, mode);
}
//////////////////////////////////////////////////////////////////////////
//
void pfclose(FILE *fh)
{
	fclose(fh);
}
//文件存在则返回true;
int pfexist(char *filename)
{
	return (access(filename, 0) == 0); 
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

/******************************************************************************
 函数名称：INT8U Cal_Keyval(void)
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
INT8U Cal_Keyval(void)
{
	uchar i = 0;
	INT8U input[22];
	char tmp_key[10];

	memset(tmp_key, 0, sizeof(tmp_key));
	EA_vCls();
	EA_vDisplay(1, "加 油 员:%02X%02X%02X", CardInfo.driver_number[0],CardInfo.driver_number[1],CardInfo.driver_number[2]);
	EA_vDisplay(2, "输入密码:");
	EA_vDisplay(4, "请按<确认>键继续");
	strcpy((void *)input, "");

	sprintf(tmp_key, "%02X%02X%02X%02X", CardInfo.authentication_code[0], CardInfo.authentication_code[1]
			,CardInfo.authentication_code[2], CardInfo.authentication_code[3]);

	for ( ;; )
	{
		i = EA_ucGetInputStr(2, 10, 20, EM_BIG_FONT | EM_MODE_NUMBER | EM_ALIGN_LEFT | EM_SHOW_PASSWORD
							 , 8, 8, 0, (void *)input);
		if ( i == EM_SUCCESS )
			break;
		if ( i == EM_ABOLISH )
			return 2;
	}
	if ( memcmp((void *)input, tmp_key, 4) == 0 )
	{
		return ok;
	}
	else
	{
		EA_ucSetInverse(EM_lcd_INVON);
		lcddisp(4, 1, (void *)"      密码错误      ");
		EA_ucSetInverse(EM_lcd_INVOFF);
		(void)EA_uiInkey(1);
		return notok;
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
	INT8U p[67];
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

	memcpy(l_rec_buf, p, 67);
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
	INT32U  temp_int32u;
	uchar   i;
	double   temp_cap = 0.00;
	double   temp_fare = 0.00;

reboot:
	memset(buf, 0, sizeof(buf));

	//消费记录区处理
	EA_vCls();
	EA_ucClrKeyBuf();
	Beep(1000);

	EA_vDisplay(1, "车  号 :鲁C-%s", CardInfo.bus_number);

	EA_ucSetInverse(EM_lcd_INVOFF);

	switch( DevStat.fill_mode )
	{
	   case 0:
		   EA_vDisplay(2, "天然气 :      立方  ");
		   EA_vDisplay(3, "单  价 : %3.2f元/立方", DevStat.price[0]/100.0);
		   CardInfo.price = DevStat.price[0];
		   break;
	   case 1:
		   EA_vDisplay(2, "汽油93#:       升(L)");
		   EA_vDisplay(3, "单  价 : %3.2f", DevStat.price[1]/100.0);
		   CardInfo.price = DevStat.price[1];
		   break;
	   case 2:
		   EA_vDisplay(2, "汽油97#:       升(L)");
		   EA_vDisplay(3, "单  价 : %3.2f", DevStat.price[2]/100.0);
		   CardInfo.price = DevStat.price[2];
		   break;
	   case 3:
		   EA_vDisplay(2, "柴  油 :       升(L)");
		   EA_vDisplay(3, "单  价 : %3.2f", DevStat.price[3]/100.0);
		   CardInfo.price = DevStat.price[3];
		   break;
	   default:
		   break;
	}
		   EA_vDisplay(4, "员工号 :%02X%02X%02X", CardInfo.driver_number[0], CardInfo.driver_number[1]
					   , CardInfo.driver_number[2]);


	strcpy((void *)input, "");

	for ( ;; )
	{
		i = get_input_str(2, 9, 1, 6, (void *)input);
		if ( i == EM_SUCCESS )
			break;
		if ( i == EM_ABOLISH )
			return notok;
	}

	sscanf((void *)input, "%lf", &temp_cap);
	
	if( DevStat.fill_mode == 0 )
	{
		sprintf((void *)buf, "加气量:%4.2lf立方", temp_cap);
	}
	else
	{                        
		sprintf((void *)buf, "加油量:%4.2lf升  ", temp_cap);
	}

//  CardInfo.capacity = (INT32U)(temp_cap * 100.0);
	CardInfo.capacity = (INT32U)((temp_cap + 0.0001) * 100.0);
	temp_fare = (temp_cap * (CardInfo.price/100.0) * 100 + 0.5) / 100.0;
	CardInfo.fare = temp_fare * 100;//四舍五入

	lcddisp(2, 1, (void *)buf);


	if ( (CardInfo.capacity < 1) || (CardInfo.capacity > 100000 ) )
	{
		//余额不足处理
		lcddisperr((char *)"输入的升数错误!");
		goto reboot;
	}
	CardInfo.card_purchase_type = TT_NORMAL_CONSUME;		 //普通卡正常交易
	//保存设备交易序号	
	temp_int32u = ((INT32U)DevStat.dev_trade_no[0] << 16) | ((INT16U)DevStat.dev_trade_no[1] << 8) | ((INT16U)DevStat.dev_trade_no[2]); 	//初始交易序号
	if ( temp_int32u >= 0xF423F )   //消费大于999999
		temp_int32u = 0;
	else
		temp_int32u++;

	DevStat.dev_trade_no[0] = (INT8U)(temp_int32u / 0x10000); //+1后的交易序号
	DevStat.dev_trade_no[1] = (INT8U)(temp_int32u % 0x10000 / 0x100); //+1后的交易序号
	DevStat.dev_trade_no[2] = (INT8U)(temp_int32u % 0x100);

	memcpy((INT8U *)&CardInfo.purchase_serial_num[0], (INT8U *)&DevStat.dev_trade_no[0], 3);

	WriteParam();

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
				   
 作      者	：刘及华
 日      期：2014-09-02
 修改历史：
		日期		修改人		修改描述
		2005.2.18		myron	                   in function 
******************************************************************************/
INT8U ICCardProcess(void)
{
	INT8U     i = 0;  
	INT8U     block_buffer1[16] = {0};			//M1卡一个block的数据缓冲
	char      tmp_buf[10];
	uchar     temp_uc[5];

	memset(tmp_buf, 0, 10);

	i = CardInit();

	if( i != ok )
	{
		EA_ucMIFHalt(hMifsHandle);  //挂卡
		return notok;
	}

	////////////////////////////////////////
	//保存时间
	////////////////////////////////////////
	i = SaveCardInTime(&CardInfo);
	if ( i != ok )
	{
		lcddisperr("时间出错");
		return notok;
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
	//Debugprintf((char *)block_buffer1);	

	if ( block_buffer1[10] != 0x01 )	  //判断启用标志
	{
		//卡未启用处理
		lcddisperr((char *)"卡片未启用");
		Beeperr();
		return notok;
	}

	//车辆自编号
	memcpy(CardInfo.mac1, &block_buffer1[0], 4);	
//  Debugprintf(CardInfo.bus_number);						 		//车自编号
	//车牌号
	memcpy(CardInfo.bus_number, &block_buffer1[4], 5);	
	CardInfo.bus_number[5] = '\0';

	//保存卡类型
//  CardInfo.card_type = ;													 //卡类型
//  EA_vDisplay(1, (void *)"车辆卡类型:%02X", CardInfo.card_type);
//  SleepMs(2000);

#ifdef _debug_m1card_
	EA_vDisplay(3, "加油卡类型:%02X", CardInfo.card_type);
	SleepMs(2000);
#endif

	if ( block_buffer1[11] != 0xAA )                 //只允许普通卡消费debug  by frank
	{
		lcddisperr((void *)"不是车辆卡!");
		return notok;
	}

	memcpy(temp_uc, &CardInfo.bus_number[0], 5);

	i = BlackName_Find((char *)temp_uc);	//白名单判断
	if ( i != ok )	 						//不是白名单
	{
		lcddisperr("该卡不在白名单中");
		Beeperr();
		return i;
	}


//    //////////////////////////////////////////////////////////////
//    //查看此卡当日是否已刷过
//    //////////////////////////////////////////////////////////////
//    i = Cal_Interval();
//    if( i != ok )
//    {
//        lcddisperr("此车当日已加过气!");
//        Beeperr();
////  	return i;
//    }

	EA_vCls();
	EA_vDisplay(1, "车  号 :鲁C-%s", CardInfo.bus_number);
	EA_ucMIFHalt(hMifsHandle);  //挂卡
	//SleepMs(1000);
	EA_vBeepMs(2000);

	return ok;

}

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
		lcddisperr("M1卡获取卡类型失败");
		return notok;
	}

	CardInfo.csn[3] = (INT8U)(ulSnr / 0x1000000);
	CardInfo.csn[2] = (INT8U)((ulSnr % 0x1000000) / 0x10000);
	CardInfo.csn[1] = (INT8U)((ulSnr % 0x10000) / 0x100);
	CardInfo.csn[0] = (INT8U)(ulSnr % 0x100);

	kind_of_card = usICType;

#ifdef _debug_m1card_
	EA_vDisplay(3, "卡类型为:%02X", kind_of_card);
	SleepMs(2000);
#endif

	if ( kind_of_card != M1_CARD )
	{
		lcddisperr("不是M1卡");   //frank -- 在出厂前测试用
		return notok;
	}
	//取得硬件卡号
	return ok;
}


