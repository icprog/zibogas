// This file contains all API to operate SIM200 GRPS Module.


#include  "main.h"


//extern INT8U  KeySector[14][6];
extern DEV_STAT       DevStat;					//设备状态
extern CARD_INFO      CardInfo;					//卡片信息，交易记录由此变量生成
//extern GRAYRECORD   GrayRecord;

extern const char parmFileName[];
extern const char blackFileName[];
extern const char currecFileName[];
extern const char conrecFileName[];
extern const char hisrecFileName[];
extern const char grayrecFileName[];

//#define hd6305_III
////extern uart_send(unsigned char *lb, unsigned char cnt);
//#define IPMODE  0
//#define DNSMODE 1
extern int socketID;
//
///******************************************************************************
// 函数名称：GPRS_CAL_KEY
// 功能描述：GPRS计算充值密钥
// 参数名称：输入/输出？  类型        描述
//输入		    no
//
// 返  回  值：无
//
// 作      者 ：YFY
// 日      期：2007-05-2
// 修改历史：
//        日期        修改人      修改描述
//******************************************************************************/
//INT8U GPRS_CAL_KEY()
//{
//
//    short  i=0;
//    INT8U sendbuf[256];
//    int   sendlength=0;
//    INT8U temp_data[256];
//    INT8U p[60];
//    uint  db_rec_num = 0;
//    int   ret=0;
//    uchar ucOpenID = 0;
//    uchar ucResult;
//    uint  uiReadCnt;
//    uint  uiReadNum;
//
//    (void)EA_vCls();
//    (void)EA_vDisplay(1,"      充值申请      ");
//    (void)EA_vDisplay(2,"     GPRS通讯中     ");
//    (void)EA_vDisplay(3,"       请等待...    ");
//    (void)EA_uiInkeyMs(500);
//
//    EA_vDisplay(4,"      传输数据      ");
//
////	printf_debug("config ok");
////判断是否有未上传的数据
//    if(DevStat.have_data_not_send_flag)//有未上传数据
//    {
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
////数据上传
//        //DevStat.last_trade_type=TT_CREDIT_MONEY;
//        if(DevStat.last_trade_type != TT_CARD_SALE)//充值充帐数据
//        {
//            sprintf((void *)sendbuf, "@START%02x%02x%02x", DevStat.equ_id[0],DevStat.equ_id[1],DevStat.equ_id[2]);
//            strcat((void *)sendbuf, (void *)"SDAT114");//command
////  		db_rec_num = DB_count_records(DB_REC);
////  	//	sprintf(str, "db_n:%d", db_rec_num);
////  	//	printf_debug(str);
////  		db_rec_num --;
////  		p = (INT8U *)DB_jump_to_record(DB_REC, db_rec_num, &flag);	//取记录
//            if ( EA_ucPFOpen((uchar *)currecFileName, &ucOpenID) != EM_ffs_SUCCESS )
//            {
//                (void)EA_ucPFClose(ucOpenID);
//                return notok;
//            }
//            //有效参数信息条数
//            ucResult = EA_ucPFInfo(ucOpenID, EM_ffs_VALIDREC, &uiReadCnt);
//            if ( ucResult != EM_ffs_SUCCESS )
//            {
//                (void)EA_ucPFClose(ucOpenID);
//                return notok;
//            }
//            db_rec_num = uiReadCnt;
//            (void)EA_ucPFReadRec(ucOpenID, db_rec_num, 1, &uiReadNum, (uchar *)&p);
//            (void)EA_ucPFClose(ucOpenID);
//            memcpy(temp_data, p, 57);
//            for(i=0;i<57;i++)
//            {
//                sprintf((void *)(sendbuf+19+2*i), "%02x", temp_data[i]);
//            }
//            //sendbuf[19+114] = '\r';
//            //strcat(sendbuf, "\r");
//            sendlength = strlen((void *)sendbuf);
////			memcpy(sendbuf+19, p, 57);
////			crc = Cal_CRC(sendbuf, 76);
////			sendbuf[76] = ((crc >> 8)& 0xff);//CRC1
////			sendbuf[77] = (crc & 0xff);//CRC2
////			sendlength = 78;
//        }
//        else//售卡数据----附加充值数据
//        {
//            sprintf((void *)sendbuf, "@START%02x%02x%02x", DevStat.equ_id[0],DevStat.equ_id[1],DevStat.equ_id[2]);
//            strcat((void *)sendbuf, "SDAT228");//command
////  		db_rec_num = DB_count_records(DB_REC);
////  		db_rec_num -= 2;//售卡数据
////  		p = (INT8U *)DB_jump_to_record(DB_REC, db_rec_num, &flag);	//取记录
//            if ( EA_ucPFOpen((uchar *)currecFileName, &ucOpenID) != EM_ffs_SUCCESS )
//            {
//                (void)EA_ucPFClose(ucOpenID);
//                return notok;
//            }
//            //有效参数信息条数
//            ucResult = EA_ucPFInfo(ucOpenID, EM_ffs_VALIDREC, &uiReadCnt);
//            if ( ucResult != EM_ffs_SUCCESS )
//            {
//                (void)EA_ucPFClose(ucOpenID);
//                return notok;
//            }
//            db_rec_num = uiReadCnt - 1;
//            ucResult = EA_ucPFReadRec(ucOpenID, db_rec_num, 1, &uiReadNum, (INT8U *)&p);
//            if ( ucResult != EM_ffs_SUCCESS )
//            {
//                (void)EA_ucPFClose(ucOpenID);
//                return notok;
//            }
//            //(void)EA_ucPFClose(ucOpenID);
//            memcpy(temp_data, p, 57);
////  		memcpy(temp_data, p, 57);
//            for(i=0;i<57;i++)
//            {
//                sprintf((void *)(sendbuf+19+2*i), "%02x", temp_data[i]);
//            }
//
//            db_rec_num++;//充值数据
////  		p = (INT8U *)DB_jump_to_record(DB_REC, db_rec_num, &flag);	//取记录
//            (void)EA_ucPFReadRec(ucOpenID, db_rec_num, 1, &uiReadNum, (INT8U *)&p);
//            memcpy(temp_data, p, 57);
//            for(i=0;i<57;i++)
//            {
//                sprintf((void *)(sendbuf+19+114+2*i), "%02x", temp_data[i]);
//            }
//        //  strcat(sendbuf, "\r");
//            sendlength = strlen((void *)sendbuf);
//            (void)EA_ucPFClose(ucOpenID);
//        }
//
//        ret = GPRS_SEND_Receive(sendbuf,sendlength);
//        if(ret != ok)
//        {
//            lcddisperr("    数据传输失败    ");
//            return notok;
//        }
//        else
//        {
//            DevStat.have_data_not_send_flag = FALSE;
//            WriteParam();
//        }
//
//    }
//
//    lcddisperr("      充值申请      ");
//
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
////密钥申请
//
//    sprintf((void *)sendbuf, "@START%02x%02x%02x", DevStat.equ_id[0],DevStat.equ_id[1],DevStat.equ_id[2]);
//    strcat((void *)sendbuf, "CALK024");//command
//
//    memcpy(temp_data, &CardInfo._CARD_NUMBER.detail_card_number.city_code[0], 2);//城市代码
//    memcpy(temp_data+2, &CardInfo.csn[0], 4);//CSN硬件卡号
//    memcpy(temp_data+6, &CardInfo._CARD_NUMBER.detail_card_number.serial_number[2], 2);//卡发行流水号后2字节
//    memcpy(temp_data+8, &CardInfo.authentication_code[0], 4);
//
//    for(i=0;i<12;i++)
//    {
//        sprintf((void *)(sendbuf+19+2*i), "%02x",temp_data[i]);
//    }
//    sendlength = strlen((void *)sendbuf);
//    ret = GPRS_SEND_Receive( sendbuf,sendlength );
//    if(ret!=ok)
//    {
//
//        lcddisperr("充值申请失败");
//        return notok;
//    }
//    else
//    {
//        DevStat.have_data_not_send_flag = FALSE;
//        WriteParam();
//    }
//    EA_vDisplay(4,"    充值申请完成    ");
//    EA_uiInkeyMs(500);
//    return ok;
//}
//
//
//

