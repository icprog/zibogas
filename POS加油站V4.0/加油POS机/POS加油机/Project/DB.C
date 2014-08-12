/*****************************************************************
模块名称：EH0218菜单
功能概要：本程序是以EH0218手持设备为基础而设计的数据库管理程序
作    者：大连金德姆电子有限公司/于福勇
日    期：2006－06－13
修改历史：
日期		修改人		修改描述
------		---------	-------------
*****************************************************************/
#include "main.h"


extern DEV_STAT 	DevStat;					//设备状态
//extern GRAYRECORD  GrayRecord;
extern CARD_INFO      CardInfo;					//卡片信息，交易记录由此变量生成
extern ERROR_CARD_INFO ErrCardInfo;				//出错卡状态

extern const char parmFileName[];
extern const char blackFileName[];
extern const char currecFileName[];
extern const char conrecFileName[];
extern const char hisrecFileName[];
extern const char grayrecFileName[];

// *****************************************************************
// 功能：		保存刷卡记录
// 入口参数：	
// 出口参数：	Null
// 作者：
// 返回值：		0://ok,保存成功			
//				1;//记录已满，保存失败
// *****************************************************************
INT8U SaveCardRecord(RECORD * CardRecord)
{ 
	uint    uiRetNum;
	uchar   ucOpenID = 0;	
	uchar   i;
	// 打开当天交易记录文件
	i = EA_ucPFOpen((uchar *)currecFileName, &ucOpenID);

	if ( i == EM_ffs_NOTEXIST )
	{
		 EA_ucPFCreateLoop((uchar *)currecFileName, sizeof(RECORD), 2000);  //2000条循环记录;
		 i == EA_ucPFOpen((uchar *)currecFileName, &ucOpenID);
		 if( i != EM_SUCCESS )
		 {
			lcddisperr("打开记录文件失败!");
			return notok;
		 }
	}

//  if ( EA_ucPFOpen((uchar *)currecFileName, &ucOpenID) != EM_ffs_SUCCESS )
//  {
//  	lcddisperr("打开记录文件失败");
//  	(void)EA_ucPFClose(ucOpenID);
//  	return notok;
//  }

	if( EA_ucPFWriteRec(ucOpenID, 0, (uchar *)CardRecord) != EM_SUCCESS )
	{	
		(void)EA_ucPFClose(ucOpenID);
		lcddisperr("添加记录文件失败");
		return notok;
	}
	if ( EA_ucPFInfo(ucOpenID, EM_ffs_VALIDREC, &uiRetNum) != EM_ffs_SUCCESS )
	{
		lcddisperr("读取记录文件失败");
		(void)EA_ucPFClose(ucOpenID);
		return notok;
	}
	(void)EA_ucPFClose(ucOpenID);

	DevStat.record_number = uiRetNum;

	// 打开历史交易记录文件
	if ( EA_ucPFOpen((uchar *)hisrecFileName, &ucOpenID) != EM_ffs_SUCCESS )
	{
		lcddisperr("打开历史文件失败");
		(void)EA_ucPFClose(ucOpenID);
		return notok;
	}

	if( EA_ucPFWriteRec(ucOpenID, 0, (uchar *)CardRecord) != EM_SUCCESS )
	{
		(void)EA_ucPFClose(ucOpenID);
		lcddisperr("添加历史文件失败");
		return notok;
	}

	(void)EA_ucPFClose(ucOpenID);

	return ok;		            				    	  			  				   			 			       
}



/*****************************************************************
函数原型：BlackName_Find
 功能描述：查找黑名单函数
 参数描述：
 参数名称：	输入/输出？	类型		描述
		card_number 		输入		INT32U			要查找的卡号，4字节
				
 返  回  值：	ok(0)-找到，是黑名单
				notok(0xFF)-没找到，不是黑名单
 
 作      者	：许岩
 日      期：2003-12-19
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
*****************************************************************/
INT8U BlackName_Find(char *card_number)
{
	INT32U 		black_name_number;
	ulong   	ulRetNum;
	uint		uiOpenID;
	uchar		ucRet;

//  return ok;   //frank debug, 正式版本应该删除。

	if ( EA_ucBlkOpen((char *)blackFileName, &uiOpenID) != EM_ffs_SUCCESS )
	{
		lcddisperr("打开白名单文件失败");
		return notok;
	}
	if ( EA_ucBlkGetFileInfo(uiOpenID, EM_bfs_GET_VALID_RECORD, &ulRetNum) != EM_ffs_SUCCESS )
	{
		lcddisperr("读取白名单文件失败");
		return notok;
	}

	black_name_number = ulRetNum;

//  if( black_name_number == 0)
//  {
//  	EA_vDisplay(3, "   没有白名单记录   ");
//  	EA_vDisplay(4, "  请手动下载白名单  ");
//  	SleepMs(2500);
//  	(void)EA_ucBlkClose(uiOpenID);
//  	return notok;      //没有白名单记录
//  }

	ucRet = EA_ucBlkSearch(uiOpenID, card_number);
	(void)EA_ucBlkClose(uiOpenID);
	switch (ucRet)
	{
		case EM_bfs_FIND:         // 找到记录
			   return ok;
		case EM_bfs_NOT_FIND:     // 未找到记录   
			   return notok; 
	    default:
		   break;
	}
	return notok;
}

/******************************************************************************
 函数名称：GetBlackName
 功能描述：得到指定条号的黑名单，只读主区
 参数名称：	输入/输出？	类型		描述
 black			输出		INT8U*		返回的黑名单卡号
 black_num	输入	INT16U		要读取黑名单的条号0-7999
 
 返  回  值：无
 
 作      者	：xuhongliang
 日      期：2004-12-08
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
******************************************************************************/
void GetBlackName(INT8U *black, INT16U black_num)
{
	INT8U   p[4];
	uint    uiOpenID = 0;

	if ( EA_ucBlkOpen((char *)blackFileName, &uiOpenID) != EM_ffs_SUCCESS )
	{
		lcddisperr("打开白名单失败");
		return ;
	}
	if ( EA_ucBlkSearch(uiOpenID, black) != EM_bfs_FIND  )
	{
		lcddisperr("获取白名单失败");
		(void)EA_ucBlkClose(uiOpenID);
		return ;
	}
	 
//  (void)EA_ucPFReadRec(ucOpenID, black_num, 1, &uiReadCnt, p);
//  p = (INT8U *)DB_jump_to_record(DB_BLACK, black_num, &flag);//count -1 :代表记录序号
	memcpy(black, p, 4);
	(void)EA_ucBlkClose(uiOpenID);
	return;
}

