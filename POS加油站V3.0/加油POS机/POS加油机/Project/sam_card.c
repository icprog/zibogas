/******************************************************************************
Copyright 2004 大连现代高技术发展有限公司
All rights riserved.

文件名	：sam_card.c
模块名称：PSAM卡控制和通讯函数
功能概要：

取代版本：0.0.1
修改人	：许岩
完成日期：2004.09.09
升级说明：create

******************************************************************************/
#include "main.h"


//向PSAM卡发命令用到的一些code变量
INT8U const       ReadPOSNumber[5]=         {0x00,0xB0,0x96,0x00,0x06};     
INT8U const       GetChallenge[5]=          {0x00,0x84,0x00,0x00,0x04};		//取POS号
INT8U const       SELECT_SAM_ADF1[7]=      {0x00,0xA4,0x00,0x00,0x02,0x10,0x01};		//选1001目录
INT8U const       SELECT_JDSAM_ADF1[7]=      {0x00,0xA4,0x00,0x0C,0x02,0x10,0x01};		//选1001目录

INT8U const       SELECT_SAM_ADF2[7]=      {0x00,0xA4,0x00,0x00,0x02,0x10,0x02};		//选1002目录
INT8U const       SELECT_SAM_ADF3[7]=      {0x00,0xA4,0x00,0x00,0x02,0x10,0x03};		//选1003目录
INT8U const       DES_INIT[5]=                     {0x80,0x1A,0x45,0x01,0x10};							//DES计算初始化
INT8U const      GetResponse[4]=                {0x00,0xC0,0x00,0x00};									//取响应
INT8U const      DES_CRYPT[4]=                   {0x80,0xFA,0x05,0x00};									 //DES计算
INT8U const      READ_SAM_KEY_NUM[5]=   {0x00,0xb0,0x97,0x00,0x19};						 //读密钥版本号
INT8U const      GET_MAC1[5]=                     {0x80,0x70,0x00,0x00,0x24};						  //PSAM卡算MAC1
INT8U const      VERIFY_MAC2[5]=                {0x80,0x72,0x00,0x00,0x04};							//验证MAC2
INT8U const      GET_M1_KEY[5]=                 {0x80,0xFC,0x01,0x01,0x11};							//计算M1卡个区密钥
INT8U const      TAC_INIT[5]=                       {0x80,0x1A,0x44,0x01,0x10};							//M1卡交易流程中，TAC码计算初始化
//INT8U const      TAC_INIT[5]=                       {0x80,0x1A,0x24,0x01,0x08};							//M1卡交易流程中，TAC码计算初始化
INT8U const      TAC_JDINIT[5]=                       {0x80,0x1A,0x24,0x07,0x08};							//M1卡交易流程中，TAC码计算初始化
INT8U const      CAL_TAC_M1[5]=                 {0x80,0xFA,0x01,0x00,0x18};							//M1卡交易过程中，计算TAC码
INT8U const      DESCOMMUNICATION1[5]=   {0x80,0xfa,0x05,0x00,0x20};						//CPU卡写0017文件
INT8U const      DESCOMMUNICATION[4]=   {0x80,0xfa,0x05,0x00};									//DES计算
INT8U const		 ISAM_CAL_KEY_INIT[5]={0x80,0x1A,0x28,0x01,0x08};//isam卡计算密钥初始化
INT8U const      ISAM_CAL_KEY[5]={0x80,0xFA,0x00,0x00,0x08};//ISAM卡计算充值密钥



