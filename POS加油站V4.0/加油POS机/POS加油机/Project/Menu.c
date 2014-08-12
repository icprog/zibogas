/*****************************************************************
模块名称：菜单
功能概要：本程序是为E550的基础而设计的菜单程序
作    者：大连金德姆电子有限公司 刘及华
日    期：2013－06－23
修改历史：
日期		修改人		修改描述
------		---------	-------------
*****************************************************************/

#include "main.h"

#define EN  1
#define CN  0


//系统超级变量
extern DEV_STAT     DevStat;					//设备状态
extern CARD_INFO    CardInfo;					//卡片信息，交易记录由此变量生成
extern const char   parmFileName[];
extern const char   blackFileName[];
extern const char   currecFileName[];
extern const char   conrecFileName[];
extern const char   hisrecFileName[];
extern const char   grayrecFileName[];

DevHandle hDprinterHandle;


// *****************************************************************
// 功能：		CRC校验码计算
// 入口参数：	data_to_cal_crc 输入字符串
//				plength 字符串长度
// 出口参数：	int crc 4字节CRC校验码
// 返回:		
// 作者：	    刘及华    
//  日期:      2013-6-23
// *****************************************************************
int Cal_CRC(uchar * data_to_cal_crc, int length)
{
	unsigned int crc;
	unsigned char i;

	crc = 0xFFFF;			//CRC初始值

	while ( length-- != 0 )
	{
		crc = crc ^ ((unsigned int)*data_to_cal_crc++ << 8);

		for ( i = 0; i < 8; ++i )
			if ( crc & 0x8000 )
				crc = (crc << 1) ^ 0x1021;
			else
				crc	= crc << 1;
	}

	return(crc & 0xFFFF);

}

/******************************************************************************
 函数名称：htoa
 功能描述：hex转换为ascii码
 参数描述：
 参数名称： 输入/输出？ 类型		描述
				
 返  回  值：ok(0)-成功
				notok(0xFF)-失败
				   
 作 	 者 ：许岩
 日 	 期：2005-04-07
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
******************************************************************************/
void htoa(void *des, INT8U *sour, int hex_len)
{
	int i = 0;
	char *dest = (char *)des;

	if ( hex_len <= 0 )
		return;

	for ( i=0; i<hex_len; i++ )
		sprintf((char *)dest + i * 2, "%02X", sour[i]);

	dest[hex_len * 2] = '\0';
}

void htoa2(void *des, INT8U *sour, int hex_len)
{
	int i = 0;
	char *dest = (char *)des;

	if ( hex_len <= 0 )
		return;

	for ( i=0; i<hex_len; i++ )
		sprintf((char *)dest + i * 2, "%02d", sour[i]);

	dest[hex_len * 2] = '\0';
}

/******************************************************************************
 函数名称：a_to_h
 功能描述：把一个0-9,a-f,A-F的ascii代码，转化为16进制数
 参数名称： 输入/输出？ 类型        描述
 num        输入        INT8U           要转化为ascii代码的数，要求范围0-9

 返  回  值：转化后的16进制数

 作      者 ：许岩
 日      期：2004-09-02
 修改历史：
		日期        修改人      修改描述
		------      ---------   -------------
******************************************************************************/
INT8U a_to_h(INT8U num)
{
	if (num>='0' && num <= '9')
		return (num - 0x30);
	else if (num == 'A' || num== 'a')
		return 0x0A;
	else if (num == 'B' || num== 'b')
		return 0x0B;
	else if (num == 'C' || num== 'c') 
		return 0x0C;
	else if (num == 'D' || num== 'd')
		return 0x0D;
	else if (num == 'E' || num== 'e')
		return 0x0E;
	else
//      if (num == 0x0F)
		return 0x0F;
//	return (num - 0x30);
}