/******************************************************************************
 函数名称：GPRS_SEND_RECORD
 功能描述：GPRS上传记录
 参数名称：输入/输出？	类型		描述
输入		    no
				
 返  回  值：无
 
 作      者	：YFY
 日      期：2007-05-2
 修改历史：
		日期		修改人		修改描述
******************************************************************************/
INT8U GPRS_SEND_RECORD(void)
{
	uchar ucOpenID = 0;
	uint  uiReadCnt;
	short i=0,j=0;
	uchar sendbuf[1024];
	INT8U str[50];
	INT8U p[60];
	int   ret=0;
	unsigned long	  record_index = 0;
	int   package_num=0,package_index=0, package_left=0,last_record_no=0;

	memset(p, 0, sizeof(p));

 	DevStat.record_number = query_rec_num();

	if( DevStat.record_number == 0 )
	{
		lcddisperr((void *)"没有交易记录!");
		return ok;
	}

	EA_vCls();
	EA_vDisplay(1,"      记录上传      ");
	EA_vDisplay(2,"     GPRS通讯中     ");
	EA_vDisplay(3,"      请等待...     ");
	EA_vDisplay(4,"    传输数据中...   ");
	(void)EA_uiInkeyMs(2000);
//  SleepMs(2000);

	if(DevStat.record_number > 0)
	{
		i = 0;
		if( i != DevStat.send_total_num )//断点续传
		{
			i = DevStat.send_total_num;
		}

		EA_vCls();	
		EA_vDisplay(1, "    GPRS数据传输    ");
		EA_vDisplay(2, "    消费记录上传    ");	
		sprintf((void *)str, "   共有%4d条记录   ",  DevStat.record_number-i);
		EA_vDisplay(3, (void *)str);
		(void)EA_uiInkeyMs(2000);

		last_record_no = i;

		package_num = (DevStat.record_number - i)/10;//10条一个包，可以打包的整数倍
		
		package_left = (DevStat.record_number - i)%10;//剩余条数
		
		package_index = 0;

		record_index = last_record_no + 1;
		//sprintf(str,"rec:%d",record_index);
		//printf_debug(str);

		if ( EA_ucPFOpen((uchar *)currecFileName, &ucOpenID) != EM_ffs_SUCCESS )
		{
			(void)EA_ucPFClose(ucOpenID);
			return notok;
		}

		for( package_index=0; package_index<package_num; package_index++ )
		{
			sprintf((void *)sendbuf, "@START%02x%02x%02xUPDD%03d%02x%02x%02x%02x%02x", DevStat.equ_id[0], DevStat.equ_id[1],DevStat.equ_id[2], (490+22), 
				DevStat.acnt_id[0],DevStat.acnt_id[1],DevStat.equ_id[0],DevStat.equ_id[1],DevStat.equ_id[2]);//上传记录
			sprintf((void *)(sendbuf+19+10), "%02x%02x%02x%02x%02x%02x", DevStat.binhand.work_datetime.year,DevStat.binhand.work_datetime.month,
				DevStat.binhand.work_datetime.day,DevStat.binhand.work_datetime.hour,DevStat.binhand.work_datetime.minute,DevStat.binhand.work_datetime.second);//上传记录
			for(j=0;j<10;j++)
			{						
//  			p = (INT8U *)DB_jump_to_record(DB_REC, record_index, &flag);
				(void)EA_ucPFReadRec(ucOpenID, record_index, 1, &uiReadCnt, p);
				record_index ++;
				memcpy((void *)(sendbuf+19+22+49*j), p, 49);				
			}
			sendbuf[490+19+22] = 0;

			ret = GPRS_SEND_Receive(sendbuf,490+19+22);

			if( ret == ok )
			{
				DevStat.send_total_num+=10;
				sprintf((void *)str, "上传进度%03d%%",package_index*100/(package_num+1));
				EA_vDisplay(3, (void *)str);
//  			for(j=0;j<10;j++)
//  			{
//  //  			p = (INT8U *)DB_jump_to_record(DB_REC, record_index, &flag);
//  				(void)EA_ucPFDelRec(ucOpenID, i);
//  			}
			}
			else
				break;
				
		}
		(void)EA_ucPFClose(ucOpenID);

		if( ret != ok )
		{
			WriteParam();    //保存断点续传参数
			EA_vCls();
			EA_vDisplay(1,"    GPRS数据传输    ");
			EA_vDisplay(2,"  网络故障上传失败  ");	
			EA_vDisplay(3,"  网络正常后再上传  ");
			SleepMs(2000);
			GPRS_Close();
			return notok;
		}
		//剩余记录打一个包
		if(package_left > 0)
		{
			if ( EA_ucPFOpen((uchar *)currecFileName, &ucOpenID) != EM_ffs_SUCCESS )
			{
				(void)EA_ucPFClose(ucOpenID);
				return notok;
			}
			sprintf((void *)sendbuf, "@START%02x%02x%02xUPDD%03d%02x%02x%02x%02x%02x", DevStat.equ_id[0],DevStat.equ_id[1],DevStat.equ_id[2],package_left*49+22, 
					DevStat.acnt_id[0],DevStat.acnt_id[1],DevStat.equ_id[0],DevStat.equ_id[1],DevStat.equ_id[2]);//上传记录
			sprintf((void *)(sendbuf+19+10), "%02x%02x%02x%02x%02x%02x", DevStat.binhand.work_datetime.year,DevStat.binhand.work_datetime.month,
					DevStat.binhand.work_datetime.day,DevStat.binhand.work_datetime.hour,
					DevStat.binhand.work_datetime.minute,DevStat.binhand.work_datetime.second);//上传记录
			
			for(j=0;j<package_left;j++)
			{			
				(void)EA_ucPFReadRec(ucOpenID, record_index, 1, &uiReadCnt, p);
				record_index ++;
				memcpy(sendbuf+19+22+49*j, p, 49);
			}
			(void)EA_ucPFClose(ucOpenID);

			ret = GPRS_SEND_Receive(sendbuf,package_left*49+19+22);		
		
			if( ret == ok )
			{
				sprintf((void *)sendbuf, "@START%02x%02x%02xUPDE022%02x%02x%02x%02x%02x", DevStat.equ_id[0],DevStat.equ_id[1],DevStat.equ_id[2],
						DevStat.acnt_id[0],DevStat.acnt_id[1],DevStat.equ_id[0],DevStat.equ_id[1],DevStat.equ_id[2]);//上传记录
				sprintf((void *)(sendbuf+19+10), "%02x%02x%02x%02x%02x%02x", DevStat.binhand.work_datetime.year,DevStat.binhand.work_datetime.month,
						DevStat.binhand.work_datetime.day,DevStat.binhand.work_datetime.hour,DevStat.binhand.work_datetime.minute,DevStat.binhand.work_datetime.second);//上传记录
				
				ret = GPRS_SEND_Receive(sendbuf,19+22);
				if(ret == ok)
				{
					DevStat.send_total_num = 0;
					EA_vDisplay(3, "      上传完成      ");
					SleepMs(2000);
				}
				else
				{
					WriteParam();//保存断点续传参数
					EA_vCls();
					EA_vDisplay(1,(void *)"    GPRS数据传输    ");
					EA_vDisplay(2,(void *)"  网络故障上传失败  ");	
					EA_vDisplay(3,(void *)"  网络正常后再上传  ");
					SleepMs(2000);
					GPRS_Close();
					return notok;
				}
			}
			else
			{
					WriteParam();//保存断点续传参数
					EA_vCls();
					EA_vDisplay(1,(void *)"    GPRS数据传输    ");
					EA_vDisplay(2,(void *)"  网络故障上传失败  ");	
					EA_vDisplay(3,(void *)"  网络正常后再上传  ");
					SleepMs(2000);
					GPRS_Close();
					return notok;
			}
		}				
	}

//  EA_ucPFOpen((uchar *)currecFileName, &ucOpenID);
//  DB_erase_filesys(DB_REC);
	EA_ucPFDel((uchar *)currecFileName);
//    for(i=1;i<=DevStat.record_number;i++)
//    {
////  	EA_ucPFDel((uchar *)currecFileName);
//        EA_ucPFDelRec(ucOpenID, i);
//    }
//    (void)EA_ucPFClose(ucOpenID);

	DevStat.record_number = 0;
	
	WriteParam();

//  GPRS_Close();
	
	//GPRS_Power_Off();	
	return ok;
}