//城市通卡
INT8U const       SELECT_SAM_3F01[7]=       {0x00,0xA4,0x00,0x00,0x02,0x3F,0x01};		//选1001目录
INT8U const       GET_M1_FUZA_KEY[5]=       {0x80,0xFC,0x0E,0x0D,0x19};							//计算M1卡个区密钥
INT8U const       TAC_FUZA_INIT[5]=         {0x80,0x1A,0x44,0x01,0x10};							//M1卡交易流程中，TAC码计算初始化
INT8U const       CAL_TAC_M1_FUZA[5]=       {0x80,0xFA,0x01,0x00,0x18};							//M1卡交易过程中，计算TAC码
//INT8U const   code    GET_MI_FUZA_KEY_YINZI[8]= {0xD1,0xCC,0xCC,0xA8,0x00,0x00,0x00,0x00};                          //M1卡交易过程中，计算TAC码
INT8U const       GET_MI_FUZA_KEY_YINZI[8]= {0xD7,0xCD,0xB2,0xA9,0x00,0x00,0x00,0x00};                          //M1卡交易过程中，计算TAC码
INT8U const       FUZHA_TAC_INIT[5]=        {0x80,0x1A,0x48,0x0A,0x10};							//M1卡交易流程中，TAC码计算初始化
INT8U const       FUZHA_CAL_TAC_M1[5]=      {0x80,0xFA,0x05,0x00,30};							//M1卡交易过程中，计算TAC码





//extern volatile INT8U idata OutPortStat;					//273扩展输出的状态寄存器
extern DEV_STAT     DevStat;					//设备状态
extern DevHandle hSam1Handle;      //SAM1卡
extern DevHandle hSam2Handle ;     //SAM2卡
extern CARD_INFO    CardInfo;	

/******************************************************************************
 函数名称：CmdPsam
 功能描述：使用T=0协议与PSAM卡进行通讯。比烟台读写器减少了0x6C(发送长度不对)的处理
 参数描述：
 参数名称：	输入/输出？	类型		描述
 psam_handle		输入	PSAM卡的句柄
 snd_buf			输入	INT8U *			发送缓冲区
 snd_length			输入	INT8U			要发送命令的长度
 rcv_buf			输出	INT8U *			PSAM卡送回的字节缓冲区
 rcv_length			输出	INT8U *			PSAM卡送回的字节长度
				
 返  回  值：ok ?  notok
				   
 作      者	：许岩/刘及华
 日      期：2012-12-02
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
******************************************************************************/
INT8U CmdPsam(DevHandle Cardhandle, INT8U  * const snd_buf, uint const snd_length,
			   INT8U *rcv_buf, uint *rcv_length)
{
	uchar result;
//  (void)EA_ucSetStopFlag( EM_DISABLE_STOP );   //允许进入节能 状态
	result = EA_ucICExchangeAPDU(Cardhandle, snd_length, snd_buf, rcv_length, rcv_buf);
	if( result != EM_SUCCESS )
	{
//  	EA_vCls();
//  	EA_vDisplay(2, "错误类型：%x", result);
//  	EA_uiInkey(1);
		return notok;
	}
//  (void)EA_ucSetStopFlag( EM_ENABLE_STOP );   //允许进入节能 状态
	return ok;
}