/*****************************************************************
函数原型：bcdhex
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
INT8U bcdhex(INT8U bcd)
{
	return ((bcd >> 4) * 10 + (bcd & 0x0F));
}

/******************************************************************************
 函数名称：ascii
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
INT8U ascii(INT8U num)
{
//     return (num + 0x30);
	if (num <= 9)
		return (num + 0x30);
	else if (num == 0x0A)
		return 'A';
	else if (num == 0x0B)
		return 'B';
	else if (num == 0x0C)
		return 'C';
	else if (num == 0x0D)
		return 'D';
	else if (num == 0x0E)
		return 'E';
	else
//      if (num == 0x0F)
		return 'F';
}

/******************************************************************************
 函数名称：print_record
 功能描述：打印充值记录
 参数名称：输入/输出？	类型		描述
 输入		
 
 返  回  值：
 
 作      者	：于福勇
 日      期：2004-09-02
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
******************************************************************************/
void print_record (RECORD *prec)
{
	INT8U       RecBuff[70];
	INT8U       strbuff[50];
	double		fBuf = 0;
	uchar       ucRet;


	EA_vCls();
	EA_vDisp(1, 1, "   打印历史交易记   ");
	EA_vDisp(2, 1, "      请等待...     ");
	EA_uiInkeyMs(500);

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

	memset(strbuff, 0 , sizeof(strbuff));
	sprintf((char*)strbuff, "   淄博市公共汽车公司\n");
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	sprintf((char*)strbuff, "   历史交易记录\n");
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);


	sprintf((char*)strbuff, "----------------------\n");
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	//卡号
	htoa(RecBuff, &prec->bus_number[0], 3);
	//memcpy(RecBuff, &prec->bus_number[0], 5);
	//strcpy((void *)RecBuff, (void *)&prec->bus_number[0]);
	sprintf((char*)strbuff, "   车    号：鲁C-%s\n", RecBuff);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);

	//燃料品种
	if(prec->fuel_type == 0)
	{
	sprintf((char*)strbuff, "   燃    料：天然气\n");
	}
	else if(prec->fuel_type == 1)
	{
	sprintf((char*)strbuff, "   燃    料：汽油93#\n");
	}
	else if(prec->fuel_type == 2)
	{
	sprintf((char*)strbuff, "   燃    料：汽油97#\n");
	}
	else if(prec->fuel_type == 3)
	{
	sprintf((char*)strbuff, "   燃    料：柴油\n");
	}
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);


	//加油升数
	if( prec->fuel_type == 0 )
	{
	fBuf= (double)(prec->capacity)/100;
	sprintf((char*)strbuff, "   加气体积：%6.2lf立方\n", fBuf);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	}
	else
	{
	fBuf= (double)(prec->capacity)/100;
	sprintf((char*)strbuff, "   加油体积：%6.2lf升\n", fBuf);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	}

	//加油余额
	fBuf= (double)(prec->fare)/100;
	sprintf((char*)strbuff, "   消费金额：%6.2lf元\n", fBuf);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);

	//油品单价
	fBuf = (double)(prec->price)/100.0;
	sprintf((char*)strbuff, "   油品单价：%6.2lf元\n", fBuf);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);

	//员工卡卡号
	memcpy(RecBuff, &prec->driver_no[0], 4);
	sprintf((char*)strbuff, "   员 工 号：%02X%02X%02X%02X\n", RecBuff[0], RecBuff[1], RecBuff[2], RecBuff[3]);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);

	//交易序号
	sprintf((char*)strbuff, "   交易序号：%08d\n", prec->pos_purchase_serial_num[0]*65535+prec->pos_purchase_serial_num[1]*256+prec->pos_purchase_serial_num[2]);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	sprintf((char*)strbuff, "----------------------\n");
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);

	//交易设备编号
	htoa(RecBuff, &prec->equ_id[0], 3);
	sprintf((char*)strbuff, "   设备编号：%s\n", RecBuff);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	htoa(RecBuff, &prec->acnt_id[0], 2);
	sprintf((char*)strbuff, "   站点编号：%s\n", RecBuff);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);

	//操作员号
	htoa(RecBuff, &prec->oper_id[0], 1);
	sprintf((char*)strbuff, "   加油员号：%s\n", RecBuff);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);

	//交易日期
	memcpy(RecBuff, (INT8U*)&prec->card_in_time, 7);
	sprintf((char*)strbuff, "   加油时间：\r\n");
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	sprintf((char*)strbuff, "   %02X%02X-%02X-%02X %02X:%02X:%02X\r\n", 
	RecBuff[0],RecBuff[1],RecBuff[2],RecBuff[3],RecBuff[4],RecBuff[5],RecBuff[6]);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);

	sprintf((char*)strbuff, "----------------------\n");
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	// 打印最后调用换页命令
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)"   备注");
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)"\n\n\n\n");

	ucRet = EA_ucLinePrinter(hDprinterHandle, 0, "\f");
	if ( ucRet == EM_SUCCESS )
	{
		EA_vDisplay(2, "    本次打印结束!   ");
		EA_uiInkeyMs(500);
	}

}


