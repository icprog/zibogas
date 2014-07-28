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

//#define 	SUCCESSFUL		0x00
//#define		FAILURE			0xff

//#define DB_NUM  6					//数据库数量

//#define DEBUG

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

//how many blocks of every db table 
//short g_blocknum[DB_NUM] = {1,1,5,5,10,1};						//每个数据库所占用的sector数量，一个sector64K
//every record's size of db table  

/*INT16U g_rec_size[DB_NUM] = {&DevStat.end - &DevStat.start,		// 参数表												   
							sizeof(BlackCardInfo), 	//黑名单表
							sizeof(RECORD), 	//本次记录表
							sizeof(CONSUMERECORD),//sizeof(RECORD),	//正常补采表
							sizeof(RECORD),//历史记录
							sizeof(GRAYRECORD)};//   灰记录表
*/
//unsigned char *g_strTableName[DB_NUM]= {"参数表","黑名单表","当前记录表","消费表","历史记录表","灰记录表"};
//unsigned char *g_strTableName[DB_NUM]= {"参数表","黑名单表","当前记录表","补采记录表","历史记录表"};

//NVARR(CardInfoOne strCardInfoRead);//定义全局的消费记录存储结构体
//NVARR(BlackCardInfo strBlackCardInfo);//定义全局的黑名单记录存储结构体
//extern ParamCardInfo strParamInfo;
//extern DriverParam   strDriverInfo;
//
///******************************************************************************
// 函数名称：StoreGrayRecord
// 功能描述：灰记录存储函数
// 参数名称： 输入/输出？ 类型        描述
//
// 返  回  值：无
//
// 作      者 ：于福勇
// 日      期：2007-04-29
// 修改历史：
//        日期        修改人      修改描述
//******************************************************************************/
//
//INT8U StoreGrayRecord(void)
//{
//
//
////	memset(GrayRecord, 0x00, sizeof(GRAYRECORD));
//
//    //////////////////////////////////////////
//    //组织交易记录格式，相同部份
//    //////////////////////////////////////////
//    GrayRecord.card_type = CardInfo.card_type;//卡类型1字节
//    GrayRecord.card_purchase_type = CardInfo.card_purchase_type + 0x90;//交易类型 1字节
//    GrayRecord.pos_purchase_serial_num[0] = DevStat.dev_trade_no[0];    //终端机交易序号
//    GrayRecord.pos_purchase_serial_num[1] = DevStat.dev_trade_no[1];
//
//    memcpy(&GrayRecord.card_serial_number[0], &CardInfo._CARD_NUMBER.card_number[0], 8);//城市代码2字节+行业代码2字节+卡发行流水号4字节
//    memcpy((void *)&GrayRecord.card_in_time, &CardInfo.card_in_time, 7);//交易日期4字节和时间 3字节
//    memcpy(&GrayRecord.tac[0], "\0x00\0x00\0x00\0x00", 4);//TAC码 4字节
//    memcpy(&GrayRecord.isam_no[0], &DevStat.pos_number[0], 6);//终端机编号
////  memcpy(&GrayRecord.balance[0], ((INT8U *)(&CardInfo.init_balance))+1, 3); //交易前余额 3字节
//    GrayRecord.balance[0] = (INT8U)(CardInfo.init_balance / 0x10000);
//    GrayRecord.balance[1] = (INT8U)(CardInfo.init_balance % 0x10000 / 0x100);
//    GrayRecord.balance[2] = (INT8U)(CardInfo.init_balance % 0x100);
////  memcpy(&GrayRecord.fare[0], ((INT8U *)(&CardInfo.fare))+1, 3);	//交易金额 3字节
//    GrayRecord.fare[0] = (INT8U)(CardInfo.fare / 0x10000);
//    GrayRecord.fare[1] = (INT8U)(CardInfo.fare % 0x10000 / 0x100);
//    GrayRecord.fare[2] = (INT8U)(CardInfo.fare % 0x100);
//    memcpy(&GrayRecord.purchase_serial_num[0], (INT8U *)(CardInfo.purchase_serial_num), 2); //用户卡脱机交易序号 2字节
//    memcpy(&GrayRecord.oper_id[0], &DevStat.cur_driver_number[1], 3);//操作员编号3字节 by liujihua
////  GrayRecord.oper_id[0] = DevStat.cur_driver_number[2];
////  GrayRecord.oper_id[1] = DevStat.cur_driver_number[1];
////  GrayRecord.oper_id[2] = DevStat.cur_driver_number[0];
//
//    memcpy(&GrayRecord.csn[0], &CardInfo.csn[0], 4);
//    memcpy(&GrayRecord.acnt_id[0], &DevStat.acnt_id[0],2);
//    memcpy(&GrayRecord.spot_id[0], &DevStat.spot_id[0],2);
//    memcpy(&GrayRecord.equ_id[0], &DevStat.equ_id[0],3);
//    memcpy(&GrayRecord.block24_buf_to_restore[0], &ErrCardInfo.block24_buf_to_restore[0], 16);
//    //移位修改
//    GrayRecord.oper_card_amt[0] = (INT8U)(DevStat.oper_card_amt / 0x1000000);
//    GrayRecord.oper_card_amt[1] = (INT8U)(DevStat.oper_card_amt % 0x1000000 / 0x10000);
//    GrayRecord.oper_card_amt[2] = (INT8U)(DevStat.oper_card_amt % 0x10000 / 0x100);
//    GrayRecord.oper_card_amt[3] = (INT8U)(DevStat.oper_card_amt % 0x100);
//
////  GrayRecord.oper_card_amt[0] = (INT8U)(DevStat.oper_card_amt>>24);
////  GrayRecord.oper_card_amt[1] = (INT8U)(DevStat.oper_card_amt>>16);
////  GrayRecord.oper_card_amt[2] = (INT8U)(DevStat.oper_card_amt>>8);
////  GrayRecord.oper_card_amt[3] = (INT8U)DevStat.oper_card_amt;
//
//
//    WriteGrayRecord();
//
////	有未上传灰记录
//    DevStat.gray_flag = TRUE;
//    WriteParam();
//
//    return ok;
//
//}
//
//
///******************************************************************************
// 函数名称：StoreConsumeRecord
// 功能描述：交易记录存储函数
// 参数名称： 输入/输出？ 类型        描述
// cardinfo   输入            CARD_INFO*  交易完毕后的卡片的所有信息
//
// 返  回  值：无
//
// 作      者 ：许岩/刘及华
// 日      期：2012-12-12
// 修改历史：
//        日期        修改人      修改描述
//        2005.2.18       myron                        in function
//******************************************************************************/
//INT8U StoreConsumeRecord(CARD_INFO *cardinfo)
//{
//    INT8U  record[34];
//    INT32U temp_int32u = 0;
//    INT16U crc = 0;
//    INT8U  ucOpenID  = 0;
//    INT32U uiRetNum;
//    uint   uiReadCnt;
//
//    CONSUMERECORD *prec = (CONSUMERECORD *)record;
//
//    memset(record, 0x00, sizeof(record));
//
//    //////////////////////////////////////////
//    //组织交易记录格式，相同部份
//    //////////////////////////////////////////
//    prec->card_type = cardinfo->card_type;//卡类型1字节
//    prec->card_purchase_type = cardinfo->card_purchase_type;//交易类型 1字节
//
////  temp_int32u = DB_count_records(DB_REC);     //读3个字节的记录交易序列号
//    // 打开文件
//    if ( EA_ucPFOpen((uchar *)conrecFileName, &ucOpenID) != EM_ffs_SUCCESS )
//    {
//        lcddisperr("打开补采记录文件失败");
//        (void)EA_ucPFClose(ucOpenID);
//        return notok;
//    }
//    if ( EA_ucPFInfo(ucOpenID, EM_ffs_VALIDREC, &uiRetNum) != EM_ffs_SUCCESS )
//    {
//        lcddisperr("读取补采记录文件失败");
//        (void)EA_ucPFClose(ucOpenID);
//        return notok;
//    }
//    temp_int32u = uiRetNum;
////  prec->pos_purchase_serial_num[0] = (INT8U)(temp_int32u >> 16);	//终端机交易序号
////  prec->pos_purchase_serial_num[1] = (INT8U)(temp_int32u >> 8);
////  prec->pos_purchase_serial_num[2] = (INT8U)temp_int32u;
//    prec->pos_purchase_serial_num[0] = (INT8U)(temp_int32u / 0x10000);  //终端机交易序号
//    prec->pos_purchase_serial_num[1] = (INT8U)(temp_int32u % 0x10000 / 0x100);
//    prec->pos_purchase_serial_num[2] = (INT8U)(temp_int32u % 0x100);
//    (void)EA_ucPFReadRec(ucOpenID, temp_int32u, 1, &uiReadCnt, record);   //读三个字节
//
//    memcpy(&prec->card_serial_number[0], &cardinfo->_CARD_NUMBER.card_number[0], 8);//城市代码2字节+行业代码2字节+卡发行流水号4字节
//    memcpy((void *)&prec->card_in_time, &cardinfo->card_in_time, 7);//交易日期4字节和时间 3字节
//    memcpy(&prec->tac[0], &cardinfo->tac[0], 4);//TAC码 4字节
//    //关闭记录
//    if ( EA_ucPFClose(ucOpenID) != EM_ffs_SUCCESS )
//    {
//        lcddisperr("关闭补采记录文件失败");
//        return notok;
//    }
//
//    //不同部分
//    switch ( cardinfo->card_type )
//    {
//        case CARDT_NORMAL:
//        case CARDT_STUDENT:
//        case CARDT_FREE:
//        case CARDT_MEMORY:
//        case CARDT_OLDMAN:
//        case CARDT_DRIVER:
//// 		case CARDT_STUDENT_B:
//            memcpy(&prec->balance[0], ((INT8U *)(&cardinfo->balance))+1, 3); //交易后余额 3字节
//            memcpy(&prec->fare[0], ((INT8U *)(&cardinfo->fare))+1, 3);  //交易金额 3字节
//            memcpy(&prec->purchase_serial_num[0], (INT8U *)(cardinfo->purchase_serial_num), 2); //用户卡脱机交易序号 2字节
//            break;
//        default:
//            return notok;
//    }
//
//    crc = Cal_CRC(record, 32);
//
//    record[32] = (crc / 0x100);//CRC1
//    record[33] = (crc % 0x100);//CRC2
//
////#ifdef DEBUG
////    htoa(temp, record, sizeof(record));
////    TRACE232("record: ");
////    TRACE232(temp);
////    TRACE232("\n\r");
////#endif
//    if( SaveCardConsumeRecord(record) != ok )
//    {
//        lcddisperr("保存补采记录失败");
//        return notok;
//    }
//    // 打开文件
////  if ( EA_ucPFOpen((uchar *)conrecFileName, &ucOpenID) != EM_ffs_SUCCESS )
////  {
////  	lcddisperr("打开补采记录文件失败");
////  	return notok;
////  }
////  if ( EA_ucPFWriteRec(ucOpenID, 0, record) != EM_ffs_SUCCESS )
////  {
////  	lcddisperr("添加补采记录失败");
////  	return notok;
////  }
////  //关闭记录
////  if ( EA_ucPFClose(ucOpenID) != EM_ffs_SUCCESS )
////  {
////  	lcddisperr("关闭补采记录文件失败");
////  	return notok;
////  }
////
////#ifdef DEBUG
////    TRACE232("Store record success.\n\r");
////#endif
//    return ok;
//}
//
///******************************************************************************
// 函数名称：StoreRecord
// 功能描述：交易记录存储函数
// 参数名称： 输入/输出？ 类型        描述
//cardinfo	输入			CARD_INFO*	交易完毕后的卡片的所有信息
//
// 返  回  值：无
//
// 作      者 ：许岩/刘及华
// 日      期：2012-12-12
// 修改历史：
//        日期        修改人      修改描述
//        2005.2.18       myron                        in function
//******************************************************************************/
//INT8U StoreRecord(CARD_INFO *cardinfo)
//{
//
//    INT8U LOCAL_V_TYPE i = 0;
//    INT8U record[83];
//    INT16U crc = 0;
//    RECORD *prec = (RECORD *)record;
//
//    memset(record, 0x00, sizeof(record));
//
//    //////////////////////////////////////////
//    //组织交易记录格式，相同部份
//    //////////////////////////////////////////
//    prec->card_type = cardinfo->card_type;//卡类型1字节
//    prec->card_purchase_type = cardinfo->card_purchase_type;//交易类型 1字节
//
//    prec->pos_purchase_serial_num[0] = DevStat.dev_trade_no[0]; //终端机交易序号
//    prec->pos_purchase_serial_num[1] = DevStat.dev_trade_no[1];
//
//    memcpy(&prec->card_serial_number[0], &cardinfo->_CARD_NUMBER.card_number[0], 8);//城市代码2字节+行业代码2字节+卡发行流水号4字节
//    memcpy((void *)&prec->card_in_time, &cardinfo->card_in_time, 7);//交易日期4字节和时间 3字节
//    memcpy(&prec->tac[0], &cardinfo->tac[0], 4);//TAC码 4字节
//    memcpy(&prec->isam_no[0], &DevStat.pos_number[0], 6);//终端机编号
////  memcpy(&prec->balance[0], ((INT8U *)(&cardinfo->balance)+1), 3); //交易后余额 3字节   刘及华
////  memcpy(&prec->fare[0], ((INT8U *)(&cardinfo->fare)+1), 3);	//交易金额 3字节
//    prec->balance[0] = (INT8U)(cardinfo->balance / 0x10000);
//    prec->balance[1] = (INT8U)(cardinfo->balance % 0x10000 / 0x100);
//    prec->balance[2] = (INT8U)(cardinfo->balance % 0x100 );
//    prec->fare[0] = (INT8U)(cardinfo->fare / 0x10000);
//    prec->fare[1] = (INT8U)(cardinfo->fare % 0x10000 / 0x100);
//    prec->fare[2] = (INT8U)(cardinfo->fare % 0x100);
//    memcpy(&prec->purchase_serial_num[0], (INT8U *)(cardinfo->purchase_serial_num), 2); //用户卡脱机交易序号 2字节
//    memcpy(&prec->oper_id[0], &DevStat.cur_driver_number[1], 3);//操作员编号3字节by liujihua
////  prec->oper_id[0] = DevStat.cur_driver_number[2];
////  prec->oper_id[1] = DevStat.cur_driver_number[1];
////  prec->oper_id[2] = DevStat.cur_driver_number[0];
//
//    memcpy(&prec->hard_no[0], &cardinfo->csn[0], 4);
//    memcpy(&prec->acnt_id[0], &DevStat.acnt_id[0],2);
//    memcpy(&prec->spot_id[0], &DevStat.spot_id[0],2);
//    memcpy(&prec->equ_id[0], &DevStat.equ_id[0],3);
//
////  EA_vCls();
////  EA_vDisplay(1, "%02x%02x", prec->acnt_id[0], prec->acnt_id[1]);
////  EA_vDisplay(2, "%02x%02x", prec->spot_id[0], prec->spot_id[1]);
////  EA_vDisplay(3, "%02d%02d%02d", prec->equ_id[0],prec->equ_id[1],prec->equ_id[2]);
////  EA_vDisplay(4, "%02x%02x%02x%02x", prec->hard_no[0],prec->hard_no[1],prec->hard_no[2],prec->hard_no[3]);
////  EA_uiInkey(0);
////  memcpy(&prec->oper_card_amt[0], ((INT8U *)(&DevStat.oper_card_amt)), 4);
//
////  prec->oper_card_amt[0] = (INT8U)(DevStat.oper_card_amt>>24);
////  prec->oper_card_amt[1] = (INT8U)(DevStat.oper_card_amt>>16);
////  prec->oper_card_amt[2] = (INT8U)(DevStat.oper_card_amt>>8);
////  prec->oper_card_amt[3] = (INT8U)DevStat.oper_card_amt;
//
//    prec->oper_card_amt[0] = (INT8U)(DevStat.oper_card_amt / 0x1000000);
//    prec->oper_card_amt[1] = (INT8U)(DevStat.oper_card_amt % 0x1000000 / 0x10000 );
//    prec->oper_card_amt[2] = (INT8U)(DevStat.oper_card_amt % 0x10000 / 0x100);
//    prec->oper_card_amt[3] = (INT8U)(DevStat.oper_card_amt % 0x100 );
//
////清空个人化信息区
//
//    memset(&prec->deposit_amt[0], 0, 2);
//    if(cardinfo->card_purchase_type == TT_CARD_SALE)
//    {
//        memset(&prec->fare[0],0x00,3);
//        memset(&prec->balance[0], 0x00, 3);
//        memset(&prec->tac[0], 0, 4);//TAC码 4字节
//        prec->pos_purchase_serial_num[0] = 0x00;
//        prec->pos_purchase_serial_num[1] = 0x00;
//        memcpy(&prec->deposit_amt[0], &DevStat.deposit_amt[0],2);//售卡押金
//    }
//    crc = Cal_CRC(record, 57);
//
//    record[57] = (crc / 0x100);//CRC1
//    record[58] = (crc % 0x100);//CRC2
//
//
//    i = SaveCardRecord(record);
//    if( i != ok)
//    {
//        return notok;
//    }
////	有未上传数据
//    DevStat.have_data_not_send_flag = TRUE;
//    return ok;
//}
//
// *****************************************************************
// 功能：		保存刷卡记录
// 入口参数：	
// 出口参数：	Null
// 作者：
// 返回值：		0://ok,保存成功			
//				1;//记录已满，保存失败
// *****************************************************************
INT8U SaveCardRecord(unsigned char * CardRecord)
{ 
	uint    		uiRetNum;
	uchar   		ucOpenID = 0;	
	// 打开文件
	if ( EA_ucPFOpen((uchar *)currecFileName, &ucOpenID) != EM_ffs_SUCCESS )
	{
		lcddisperr("打开记录文件失败");
		(void)EA_ucPFClose(ucOpenID);
		return notok;
	}

	if( EA_ucPFWriteRec(ucOpenID, 0, CardRecord) != EM_SUCCESS )
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

	DevStat.record_number = uiRetNum ;

	return ok;		            				    	  			  				   			 			       
}


