/******************************************************************************
Copyright 2006 	�������ķ�������޹�˾
All rights riserved.
�� �� ��	��cardprocess.c
ģ�����ƣ�EH2108 ˢ���ӳ���
���ܸ�Ҫ�������� 

******************************************************************************/
#include "main.h" 
/******************************************************************************
															ȫ�ֱ�������
******************************************************************************/
//����CPU��ʹ�õı���
extern  INT8U const    SELECT_USER_ADF2[7];
extern INT8U const    VERIFY_PIN[11];//��֤����
extern INT8U const    READ_15_FILE[5];//��ȡ15�ļ�����
extern  INT8U const    READ_16_FILE[5];//��ȡ16�ļ�����

////��PSAM���������õ���һЩcode����
extern INT8U   SELECT_SAM_ADF1[7];		//ѡ1001Ŀ¼
extern INT8U   CAL_TAC_M1[5];							//M1�����׹����У�����TAC��
extern INT8U const    TAC_INIT[5];
extern INT8U const    TAC_JDINIT[5];

extern DEV_STAT       DevStat;					//�豸״̬
extern CARD_INFO      CardInfo;					//��Ƭ��Ϣ�����׼�¼�ɴ˱�������
extern ERROR_CARD_INFO ErrCardInfo;				//����״̬
//extern GRAYRECORD  GrayRecord;
extern CLERK_STRUCT clerk_list[MAX_CLERK_NUM];

extern DevHandle hSIMHandle;     //SIM��
extern DevHandle hCpuHandle;      //CPU��
extern DevHandle hSam1Handle;      //SAM1��
extern DevHandle hMifsHandle;      //SAM1��
//extern DevHandle hApptimerHandle;

extern const char parmFileName[];
extern const char blackFileName[];
extern const char currecFileName[];
extern const char conrecFileName[];
extern const char hisrecFileName[];
extern const char grayrecFileName[];

//#define Debug_M1card   //by frank

INT8U  main_menu[] = "1.��������          2.��Ϣ��ѯ          3.���ݴ���          4.����Ա�����޸�    ";
INT8U  KeySector[14][6];								//M1��������Կ��ÿ������ԿΪ6�ֽ�

INT8U  app_menu[8*20+2] = { "F1--�ۿ�            F2--��ֵ            F3--����            04--ǩ��            05--��ѯ            06--��ӡ            07--����            08--ϵͳ����        " };
//INT8U  PersonInfo[5*20+2]= { "1--�ֿ�������       2--�ֿ����Ա�       3--�ֿ���֤��       4--��Ч֤����       5--�������         "};
//#define _debug_m1card_
// *****************************************************************
// ���ܣ�		printf_debug
// ��ڲ�����	buf    ��ʾ����ַ���
//				
// ���ڲ�����	Null
// ���ߣ� ������
// ����������
// *****************************************************************
void printf_debug(char * buf)
{
	EA_vCls();
	EA_vDisp(3, 1, (void *)buf);
	Beep(1000);
	(void)EA_uiInkey(2);

}

// *****************************************************************
// ���ܣ�		����
// ��ڲ�����	i=0:����"��"һ��
//				i=1:����"������"����
// ���ڲ�����	Null
// ���ߣ� ������
// ����������EH0218---COMMON.C���ӳ���
// *****************************************************************
void Beep(int i)
{
	(void)EA_vBeepMs(i);
}
void Beeperr(void)
{
	EA_vBeepMs(300);
	SleepMs(500);
	EA_vBeepMs(300);
}
/*****************************************************************
����ԭ�ͣ�CalConsumeTAC
 ��������������TAC�룬M1����������ר��
 ����������
 �������ƣ�	����/�����	����		����
				
 ��  ��  ֵ����
 ��      ��	������
 ��      �ڣ�2004-10-08
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
*****************************************************************/
void CalConsumeTAC(void)
{
	INT8U LOCAL_V_TYPE  i = 0;
	INT16U LOCAL_V_TYPE k = 0;
	INT8U  temp_snd_buf[50];
	INT8U  temp_rcv_buf[50];

	unsigned int LOCAL_V_TYPE  rcv_len = 0;

	for ( i=0; i<8; i++ )			  //������㲻�ɹ��������ѭ������8��
	{
		////////////////////////////////////////
		//TAC������ʼ��
		////////////////////////////////////////
		memcpy(temp_snd_buf, TAC_INIT, 5);		//TAC�����ʼ��
		memcpy(&temp_snd_buf[5], &CardInfo._CARD_NUMBER.card_number[0], 8);	// 2�ֽڳ��д��� + 2�ֽ���ҵ���� + 4�ֽڷ�����ˮ��
		memcpy(&temp_snd_buf[13], &CardInfo._CARD_NUMBER.detail_card_number.city_code[0], 2);			//��ҵ����
		memcpy(&temp_snd_buf[15], "\xFF\x00\x00\x00\x00\x00", 6);			//β
		k = CmdPsam(hSam1Handle, temp_snd_buf, 21, temp_rcv_buf, &rcv_len);		//TAC�����ʼ��
		if ( k != ok )
			continue;

		////////////////////////////////////////
		//TAC�����//test:00 00 00 50 06 11 80 00 00 00 05 00 00 00 00 20 05 02 24 11 06 00 80 00
		////////////////////////////////////////

		memcpy(temp_snd_buf, CAL_TAC_M1, 5);//ͷ

		temp_snd_buf[5] = (INT8U)(CardInfo.balance / 0x10000);//3�ֽڽ��׺����
		temp_snd_buf[6] = (INT8U)(CardInfo.balance % 0x10000 / 0x100);//3�ֽڽ��׺����
		temp_snd_buf[7] = (INT8U)(CardInfo.balance % 0x100);//3�ֽڽ��׺����		

		temp_snd_buf[8] = (INT8U)(CardInfo.fare / 0x10000);	//3�ֽڽ��׽��
		temp_snd_buf[9] = (INT8U)(CardInfo.fare % 0x10000 / 0x100);	//3�ֽڽ��׽��		
		temp_snd_buf[10]= (INT8U)(CardInfo.fare % 0x100);	//3�ֽڽ��׽��

				
		memcpy(temp_snd_buf+11, &DevStat.pos_number[0], 6);	//6�ֽ��ն˻����
		//memcpy(temp_snd_buf+11, "\x00\x00\x00\x00\x00\x00", 6);	//6�ֽ��ն˻����
		memcpy(temp_snd_buf+17, &CardInfo.card_in_time, 7);	//7�ֽڽ������ں�ʱ��
		memcpy(temp_snd_buf+24, &CardInfo._CARD_NUMBER.card_number[4], 4);//4�ֽڿ�Ƭ��ˮ��
		memcpy(temp_snd_buf+28, "\x80", 1);     

		k = CmdPsam(hSam1Handle, temp_snd_buf, 29, temp_rcv_buf, &rcv_len);		//TAC�����

		if ( k != ok )
			continue;								//������ɹ���ѭ������8��
		else
			break;									//ֱ������ɹ����˳�
	}

	if ( i >= 8 )
	{	//TAC�����ʧ�ܣ�ȫд0
		memset(&CardInfo.tac[0], 0x00, 4);
	}
	else
	{	//TAC�����ɹ�
		memcpy(&CardInfo.tac[0], &temp_rcv_buf[0], 4);		//����TAC��

	}
}
/*****************************************************************
����ԭ�ͣ�  query_rec_num(void)
 ������������ѯ��¼����
 ����������
 �������ƣ�	����/�����	����		����
				
 ��  ��  ֵ����
 ��      ��	��������
 ��      �ڣ�2004-10-08
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
*****************************************************************/
uint query_rec_num(void)
{
	uchar i;
	uchar ucOpenID;
	uint uiReadCnt;

	i = EA_ucPFOpen((uchar *)currecFileName, &ucOpenID);

	if ( i == EM_ffs_NOTEXIST )
	{
		 EA_ucPFCreateLoop((uchar *)currecFileName, sizeof(RECORD), 2000);  //2000��ѭ����¼;
		 EA_ucPFOpen((uchar *)currecFileName, &ucOpenID);
	}
	else if( i != EM_SUCCESS )
	{
		lcddisperr("�򿪼�¼�ļ�ʧ��!");
		return notok;
	}

	(void)EA_ucPFInfo(ucOpenID, EM_ffs_VALIDREC, &uiReadCnt);

	(void)EA_ucPFClose(ucOpenID);

	return uiReadCnt;
}
/******************************************************************************
 �������ƣ�CheckCardCredit
 ������������鿨�Ƿ���ϳ���
 ����������
 �������ƣ�	����/�����	����		����
 
 ��  ��  ֵ��ok(0)-�ɹ�
				 notok(0xFF)-ʧ��
				 0xF0-����ˢ��
				   
 ��      ��	������/������
 ��      �ڣ�2004-09-02
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
******************************************************************************/
INT8U CheckCardCredit(void)
{
	uint	  uiRetCnt;
	INT8U     p[60];
	uchar     ucOpenID = 0;
	uchar	  ucRet;
	uint   	  uiReadCnt;
	uint      count;
	RECORD    *prec = (RECORD *)p;
	uchar     i = 0;

	memset(p, 0x00, sizeof(RECORD));
	EA_vCls();
	i = EA_ucPFOpen( (uchar *)currecFileName, &ucOpenID);
	if ( i != EM_ffs_SUCCESS )
	{
		lcddisperr("�򿪼�¼�ļ�ʧ��");
		return notok;
	}
	// ��ȡ��Ч��Ϣ����
	(void)EA_ucPFInfo(ucOpenID, EM_ffs_VALIDREC, &uiReadCnt);
	if( uiReadCnt == 0)
	{
		lcddisperr("û�м�¼��Ϣ");
		(void)EA_ucPFClose(ucOpenID);
		return notok;//��ʾû�м�¼
	}
	count = uiReadCnt;//���һ��		
	ucRet = EA_ucPFReadRec(ucOpenID, count, 1, &uiRetCnt, p);
	if(ucRet != EM_SUCCESS)
	{
		(void)EA_ucPFClose(ucOpenID);
		lcddisperr((void *)"��ȡ������");
		return notok;
	}
	(void)EA_ucPFClose(ucOpenID);
	memcpy(prec, p, sizeof(RECORD));
	if(memcmp(&CardInfo._CARD_NUMBER.detail_card_number.serial_number[0], &prec->card_serial_number[4], 4) !=0)
	{
		lcddisperr((void *)"���ʿ��Ų�����");
		return notok;
	}

	if(prec->card_purchase_type != TT_CREDIT_MONEY)
	{
		printf_debug((void *)"ǰ�ʽ��׷ǳ�ֵ");
		return notok;
	}

	CardInfo.fare = (float)(prec->fare[0] * 65536 + prec->fare[1] * 256 + prec->fare[2]);
	return ok;
}

/******************************************************************************
 �������ƣ�CardQuery
 ��������������Ϣ��ѯ
 ����������
 �������ƣ�	����/�����	����		����
 
 ��  ��  ֵ��ok(0)-�ɹ�
				 notok(0xFF)-ʧ��
				   
 ��      ��	�����ҡ�������
 ��      �ڣ�2004-09-02
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
******************************************************************************/
INT8U CardQuery(void)
{
//    INT8U   LOCAL_V_TYPE  temp_buf[50];
//    ulong   ulSnr;
//    uint    LOCAL_V_TYPE  rcv_length;
//    INT8U   LOCAL_V_TYPE  k = 0;
//    uchar   ucResult;
//    uchar   i;
//    uint    uiSize;
//    ushort  usICType;
//    //ȡ�ÿ�����
//    ucResult = EA_ucMIFRequest (hMifsHandle, EM_mifs_IDLE, &usICType);
//    if (ucResult != EM_SUCCESS)
//    {
////  	lcddisperr("M1������ʧ��");
//        return notok;
//    }
//    ucResult = EA_ucMIFAntiColl (hMifsHandle, &ulSnr);
//    if (ucResult != EM_SUCCESS)
//    {
////  	lcddisperr("M1����ȡ����ʧ��");
//        return notok;
//    }
//
//    ucResult = EA_ucMIFSelect (hMifsHandle, ulSnr, &uiSize);
//    if (ucResult != EM_SUCCESS)
//    {
////  	lcddisperr("M1����ȡ������ʧ��");
//        return notok;
//    }
//
//    CardInfo.csn[3] = (INT8U)(ulSnr / 0x1000000);
//    CardInfo.csn[2] = (INT8U)((ulSnr % 0x1000000) / 0x10000);
//    CardInfo.csn[1] = (INT8U)((ulSnr % 0x10000) / 0x100);
//    CardInfo.csn[0] = (INT8U)(ulSnr % 0x100);
//    ////////////////////////////////////////
//    //����ʱ��
//    ////////////////////////////////////////
//    i = SaveCardInTime(&CardInfo);
//    if ( i != ok )
//    {
//        lcddisperr("����ʱ��ʧ��");
//        return notok;
//    }
//
//    ////////////////////////////////////////
//    //PSAM����λ
//    ////////////////////////////////////////
//    i = PsamRst();
//    if ( i != ok )
//    {
//        lcddisperr("PSAM����λʧ��");
//        return notok;
//    }
//    ////////////////////////////////////////
//    //PSAM��ѡ1001Ŀ¼
//    ////////////////////////////////////////
//    k = CmdPsam(hSam1Handle, (void *)SELECT_SAM_ADF1, sizeof(SELECT_SAM_ADF1),
//                temp_buf, &rcv_length);     //ѡ1001Ŀ¼
//    if ( k != ok )
//    {
//        lcddisperr("chose menu fail");
//        return notok;
//    }
//    k = M1CardQuery();
//    if( k != ok )
//    {
//        EA_vCls();
//        EA_vDisplay(3, "     ������ˢ��     ");
//        Beep(1000);
//        EA_vDelayMs(500);
//        Beep(1000);
//        return notok;
//    }
	return ok;
}

/******************************************************************************
 �������ƣ�SaveCardInTime
 �����������뿨ʱ����ʱ��
 ����������
 �������ƣ�	����/�����	����		����
 card_info	����		CARD_INFO *		׼��Ҫ����ʱ��ı�����ָ��
 
 ��  ��  ֵ��ok(0)-�ɹ�
				 notok(0xFF)-ʧ��
				   
 ��      ��	������
 ��      �ڣ�2004-09-02
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
******************************************************************************/
INT8U SaveCardInTime(CARD_INFO *card_info)
{
	INT8U LOCAL_V_TYPE i = 0;
	BUS_TIME LOCAL_V_TYPE time;

	i = Get_Time(&time);
	if ( i != ok )
		return notok;

	i = CheckTimeFormat(&time);								//���ʱ���ʽ
	if ( i != ok )
		return notok;

	memcpy(&card_info->card_in_time, &time, sizeof(BUS_TIME));
	return ok;
}

/******************************************************************************
 �������ƣ�QuitWorkProcess
 �����������°����
 ����������
 �������ƣ�	����/�����	����		����

 ��  ��  ֵ����

 ��      ��	������
 ��      �ڣ�2004-09-20
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
******************************************************************************/
void QuitWorkProcess(void)
{
	memset(&DevStat.cur_driver_number[0], 0x00, sizeof(DevStat.cur_driver_number));					//˾������0
	DevStat.driver_is_working = FALSE;
	//����˾��������
	WriteParam();
	CardInfo.card_purchase_type = TT_DRIVER_REST;
//	CardInfo.quit_work = TRUE;//�ú����������ʾ"˾�����°�"
}

/******************************************************************************
 �������ƣ�BeginWorkProcess
 �����������ϰ����
 ����������
 �������ƣ�	����/�����	����		����
 _buf	����	INT8U *	˾����ŵ�����

 ��  ��  ֵ����

 ��      ��	������
 ��      �ڣ�2004-09-20
 �޸���ʷ��
		����		�޸���		�޸�����
		2005.2.21		myron				in function
******************************************************************************/
void BeginWorkProcess(INT8U const * const driver_number_buf)
{

	//modified by myron for dandong flow 2005.2.21
	memcpy(&DevStat.cur_driver_number[0], driver_number_buf+2, 4);//����˾����
	memcpy(&CardInfo.driver_number[0], driver_number_buf+2, 4);//����˾����
	DevStat.driver_is_working = TRUE;
	//����˾��������
	WriteParam();

	CardInfo.card_purchase_type = TT_DRIVER_WORKING;
}
/******************************************************************************
 �������ƣ�CheckPeriodOfValidity
 ������������Ч���ж�
 ����������
 �������ƣ�	����/�����	����		����
 start			����		INT32U		��������
 current		����		INT32U		��ǰ����
 end			����		INT32U		��Ч��ֹ����
 
 ��  ��  ֵ��ok(0)-�ɹ�
				 notok(0xFF)-ʧ��(����)
				   
 ��      ��	������
 ��      �ڣ�2004-09-20
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
******************************************************************************/
INT8U CheckPeriodOfValidity(INT32U start, INT32U current, INT32U end)
{
	if ( (current >= start) && (current <= end) )
		return ok;
	else
		return notok;
//  return ok;
}

/*****************************************************************
����ԭ�ͣ�Sec6_Block_Is_Legal
 �����������ж�һ����(16���ֽ�)�������Ƿ���ȷ���ж����4�ֽ��Ƿ�Ϊ18E718E7
 ����������
 �������ƣ�	����/�����	����		����
 data_to_verify	����	const INT8U *		Ҫ��֤���������ʼָ��
				
 ��  ��  ֵ��ok(0)-��֤�ɹ�		
				notok(0xff)-��֤ʧ��
 ��      ��	������
 ��      �ڣ�2003-10-14
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
*****************************************************************/
INT8U Sec6_Block_Is_Legal(const INT8U * data_to_verify)
{
	//ToggleWD();

	//�ж����4�ֽ��Ƿ�Ϊ18E718E7
	if ( (*(data_to_verify + 12) == 0x18) 
		 && (*(data_to_verify + 13) == 0xE7) 
		 && (*(data_to_verify + 14) == 0x18) 
		 && (*(data_to_verify + 15) == 0xE7) )
	{
		return ok;
	}
	else
	{
		return notok;
	}
}

/*****************************************************************
����ԭ�ͣ�Purse_Block_Is_Right
 �����������ж�Ǯ����(16���ֽ�)�������Ƿ���ȷ
 ����������
 �������ƣ�	����/�����	����		����
 data_to_verify	����	const INT8U *		Ҫ��֤���������ʼָ��
				
 ��  ��  ֵ��ok(0)-��֤�ɹ�		notok(0xff)-��֤ʧ��
 ��      ��	��������������
 ��      �ڣ�2003-10-14
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
*****************************************************************/
INT8U Purse_Block_Is_Right(INT8U * data_to_verify)
{
//	ToggleWD();

	// 1. �жϺ�4���ֽ��Ƿ�255
	// 2. �ж�0-3�Ƿ����8-11
	// 3. �ж�0-3�Ƿ���4-7����
	if ( (*(data_to_verify + 12) + *(data_to_verify + 13) == 255)
		 && (*(data_to_verify + 12) == *(data_to_verify + 14))
		 && (*(data_to_verify + 13) == *(data_to_verify + 15)) )
	{

		if ( (*(data_to_verify + 0) == *(data_to_verify + 8))
			 && (*(data_to_verify + 1) == *(data_to_verify + 9))
			 && (*(data_to_verify + 2) == *(data_to_verify + 10))
			 && (*(data_to_verify + 3) == *(data_to_verify + 11)) )
		{

			if ( (*(data_to_verify + 0) == (~(*(data_to_verify + 4))    &0x000000ff))
				 && (*(data_to_verify + 1) == (~(*(data_to_verify + 5)) &0x000000ff))
				 && (*(data_to_verify + 2) == (~(*(data_to_verify + 6)) &0x000000ff))
				 && (*(data_to_verify + 3) == (~(*(data_to_verify + 7)) &0x000000ff)) )
			{
				return ok;
			}
			else
			{
				// step 3 fail
				return notok;
			}
		}
		else
		{
			// step 2 fail
			return notok;
		}
	}
	else
	{
		// step 1 fail
		return notok;
	}
}

/*****************************************************************
����ԭ�ͣ�BlockToCash
 �����������ж�Ǯ���������->INT32U������
 ����������
 �������ƣ�	����/�����	����		����
 buf			����		const INT8U *		Ҫ��֤���������ʼָ��
				
 ��  ��  ֵ��INT32U��Ǯ
 ��      ��	������
 ��      �ڣ�2004-09-29
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
*****************************************************************/
INT32U BlockToCash(const INT8U *buf)
{
	return( ((INT32U)buf[3] * 0x1000000 ) + ((INT32U)buf[2] * 0x10000 ) + ((INT32U)buf[1] * 0x100 ) + (INT32U)buf[0] );
}