/******************************************************************************
 函数名称：GPRS_TRANS_DATA
 功能描述：GPRS命令处理
 参数名称：输入/输出？	类型		描述
输入		    index	命令索引
				
 返  回  值：无
 
 作      者	：于福勇
 日      期：2007-03-05
 修改历史：
		日期		修改人		修改描述
******************************************************************************/
INT8U  GPRS_TRANS_DATA()
{
	INT8U sendbuf[1024];
	int	 sendlength=0;	
	int  ret=0;
	
	EA_vCls();
	EA_vDisplay(1, "     操作员签退     ");
	EA_vDisplay(2, "     GPRS通讯中     ");
	EA_vDisplay(3, "      请等待...     ");
	EA_uiInkeyMs(2000);
	
	EA_vCls();
	EA_vDisplay(1, "      传输数据      ");
	EA_vDisplay(2, " 传送日结数据中.... ");

	sprintf((void *)sendbuf, "@START%02x%02x%02x", DevStat.equ_id[0],DevStat.equ_id[1],DevStat.equ_id[2]);
	strcat((void *)sendbuf, "OQUT099");//command 

//oper id
	sprintf((void *)(sendbuf+19), "%02x%02x%02x",DevStat.binhand.oper_id[0],DevStat.binhand.oper_id[1],DevStat.binhand.oper_id[2]);

//	BUS_TIME work_datetime; //上班时间
	sprintf((void *)(sendbuf+25), "%02x%02x%02x%02x%02x%02x%02x",DevStat.binhand.work_datetime.century,DevStat.binhand.work_datetime.year,
		   DevStat.binhand.work_datetime.month,DevStat.binhand.work_datetime.day,DevStat.binhand.work_datetime.hour,
		   DevStat.binhand.work_datetime.minute,DevStat.binhand.work_datetime.second);

//	BUS_TIME down_datetime; //下班时间
	sprintf((void *)(sendbuf+39), "%02x%02x%02x%02x%02x%02x%02x",DevStat.binhand.down_datetime.century,DevStat.binhand.down_datetime.year,
		   DevStat.binhand.down_datetime.month,DevStat.binhand.down_datetime.day,DevStat.binhand.down_datetime.hour,
		   DevStat.binhand.down_datetime.minute,DevStat.binhand.down_datetime.second);

	sendlength = strlen((void *)sendbuf);

	ret = GPRS_SEND_Receive(sendbuf,sendlength);
	if(ret != ok)
	{
		goto end;
	}

	EA_vDisplay(4, "   操作员日结完成   ");
	EA_uiInkeyMs(2000);
	return ok;
	
end:
	WriteParam();

	EA_vCls();
	EA_vDisplay(1, "    GPRS数据传输    ");
	EA_vDisplay(2, "  网络故障通讯失败  ");
	EA_vDisplay(3, "  网络正常后再上传  ");
	(void)EA_uiInkeyMs(2000);
	return notok;
}