// *****************************************************************
// 功能：		保存刷卡记录
// 入口参数：	
// 出口参数：	Null
// 作者：
// 返回值：		0://ok,保存成功			
//				1;//记录已满，保存失败
// *****************************************************************
//INT8U SaveCardConsumeRecord(unsigned char * CardRecord)
//{
//    uint        uiRetNum;
//    uchar       ucResult;
//    uchar       ucOpenID;
//    // 打开文件
//    if ( EA_ucPFOpen((uchar *)conrecFileName, &ucOpenID) != EM_ffs_SUCCESS )
//    {
//        lcddisperr("打开补采记录失败");
//        (void)EA_ucPFClose(ucOpenID);
//        return notok;
//    }
//    if ( EA_ucPFInfo(ucOpenID, EM_ffs_VALIDREC, &uiRetNum) != EM_ffs_SUCCESS )
//    {
//        lcddisperr("读取补采记录失败");
//        (void)EA_ucPFClose(ucOpenID);
//        return notok;
//    }
//    ucResult = EA_ucPFWriteRec(ucOpenID, 0, CardRecord);
//    if( ucResult != EM_SUCCESS)
//    {
//        (void)EA_ucPFClose(ucOpenID);
//        lcddisperr("写补采记录失败");
//        return notok;
//    }
//    (void)EA_ucPFClose(ucOpenID);
//    return ok;
//
//}