/*****************************************************************
����ԭ�ͣ�CharToCash
 �����������ж�Ǯ���������->INT32U������
 ����������
 �������ƣ�	����/�����	����		����
 buf			����		const INT8U *		Ҫ��֤���������ʼָ��
				
 ��  ��  ֵ��INT32U��Ǯ
 ��      ��	������
 ��      �ڣ�2004-09-29
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
*****************************************************************/
INT32U CharToCash(const INT8U *buf)
{
//  return( ((INT32U)buf[0]<<16) + ((INT32U)buf[1]<<8) + (INT32U)buf[2] );

	return( ((INT32U)buf[0] * 0x10000 ) + ((INT32U)buf[1] * 0x100 ) + (INT32U)buf[2] );
}

/*****************************************************************
����ԭ�ͣ�BlockToCash
 �����������ж�Ǯ���������->INT32U������
 ����������
 �������ƣ�	����/�����	����		����
 cash		����		INT32U		Ҫ�۵Ŀ�
				
 ��  ��  ֵ��ָ��ۿ�block��ʽ��ָ��
 ��      ��	������
 ��      �ڣ�2004-09-29
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
*****************************************************************/
unsigned long  CashToBlock(INT32U cash)
{

	INT8U block[4];
	unsigned long temp;
	block[3] = (INT8U)(cash / 0x1000000);
	block[2] = (INT8U)(cash % 0x1000000 / 0x10000 );
	block[1] = (INT8U)(cash % 0x10000 / 0x100 );
	block[0] = (INT8U)(cash % 0x100) ;
	memcpy(&temp,block,4);
	return temp;
}


/*****************************************************************
����ԭ�ͣ�CalTAC
 ��������������TAC�룬M1����������ר��
 ����������
 �������ƣ�	����/�����	����		����
				
 ��  ��  ֵ����
 ��      ��	������
 ��      �ڣ�2004-10-08
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
*****************************************************************/
void CalTAC(void)
{
	
//    INT8U LOCAL_V_TYPE  i = 0;
//    INT16U LOCAL_V_TYPE k = 0;
//    INT8U  temp_snd_buf[50];
//    INT8U  temp_rcv_buf[50];
//
//    unsigned int   LOCAL_V_TYPE  rcv_len = 0;
//
//    ////////////////////////////////////////
//    //ISAM��ѡ1001Ŀ¼
//    ////////////////////////////////////////
////	PsamRst(,, INT8U * rcv_length)
//    k = PsamRst();//PSAM����λ
//    if ( k != ok )
//    {
////  	printf_debug("PSAM������");
//        return ;
//    }
//
//    k = CmdPsam(hSam1Handle, (void *)SELECT_SAM_ADF1, sizeof(SELECT_SAM_ADF1),
//                    temp_rcv_buf, &rcv_len);    //ѡ1001Ŀ¼
//    if ( k != ok )
//    {
////  	printf_debug("PSAM������");
//        return ;
//    }
//
//    for ( i=0; i<8; i++ )             //������㲻�ɹ��������ѭ������8��
//    {
//        ////////////////////////////////////////
//        //TAC������ʼ��
//        ////////////////////////////////////////
//        memcpy(temp_snd_buf, TAC_INIT, 5);      //TAC�����ʼ��
//
////		temp_snd_buf[5] = 0;								//8�ֽ��û���Ӧ�����к�
//
//        //modified by myron for dadong flow 2005.2.28
//        memcpy(&temp_snd_buf[5], &CardInfo._CARD_NUMBER.card_number[0], 8); // 2�ֽڳ��д��� + 2�ֽ���ҵ���� + 4�ֽڷ�����ˮ��
//        memcpy(&temp_snd_buf[13], &CardInfo._CARD_NUMBER.detail_card_number.city_code[0], 2);           //��ҵ����
//        memcpy(&temp_snd_buf[15], "\xFF\x00\x00\x00\x00\x00", 6);           //β
//        k = CmdPsam(hSam1Handle, temp_snd_buf, 21, temp_rcv_buf, &rcv_len);     //TAC�����ʼ��
////		k = CmdPsam(DevStat.cur_psam_channel, temp_snd_buf, 13, temp_rcv_buf, &rcv_len);		//TAC�����ʼ��
//
//        if ( k != ok )
//        {
//            lcddisperr("����TAC��ʧ��");
//            continue;
//        }
//        ////////////////////////////////////////
//        //TAC�����//test:00 00 00 50 06 11 80 00 00 00 05 00 00 00 00 20 05 02 24 11 06 00 80 00
//        ////////////////////////////////////////
//
//        memcpy(temp_snd_buf, CAL_TAC_M1, 5);//ͷ
//
////		if(CardInfo.card_purchase_type == TT_NORMAL_CONSUME_MONEY)//���ʽ���
////		{
////			temp_snd_buf[5] = (INT8U)(CardInfo.balance >>16);//3�ֽڽ��׺����
////			temp_snd_buf[6] = (INT8U)(CardInfo.balance >>8);//3�ֽڽ��׺����
////			temp_snd_buf[7] = (INT8U)(CardInfo.balance);//3�ֽڽ��׺����
//
////			temp_snd_buf[8] = (INT8U)(CardInfo.fare >>16);	//3�ֽڽ��׽��
////			temp_snd_buf[9] = (INT8U)(CardInfo.fare >>8);	//3�ֽڽ��׽��
////			temp_snd_buf[10]= (INT8U)(CardInfo.fare );	//3�ֽڽ��׽��
//
////			//if(DevStat.employee_a_free == TRUE && CardInfo.card_type == CARDT_FREE)//��������ѿ�Ʊ��
////			//{
////			//	temp_snd_buf[8] = (INT8U)(DevStat.free_price>>16);	//3�ֽڽ��׽��
////			//	temp_snd_buf[9] = (INT8U)(DevStat.free_price >>8);	//3�ֽڽ��׽��
////			//	temp_snd_buf[10]= (INT8U)(DevStat.free_price );	//3�ֽڽ��׽��
////			//}
////			memcpy(temp_snd_buf+11, &DevStat.pos_number[0], 6);	//6�ֽ��ն˻����
////			//memcpy(temp_snd_buf+11, "\x00\x00\x00\x00\x00\x00", 6);	//6�ֽ��ն˻����
////			memcpy(temp_snd_buf+17, &CardInfo.card_in_time, 7);	//7�ֽڽ������ں�ʱ��
////			memcpy(temp_snd_buf+24, &CardInfo._CARD_NUMBER.card_number[4], 4);//4�ֽڿ�Ƭ��ˮ��
////			memcpy(temp_snd_buf+28, "\x80", 1);
////			k = CmdPsam(DevStat.cur_psam_channel, temp_snd_buf, 29, temp_rcv_buf, &rcv_len);		//TAC�����
////		}
////		else//��ֵ����
////  	{
//            //80FA010018 + 3�ֽڽ��׺���� + 2�ֽ������������ + 3�ֽڽ��׽�� +
//            //1�ֽڽ������� + 6�ֽ��ն˻���� + 4�ֽڽ������� + 3�ֽڽ���ʱ�� + 8000
//            temp_snd_buf[5] = (INT8U)(CardInfo.balance / 0x10000);//3�ֽڽ��׺����
//            temp_snd_buf[6] = (INT8U)(CardInfo.balance % 0x10000 / 0x100);//3�ֽڽ��׺����
//            temp_snd_buf[7] = (INT8U)(CardInfo.balance % 0x100);//3�ֽڽ��׺����
//
//            memcpy(&temp_snd_buf[8], &CardInfo.purchase_serial_num[0], 2);//�������
//
//            temp_snd_buf[10] = (INT8U)(CardInfo.fare / 0x10000);    //3�ֽڽ��׽��
//            temp_snd_buf[11] = (INT8U)(CardInfo.fare % 0x10000 / 0x100);    //3�ֽڽ��׽��
//            temp_snd_buf[12]= (INT8U)(CardInfo.fare % 0x100);   //3�ֽڽ��׽��
//
//            temp_snd_buf[13] = CardInfo.card_purchase_type ;//��������
//
//            memcpy(temp_snd_buf+14, &DevStat.pos_number[0], 6); //6�ֽ��ն˻����
//
//            memcpy(temp_snd_buf+20, &CardInfo.card_in_time, 7); //7�ֽڽ������ں�ʱ��
//
//            memcpy(temp_snd_buf+27, "\x80\x00", 2);
//            k = CmdPsam(hSam1Handle, temp_snd_buf, 29, temp_rcv_buf, &rcv_len);     //TAC�����
////  	}
//
//        if ( k != ok )
//            continue;                               //������ɹ���ѭ������8��
//        else
//            break;                                  //ֱ������ɹ����˳�
//    }
//
//    if ( i >= 8 )
//    {   //TAC�����ʧ�ܣ�ȫд0
//        memset(&CardInfo.tac[0], 0x00, 4);
////  	printf_debug("TAC�����ʧ��");
//        return ;
//    }
//    else
//    {   //TAC�����ɹ�
//        memcpy(&CardInfo.tac[0], &temp_rcv_buf[0], 4);      //����TAC��
////  	printf_debug("TAC�����ɹ�");
////  	EA_vDisplay(4, "%02x%02x%02x%02x", CardInfo.tac[0],CardInfo.tac[1],CardInfo.tac[2],CardInfo.tac[3]);
////  	EA_vDelayMs(2000);
//        return;
//    }
	INT8U LOCAL_V_TYPE  i = 0;

	INT8U tac1[4];
	INT8U tac2[4];

	memset(tac1, 0x00, 4);
	memset(tac2,0x11,4);
	
	for(i=0; i<10; i++)
	{
		CalTAC2(tac1);
		CalTAC2(tac2);

		if (memcmp(tac1, tac2, 4) == 0)		//��2�飬һ��
		{
			memcpy(&CardInfo.tac[0], tac1, 4);
			return;
		}
	}

	//����10�飬����һ��
	memcpy(&CardInfo.tac[0], tac1, 4);
}

/*****************************************************************
����ԭ�ͣ�Get_Record
����������ȡָ����ַ�ļ�¼

����������
�������ƣ�	����/�����	����		����
-----------		-----------	------	   	-------
 rec_buf		���	INT8U *	ȡ�ؼ�¼����ڴ�
 start_position	����	INT16U		p0+2����NewPtr����DevStat.record_base_ptr
 rec_pointer	����	INT16U		Ҫȡ��¼��ָ�룬��DevStat.record_number

��  ��  ֵ����
��      ��	��������������
��      �ڣ�2004-10-10
�޸���ʷ��
����		�޸���		�޸�����
------		---------	-------------
*****************************************************************/
//void Get_Record(INT8U *l_rec_buf, INT16U start_position, INT16U rec_pointer)
//{
//    INT16U LOCAL_V_TYPE rec_ptr = 0;
//    INT16U LOCAL_V_TYPE page = 0;
//    INT8U LOCAL_V_TYPE ptr_in_page = 0;
//
//
//    rec_ptr = rec_pointer + start_position;
//    if ( rec_ptr >= (INT16U)MAX_REC_NUM )
//        rec_ptr = rec_ptr % (INT16U)MAX_REC_NUM;
//
//    page = rec_ptr / 3 + RECORDSTART;               //���Ҫ�洢�ļ�¼��ҳ��
//    ptr_in_page = rec_ptr % 3;                          //���ҳ������
//
//}

/*****************************************************************
 ����ԭ�ͣ�	SingleFlashBoot
 ������������FLASH����������һ��FLASHʧЧ���ɼ���һ��FLASH������
 ����������	��������		����/�����		����			����
 pri_slv		����		INT8U const		����FLASH���Ǵ�FLASH��������PRI �� ��SLV
 
 ����ֵ��	��
 
 ��  �ߣ�	�����ִ�/����
 ��  �ڣ�	2005-01-31
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
*****************************************************************/
uchar  Sector6_Process(void)
{

	INT8U LOCAL_V_TYPE  i = 0;
	INT8U LOCAL_V_TYPE  j = 0;
	INT8U LOCAL_V_TYPE  m = 0;
	INT8S LOCAL_V_TYPE g = 0;
	INT8U  block_buffer1[16] = {0};
	INT8U  block_buffer2[16] = {0};
	////////////////////////////////////////
	//��֤Sector 6 ��Կ
	////////////////////////////////////////
	i = Get_Sector_PermissionA(6, &KeySector[6][0]);
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
		//�������Ϸ��������������Ƿ�һ��
		g = memcmp(block_buffer1, block_buffer2, 16);
		if ( g == ok )
		{
			//����һ�£�ʲôҲ����
			;
		}
		else
		{
			//������һ�£���24��Ϊ��
			m = EA_ucMIFWrite(hSam1Handle,25,block_buffer1);
			if ( m != ok )
				return notok;
		}

	}
	else if ( i == ok )
	{
		//24���Ϸ���24->25
		m = EA_ucMIFWrite(hSam1Handle,25,block_buffer1);
		if ( m != ok )
			return notok;
	}
	else if ( j == ok )
	{
		//25���Ϸ���25->24
		m = EA_ucMIFWrite(hSam1Handle,24,block_buffer2);
		if ( m != ok )
			return notok;
		memcpy(block_buffer1, block_buffer2, 16);//�õ����ݴ���block_buffer1���±ߴ���������block_buffer1
	}
	else
	{
		//���������Ϸ����˳�
		return notok;
	}

	////////////////////////////////////////
	//�жϴ˿��Ƿ��Ѿ��ú�������־
	////////////////////////////////////////
	if ( block_buffer1[7] == 0x04 )
	{
		printf_debug((void *)"�Ƿ���");	
		return notok;
	}
	return ok;
}
/*****************************************************************
����ԭ�ͣ�DisplayPurchaseResult
������������ʾ���׽��(�ۿ�ɹ���)
����������
�������ƣ�	����/�����	����		����
cardinfo	����			const CARD_INFO*	������Ϻ�Ŀ�Ƭ��������Ϣ

��  ��  ֵ����
				
��      ��	������
��      �ڣ�2004-09-23
�޸���ʷ��
����		�޸���		�޸�����
2005.2.21		myron	                   in function 
*****************************************************************/
int DisplayPurchaseResult(const CARD_INFO *cardinfo)
{	
//	INT8U   temp[50];
	char    strbuf[50];
	float   fBuf=0;

	EA_vCls();
	EA_ucClrKeyBuf();
//  Beep(2000);
	sprintf(strbuf, "��  ��: ³C-%s", &CardInfo.bus_number[0]);
	EA_vDisp(1, 1, strbuf);

	if( DevStat.fill_mode == 0 )
	{
		sprintf(strbuf, "��  Ʒ: ��Ȼ��");
	}
	else if( DevStat.fill_mode == 1 )
	{
		sprintf(strbuf, "��  Ʒ: ����");
	}	
		else if( DevStat.fill_mode == 2 )
	{
		sprintf(strbuf, "��  Ʒ: ����");
	}	
	else
	{
		return notok;
	}
	EA_vDisp(2, 1, strbuf);
	fBuf = ((float)(cardinfo->fare))/100;
	sprintf(strbuf, "��  ��: %6.2f��", fBuf);
	EA_vDisp(3, 1, strbuf);
	EA_vDisp(4, 1, "����Ա: %02d", DevStat.cur_driver_number[0]);
	Beep(1000);

	if( EA_uiInkey(0) == EM_key_ENTER )
	{
		return ok;
	}
	return notok;
}

// *****************************************************************
// ���ܣ�����Ļ������ַ�
// ��ڲ�����	lb->��ʾ����ַ�
// ���ڲ�����	Null
// ���ߣ�
// ����������EH0218---COMMON.C���ӳ���
// *****************************************************************
char HextoBcd(char sour)
{
	return(((sour/10)<<4) |(sour%10));
}

// *****************************************************************
// ���ܣ�������Ƭ������Ϣ
// ��ڲ�����	lkeyvalue->����ֵ
// ���ڲ�����	Null
// ���ߣ�
// ����������EH0218---COMMON.C���ӳ���
// *****************************************************************
void GetCardType(INT8U  cardtype, INT8U * temp)
{

	switch ( cardtype )
	{
		case CARDT_NORMAL:
			strcpy((void *)temp, (void *)"  ��ͨ��");
			break;

		case CARDT_OLDMAN:
			strcpy((void *)temp, (void *)"  ���꿨");
			break;

		case CARDT_FREE:
			strcpy((void *)temp, (void *)"  ��ѿ�");
			break;

		case CARDT_MEMORY:
			strcpy((void *)temp, (void *)"  ���");
			break;

		case CARDT_DRIVER:
			strcpy((void *)temp, (void *)"  Ա����");
			break;
		case CARDT_STUDENT:
			strcpy((void *)temp, (void *)"  ѧ����");
			break;

		case CARDT_GATHER:
			strcpy((void *)temp, (void *)"  �ɼ���");
			break;

		case CARDT_SET:
			strcpy((void *)temp, (void *)"  ���ÿ�");
			break;

		case CARDT_SIGNED:
			strcpy((void *)temp, (void *)"  ǩ�㿨");
			break;

		case CARDT_CHECKED:
			strcpy((void *)temp, (void *)"   ��⿨");
			break;

		case CARDT_INSPECTOR:
			strcpy((void *)temp, (void *)"  ���鿨");
			break;

		case 0xF0:
			strcpy((void *)temp, (void *)"    ׷��");
			break;

		default :
			strcpy((void *)temp, (void *)" �� �� ��");
			break;
	}
}

/******************************************************************************
 �������ƣ�CPUCardQuery
 ����������CPU����Ϣ��ѯ
 ����������
 �������ƣ�	����/�����	����		����
 
 ��  ��  ֵ��ok(0)-�ɹ�
				 notok(0xFF)-ʧ��
				   
 ��      ��	������/������
 ��      �ڣ�2012-12-12
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
******************************************************************************/
INT8U QueryNormal(void)
{
	
	INT8U   LOCAL_V_TYPE  i = 0;
	INT8U   LOCAL_V_TYPE  j = 0;
	INT8U   LOCAL_V_TYPE    block9[16];
	INT8U   LOCAL_V_TYPE    block10[16];
	INT8U   LOCAL_V_TYPE  temp_buf[50];
	float   fbuf = 0.0;
	INT8U   temp[20];
	INT8U   strbuf[50];
	INT32U  LOCAL_V_TYPE  temp_int32u = 0;
	INT16U  key = EM_key_NOHIT;

	////////////////////////////////////////
	//��֤Sector 2 ��Կ
	////////////////////////////////////////
	i = Get_Sector_PermissionA(2, &KeySector[2][0]);
	if ( i != ok )
	{
		return notok;
	}

	i = EA_ucMIFRead(hMifsHandle, 9, block9);
	if ( i != ok )
	{
		return notok;
	}

	i = EA_ucMIFRead(hMifsHandle, 10, block10);
	if ( i != ok )
		return notok;

	i = Purse_Block_Is_Right(block9);
	j = Purse_Block_Is_Right(block10);

	if ( i == ok )
	{
		temp_int32u = BlockToCash(block9);			//9���Ϸ�
		fbuf = (float)(temp_int32u) / 100.0;
	}

	else if ( j == ok )
	{
		temp_int32u = BlockToCash(block10);			 //10���Ϸ�
		fbuf = (float)(temp_int32u) / 100.0;
	}

	else
	{
		return notok;		//���������Ϸ����˳�
	}

	//��ʾ��Ƭ��Ϣ
	EA_vCls();
	GetCardType(CardInfo.card_type, temp);

	htoa(temp_buf, &CardInfo._CARD_NUMBER.detail_card_number.serial_number[0], 4);
	strcpy((void *)strbuf, "����:   ");
	strcat((void *)strbuf, (void *)temp_buf);
	EA_vDisp(1, 1, (void *)strbuf);

	sprintf((void *)strbuf, "����: %s", temp);
	EA_vDisp(2, 1, (void *)strbuf);

	sprintf((void *)strbuf, "���: %8.2fԪ", fbuf);
	EA_vDisp(3, 1, (void *)strbuf);

	EA_vDisp(4, 1, "    ��������˳�    ");
	Beep(1000);
	key = EA_uiInkey(5);
	return ok;
}

/******************************************************************************
 �������ƣ�CardFlush
 �������������Ϳ���������
 ����������
 �������ƣ�����/�����	����		����
 
 ��  ��  ֵ��    ok(0)-�ɹ�
				 notok(0xFF)-ʧ��
				 0xFE-û�ҵ���
				 0xF0-����ˢ��
				   
 ��      ��	��������
 ��      �ڣ�2014-01-30
 �޸���ʷ��
		����		�޸���		�޸�����
******************************************************************************/
INT8U CardFlush(void)
{
	INT8U  ret = 0;
	INT8U  cardtype = 0;

#ifndef Debug_M1card
	ret = CardInit();

	if( ret != ok )
	{
		EA_ucMIFHalt(hMifsHandle);	//�ҿ�
		return notok;
	}
#endif
    //lcddisperr("��źÿ�Ƭ���ƶ�");

	ret = M1CardProcess();

	EA_ucMIFHalt(hMifsHandle);	//�ҿ�

	return ret;//ѡ���˳�
}


