// This file contains all API to operate SIM200 GRPS Module.


#include  "main.h"
//#define _zb_debug_sendrec_

extern DEV_STAT       DevStat;					//设备状态
extern CARD_INFO      CardInfo;					//卡片信息，交易记录由此变量生成


extern const char parmFileName[];
extern const char blackFileName[];
extern const char currecFileName[];
extern const char conrecFileName[];
extern const char hisrecFileName[];
extern const char grayrecFileName[];

extern int socketID;

/******************************************************************************
 函数名称：gprs_send_record
 功能描述：GPRS上传记录
 参数名称：输入/输出？	类型		描述
输入		    no
				
 返  回  值：无
 
 作      者	：YFY
 日      期：2007-05-2
 修改历史：
		日期		修改人		修改描述
******************************************************************************/
INT8U gprs_send_record(void)
{
	uchar ucOpenID = 0;
	uint  uiReadCnt;
	short i=0,j=0;
	uchar sendbuf[1024];
//  uchar STR[20];
	uint  sendbuf_len = 0;
	INT8U str[50];
	INT8U p[60];
	int   ret=0;
	unsigned long	  record_index = 0;
	int   package_num=0,package_index=0, package_left=0,last_record_no=0;

	RECORD *prec = (RECORD *)p;

	memset(p, 0, sizeof(p));
	memset(sendbuf, 0, sizeof(sendbuf));

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
	SleepMs(2000);

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
		sprintf((void *)str, "   共有%4d条记录   ", DevStat.record_number-i);
		EA_vDisplay(3, (void *)str);
		(void)EA_uiInkeyMs(2000);

		last_record_no = i;

		package_num = (DevStat.record_number - i)/14;//14条一个包，可以打包的整数倍
		
		package_left = (DevStat.record_number - i)%14;//剩余条数
		
		package_index = 0;

		record_index = last_record_no + 1;

		if ( EA_ucPFOpen((uchar *)currecFileName, &ucOpenID) != EM_ffs_SUCCESS )
		{
			(void)EA_ucPFClose(ucOpenID);
			return notok;
		}

#ifdef _zb_debug_sendrec_
		Debugprintf("//////////////dbug_send_rec/////////////////\n");
#endif
		for( package_index=0; package_index<package_num; package_index++ )
		{
			sprintf((void *)sendbuf, "@START%02X%02X%02XUPDD%03d", DevStat.equ_id[0], DevStat.equ_id[1], DevStat.equ_id[2], (945));//上传记录
			sendbuf_len = 19;
			for(j=0; j<14; j++)
			{						
				(void)EA_ucPFReadRec(ucOpenID, record_index, 1, &uiReadCnt, p);
				record_index ++;
//	2B 商户号
		var_bcd2asc(sendbuf+sendbuf_len, DevStat.acnt_id, 2);
		sendbuf_len+=4;
#ifdef _zb_debug_sendrec_
		scrShowMsgInfo((char *)"MID", sendbuf+sendbuf_len-4, 4, ORG_ASCII);
#endif
//	3B 操作员
		var_bcd2asc(sendbuf+sendbuf_len, (uchar *)(&prec->oper_id[0]), 3); 
		sendbuf_len+=6;     	
#ifdef _zb_debug_sendrec_
		scrShowMsgInfo((char *)"operid", sendbuf+sendbuf_len-6, 6, ORG_ASCII);
#endif

//  3B HEX -- 交易序号
		var_bcd2asc(sendbuf+sendbuf_len, (uchar *)&prec->pos_purchase_serial_num, 3);
		sendbuf_len+=6;		
#ifdef _zb_debug_sendrec_
		scrShowMsgInfo((char *)"Tran Idx", sendbuf+sendbuf_len-6, 6, ORG_ASCII);
#endif

//	5B 车牌号
//  	memcpy(sendbuf+sendbuf_len, &prec->bus_number[0], 5);
		var_bcd2asc(sendbuf+sendbuf_len, &prec->bus_number[0], 5);
		sendbuf_len+=5;     	
#ifdef _zb_debug_sendrec_
		scrShowMsgInfo((char *)"bus num", sendbuf+sendbuf_len-5, 5, ORG_ASCII);
#endif
//	3B 员工卡卡号
		var_bcd2asc(sendbuf+sendbuf_len, &prec->driver_no[0], 3); 
		sendbuf_len+=6;     	
#ifdef _zb_debug_sendrec_
		scrShowMsgInfo((char *)"clerkid", sendbuf+sendbuf_len-6, 6, ORG_ASCII);
#endif

//  1B HEX  -- 加油类型
		var_bcd2asc(sendbuf+sendbuf_len, (uchar *)&prec->fuel_type, 1);
		sendbuf_len+=2;
#ifdef _zb_debug_sendrec_
		scrShowMsgInfo((char *)"fuel Type", sendbuf+sendbuf_len-2, 2, ORG_ASCII);
#endif

//  4B HEX  -- 加油单价
		sprintf((char*)(sendbuf+sendbuf_len), "%08d", prec->price);
		sendbuf_len+=8;
#ifdef _zb_debug_sendrec_
		scrShowMsgInfo((char *)"fuel price", sendbuf+sendbuf_len-8, 8, ORG_ASCII);
#endif
	
//  4B  HEX-- 交易金额
//  	memset(tmpbuf, 0, 10);
//  	packInt(tmpbuf, prec->fare);
//  	var_bcd2asc(sendbuf+sendbuf_len, tmpbuf, 4);
		sprintf((char*)(sendbuf+sendbuf_len), "%08d", prec->fare);
		sendbuf_len+=8;
#ifdef _zb_debug_sendrec_
		scrShowMsgInfo((char *)"The cash", sendbuf+sendbuf_len-8, 8, ORG_ASCII);
#endif
//  4B -- 交易油量
//  	memset(tmpbuf,0, 10);
//  	packInt(tmpbuf, prec->capacity);
//  	var_bcd2asc(sendbuf+sendbuf_len, tmpbuf, 4);
		sprintf((char*)(sendbuf+sendbuf_len), "%08d", prec->capacity);
		sendbuf_len+=8;	
#ifdef _zb_debug_sendrec_
		scrShowMsgInfo((char *)"The capacity", sendbuf+sendbuf_len-8, 8, ORG_ASCII);
#endif

//  7B -- YYYYMMDDHHMMSS -- BCD
		var_bcd2asc(sendbuf+sendbuf_len, (uchar *)&prec->card_in_time, 7);	
		sendbuf_len+=14;
#ifdef _zb_debug_sendrec_
		scrShowMsgInfo((char *)"Date time", sendbuf+sendbuf_len-14, 14, ORG_ASCII);
#endif
				
			}
			sendbuf[938 + 19] = 0;

			ret = GPRS_Send_Receive(sendbuf, (938+19));

			if( ret == ok )
			{
				DevStat.send_total_num+=15;
				sprintf((void *)str, "上传进度%03d%%", package_index*100/(package_num+1));
				EA_vDisplay(3, (void *)str);
				SleepMs(1000);
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
			sprintf((void *)sendbuf, "@START%02X%02X%02XUPDD%03d", DevStat.equ_id[0], DevStat.equ_id[1], DevStat.equ_id[2], package_left*63);//上传记录
			sendbuf_len = 19;
			for(j=0;j<package_left;j++)
			{			
				(void)EA_ucPFReadRec(ucOpenID, record_index, 1, &uiReadCnt, p);
				record_index ++;
//	2B 商户号
		var_bcd2asc(sendbuf+sendbuf_len, DevStat.acnt_id, 2);
		sendbuf_len+=4;
#ifdef _zb_debug_sendrec_
		scrShowMsgInfo((char *)"MID", sendbuf+sendbuf_len-4, 4, ORG_ASCII);
#endif
//	3B 操作员
		var_bcd2asc(sendbuf+sendbuf_len, (uchar *)(&prec->oper_id[0]), 1); 
		sendbuf_len+=2;     	
#ifdef _zb_debug_sendrec_
		scrShowMsgInfo((char *)"clerkid", sendbuf+sendbuf_len-2, 2, ORG_ASCII);
#endif

//  3B HEX -- 交易序号
		var_bcd2asc(sendbuf+sendbuf_len, (uchar *)&prec->pos_purchase_serial_num, 3);
		sendbuf_len+=6;		
#ifdef _zb_debug_sendrec_
		scrShowMsgInfo((char *)"Tran Idx", sendbuf+sendbuf_len-6, 6, ORG_ASCII);
#endif

//	5B 车牌号
//  	memcpy(sendbuf+sendbuf_len, &prec->bus_number[0], 5);
		var_bcd2asc(sendbuf+sendbuf_len, &prec->bus_number[0], 5);
		sendbuf_len+=5;     	
#ifdef _zb_debug_sendrec_
		scrShowMsgInfo((char *)"bus num", sendbuf+sendbuf_len-5, 5, ORG_ASCII);
#endif
//	3B 员工卡号
		var_bcd2asc(sendbuf+sendbuf_len, &prec->driver_no[0], 4); 
		sendbuf_len+=8;     	
#ifdef _zb_debug_sendrec_
		scrShowMsgInfo((char *)"driver_no", sendbuf+sendbuf_len-8, 8, ORG_ASCII);
#endif

//  1B HEX  -- 加油类型
		var_bcd2asc(sendbuf+sendbuf_len, (uchar *)&prec->fuel_type, 1);
		sendbuf_len+=2;
#ifdef _zb_debug_sendrec_
		scrShowMsgInfo((char *)"fuel Type", sendbuf+sendbuf_len-2, 2, ORG_ASCII);
#endif

//  4B HEX  -- 加油单价
		sprintf((char*)(sendbuf+sendbuf_len), "%08d", prec->price);
		sendbuf_len+=8;
#ifdef _zb_debug_sendrec_
		scrShowMsgInfo((char *)"fuel price", sendbuf+sendbuf_len-8, 8, ORG_ASCII);
#endif	
//  4B  HEX-- 交易金额
//  	memset(tmpbuf, 0, 10);
//  	packInt(tmpbuf, prec->fare);
//  	var_bcd2asc(sendbuf+sendbuf_len, tmpbuf, 4);
		sprintf((char*)(sendbuf+sendbuf_len), "%08d", prec->fare);
		sendbuf_len+=8;
#ifdef _zb_debug_sendrec_
		scrShowMsgInfo((char *)"The cash", sendbuf+sendbuf_len-8, 8, ORG_ASCII);
#endif
//  4B -- 交易油量
//  	memset(tmpbuf,0, 10);
//  	packInt(tmpbuf, prec->capacity);
//  	var_bcd2asc(sendbuf+sendbuf_len, tmpbuf, 4);
		sprintf((char*)(sendbuf+sendbuf_len), "%08d", prec->capacity);
		sendbuf_len+=8;	
#ifdef _zb_debug_sendrec_
		scrShowMsgInfo((char *)"The capacity", sendbuf+sendbuf_len-8, 8, ORG_ASCII);
#endif

//  7B -- YYYYMMDDHHMMSS -- BCD
		var_bcd2asc(sendbuf+sendbuf_len, (uchar *)&prec->card_in_time, 7);	
		sendbuf_len+=14;
#ifdef _zb_debug_sendrec_
		scrShowMsgInfo((char *)"Date time", sendbuf+sendbuf_len-14, 14, ORG_ASCII);
#endif
			}
			(void)EA_ucPFClose(ucOpenID);

			ret = GPRS_Send_Receive(sendbuf, package_left*67 + 19);		
		
			if( ret == ok )
			{
				DevStat.send_total_num = 0;
				EA_vDisplay(3, "      上传完成      ");
				SleepMs(2000);
				GPRS_Close();
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

	EA_ucPFDel((uchar *)currecFileName);
	DevStat.record_number = 0;
	WriteParam();
	GPRS_Close();
	
	return ok;
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
	
	if(memcmp(buff, "@START", 6)!=0)
		return -1;

	if(memcmp(buff+len-4, "END", 3)!=0)
		return -2;


	memcpy(temp_buf, buff+16, 3);
	temp_buf[3] = 0;
	length = atoi(temp_buf);				
	if(len !=27 + length)
		return -3;

	crc = Cal_CRC((unsigned char *)buff, len-8);

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
	EA_vDisplay(1,       "    按确认键继续    ");
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


	if( open_gprs_module() != ok )
	{
		lcddisperr("PPP拨号连接失败");
		Beeperr();
		return notok;
	}

	if( gprs_send_record() != ok )
	{
		EA_vCls();
		EA_vDisplay(1, "    有数据未上传    ");
		EA_vDisplay(2, "  请稍后再继续上传  ");
		GPRS_Close();
		Beeperr();
		EA_uiInkey(3);
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
	uchar ucOpenID = 0;

	if ( EA_ucPFOpen((uchar *)conrecFileName, &ucOpenID) != EM_ffs_SUCCESS )
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
 函数名称：gprs_test
 功能描述：GPRS测试
 参数名称：输入/输出？	类型		描述
输入		    no
				
 返  回  值：无

 作      者	：YFY/刘及华
 日      期：2012-12-12
 修改历史：
		日期		修改人		修改描述
******************************************************************************/
INT8U gprs_test(void)
{
	uchar ucRet;
	INT8U  sendbuf[50];

	EA_vCls();
	EA_vDisp(1,1,"    GPRS连接测试    ");
	EA_vDisp(2,1,"     GPRS通讯中     ");
	EA_vDisp(3,1,"       请等待...    ");
	(void)EA_uiInkeyMs(2000);
	//更新时间
	if( open_gprs_module() != ok )
	{
		lcddisperr("PPP拨号连接失败");
		return notok;
	}

	strcpy((void *)sendbuf, "@START000000TIME000");
	ucRet = GPRS_Send_Receive(sendbuf, strlen((void *)sendbuf));
	if(ucRet != ok)
	{
		lcddisperr("同步服务器时间失败");
		return notok;
	}
	lcddisperr("同步服务器时间成功");
	GPRS_Close();
	return ok;
}