// *****************************************************************
// 功能：		按照卡号查找最近一条记录
// 入口参数：	card_no:卡片流水号
// 出口参数：	card_record:返回交易记录
// 作者：
// 返回值：		1://ok,找到记录		
//				0;//没有找到记录
// *****************************************************************
//short QueryCardTradeInfo(unsigned char * card_no, unsigned char *card_record)
//{
//    int         ret = 0;
//    int         i   = 0;
//    long        count   = 0;
//    char        flag = 0;
//    CardInfoOne         *strCardInfoRead;
////从最后一条向上查找20条记录，判断有无此卡号记录
//    count = DB_count_records(RECORDTABONE);
//    if(count >= 21)
//    {
//        for(i=count-1;i>=count-21;i--)
//        {
//            strCardInfoRead = DB_jump_to_record(RECORDTABONE,i,&flag);//count -1 :代表记录序号
//            if((memcmp(strCardInfoRead->record_info + 9, card_no, 4) == 0) && (flag == 0))//flag=0代表正常记录
//            {
//                memcpy(card_record, strCardInfoRead->record_info, 32);
//                card_record[32] = 0;
//                return 1;
//            }
//        }
//    }
//    else
//    {
//        if(count == 0)//没有记录
//            return 0;
//        else
//        {
//            for(i=count-1;i>=0;i--)
//            {
//                strCardInfoRead = DB_jump_to_record(RECORDTABONE,i,&flag);//count -1 :代表记录序号
//                if(memcmp(strCardInfoRead->record_info+9, card_no, 4) == 0)
//                {
//                    memcpy(card_record, strCardInfoRead->record_info, 32);
//                    card_record[32] = 0;
//                    return 1;
//                }
//            }
//        }
//    }
//    return 0;//没有找到记录
//
//}