/******************************************************************************
 �������ƣ�CardSale
 �����������ۿ���������
 ����������
 �������ƣ�����/�����	����		����
 
 ��  ��  ֵ��    ok(0)-�ɹ�
				 notok(0xFF)-ʧ��
				 0xFE-û�ҵ���
				 0xF0-����ˢ��
				   
 ��      ��	���ڸ���
 ��      �ڣ�2007-01-30
 �޸���ʷ��
		����		�޸���		�޸�����
******************************************************************************/
INT8U CardSale(void)
{
//    INT8U   LOCAL_V_TYPE  i = 0;
//    INT8U   LOCAL_V_TYPE  j = 0;
//    INT32U  LOCAL_V_TYPE  temp_int32u = 0;
//    INT8S   LOCAL_V_TYPE  h = 0;
////	ulong	ulSnr;
////	uchar   ucResult;
////	uint 	uiSize;
////	ushort  usICType;
//
//    INT8U   LOCAL_V_TYPE  block_buffer1[16] = {0};          //M1��һ��block4�����ݻ���
//    INT8U   LOCAL_V_TYPE  block_buffer2[16] = {0};          //M1��һ��block5�����ݻ���
//    INT8U   LOCAL_V_TYPE  block_buffer3[16] = {0};          //M1��һ��block6�����ݻ���
//    ////////////////////////////////////////
//    //��Ƭ��ʼ��
//    ////////////////////////////////////////
//    if( CardInit() != ok )
//    {
//        //rrCardInfo.sale_error_occured = TRUE;
//        EA_ucMIFHalt(hMifsHandle);  //�ҿ�
//        return notok;
//    }
//
////	ucResult = EA_ucMIFRequest (hMifsHandle, EM_mifs_IDLE, &usICType);
////	if (ucResult != EM_SUCCESS)
////	{
//////  	lcddisperr("M1����λʧ��");
////		return notok;
////	}
////	ucResult = EA_ucMIFAntiColl (hMifsHandle, &ulSnr);
////	if (ucResult != EM_SUCCESS)
////	{
//////  	lcddisperr("M1����ȡ����ʧ��");
////		return notok;
////	}
////	CardInfo.csn[3] = (INT8U)(ulSnr / 0x1000000);
////	CardInfo.csn[2] = (INT8U)((ulSnr % 0x1000000) / 0x10000);
////	CardInfo.csn[1] = (INT8U)((ulSnr % 0x10000) / 0x100);
////	CardInfo.csn[0] = (INT8U)(ulSnr % 0x100);
////
////	ucResult = EA_ucMIFSelect( hMifsHandle, ulSnr, &uiSize );
////	if (ucResult != EM_SUCCESS)
////	{
//////  	lcddisperr("M1��ѡ��ʧ��");
////		return notok;
////	}
//    ////////////////////////////////////////
//    //��Sector 1 ��4 ����Ϣ
//    ////////////////////////////////////////
//    memcpy(&KeySector[1][0], &CardInfo.csn[0], 4);
//    KeySector[1][4] = ~KeySector[1][0];
//    KeySector[1][5] = ~KeySector[1][1];
//
//    i = Get_Sector_PermissionA(1, &KeySector[1][0]);
//    if ( i != ok )
//        return notok;
//
//    i = EA_ucMIFRead(hMifsHandle, 4, block_buffer1);
//    if ( i != ok )
//        return notok;
//
//    i = EA_ucMIFRead(hMifsHandle, 5, block_buffer2);
//    if ( i != ok )
//        return notok;
//
//    i = EA_ucMIFRead(hMifsHandle, 6, block_buffer3);
//    if ( i != ok )
//        return notok;
//
//    if ( block_buffer1[12] == 0x01 )      //�ж����ñ�־
//    {
//        if((ErrCardInfo.sale_error_occured == FALSE) && (ErrCardInfo.error_occured == FALSE))
//        {
//            //�����ô���
//            EA_vCls();
//            EA_vDisplay(2,"     ��Ƭ������     ");
//            EA_uiInkeyMs(500);
//            Beep(1000);
//            (void)EA_ucMIFHalt (hMifsHandle);
//            return ok;
//        }
//    }
//
//    //���濨��Ӧ��ϵ�к�
//    memcpy(&CardInfo._CARD_NUMBER.detail_card_number.city_code[0], block_buffer1, 2);   //���д���
//    memcpy(&CardInfo._CARD_NUMBER.detail_card_number.calling_code[0], block_buffer1+2, 2);  //���д���
//    memcpy(&CardInfo._CARD_NUMBER.detail_card_number.serial_number[0], &block_buffer1[4], 4);//������ˮ��
//    memcpy(&CardInfo.authentication_code[0], &block_buffer1[8], 4);//���濨��֤��
//    CardInfo.card_type = block_buffer1[13]; //������
//
////�����ֵ������Ա��Ϣ
//    if((ErrCardInfo.error_occured == FALSE)&&(ErrCardInfo.sale_error_occured == FALSE))
//    {
//        i = SaveCardInTime(&CardInfo);//����ʱ��
//        if ( i != ok )
//        {
//            lcddisperr("ʱ�Ӵ����޷�����");
//            (void)EA_ucMIFHalt (hMifsHandle);
//            return notok;
//        }
//        if( GetTradeInfo() != ok )
//        {
//            lcddisperr("�����½���");
//            (void)EA_ucMIFHalt (hMifsHandle);
//            return ok;
//        }
//
/////////////////////////////////
////�жϲ���Ա���ڽ���Ƿ񹻱����ۿ���ֵ���
//        if( DevStat.oper_card_amt < ( CardInfo.fare + DevStat.deposit_amt[0]*256 + DevStat.deposit_amt[1] ))
//        {
//            lcddisperr("�󿨽���");
//            (void)EA_ucMIFHalt (hMifsHandle);
//            return ok;
//        }
//
//        /*
//        //�۳�����Ա���ڽ��
//        Cpucard_Init(ICC_SOCK_MAIN);
//        temp_int32u = (INT32U)(CardInfo.deposit_amt[0]<<8) | (INT32U)(CardInfo.deposit_amt[1]);
//        if(OperTradeProcess(0, temp_int32u) != 0)
//        {
//            Cpucard_power_off();
//            return notok;
//        }
//        Cpucard_power_off();
//        */
//    ////////////////////////////////////////
//    //��Ƭ��Ч������
//    //01    ��ͨ��
//    //02    ����(���)��
//    //03    ���ٿ�
//    //05    ���
//    //06    Ա����
//    //07    ѧ����
//    //08    ְ����
//    ////////////////////////////////////////
//    //  if(CardInfo.card_type==CARDT_STUDENT || CardInfo.card_type==CARDT_OLDMAN || CardInfo.card_type==CARDT_FREE)//ѧ������Ҫ����ʱ��
//    //  {
//    //      if(SetCardUseDT() != ok)
//    //          return notok;
//    //  }
//
//        if(CardInfo.card_type != CARDT_NORMAL )//ֻ����ͨ�������ۿ�
//        {
//            lcddisperr("����ͨ�������ۿ�");
//            (void)EA_ucMIFHalt (hMifsHandle);
//            return ok;
//        }
//    }
//
//
//////////////////////////////////////////
////��������
//////////////////////////////////////////
//    if (( ErrCardInfo.error_occured == TRUE )||(ErrCardInfo.sale_error_occured == TRUE))
//    {//�������룬PSAM�����ܸ�λ
//        h = memcmp(&CardInfo.csn[0], &ErrCardInfo.csn[0], 4);//����Ӳ�����űȽ�
//        if ( h != ok )
//        {
//            lcddisperr("��ˢ����");
//            (void)Beep(1000);
//            (void)EA_ucMIFHalt (hMifsHandle);
//            return notok;//�����׳������صĿ�Ƭ���ǳ���
//        }
//    }
//    else
//    {
//        i = PsamRst();//PSAM����λ
//        if ( i != ok )
//        {
//            lcddisperr("PSAM������");
//            (void)EA_ucMIFHalt (hMifsHandle);
//            return notok;
//        }
//    }
//
//
//////////////////////////////////////////
////��̨������Կ
//////////////////////////////////////////
//
//    if ((ErrCardInfo.error_occured == FALSE)&&(ErrCardInfo.sale_error_occured == FALSE))
//    {
//        DevStat.key_query_flag=TRUE;
//        i = GPRS_CAL_KEY();
//        DevStat.key_query_flag=FALSE;
//    }
//
//    if( i != ok )//������ȡ�������������
//    {
////  	lcddisperr("����,����������ϵ");
////  	EA_ucMIFHalt();	//�ҿ�
//        (void)EA_ucMIFHalt (hMifsHandle);
//        return ok;
//    }
//
//////////////////////////////////////////
////��ʾ����
//////////////////////////////////////////
//    EA_vCls();
//    EA_vDisplay(1,"      ��Ƭ�ۿ�      ");
//    EA_vDisplay(2,"  ��źÿ�Ƭ���ƶ�  ");
//    EA_vDisplay(3,"  ������,��ȴ�...  ");
//    EA_uiInkeyMs(500);
//
//    CardInfo.card_purchase_type = TT_CARD_SALE;
//
//    if (ErrCardInfo.sale_error_occured == FALSE)
//    {
//        i = M1_Card_Credit_Process();
//        if(i == 0xF0)
//        {
//            (void)EA_ucMIFHalt (hMifsHandle);
//            return 0xF0;
//        }
//
//        if(i != ok)
//        {
//            (void)EA_ucMIFHalt (hMifsHandle);
//            return notok;
//        }
//    }
//
////2008-03-14���ۿ���Ϣ�ŵ���ֵ����ǰ
////��д1��040506������
//    ////////////////////////////////////////
//    //��֤Sector 1 ��Կ
//    ////////////////////////////////////////
////  if( CardInit() != ok )
////  {
////  	ErrCardInfo.sale_error_occured = TRUE;
////  	return 0xF0;
////  }
//
//    i = Get_Sector_PermissionB(1, &KeySector[1][0]);
//    if ( i != ok )
//    {
//        ErrCardInfo.sale_error_occured = TRUE;
//        return 0xF0;
//    }
//
//    //��д4��5��6����Ϣ
//    block_buffer1[12] = 0x01;//���ñ�־
//    memcpy(&block_buffer1[14], &DevStat.deposit_amt[0],2);
//
//    memcpy(&block_buffer2[0], &CardInfo.card_begin_dt[0], 4);
//    memcpy(&block_buffer2[4], &CardInfo.card_useful_dt[0],4);
//    memcpy(&block_buffer2[13], &DevStat.equ_id[0], 3);
//
//    memcpy(&block_buffer3[0], &CardInfo.card_add_dt[0], 4);
//    block_buffer3[7] = (CardInfo.fare / 0x10000);
//    block_buffer3[8] = (CardInfo.fare % 0x10000 / 0x100);
//    block_buffer3[9] = (CardInfo.fare % 0x100);
//
//    block_buffer3[10] = block_buffer3[7];//��ֵ���
//    block_buffer3[11] = block_buffer3[8];
//    block_buffer3[12] = block_buffer3[9];
//
//    memcpy(&block_buffer3[13], &DevStat.cur_driver_number[1], 3);//����Ա��        //by liujihua
//
//
//    //�۳�����Ա���ڽ��Ѻ��
//
//    //ȡ������Ա����Ѻ��
//
//    Cpucard_Init();
//
//    temp_int32u = DevStat.deposit_amt[0] * 0x100 + DevStat.deposit_amt[1];
//    if(OperTradeProcess(0, temp_int32u) != ok)
//    {
//        Cpucard_power_off();
//        ErrCardInfo.sale_error_occured = TRUE;
//        lcddisperr("�󿨹��ϲ��ܲ���");
//        EA_ucMIFHalt(hMifsHandle);  //�ҿ�
//        return 0xF0;
//    }
//    Cpucard_power_off();
//
//    i = EA_ucMIFWrite(hMifsHandle, 6, block_buffer3);
//    if ( i != ok )
//    {
//        //ErrCardInfo.error_occured = TRUE;
//        ErrCardInfo.sale_error_occured = TRUE;
//        return 0xF0;
//    }
//
//    i = EA_ucMIFWrite(hMifsHandle, 5, block_buffer2);
//    if ( i != ok )
//    {
//        //ErrCardInfo.error_occured = TRUE;
//        ErrCardInfo.sale_error_occured = TRUE;
//        return 0xF0;
//    }
//
//    i = EA_ucMIFWrite(hMifsHandle, 4, block_buffer1);
//    if ( i != ok )
//    {
//       // ErrCardInfo.error_occured = TRUE;
//        ErrCardInfo.sale_error_occured = TRUE;
//        return 0xF0;
//    }
//
////�����ۿ���¼
//    CardInfo.card_purchase_type = TT_CARD_SALE;
//    (void)StoreRecord(&CardInfo);           //�����ۿ���¼
//
////�����ֵ��¼
//    CardInfo.card_purchase_type = TT_CREDIT_MONEY;
//    CalTAC();
//    //EA_ucMIFHalt();   //�ҿ�
//    EA_ucMIFHalt(hMifsHandle);
//    (void)StoreRecord(&CardInfo);           //�����ֵ��¼
//
////��ʾ���
//    CardInfo.card_purchase_type = TT_CARD_SALE;
//    DisplayPurchaseResult(&CardInfo);   //��ʾ���׽��
//
//////�����ս�Ѻ��
//    DevStat.binhand.sale_cnt++;
//    DevStat.binhand.deposit_amt += (DevStat.deposit_amt[0] *256 + DevStat.deposit_amt[1])/100;
//    DevStat.last_trade_type = TT_CARD_SALE;
//    DevStat.gray_flag = FALSE;
//    WriteParam();
//
//    if(DevStat.mode == 0x00)
//    {
////  	EA_ucSetStopFlag(EM_DISABLE_STOP);
//        print_sale_record();
////  	EA_ucSetStopFlag(EM_ENABLE_STOP);
//    }
//
////���������ϴ�
//    //�����ϴ�
//    i = GPRS_SEND_RECORD();
//    if(i == ok)
//    {
//        DevStat.have_data_not_send_flag = FALSE;
//    }
//
////�����ֵ��Կ
//    for(j=1;j<7;j++)
//    memcpy(DevStat.KEYVALUE[j-1], KeySector[j], 6);
//    WriteParam();
//
//
//    //�ָ�״̬
//    memset(&CardInfo, 0x00, sizeof(CardInfo));      //�忨Ƭ������Ϣ
//    memset(&ErrCardInfo, 0x00, sizeof(ErrCardInfo));    //�������Ϣ
//    //DispPurchaseResultDelay = DISP_DELAY;
//    DevStat.purchase_again_req = FALSE;                 //������ˢ��������־
//    DevStat.voice_purchase_again_freq = FALSE;
//    (void)EA_ucMIFHalt (hMifsHandle);
//    return i;
return ok;
}
//
///******************************************************************************
// �������ƣ�CardCreditAmt
// ������������ֵ��������
// ����������
// �������ƣ�����/�����  ����        ����
//
// ��  ��  ֵ��    ok(0)-�ɹ�
//                 notok(0xFF)-ʧ��
//                 0xFE-û�ҵ���
//                 0xF0-����ˢ��
//
// ��      �� ���ڸ���
// ��      �ڣ�2007-01-30
// �޸���ʷ��
//        ����        �޸���      �޸�����
//******************************************************************************/
//INT8U CardCreditAmt(void)
//{
//    INT8U   LOCAL_V_TYPE  i = 0;
//    INT8U   LOCAL_V_TYPE  j = 0;
//    INT32U  LOCAL_V_TYPE  start_using_date = 0;             //��������
//    INT32U  LOCAL_V_TYPE  end_using_date = 0;               //��Ч��������
//    INT32U  LOCAL_V_TYPE  cur_date = 0;                         //��ǰʱ��
//    INT8S   LOCAL_V_TYPE  h = 0;
//    INT8U   LOCAL_V_TYPE  block_buffer1[16] = {0};          //M1��һ��block4�����ݻ���
//    INT8U   LOCAL_V_TYPE  block_buffer2[16] = {0};          //M1��һ��block5�����ݻ���
//    INT8U   LOCAL_V_TYPE  block_buffer3[16] = {0};          //M1��һ��block6�����ݻ���
//    INT16U  key;
//    INT8U   buffer[16] = {0};           //M1��һ��block4�����ݻ���
//////////////////////////////////////////
////��Ƭ��ʼ��
//////////////////////////////////////////
//    if( CardInit() != ok )
//    {
//        EA_ucMIFHalt(hMifsHandle);  //�ҿ�
//        return notok;
//    }
//
////�����ֵ������Ա��Ϣ
//    if( ErrCardInfo.error_occured == FALSE )
//    {
//        if( GetTradeAmt() != ok )
//        {
//            EA_ucMIFHalt(hMifsHandle);
//            return ok;
//        }
//
/////////////////////////////////
////�жϲ���Ա���ڽ���Ƿ񹻱����ۿ���ֵ���
//        if( DevStat.oper_card_amt < CardInfo.fare )
//        {
//            lcddisperr("�󿨽���");
//            EA_ucMIFHalt(hMifsHandle);
//            return ok;
//        }
///*
//        //�۳�����Ա���ڽ��
//        Cpucard_Init(ICC_SOCK_MAIN);
//        if(OperTradeProcess(0, CardInfo.fare) != 0)
//        {
//            Cpucard_power_off();
//            EA_vCls();
//            //printf_debug("1");
//            MC530Off();
//            InitMC530(TYPE_A_ISO14443);
//            return notok;
//        }
//        Cpucard_power_off();
//*/
//    }
//
////  MC530Off();
////  if( InitMC530(TYPE_A_ISO14443) != MI_OK)
////  	return notok;
//
////  if(CardInit() != ok)
////  	return notok;
//////////////////////////////////////////
////��������
//////////////////////////////////////////
//    if ( ErrCardInfo.error_occured == TRUE )
//    {//�������룬PSAM�����ܸ�λ
//        h = memcmp(&CardInfo.csn[0], &ErrCardInfo.csn[0], 4);//����Ӳ�����űȽ�
//        if ( h != ok )
//        {
//            lcddisperr("��ˢ����");
//            EA_ucMIFHalt(hMifsHandle);  //�ҿ�
//            Beep(1000);
//            return notok;//�����׳������صĿ�Ƭ���ǳ���
//        }
//    }
//    else
//    {
//        i = SaveCardInTime(&CardInfo);//����ʱ��
//        if ( i != ok )
//            return notok;
///*		//�ж��Ƿ��ǵ��չ���
//        if(memcmp(DevStat.binhand.end_date, (INT8U *)&CardInfo.card_in_time, 4) != 0)//��Ҫ���ս�
//        {
//            printf_debug("�������ս�");
//            return ok;
//        }
//*/
//    }
//
//
//    i = PsamRst();//PSAM����λ
//    if ( i != ok )
//    {
//        lcddisperr("PSAM������");
//        //j =EA_ucMIFHalt();    //�ҿ�
//        EA_ucMIFHalt(hMifsHandle);
//        return notok;
//    }
//
//////////////////////////////////////////
////��Sector 1 ��4 ����Ϣ
//////////////////////////////////////////
//    memcpy(&KeySector[1][0], &CardInfo.csn[0], 4);
//    KeySector[1][4] = ~KeySector[1][0];
//    KeySector[1][5] = ~KeySector[1][1];
//
//    i = Get_Sector_PermissionA(1, &KeySector[1][0]);
//    if ( i != ok )
//        return notok;
//
//    i = EA_ucMIFRead(hMifsHandle, 4, block_buffer1);
//    if ( i != ok )
//        return notok;
//
//    i = EA_ucMIFRead(hMifsHandle, 5, block_buffer2);
//    if ( i != ok )
//        return notok;
//
//    i = EA_ucMIFRead(hMifsHandle, 6, block_buffer3);
//    if ( i != ok )
//        return notok;
//
//    if ( block_buffer1[12] != 0x01 )      //�ж����ñ�־
//    {
//        //�����ô���
//        lcddisperr("��Ƭδ����");
//        Beep(1000);
//        EA_uiInkey(2);
//        EA_ucMIFHalt(hMifsHandle);
//        return ok;
//    }
//
//    memcpy(&start_using_date, block_buffer2, 4);//��������
//    memcpy(&end_using_date, block_buffer2+4, 4);//��Ч��������
////  memcpy(&buffer, block_buffer2, 4);//��������
////  over_turn(4, buffer);
////  start_using_date = unpackInt(buffer);
////
////  memcpy(&buffer, block_buffer2+4, 4);//��Ч��������
////  over_turn(4, buffer);
////  end_using_date = unpackInt(buffer);
//
//    #ifdef DEBUG
//        lcddisperr("%04d", start_using_date);
//        lcddisperr("%04d", end_using_date);
//    #endif
//
//    //���濨��Ӧ��ϵ�к�
//    memcpy(&CardInfo._CARD_NUMBER.detail_card_number.city_code[0], block_buffer1, 2);   //���д���
//    memcpy(&CardInfo._CARD_NUMBER.detail_card_number.calling_code[0], block_buffer1+2, 2);  //���д���
//    memcpy(&CardInfo._CARD_NUMBER.detail_card_number.serial_number[0], &block_buffer1[4], 4);//������ˮ��
//    memcpy(&CardInfo.authentication_code[0], &block_buffer1[8], 4);//���濨��֤��
//    CardInfo.card_type = block_buffer1[13]; //������
//
///*
//    if ( DevStat.record_number >= (MAX_REC_NUM - 30) )
//    {
//        (void)EA_vCls();
//        DispStr_CE(0, 3, "��¼������!", DISP_POSITION);
//        Beep(1000);
//        (void)delay_and_wait_key(3, 0, 0);
//        ////////////////////////////////////////
//        //M1���ҿ�
//        ////////////////////////////////////////
//        j =EA_ucMIFHalt();  //�ҿ�
//        return ok;                                      //��������
//    }
//*/
//    if(CardInfo.card_type > 0x10)
//    {
//        lcddisperr((void *)"�������ܳ�ֵ!");
//        EA_ucMIFHalt(hMifsHandle);
//        return notok;
//    }
//
//    if(CardInfo.card_type == CARDT_FREE)
//    {
//        lcddisperr((void *)"��ѿ����ܳ�ֵ!");
//        EA_ucMIFHalt(hMifsHandle);
//        return notok;
//    }
//
//    ////////////////////////////////////////
//    //�ж���Ч��
//    ////////////////////////////////////////
//
////  memcpy(&cur_date, &CardInfo.card_in_time, 4);//ȡ��ǰ��avoid warning
//    memcpy(&buffer, &CardInfo.card_in_time, 4);//ȡ��ǰ��avoid warning
//    over_turn(4, buffer);
//    cur_date = unpackInt(buffer);
//    i = CheckPeriodOfValidity(start_using_date, cur_date, end_using_date);  //��Ч���ж���������������
//    if ( i != ok )
//    {//���ڴ�����������
//        EA_vCls();
//        EA_vDisp(4, 1, "  �˿�����,������?  ");
//        key = EA_uiInkey(0);
//        switch(key)
//        {
//           case EM_key_ENTER:
//               break;
//           case EM_key_CANCEL:
//               EA_ucMIFHalt(hMifsHandle);
//               return notok;
//        }
//
//        //if(select_affirm(4, (void *)"�˿�����,������?") != 0)
////  	{
////  		j = EA_ucMIFHalt();	//�ҿ�
////  		EA_ucMIFHalt(hMifsHandle);
////  		return notok;
////  	}
//    }
///*
//
//    switch ( CardInfo.card_type )
//    {
//        case CARDT_NORMAL:  //��ͨ��
//        case CARDT_MEMORY:  //���
//            //����Ч���ж�
//            memcpy(&cur_date, &CardInfo.card_in_time, 4);//ȡ��ǰ��avoid warning
//            i = CheckPeriodOfValidity(start_using_date, cur_date, end_using_date);  //��Ч���ж�
//            if ( i != ok )
//            {//���ڴ�����������
//                printf_debug("����Ч��");
//                j = EA_ucMIFHalt(); //�ҿ�
//                return notok;
//            }
//            break;  //v0.1.1 �������Ч���ж�
//
//        default:
//            //��������ͳһ����Ч���жϣ���ѿ��������Ҳ��ʾ����
//            memcpy(&cur_date, &CardInfo.card_in_time, 4);//ȡ��ǰ��avoid warning
//            i = CheckPeriodOfValidity(start_using_date, cur_date, end_using_date);  //��Ч���ж�
//            if ( i != ok )
//            {//���ڴ�����������
//                printf_debug("����Ч��");
//                j = EA_ucMIFHalt(); //�ҿ�
//                return notok;
//            }
//            break;  //v0.1.1 �������Ч���ж�
//    }
//*/
//////////////////////////////////////////
////��̨������Կ
//////////////////////////////////////////
////  MC530Off();
//
//    DevStat.key_query_flag = TRUE;
////	i = GPRSProcess();
//    i = GPRS_CAL_KEY();
//    DevStat.key_query_flag = FALSE;
//
//    if(i != ok)//������ȡ�������������
//    {
//        EA_ucMIFHalt(hMifsHandle);
//        return ok;
//    }
//
//
//////////////////////////////////////////
////��ʾ����
//////////////////////////////////////////
//    EA_vCls();
//    EA_vDisplay(1,"  ��Ƭ��ֵ�������  ");
//    EA_vDisplay(2,"  ��źÿ�Ƭ���ƶ�  ");
//    EA_vDisplay(3,"  ������,��ȴ�...  ");
//    EA_uiInkeyMs(500);
///*
//////////////////////////////////////////
////ISAM��������Կ
//////////////////////////////////////////
//
//    if(DevStat.samcompcode==1)//�ݵ¿�Ƭ
//    {
//        ////////////////////////////////////////
//        //ISAM��ѡ1001Ŀ¼
//        ////////////////////////////////////////
//        k = CmdPsam(DevStat.cur_psam_channel, (void *)SELECT_JDSAM_ADF1, sizeof(SELECT_JDSAM_ADF1),
//                    temp_buf, &rcv_length);     //ѡ1001Ŀ¼
//        if ( k != 0x9000 )
//            return notok;
//
//        ////////////////////////////////////////
//        //ISAM���������Կ
//        ////////////////////////////////////////
//
//        //80FC010111'+ astr_readcard.city_code + astr_readcard.card_hardno + right(astr_readcard.card_no, 4) + astr_readcard.card_auth + '0102030405'
//
//        memcpy(temp_snd_buf, GET_M1_KEY, sizeof(GET_M1_KEY));//ͷ
//        memcpy(&temp_snd_buf[5], &CardInfo._CARD_NUMBER.detail_card_number.city_code[0], 2);//���д���
//        memcpy(&temp_snd_buf[7], &CardInfo.csn[0], 4);//CSNӲ������
//        memcpy(&temp_snd_buf[11], &CardInfo._CARD_NUMBER.detail_card_number.serial_number[2], 2);//��������ˮ�ź�2�ֽ�
//        memcpy(&temp_snd_buf[13], &CardInfo.authentication_code[0], 4);
//        memcpy(&temp_snd_buf[17],"\x01\x02\x03\0x04\0x05", 5);
//        k = CmdPsam(DevStat.cur_psam_channel, temp_snd_buf, 22, temp_rcv_buf, &rcv_length);//��MAC1
//        if ( k != 0x9000 )
//            return notok;
//        for(cnt=0;cnt<5;cnt++)
//        {
//            memcpy(&KeySector[cnt+1][0], &temp_rcv_buf[cnt*6], 6);//������������Կ��2sector
//        }
//        memcpy(&temp_snd_buf[17],"\x06\x0A\x0C\x0B\x0D", 5);
//        k = CmdPsam(DevStat.cur_psam_channel, temp_snd_buf, 13, temp_rcv_buf, &rcv_length);//��MAC1
//        if ( k != 0x9000 )
//            return notok;
//
//        memcpy(&KeySector[6][0], &temp_rcv_buf[0], 6);//������������Կ��2sector
//    }
//    if(DevStat.samcompcode==0)//���濨Ƭ
//    {
//        ////////////////////////////////////////
//        //ISAM��ѡ1001Ŀ¼
//        ////////////////////////////////////////
//        k = CmdPsam(DevStat.cur_psam_channel, (void *)SELECT_SAM_ADF1, sizeof(SELECT_SAM_ADF1),
//                    temp_buf, &rcv_length);     //ѡ1001Ŀ¼
//        if ( k != 0x9000 )
//            return notok;
//
//        ////////////////////////////////////////
//        //ISAM���������Կ
//        ////////////////////////////////////////
//        for(cnt=1;cnt<7;cnt++)
//        {
//            if(cnt==7 ||cnt==8 || cnt==9)
//                continue;
//            memcpy(temp_snd_buf, ISAM_CAL_KEY_INIT, sizeof(ISAM_CAL_KEY_INIT));//ͷ
//            memcpy(&temp_snd_buf[5], &CardInfo._CARD_NUMBER.detail_card_number.city_code[0], 2);//���д���
//            memcpy(&temp_snd_buf[7], &CardInfo.csn[0], 4);//CSNӲ������
//            memcpy(&temp_snd_buf[11], &CardInfo._CARD_NUMBER.detail_card_number.serial_number[2], 2);//��������ˮ�ź�2�ֽ�
//
//            k = CmdPsam(DevStat.cur_psam_channel, temp_snd_buf, 13, temp_rcv_buf, &rcv_length);//��MAC1
//
//            if ( k != 0x9000 )
//                return notok;
//
//            memcpy(temp_snd_buf, ISAM_CAL_KEY, sizeof(ISAM_CAL_KEY));//ͷ
//            memcpy(&temp_snd_buf[5], &CardInfo._CARD_NUMBER.detail_card_number.city_code[0], 2);//���д���
//            memcpy(&temp_snd_buf[7], &CardInfo.csn[0], 4);//CSNӲ������
//            temp_snd_buf[11] = cnt;//������
//            temp_snd_buf[12] = 0x00;
//
//            k = CmdPsam(DevStat.cur_psam_channel, temp_snd_buf, 13, temp_rcv_buf, &rcv_length);//��MAC1
//
//            if ( k != 0x9000 )
//                return notok;
//
//            memcpy(&KeySector[cnt][0], &temp_rcv_buf[0], 6);//������������Կ��2sector
//        }
//    }
//*/
//    ////////////////////////////////////////
//    //�������
//    ////////////////////////////////////////
//
///*
//    memcpy(&temp_int32u, &CardInfo._CARD_NUMBER.detail_card_number.serial_number[0], 4);
//    i = BlackName_Find(temp_int32u);    //�������ж�
//    if ( i == ok )
//    {   //�����Ǻ�������
//#ifdef DEBUG
//        TRACE232("find black name card!\n\r");
//#endif
//    //  i = Lock_M1_Card_Process();//����������������
//    //  return i;
//        if(select_affirm(4, "�Ƿ����Ƿ����?") != 0)
//            return notok;
//    }
//*/
////  MC530Off();
////
////  if( InitMC530(TYPE_A_ISO14443) != MI_OK)
////  	return notok;
////��ֵ����
//    CardInfo.card_purchase_type = TT_CREDIT_MONEY;  //��ֵ��������
//    i = M1_Card_Credit_Process();
//    if(i == 0xF0)
//    {
////  	lcddisperr("i == 0xf0");
//        EA_ucMIFHalt(hMifsHandle);  //�ҿ�
//        return 0xF0;
//    }
//
//    if(i != ok)
//    {
////  	lcddisperr("i != ok");
//        EA_ucMIFHalt(hMifsHandle);  //�ҿ�
//        return notok;
//    }
//
////�����ֵ��¼
//    CardInfo.card_purchase_type = TT_CREDIT_MONEY;
//    (void)CalTAC();
//    (void)EA_ucMIFHalt(hMifsHandle);     //�ҿ�
//
//    (void)StoreRecord(&CardInfo);           //�����ֵ��¼
//    (void)DisplayPurchaseResult(&CardInfo); //��ʾ���׽��
//
//    //��¼���һ�εĽ�������
//    DevStat.last_trade_type = TT_CREDIT_MONEY;
//    DevStat.gray_flag = FALSE;
//    WriteParam();
//
//    if(DevStat.mode==0x00)
//    {
////  	EA_ucSetStopFlag(EM_DISABLE_STOP);							 //��ֹ����ʡ��ģʽ
//        print_record();//��ӡ��¼
////  	EA_ucSetStopFlag(EM_ENABLE_STOP);
//    }
//
////�����ϴ�
////  MC530Off();
//    i = GPRS_SEND_RECORD();
//    if(i == ok)
//    {
//        DevStat.have_data_not_send_flag = FALSE;
//    }
//
////�����ֵ��Կ
//    for(j=2;j<7;j++)
//    {
//        memcpy(DevStat.KEYVALUE[j-2], KeySector[j], 6);
//    }
//    WriteParam();
//
////	GPRSProcess();
//    //�ָ�״̬
//    memset(&CardInfo, 0x00, sizeof(CardInfo));      //�忨Ƭ������Ϣ
//    memset(&ErrCardInfo, 0x00, sizeof(ErrCardInfo));    //�������Ϣ
//    //DispPurchaseResultDelay = DISP_DELAY;
//    DevStat.purchase_again_req = FALSE;                 //������ˢ��������־
//    DevStat.voice_purchase_again_freq = FALSE;
//
////��¼�ϴ�
//    return i;
//}
/*****************************************************************
����ԭ�ͣ�unpackInt(unsigned char *msg)
����������///\x00\x00\x00\x64 -> 100
����������
�������ƣ�	����/�����	����		����
-----------		-----------	------	   	-------
�� �� ֵ����
*****************************************************************/
unsigned int unpackInt(unsigned char *msg)
{
	union
	{
		int i;
		struct 
		{
			unsigned char ch1;
			unsigned char ch2;
			unsigned char ch3;
			unsigned char ch4;
		} ch;
	} ui;
	
	ui.ch.ch4=msg[0];
	ui.ch.ch3=msg[1];
	ui.ch.ch2=msg[2];
	ui.ch.ch1=msg[3];
	
	return ui.i;
}
/*****************************************************************
����ԭ�ͣ�over_turn(unsigned char length,unsigned char *ptr)
����������������Ϊ�ַ����ߵ��ֽ�ת������
����������lengrth = �ַ������� 
		  *ptr    = ������ַ��� 
�������ƣ�	����/�����	����		����
-----------		-----------	------	   	-------
�� �� ֵ����
*****************************************************************/
void over_turn(unsigned char length,unsigned char *ptr)
{
	unsigned char i;
	unsigned char temp[16];
	memcpy(temp,ptr,length);
	for(i=0;i<length;i++)
	{
		ptr[i]=temp[length-i-1];
	}
}
/*****************************************************************
����ԭ�ͣ�Cpucard_Init
������������������ʼ��SAM OR CPU��ͨ��
������������
�������ƣ�	����/�����	����		����
-----------		-----------	------	   	-------
�� �� ֵ����
*****************************************************************/
INT8U Cpucard_Init()  
{	
//  INT16U key = 0;
//
//  (void)EA_vCls();
//  while( EA_ucICCardIn(hCpuHandle) != EM_SUCCESS )
//  {
//  	(void)EA_vDisplay(2, "     �����CPU��    ");
//  	key = EA_uiInkeyMs(10);
//  	if( key == EM_key_CANCEL )
//  		return notok;
//  }
	return ok;	 
}
	
