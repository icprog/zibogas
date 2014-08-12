// This file contains all API to operate SIM200 GRPS Module.


#include  "main.h"
//#define _zb_debug_sendrec_
#define REC_NUM_PER_PACK	14			//ÿ��14����¼
#define REC_LEN 67


extern DEV_STAT       DevStat;			//�豸״̬
extern CARD_INFO      CardInfo;		    //��Ƭ��Ϣ�����׼�¼�ɴ˱�������
extern GPRS gprs;


extern const char parmFileName[];
extern const char blackFileName[];
extern const char currecFileName[];
extern const char conrecFileName[];
extern const char hisrecFileName[];
extern const char grayrecFileName[];

extern int socketID;
INT8U sendbuf[1024];
/******************************************************************************
 �������ƣ�gprs_send_record
 ����������GPRS�ϴ���¼
 �������ƣ�����/�����	����		����
����		    no
				
 ��  ��  ֵ����
 
 ��      ��	��YFY
 ��      �ڣ�2007-05-2
 �޸���ʷ��
		����		�޸���		�޸�����
******************************************************************************/
//INT8U gprs_send_record(void)
//{
//
//	if (DevStat.request_all_rec == TRUE)
//	{
//		gprs.record_base_ptr = 0;
//		ReadParamRecNum();
//		if ( DevStat.NVRAM_cycled == TRUE )       //ѭ������12000����¼ȫ������
//		{
//			gprs.record_number = MAX_REC_NUM;     //��������ʱ�ļ�¼��ʼָ�������
//		}
//		else
//		{
//			gprs.record_number = DevStat.record_base_ptr + DevStat.record_number;   //��������ʱ�ļ�¼��ʼָ�������
//		}
//	}
//	else
//	{
//		gprs.record_base_ptr = DevStat.record_base_ptr;
//		ReadParamRecNum();
//		gprs.record_number = DevStat.record_number;
//	}
//
//}