// // *****************************************************************
// // 功能：		查找黑名单记录
// // 入口参数：	card_no:黑名单卡号
// // 出口参数：	Null
// // 作者：
// // 返回值：		1:找到
// //				0:没有找到
// // *****************************************************************
// int BlackName_Find(unsigned char * card_no)
// {
// 	int			ret = 0;
// 	int			i	= 0;
// 	long 		count	= 0;
// 	char		flag = 0;
// 	BlackCardInfo   *strBlackCardInfo;
// 	unsigned int   	BlackNameEnd;
// 	unsigned int   	BlackNameStart;
// 	unsigned int  	z;
//
// 	count = DB_count_records(DB_BLACK);
// 	if(count == 0)
// 	{
// 		return notok;
// 	}
//
// 	BlackNameEnd = count;
// 	if(BlackNameEnd > 8000)
// 		BlackNameEnd=8000;
// 	BlackNameStart=0;
//
// 	for ( ;; )
// 	{
// 		z=BlackNameStart+(BlackNameEnd-BlackNameStart)/2;
// 		strBlackCardInfo = DB_jump_to_record(BLACKTAB,z-1,&flag);//count -1 :代表记录序号
// 		if(memcmp(strBlackCardInfo->black_card_no, card_no, 4) == 0)
// 		{
// 			return ok;			//找到,是黑名单卡
// 		}
// 		if ( BlackNameEnd==BlackNameStart )
// 		{
// 			return notok;
// 		}
// 		if ( memcmp(strBlackCardInfo->black_card_no, card_no, 4) < 0 )
// 		{
// 			if ( BlackNameStart!=z )
// 			{
// 				BlackNameStart=z;
// 			}
// 			else
// 			{
// 				BlackNameStart=BlackNameEnd;
// 			}
// 		}
// 		else
// 		{
// 			if ( BlackNameEnd!=z )
// 			{
// 				BlackNameEnd=z;
// 			}
// 			else
// 			{
// 				BlackNameEnd=BlackNameStart;
// 			}
// 		}
// 	}
//
//
// 	return notok;//没有找到记录
// }

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
INT8U BlackName_Find(INT32U card_number)
{
	INT32U 		black_name_number;
	ulong   	ulRetNum;
	uint		uiOpenID;
	uchar		ucRet;

	return ok;   //frank debug, 正式版本应该删除。

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

	if(black_name_number == 0)
	{
		EA_vDisplay(3, "   没有白名单记录   ");
		EA_vDisplay(4, "  请手动下载白名单  ");
		SleepMs(2500);
		(void)EA_ucBlkClose(uiOpenID);
		return notok;      //没有黑名单记录
	}

	ucRet = EA_ucBlkSearch(uiOpenID, &card_number);
	(void)EA_ucBlkClose(uiOpenID);
	switch (ucRet)
	{
		case EM_bfs_FIND:         // 找到记录
			   return ok;
		case EM_bfs_NOT_FIND:   // 未找到记录   
			   return notok; 
	    default:
		   break;
	}
	return notok;
}