///******************************************************************************
// 函数名称：print_consume_record
// 功能描述：打印充值记录
// 参数名称：输入/输出？  类型        描述
// 输入
//
// 返  回  值：
//
// 作      者 ：于福勇
// 日      期：2004-09-02
// 修改历史：
//        日期        修改人      修改描述
//        ------      ---------   -------------
//******************************************************************************/
//void print_consume_record (void)
//{
//
//    int         count;
////	CardInfoOne 	*strCardInfo;
//    INT8U       RecBuff[70];
//    INT8U       strbuff[50];
//    INT8U  rec[34];
//    CONSUMERECORD* prec=(CONSUMERECORD*)rec ;
//    double   fBuf = 0;
//    uchar  ucOpenID = 0;
//    uchar  ucRet;
//    uint   uiReadCnt;
//    uint   uiRetCnd;
//
//    EA_vCls();
//    //读记录有效条数
//    if ( EA_ucPFOpen( (uchar *)conrecFileName, &ucOpenID) != EM_ffs_SUCCESS )
//    {
//        lcddisperr("打开消费记录失败");
//        EA_ucPFClose(ucOpenID);
//        return;
//    }
//    // 获取剩余可写记录数
//    (void)EA_ucPFInfo(ucOpenID, EM_ffs_VALIDREC, &uiReadCnt);
//    if( uiReadCnt == 0)
//    {
//        lcddisperr("没有消费记录信息");
//        EA_ucPFClose(ucOpenID);
//        return;//表示没有记录
//    }
//    count = uiReadCnt;//最后一条
////  prec = (RECORD *)DB_jump_to_record(DB_REC,count,&flag);//count -1 :代表记录序号
//    ucRet = EA_ucPFReadRec(ucOpenID, count, 1, &uiRetCnd, rec);
//    if(ucRet != EM_SUCCESS)
//    {
//        EA_ucPFClose(ucOpenID);
//        return;
//    }
//    EA_ucPFClose(ucOpenID);
////  DispStr_CE(0, 0, "打印记录 ", DISP_CENTER);//(int qx, int qy, char * ceStr, int xyFlag)
////  DispStr_CE(0, 4, "请等待... ...", DISP_CENTER);//(int qx, int qy, char * ceStr, int xyFlag)
//    EA_vDisp(1, 1, "打印消费记录");
//    EA_vDisp(2, 1, "请等待...");
//    EA_uiInkeyMs(500);
//    //mtclprn_init_page(pagebuff, PageSize);
//    // 可进行打印的一些基本设置
//
////  ucRet = EA_ucSetPrinterFormat(hDprinterHandle, EM_prn_XSPACE, 0);
////  if ( ucRet != EM_SUCCESS )
////  {
////  	//  	(void)EA_ucCloseDevice(&hDprinterHandle);
////  	lcddisperr("打印机设置失败!");
////  	return;
////  }
////  ucRet =  EA_ucSetPrinterFormat(hDprinterHandle, EM_prn_YSPACE, 6);
////  if ( ucRet != EM_SUCCESS )
////  {
////  	//  	(void)EA_ucCloseDevice(&hDprinterHandle);
////  	lcddisperr("打印机设置失败!");
////  	return;
////  }
////
////  //  ucRet = EA_ucSetPrinterFormat(hDprinterHandle, EM_prn_PAGESPACE, 578);
////  ucRet = EA_ucSetPrinterFormat(hDprinterHandle, EM_prn_PAGESPACE , 600);
////  if ( ucRet != EM_SUCCESS )
////  {
////  	//  	(void)EA_ucCloseDevice(&hDprinterHandle);
////  	lcddisperr("打印机设置失败!");
////  	return;
////  }
////
////  ucRet = EA_ucSetPrinterFormat(hDprinterHandle, EM_prn_ASCII_DOTTYPE, EM_prn_ASCIIDOT24X12);
////  if ( ucRet != EM_SUCCESS )
////  {
////  	//  	(void)EA_ucCloseDevice(&hDprinterHandle);
////  	lcddisperr("打印机设置失败!");
////  	return;
////  }
////  ucRet = EA_ucSetPrinterFormat(hDprinterHandle, EM_prn_ASCII_PRNTYPE, EM_prn_ASCIIPRN1X1);
////  if ( ucRet != EM_SUCCESS )
////  {
////  	//  	(void)EA_ucCloseDevice(&hDprinterHandle);
////  	lcddisperr("打印机设置失败!");
////  	return;
////  }
////  ucRet = EA_ucSetPrinterFormat(hDprinterHandle, EM_prn_HZ_DOTTYPE, EM_prn_HZDOT24X24);
////  if ( ucRet != EM_SUCCESS )
////  {
////  	//  	(void)EA_ucCloseDevice(&hDprinterHandle);
////  	lcddisperr("打印机设置失败!");
////  	return;
////  }
////
////  ucRet = EA_ucSetPrinterFormat(hDprinterHandle, EM_prn_HZ_PRNTYPE, EM_prn_HZPRN1X1);
////  if ( ucRet != EM_SUCCESS )
////  {
////  	//  	(void)EA_ucCloseDevice(&hDprinterHandle);
////  	lcddisperr("打印机设置失败!");
////  	return;
////  }
////  ucRet = EA_ucSetPrinterFormat(hDprinterHandle, EM_prn_PRNGRAY, 4);
////  if ( ucRet != EM_SUCCESS )
////  {
////  	//  	(void)EA_ucCloseDevice(&hDprinterHandle);
////  	lcddisperr("打印机设置失败!");
////  	return;
////  }
//
//    ucRet = EA_ucGetPrinterStatus(hDprinterHandle);      //打印前必须判断打印机的状态
//    if( ucRet == EM_prn_PAPERENDED )
//    {
//        lcddisperr("   缺纸,打印失败!  ");
//        return;
//    }
//    if( ucRet == EM_prn_OVERHEAT )
//    {
//        EA_vDisplay(2, "    机芯温度过高    ");
//        EA_vDisplay(3, "    请稍后再打印     ");
//        EA_uiInkeyMs(500);
//        return;
//    }
//    if( ucRet == EM_prn_LOWVOL )
//    {
//        EA_vDisplay(2, "    电池电量过低    ");
//        EA_vDisplay(3, "   请充电后再打印   ");
//        EA_uiInkeyMs(500);
//        return;
//    }
//    (void)EA_ucPrinterAutoFeeding(hDprinterHandle);
//    memset(RecBuff, 0, sizeof(RecBuff));
//
//    sprintf((char*)strbuff, "欢迎使用淄博公交IC卡\n");
//    (void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
//    sprintf((char*)strbuff, "www.zbbus.com\n");
//    (void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
////公司网址
//    sprintf((char*)strbuff, "-----------\n");
//    (void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
////卡号
//    htoa(RecBuff,&prec->card_serial_number[4], 4);//城市代码2字节+行业代码2字节+卡发行流水号4字节
//    sprintf((char*)strbuff, "卡    号：%s\n", RecBuff);
//    (void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
//
////卡片类型
//    GetCardType(prec->card_type, RecBuff);
//    sprintf((char*)strbuff, "卡片类型: %s\n", RecBuff);
//    (void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
//
////有效期
//
////押金或者工本费
//
////交易类型
////	htoa(RecBuff,&prec->card_purchase_type, 1);//城市代码2字节+行业代码2字节+卡发行流水号4字节
////	sprintf((char*)strbuff, "\x1Bw0\x1BW1交易类型：%s\r\r", RecBuff);
////	mtclprn_charline_print(strbuff);
//
//    switch ( prec->card_purchase_type)
//    {
//        case TT_NORMAL_CONSUME_MONEY:                   //检索卡片类型信息
//            strcpy((char*)RecBuff,"    冲帐");
//            break;
//        case TT_CARD_SALE:
//            strcpy((char*)RecBuff,"    售卡");
//            break;
//        case TT_CREDIT_MONEY:
//            strcpy((char*)RecBuff,"    充值");
//            break;
//        case TT_NORMAL_CONSUME:
//            strcpy((char*)RecBuff,"    消费");
//            break;
//        default:
//            strcpy((char*)RecBuff,"    其他");
//            break;
//
//    }
//    sprintf((char*)strbuff, "交易类型：%s\n", RecBuff);
//    (void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
//
////硬件卡号
////	mtclprn_charline_print("\x1Bw0\x1BW1 硬件卡号：\r");
////	mtclprn_charline_print("\x1Bw1\x1BW0  ABCDEFGH\r\r");
//
////交易金额
//    fBuf= (double)(prec->fare[0]*65536 + prec->fare[1]*256 + prec->fare[2])/100;
//    sprintf((char*)strbuff, "交易金额：%6.2f元\n", fBuf);
//    (void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
//
////卡片余额
//    fBuf= (double)(prec->balance[0]*65536 + prec->balance[1]*256 + prec->balance[2])/100;
//    sprintf((char*)strbuff, "卡片余额：%6.2f元\n", fBuf);
//    (void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
//
////卡片交易序号
////	htoa(RecBuff, &prec->purchase_serial_num[0], 2);
////	sprintf((char*)strbuff, "\x1Bw0\x1BW1 交易序号：%d\r\r", RecBuff);
//    sprintf((char*)strbuff, "交易序号:%8d\n", prec->purchase_serial_num[0]*256 + prec->purchase_serial_num[1]);
//    (void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
//
//
////交易验证码－－TAC码
////	mtclprn_charline_print("\x1Bw0\x1BW1 交易验证码：\r");
//    htoa(RecBuff, &prec->tac[0], 4);
//    sprintf((char*)strbuff, "交易代码：%s\n", RecBuff);
//    (void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
//
//    sprintf((char*)strbuff, "-----------\n");
//    (void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
//
////交易设备编号
//    htoa(RecBuff, &DevStat.acnt_id[0], 2);
//    htoa(RecBuff+4, &DevStat.equ_id[0], 3);
//    sprintf((char*)strbuff, "\x1Bw1\x1BW1交易设备：%s\r\r", RecBuff);
//    (void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
//
////操作员号
////	mtclprn_charline_print("\x1Bw0\x1BW1 操作员号：\r");
//    htoa(RecBuff, &DevStat.cur_driver_number[1], 3);           //by liujihua
//    sprintf((char*)strbuff, "操作员号：%s\n", RecBuff);
//    (void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
//
////交易日期
////	mtclprn_charline_print("\x1Bw0\x1BW1 交易日期：\r");
//    htoa(RecBuff, (INT8U*)&prec->card_in_time, 7);
//    sprintf((char*)strbuff, "交易时间：%s\n", RecBuff);
//    (void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
///*
////顾客签名
////	mtclprn_charline_print("\x1Bw0\x1BW1 交易日期：\r");
////	htoa(RecBuff, (INT8U*)&prec->card_in_time, 7);
//    strcpy(strbuff, "\x1Bw1\x1BW1顾客签名：______________\r\r");
//    mtclprn_charline_print(strbuff);
//
//
////提示
////	mtclprn_charline_print("\x1Bw0\x1BW1 交易日期：\r");
//    strcpy(strbuff, "\r\x1Bw1\x1BW1  ※ 请妥善保管此票。※ \r\r");
//    mtclprn_charline_print(strbuff);
//*/
//
////  mtclprn_charline_print("\x1Bw0\x1BW0\r\r\r\r\r\r");
////
////  mtcl_prn_power_on();
////
////  rcode = mtclprn_print_page(pagebuff, total_dotlines);
////
////  mtcl_prn_power_off();
//
////	delay_n_ms(2000);
////	delay_n_ms(200);
//
////	init_console(CONSOLE_CHINESE);
////	KEY_init(KEY_BEEP_KEY|KEY_AUTO_EL_ON|KEY_FIFO_MODE);//开启按键声音和背光
////	(void)sm_setting(MSK_KEY_UP|MSK_KEY_DOWN); //禁止键按下或抬起来时做响应
////	EA_ucClrKeyBuf();
////	printf_debug("2");
////	delay_and_wait_key(30,0,0);
//    // 打印最后调用换页命令
//    ucRet = EA_ucLinePrinter(hDprinterHandle, 0, "\f");
//    if ( ucRet == EM_SUCCESS )
//    {
//        EA_vDisplay(2, "本次打印结束!");
//        EA_uiInkeyMs(500);
//    }
//
//}