INT8U gprs_send_record(void)
{
	uchar ucOpenID = 0;
	uint  uiReadCnt;
	short j=0;

	uint  sendbuf_len = 0;
	INT8U str[50];
	INT8U p[57];
	int   ret=0;
	unsigned long	  record_index = 0;
	int   package_num=0, package_index=0, package_left=0, last_record_no=0;

	RECORD *prec = (RECORD *)p;

	memset(p, 0, sizeof(p));
	memset(sendbuf, 0, sizeof(sendbuf));

//  DevStat.record_number = query_rec_num();
//
//  if( DevStat.record_number == 0 )
//  {
//  	lcddisperr((void *)"û�н��׼�¼!");
//  	return ok;
//  }

	EA_vCls();
	EA_vDisplay(1,"      ��¼�ϴ�      ");
	EA_vDisplay(2,"     GPRSͨѶ��     ");
	EA_vDisplay(3,"      ��ȴ�...     ");
	EA_vDisplay(4,"    ����������...   ");
	sprintf((void *)str, "   ����%4d����¼   ", DevStat.record_number);
	EA_vDisplay(5, (void *)str);
	SleepMs(1000);

	if(DevStat.record_number > 0)
	{
//  	i = 0;
//  	if( i != DevStat.send_total_num )//�ϵ�����
//  	{
//  		i = DevStat.send_total_num;
//  	}

//  	EA_vCls();
//  	EA_vDisplay(1, "    GPRS���ݴ���    ");
//  	EA_vDisplay(2, "    ���ͼ�¼�ϴ�    ");
//  	sprintf((void *)str, "   ����%4d����¼   ", DevStat.record_number-i);
//  	EA_vDisplay(3, (void *)str);
//  	(void)EA_uiInkeyMs(1200);

//  	last_record_no = i;
//
//  	package_num = (DevStat.record_number - i)/14;//14��һ���������Դ����������
//
//  	package_left = (DevStat.record_number - i)%14;//ʣ������
		
		package_num = (DevStat.record_number)/REC_NUM_PER_PACK;//14��һ���������Դ����������
		package_left = (DevStat.record_number)%REC_NUM_PER_PACK;//ʣ������

		package_index = 0;

		record_index = 1;

		if ( EA_ucPFOpen((uchar *)currecFileName, &ucOpenID) != EM_ffs_SUCCESS )
		{
			(void)EA_ucPFClose(ucOpenID);
			lcddisperr("�򿪼�¼�ļ�ʧ��");
			return notok;
		}

#ifdef _zb_debug_sendrec_
		Debugprintf("//////////////dbug_send_rec/////////////////\n");
#endif
		for( package_index=0; package_index<package_num; package_index++ )
		{
			sprintf((void *)sendbuf, "@START%02X%02X%02XUPDD%03d", DevStat.equ_id[0], DevStat.equ_id[1], DevStat.equ_id[2], (REC_LEN * 14));//�ϴ���¼
			sendbuf_len = 19;
			for(j=0; j<REC_NUM_PER_PACK; j++)
			{						
				(void)EA_ucPFReadRec(ucOpenID, record_index, 1, &uiReadCnt, p);
				record_index ++;
//	2B �̻���
		var_bcd2asc(sendbuf+sendbuf_len, DevStat.acnt_id, 2);
		sendbuf_len+=4;
#ifdef _zb_debug_sendrec_
		scrShowMsgInfo((char *)"MID", sendbuf+sendbuf_len-4, 4, ORG_ASCII);
#endif
//	3B ����Ա
		var_bcd2asc(sendbuf+sendbuf_len, (uchar *)(&prec->oper_id[0]), 3); 
		sendbuf_len+=6;     	
#ifdef _zb_debug_sendrec_
		scrShowMsgInfo((char *)"operid", sendbuf+sendbuf_len-6, 6, ORG_ASCII);
#endif

//  3B HEX -- �������
		var_bcd2asc(sendbuf+sendbuf_len, (uchar *)&prec->pos_purchase_serial_num, 3);
		sendbuf_len+=6;		
#ifdef _zb_debug_sendrec_
		scrShowMsgInfo((char *)"Tran Idx", sendbuf+sendbuf_len-6, 6, ORG_ASCII);
#endif

//	5B ���ƺ�
		memcpy(sendbuf+sendbuf_len, &prec->bus_number[0], 5);
		//var_bcd2asc(sendbuf+sendbuf_len, &prec->bus_number[0], 5);
		sendbuf_len+=5;     	
#ifdef _zb_debug_sendrec_
		scrShowMsgInfo((char *)"bus num", sendbuf+sendbuf_len-5, 5, ORG_ASCII);
#endif
//	3B Ա��������
		var_bcd2asc(sendbuf+sendbuf_len, &prec->driver_no[0], 3); 
		sendbuf_len+=6;     	
#ifdef _zb_debug_sendrec_
		scrShowMsgInfo((char *)"clerkid", sendbuf+sendbuf_len-6, 6, ORG_ASCII);
#endif

//  1B HEX  -- ��������
		var_bcd2asc(sendbuf+sendbuf_len, (uchar *)&prec->fuel_type, 1);
		sendbuf_len+=2;
#ifdef _zb_debug_sendrec_
		scrShowMsgInfo((char *)"fuel Type", sendbuf+sendbuf_len-2, 2, ORG_ASCII);
#endif

//  4B HEX  -- ���͵���
		sprintf((char*)(sendbuf+sendbuf_len), "%08d", prec->price);
		sendbuf_len+=8;
#ifdef _zb_debug_sendrec_
		scrShowMsgInfo((char *)"fuel price", sendbuf+sendbuf_len-8, 8, ORG_ASCII);
#endif
	
//  4B  HEX-- ���׽��
//  	memset(tmpbuf, 0, 10);
//  	packInt(tmpbuf, prec->fare);
//  	var_bcd2asc(sendbuf+sendbuf_len, tmpbuf, 4);
		sprintf((char*)(sendbuf+sendbuf_len), "%08d", prec->fare);
		sendbuf_len+=8;
#ifdef _zb_debug_sendrec_
		scrShowMsgInfo((char *)"The cash", sendbuf+sendbuf_len-8, 8, ORG_ASCII);
#endif
//  4B -- ��������
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
			sendbuf[REC_LEN * REC_NUM_PER_PACK + 19] = 0;

			ret = GPRS_Send_Receive(sendbuf, (REC_LEN * REC_NUM_PER_PACK + 19));

			if( ret == ok )
			{
//  			DevStat.send_total_num += 14;
				sprintf((void *)str, "�����ϴ�%03d��", package_index*REC_NUM_PER_PACK);
				EA_vDisplay(3, (void *)str);
				//SleepMs(1000);
			}
			else
				break;
				
		}
		(void)EA_ucPFClose(ucOpenID);

		if( ret != ok )
		{
			WriteParam();    //����ϵ���������
			EA_vCls();
			EA_vDisplay(1,"    GPRS���ݴ���    ");
			EA_vDisplay(2,"  ��������ϴ�ʧ��  ");	
			EA_vDisplay(3,"  �������������ϴ�  ");
			SleepMs(2000);
			GPRS_Close();
			return notok;
		}
		//ʣ���¼��һ����
		if(package_left > 0)
		{
			if ( EA_ucPFOpen((uchar *)currecFileName, &ucOpenID) != EM_ffs_SUCCESS )
			{
				(void)EA_ucPFClose(ucOpenID);
				return notok;
			}
			sprintf((void *)sendbuf, "@START%02X%02X%02XUPDD%03d", DevStat.equ_id[0], DevStat.equ_id[1], DevStat.equ_id[2], package_left * REC_LEN);//�ϴ���¼
			sendbuf_len = 19;
			for( j=0; j<package_left; j++ )
			{			
				(void)EA_ucPFReadRec(ucOpenID, record_index, 1, &uiReadCnt, p);
				record_index ++;
//	2B �̻���
		var_bcd2asc(sendbuf+sendbuf_len, DevStat.acnt_id, 2);
		sendbuf_len+=4;
#ifdef _zb_debug_sendrec_
		scrShowMsgInfo((char *)"MID", sendbuf+sendbuf_len-4, 4, ORG_ASCII);
#endif
//	3B ����Ա
		var_bcd2asc(sendbuf+sendbuf_len, (uchar *)(&prec->oper_id[0]), 3); 
		sendbuf_len+=6;     	
#ifdef _zb_debug_sendrec_
		scrShowMsgInfo((char *)"clerkid", sendbuf+sendbuf_len-2, 2, ORG_ASCII);
#endif

//  3B HEX -- �������
		var_bcd2asc(sendbuf+sendbuf_len, (uchar *)&prec->pos_purchase_serial_num, 3);
		sendbuf_len+=6;		
#ifdef _zb_debug_sendrec_
		scrShowMsgInfo((char *)"Tran Idx", sendbuf+sendbuf_len-6, 6, ORG_ASCII);
#endif

//	5B ���ƺ�
		memcpy(sendbuf+sendbuf_len, &prec->bus_number[0], 5);
		//var_bcd2asc(sendbuf+sendbuf_len, &prec->bus_number[0], 5);
		sendbuf_len+=5;     	
#ifdef _zb_debug_sendrec_
		scrShowMsgInfo((char *)"bus num", sendbuf+sendbuf_len-5, 5, ORG_ASCII);
#endif
//	3B Ա������
		var_bcd2asc(sendbuf+sendbuf_len, &prec->driver_no[0], 3); 
		sendbuf_len+=6;     	
#ifdef _zb_debug_sendrec_
		scrShowMsgInfo((char *)"driver_no", sendbuf+sendbuf_len-8, 8, ORG_ASCII);
#endif

//  1B HEX  -- ��������
		var_bcd2asc(sendbuf+sendbuf_len, (uchar *)&prec->fuel_type, 1);
		sendbuf_len+=2;
#ifdef _zb_debug_sendrec_
		scrShowMsgInfo((char *)"fuel Type", sendbuf+sendbuf_len-2, 2, ORG_ASCII);
#endif

//  4B HEX  -- ���͵���
		sprintf((char*)(sendbuf+sendbuf_len), "%08d", prec->price);
		sendbuf_len+=8;
#ifdef _zb_debug_sendrec_
		scrShowMsgInfo((char *)"fuel price", sendbuf+sendbuf_len-8, 8, ORG_ASCII);
#endif	
//  4B  HEX-- ���׽��
//  	memset(tmpbuf, 0, 10);
//  	packInt(tmpbuf, prec->fare);
//  	var_bcd2asc(sendbuf+sendbuf_len, tmpbuf, 4);
		sprintf((char*)(sendbuf+sendbuf_len), "%08d", prec->fare);
		sendbuf_len+=8;
#ifdef _zb_debug_sendrec_
		scrShowMsgInfo((char *)"The cash", sendbuf+sendbuf_len-8, 8, ORG_ASCII);
#endif
//  4B -- ��������
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

			ret = GPRS_Send_Receive(sendbuf, package_left*REC_LEN + 19);		
			if( ret == ok )
			{
//  			DevStat.send_total_num = 0;
				sprintf((void *)str, "�����ϴ�%02d��", (package_num)*REC_NUM_PER_PACK + package_left);
				EA_vDisplay(2, (void *)str);

				EA_vDisplay(3, "    ��¼�ϴ����    ");
				SleepMs(9000);
				GPRS_Close();
			}
			else
			{
				WriteParam();//����ϵ���������
				EA_vCls();
				EA_vDisplay(1,(void *)"    GPRS���ݴ���    ");
				EA_vDisplay(2,(void *)"  ��������ϴ�ʧ��  ");	
				EA_vDisplay(3,(void *)"  �������������ϴ�  ");
				SleepMs(2000);
				GPRS_Close();
				return notok;
			}
		}				
	}

	EA_ucPFDel((uchar *)currecFileName);//���ﲻɾ����¼���ݰɣ�
	DevStat.record_number = 0;
	WriteParam();
	GPRS_Close();
	
	return ok;
}



/******************************************************************************
 �������ƣ�PackageCheck
 �����������жϽ��ܰ��Ƿ���ȷ
 �������ƣ�����/�����	����		����
����		    	data	����
				
 ��  ��  ֵ����
 
 ��      ��	��YFY
 ��      �ڣ�2007-05-2
 �޸���ʷ��
		����		�޸���		�޸�����
******************************************************************************/
int  PackageCheck(char * buff, int len)
{
	char	CRC[5];
	//�����ж�
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
 �������ƣ�upload_price_log
 ����������GPRS�����
 �������ƣ�����/�����	����		����
����		    data	����
				
 ��  ��  ֵ����
 
 ��      ��	������
 ��      �ڣ�2014-08-05
 �޸���ʷ��
		����		�޸���		�޸�����
******************************************************************************/
int upload_price_log(INT32U  *tmp_price)
{
	int sendbuf_len = 0;
	BUS_TIME ltime;
	INT8U ret;
	INT32U price[4];

	Get_Time(&ltime);
	memset(sendbuf, 0, sizeof(sendbuf));
	memcpy((INT8U *)price, (INT8U *)tmp_price, 16);

	EA_vCls();
	EA_vDisplay(2, " �ϴ������ͼ���־...");
	EA_uiInkeyMs(1200);
	
	if( open_gprs_module() != ok )
	{
		lcddisperr("PPP��������ʧ��");
		return notok;
	}

	
	strcpy((void *)sendbuf, "@START000000TIME000");
	ret = GPRS_Send_Receive(sendbuf, strlen((void *)sendbuf));
	if(ret != ok)
	{
		lcddisperr("ͬ��������ʱ��ʧ��");
		return notok;
	}

	lcddisperr("ͬ��������ʱ��ɹ�");

	sprintf((void *)sendbuf, "@START%02X%02X%02XUPLG048", DevStat.equ_id[0], DevStat.equ_id[1], DevStat.equ_id[2]);//�ϴ���¼
	sendbuf_len = 19;

	//	2B �̻���
	var_bcd2asc(sendbuf+sendbuf_len, DevStat.acnt_id, 2);
	sendbuf_len+=4;
	#ifdef _zb_debug_sendrec_
	scrShowMsgInfo((char *)"MID", sendbuf+sendbuf_len-4, 4, ORG_ASCII);
	#endif
	//	3B ����Ա
	var_bcd2asc(sendbuf+sendbuf_len, DevStat.oper_cardno, 3); 
	sendbuf_len+=6;     	
	#ifdef _zb_debug_sendrec_
	scrShowMsgInfo((char *)"operid", sendbuf+sendbuf_len-6, 6, ORG_ASCII);
	#endif

//  1B HEX  -- 2B ascii��������
//	var_bcd2asc(sendbuf+sendbuf_len, 0x00, 1);
	sendbuf[sendbuf_len] = 0x30;
	sendbuf_len ++;
	sendbuf[sendbuf_len] = 0x30;
	sendbuf_len ++;
#ifdef _zb_debug_sendrec_
	scrShowMsgInfo((char *)"fuel Type", sendbuf+sendbuf_len-2, 2, ORG_ASCII);
#endif

//  4B HEX  -- ���͵���
	sprintf((char*)(sendbuf+sendbuf_len), "%04d", (INT16U)price[0]);
	sendbuf_len+=4;
#ifdef _zb_debug_sendrec_
	scrShowMsgInfo((char *)"fuel price", sendbuf+sendbuf_len-4, 4, ORG_ASCII);
#endif

	//  1B HEX  -- 2B ascii��������
	//var_bcd2asc(sendbuf+sendbuf_len, 0x01, 1);
	sendbuf[sendbuf_len] = 0x30;
	sendbuf_len ++;
	sendbuf[sendbuf_len] = 0x31;
	sendbuf_len ++;
#ifdef _zb_debug_sendrec_
	scrShowMsgInfo((char *)"fuel Type", sendbuf+sendbuf_len-2, 2, ORG_ASCII);
#endif

//  4B HEX  -- ���͵���
	sprintf((char*)(sendbuf+sendbuf_len), "%04d", (INT16U)price[1]);
	sendbuf_len+=4;
#ifdef _zb_debug_sendrec_
	scrShowMsgInfo((char *)"fuel price", sendbuf+sendbuf_len-4, 4, ORG_ASCII);
#endif

	//  1B HEX  -- 2B ascii��������
	//var_bcd2asc(sendbuf+sendbuf_len, 0x02, 1);
	//sendbuf_len+=2;
	sendbuf[sendbuf_len] = 0x30;
	sendbuf_len ++;
	sendbuf[sendbuf_len] = 0x32;
	sendbuf_len ++;
#ifdef _zb_debug_sendrec_
	scrShowMsgInfo((char *)"fuel Type", sendbuf+sendbuf_len-2, 2, ORG_ASCII);
#endif

//  4B HEX  -- ���͵���
	sprintf((char*)(sendbuf+sendbuf_len), "%04d", (INT16U)price[2]);
	sendbuf_len+=4;
#ifdef _zb_debug_sendrec_
	scrShowMsgInfo((char *)"fuel price", sendbuf+sendbuf_len-4, 4, ORG_ASCII);
#endif

	//  1B HEX  -- 2B ascii��������
	//var_bcd2asc(sendbuf+sendbuf_len, 0x03, 1);
	//sendbuf_len+=2;
	sendbuf[sendbuf_len] = 0x30;
	sendbuf_len ++;
	sendbuf[sendbuf_len] = 0x33;
	sendbuf_len ++;
#ifdef _zb_debug_sendrec_
	scrShowMsgInfo((char *)"fuel Type", sendbuf+sendbuf_len-2, 2, ORG_ASCII);
#endif

//  4B HEX  -- ���͵���
	sprintf((char*)(sendbuf+sendbuf_len), "%04d", (INT16U)price[3]);
	sendbuf_len+=4;
#ifdef _zb_debug_sendrec_
	scrShowMsgInfo((char *)"fuel price", sendbuf+sendbuf_len-4, 4, ORG_ASCII);
#endif

//  7B -- YYYYMMDDHHMMSS -- BCD
	var_bcd2asc(sendbuf+sendbuf_len, (INT8U *)&ltime, 7);	
	sendbuf_len+=14;
#ifdef _zb_debug_sendrec_
	scrShowMsgInfo((char *)"Date time", sendbuf+sendbuf_len-14, 14, ORG_ASCII);
#endif
			
	sendbuf[sendbuf_len] = 0;

	ret = GPRS_Send_Receive(sendbuf, sendbuf_len);

	if( ret == ok )
		return ok;
	else
		return notok;

}

/******************************************************************************
 �������ƣ�Gprs_Upload_data
 ����������GPRS�����
 �������ƣ�����/�����	����		����
����		    data	����
				
 ��  ��  ֵ����
 
 ��      ��	���ڸ���
 ��      �ڣ�2007-03-05
 �޸���ʷ��
		����		�޸���		�޸�����
******************************************************************************/
int Gprs_Upload_data(void)
{
	char 		buf[22];
	uchar      ret;

	memset((void *)buf, 0, sizeof(buf));

	EA_vCls();

	DevStat.record_number = query_rec_num();

	if( DevStat.record_number == 0 )
	{
		lcddisperr("û�м�¼Ҫ�ϴ�");
		GPRS_Close();
		return ok;
	}

	EA_vDisplay(2,       "    ��ȷ�ϼ�����    ");
	sprintf((void *)buf, "����%04d����¼δ�ϴ�", DevStat.record_number);
	EA_vDisplay(1, buf);
	EA_vDisplay(3,       "��ȷ�����ڰ�ȫ��Χ��");
	EA_vDisplay(4,       "    �ٽ��������ϴ�  ");

	if( EA_uiInkey(0) == EM_key_ENTER )
	{
		;
	}
	else
	{
		GPRS_Close();
		return notok;
	}


	if( open_gprs_module() != ok )
	{
		lcddisperr("PPP��������ʧ��");
		Beeperr();
		GPRS_Close();
		return notok;
	}

	strcpy((void *)sendbuf, "@START000000TIME000");
	ret = GPRS_Send_Receive(sendbuf, strlen((void *)sendbuf));
	if(ret != ok)
	{
		lcddisperr("ͬ��������ʱ��ʧ��");
		return notok;
	}

	if( gprs_send_record() != ok )
	{
		EA_vCls();
		EA_vDisplay(1, "    ������δ�ϴ�    ");
		EA_vDisplay(2, "  ���Ժ��ټ����ϴ�  ");
		Beeperr();
		EA_uiInkey(3);
		GPRS_Close();
		return notok;
	}

//  Get_Time(&time_data);
//  memcpy(&DevStat.binhand.down_datetime, &time_data, sizeof(BUS_TIME));
//
//  //��ӡ�ս�����
//  print_work_note();
//  memcpy(record, (INT8U*)&DevStat.binhand, sizeof(DevStat.binhand));
//  SaveNoteRecord(record);
//  DevStat.driver_is_working = FALSE;
//  memset(&DevStat.binhand, 0x00, sizeof(DevStat.binhand));
//  WriteParam();
	return ok;
}


// *****************************************************************
// ���ܣ�		�����ս��¼
// ��ڲ�����	
// ���ڲ�����	Null
// ���ߣ�
// ����ֵ��		0://ok,����ɹ�			
//				1;//��¼����������ʧ��
// *****************************************************************
INT8U SaveNoteRecord(unsigned char * NoteRecord)
{
	uchar ucOpenID = 0;

	if ( EA_ucPFOpen((uchar *)conrecFileName, &ucOpenID) != EM_ffs_SUCCESS )
	{
		(void)EA_ucPFClose(ucOpenID);
		lcddisperr("���ս��ļ�ʧ��");
		return notok;
	}
	if ( EA_ucPFWriteRec(ucOpenID, 0, NoteRecord) !=  EM_ffs_SUCCESS )
	{
		(void)EA_ucPFClose(ucOpenID);
		lcddisperr("д�ս��ļ�ʧ��");
		return notok;
	}
	(void)EA_ucPFClose(ucOpenID);

	return ok;
}


/******************************************************************************
 �������ƣ�gprs_test
 ����������GPRS����
 �������ƣ�����/�����	����		����
����		    no
				
 ��  ��  ֵ����

 ��      ��	��YFY/������
 ��      �ڣ�2012-12-12
 �޸���ʷ��
		����		�޸���		�޸�����
******************************************************************************/
INT8U gprs_test(void)
{
	uchar ucRet;

	EA_vCls();
	EA_vDisp(1,1,"    GPRS���Ӳ���    ");
	EA_vDisp(2,1,"     GPRSͨѶ��     ");
	EA_vDisp(3,1,"       ��ȴ�...    ");
	(void)EA_uiInkeyMs(2000);
	//����ʱ��
	if( open_gprs_module() != ok )
	{
		lcddisperr("PPP��������ʧ��");
		return notok;
	}

	strcpy((void *)sendbuf, "@START000000TIME000");
	ucRet = GPRS_Send_Receive(sendbuf, strlen((void *)sendbuf));
	if(ucRet != ok)
	{
		lcddisperr("ͬ��������ʱ��ʧ��");
		return notok;
	}
	lcddisperr("ͬ��������ʱ��ɹ�");
	GPRS_Close();
	return ok;
}