// *****************************************************************
// 功能：		保存参数到FLASH中
// 入口参数：	pSaveParam:参数信息(完整的)
// 出口参数：	
// 作者：
// 返回值：		0://ok,
//				1;//
// *****************************************************************
// int SaveTypeBParamtoFlash(ParamCardInfo *pSaveParam)
// {
// 	ParamCardInfo	*strParamInfo;
//
// 	long		RecordCapacity = 0;//记录容量
// 	long		RecordCount	   = 0;//数据库中现有记录总数
// 	long		RecordLeft	   = 0;//剩余空间
// 	char		strbuf[20];
// 	int			ret = 0,i=0;
// 	char		flag = 0;
//
// 	RecordCapacity = DB_capacity(PARAMINFO);
// 	RecordCount	   = DB_count_records(PARAMINFO);
//
// 	if(RecordCount == RecordCapacity)//数据库已满，需要清空
// 	{
//
// 		strcpy(strbuf, "参数表已满");
// 		DispStr_CE(0, 2, strbuf, DISP_CENTER|DISP_CLRSCR);
// 		strcpy(strbuf, "正在格式化");
// 		DispStr_CE(0, 4, strbuf, DISP_CENTER|DISP_CLRSCR);
// 		strcpy(strbuf, "请稍等....");
// 		DispStr_CE(0, 6, strbuf, DISP_CENTER|DISP_CLRSCR);
// 		DB_erase_filesys(PARAMINFO);
// 		DispStr_CE(0, 4, "正在保存参数..", DISP_CENTER|DISP_CLRSCR);
// 	}
//
// 	for(i=0;i<8;i++)
// 	{
// 		ret = DB_add_record(PARAMINFO,pSaveParam);
//
// 		if(!ret)
// 		{
// 			DB_erase_filesys(PARAMINFO);
// 			continue;//返回0时代表记录已满，无法保存，需要采集记录了，这种情况基本上不会发生
// 		}
// 		else
// 		{
// 			strParamInfo = DB_jump_to_record(PARAMINFO,ret-1,&flag);//count -1 :代表记录序号
// 			if((strParamInfo->have_been_set == pSaveParam->have_been_set) &&
//  				(strParamInfo->normal_card_fare ==pSaveParam->normal_card_fare) &&
//  				(memcmp(strParamInfo->line_no, pSaveParam->line_no, 2)==0))
// 			{
// 				EA_vCls();
// 				return 0;
// 			}
// 		}
// 	}
//
// 	if(i==8)
// 	{
// 		EA_vCls();
// 		strcpy(strbuf, "参数保存失败");
// 		DispStr_CE(0, 3, strbuf, DISP_CENTER|DISP_CLRSCR);
// 		delay_and_wait_key(2, 0, 0);
// //         DevStat.flash_err = TRUE;
// 		return 1;
// 	}
// }

// // *****************************************************************
// // 功能：		从FLASH中检索参数信息
// // 入口参数：	pSaveParam:参数信息(完整的)
// // 出口参数：
// // 作者：
// // 返回值：		0://ok,
// //				0;//
// // *****************************************************************
// int GetTypeBParamfromFlash(ParamCardInfo	*strParamInfo)
// {
// 	long		RecordCount	= 0;//数据库中现有记录总数
// 	char		flag = 0;
// 	char		strbuf[20];
//
// 	RecordCount = DB_count_records(PARAMINFO);
// 	if(RecordCount == 0)//没有设置参数表返回0；
// 		return 1;
//
// 	strParamInfo = DB_jump_to_record(PARAMINFO,RecordCount-1,&flag);//count -1 :代表记录序号
//
// 	return 0;
//
// }

// *****************************************************************
// 功能：		保存刷卡记录
// 入口参数：	
// 出口参数：	Null
// 作者：
// 返回值：		0://ok,保存成功			
//				1;//记录已满，保存失败
// *****************************************************************
//char SaveRecordIndex()
//{
//    int         count;
//    char        flag=1;
//    int         length,q,i;
//    int         ret = 0;
//    char        strbuf[20];
//    long        RecordCapacity=0;
//    long        RecordCount = 0;
//    RecordIndex strRecordIndex, *pRecordIndex;
//
//    RecordCapacity = DB_capacity(RECORDINDEX);
//    RecordCount    = DB_count_records(RECORDINDEX);
//
//    if(RecordCount == 0)//记录为空，直接添加新记录即可
//    {
//        //查询备份表中是否有数据
//        strRecordIndex.cur_normal_gather_index = 0;
//        strRecordIndex.cur_normal_record_count = 1;
//        strRecordIndex.last_normal_gather_index = 0;
//        strRecordIndex.last_normal_record_count = 0;
//        strRecordIndex.last_index_loca = 0;
//    }
//    else
//    {
//        count = RecordCount-1;//记录序号
//        while((!flag) && (count--))
//        {
//            pRecordIndex = DB_jump_to_record(RECORDINDEX,count,&flag);//count -1 :代表记录序号
//        }
//
//        if(!flag)//找到一条配置正确记录
//        {
//            //需要先读出备份记录
//            strRecordIndex.cur_normal_gather_index = pRecordIndex->cur_normal_gather_index;
//            //+2表示上次存储失败，实际上消费记录已经保存成功
//            strRecordIndex.cur_normal_record_count = pRecordIndex->cur_normal_record_count + 1;
//            strRecordIndex.last_normal_gather_index = pRecordIndex->last_normal_gather_index;
//            strRecordIndex.last_normal_record_count = pRecordIndex->last_normal_record_count;
//            strRecordIndex.last_index_loca = pRecordIndex->last_index_loca;
//        }
//
//        if(!count)//数据全部无效，******应该在保存的时候提示FLASH错误，进行异常采集
//        {
//            RecordCount == RecordCapacity;//下面直接格式化FLASH
//        }
//    }
//
//    if(RecordCount == RecordCapacity)//数据库已满，需要清空
//    {
//        //清空指针
//        strRecordIndex.cur_normal_gather_index = 0;
//        strRecordIndex.cur_normal_record_count = 1;
//        strRecordIndex.last_normal_gather_index = 0;
//        strRecordIndex.last_normal_record_count = 0;
//        strRecordIndex.last_index_loca = 0;
//
//        strcpy(strbuf, "参数表已满");
//        DispStr_CE(0, 2, strbuf, DISP_CENTER);
//        strcpy(strbuf, "正在格式化");
//        DispStr_CE(0, 4, strbuf, DISP_CENTER);
//        strcpy(strbuf, "请稍等....");
//        DispStr_CE(0, 6, strbuf, DISP_CENTER);
//        DB_erase_filesys(RECORDINDEX);
//        printf_debug("正在保存..");
//    }
//
//
//    for(i=0;i<8;i++)
//    {
//
//        ret = DB_add_record(RECORDINDEX,&strRecordIndex);
//
//        if(!ret)
//        {
//            DB_erase_filesys(RECORDINDEX);
//            i--;
//            continue;//返回0时代表记录已满，无法保存
//        }
//        else
//        {
//            pRecordIndex = DB_jump_to_record(RECORDINDEX,ret-1,&flag);//count -1 :代表记录序号
//            if(memcmp(&strRecordIndex, pRecordIndex, sizeof(RecordIndex)) == 0)
//            {
//                EA_vCls();
//
//                return 0;
//            }
//        }
//    }
//
//    if(i==8)//FLASH严重错误
//    {
//        EA_vCls();
//        strcpy(strbuf, "参数保存失败");
//        DispStr_CE(0, 3, strbuf, DISP_CENTER|DISP_CLRSCR);
//        delay_and_wait_key(2, 0, 0);
////         DevStat.flash_err = TRUE;
//        return 1;
//    }
//}