/******************************************************************************
 函数名称：print_sale_record
 功能描述：打印售卡记录
 参数名称：输入/输出？	类型		描述
 输入		
 
 返  回  值：
 
 作      者	：于福勇
 日      期：2004-09-02
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
******************************************************************************/
void print_sale_record(void)
{
	int 		count;
    INT8U		RecBuff[70];
	INT8U		strbuff[50];
	INT8U       rec[57];

	RECORD * prec = (RECORD*)rec;
	double	fBuf = 0.00;
	uchar   ucOpenID = 0;
	uchar   ucRet;
	uint    uiReadCnt;
	uint    uiRetCnd;

	memset(rec, 0, sizeof(rec));

	EA_vCls();
	//读记录有效条数
	if ( EA_ucPFOpen( (uchar *)currecFileName, &ucOpenID) != EM_ffs_SUCCESS )
	{
		lcddisperr("打开记录文件失败");
		EA_ucPFClose(ucOpenID);
		return;
	}
	// 获取剩余可写记录数
	(void)EA_ucPFInfo(ucOpenID, EM_ffs_VALIDREC, &uiReadCnt);
	if( uiReadCnt == 0)
	{
		lcddisperr("    没有记录信息    ");
		EA_ucPFClose(ucOpenID);
		return;//表示没有记录
	}
	count = uiReadCnt;//最后一条		
	ucRet = EA_ucPFReadRec(ucOpenID, count, 1, &uiRetCnd, rec);
	if(ucRet != EM_SUCCESS)
	{
		EA_ucPFClose(ucOpenID);
		return;
	}
	(void)EA_ucPFClose(ucOpenID);
	EA_vDisp(1, 1, "  打印本次加油记录  ");
	EA_vDisp(2, 1, "      请等待...     ");
	EA_uiInkeyMs(500);

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

	memset(strbuff, 0 , sizeof(strbuff));
	memset(RecBuff, 0 , sizeof(RecBuff));

	(void)EA_ucPrinterAutoFeeding(hDprinterHandle);
///打印头
	sprintf((char*)strbuff, "   淄博市公共汽车公司\n");
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	sprintf((char*)strbuff, "   客 户 联\n");
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	sprintf((char*)strbuff, "----------------------\n");
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
//卡号
	//htoa(RecBuff, &prec->bus_number[0], 3);
	memcpy(RecBuff, &prec->bus_number[0], 5);
	//strcpy((void *)RecBuff, (void *)&prec->bus_number[0]);
	sprintf((char*)strbuff, "   车    号：鲁C-%s\n", RecBuff);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
//燃料品种
//	htoa(RecBuff, &prec->bus_number[0], 5);
//  memcpy(RecBuff, &prec->fuel_type, 1);
	if(prec->fuel_type == 0)
	{
		sprintf((char*)strbuff, "   燃    料：天然气\n");
	}
	else if(prec->fuel_type == 1)
	{
		sprintf((char*)strbuff, "   燃    料：汽油93#\n");
	}
	else if(prec->fuel_type == 2)
	{
		sprintf((char*)strbuff, "   燃    料：汽油97#\n");
	}
	else if(prec->fuel_type == 3)
	{
		sprintf((char*)strbuff, "   燃    料：柴油\n");
	}
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);