/*****************************************************************
����ԭ�ͣ�Cpucard_power_off
�����������������ر�ͨ��
������������
�������ƣ�	����/�����	����		����
-----------		-----------	------	   	-------
�� �� ֵ����
*****************************************************************/
void Cpucard_power_off()
{
//  uchar result;
//  result = EA_ucICPowerDown(hCpuHandle);
//  if  (result != EM_SUCCESS)
//  {
//  	return;
//  }

}
//
///*****************************************************************
//����ԭ�ͣ�CPU_SET_PROCESS
//����������������ΪCPU���ÿ��������
//������������
//�������ƣ�	����/�����	����		����
//-----------		-----------	------	   	-------
//�� �� ֵ����
//*****************************************************************/
//int CPU_SET_PROCESS(INT8U * data)
//{
///*
//    �����ͱ�ʶ  1
//    ��ֵ�������    3
//    �����������    4
//    ����Ա��    3
//    ����Ա����  3
//    ����Ӫʱ��    4
//    �ۿ������Ĵ���  2
//    �ۿ������  2
//    ���ߴ���    3
//    ���ķ�������ַ  4
//    �������˿ں�    2
//    ��Ƭ״̬    1
//    �����ֵ����    1
//*/
//
////	DevStat.max_price_limit = HextoBcd(*(data+1)) * 10000  + HextoBcd(*(data+2))*100 + HextoBcd(*(data+3));
////	DevStat.max_card_limit = HextoBcd(*(data+4)) * 1000000  + HextoBcd(*(data+5))*10000 + HextoBcd(*(data+6))*100 + HextoBcd(*(data+7));
//
//    DevStat.max_price_limit = bcdhex(*(data+1)) * 10000  + bcdhex(*(data+2)) * 100 + bcdhex(*(data+3));
//    DevStat.max_card_limit = bcdhex(*(data+4)) * 1000000  + bcdhex(*(data+5)) * 10000 + bcdhex(*(data+6)) * 100 + bcdhex(*(data+7));
//
//    htoa(&DevStat.card_password[0], data+11, 3);
//    memcpy(&DevStat.acnt_id[0], data+18, 2);
////  memcpy(&DevStat.spot_id[0], data+20, 2);    by liujihua  û���ҵ�ԭ�򣬴�����ʾ��2600����ȷ��ֵΪ��0032
//    memcpy(&DevStat.spot_id[0], data+18, 2);
//    memcpy(&DevStat.equ_id[0], data+22, 3);
//
//    memcpy(&DevStat.deposit_amt[0], data+34, 2);
//
//    //��������ַ
//    memcpy(&DevStat.ServerIp[0], data+25, 4);
//    //�������˿ں�
//    memcpy(&DevStat.ServerPort[0], data+29, 2);
//
//    if(*(data+31)==0x02)//�˿��Ѿ�ͣ��
//        return -1;
//
//    DevStat.have_been_set = TRUE;
//    DevStat.driver_is_working=FALSE;
//
//    WriteParam();
//
//    return 1;
//
//}