// *****************************************************************
// 功能：		采集结束后更新记录指针
// 入口参数：	
// 出口参数：	Null
// 作者：
// 返回值：		0://ok,保存成功			
//				1;//记录已满，保存失败
// *****************************************************************
//short RefreshRecordIndex()
//{
//    int         count;
//    char        flag=1;
//    int         length,q,i;
//    int         ret = 0;
//    char        strbuf[20];
//    long        RecordCapacity=0;
//    long        RecordCount = 0;
//    RecordIndex strRecordIndex, *pRecordIndex;
//
//    RecordCapacity = DB_capacity(RECORDINDEX);
//    RecordCount    = DB_count_records(RECORDINDEX);
//
//    if(RecordCount == 0)//记录为空，直接添加新记录即可
//    {
//        //查询备份表中是否有数据
//        strRecordIndex.cur_normal_gather_index = 0;
//        strRecordIndex.cur_normal_record_count = 1;
//        strRecordIndex.last_normal_gather_index = 0;
//        strRecordIndex.last_normal_record_count = 0;
//        strRecordIndex.last_index_loca = 0;
//    }
//    else
//    {
//        count = RecordCount-1;//记录序号
//        while((!flag) && (count--))
//        {
//            pRecordIndex = DB_jump_to_record(RECORDINDEX,count,&flag);//count -1 :代表记录序号
//        }
//
//        if(!flag)//找到一条配置正确记录
//        {
//            //需要先读出备份记录
//            strRecordIndex.cur_normal_gather_index = pRecordIndex->cur_normal_gather_index;
//            //+2表示上次存储失败，实际上消费记录已经保存成功
//            strRecordIndex.cur_normal_record_count = pRecordIndex->cur_normal_record_count + 1;
//            strRecordIndex.last_normal_gather_index = pRecordIndex->last_normal_gather_index;
//            strRecordIndex.last_normal_record_count = pRecordIndex->last_normal_record_count;
//            strRecordIndex.last_index_loca = pRecordIndex->last_index_loca;
//        }
//
//        if(!count)//数据全部无效，******应该在保存的时候提示FLASH错误，进行异常采集
//        {
//            RecordCount == RecordCapacity;//下面直接格式化FLASH
//        }
//    }
//
//    if(RecordCount == RecordCapacity)//数据库已满，需要清空
//    {
//        //清空指针
//        strRecordIndex.cur_normal_gather_index = 0;
//        strRecordIndex.cur_normal_record_count = 1;
//        strRecordIndex.last_normal_gather_index = 0;
//        strRecordIndex.last_normal_record_count = 0;
//        strRecordIndex.last_index_loca = 0;
//
//        strcpy(strbuf, "参数表已满");
//        DispStr_CE(0, 2, strbuf, DISP_CENTER);
//        strcpy(strbuf, "正在格式化");
//        DispStr_CE(0, 4, strbuf, DISP_CENTER);
//        strcpy(strbuf, "请稍等....");
//        DispStr_CE(0, 6, strbuf, DISP_CENTER);
//        DB_erase_filesys(RECORDINDEX);
//        printf_debug("正在保存..");
//    }
//
//
//    for(i=0;i<8;i++)
//    {
//
//        ret = DB_add_record(RECORDINDEX,&strRecordIndex);
//
//        if(!ret)
//        {
//            DB_erase_filesys(RECORDINDEX);
//            i--;
//            continue;//返回0时代表记录已满，无法保存
//        }
//        else
//        {
//            pRecordIndex = DB_jump_to_record(RECORDINDEX,ret-1,&flag);//count -1 :代表记录序号
//            if(memcmp(&strRecordIndex, pRecordIndex, sizeof(RecordIndex)) == 0)
//            {
//                EA_vCls();
//
//                return 0;
//            }
//        }
//    }
//
//    if(i==8)//FLASH严重错误
//    {
//        EA_vCls();
//        strcpy(strbuf, "参数保存失败");
//        DispStr_CE(0, 3, strbuf, DISP_CENTER|DISP_CLRSCR);
//        delay_and_wait_key(2, 0, 0);
////         DevStat.flash_err = TRUE;
//        return 1;
//    }
//}