//加油升数
	if( prec->fuel_type == 0 )
	{
		fBuf= (double)(prec->capacity/100.00);
		sprintf((char*)strbuff, "   加气体积：%6.2lf立方\n", fBuf);
		(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	}
	else
	{
		fBuf= (double)(prec->capacity/100.00);
		sprintf((char*)strbuff, "   加油体积：%6.2lf升\n", fBuf);
		(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	}

//加油余额
	fBuf= (double)(prec->fare)/100;
	sprintf((char*)strbuff, "   消费金额：%6.2lf元\n", fBuf);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);

//油品单价
	fBuf= (double)(prec->price)/100;
	sprintf((char*)strbuff, "   油品单价：%6.2lf元\n", fBuf);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	

//员工卡卡号
	memcpy(RecBuff, &prec->driver_no[0], 3);
	sprintf((char*)strbuff, "   员 工 号：%02X%02X%02X\n", RecBuff[0], RecBuff[1], RecBuff[2]);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	
//交易序号
	sprintf((char*)strbuff, "   交易序号：%08d\n", prec->pos_purchase_serial_num[0]*65535+prec->pos_purchase_serial_num[1]*256+prec->pos_purchase_serial_num[2]);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);

	sprintf((char*)strbuff, "----------------------\n");
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);

//交易设备编号
	htoa(RecBuff, &prec->equ_id[0], 3);
	sprintf((char*)strbuff, "   设备编号：%s\n", RecBuff);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	htoa(RecBuff, &prec->acnt_id[0], 2);
	sprintf((char*)strbuff, "   站点编号：%s\n", RecBuff);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);