/******************************************************************************
 函数名称：PackageCheck
 功能描述：判断接受包是否正确
 参数名称：输入/输出？	类型		描述
输入		    	data	数据
				
 返  回  值：无
 
 作      者	：YFY
 日      期：2007-05-2
 修改历史：
		日期		修改人		修改描述
******************************************************************************/
int  PackageCheck(char * buff, int len)
{

	char	CRC[5];
	//长度判断
	char	temp_buf[10];
	
	unsigned int     crc=0;

	int length = 0;
	
	if(memcmp(buff,"@START",6)!=0)
		return -1;

	if(memcmp(buff+len-4,"END",3)!=0)
		return -2;


	memcpy(temp_buf,buff+16,3);
	temp_buf[3] = 0;
	length = atoi(temp_buf);				
	if(len!=27+length)
		return -3;

	crc = Cal_CRC((unsigned char *)buff,len-8);

	sprintf((void *)CRC, "%02X%02X", ((crc >> 8) & 0xff), (crc & 0xff));

	if(memcmp(buff+len-8,CRC, 4)!=0)
		return -4;

	return 1;

}


/******************************************************************************
 函数名称：Gprs_Upload_data
 功能描述：GPRS命令处理
 参数名称：输入/输出？	类型		描述
输入		    data	数据
				
 返  回  值：无
 
 作      者	：于福勇
 日      期：2007-03-05
 修改历史：
		日期		修改人		修改描述
******************************************************************************/