// *****************************************************************
// 功能：		查询记录指针信息
// 入口参数：	
// 出口参数：	lBaseIndex->记录序号;
//				lRecordCount->记录总数
// 作者：
// 返回值：		0://ok,
//				1;//应该异常采集
// *****************************************************************
//char QueryRecordIndex(long *lBaseIndex, long* lRecordCount)
//{
//    long        RecordCount    = 0;//数据库中现有记录总数
//    char        flag = 0;
//    char        strbuf[20];
//    long        count = 0;
//    RecordIndex *pRecordIndex;
//
//    RecordCount = DB_count_records(RECORDINDEX);
//    if(RecordCount == 0)//没有设置参数表返回0；
//    {
//        *lBaseIndex = 0;
//        *lRecordCount = 0;
//        return 0;
//    }
//
//    count = RecordCount-1;//记录序号
//    while((!flag) && (count--))
//    {
//        pRecordIndex = DB_jump_to_record(RECORDINDEX,count,&flag);//count -1 :代表记录序号
//    }
//
//    if(!flag)
//    {
//        *lBaseIndex = pRecordIndex->cur_normal_gather_index;
//        *lRecordCount = pRecordIndex->cur_normal_record_count;
//        return 0;
//    }
//
////     DevStat.flash_err = TRUE;
//
//    return 1;//FLASH异常，应该做一次异常采集
//
//}

// *****************************************************************
// 功能：		查询补采集记录指针信息
// 入口参数：	
// 出口参数：	lBaseIndex->记录序号;
//				lRecordCount->记录总数
// 作者：
// 返回值：		0://ok,
//				1;//应该异常采集
// *****************************************************************
//int QueryLastRecordIndex(long *lBaseIndex, long* lRecordCount, char *RecordLocal)
//{
//    long        RecordCount    = 0;//数据库中现有记录总数
//    char        flag = 0;
//    char        strbuf[20];
//    long        count = 0;
//    RecordIndex *pRecordIndex;
//
//    RecordCount = DB_count_records(RECORDINDEX);
//    if(RecordCount == 0)//没有设置参数表返回0；
//    {
//        *lBaseIndex = 0;
//        *lRecordCount = 0;
//        return 0;
//    }
//
//    count = RecordCount-1;//记录序号
//    while((!flag) && (count--))
//    {
//        pRecordIndex = DB_jump_to_record(RECORDINDEX,count,&flag);//count -1 :代表记录序号
//    }
//
//    if(!flag)
//    {
//        *lBaseIndex = pRecordIndex->last_normal_gather_index;
//        *lRecordCount = pRecordIndex->cur_normal_record_count;
//        *RecordLocal  = pRecordIndex->last_index_loca;
//        return 0;
//    }
//
//    return 1;//FLASH异常，应该做一次异常采集
//
//}

///******************************************************************************
// 函数名称：WriteParam
// 功能描述：保存参数
// 参数描述：
// 参数名称： 输入/输出？ 类型        描述
//
// 返  回  值：无
//
// 作      者 ：许岩
// 日      期：2006-08-02
// 修改历史：
//        日期        修改人      修改描述
//        ------      ---------   -------------
//******************************************************************************/
//void WriteParam(void)
//{
//
//    int         count;
//    char        flag;
//    int         length,q,i;
//    int         ret = 0;
//    char        strbuf[20];
//    long        RecordCapacity=0;
//    long        RecordCount = 0;
//    DriverParam * strParamInfo;
//
//    RecordCapacity = DB_capacity(DB_PARAM);
//#ifdef DEBUG
//    TRACE232("DB_capacity = %d\n\r", RecordCapacity);
//#endif
//    RecordCount    = DB_count_records(DB_PARAM);
//
//// 	if(RecordCount >= RecordCapacity)//数据库已满，需要清空
//    if(RecordCount >= RecordCapacity - 1)//数据库已满，需要清空 其实可以不用减一，怕出事，by xuyan
//    {
//
//// 		strcpy(strbuf, "参数表已满");
//// 		DispStr_CE(0, 2, "参数表已满", DISP_CENTER);
//// 		strcpy(strbuf, "正在格式化");
//// 		DispStr_CE(0, 4, strbuf, DISP_CENTER);
//        DispStr_CE(0, 2, "正在保存", DISP_CENTER);
//        DispStr_CE(0, 4, "请稍等....", DISP_CENTER);
//        DB_erase_filesys(DB_PARAM);
//// 		printf_debug("正在保存..");
//
//    }
//
//    ret = DB_add_record(DB_PARAM, &DevStat.start);
//
//// 	for(i=0;i<8;i++)
//// 	{
////
//// 		ret = DB_add_record(DB_PARAM, DevStat);
////
//// 		if(!ret)
//// 		{
//// 			DB_erase_filesys(DRIVERPARAM);
//// 			i--;
//// 			continue;//返回0时代表记录已满，无法保存，需要采集记录了，这种情况基本上不会发生
//// 		}
//// 		else
//// 		{
//// 			strParamInfo = DB_jump_to_record(DRIVERPARAM,ret-1,&flag);//count -1 :代表记录序号
//// 			if(memcmp(strParamInfo, strDevParam, sizeof(DriverParam)) == 0)
//// 			{
//// 				EA_vCls();
//// 				return 0;
//// 			}
//// 		}
//// 	}
////
//// 	if(i==8)
//// 	{
//// 		EA_vCls();
//// 		strcpy(strbuf, "参数保存失败");
//// 		DispStr_CE(0, 3, strbuf, DISP_CENTER|DISP_CLRSCR);
//// 		delay_and_wait_key(2, 0, 0);
//// //         DevStat.flash_err = TRUE;
//// 		return 1;
//// 	}
//
//}