//操作员号

	memcpy(RecBuff, &prec->oper_id[0], 3);
	sprintf((char*)strbuff, "   加油员号：%02X%02X%02X\n", RecBuff[0], RecBuff[1], RecBuff[2]);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);

//交易日期
//  htoa(RecBuff, (INT8U*)&prec->card_in_time, 7);
	memcpy(RecBuff, (INT8U*)&prec->card_in_time, 7);
	sprintf((char*)strbuff, "   加油时间：\r\n");
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	sprintf((char*)strbuff, "   %02X%02X-%02X-%02X %02X:%02X:%02X\r\n", 
			RecBuff[0],RecBuff[1],RecBuff[2],RecBuff[3],RecBuff[4],RecBuff[5],RecBuff[6]);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);

	sprintf((char*)strbuff, "----------------------\n");
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	// 打印最后调用换页命令
	sprintf((char*)strbuff, "\n\n\n");
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
//////////////////////////////////////////////////////////////////////////////////////////////////////////
////商户
//////////////////////////////////////////////////////////////////////////////////////////////////////////
	SleepMs(2500);

	memset(strbuff, 0 , sizeof(strbuff));
	memset(RecBuff, 0 , sizeof(RecBuff));
	sprintf((char*)strbuff, "   淄博市公共汽车公司\n");
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	sprintf((char*)strbuff, "   商 户 联\n");
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);


	sprintf((char*)strbuff, "----------------------\n");
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
//卡号
//  htoa(RecBuff, &prec->bus_number[0], 3);
	memcpy(RecBuff, &prec->bus_number[0], 5);
	//strcpy((void *)RecBuff, (void *)&prec->bus_number[0]);
	sprintf((char*)strbuff, "   车    号：鲁C-%s\n", RecBuff);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);

//燃料品种
	if(prec->fuel_type == 0)
	{
		sprintf((char*)strbuff, "   燃    料：天然气\n");
	}
	else if(prec->fuel_type == 1)
	{
		sprintf((char*)strbuff, "   燃    料：汽油93#\n");
	}
	else if(prec->fuel_type == 2)
	{
		sprintf((char*)strbuff, "   燃    料：汽油97#\n");
	}
	else if(prec->fuel_type == 3)
	{
		sprintf((char*)strbuff, "   燃    料：柴油\n");
	}
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);


//加油升数
	if( prec->fuel_type == 0 )
	{
		fBuf= (double)(prec->capacity)/100;
		sprintf((char*)strbuff, "   加气体积：%6.2lf立方\n", fBuf);
		(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	}
	else
	{
		fBuf= (double)(prec->capacity)/100;
		sprintf((char*)strbuff, "   加油体积：%6.2lf升\n", fBuf);
		(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	}

//加油余额
	fBuf= (double)(prec->fare)/100;
	sprintf((char*)strbuff, "   消费金额：%6.2lf元\n", fBuf);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);

//油品单价
	fBuf = (double)(prec->price)/100.0;
	sprintf((char*)strbuff, "   油品单价：%6.2lf元\n", fBuf);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);

//员工卡卡号
	memcpy(RecBuff, &prec->driver_no[0], 3);
	sprintf((char*)strbuff, "   员 工 号：%02X%02X%02X\n", RecBuff[0], RecBuff[1], RecBuff[2]);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);

//交易序号
	sprintf((char*)strbuff, "   交易序号：%08d\n", prec->pos_purchase_serial_num[0]*65535+prec->pos_purchase_serial_num[1]*256+prec->pos_purchase_serial_num[2]);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	sprintf((char*)strbuff, "----------------------\n");
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);