//
///*****************************************************************
//����ԭ�ͣ�CPU_OPER_PROCESS
//����������������ΪCPU����Ա���������
//������������
//�������ƣ�	����/�����	����		����
//-----------		-----------	------	   	-------
//�� �� ֵ��     -1:��һ������Աû��ǩ��
//               -2:����Ա�������
//               -3:��Ƭ�Ѿ�����Ч��
//               -4:��Ƭ�Ѿ�ͣ��
//               -5: û�п��õ���Ч����
//               -6: ʲô��������ֱ���˳�
//               1:  �ɹ�
//*****************************************************************/
//int CPU_OPER_PROCESS(INT8U * data)
//{
///*
//    �����ͱ�ʶ  1
//    ��ֵ�������    3
//    �����������    4
//    ����Ա��    3
//    ����Ա����  3
//    ����Ӫʱ��    4
//    �ۿ������Ĵ���  2
//    �ۿ������  2
//    ���ߴ���    3
//    ���ķ�������ַ  4
//    �������˿ں�    2
//    ��Ƭ״̬    1
//    �����ֵ����    1
//*/
//    BUS_TIME            time_data;
//    INT8U               temp_rcv_buf[50], temp_snd_buf[50];
//    INT8U               i;
//    INT16U              k;
//    uint                rcv_len = 0;
//    int                 ret = 0;
//
////ȡϵͳ����
//    Get_Time(&time_data);
//
//    temp_snd_buf[0] = time_data.century;
//    temp_snd_buf[1] = time_data.year;
//    temp_snd_buf[2] = time_data.month;
//    temp_snd_buf[3] = time_data.day;
////��Ƭ״̬���
//
//    if(*(data+31)==0x02)//�˿��Ѿ�ͣ��
//        return -4;
//
////����Ա���ű���
//    if(!DevStat.driver_is_working)
//    {
//        DevStat.cur_driver_number[0] = 0x30;
//        memcpy(&DevStat.cur_driver_number[1], data+8, 3);          //by liujihua
//        memset(&DevStat.binhand, 0x00, sizeof(DevStat.binhand));
//        memcpy(&DevStat.binhand.oper_id[0], data+8, 3);
//    //  memcpy(&DevStat.binhand.end_date[0], temp_snd_buf, 4);
//    }
//    else
//    {
//        if(memcmp(&DevStat.cur_driver_number[1], data+8, 3) != 0)//����ͬһ������Ա��  //by liujihua
//        {
//            return -1;
//        }
//    }
//
//    htoa(&DevStat.card_password[0], data+11, 3);
//
//
////����Ա���������
////����ֵ	== 0  ��ȷ����
////			== 1  δ�����ַ�
////			== -1 ���˳���
////			== -2 �ο��˳�
////			== -3 ��FUNC2�˳�
////			== -4 ��FUNC1�˳�
//
//    ret = OperPasswordCheck();
//    if(ret == notok)//����������
//    {
//
//        lcddisperr("�������һ��");
//        ret = OperPasswordCheck();
//        if(ret != ok)
//        {
//            lcddisperr("����������");
//
//            ret = OperPasswordCheck();
//            if(ret != ok)
//            {
//                lcddisperr("�����������");
//                *(data+31) = 0x02;
//                for ( i=0; i<8; i++ )             //������㲻�ɹ��������ѭ������8��
//                {
//                    ////////////////////////////////////////
//                    //���Ŀ�Ƭʹ������ʶ
//                    ////////////////////////////////////////
//                    memcpy(temp_snd_buf, WRITE_15_FILE, 5);
//
//                    memcpy(&temp_snd_buf[5], data, 36);
//                    k = CmdPsam(hCpuHandle, temp_snd_buf, 41, temp_rcv_buf, &rcv_len);      //TAC�����ʼ��
//                    if ( k != ok )
//                        continue;
//                    else
//                        break;
//                }
//                return -2;//������֤����ֱ���˳�
//            }
//        }
//    }
//    else if( ret == -6 )
//    {
//        return -6;
//    }
//
////��Ч�ڼ��
//    memcpy(&DevStat.useful_datetime[0],data+14, 4);
//    if(memcmp(&DevStat.useful_datetime[0], temp_snd_buf, 4) < 0)
//        return -3;
//
//////��Ƭ��Ч����
////	DevStat.useful_cnt = *(data+32);
////	if(DevStat.useful_cnt <=0)
////		return -5;
//
//    DevStat.driver_is_working = TRUE;
//
//    //��ȡ16�ļ����ݣ�����ȡ���ڽ��
//
//    k = CmdPsam(hCpuHandle, (void *)READ_16_FILE, sizeof(READ_16_FILE) ,temp_rcv_buf, &rcv_len);
//    if ( k != ok )
//    {
//        EA_vCls();
//        EA_vDisplay(1,"��ȡǮ���ļ�����");
//        EA_vDisplay(2,"����������˳�");
//        EA_uiInkey(0);
//        return -5;
//    }
//
//    if( Purse_Block_Is_Right(temp_rcv_buf) != ok )//Ǯ����ʽ����
//    {
//        EA_vCls();
//        EA_vDisp(1,1,"����Ա��Ǯ����");
//        EA_vDisp(2,1,"�����޷���������");
//        EA_vDisp(3,1,"��ֿ������Ĵ���");
//        EA_vDisp(4,1,"����������˳�");
//        EA_uiInkey(0);
//        return -5;
//    }
//
//    DevStat.oper_card_amt = (((INT32U)temp_rcv_buf[0] * 0x1000000 + (INT32U)temp_rcv_buf[1] * 0x10000 + (INT32U)temp_rcv_buf[2] * 0x100 + (INT32U)temp_rcv_buf[3]));
//
//    if( !DevStat.binhand.begin_flag )
//    {
//        memcpy(&DevStat.binhand.work_datetime, &time_data, sizeof(BUS_TIME));//begin work datetime
//        DevStat.binhand.begin_flag = TRUE;
////SEND OPER CARDNO TO BANKSERVICE. BANKSERVICE CHECK if OPER CARD IS BLACK.
//        if(GPRS_OPER_CARD_CHECK(DevStat.oper_cardno) != ok)
//        {
//            DevStat.driver_is_working = FALSE;
//            return -6;
//        }
//
//    }
//
//    //20071019 modify by yfy add oper card_amt_e
//    DevStat.binhand.cpu_card_amt_b = DevStat.oper_card_amt ;
//    DevStat.driver_is_working = TRUE;
//    WriteParam();
//
//    return 1;
//}
//
//
///*****************************************************************
//����ԭ�ͣ�CpuDecrement
//����������������Ϊ����Ա���ۿ�
//������������
//�������ƣ�	����tFlag---��������:0---�ۿ�ף�1:��ֵ����
/////�����	����		����
//-----------		-----------	------	   	-------
//�� �� ֵ��	-1:�޿�
//              -2:ѡ��Ŀ¼����
//              -3:������֤����
//              -4:
//
//*****************************************************************/
//int CpuDecrement(INT8U * data, INT32U trade_amt)
//{
//    INT32U          fCpuBalance;
////	unsigned long  lCpuBalance;
//    INT8U           cnt=0;
//    unsigned int    LOCAL_V_TYPE  rcv_len = 0;
//    INT8U           temp_rcv_buf[50], temp_snd_buf[50];
//    uchar           k=0;
//
//    if(trade_amt == 0)
//        return ok;
//
//    fCpuBalance = ( ((INT32U)data[0] * 0x1000000) + ((INT32U)data[1] * 0x10000 ) + ((INT32U)data[2] * 0x100 ) + (INT32U)data[3] ) ;
//
////  fCpuBalance = 1003000;
//
//    if( ErrCardInfo.error_occured == TRUE || ErrCardInfo.sale_error_occured == TRUE)
//    {
//        if( fCpuBalance == ErrCardInfo.init_cpu_card_balance )//û�пۿ�
//        {
//
//            //lcddisperr("û�м���");
//        }
//        else
//        {
////			DevStat.oper_card_amt -= trade_amt;
////			DevStat.binhand.credit_amt += trade_amt;
////			DevStat.binhand.credit_amt_cnt ++;
////			WriteParam();
//            return ok;     //������������Ĵ�����û�пۿ�Ļ����ѿ�۵�
//        }
//    }
//
//    ErrCardInfo.init_cpu_card_balance = fCpuBalance;
//
//    if(fCpuBalance < trade_amt)
//    {
//        EA_vCls();
//        EA_vDisplay(1,"����Ա������");
//        EA_vDisplay(2,"�����޷��������а�");
//        EA_vDisplay(3,"��ֿ������ĳ�ֵ");
//        EA_vDisplay(4,"����������˳�");
//        EA_uiInkey(5);
//        return notok;
//    }
//
//    fCpuBalance -= trade_amt;
//
//    data[0] = (INT8U)(fCpuBalance / 0x1000000 );
//    data[1] = (INT8U)(fCpuBalance % 0x1000000 / 0x10000 );
//    data[2] = (INT8U)(fCpuBalance % 0x10000 / 0x100 );
//    data[3] = (INT8U)(fCpuBalance % 0x100 );
//
//    data[4] = ~(INT8U)(fCpuBalance / 0x1000000 );
//    data[5] = ~(INT8U)(fCpuBalance % 0x1000000 / 0x10000 );
//    data[6] = ~(INT8U)(fCpuBalance % 0x10000 / 0x100 );
//    data[7] = ~(INT8U)(fCpuBalance % 0x100 );
//
//    data[8] = (INT8U)(fCpuBalance / 0x1000000);
//    data[9] = (INT8U)(fCpuBalance % 0x1000000 / 0x10000);
//    data[10]  = (INT8U)(fCpuBalance % 0x10000 / 0x100);
//    data[11]  = (INT8U)(fCpuBalance % 0x100);
//
//    memcpy(data+12, "\x18\xE7\x18\xE7", 4);
//    for ( cnt = 0; cnt < 8; cnt ++ )              //������㲻�ɹ��������ѭ������8��
//    {
//        ////////////////////////////////////////
//        //����Ǯ��
//        ////////////////////////////////////////
//        memcpy(temp_snd_buf, WRITE_16_FILE, 5);
//
//        memcpy(&temp_snd_buf[5], data, 16);
//        k = CmdPsam(hCpuHandle, temp_snd_buf, 21, temp_rcv_buf, &rcv_len);      //TAC�����ʼ��
//        if ( k != ok )
//        {
//            continue;
//        }
//        else
//        {
//            DevStat.oper_card_amt -= trade_amt;
//            DevStat.binhand.credit_amt += trade_amt;
//            DevStat.binhand.credit_amt_cnt ++;
//            WriteParam();
//            return ok;
//        }
//    }
//
//    if(cnt == 8)
//    {
//    //  ErrCardInfo.error_occured=TRUE ;
//        EA_vCls();
//        EA_vDisplay(1,"����Ա������ʧ��");
//        EA_vDisplay(2,"�����޷���������");
//        EA_vDisplay(3,"����������˳�");
//        EA_uiInkey(5);
//        return notok;
//    }
//    else
//        return ok;
//
//}
//
///*****************************************************************
//����ԭ�ͣ�CpuIncrement
//����������������Ϊ����Ա���ۿ�
//������������
//�������ƣ�	����tFlag---��������:0---�ۿ�ף�1:��ֵ����
/////�����	����		����
//-----------		-----------	------	   	-------
//�� �� ֵ��	-1:�޿�
//              -2:ѡ��Ŀ¼����
//              -3:������֤����
//              -4:
//
//*****************************************************************/
//int CpuIncrement(INT8U * data, INT32U trade_amt)
//{
//    INT32U      fCpuBalance;
////	unsigned long  lCpuBalance;
//    INT8U       cnt=0;
//    INT8U       temp_rcv_buf[50], temp_snd_buf[50];
//    uchar      k=0;
//    uint        LOCAL_V_TYPE  rcv_len = 0;
//
//    if(trade_amt == 0)
//        return ok;
//
////  fCpuBalance = ( ((INT32U)data[0]<<24) + ((INT32U)data[1]<<16) + ((INT32U)data[2]<<8) + (INT32U)data[3] );
//    fCpuBalance = (((INT32U)data[0] * 0x1000000 ) + ((INT32U)data[1] * 0x10000 ) + ((INT32U)data[2] * 0x100 ) + (INT32U)data[3] ) ;
//
//    if( ErrCardInfo.error_occured==TRUE || ErrCardInfo.sale_error_occured==TRUE)
//    {
//        if(fCpuBalance == ErrCardInfo.init_cpu_card_balance)//û�пۿ�
//        {
//            //lcddisperr("û�пۿ�");
//        }
//        else
//        {
////			DevStat.oper_card_amt -= trade_amt;
////			DevStat.binhand.credit_amt += trade_amt;
////			DevStat.binhand.credit_amt_cnt ++;
////  		WriteParam();
//            return ok;
//        }
//    }
//
//    ErrCardInfo.init_cpu_card_balance = fCpuBalance;
//
//    fCpuBalance += trade_amt;
//
//    data[0] = (INT8U)(fCpuBalance / 0x1000000 );
//    data[1] = (INT8U)(fCpuBalance % 0x1000000 / 0x10000 );
//    data[2] = (INT8U)(fCpuBalance % 0x10000 / 0x100 );
//    data[3] = (INT8U)(fCpuBalance % 0x100 );
//
//    data[4] = ~(INT8U)(fCpuBalance / 0x1000000 );
//    data[5] = ~(INT8U)(fCpuBalance % 0x1000000 / 0x10000);
//    data[6] = ~(INT8U)(fCpuBalance % 0x10000 / 0x100 );
//    data[7] = ~(INT8U)(fCpuBalance % 0x100 );
//
//    data[8] = (INT8U)(fCpuBalance / 0x1000000 );
//    data[9] = (INT8U)(fCpuBalance % 0x1000000 / 0x10000 );
//    data[10] = (INT8U)(fCpuBalance % 0x10000 / 0x100 );
//    data[11] = (INT8U)(fCpuBalance % 0x100 );
//
//    memcpy(data+12, "\x18\xE7\x18\xE7", 4);
//    for ( cnt = 0; cnt < 8; cnt ++ )              //������㲻�ɹ��������ѭ������8��
//    {
//        ////////////////////////////////////////
//        //����Ǯ��
//        ////////////////////////////////////////
//        memcpy(temp_snd_buf, WRITE_16_FILE, 5);
//
//        memcpy(&temp_snd_buf[5], data, 16);
//        k = CmdPsam(hCpuHandle, temp_snd_buf, 21, temp_rcv_buf, &rcv_len);      //TAC�����ʼ��
//        if ( k != ok )
//        {
//            //lcddisperr("k == %02x ", k);
//            continue;
//        }
//        else
//        {
//
//            DevStat.oper_card_amt += trade_amt;
//            DevStat.binhand.flush_amt_cnt++;
//            DevStat.binhand.flush_amt += trade_amt;
//            WriteParam();
//            return ok;
//        }
//    }
//
//    if(cnt == 8)
//    {
//        //ErrCardInfo.error_occured = TRUE;
//        EA_vCls();
//        EA_vDisplay(1,"����Ա������ʧ��");
//        EA_vDisplay(2,"�����޷��������а�");
//        EA_vDisplay(3,"����������˳�");
//        EA_uiInkey(5);
//        return notok;
//    }
//    else
//        return ok;
//}
//
//
/*****************************************************************
����ԭ�ͣ�OperTradeProcess
����������������Ϊ����Ա�����׺���
������������
�������ƣ�	����tFlag---��������:0---�ۿ�ף�1:��ֵ����
///�����	����		����
-----------		-----------	------	   	-------
�� �� ֵ�� 	  -1:�޿�
			  -2:ѡ��Ŀ¼����
			  -3:������֤����
			  -4:

*****************************************************************/
int OperTradeProcess(int tFlag, INT32U trade_amt)
{
	INT8U rcv_buf[100];
	uint rcv_length;
	INT16U  k;
	int     ret = 0;

	//cpu����λ
	ret = Cpucard_power_on(rcv_buf, &rcv_length);
	if( ret != ok )
	{
		Cpucard_power_off();
		lcddisperr("CPU����λʧ��");
		return -1;//�޿�
	}
		
	memset(rcv_buf, 0x00, sizeof(rcv_buf));
	rcv_length = 0;
	
	//ѡ��1002Ŀ¼
	k = CmdPsam(hCpuHandle, (void *)SELECT_USER_ADF2, sizeof(SELECT_USER_ADF2) ,rcv_buf, &rcv_length);
	if ( k != ok )
	{
		EA_vCls();
		EA_vDisplay(1,"ѡ��Ŀ¼����");
		EA_uiInkeyMs(2000);
		return -2;
	}

	//��֤����
	k = CmdPsam(hCpuHandle, (void *)VERIFY_PIN, sizeof(VERIFY_PIN) ,rcv_buf, &rcv_length);
	if ( k != ok )
	{
		EA_vCls();
		EA_vDisplay(1,"��֤�������");
		EA_vDisplay(2,"�Ǳ�ϵͳ��Ƭ");
		EA_vDisplay(3,"����������˳�");
		EA_uiInkeyMs(2000);
		return -3;
	}

	//��ȡ15�ļ����ݣ����ж���ʲô��Ƭ����
	k = CmdPsam(hCpuHandle, (void *)READ_15_FILE, sizeof(READ_15_FILE) ,rcv_buf, &rcv_length);
	if ( k != ok )
	{
		EA_vCls();
		EA_vDisplay(1,"��ȡ��Ϣ�ļ�����");
		EA_vDisplay(2,"����������˳�");
		EA_uiInkeyMs(2000);
		return -3;
	}

	//�ж��Ƿ���ͬһ�Ų���Ա��
	if(memcmp(&DevStat.cur_driver_number[1], rcv_buf+8, 3) != 0)//����ͬһ������Ա��
		return -6;		

	//��ȡ16�ļ����ݣ�����ȡ���ڽ��
	//
	k = CmdPsam(hCpuHandle, (void *)READ_16_FILE, sizeof(READ_16_FILE) ,rcv_buf, &rcv_length);
	if ( k != ok )
	{
		EA_vCls();
		EA_vDisplay(1,"��ȡǮ���ļ�����");
		EA_vDisplay(2,"����������˳�");
		EA_uiInkeyMs(2000);
		return -3;
	}

	if(Purse_Block_Is_Right(rcv_buf) !=ok)//Ǯ����ʽ����
	{
		EA_vCls();
		EA_vDisplay(1,"����Ա��Ǯ����");
		EA_vDisplay(2,"�����޷���������");
		EA_vDisplay(3,"��ֿ������Ĵ���");
		EA_vDisplay(4,"����������˳�");
		EA_uiInkey(5);
		return -4;
	}

	if(tFlag)
	{
		if(CpuIncrement(rcv_buf, trade_amt) == ok)
			return ok;
	}
	else
	{
		if(CpuDecrement(rcv_buf, trade_amt) == ok)
			return ok;
	}
	
	return notok;

}
/******************************************************************************
 �������ƣ�Cpucard_power_on
 ������������λCPU��
 ����������
 �������ƣ�	����/�����	����		����  
   
 ��  ��  ֵ��ok(0)-�ɹ�
			 notok(0xFF)-ʧ��
				   
 ��      ��	��������
 ��      �ڣ�2012-12-12
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
******************************************************************************/
INT8U Cpucard_power_on(INT8U *prcvbuf, uint *prcvLen)
{
	uchar result;
	uchar prot;
	// �����ϵ�
	result = EA_ucICPowerUp(hCpuHandle, prcvLen, prcvbuf, &prot);
	if  ((result != EM_SUCCESS) || (*prcvbuf != 0x3b ))
	{
		return notok;
	}
	return ok;
}