///******************************************************************************
// 函数名称：ReadParam
// 功能描述：读取参数
// 参数描述：
// 参数名称： 输入/输出？ 类型        描述
//
// 返  回  值：无
//
// 作      者 ：许岩
// 日      期：2006-08-02
// 修改历史：
//        日期        修改人      修改描述
//        ------      ---------   -------------
//******************************************************************************/
//void ReadParam(void)
//{
//    int         count;
//    char        flag;
//    INT32U rec_num;
//    INT8U *p;
//
//    rec_num = DB_count_records(DB_PARAM);
//    if(rec_num == 0)            // 没有记录
//        return;
//
//    p = (INT8U *)DB_jump_to_record(DB_PARAM, rec_num - 1, &flag);//count -1 :代表记录序号
//
//    memcpy(&DevStat.start, p, &DevStat.end - &DevStat.start);
////	DevStat.ServerIp[0] = 59;
////	DevStat.ServerIp[1] = 46;
////	DevStat.ServerIp[2] = 156;
////	DevStat.ServerIp[3] = 13;
//
////	DevStat.ServerPort[0] = 0x90;
////	DevStat.ServerPort[1] = 0x01;
//
////	DevStat.equ_id[0] = 0x00;
////	DevStat.equ_id[1] = 0x23;
////	DevStat.equ_id[2] = 0x42;
//
//// 	if(!flag)
//// 		return 0;
//// 	else//上次司机卡刷卡保存失败，应当重新刷司机卡
//// 		return 1;
//}


//
///******************************************************************************
// 函数名称：ReadGrayRecord
// 功能描述：读取灰记录
// 参数描述：
// 参数名称： 输入/输出？ 类型        描述
//
// 返  回  值：无
//
// 作      者 ：许岩
// 日      期：2006-08-02
// 修改历史：
//        日期        修改人      修改描述
//        ------      ---------   -------------
//******************************************************************************/
//INT8U ReadGrayRecord(void)
//{
//
//    uint  uiRetNum;
//    INT8U  p[73];
//    uchar ucOpenID= 0;
//    uint  uiReadCnt;
////	GrayRecord *record = (GrayRecord *)p;
//
//    // 打开文件
//    if ( EA_ucPFOpen((uchar *)grayrecFileName, &ucOpenID) != EM_ffs_SUCCESS )
//    {
//        lcddisperr("打开灰记录失败");
//        (void)EA_ucPFClose(ucOpenID);
//        return notok;
//    }
//    if ( EA_ucPFInfo(ucOpenID, EM_ffs_VALIDREC, &uiRetNum) != EM_ffs_SUCCESS )
//    {
//        lcddisperr("获取灰记录信息失败");
//        (void)EA_ucPFClose(ucOpenID);
//        return notok;
//    }
//    if( EA_ucPFReadRec(ucOpenID, uiRetNum, 1, &uiReadCnt, p) != EM_SUCCESS )
//    {
//        lcddisperr("读取灰记录失败");
//        (void)EA_ucPFClose(ucOpenID);
//        return notok;
//    }
//    (void)EA_ucPFClose(ucOpenID);
////  p = (INT8U *)DB_jump_to_record(DB_GRAY_RECORD, rec_num - 1, &flag);//count -1 :代表记录序号
//    memcpy(&GrayRecord, p, sizeof(GRAYRECORD));
//    return ok;
//}
//
/******************************************************************************
 函数名称：InitImportantParameter
 功能描述：初始化关键参数
 参数描述：
 参数名称：	输入/输出？	类型		描述
				
 返  回  值：无

 作      者	：许岩
 日      期：2004-09-02
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
******************************************************************************/
//void InitImportantParameter(void)
//{
//
//    //DevStat初始化
//    memset(&DevStat, 0x00, sizeof(DevStat));
//    strcpy((void *)DevStat.user_passwd, (void *)"000000");
//    DevStat.kb_mask = 0xffffffff;                   //开声音
//    DevStat.el_mask = 0xffffffff;                   //开背光
//    DevStat.poweroff_time = 30;                     //关机时间
//
////	DevStat.oldman_free = TRUE;
////	DevStat.employee_a_free = TRUE;
////	DevStat.driver_free = TRUE;
//
////	DevStat.oldman_free = FALSE;
////	DevStat.employee_a_free = TRUE;
////	DevStat.driver_free = FALSE;
//
////	DevStat.purchase_hold_time = 10;		//刷卡持续时间默认为10秒
////	DevStat.max_price_limit = 20000;
////	DevStat.free_price = 100;//免费卡票价
//    DevStat.POS_type = 0x34;                            //正常采集
//    DevStat.have_been_set = FALSE;
//    DevStat.driver_is_working = FALSE;
//    memset(&DevStat.dev_trade_no[0],0, 2);
//    DevStat.mode = 0x00;
//    DevStat.samcompcode = 1;
//    memset(&DevStat.binhand, 0x00, sizeof(DevStat.binhand));
//    DevStat.have_data_not_send_flag = FALSE;
//    DevStat.last_trade_type = 0x00;
//    WriteParam();
//
//}

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
//	uint    uiReadCnt;
//	ulong   ulRetNum;

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


//
///******************************************************************************
// 函数名称：WriteGrayRecord
// 功能描述：保存灰记录
// 参数描述：
// 参数名称： 输入/输出？ 类型        描述
//
// 返  回  值：无
//
// 作      者 ：许岩
// 日      期：2006-08-02
// 修改历史：
//        日期        修改人      修改描述
//        ------      ---------   -------------
//******************************************************************************/
//INT8U WriteGrayRecord(void)
//{
////	int   i;
//    INT8U record[73];
//    uchar ucOpenID = 0;
////	uint  uiReadcnt;
//
//    memset( record, 0x00, 73 );
//    memcpy((GRAYRECORD*)record, &GrayRecord, sizeof(GRAYRECORD));
//    if ( EA_ucPFOpen( (uchar *)grayrecFileName, &ucOpenID) != EM_ffs_SUCCESS )
//    {
//        lcddisperr("打开灰记录失败");
//        return  notok;
//    }
//    // 获取剩余可写记录数
////  (void)EA_ucPFInfo(ucOpenID, EM_ffs_VALIDREC, &uiReadcnt);
//    //将参数文件写入FLASH中
//    if ( EA_ucPFWriteRec( ucOpenID, 0, record) != EM_ffs_SUCCESS )
//    {
//        lcddisperr("添加灰记录失败");
//        return notok;
//    }
//    if ( EA_ucPFClose(ucOpenID) != EM_ffs_SUCCESS )
//    {
//        lcddisperr("关闭灰记录文件失败");
//        return notok;
//    }
//    return ok;
//}