int Gprs_Upload_data(void)
{
	char 		buf[30];

	memset((void *)buf, 0, sizeof(buf));

	EA_vCls();
	EA_vDisplay(1,       "   未上传记录统计   ");
	sprintf((void *)buf, "   共有%04d条记录   ", DevStat.record_number);
	EA_vDisplay(2, buf);
	EA_vDisplay(3,       "请确保处于安全范围内");
	EA_vDisplay(4,       "    再进行数据上传  ");
	if( EA_uiInkey(0) == EM_key_ENTER )
	{
		;
	}
	else
	{
		return notok;
	}


	if( Open_GPRS_Module() != ok )
	{
		lcddisperr("PPP拨号连接失败");
		Beeperr();
		return notok;
	}

	if( GPRS_SEND_RECORD() != ok )
	{
		EA_vCls();
		EA_vDisplay(1, "    有数据未上传    ");
		EA_vDisplay(2, "   操作员日结失败   ");
		EA_vDisplay(3, "    请重新签退!!!   ");
		Beeperr();
		EA_uiInkey(4);
		return notok;
	}

//  Get_Time(&time_data);
//  memcpy(&DevStat.binhand.down_datetime, &time_data, sizeof(BUS_TIME));
//
//  //打印日结数据
//  print_work_note();
//  memcpy(record, (INT8U*)&DevStat.binhand, sizeof(DevStat.binhand));
//  SaveNoteRecord(record);
//  DevStat.driver_is_working = FALSE;
//  memset(&DevStat.binhand, 0x00, sizeof(DevStat.binhand));
//  WriteParam();
	return ok;
}