//交易设备编号
	htoa(RecBuff, &prec->equ_id[0], 3);
	sprintf((char*)strbuff, "   设备编号：%s\n", RecBuff);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	htoa(RecBuff, &prec->acnt_id[0], 2);
	sprintf((char*)strbuff, "   站点编号：%s\n", RecBuff);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);

//操作员号
	memcpy(RecBuff, &prec->oper_id[0], 3);
	sprintf((char*)strbuff, "   加油员号：%02X%02X%02X\n", RecBuff[0], RecBuff[1], RecBuff[2]);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);

//交易日期
	memcpy(RecBuff, (INT8U*)&prec->card_in_time, 7);
	sprintf((char*)strbuff, "   加油时间：\r\n");
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	sprintf((char*)strbuff, "   %02X%02X-%02X-%02X %02X:%02X:%02X\r\n", 
			RecBuff[0],RecBuff[1],RecBuff[2],RecBuff[3],RecBuff[4], RecBuff[5],RecBuff[6]);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);

	sprintf((char*)strbuff, "----------------------\n");
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	// 打印最后调用换页命令
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)"   备注");
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)"\n\n\n\n");
	ucRet = EA_ucLinePrinter(hDprinterHandle, 0, "\f");

	if ( ucRet == EM_SUCCESS )
	{
		EA_vDisplay(2, "    本次打印结束!   ");
		EA_uiInkeyMs(500);
	}
	
}


// *****************************************************************
// 功能：		GPRS_Input_IP
// 说明:		手工输入IP地址
// 入口参数：	
// 出口参数：	Null
// 作者：
// 返回值：	-1 关机							
// *****************************************************************
INT8U GPRS_Input_IP(void)
{

	INT8U IP_addr[20];
	INT8U input[20];
	int   IP[4],i, j;

	EA_vCls();
	EA_ucClrKeyBuf();

	//增加密码修改功能
	switch (PasswordCheck())
	{
		case notok:
			printf_debug((void *)"密码错误");
			return 1;
		case 2:
			return 1;
		case ok:
			EA_ucClrKeyBuf();
			break;
		default:
			return 1;
	}

	j = 0;

	EA_vCls();
	EA_vDisp(1, 1, "原服务器地址为:");
	EA_vDisp(3, 1, "请输入新服务器地址:");
	sprintf((char *)IP_addr, "%d.%d.%d.%d", DevStat.ServerIp[0], DevStat.ServerIp[1], DevStat.ServerIp[2], DevStat.ServerIp[3]);
	EA_vDisp(2, 1, (char *)IP_addr);

	EA_vDisplay(4, "   .   .   .      ");
//  IP_addr_1[15] = '\0';
//  EA_vDisp(4, 1, (void *)IP_addr_1);
	for ( ;; )
	{
		i = EA_ucGetInputStr(4, 1, 20, EM_BIG_FONT | EM_MODE_CHAR | EM_ALIGN_LEFT | EM_SHOW_ORIGINALLY
							 , 7, 15, 0, (char *)input);
		if ( i == EM_SUCCESS )
			break;
		if ( i == EM_ABOLISH )
			return notok;
	}

	//if (ReadNumeric_(IP_addr_1, 15, 0, 2) < 0) return;
//  sprintf((uint *)input, "%03d.%03d.%03d.%03d", (void *)&IP_addr_1);
	if(strlen((void *)input) < 7)
	{
		lcddisperr("IP地址输入格式错误");
		return notok;
	}
	EA_vDisp(4, 1, (void *)input);
	sscanf((void *)input, "%d.%d.%d.%d", &IP[0], &IP[1], &IP[2], &IP[3]);
	for(i = 0; i < 4; i++)
	{
		if( (IP[i] > 255) || (IP[i] < 0) )
		{
			lcddisperr("IP地址输入格式错");
			return notok;
		}
	}

	for(i=0;i<4;i++)
	{
		DevStat.ServerIp[i] = IP[i];
	}
	DevStat.ServerPort[0]=0x55;
	DevStat.ServerPort[1]=0x55;
//	memset(IP_addr, '\0', 16);
//	sprintf(IP_addr, "%d.%d.%d.%d", iTmp[0], iTmp[1], iTmp[2], iTmp[3]);
 /*
PORT_AGAIN:
	DispStr_CE(0, 0, "请输入端口号:", DISP_POSITION | DISP_CLRSCR);
	memcpy(IP_addr_1, "     ", 5);
	IP_addr_1[5] = '\0';
	if (ReadNumeric_(IP_addr_1, 5, 24, 2) < 0) return;
	iport = atoi(IP_addr_1);
	if( iport > 65535 )
	{
//		DispStr_CE(0, 6, "端口号输入有误", DISP_POSITION);
//		PressAnyKey();
		printf_debug("");
		goto PORT_AGAIN;
	}
//	memset(Comm_port, '\0', 16);
//	sprintf(Comm_port, "%d", iport);
	DevStat.ServerPort[0] = (INT8U)(iport>>8);
	DevStat.ServerPort[1] = (INT8U)iport;
*/
	WriteParam();
	return ok;
}