/*****************************************************************
����ԭ�ͣ�CpuProcess
����������������ΪCPU������
������������
�������ƣ�	����/�����	����		����
-----------		-----------	------	   	-------
�� �� ֵ�� 		0:���ս�
				1:��ʾ�ɹ�		
			   -1:�޿�
			  -2:ѡ��Ŀ¼����
			  -3:������֤����
    		  -4:��Ƭ���Ͳ�����
    		  -5:��Ƭ�Ѿ�ͣ��
    		  -6:ʲô��������ֱ���˳�

*****************************************************************/
int CpuProcess()
{
//    INT8U  rcv_buf[20];
//    uint   rcv_length = 0;
//    INT8U  k;
//    INT16U  key;
//    INT8S  ret = 0;
//
//    //cpu����λ
//    memset(rcv_buf,0,sizeof(rcv_buf));
//    ret = Cpucard_power_on( rcv_buf, &rcv_length );
//    if( ret != ok)
//    {
//        lcddisperr("        �޿�        ");
//        EA_vDisplay(4, "   ����������˳�   ");
//        (void)EA_uiInkey(0);
//        return -1;
//    }
//    //ѡ��1002Ŀ¼ ,��CPU���ļ�
//    rcv_length = 0;
//    k = CmdPsam(hCpuHandle, (void *)SELECT_USER_ADF2, sizeof(SELECT_USER_ADF2) ,rcv_buf, &rcv_length);
//    if ( k != ok )
//    {
//        lcddisperr("ѡ��Ŀ¼����");
////  	EA_vDisplay(3,"����������˳�");
////  	(void)EA_uiInkey(0);
//        return -2;
//    }
//    //��֤����
//    k = CmdPsam(hCpuHandle, (void *)VERIFY_PIN, sizeof(VERIFY_PIN) ,rcv_buf, &rcv_length);
//    if ( k != ok )
//    {
//        EA_vCls();
//        EA_vDisplay(1,"��֤�������");
//        EA_vDisplay(2,"�Ǳ�ϵͳ��Ƭ");
//        EA_vDisplay(3,"����������˳�");
//        (void)EA_uiInkey(0);
//        return -3;
//    }
//    //��ȡ15�ļ����ݣ����ж���ʲô��Ƭ����
//    k = CmdPsam(hCpuHandle, (void *)READ_15_FILE, sizeof(READ_15_FILE) ,rcv_buf, &rcv_length);
//    if ( k != ok )
//    {
//        EA_vCls();
//        EA_vDisplay(1,"��ȡ��Ϣ�ļ�����");
//        EA_vDisplay(2,"����������˳�");
//        (void)EA_uiInkey(0);
//        return -3;
//    }
//    //add gather_type card --to gather data when oper_type card damage
//    if(rcv_buf[0] == 0x82)
//    {
//        //DataGather();
//        return 0;
//    }
//     //add worker_down card --- permit  oper down
//    if(rcv_buf[0] == 0x83)
//    {
//        EA_vCls();
//        EA_vDisp(4, 1, "   �Ƿ��������ս�?  ");
//        key = EA_uiInkey(0);
//        switch(key)
//        {
//           case EM_key_ENTER:
//                if(Gprs_Upload_data() != ok)
//                {
//                    lcddisperr("     ǩ�˲��ɹ�!    ");
//                }
//                else
//                {
//                    EA_vCls();
//                    EA_vDisplay(1, "   ��ӭʹ�ñ��豸   ");
//                    EA_vDisplay(2, "      ǩ�˳ɹ�      ");
//                    EA_vDisplay(3, "   ����������˳�   ");
//                    (void)EA_uiInkey(5);
//                }
//                break;
//            default :
//                break;
//        }
//        return 0;
//    }
//    if((rcv_buf[0] != 0x80) && (rcv_buf[0] != 0x81))//�����Ͳ����ϣ��˳�
//    {
//        (void)EA_vCls();
//        (void)EA_vDisplay(1,"   ��Ƭ���Ͳ�����   ");
//        (void)EA_vDisplay(2,"   ����������˳�   ");
//        (void)EA_vDisplay(3,"   �����ͣ�%x", rcv_buf[0]);
//        (void)EA_uiInkey(0);
//        return -4;
//    }
//    if( !DevStat.have_been_set )//δˢ���ÿ�         ������  ������
////  if(0)
//    {
//        if((rcv_buf[0] != 0x81) )//�����Ͳ����ϣ��˳�
//        {
//            EA_vCls();
//            EA_vDisplay(1,"    ��������ÿ�    ");
//            EA_vDisplay(2,"   ����������˳�   ");
//            EA_uiInkeyMs(500);
//            return -4;
//        }
//        else
//        {
//            if(CPU_SET_PROCESS(rcv_buf) < 0)
//            {
//                EA_vCls();
//                EA_vDisplay(1,"    ��Ƭ�Ѿ�ͣ��    ");
//                EA_vDisplay(2,"   ����������˳�   ");
//                EA_uiInkey(10);
//                return -5;
//            }
//            else
//            {
//                EA_vCls();
//                EA_vDisplay(1,"   ���ÿ����óɹ�   ");
//                EA_vDisplay(2,"   ����������˳�   ");
//                EA_uiInkey(10);
//                return 1;
//            }
//
//        }
//    }
//    else//ˢ�����ÿ����ж��Ƿ��ǲ���Ա��
//    {
//        if(rcv_buf[0] == 0x80)//����Ա��
//        {
//            ret = CPU_OPER_PROCESS(rcv_buf);
//            if( ret == -1 )//�ǵ������Ա
//            {
//                EA_vCls();
//                EA_vDisplay(1,"    �ǵ������Ա    ");
//                EA_vDisplay(2,"   ����������˳�   ");
//                EA_uiInkey(5);
//                return -5;
//            }
//            else if( ret == -2 )
//            {
//                EA_vCls();
//                EA_vDisplay(1,"   ����Ա�������   ");
//                EA_vDisplay(2,"   ��Ƭ�Ѿ�����ס   ");
//                EA_vDisplay(3,"   �ֿ������Ľ���   ");
//                EA_vDisplay(4,"   ����������˳�   ");
//                EA_uiInkey(5);
//                return -5;
//            }
//            else if( ret == -3 )
//            {
//                EA_vCls();
//                EA_vDisplay(1,"   ��Ƭ�ѹ���Ч��   ");
//                EA_vDisplay(2,"   �ֿ������Ļָ�   ");
//                EA_vDisplay(3,"   ����������˳�   ");
//                EA_uiInkey(5);
//                return -5;
//            }
//            else if( ret == -4 )
//            {
//                EA_vCls();
//                EA_vDisplay(1,"    ��Ƭ�Ѿ�ͣ��    ");
//                EA_vDisplay(2,"   ����������˳�   ");
//                EA_uiInkey(10);
//                return -5;
//            }
//            else if( ret == 1 )
//            {
//                EA_vCls();
//                EA_vDisplay(1,"   ����Ա��½�ɹ�   ");
//                EA_vDisplay(2,"   ����������˳�   ");
//                EA_uiInkey(5);
//                return 1;
//            }
//            else if( ret == -6 )
//            {
//                return -6;
//            }
//            else
//            {
//                return -5;
//            }
//        }
//        if((rcv_buf[0] == 0x81) )//���ÿ�
//        {
//            if( CPU_SET_PROCESS(rcv_buf ) < 0)
//            {
//                EA_vCls();
//                EA_vDisplay(1,"    ��Ƭ�Ѿ�ͣ��    ");
//                EA_vDisplay(2,"   ����������˳�   ");
//                EA_uiInkey(10);
//                return -5;
//            }
//            else
//            {
//                EA_vCls();
//                EA_vDisplay(1,"   ���ÿ����óɹ�   ");
//                EA_vDisplay(2,"   ����������˳�   ");
//                EA_uiInkey(10);
//                return 1;
//            }
//        }
//        return -6;
//    }
	return 1;
}

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
INT8U oper_login_check(void)
{	
	char input[22];
	uint i = 0;
	uchar k = 0;
//  EA_vCls();
//  EA_vDisplay(1, "     ���ڵ�½...    ");
//  EA_vDisplay(2, "      ��ȴ�...     ");
//  SleepMs(2000);
	
//  if( Open_GPRS_Module() != ok )
//  {
//  	lcddisperr("PPP��������ʧ��");
//  	return notok;
//  }
//  if( GPRS_TEST() != ok )          //����2012.12.18
//  {
//  	lcddisperr("ͬ��������ʱ��ʧ��");
//  	return notok;
//  }
	//��������Ƶ��100S
//  ret = EA_uiCreateAppTimer("AppTime", (void *)AppTimerISR, 0, 10000, 10000, EM_rtos_ENABLE_TIMER, &hApptimerHandle); //����ʱ��Ϊһ����
//  if (ret != EM_SUCCESS)
//  {
//  	lcddisperr("��ʱ���趨ʧ��");
//  	return notok;
//  }
	EA_vCls();
	EA_ucClrKeyBuf();

	EA_vDisplay(1, "���������ԱID:     ");
	memset((void *)input, 0, sizeof(input));

	for ( ;; )
	{
		i = EA_ucGetInputStr(1, 16, 20, EM_BIG_FONT | EM_MODE_NUMBER | EM_ALIGN_LEFT | EM_SHOW_ORIGINALLY
							 , 2, 2, 0, (void *)input);
		if ( i == EM_SUCCESS )
			break;
		if ( i == EM_ABOLISH )
			return notok;
	}

	for(k=0; k<MAX_CLERK_NUM; k++)
	{
		if ( memcmp(&input[0], &clerk_list[k].id[0], 2) == 0 )
		{
//  		memcpy(DevStat.cur_driver_number, clerk_list[k].id, 2);
			DevStat.cur_driver_number[0] = (ascii_to_hex(input[0]) << 4) | ascii_to_hex(input[1]);
			break;
		}
	}

	if( k >= MAX_CLERK_NUM )
	{
		lcddisp(4, 1, (void *)"     �û�������     ");
		(void)EA_uiInkey(2);
		return notok;
	}


	EA_vDisplay(2, "����������:         ");

	memset((void *)input, 0, sizeof(input));

	for ( ;; )
	{
		i = EA_ucGetInputStr(2, 12, 20, EM_BIG_FONT | EM_MODE_NUMBER | EM_ALIGN_LEFT | EM_SHOW_PASSWORD
							 , 4, 4, 0, (void *)input);
		if ( i == EM_SUCCESS )
			break;
		if ( i == EM_ABOLISH )
			return notok;
	}

	if ( memcmp((void *)input, &clerk_list[k].pw[0], 4) == 0 )
	{
		;
	}
	else
	{
		lcddisp(4, 1, (void *)"     ���벻��ȷ     ");
		(void)EA_uiInkey(2);
		return notok;
	}

//  if( GPRS_TEST() != ok )          //����2012.12.18
//  {
//  	lcddisperr("ͬ��������ʱ��ʧ��");
//  	return notok;
//  }
//
//  if( download_whiltelist() != ok )
//  {
//  	lcddisperr("����������ʧ��");
//  	return notok;
//  }

	return ok;
}
/******************************************************************************
 �������ƣ�download_whiltelist(void)
 �����������������µİ�����
 �������ƣ���
 ����		   ��
				
 ��  ��  ֵ����
 
 ��      ��	��������
 ��      �ڣ�2013-06-2
 �޸���ʷ��
		����		�޸���		�޸�����
******************************************************************************/
uchar download_whiltelist(void)
{
	INT8U   sendbuf[1024], str[50];
	uchar   ret = 0;
	int     package_num=0,package_index=0, package_left=0;
	INT8U	ch=0;
//��ѯ�������汾��
	EA_vCls();
	EA_vDisplay(1, "     ͬ��������     ");
	EA_vDisplay(2, "�汾:%02x%02x%02x%02x%02x%02x%02x", DevStat.black_version[0],
		 DevStat.black_version[1],DevStat.black_version[2],DevStat.black_version[3],
		 DevStat.black_version[4],DevStat.black_version[5],DevStat.black_version[6]);
	EA_vDisplay(3, "����������:%04d", DevStat.black_name_number);
	SleepMs(2000);

	if( Open_GPRS_Module() != ok )
	{
		lcddisperr("PPP��������ʧ��");
		return notok;
	}

	DevStat.black_flag = 0;
	
	memset(sendbuf, 0x00, sizeof(sendbuf));

	sprintf((void *)sendbuf, "@START%02x%02x%02xWHTV022%02x%02x%02x%02x%02x%02x%02x%02x%02x%04d",
		 DevStat.equ_id[0],DevStat.equ_id[1],DevStat.equ_id[2],
		 DevStat.acnt_id[0],DevStat.acnt_id[1],
		 DevStat.black_version[0],DevStat.black_version[1],DevStat.black_version[2],
		 DevStat.black_version[3],DevStat.black_version[4],DevStat.black_version[5],
		 DevStat.black_version[6],DevStat.black_name_number);
	
	ret = GPRS_SEND_Receive(sendbuf,41);

	if( ret != ok )
	{
		EA_vCls();
		EA_vDisplay(1, "    GPRS���ݴ���    ");
		EA_vDisplay(2, "  �������ͨѶʧ��  ");	
		EA_vDisplay(3, "  �������������ϴ�  ");
		SleepMs(2000);
		GPRS_Close();
		return notok;
	}

//��Ҫ���°�����
	if( DevStat.black_flag == 1 )
	{
		//��հ������б�
		EA_ucBlkClean ((char *)blackFileName);
		DevStat.current_record_index = 0;
		sprintf((void *)str, "����%04d��������", DevStat.black_name_number);
		EA_vDisplay(2, (void *)str);	
		SleepMs(2000);

		package_num = DevStat.black_name_number  / 90;
		package_left = DevStat.black_name_number % 90;

		//���������
		for(package_index=0;package_index<package_num;package_index++)
		{
			memset(sendbuf,0x00,sizeof(sendbuf));
			ch = (int)((package_index*100)/(package_num+1));
			sprintf((void *)str, "���ؽ���%03d%%", ch);
			EA_vDisplay(3, (void *)str);
			//SleepMs(500);
			sprintf((void *)sendbuf, "@START%02x%02x%02xWHTD012%02x%02x%04d0090", 
					DevStat.equ_id[0], DevStat.equ_id[1], DevStat.equ_id[2],
					DevStat.acnt_id[0], DevStat.acnt_id[1], package_index * 90);
			ret = GPRS_SEND_Receive((void *)sendbuf,31);
			if( ret != ok )
			{
				EA_vCls();
				EA_vDisplay(4, "����������ʧ��!");
				memset(DevStat.black_version, 0, 7);
				DevStat.black_name_number = 0;
				EA_ucBlkClean ((char *)blackFileName);
				DevStat.current_record_index = 0;
				return notok;
			}
		}

		if( ret == ok )
		{
			if( package_left>0 )
			{
				memset(sendbuf,0x00,sizeof(sendbuf));
				sprintf((void *)sendbuf, "@START000000WHTD012%02x%02x%04d%04d", DevStat.acnt_id[0],DevStat.acnt_id[1],
					(package_num*80), package_left);
				ret = GPRS_SEND_Receive(sendbuf,31);
				if( ret == ok )
				{
					EA_vDisplay(3, "�������");			
				}
				else
				{
					EA_vDisplay(4, (void *)"����������ʧ��!");
					memset(DevStat.black_version,0,7);
					DevStat.black_name_number = 0;
//  				DB_erase_filesys(DB_BLACK);
					EA_ucBlkClean((char *)blackFileName);
					DevStat.current_record_index = 0;
					return notok;
				}
			}
			else
			{
				EA_vDisplay(3, "�������");
			}
			DevStat.driver_is_working = TRUE;
			WriteParam();
			SleepMs(1000);
		}
		if( ret != ok )
		{
			EA_vCls();
			EA_vDisplay(1, "GPRS���ݴ���");
			EA_vDisplay(2, "�������ͨѶʧ��");	
			EA_vDisplay(3, "������������ʹ��");
			SleepMs(2000);
			GPRS_Close();
			return notok;
		}
	}

	else if(DevStat.black_flag == 0)
	{
		lcddisperr("�����������²��ø���");
		return ok;
	}

	return ok;
}

/*****************************************************************
����ԭ�ͣ�Login
����������������Ϊ����Ա��½����
������������
�������ƣ�	����/�����	����		����
-----------		-----------	------	   	-------
�� �� ֵ����
*****************************************************************/
void  Login(void)
{
	INT8U   strbuf[40];
//  INT8U   iflag = 0;
	INT8U   ret;
	uchar   uckey;
	uint    uikey;
	INT8U   record[60];
	BUS_TIME    ltime;

	EA_vCls();

	memset(record, 0x00, sizeof(record));

	if( DevStat.driver_is_working == FALSE )  //δ�ϰ�
	{
		if( oper_login_check() != ok )  	  //debug by frank
		{
			DevStat.driver_is_working = FALSE;
			lcddisperr("��¼ʧ��");
			return ;
		}
		else
		{
			Get_Time(&ltime); 

			memcpy(&DevStat.binhand.work_datetime, &ltime, sizeof(BUS_TIME));
			memcpy(DevStat.binhand.acnt_id, DevStat.acnt_id, 2);
			memcpy(DevStat.binhand.equ_id, DevStat.equ_id, 3);
			memcpy(&DevStat.binhand.oper_id[1], DevStat.cur_driver_number, 2);
			DevStat.binhand.consume_cnt = 0;
			DevStat.binhand.consume_amt = 0;
			DevStat.driver_is_working = TRUE;
			WriteParam();		
			lcddisperr("��¼�ɹ�");
		}
	}

#ifdef _debug_m1card_
	EA_vDisplay(1, "%02X%02X%02X%02X%02X%02X%02X", ltime.century, DevStat.binhand.work_datetime.year, 
				DevStat.binhand.work_datetime.month, DevStat.binhand.work_datetime.day, DevStat.binhand.work_datetime.hour,
				DevStat.binhand.work_datetime.minute, DevStat.binhand.work_datetime.second);
	SleepMs(3000);
#endif

	if( DevStat.driver_is_working == TRUE )   //�Ѿ��ϰ�
	{		
		for ( ;; )
		{
//			EA_vCls();
			EA_vDisplay(1, "��ɨ���Ϳ�...");
			EA_vDisplay(4, "ǩ��            ����");
			sprintf((void *)strbuf, "�������:%3d��", DevStat.binhand.consume_cnt);		
			EA_vDisplay(2, (void *)strbuf);
			EA_vDisplay(3, " ");
			if( LowBatteryDisplay() == notok )
			{
				Power_Down();
				continue;
			}
			
			if( DevStat.record_number >= MAX_REC_NUM )
			{
				lcddisperr((void *)"��¼��������ǩ��");
				Gprs_Upload_data();	 	//�����ϴ�
				break;
			}
			
			ret = CardFlush();

			if ( ret == 0xF0 )		    //����ˢ��
			{
				for ( ;; )
				{
					EA_vCls();
					EA_vDisplay(3,(void *)"������ˢ��");
					Beeperr();
					ret = CardFlush();
					if ( ret == ok )
					{
						break;
					}
					else
					{
						uckey = EA_ucKBHit();
						if ( uckey ==  EM_key_HIT )		 //����ȡ����
						{
							uikey = EA_uiInkey(0);
							if ( uikey == EM_key_CANCEL )		 //����ȡ����
							{
								break;
							}
						}
					}
				}
			}
			else
			{
				uckey = EA_ucKBHit();
				if ( uckey ==  EM_key_HIT )		 
				{		
					uikey = EA_uiInkey(0);
					if(uikey == EM_key_F1 )
					{
						EA_vCls();
						EA_vDisp(2, 1, "   �Ƿ��������ս�?  ");
						EA_vDisp(4, 1, "F1-ȷ��    ����-ȡ��");	
						uikey = EA_uiInkey(0);
						switch(uikey)
						{
						   case EM_key_F1:
							    if( Logout() == ok )
								{
									EA_vCls();
									EA_vDisplay(1, "   ��ӭʹ�ñ��豸   ");
									EA_vDisplay(2, "      ǩ�˳ɹ�      ");
									EA_vDisplay(3, "   ����������˳�   ");
									(void)EA_uiInkey(5);
									return;
								}
								else
								{
									EA_vCls();
									EA_vDisplay(1, "     ǩ�˲��ɹ�     ");
									EA_vDisplay(2, "     ������ǩ��     ");
									EA_vDisplay(3, "   ����������˳�   ");
									(void)EA_uiInkey(5);
									break;
								}
						   default :
								break;
						}
					}
					else
						break;
				}
			}
		}               
	}
}

/*****************************************************************
����ԭ�ͣ�INT8U Logout(void)
����������������Ϊǩ�˹���
������������
�������ƣ�	����/�����	����		����
-----------		-----------	------	   	-------
�� �� ֵ��ok  ?  notok
*****************************************************************/
INT8U Logout(void)
{
	BUS_TIME	time_data;
	INT8U	    record[100];

	if( Gprs_Upload_data() == ok ) 
	{
		Get_Time(&time_data);
		memcpy(&DevStat.binhand.down_datetime, &time_data, sizeof(BUS_TIME));

		//��ӡ�ս�����
		print_work_note();
		memcpy(record, (INT8U*)&DevStat.binhand, sizeof(DevStat.binhand));
		SaveNoteRecord(record);
		DevStat.driver_is_working = FALSE;
		memset(&DevStat.binhand, 0x00, sizeof(DevStat.binhand));
		WriteParam();

		return ok;
	}

	return notok;
}
/*****************************************************************
����ԭ�ͣ�FuncMenu
����������������Ϊ���ܲ˵���ʾ
������������
�������ƣ�	����/�����	����		����
-----------		-----------	------	   	-------
�� �� ֵ����
*****************************************************************/
void  FuncMenu()
{
	int             ret;
	long            lSubKeyRet=255;
	char            mainflag=0;

	for( ;; )//���˵�ѭ��
	{
		lSubKeyRet = browse_menu(0, (unsigned char*)"" , main_menu, TRUE );
		switch ( lSubKeyRet )
		{
//			case 0://��������
//				ret = Func_Setting();
//				if ( ret == -1 )
//				{
//					//Halt_EH0218(POWER_DELAY);			//	�ػ�
//					Halt_HD6305 ();//�� ��Դ���������ֳֻ�
					
//				}
//				break;

			case 0://��������
				ret = Parm_Setting();
				if ( ret == -1 )
				{
					//Halt_EH0218(POWER_DELAY);			//	�ػ�
//  				Halt_HD6305 ();
				}
				break;

			case 1://��Ϣ��ѯ
				ret = Information_Query();
				if ( ret == -1 )
				{
					//Halt_EH0218(POWER_DELAY);			//	�ػ�
//  				Halt_HD6305 ();
					
				}
				break;

			case 2://ͨѶ
				//ret = DataTransport();
				ret = 1;
				if ( ret == -1 )
				{
					//Halt_EH0218(POWER_DELAY);			//	�ػ�
					//Halt_HD6305 ();
					
				}
				break;
			case 3://����Ա�����޸�
				ret = OperPasswordModify();        
				if ( ret !=ok)
				{
					//Halt_EH0218(POWER_DELAY);			//	�ػ�
					printf_debug((void *)"�������ʧ��");
					
				}
				break;
			case -1://KEY_POWER,AUTO_QUIT
				return;
			case -5:
				mainflag = 1;
				break;

			default:
				break;
		}
		if ( mainflag )
		{
			break;
		}
	}
}


/******************************************************************************
 �������ƣ�SetCardUseDT
 �������������ÿ�Ƭ��Ч��
 ����������
 �������ƣ�����/�����	����		����
 
 ��  ��  ֵ��    ok(0)-�ɹ�
				 notok(1)-ʧ��
				 				   
 ��      ��	���ڸ���
 ��      �ڣ�2007-01-30
 �޸���ʷ��
		����		�޸���		�޸�����
******************************************************************************/
INT8U SetCardUseDT()

{

//	INT8U  temp_buf[20];
	BUS_TIME ltimer;
//	typ_RTC_date_rec rdr;
	
//	EA_ucClrKeyBuf();			
//	init_console(CONSOLE_CHINESE);
	EA_ucClrKeyBuf();		
	EA_vCls();

	EA_vDisp(1,1, "��Ƭ��Ч������");
	Get_Time(&ltimer);
	if (Get_Time(&ltimer) == ok)
	{
//		temp_buf[4] = temp_buf[7] = 0;
//		rdr.fields.year = atol(temp_buf);
//		rdr.fields.month = atol(temp_buf+5);
//		rdr.fields.day = atol(temp_buf+8);
		CardInfo.card_useful_dt[0] = ltimer.century;
		CardInfo.card_useful_dt[1] = ltimer.year;
		CardInfo.card_useful_dt[2] = ltimer.month;
		CardInfo.card_useful_dt[3] = ltimer.day;

//		init_console(CONSOLE_CHINESE);	
//		KEY_init(KEY_BEEP_KEY|KEY_AUTO_EL_ON|KEY_FIFO_MODE);//�������������ͱ���
//		(void)sm_setting(MSK_KEY_UP|MSK_KEY_DOWN); //��ֹ�����»�̧����ʱ����Ӧ
//		KEY_beep_mask_set(0xFFFfffff);
//		KEY_EL_mask_set(0xFFFfffff);
//		EA_ucClrKeyBuf();		
		return ok;
	}	
	else
	{
//		init_console(CONSOLE_CHINESE);	
//		KEY_init(KEY_BEEP_KEY|KEY_AUTO_EL_ON|KEY_FIFO_MODE);//�������������ͱ���
//		(void)sm_setting(MSK_KEY_UP|MSK_KEY_DOWN); //��ֹ�����»�̧����ʱ����Ӧ
//		KEY_beep_mask_set(0xFFFfffff);
//		KEY_EL_mask_set(0xFFFfffff);
		EA_ucClrKeyBuf();

		return notok;
	}
	
	return ok;
}