// *****************************************************************
// 功能：		保存日结记录
// 入口参数：	
// 出口参数：	Null
// 作者：
// 返回值：		0://ok,保存成功			
//				1;//记录已满，保存失败
// *****************************************************************
INT8U SaveNoteRecord(unsigned char * NoteRecord)
{ 
	uchar 		ucOpenID = 0;
//	uint 		uiReadCnt = 0;

	if ( EA_ucPFOpen((uchar *)hisrecFileName, &ucOpenID) != EM_ffs_SUCCESS )
	{
		(void)EA_ucPFClose(ucOpenID);
		lcddisperr("打开日结文件失败");
		return notok;
	}
	if ( EA_ucPFWriteRec(ucOpenID, 0, NoteRecord) !=  EM_ffs_SUCCESS )
	{
		(void)EA_ucPFClose(ucOpenID);
		lcddisperr("写日结文件失败");
		return notok;
	}
	(void)EA_ucPFClose(ucOpenID);
	
	return ok;
}


/******************************************************************************
 函数名称：GPRS_TEST
 功能描述：GPRS测试
 参数名称：输入/输出？	类型		描述
输入		    no
				
 返  回  值：无

 作      者	：YFY/刘及华
 日      期：2012-12-12
 修改历史：
		日期		修改人		修改描述
******************************************************************************/
INT8U GPRS_TEST(void)
{
	uchar ucRet;
	INT8U  sendbuf[100];


	EA_vCls();
	EA_vDisp(1,1,"    GPRS连接测试    ");
	EA_vDisp(2,1,"     GPRS通讯中     ");
	EA_vDisp(3,1,"       请等待...    ");
	(void)EA_uiInkeyMs(2000);
	//更新时间
	if( Open_GPRS_Module() != ok )
	{
		lcddisperr("PPP拨号连接失败");
		return notok;
	}

	strcpy((void *)sendbuf, "@START000000TIME000");
	ucRet = GPRS_SEND_Receive(sendbuf,strlen((void *)sendbuf));
	if(ucRet != ok)
	{
		lcddisperr("同步服务器时间失败");
		return notok;
	}
	lcddisperr("同步服务器时间成功");
	return ok;
}