/******************************************************************************
 函数名称：PsamRst
 功能描述：复位PSAM卡
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
INT8U PsamRst(void)
{
	INT8U rcv_buf[100];
	uint  rcv_length;
	INT8U ret;
	uint  len;
	uchar aucATR[100];
	uchar prot;

	(void)EA_ucICPowerDown(hSam1Handle);
	(void)EA_ucICPowerUp(hSam1Handle, &len, aucATR, &prot);
//  (void)EA_ucSetStopFlag(EM_DISABLE_STOP);   //不允许进入节能 状态
	////////////////////////////////////////
	//PSAM卡取POS号
	////////////////////////////////////////
	memset(rcv_buf, 0, sizeof(rcv_buf));
	ret = CmdPsam(hSam1Handle, (INT8U *)ReadPOSNumber, 5, rcv_buf, &rcv_length);       //取POS号
	if (ret != ok )
	{
		(void)EA_ucSetStopFlag(EM_ENABLE_STOP);   //允许进入节能 状态
		return notok;
	}
	memcpy(&DevStat.pos_number[0], rcv_buf, 6);     //存POS号
	WriteParam();
//  (void)EA_ucSetStopFlag(EM_ENABLE_STOP);   //允许进入节能 状态
	return ok;

}

/******************************************************************************
 函数名称：SamCheck
 功能描述：检测SAM卡，并读取16文件
 参数描述：
 参数名称：	输入/输出？	类型		描述
 参    数:  无
				
 返  回  值：ok(0)-成功
				 notok(0xFF)-失败
				   
 作      者	：许岩/刘及华
 日      期：2012-11-28
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
******************************************************************************/
INT8U SamCheck(void)
{
	uchar result = 0;
	uchar prot = 0;
	uint  len = 0;
	uchar aucDataOut[100];
	uchar aucAtrData[50];
	ET_CPUCOM  apducomm;
	ET_CPURESP apduresp;

	result = EA_ucICInitModule ( hSam1Handle, EM_ic_VOL5, EM_ic_384MODE );  //EM_ic_384MODE
	if (result != EM_SUCCESS)
	{
//  		EA_vDisplay(2, 1, "供电类型为:%d", i);
//  		break;
		lcddisperr("  PSAM卡初始化失败  ");
		return notok;
	}
	// 给卡上电
	//(void)EA_uiInkey(0);
	result = EA_ucICPowerUp(hSam1Handle, &len, aucAtrData, &prot);
	if( result !=  EM_SUCCESS )
	{
		  lcddisperr("   PSAM卡上电失败   ");
//  	  (void)EA_vDisplay(2, "协议类型:%02X", prot);
//  	  (void)EA_vDisplay(3, "协议类型:%02X", result);
//  	  (void)EA_vDelay(5);
     	  goto releaseOperate;
	}
//  (void)EA_ucSetStopFlag(EM_DISABLE_STOP);   //不允许进入节能 状态
	// 执行选主文件操作
	apducomm.CLA=0x00;
	apducomm.INS=0xa4;
	apducomm.P1=0x00;
	apducomm.P2=0x00;
	apducomm.Lc=0x02;
	apducomm.Le=0x00;
	apducomm.LeFlag=0x01;
	result = EA_ucICInComing (hSam1Handle , &apducomm, (uchar*)"\x3f\x00",&apduresp);
	if  ((result != EM_SUCCESS) && (result != EM_ic_SWDIFF) && (result != EM_ic_FORRESP))
	{
		   // 显示错误, 并做错误处理
		  goto releaseOperate;
	}

	// 执行取随机数命令
	apducomm.CLA=0x00;
	apducomm.INS=0x84;
	apducomm.P1=0x00;
	apducomm.P2=0x00;
	apducomm.Lc=0x00;
	apducomm.Le=0x08;
	apducomm.LeFlag=0x01;
	apduresp.LengthOut=0x00;
	apduresp.SW1=0x00;
	apduresp.SW2=0x00;
	memset(aucDataOut,0x00,100);
	result = EA_ucICOutGoing(hSam1Handle , &apducomm, &apduresp, aucDataOut);
	if  ((result != EM_SUCCESS) && (result != EM_ic_SWDIFF))
	{
		   lcddisperr("PSAM卡数据读写失败");
		   goto releaseOperate;
	}

//  (void)EA_ucSetStopFlag(EM_ENABLE_STOP);   //允许进入节能 状态
	result = EA_ucICPowerDown(hSam1Handle);
	return ok;

releaseOperate: 
	result = EA_ucICPowerDown(hSam1Handle);
	if  (result != EM_SUCCESS)
	{
		   // 显示错误, 并做错误处理
		   goto releaseOperate;
	}
//  (void)EA_ucSetStopFlag(EM_ENABLE_STOP);   //允许进入节能 状态
	 // 该卡座暂时不使用，释放其控制权
	result = EA_ucCloseDevice(&hSam1Handle);
	// 显示错误, 并做错误处
	return notok;
}

