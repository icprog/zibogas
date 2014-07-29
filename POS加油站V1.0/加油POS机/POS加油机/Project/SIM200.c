// This file contains all API to operate SIM200 GRPS Module.


#include  "main.h"


//extern INT8U  KeySector[14][6];
extern DEV_STAT       DevStat;					//�豸״̬
extern CARD_INFO      CardInfo;					//��Ƭ��Ϣ�����׼�¼�ɴ˱�������
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
// �������ƣ�GPRS_CAL_KEY
// ����������GPRS�����ֵ��Կ
// �������ƣ�����/�����  ����        ����
//����		    no
//
// ��  ��  ֵ����
//
// ��      �� ��YFY
// ��      �ڣ�2007-05-2
// �޸���ʷ��
//        ����        �޸���      �޸�����
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
//    (void)EA_vDisplay(1,"      ��ֵ����      ");
//    (void)EA_vDisplay(2,"     GPRSͨѶ��     ");
//    (void)EA_vDisplay(3,"       ��ȴ�...    ");
//    (void)EA_uiInkeyMs(500);
//
//    EA_vDisplay(4,"      ��������      ");
//
////	printf_debug("config ok");
////�ж��Ƿ���δ�ϴ�������
//    if(DevStat.have_data_not_send_flag)//��δ�ϴ�����
//    {
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
////�����ϴ�
//        //DevStat.last_trade_type=TT_CREDIT_MONEY;
//        if(DevStat.last_trade_type != TT_CARD_SALE)//��ֵ��������
//        {
//            sprintf((void *)sendbuf, "@START%02x%02x%02x", DevStat.equ_id[0],DevStat.equ_id[1],DevStat.equ_id[2]);
//            strcat((void *)sendbuf, (void *)"SDAT114");//command
////  		db_rec_num = DB_count_records(DB_REC);
////  	//	sprintf(str, "db_n:%d", db_rec_num);
////  	//	printf_debug(str);
////  		db_rec_num --;
////  		p = (INT8U *)DB_jump_to_record(DB_REC, db_rec_num, &flag);	//ȡ��¼
//            if ( EA_ucPFOpen((uchar *)currecFileName, &ucOpenID) != EM_ffs_SUCCESS )
//            {
//                (void)EA_ucPFClose(ucOpenID);
//                return notok;
//            }
//            //��Ч������Ϣ����
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
//        else//�ۿ�����----���ӳ�ֵ����
//        {
//            sprintf((void *)sendbuf, "@START%02x%02x%02x", DevStat.equ_id[0],DevStat.equ_id[1],DevStat.equ_id[2]);
//            strcat((void *)sendbuf, "SDAT228");//command
////  		db_rec_num = DB_count_records(DB_REC);
////  		db_rec_num -= 2;//�ۿ�����
////  		p = (INT8U *)DB_jump_to_record(DB_REC, db_rec_num, &flag);	//ȡ��¼
//            if ( EA_ucPFOpen((uchar *)currecFileName, &ucOpenID) != EM_ffs_SUCCESS )
//            {
//                (void)EA_ucPFClose(ucOpenID);
//                return notok;
//            }
//            //��Ч������Ϣ����
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
//            db_rec_num++;//��ֵ����
////  		p = (INT8U *)DB_jump_to_record(DB_REC, db_rec_num, &flag);	//ȡ��¼
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
//            lcddisperr("    ���ݴ���ʧ��    ");
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
//    lcddisperr("      ��ֵ����      ");
//
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
////��Կ����
//
//    sprintf((void *)sendbuf, "@START%02x%02x%02x", DevStat.equ_id[0],DevStat.equ_id[1],DevStat.equ_id[2]);
//    strcat((void *)sendbuf, "CALK024");//command
//
//    memcpy(temp_data, &CardInfo._CARD_NUMBER.detail_card_number.city_code[0], 2);//���д���
//    memcpy(temp_data+2, &CardInfo.csn[0], 4);//CSNӲ������
//    memcpy(temp_data+6, &CardInfo._CARD_NUMBER.detail_card_number.serial_number[2], 2);//��������ˮ�ź�2�ֽ�
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
//        lcddisperr("��ֵ����ʧ��");
//        return notok;
//    }
//    else
//    {
//        DevStat.have_data_not_send_flag = FALSE;
//        WriteParam();
//    }
//    EA_vDisplay(4,"    ��ֵ�������    ");
//    EA_uiInkeyMs(500);
//    return ok;
//}
//
//
//