/******************************************************************************
 函数名称：GPRS_HEART
 功能描述：GPRS心跳
 参数名称：输入/输出？	类型		描述
输入		    no
				
 返  回  值：无
 

 作      者	：YFY
 日      期：2007-05-2
 修改历史：
		日期		修改人		修改描述
******************************************************************************/
//INT8U GPRS_HEART()
//{
//
//    INT8U sendbuf[256];
////	int   sendlength=0;
//    int   ret=0;
//
//    //打开串口
////  Open_GPRS_Module ();
//
//    //更新时间
//    strcpy((void *)sendbuf, "@START000000TTTT000");
//
//    ret = GPRS_SEND_Receive(sendbuf,strlen((void *)sendbuf));
//    if(ret!=ok)
//    {
//
//        //SIM_Tcpip_Close();
//
//        //Close_GPRS_Module ();
//        //DevStat.GPRS_CONNECT_FLAG=0;
//        return notok;
//    }
//
//    //SIM_Tcpip_Close();
//    //Close_GPRS_Module ();
//    return ok;
//
//
//}



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
//INT8U GPRS_OPER_CARD_CHECK(INT8U *opercardno)
//{
//    INT8U sendbuf[1024];
//    int  sendlength=0;
//    int   ret=0;
//
//    EA_vCls();
//    EA_vDisplay(1,"    操作员卡登陆    ");
//    EA_vDisplay(2,"     GPRS通讯中     ");
//    EA_vDisplay(3,"       请等待...    ");
//    EA_uiInkeyMs(500);
//
////  EA_vDisplay(4,"                    ");
//    EA_vDisplay(4,"      开始登陆      ");
//    EA_uiInkeyMs(500);
//
//    sprintf((void *)sendbuf, "@START%02x%02x%02x", DevStat.equ_id[0],DevStat.equ_id[1],DevStat.equ_id[2]);
//    strcat((void *)sendbuf, "OCHK010");//command
//
//    sprintf((void *)(sendbuf+19), "%02x%02x%02x%02x%02x", opercardno[0],opercardno[1],opercardno[2],opercardno[3],opercardno[4]);
//
//    sendlength = strlen((void *)sendbuf);
//
//    ret = GPRS_SEND_Receive(sendbuf,sendlength);
//
//    if(ret != ok)
//    {
//        EA_vCls();
//        EA_vDisplay(4,"      登录失败      ");
//        EA_uiInkeyMs(500);
//        return notok;
//    }
//    else
//    {
//        EA_vCls();
//        EA_vDisplay(4,"      登录成功      ");
//        Beep(1000);
//        EA_uiInkeyMs(500);
//        Beep(1000);
//    }
//    return ok;
//
//}