/******************************************************************************
 函数名称：print_work_note
 功能描述：打印日结算票据
 参数名称：输入/输出？	类型		描述
 输入		
 
 返  回  值：
 
 作      者	：于福勇
 日      期：2004-09-02
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
******************************************************************************/
void print_work_note(void)
{
	INT8U		RecBuff[70];
	INT8U		strbuff[50];
	INT8U		time[7+2];
	uchar     	ucRet;

	EA_vCls();
	EA_vDisplay(1,"    日结帐单打印    ");
	EA_vDisplay(2,"      请等待...     ");
	SleepMs(1000);

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

	sprintf((char*)strbuff, "  淄博市公共汽车公司\n");
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	sprintf((char*)strbuff, "  日结帐单\n");
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);


	sprintf((char*)strbuff, "-----------------------------------------\n");
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	
//消费次数
	sprintf((char*)strbuff, "  天然气加气次数 ：%6d次\r\n", DevStat.binhand.consume_cnt[0]);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	sprintf((char*)strbuff, "  汽油93#加油次数：%6d次\r\n", DevStat.binhand.consume_cnt[1]);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	sprintf((char*)strbuff, "  汽油97#加油次数：%6d次\r\n", DevStat.binhand.consume_cnt[2]);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	sprintf((char*)strbuff, "  柴油加油次数   ：%6d次\r\n", DevStat.binhand.consume_cnt[3]);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	sprintf((char*)strbuff, "-----------------------------------------\n");
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
//加油体积
	sprintf((char*)strbuff, "  天然气体积 ：%6.2lf立方\r\n", (double)((DevStat.binhand.consume_cap[0])/100.0));
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	sprintf((char*)strbuff, "  汽油93#体积：%6.2lf升\r\n", (double)((DevStat.binhand.consume_cap[1])/100.0));
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	sprintf((char*)strbuff, "  汽油97#体积：%6.2lf升\r\n", (double)((DevStat.binhand.consume_cap[2])/100.0));
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	sprintf((char*)strbuff, "  柴 油 体 积：%6.2lf升\r\n", (double)((DevStat.binhand.consume_cap[3])/100.0));
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	sprintf((char*)strbuff, "-----------------------------------------\n");
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	//消费金额
	sprintf((char*)strbuff, "  天然气金额 ：%6.2lf元\r\n", (double)((DevStat.binhand.consume_amt[0])/100.0));
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	sprintf((char*)strbuff, "  汽油93#金额：%6.2lf元\r\n", (double)((DevStat.binhand.consume_amt[1])/100.0));
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	sprintf((char*)strbuff, "  汽油97#金额：%6.2lf元\r\n", (double)((DevStat.binhand.consume_amt[2])/100.0));
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	sprintf((char*)strbuff, "  柴 油 金 额：%6.2lf元\r\n", (double)((DevStat.binhand.consume_amt[3])/100.0));
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	sprintf((char*)strbuff, "-----------------------------------------\n");
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);

	sprintf((char*)strbuff, "  总  金  额 ：%6.2lf元\r\n", (double)((DevStat.binhand.consume_amt[0]+DevStat.binhand.consume_amt[1]+
														   DevStat.binhand.consume_amt[2]+DevStat.binhand.consume_amt[3])/100.0));
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
//上班时间	
	memcpy(time, (INT8U*)&DevStat.binhand.work_datetime, 7);
	sprintf((void *)strbuff, "上班时间：\r\n");
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	sprintf((void *)strbuff, "%02X%02X-%02X-%02X %02X:%02X:%02X\r\n", time[0],time[1],time[2],time[3],time[4],time[5],time[6]);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);	
//下班时间
	memcpy(time, (INT8U*)&DevStat.binhand.down_datetime, 7);
	sprintf((void *)strbuff, "下班时间：\r\n");
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	sprintf((void *)strbuff, "%02X%02X-%02X-%02X %02X:%02X:%02X\r\n", time[0],time[1],time[2],time[3],time[4],time[5],time[6]);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);	


//分行符号
	sprintf((char*)strbuff, "-----------------------------------------\n");
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char *)strbuff);
	
//设备编号
	htoa(RecBuff, &DevStat.equ_id[0], 3);
	sprintf((char*)strbuff, "  设 备 编 号：%s\r\n", RecBuff);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char *)strbuff);
//商户号
	htoa(RecBuff, &DevStat.acnt_id[0], 2);	
	sprintf((char*)strbuff, "  站 点 编 号：%s\r\n", RecBuff);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char *)strbuff);
//操作员号
	htoa(RecBuff, &DevStat.cur_driver_number[0], 1);	   // by liujihua
	sprintf((char*)strbuff, "  加油员工号 ：%s\r\n", RecBuff);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);

	// 打印最后调用换页命令
	ucRet = EA_ucLinePrinter(hDprinterHandle, 0, "\f");
	if ( ucRet == EM_SUCCESS )
	{
		EA_vDisplay(2, "    本次打印结束!   ");
		EA_uiInkeyMs(2000);
	}
	return ;
}