/******************************************************************************
 �������ƣ�GPRS_SEND_RECORD
 ����������GPRS�ϴ���¼
 �������ƣ�����/�����	����		����
����		    no
				
 ��  ��  ֵ����
 
 ��      ��	��YFY
 ��      �ڣ�2007-05-2
 �޸���ʷ��
		����		�޸���		�޸�����
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
		lcddisperr((void *)"û�н��׼�¼!");
		return ok;
	}

	EA_vCls();
	EA_vDisplay(1,"      ��¼�ϴ�      ");
	EA_vDisplay(2,"     GPRSͨѶ��     ");
	EA_vDisplay(3,"      ��ȴ�...     ");
	EA_vDisplay(4,"    ����������...   ");
	(void)EA_uiInkeyMs(2000);
//  SleepMs(2000);

	if(DevStat.record_number > 0)
	{
		i = 0;
		if( i != DevStat.send_total_num )//�ϵ�����
		{
			i = DevStat.send_total_num;
		}

		EA_vCls();	
		EA_vDisplay(1, "    GPRS���ݴ���    ");
		EA_vDisplay(2, "    ���Ѽ�¼�ϴ�    ");	
		sprintf((void *)str, "   ����%4d����¼   ",  DevStat.record_number-i);
		EA_vDisplay(3, (void *)str);
		(void)EA_uiInkeyMs(2000);

		last_record_no = i;

		package_num = (DevStat.record_number - i)/10;//10��һ���������Դ����������
		
		package_left = (DevStat.record_number - i)%10;//ʣ������
		
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
				DevStat.acnt_id[0],DevStat.acnt_id[1],DevStat.equ_id[0],DevStat.equ_id[1],DevStat.equ_id[2]);//�ϴ���¼
			sprintf((void *)(sendbuf+19+10), "%02x%02x%02x%02x%02x%02x", DevStat.binhand.work_datetime.year,DevStat.binhand.work_datetime.month,
				DevStat.binhand.work_datetime.day,DevStat.binhand.work_datetime.hour,DevStat.binhand.work_datetime.minute,DevStat.binhand.work_datetime.second);//�ϴ���¼
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
				sprintf((void *)str, "�ϴ�����%03d%%",package_index*100/(package_num+1));
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
			sprintf((void *)sendbuf, "@START%02x%02x%02xUPDD%03d%02x%02x%02x%02x%02x", DevStat.equ_id[0],DevStat.equ_id[1],DevStat.equ_id[2],package_left*49+22, 
					DevStat.acnt_id[0],DevStat.acnt_id[1],DevStat.equ_id[0],DevStat.equ_id[1],DevStat.equ_id[2]);//�ϴ���¼
			sprintf((void *)(sendbuf+19+10), "%02x%02x%02x%02x%02x%02x", DevStat.binhand.work_datetime.year,DevStat.binhand.work_datetime.month,
					DevStat.binhand.work_datetime.day,DevStat.binhand.work_datetime.hour,
					DevStat.binhand.work_datetime.minute,DevStat.binhand.work_datetime.second);//�ϴ���¼
			
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
						DevStat.acnt_id[0],DevStat.acnt_id[1],DevStat.equ_id[0],DevStat.equ_id[1],DevStat.equ_id[2]);//�ϴ���¼
				sprintf((void *)(sendbuf+19+10), "%02x%02x%02x%02x%02x%02x", DevStat.binhand.work_datetime.year,DevStat.binhand.work_datetime.month,
						DevStat.binhand.work_datetime.day,DevStat.binhand.work_datetime.hour,DevStat.binhand.work_datetime.minute,DevStat.binhand.work_datetime.second);//�ϴ���¼
				
				ret = GPRS_SEND_Receive(sendbuf,19+22);
				if(ret == ok)
				{
					DevStat.send_total_num = 0;
					EA_vDisplay(3, "      �ϴ����      ");
					SleepMs(2000);
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
 �������ƣ�GPRS_TRANS_DATA
 ����������GPRS�����
 �������ƣ�����/�����	����		����
����		    index	��������
				
 ��  ��  ֵ����
 
 ��      ��	���ڸ���
 ��      �ڣ�2007-03-05
 �޸���ʷ��
		����		�޸���		�޸�����
******************************************************************************/
INT8U  GPRS_TRANS_DATA()
{
	INT8U sendbuf[1024];
	int	 sendlength=0;	
	int  ret=0;
	
	EA_vCls();
	EA_vDisplay(1, "     ����Աǩ��     ");
	EA_vDisplay(2, "     GPRSͨѶ��     ");
	EA_vDisplay(3, "      ��ȴ�...     ");
	EA_uiInkeyMs(2000);
	
	EA_vCls();
	EA_vDisplay(1, "      ��������      ");
	EA_vDisplay(2, " �����ս�������.... ");

	sprintf((void *)sendbuf, "@START%02x%02x%02x", DevStat.equ_id[0],DevStat.equ_id[1],DevStat.equ_id[2]);
	strcat((void *)sendbuf, "OQUT099");//command 

//oper id
	sprintf((void *)(sendbuf+19), "%02x%02x%02x",DevStat.binhand.oper_id[0],DevStat.binhand.oper_id[1],DevStat.binhand.oper_id[2]);

//	BUS_TIME work_datetime; //�ϰ�ʱ��
	sprintf((void *)(sendbuf+25), "%02x%02x%02x%02x%02x%02x%02x",DevStat.binhand.work_datetime.century,DevStat.binhand.work_datetime.year,
		   DevStat.binhand.work_datetime.month,DevStat.binhand.work_datetime.day,DevStat.binhand.work_datetime.hour,
		   DevStat.binhand.work_datetime.minute,DevStat.binhand.work_datetime.second);

//	BUS_TIME down_datetime; //�°�ʱ��
	sprintf((void *)(sendbuf+39), "%02x%02x%02x%02x%02x%02x%02x",DevStat.binhand.down_datetime.century,DevStat.binhand.down_datetime.year,
		   DevStat.binhand.down_datetime.month,DevStat.binhand.down_datetime.day,DevStat.binhand.down_datetime.hour,
		   DevStat.binhand.down_datetime.minute,DevStat.binhand.down_datetime.second);

	sendlength = strlen((void *)sendbuf);

	ret = GPRS_SEND_Receive(sendbuf,sendlength);
	if(ret != ok)
	{
		goto end;
	}

	EA_vDisplay(4, "   ����Ա�ս����   ");
	EA_uiInkeyMs(2000);
	return ok;
	
end:
	WriteParam();

	EA_vCls();
	EA_vDisplay(1, "    GPRS���ݴ���    ");
	EA_vDisplay(2, "  �������ͨѶʧ��  ");
	EA_vDisplay(3, "  �������������ϴ�  ");
	(void)EA_uiInkeyMs(2000);
	return notok;
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
	char 		buf[30];

	memset((void *)buf, 0, sizeof(buf));

	EA_vCls();
	EA_vDisplay(1,       "   δ�ϴ���¼ͳ��   ");
	sprintf((void *)buf, "   ����%04d����¼   ", DevStat.record_number);
	EA_vDisplay(2, buf);
	EA_vDisplay(3,       "��ȷ�����ڰ�ȫ��Χ��");
	EA_vDisplay(4,       "    �ٽ��������ϴ�  ");
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
		lcddisperr("PPP��������ʧ��");
		Beeperr();
		return notok;
	}

	if( GPRS_SEND_RECORD() != ok )
	{
		EA_vCls();
		EA_vDisplay(1, "    ������δ�ϴ�    ");
		EA_vDisplay(2, "   ����Ա�ս�ʧ��   ");
		EA_vDisplay(3, "    ������ǩ��!!!   ");
		Beeperr();
		EA_uiInkey(4);
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
	uchar 		ucOpenID = 0;
//	uint 		uiReadCnt = 0;

	if ( EA_ucPFOpen((uchar *)hisrecFileName, &ucOpenID) != EM_ffs_SUCCESS )
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
 �������ƣ�GPRS_TEST
 ����������GPRS����
 �������ƣ�����/�����	����		����
����		    no
				
 ��  ��  ֵ����

 ��      ��	��YFY/������
 ��      �ڣ�2012-12-12
 �޸���ʷ��
		����		�޸���		�޸�����
******************************************************************************/
INT8U GPRS_TEST(void)
{
	uchar ucRet;
	INT8U  sendbuf[100];


	EA_vCls();
	EA_vDisp(1,1,"    GPRS���Ӳ���    ");
	EA_vDisp(2,1,"     GPRSͨѶ��     ");
	EA_vDisp(3,1,"       ��ȴ�...    ");
	(void)EA_uiInkeyMs(2000);
	//����ʱ��
	if( Open_GPRS_Module() != ok )
	{
		lcddisperr("PPP��������ʧ��");
		return notok;
	}

	strcpy((void *)sendbuf, "@START000000TIME000");
	ucRet = GPRS_SEND_Receive(sendbuf,strlen((void *)sendbuf));
	if(ucRet != ok)
	{
		lcddisperr("ͬ��������ʱ��ʧ��");
		return notok;
	}
	lcddisperr("ͬ��������ʱ��ɹ�");
	return ok;
}

/******************************************************************************
 �������ƣ�GPRS_HEART
 ����������GPRS����
 �������ƣ�����/�����	����		����
����		    no
				
 ��  ��  ֵ����
 

 ��      ��	��YFY
 ��      �ڣ�2007-05-2
 �޸���ʷ��
		����		�޸���		�޸�����
******************************************************************************/
//INT8U GPRS_HEART()
//{
//
//    INT8U sendbuf[256];
////	int   sendlength=0;
//    int   ret=0;
//
//    //�򿪴���
////  Open_GPRS_Module ();
//
//    //����ʱ��
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
 �������ƣ�GPRS_OPER_CARD_CHECK
 ����������GPRS������Ա���Ƿ�Ϊ������
 �������ƣ�����/�����	����		����
����		    opercardno: ����Ա����
				
 ��  ��  ֵ����
 
 ��      ��	��YFY
 ��      �ڣ�2007-05-2
 �޸���ʷ��
		����		�޸���		�޸�����
******************************************************************************/
//INT8U GPRS_OPER_CARD_CHECK(INT8U *opercardno)
//{
//    INT8U sendbuf[1024];
//    int  sendlength=0;
//    int   ret=0;
//
//    EA_vCls();
//    EA_vDisplay(1,"    ����Ա����½    ");
//    EA_vDisplay(2,"     GPRSͨѶ��     ");
//    EA_vDisplay(3,"       ��ȴ�...    ");
//    EA_uiInkeyMs(500);
//
////  EA_vDisplay(4,"                    ");
//    EA_vDisplay(4,"      ��ʼ��½      ");
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
//        EA_vDisplay(4,"      ��¼ʧ��      ");
//        EA_uiInkeyMs(500);
//        return notok;
//    }
//    else
//    {
//        EA_vCls();
//        EA_vDisplay(4,"      ��¼�ɹ�      ");
//        Beep(1000);
//        EA_uiInkeyMs(500);
//        Beep(1000);
//    }
//    return ok;
//
//}

///******************************************************************************
// �������ƣ�SendGrayRecord
// ����������GPRS�ϴ��Ҽ�¼
// �������ƣ�����/�����  ����        ����
//����		    no
//
// ��  ��  ֵ����
//
// ��      �� ���ڸ���
// ��      �ڣ�2007-06-29
// �޸���ʷ��
//        ����        �޸���      �޸�����
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
//    EA_vDisplay(1,"�Ҽ�¼�ϴ�");
//    EA_vDisplay(2,"GPRSͨѶ��");
//    EA_vDisplay(3,"��ȴ�...");
//    EA_vDisplay(4,"����������...");
//    EA_uiInkeyMs(500);
//
//    EA_vCls();
//    EA_vDisplay(4, "    ����������...   ");
//    EA_uiInkeyMs(500);
//
//    sprintf((void *)sendbuf, "@START%02x%02x%02x", DevStat.equ_id[0],DevStat.equ_id[1],DevStat.equ_id[2]);
//    strcat((void *)sendbuf, "SDAT114");//command
//
//    if ( EA_ucPFOpen((uchar *)grayrecFileName, &ucOpenID) != EM_ffs_SUCCESS )
//    {
//        (void)EA_ucPFClose(ucOpenID);
//        lcddisperr("�򿪻Ҽ�¼ʧ��");
//        return notok;
//    }
//    //��Ч������Ϣ����
//    ucResult = EA_ucPFInfo(ucOpenID, EM_ffs_VALIDREC, &uiReadCnt);
//    if ( ucResult != EM_ffs_SUCCESS )
//    {
//        (void)EA_ucPFClose(ucOpenID);
//        lcddisperr("��ȡ�Ҽ�¼ʧ��");
//        return notok;
//    }
//    db_rec_num = uiReadCnt;
//    if( EA_ucPFReadRec(ucOpenID, db_rec_num, 1, &uiRet, p) != EM_SUCCESS )  //�ϴ����һ��
//    {
//        (void)EA_ucPFClose(ucOpenID);
//        lcddisperr("��ȡ���һ��ʧ��");
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
//        lcddisperr("    ��������ʧ��    ");
//        return notok;
//    }
//    else
//    {
//        lcddisperr("   ����Ҽ�¼���   ");
//        DevStat.gray_flag = FALSE;
//        WriteParam();
//    }
//    return ok;
//}