/******************************************************************************
 �������ƣ�GetTradeAmt
 ������������ý��׽��
 ����������
 �������ƣ�����/�����  ����        ����

 ��  ��  ֵ��    ok(0)-�ɹ�
				 notok(1)-ʧ��

 ��      �� ���ڸ���
 ��      �ڣ�2007-01-30
 �޸���ʷ��
		����        �޸���      �޸�����
******************************************************************************/
INT8U GetTradeAmt(void)
{
	INT8U flag = FALSE;
	FP32   temp_int32u = 0;
	INT8U input[50];
	INT8U buf[50];
	uchar i;
////////////////////////////////////////
//�����ֵ���
////////////////////////////////////////

	EA_vCls();

	flag = FALSE;
	EA_ucSetInverse(EM_lcd_INVOFF);
	lcddisp(1, 1, (void *)"���뽻�׽�λ(Ԫ/��)");
	lcddisp(2, 1, (void *)"���:       Ԫ      ");
	strcpy((void *)input, "");

	for ( ;; )
	{
		i = get_input_str(2, 6, 1, 7, (void *)input);
		if ( i == EM_SUCCESS )
			break;
		if ( i == EM_ABOLISH )
			return notok;
	}
	sscanf((void *)input, "%f", &temp_int32u);
	sprintf((void *)buf, "���:%4.2f  Ԫ", temp_int32u);
	lcddisp(2, 1, (void *)buf);
	EA_ucSetInverse(EM_lcd_INVOFF);

	if(temp_int32u > 0)
		temp_int32u *=100 ;
	else
		temp_int32u = 0;

	if(temp_int32u > 10000)//credit amt must >30 and <max_price_limit
	{
//  	Beep(500);
		lcddisperr("���׶�����Ч");
		Beeperr();
		return notok;
	}
	else
	{
		CardInfo.fare = temp_int32u;
		lcddisperr("���׶����������");
	}

	return ok;
}


/******************************************************************************
 �������ƣ�GetConsumeTradeAmt
 ������������ý��׽��
 ����������
 �������ƣ�����/�����	����		����
 ��  ��  ֵ��    ok(0)-�ɹ�
				 notok(1)-ʧ��				 				   
 ��      ��	���ڸ���
 ��      �ڣ�2007-01-30
 �޸���ʷ��
		����		�޸���		�޸�����
******************************************************************************/
INT8U GetConsumeTradeAmt(void)
{
	INT8U   flag = 0;
	INT8U   input[50],buf[50];
	FP32    LOCAL_V_TYPE  temp_int32u = 0;
	uchar 	i;
////////////////////////////////////////
//�����ֵ���
////////////////////////////////////////
	
//	init_console(CONSOLE_CHINESE);				

	EA_vCls();
	

//  EA_vDisp(1, 1, (unsigned char*)"�����뽻�׽��");
//  EA_vDisp(2, 1, "��λ( Ԫ )");
	input[0] = '\0';
	EA_ucSetInverse(EM_lcd_INVON);
	lcddisp(1, 1, (void *)"�����뽻�׽��      ");
	strcpy((void *)input, "���:               ");
	for ( ;; )
	{
		i = get_input_str(2, 7, 1, 7, (void *)input);
		if ( i == EM_SUCCESS )
			break;
		if ( i == EM_ABOLISH )
			return notok;
	}
	sscanf((void *)input, "%f", &temp_int32u);
	sprintf((void *)buf, "���: %4.2fԪ", temp_int32u);
	lcddisp(2, 1, (void *)buf);
	EA_ucSetInverse(EM_lcd_INVOFF);

//  temp_int32u = get_numeric(4,4,4,0);
//	if(temp_int32u > 0)
//  	temp_int32u *=100 ;
//		;
//	else
//		temp_int32u = 0;
	
	while (!flag)
	{
		if( temp_int32u==0 )//credit amt must >30 and <max_price_limit
		{
			//EA_vCls();
			//EA_vCls();
//  		Beep(500);
			EA_vDisp(3, 1, "���׽��������Ч");  
			Beeperr();
		}
		else
		{
		//	EA_vCls();
		//	EA_ucClrKeyBuf();				
			flag = 1;
			CardInfo.fare = temp_int32u;
			break;
		//	sprintf(strbuf, "���:%6.2fԪ", CardInfo.fare/100);
		//	if(select_affirm(4, strbuf)==0)
		//		break;
				
		}
		
//�ж������˳�������
	/*	ret = KEY_read();
		if(ret ==EXIT_KEY_COMM)
		{
			Beep(2000);
			EA_uiInkeyMs(200);
			init_console(CONSOLE_CHINESE);	
			KEY_init(KEY_BEEP_KEY|KEY_AUTO_EL_ON|KEY_FIFO_MODE);//�������������ͱ���
			(void)sm_setting(MSK_KEY_UP|MSK_KEY_DOWN); //��ֹ�����»�̧����ʱ����Ӧ
			KEY_beep_mask_set(0xFFFfffff);
			KEY_EL_mask_set(0xFFFfffff);
			EA_ucClrKeyBuf();	
			return notok;	
		}
		if(ret == EXIT_KEY_POWER)
		{
			Halt_HD6305();
		}
//		else
//		{
//			flag = FALSE;
//		}
*/
//��������		
//		EA_vCls();
//  	EA_vDelay(1, 1, "                   ");
//  	EA_vDelay(2, 1, "�����뽻�׽��     ");
		temp_int32u = 0;
//  	temp_int32u = get_numeric(4,4,4,0);
		strcpy((void *)input,"");
		EA_ucSetInverse(EM_lcd_INVON);
		lcddisp(1, 1, (void *)"�����뽻�׽��      ");
		strcpy((void *)input, "���:               ");
		for ( ;; )
		{
			i = get_input_str(2, 7, 1, 7, (void *)input);
			if ( i == EM_SUCCESS )
				break;
			if ( i == EM_ABOLISH )
				return notok;
		}
		sscanf((void *)input, "%f", &temp_int32u);
		sprintf((void *)buf, "���: %4.2fԪ", temp_int32u);
		lcddisp(2, 1, (void *)buf);
		EA_ucSetInverse(EM_lcd_INVOFF);
//		if(temp_int32u > 0)
//  		temp_int32u *=100 ;
//			;
//		else
//			temp_int32u = 0;
	}
//  init_console(CONSOLE_CHINESE);
//  KEY_init(KEY_BEEP_KEY|KEY_AUTO_EL_ON|KEY_FIFO_MODE);//�������������ͱ���
//  (void)sm_setting(MSK_KEY_UP|MSK_KEY_DOWN); //��ֹ�����»�̧����ʱ����Ӧ
//  KEY_beep_mask_set(0xFFFfffff);
//  KEY_EL_mask_set(0xFFFfffff);
    EA_ucClrKeyBuf();
	

//�ж��������Ƿ�Ϸ�
	if(flag == 0)//û�������ֵ���
	{
		Beeperr();
		return notok;	
	}

	EA_vCls();
	EA_vDisp(1,1,"      ��Ƭ����      ");
	EA_vDisp(2,1,"  ��źÿ�Ƭ���ƶ�  ");
	EA_vDisp(3,1,"   �����У���ȴ�   ");
	return ok;

}


/******************************************************************************
 �������ƣ�GetTradeInfo
 ������������óֿ��˽�����Ϣ
 ����������
 �������ƣ�����/�����	����		����
 
 ��  ��  ֵ��    ok(0)-�ɹ�
				 notok(1)-ʧ��
				 				   
 ��      ��	���ڸ���
 ��      �ڣ�2007-01-30
 �޸���ʷ��
		����		�޸���		�޸�����
******************************************************************************/
INT8U GetTradeInfo(void)
{
//	INT32U  LOCAL_V_TYPE  temp_int32u = 0;
//	INT8U   LOCAL_V_TYPE  temp_buf[50];
//	INT8U   			  strbuf[50];
//	int   			  	  key;
////////////////////////////////////////
//�����ֵ���
////////////////////////////////////////
	
	if( GetTradeAmt() != ok )
		return notok;
/*	
////////////////////////////////////////
//����ֿ���֤����Ϣ
////////////////////////////////////////
	if(GetPersonInfo()!=ok)
	{
		(void)EA_vCls();
		DispStr_CE(0, 2, "��Ա��Ϣ�������", DISP_POSITION);     
		DispStr_CE(0, 6, "�ۿ�ʧ��!", DISP_CENTER);   
		Beep(1000);
		return notok;	
	}
	
	EA_vCls();
	init_console(CONSOLE_CHINESE);	
	KEY_init(KEY_BEEP_KEY|KEY_AUTO_EL_ON|KEY_FIFO_MODE);//�������������ͱ���
	(void)sm_setting(MSK_KEY_UP|MSK_KEY_DOWN); //��ֹ�����»�̧����ʱ����Ӧ
	KEY_beep_mask_set(0xFFFfffff);
	KEY_EL_mask_set(0xFFFfffff);
	EA_ucClrKeyBuf();	
*/	
//  EA_vDisp(1,1,"��Ƭ�ۿ�");
//  EA_vDisp(2,1,"��źÿ�Ƭ���ƶ�");
//  EA_vDisp(3,1,"������,��ȴ�...");

	return ok;

}

///******************************************************************************
// �������ƣ�GetPersonInfo
// ������������óֿ�����Ϣ
// ����������
// �������ƣ�����/�����  ����        ����
//
// ��  ��  ֵ��    ok(0)-�ɹ�
//                 notok(0xFF)-ʧ��
//
// ��      �� ���ڸ���
// ��      �ڣ�2007-01-30
// �޸���ʷ��
//        ����        �޸���      �޸�����
//******************************************************************************/
//INT8U GetPersonInfo(void)
//{
//    unsigned char *fp_data,SendData[100]={0};
////	INT8U	Lflag=0;//��½�ɹ����ı�־
//    int     ret;
//    INT8U   flag=0;
//
//    EA_vCls();
//
//    for(;;)
//    {
//        ret = browse_menu(0, (unsigned char *)"�ֿ�����Ϣ", PersonInfo,TRUE );
//        switch(ret)
//        {
//            case 0://��������
//            case EM_key_1:
//            {
//                EA_uiInkeyMs(200);
//                EA_vCls();
//            //  EA_ucClrKeyBuf();
//            //  init_console(CONSOLE_CHINESE);
//                EA_ucClrKeyBuf();
//                memset(SendData, 0, sizeof(SendData));
//                fp_data = SendData;
//                if (1)   //gets_dialog_ch((char *)fp_data, "��������", GETS_HINT_INVERSED) == GETS_DIALG_CONFIRM
//                {
//                    //strcat(fp_data, "\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20");
//                    if(strlen((char *)fp_data) == 0)
//                    {
//                        printf_debug((void *)"������������");
//                    }
//                    else
//                    {
//                        //sprintf(strbuf, "1:%s", fp_data);
//                        //printf_debug(strbuf);
//                        EA_uiInkeyMs(10);
//                        strcat((void *)fp_data, "\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20");
//                        //memcpy(strbuf, fp_data, 8);
//                        //strbuf[8] = 0;
//                        //sprintf(strbuf, "%s", fp_data);
//                        //printf_debug(strbuf);
//                        memcpy(CardInfo.name, fp_data, 8);
//                        //memcpy(strbuf, CardInfo.name, 8);
//                        //strbuf[8] = 0;
//                        //printf_debug(strbuf);
//                        flag = 1;
//                    }
//                }
//                else
//                    flag=0;
//
//                EA_uiInkeyMs(200);
////  			init_console(CONSOLE_CHINESE);
////  			KEY_init(KEY_BEEP_KEY|KEY_AUTO_EL_ON|KEY_FIFO_MODE);//�������������ͱ���
////  			(void)sm_setting(MSK_KEY_UP|MSK_KEY_DOWN); //��ֹ�����»�̧����ʱ����Ӧ
////  			KEY_beep_mask_set(0xFFFfffff);
////  			KEY_EL_mask_set(0xFFFfffff);
//                EA_ucClrKeyBuf();
//                *fp_data = 0;
//                break;
//            }
//            case 1://�����Ա�
//            case EM_key_2:
//            {
////  			Sex_Select();
//                break;
//            }
//            case 2://����֤������
//            case EM_key_3:
//            {
//                Id_Select();
//                break;
//            }
//            case 3://����֤������
//            case EM_key_4:
//            {
//                EA_ucClrKeyBuf();
//                EA_uiInkeyMs(200);
////  			init_console(CONSOLE_CHINESE);
//                EA_ucClrKeyBuf();
//                memset(SendData, 0, sizeof(SendData));
//                fp_data=SendData;
//                if (1)    //gets_dialog_ch((char *)fp_data, "����֤������", GETS_HINT_INVERSED) == GETS_DIALG_CONFIRM
//                {
//                    if(strlen((char *)fp_data) == 0)
//                    {
//                        printf_debug((char *)"֤�������������");
//                    }
//                    else
//                    {
//                        EA_uiInkeyMs(10);
//                        strcat((char *)fp_data, "\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20\x20");
//                        //memcpy(strbuf, fp_data, 18);
//                        //strbuf[18] = 0;
//                    //  printf_debug(strbuf);
//                        memcpy(CardInfo.id_no, fp_data, 18);
//                        flag += 2;
//                    }
//                }
//
//
//                EA_uiInkeyMs(200);
////  			init_console(CONSOLE_CHINESE);
//
////  			KEY_init(KEY_BEEP_KEY|KEY_AUTO_EL_ON|KEY_FIFO_MODE);//�������������ͱ���
////  			(void)sm_setting(MSK_KEY_UP|MSK_KEY_DOWN); //��ֹ�����»�̧����ʱ����Ӧ
////  			KEY_beep_mask_set(0xFFFfffff);
////  			KEY_EL_mask_set(0xFFFfffff);
//                EA_ucClrKeyBuf();
//                *fp_data = 0;
//                break;
//            }
//            case 4://�Ƿ񱣴����
//            case EM_key_5:
//            {
//                if(!(flag & 0x01))
//                {
//                    printf_debug((void *)"������������");
//                    continue;
//                }
//                if(!(flag & 0x02))
//                {
//                    printf_debug((void *)"֤�������������");
//                    continue;
//                }
//
////  			EA_ucClrKeyBuf();
////  			EA_uiInkeyMs(200);
////  			init_console(CONSOLE_CHINESE);
////  			EA_ucClrKeyBuf();
////  			ret = select_affirm(4,"�Ƿ񱣴����?");
//                EA_uiInkeyMs(200);
////  			init_console(CONSOLE_CHINESE);
//
////  			KEY_init(KEY_BEEP_KEY|KEY_AUTO_EL_ON|KEY_FIFO_MODE);//�������������ͱ���
////  			(void)sm_setting(MSK_KEY_UP|MSK_KEY_DOWN); //��ֹ�����»�̧����ʱ����Ӧ
////  			KEY_beep_mask_set(0xFFFfffff);
////  			KEY_EL_mask_set(0xFFFfffff);
//                EA_ucClrKeyBuf();
//                if(ret==0)
//                    return ok;
//                break;
//            }
//            case -5:
//            {
//                //EA_uiInkeyMs(200);
////  			init_console(CONSOLE_CHINESE);
////
////  			KEY_init(KEY_BEEP_KEY|KEY_AUTO_EL_ON|KEY_FIFO_MODE);//�������������ͱ���
////  			(void)sm_setting(MSK_KEY_UP|MSK_KEY_DOWN); //��ֹ�����»�̧����ʱ����Ӧ
////  			KEY_beep_mask_set(0xFFFfffff);
////  			KEY_EL_mask_set(0xFFFfffff);
//                EA_ucClrKeyBuf();
//
//                return notok;
//                break;
//            }
//            case -1:
//            {
////  			Halt_HD6305();
//                break;
//            }
//            default:
//                break;
//        }
//    }
//
//
////  KEY_init(KEY_BEEP_KEY|KEY_AUTO_EL_ON|KEY_FIFO_MODE);//�������������ͱ���
////  (void)sm_setting(MSK_KEY_UP|MSK_KEY_DOWN); //��ֹ�����»�̧����ʱ����Ӧ
////  KEY_beep_mask_set(0xFFFfffff);
////  KEY_EL_mask_set(0xFFFfffff);
//    EA_ucClrKeyBuf();
//
//    return ok;
//}

/*****************************************************************
����ԭ�ͣ�OperCardCheck
����������������Ϊ����Ƿ��в���Ա���ڣ�
				//���ж��Ƿ�Ϊͬһ�ſ�
������������
�������ƣ�	����/�����	����		����
-----------		-----------	------	   	-------
�� �� ֵ��	  -1:�޿�
			  -2:ѡ��Ŀ¼����
			  -3:������֤����
			  -4:

*****************************************************************/
int OperCardCheck()
{
//    INT8U rcv_buf[100];
//    uint  rcv_length;
//    INT16U  k;
//    int     ret = 0;
//
//    //cpu����λ
//    ret = Cpucard_power_on(rcv_buf, &rcv_length);
//    if(ret == notok)
//        return -1;//�޿�
//
//    memset(rcv_buf,0,sizeof(rcv_buf));
//    rcv_length = 0;
//
//    //ѡ��1002Ŀ¼
//    k = CmdPsam(hCpuHandle, (void *)SELECT_USER_ADF2, sizeof(SELECT_USER_ADF2) ,rcv_buf, &rcv_length);
//    if ( k != ok )
//    {
//        EA_vCls();
//        lcddisperr("ѡ��Ŀ¼����");
//        return -2;
//    }
//
//    //��֤����
//    k = CmdPsam(hCpuHandle, (void *)VERIFY_PIN, sizeof(VERIFY_PIN) ,rcv_buf, &rcv_length);
//    if ( k != ok )
//    {
//        EA_vCls();
//        EA_vDisplay(1,"ѡ��Ŀ¼����");
//        EA_vDisplay(2,"�Ǳ�ϵͳ��Ƭ");
//        EA_vDisplay(3,"����������˳�");
//        EA_uiInkeyMs(500);
//        return -3;
//    }
//
//    //��ȡ15�ļ����ݣ����ж���ʲô��Ƭ����
//    k = CmdPsam(hCpuHandle, (void *)READ_15_FILE, sizeof(READ_15_FILE) ,rcv_buf, &rcv_length);
//    if ( k != ok )
//    {
//        EA_vCls();
//        EA_vDisplay(1,"��ȡ��Ϣ�ļ�����");
//        EA_vDisplay(2,"����������˳�");
//        EA_uiInkeyMs(500);
//        return -4;
//    }
//
//    if((rcv_buf[0] != 0x80))//�����Ͳ����ϣ��˳�
//    {
//        EA_vCls();
//        EA_vDisplay(1,"��Ƭ���Ͳ�����");
//        EA_vDisplay(2,"����������˳�");
//        EA_uiInkeyMs(500);
//        return -5;
//    }
//    //��Ƭ״̬���
//    if(*(rcv_buf+31) == 0x02)//�˿��Ѿ�ͣ��
//        return -6;
////����Ա���ű���
//    if(memcmp(&DevStat.cur_driver_number[1], rcv_buf+8, 3) != 0)//����ͬһ������Ա��
//        return -6;
//
//    //��ȡ16�ļ����ݣ�����ȡ���ڽ��
//    k = CmdPsam(hCpuHandle, (void *)READ_16_FILE, sizeof(READ_16_FILE) ,rcv_buf, &rcv_length);
//    if ( k != ok )
//    {
//        EA_vCls();
//        EA_vDisplay(1,"��ȡǮ���ļ�����");
//        EA_vDisplay(2,"����������˳�");
//        EA_uiInkeyMs(500);
//        return -5;
//    }
//    if(Purse_Block_Is_Right(rcv_buf) != ok)//Ǯ����ʽ����
//    {
//        EA_vCls();
//        EA_vDisplay(1,"����Ա��Ǯ����");
//        EA_vDisplay(2,"�����޷���������");
//        EA_vDisplay(3,"��ֿ������Ĵ���");
//        EA_vDisplay(4,"����������˳�");
//        EA_uiInkey(5);
//        return -5;
//    }
//    //((INT32U)rcv_buf[3]<<24) + ((INT32U)rcv_buf[2]<<16) + ((INT32U)rcv_buf[1]<<8) + (INT32U)rcv_buf[0]
//    DevStat.oper_card_amt = (((INT32U)rcv_buf[0] * 0x1000000 + (INT32U)rcv_buf[1] * 0x10000 + (INT32U)rcv_buf[2] * 0x100 + (INT32U)rcv_buf[3]) )  ;
	return ok;
}
/*****************************************************************
����ԭ�ͣ�CPU_OPER_DOWN
����������������ΪCPU����Ա���°ദ��
������������
�������ƣ�	����/�����	����		����
-----------		-----------	------	   	-------
�� �� ֵ��   ��
*****************************************************************/
void CPU_OPER_DOWN()
{
	DevStat.driver_is_working = FALSE;
	memset(DevStat.cur_driver_number, 0x00, sizeof(DevStat.cur_driver_number));
	memset(&DevStat.binhand, 0x00, sizeof(DevStat.binhand));
	WriteParam();
}