///******************************************************************************
// 函数名称：SendGrayRecord
// 功能描述：GPRS上传灰记录
// 参数名称：输入/输出？  类型        描述
//输入		    no
//
// 返  回  值：无
//
// 作      者 ：于福勇
// 日      期：2007-06-29
// 修改历史：
//        日期        修改人      修改描述
//******************************************************************************/
//INT8U  SendGrayRecord()
//{
////	char Address[30];
////	char Port[10];
////	short ch, i=0,j=0;
//    INT8U sendbuf[256];
//    int   sendlength=0;
//    INT8U temp_data[256];
////	INT16U crc=0;
////	INT8U Buff[256], str[20];
//    INT8U p[73];
////	char        flag=1;
//    static uint db_rec_num = 0;
//    int   ret=0;
//    uchar ucOpenID = 0;
//    uchar ucResult;
//    uint  uiReadCnt;
//    uint  uiRet;
//    uchar i;
//
//    EA_vCls();
//    EA_vDisplay(1,"灰记录上传");
//    EA_vDisplay(2,"GPRS通讯中");
//    EA_vDisplay(3,"请等待...");
//    EA_vDisplay(4,"网络连接中...");
//    EA_uiInkeyMs(500);
//
//    EA_vCls();
//    EA_vDisplay(4, "    传输数据中...   ");
//    EA_uiInkeyMs(500);
//
//    sprintf((void *)sendbuf, "@START%02x%02x%02x", DevStat.equ_id[0],DevStat.equ_id[1],DevStat.equ_id[2]);
//    strcat((void *)sendbuf, "SDAT114");//command
//
//    if ( EA_ucPFOpen((uchar *)grayrecFileName, &ucOpenID) != EM_ffs_SUCCESS )
//    {
//        (void)EA_ucPFClose(ucOpenID);
//        lcddisperr("打开灰记录失败");
//        return notok;
//    }
//    //有效参数信息条数
//    ucResult = EA_ucPFInfo(ucOpenID, EM_ffs_VALIDREC, &uiReadCnt);
//    if ( ucResult != EM_ffs_SUCCESS )
//    {
//        (void)EA_ucPFClose(ucOpenID);
//        lcddisperr("读取灰记录失败");
//        return notok;
//    }
//    db_rec_num = uiReadCnt;
//    if( EA_ucPFReadRec(ucOpenID, db_rec_num, 1, &uiRet, p) != EM_SUCCESS )  //上传最后一条
//    {
//        (void)EA_ucPFClose(ucOpenID);
//        lcddisperr("读取最后一条失败");
//        return notok;
//    }
//    (void)EA_ucPFClose(ucOpenID);
//    memcpy(temp_data, p, 57);
//    for( i=0; i<57; i++ )
//    {
//        sprintf((void *)(sendbuf+19+2*i), "%02x", temp_data[i]);
//    }
//    sendlength = strlen((void *)sendbuf);
//
//    ret = GPRS_SEND_Receive(sendbuf,sendlength);
//    if(ret != ok)
//    {
//        lcddisperr("    传输数据失败    ");
//        return notok;
//    }
//    else
//    {
//        lcddisperr("   传输灰记录完成   ");
//        DevStat.gray_flag = FALSE;
//        WriteParam();
//    }
//    return ok;
//}