/******************************************************************************
 �������ƣ�GrayCardDeal
 ������������Ƭ��������
 ����������
 �������ƣ�����/�����	����		����
 
 ��  ��  ֵ��    ok(0)-�ɹ�
				 notok(0xFF)-ʧ��
				 0xFE-û�ҵ���
				 0xF0-����ˢ��
				   
 ��      ��	���ڸ���
 ��      �ڣ�2007-01-30
 �޸���ʷ��
		����		�޸���		�޸�����
******************************************************************************/
//INT8U GrayCardDeal(void)
//{
//
//
//    INT8U   LOCAL_V_TYPE  i = 0;
//    INT8U   LOCAL_V_TYPE  j = 0;
////	INT32U  LOCAL_V_TYPE  start_using_date = 0;				//��������
////	INT32U  LOCAL_V_TYPE  end_using_date = 0;				//��Ч��������
////	INT32U  LOCAL_V_TYPE  cur_date = 0;							//��ǰʱ��
//    INT32U  LOCAL_V_TYPE  temp_int32u = 0;
//    INT8S   LOCAL_V_TYPE  h = 0;
//
////	INT16U  LOCAL_V_TYPE  k = 0;
//
////	INT8U   LOCAL_V_TYPE  temp_rcv_buf[50];
////	INT8U   LOCAL_V_TYPE  temp_snd_buf[50];
//    INT8U   LOCAL_V_TYPE  block_buffer1[16] = {0};          //M1��һ��block4�����ݻ���
//    INT8U   LOCAL_V_TYPE  block_buffer2[16] = {0};          //M1��һ��block5�����ݻ���
//    INT8U   LOCAL_V_TYPE  block_buffer3[16] = {0};          //M1��һ��block6�����ݻ���
////	INT8U   			  strbuf[50];
////	int		key;
//    INT8U   LOCAL_V_TYPE    m = 0;
////	INT16U  LOCAL_V_TYPE    temp_int16u;
//    INT8S   LOCAL_V_TYPE    g = 0;
//    INT8U   LOCAL_V_TYPE    block9[16];
//    INT8U   LOCAL_V_TYPE    block10[16];
//
//    INT32U  LOCAL_V_TYPE    temp_balance;
//
//////////////////////////////////////////
////��Ƭ��ʼ��
//////////////////////////////////////////
//    if(CardInit() != ok)
//        return notok;
//
//////////////////////////////////////////
////�ж��Ƿ��ǳ���
//////////////////////////////////////////
//    h = memcmp(&CardInfo.csn[0], &GrayRecord.csn[0], 4);//����Ӳ�����űȽ�
//    if ( h != ok )
//    {
////  	(void)EA_vCls();
//        lcddisperr("��ˢ����");
//        Beep(1000);
//        return notok;//�����׳������صĿ�Ƭ���ǳ���
//    }
//
//    i = PsamRst();//PSAM����λ
//    if ( i != ok )
//    {
//        lcddisperr((void *)"PSAM������");
//        j = EA_ucMIFHalt(hMifsHandle);  //�ҿ�
//        return notok;
//    }
//
//////////////////////////////////////////
////�ָ�����ĳ�ֵ��Կ
//////////////////////////////////////////
//    for(i=1;i<=6;i++)
//    {
//        memcpy(&KeySector[i][0], &DevStat.KEYVALUE[i][0], 6);
//    }
//
//
//////////////////////////////////////////
////��ʾ����
//////////////////////////////////////////
//    EA_vCls();
//    EA_vDisp(1,1,"��Ƭ����");
//    EA_vDisp(2,1,"��źÿ�Ƭ���ƶ�");
//    EA_vDisp(3,1,"�����У���ȴ�");
//
//////////////////////////////////////////
////��ֵ����
//////////////////////////////////////////
//
//
//////////////////////////////////////////
////��֤Sector 6 ��Կ
//////////////////////////////////////////
//    i = Get_Sector_PermissionB(6, &KeySector[6][0]);
//    if ( i != ok )
//    {
//        return notok;
//    }
//    CardInfo.credit_rec_pos = GrayRecord.block24_buf_to_restore[8];
//
//    if(CardInfo.credit_rec_pos == 0x00 ||CardInfo.credit_rec_pos == 0xFF)
//        CardInfo.credit_rec_pos = 0x0C;
//    else
//        CardInfo.credit_rec_pos ++;
//
//    if(CardInfo.credit_rec_pos==0x0f ||CardInfo.credit_rec_pos==0x13 )
//        CardInfo.credit_rec_pos ++;
//    if(CardInfo.credit_rec_pos>=0x17)
//        CardInfo.credit_rec_pos=0x0C;
//
//////////////////////////////////////////
////write  sector 6 info
//////////////////////////////////////////
//    i = EA_ucMIFWrite(hMifsHandle,24, &GrayRecord.block24_buf_to_restore[0]);
//    if ( i != ok )
//        return notok;
//
//    i = EA_ucMIFWrite(hMifsHandle,25, &GrayRecord.block24_buf_to_restore[0]);
//    if ( i != ok )
//        return notok;
//
////�����豸�������
//    memcpy(&CardInfo.dev_trade_no[0],&DevStat.dev_trade_no[0], 2);                                      //PSAM�������������
//
//    CardInfo.psam_purchase_serial_num = 0;
//
//
//////////////////////////////////////////
////��֤Sector 2 ��Կ
//////////////////////////////////////////
//    i = Get_Sector_PermissionB(2, &KeySector[2][0]);
//    if ( i != ok )
//    {
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
//    {   //�������Ϸ��������������Ƿ�һ��
//        g = memcmp(block9, block10, 16);
//        if ( g == ok )
//        {
//            ;           //����һ�£�ʲôҲ����
//        }
//        else
//        {
//            //������һ�£���9��Ϊ��
//            m = EA_ucMIFRestore(hMifsHandle,9);
//            m = EA_ucMIFTransfer(hMifsHandle,10);
////  		m = CardMFCValueWithTransfer(PICC_INCREMENT, 9, 10,zero);	  //������һ�£���9��Ϊ��
//            if ( m != ok )
//                return notok;
//        }
//    }
//    else if ( i == ok )
//    {
////         m = Write(10, block9);              //9���Ϸ���9->10
//        m = EA_ucMIFRestore(hMifsHandle, 9);
//        m = EA_ucMIFTransfer(hMifsHandle, 10);     //9���Ϸ���9->10
//        if ( m != ok )
//            return notok;
//    }
//    else if ( j == ok )
//    {
////         m = Write(9, block10);          //10���Ϸ���10->9
//        m = EA_ucMIFRestore(hMifsHandle, 10);
//        m = EA_ucMIFTransfer(hMifsHandle, 9);     //10���Ϸ���10->9
//        if ( m != ok )
//            return notok;
//        memcpy(block9, block10, 16);        //�õ����ݴ���block9�±ߴ���������block9
//    }
//    else
//    {
//        return notok;       //���������Ϸ����˳�
//    }
//
//
////////////////////
////trans card balance
//
//    temp_balance = BlockToCash(block9);
//////////////////////////////////////////
////judge card_purchase_type
////0x77---sale; 0x88---credit; 0x99---flush
//////////////////////////////////////////
//    if((GrayRecord.card_purchase_type==TT_GRAY_SALE)||(GrayRecord.card_purchase_type==TT_GRAY_CREDIT))
//    {
//        if((CharToCash(GrayRecord.balance) + CharToCash(GrayRecord.fare))==temp_balance)//increment ok
//        {
//            CardInfo.balance = temp_balance;
////			CardInfo.init_balance = GrayRecord.balance;
//        }
//        else if(CharToCash(GrayRecord.balance) == temp_balance)
//        {
//            if(DevStat.oper_card_amt == ( ((CharToCash(GrayRecord.oper_card_amt)) * 0x100 ) + GrayRecord.oper_card_amt[3] ))
//            {
//                //�۳�����Ա���ڽ��
//                Cpucard_Init();
//                if(OperTradeProcess(0, CharToCash(GrayRecord.fare)) != 0)
//                {
//                    Cpucard_power_off();
//                    return notok;
//                }
//                Cpucard_power_off();
//
//                //CardMFCValueWithTransfer(int flag, int s_block, int d_block, int num);
//
//                i = EA_ucMIFIncrement(hMifsHandle, 9, CharToCash(GrayRecord.fare));
//                i = EA_ucMIFTransfer(hMifsHandle, 9);
//                if ( i != ok )
//                {
//                    return notok;
//                }
//                i = EA_ucMIFRestore(hMifsHandle, 9);
//                i = EA_ucMIFTransfer(hMifsHandle, 10);
//                if ( i != ok )
//                {
//                    return notok;
//                }
//                CardInfo.balance = CharToCash(GrayRecord.balance) + CharToCash(GrayRecord.fare);//���潻�׺����
////				CardInfo.init_balance = GrayRecord.balance;
//            }
//            else if((DevStat.oper_card_amt + CharToCash(GrayRecord.fare)) == (((CharToCash(GrayRecord.oper_card_amt)) * 0x100 )+ GrayRecord.oper_card_amt[3]))
//            {
//                i = EA_ucMIFIncrement(hMifsHandle,9, CharToCash(GrayRecord.fare));
//                i = EA_ucMIFTransfer(hMifsHandle, 9);
////  			i = CardMFCValueWithTransfer(PICC_INCREMENT, 9, 9,CharToCash(GrayRecord.fare));
//                if ( i != ok )
//                {
//                    return notok;
//                }
//                i = EA_ucMIFIncrement(hMifsHandle,9, 0);
//                i = EA_ucMIFTransfer(hMifsHandle, 10);
////  			i = CardMFCValueWithTransfer(PICC_INCREMENT, 9, 10, 0);
//                if ( i != ok )
//                {
//                    return notok;
//                }
//                CardInfo.balance = CharToCash(GrayRecord.balance) + CharToCash(GrayRecord.fare);//���潻�׺����
////				CardInfo.init_balance = GrayRecord.balance;
//            }
//            else
//                return notok;//error
//        }
//        else
//        {
//            return notok;//error
//        }
//    }
//    else//flush
//    {
//        if((CharToCash(GrayRecord.balance) - CharToCash( GrayRecord.fare)) == temp_balance)//increment ok
//        {
//            CardInfo.balance = temp_balance ;//���潻�׺����;
////			CardInfo.init_balance = GrayRecord.balance;
//        }
//        else if(CharToCash(GrayRecord.balance) == temp_balance)
//        {
//            if (  DevStat.oper_card_amt == (( CharToCash(GrayRecord.oper_card_amt) * 0x100 ) + GrayRecord.oper_card_amt[3] ) )
//            {
//                //�۳�����Ա���ڽ��
//                Cpucard_Init();
//                if(OperTradeProcess(1, CharToCash(GrayRecord.fare)) != 0)
//                {
//                    Cpucard_power_off();
////  				MC530Off();
////  				InitMC530(TYPE_A_ISO14443);
//                    return notok;
//                }
//                Cpucard_power_off();
//                i = EA_ucMIFDecrement(hMifsHandle, 9, CharToCash(GrayRecord.fare));
//                i = EA_ucMIFTransfer(hMifsHandle, 9);
//                if ( i != ok )
//                {
//                    return notok;
//                }
//                i = EA_ucMIFDecrement(hMifsHandle, 9, 0);
//                i = EA_ucMIFTransfer(hMifsHandle, 10);
//                if ( i != ok )
//                {
//                    return notok;
//                }
//                CardInfo.balance = CharToCash(GrayRecord.balance) - CharToCash(GrayRecord.fare);//���潻�׺����
//            }
//            else if ((DevStat.oper_card_amt - CharToCash(GrayRecord.fare)) == ((CharToCash(GrayRecord.oper_card_amt) * 0x100 ) + GrayRecord.oper_card_amt[3]))
//            {
//                i = EA_ucMIFDecrement(hMifsHandle, 9, CharToCash(GrayRecord.fare));
//                i = EA_ucMIFTransfer(hMifsHandle, 9);
////  			i = CardMFCValueWithTransfer(PICC_DECREMENT, 9, 9,CharToCash(GrayRecord.fare));
//                if ( i != ok )
//                {
//                    return notok;
//                }
//                i = EA_ucMIFDecrement(hMifsHandle,9, 0);
//                i = EA_ucMIFTransfer(hMifsHandle, 10);
////  			i = CardMFCValueWithTransfer(PICC_DECREMENT, 9, 10, 0);
//                if ( i != ok )
//                {
//                    return notok;
//                }
//                CardInfo.balance = CharToCash(GrayRecord.balance) - CharToCash(GrayRecord.fare);//���潻�׺����
//            }
//            else
//                return notok;//error
//        }
//        else
//        {
//            return notok;//error
//
//        }
//
//    }
//
//////////////////////////////////////////
////��֤Sector 3.4.5 ��Կ��д���׼�¼
//////////////////////////////////////////
//
//    m = (CardInfo.credit_rec_pos -12 )/4  +3;//������
//    i=Get_Sector_PermissionB(m, &KeySector[m][0]);
//    if ( i != ok )
//        return ok;
//    memcpy(&CardInfo.blockcreditrec[0], &CardInfo.card_in_time.year, 5);//yymmddhhmi
//    CardInfo.blockcreditrec[5] = GrayRecord.balance[0];//��Ƭ���
//    CardInfo.blockcreditrec[6] = GrayRecord.balance[1];//��Ƭ���
//    CardInfo.blockcreditrec[7] = GrayRecord.balance[2];//��Ƭ���
//
//    CardInfo.blockcreditrec[8] = GrayRecord.fare[0];//���׽��
//    CardInfo.blockcreditrec[9] = GrayRecord.fare[1];//���׽��
//    CardInfo.blockcreditrec[10] =  GrayRecord.fare[2];//���׽��
//
//    memcpy(&CardInfo.blockcreditrec[11],&DevStat.equ_id[1],2);//�豸��
//    memcpy(&CardInfo.blockcreditrec[13], &DevStat.cur_driver_number[1],3);//����Ա��
//
////  i = CardMFCWrite16Bytes(CardInfo.credit_rec_pos, &CardInfo.blockcreditrec[0]);
//    i = EA_ucMIFWrite(hSam1Handle, CardInfo.credit_rec_pos, &CardInfo.blockcreditrec[0]);
//
//////////////////////////////////////////
/////�жϻҼ�¼����
/////card_purchase_type : 0x77--sale;  0x88--credit; 0x99---flush
/////////////////////////////////////////
//
//////write  sector one info
//    if(GrayRecord.card_purchase_type==TT_GRAY_SALE)
//    {
//        //��д1����Ϣ
//        ////////////////////////////////////////
//        //��֤Sector 1 ��Կ
//        ///////////////////////////////////////
//        i = Get_Sector_PermissionB(1, &KeySector[1][0]);
//        if ( i != ok )
//        {
//            return notok;
//        }
//
//        i = EA_ucMIFRead(hMifsHandle, 4, block_buffer1);
//        if ( i != ok )
//            return notok;
//
//        i = EA_ucMIFRead(hMifsHandle, 5, block_buffer2);
//        if ( i != ok )
//            return notok;
//
//        i = EA_ucMIFRead(hMifsHandle, 6, block_buffer3);
//        if ( i != ok )
//            return notok;
//
//        //��д4��5��6����Ϣ
//        block_buffer1[12] = 0x01;//���ñ�־
//        memcpy(&block_buffer1[14], &DevStat.deposit_amt[0],2);
//
//        memcpy(&block_buffer2[0], &CardInfo.card_begin_dt[0], 4);
//        memcpy(&block_buffer2[4], &CardInfo.card_useful_dt[0],4);
//        memcpy(&block_buffer2[13], &DevStat.equ_id[0], 3);
//
//        memcpy(&block_buffer3[0], &CardInfo.card_add_dt[0], 4);
//        block_buffer3[7] = (CardInfo.fare / 0x10000 );
//        block_buffer3[8] = (CardInfo.fare % 0x10000 / 0x100 );
//        block_buffer3[9] = (CardInfo.fare % 0x100 );
//
//        block_buffer3[10] = block_buffer3[7];//��ֵ���
//        block_buffer3[11] = block_buffer3[8];
//        block_buffer3[12] = block_buffer3[9];
//
//        memcpy(&block_buffer3[13], &DevStat.cur_driver_number[1], 3);//����Ա��
//
//        //�۳�����Ա���ڽ��Ѻ��
//        if ((DevStat.oper_card_amt == ((CharToCash(GrayRecord.oper_card_amt) * 0x100) + GrayRecord.oper_card_amt[3] )) ||
//            (DevStat.oper_card_amt == ((CharToCash(GrayRecord.oper_card_amt) * 0x100) + GrayRecord.oper_card_amt[3]) - CharToCash(GrayRecord.fare)))
//        {
//            Cpucard_Init();
//            temp_int32u = DevStat.deposit_amt[0]*256 + DevStat.deposit_amt[1];
//            if(OperTradeProcess(0, temp_int32u) != 0)
//            {
//                Cpucard_power_off();
//                return notok;
//            }
//            Cpucard_power_off();
//        }
//
//        i = EA_ucMIFWrite(hSam1Handle, 6, block_buffer3);
//        if ( i != ok )
//        {
//            return notok;
//        }
//
//        i = EA_ucMIFWrite(hSam1Handle, 5, block_buffer2);
//        if ( i != ok )
//        {
//            return notok;
//        }
//
//        i = EA_ucMIFWrite(hSam1Handle,4, block_buffer1);
//        if ( i != ok )
//        {
//            return notok;
//        }
//    }
//
////save record
//    if(GrayRecord.card_purchase_type == TT_GRAY_SALE)
//    {
//        //�����ۿ���¼
//        CardInfo.card_purchase_type = TT_CARD_SALE;
//        (void)StoreRecord(&CardInfo);           //�����ۿ���¼
//    }
////�����¼
//    if((GrayRecord.card_purchase_type == TT_GRAY_SALE)||(GrayRecord.card_purchase_type == TT_GRAY_CREDIT))
//        CardInfo.card_purchase_type = TT_CREDIT_MONEY;
//    if(GrayRecord.card_purchase_type == TT_GRAY_FLUSH)
//        CardInfo.card_purchase_type = TT_NORMAL_CONSUME_MONEY;
//    CalTAC();
//    EA_ucMIFHalt(hMifsHandle);  //�ҿ�
//    (void)StoreRecord(&CardInfo);           //�����ֵ��¼
//
////��ʾ���
//    if(GrayRecord.card_purchase_type == TT_GRAY_SALE)
//        CardInfo.card_purchase_type = TT_CARD_SALE;
//    DisplayPurchaseResult(&CardInfo);   //��ʾ���׽��
//
//
//    if(GrayRecord.card_purchase_type == TT_GRAY_SALE)
//    {
//    //�����ս�Ѻ��
//        DevStat.binhand.sale_cnt++;
//        DevStat.binhand.deposit_amt += (DevStat.deposit_amt[0] *256 + DevStat.deposit_amt[1])/100;
//        DevStat.last_trade_type = TT_CARD_SALE;
//        WriteParam();
//        if(DevStat.mode==0x00)
//        {
////  		EA_ucSetStopFlag(EM_DISABLE_STOP);
//            print_sale_record();
////  		EA_ucSetStopFlag(EM_ENABLE_STOP);
//        }
//
//    }
//    else
//    {
//        if(DevStat.mode==0x00)
//        {
////  		EA_ucSetStopFlag(EM_DISABLE_STOP);
//            print_record();
////  		EA_ucSetStopFlag(EM_ENABLE_STOP);
//        }
//    }
//
////���������ϴ�
//    //�����ϴ�
////  MC530Off();
//    i = GPRS_SEND_RECORD();
//    if(i == ok)
//    {
//        DevStat.have_data_not_send_flag = FALSE;
//    }
//
//    //�ָ�״̬
//    memset(&CardInfo, 0x00, sizeof(CardInfo));      //�忨Ƭ������Ϣ
//    memset(&ErrCardInfo, 0x00, sizeof(ErrCardInfo));    //�������Ϣ
//    //DispPurchaseResultDelay = DISP_DELAY;
//    DevStat.purchase_again_req = FALSE;                 //������ˢ��������־
//    DevStat.voice_purchase_again_freq=FALSE;
//
//    return ok;
//}





