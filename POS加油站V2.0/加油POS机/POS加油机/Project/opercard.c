/******************************************************************************
Copyright 2006 	�����״�ͨ�������޹�˾
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
extern int socketID;
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

INT8U  KeySector[14][6];								//M1��������Կ��ÿ������ԿΪ6�ֽ�

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
INT8U Sec0_Block_Is_Legal(const INT8U * data_to_verify)
{
	//ToggleWD();

	//�ж����4�ֽ��Ƿ�Ϊ18E718E7,09F609F6
	if (    (*(data_to_verify + 12) == 0x09) 
		 && (*(data_to_verify + 13) == 0xF6) 
		 && (*(data_to_verify + 14) == 0x09) 
		 && (*(data_to_verify + 15) == 0xF6) )
	{
		return ok;
	}
	else
	{
		lcddisperr("0���ֽ�У��ʧ��");
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
	INT8U  i = 0;

	INT8U tac1[4];
	INT8U tac2[4];

	memset(tac1, 0x00, 4);
	memset(tac2, 0x11, 4);
	
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
	float   temp_fare;
	char    strbuf[50];
	float   fBuf = 0.0;

	EA_vCls();
	EA_ucClrKeyBuf();

	EA_vDisplay(1, "��  �� :³C-%s", cardinfo->bus_number);

	fBuf = ((float)(cardinfo->capacity)) / 100.0;
	sprintf(strbuf, "��  ��: %5.2f", fBuf);
	EA_vDisp(3, 1, strbuf);
	//EA_vDisp(4, 1, "����Ա: %02d", DevStat.cur_driver_number[0]);
	temp_fare = cardinfo->fare / 100.0;

	if( DevStat.fill_mode == 0 )
	{
		sprintf(strbuf, "��  Ʒ: ��Ȼ��");	
	}
	else if( DevStat.fill_mode == 1 )
	{
		sprintf(strbuf, "��  Ʒ: ����93#");
	}	
	else if( DevStat.fill_mode == 2 )
	{
		sprintf(strbuf, "��  Ʒ: ����97#");
	}
	else if( DevStat.fill_mode == 3 )
	{
		sprintf(strbuf, "��  Ʒ: ����");
	}	
	else
	{
		return notok;
	}
	EA_vDisp(2, 1, strbuf);
	EA_vDisp(4, 1, "��  ��: %4.2fԪ", temp_fare);

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
	uchar  uckey = NO_KEY;
	uint   uikey = NO_KEY;

	ret = ICCardProcess();   //�Ƚ��м����Ϳ�

	if( ret == 0)
	{
RESELECT:
		lcddisp(4, 1, (void *)"     ��ˢԱ����     ");

		while(1)
		{
			if( CardInit() == ok )
				break;

			uckey = EA_ucKBHit();
			if ( uckey ==  EM_key_HIT )		 //����ȡ����
			{
				uikey = EA_uiInkey(0);
				if ( uikey == EM_key_CANCEL )		 //����ȡ����
				{
					return ok;
				}
			}
		}

		ret = M1CardProcess();   //�ټ��Ա����
		if(ret == ok)
		{
			;
		}
		else
		{
			goto RESELECT;
		}
	}

	//EA_ucMIFHalt(hMifsHandle);	//�ҿ�

	return ret;//ѡ���˳�
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
//  char input[22];
//  uint i = 0;
//  uchar k = 0;
//  EA_vCls();
//  EA_vDisplay(1, "     ���ڵ�½...    ");
//  EA_vDisplay(2, "      ��ȴ�...     ");
//  SleepMs(2000);

//  if( open_gprs_module() != ok )
//  {
//  	lcddisperr("PPP��������ʧ��");
//  	return notok;
//  }
//  if( gprs_test() != ok )          //����2012.12.18
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
/*	EA_vCls();
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

//  if( gprs_test() != ok )          //����2012.12.18
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
*/
	uchar  uckey=0;
	uint   uikey=0;
	INT8U  block_buffer1[16] = {0};
	INT8U  tmp_buffer[16] = {0};
	INT8U  input[22];
	int    i = 0;

	EA_vCls();


	while(1)
	{
		EA_vDisplay(2, (void *)"    ��ˢ����Ա��    ");
		if( CardInit() == ok )
			break;

		uckey = EA_ucKBHit();
		if ( uckey ==  EM_key_HIT )		 //����ȡ����
		{
			uikey = EA_uiInkey(0);
			if ( uikey == EM_key_CANCEL )		 //����ȡ����
			{
				return ok;
			}
		}
	}

	////////////////////////////////////////
	//��Sector 1 ��4 ����Ϣ
	////////////////////////////////////////
	memcpy(&KeySector[1][0], &CardInfo.csn[0], 4);
	KeySector[1][4] = ~KeySector[1][0];
	KeySector[1][5] = ~KeySector[1][1];

	i = Get_Sector_PermissionA(1, &KeySector[1][0]);
	if ( i != ok )
	{
		lcddisperr((char *)"������ˢ��");
		return notok;
	}

	i = EA_ucMIFRead(hMifsHandle, 4, block_buffer1);
	if ( i != ok )
	{
		lcddisperr((char *)"������ˢ��");
		return notok;
	}

	if ( block_buffer1[10] != 0x01 )	  //�ж����ñ�־
	{
		//��δ���ô���
		lcddisperr((char *)"��Ƭδ����");
		Beeperr();
		return notok;
	}

	memcpy(&DevStat.oper_cardno[0], &block_buffer1[1], 3);							 //����Ա����																				 //
	memcpy(&CardInfo.card_random_num[0], &block_buffer1[4], 4);						 //����Ա����
	CardInfo.card_type = block_buffer1[11];											     //����

	if (CardInfo.card_type != 0xBB)                 //ֻ������ͨ������debug  by frank
	{
		lcddisperr((void *)"���Ǽ���Ա��!");
		return notok;
	}

	EA_vCls();
	EA_vDisplay(1, "�� �� Ա:%02X%02X%02X", DevStat.oper_cardno[0], DevStat.oper_cardno[1], DevStat.oper_cardno[2]);
	EA_vDisplay(2, "��������:            ");

	memset((void *)input, 0, sizeof(input));

	for ( ;; )
	{
		i = EA_ucGetInputStr(2, 10, 20, EM_BIG_FONT | EM_MODE_NUMBER | EM_ALIGN_LEFT | EM_SHOW_PASSWORD
							 , 8, 8, 0, (void *)input);
		if ( i == EM_SUCCESS )
			break;
		if ( i == EM_ABOLISH )
			return notok;
	}

	var_asc2bcd(&tmp_buffer[0], &input[0], 8);

	if ( memcmp((void *)tmp_buffer, &CardInfo.card_random_num[0], 4) == 0 )
	{
		;
	}
	else
	{
		lcddisp(4, 1, (void *)"     ���벻��ȷ     ");
		(void)EA_uiInkey(2);
		return notok;
	}
	WriteParam();
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
INT8U download_price_table(void)
{
	INT8U   sendbuf[1024];
	uchar   ret = 0;

	lcddisperr("�����ͼ۱�");
	EA_vCls();     
	EA_vDisplay(1, "��Ȼ�� :%2.2fԪ/����", DevStat.price[0]/100.0);
	EA_vDisplay(2, "����93#:%2.2fԪ/�� ", DevStat.price[1]/100.0);
	EA_vDisplay(3, "����97#:%2.2fԪ/�� ", DevStat.price[2]/100.0);
	EA_vDisplay(4, "��  �� :%2.2fԪ/�� ", DevStat.price[3]/100.0);
    SleepMs(2000);

	if( open_gprs_module() != ok )
	{
		lcddisperr("PPP��������ʧ��");
		return notok;
	}

	memset(sendbuf, 0x00, sizeof(sendbuf));

	sprintf((void *)sendbuf, "@START%02X%02X%02XUPYJ000",
		 DevStat.equ_id[0], DevStat.equ_id[1], DevStat.equ_id[2]);
	
	ret = GPRS_Send_Receive(sendbuf, 19);
	if( ret != ok )
	{
		EA_vCls();
		EA_vDisplay(1, "    GPRS���ݴ���    ");
		EA_vDisplay(2, "  �������ͨѶʧ��  ");	
		EA_vDisplay(3, "  �����������ٳ���  ");
		SleepMs(2000);
//  	GPRS_Close();
		return notok;
	}

	WriteParam();
	lcddisperr("�ͼ۸��³ɹ�");
//  GPRS_Close();
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
	uchar   ret=0;
	uint    package_num=0, package_index=0, package_left=0;
	INT8U	ch=0;

	memset(str, 0, sizeof(str));
//��ѯ�������汾��
	EA_vCls();
	EA_vDisplay(1, "     ͬ��������     ");
	EA_vDisplay(3, "    ��������:%04d   ", DevStat.black_name_number);
	SleepMs(2000);

	if( open_gprs_module() != ok )
	{
		lcddisperr("PPP��������ʧ��");
		return notok;
	}

	DevStat.black_flag = 0;
	
	memset(sendbuf, 0x00, sizeof(sendbuf));

	sprintf((void *)sendbuf, "@START%02X%02X%02XWHNM000",
		 DevStat.equ_id[0], DevStat.equ_id[1], DevStat.equ_id[2]);
	
	ret = GPRS_Send_Receive(sendbuf, 19);
	if( ret != ok )
	{
		EA_vCls();
		EA_vDisplay(1, "    GPRS���ݴ���    ");
		EA_vDisplay(2, "  �������ͨѶʧ��  ");	
		EA_vDisplay(3, "  �����������ٳ���  ");
		SleepMs(2000);
		return notok;
	}

//��Ҫ���°�����
	if( DevStat.black_flag == 1 )
	{
		EA_ucBlkClean ((char *)blackFileName);
		DevStat.current_record_index = 0;
		sprintf((void *)str, "  ����%04d��������  ", DevStat.black_name_number);
		EA_vDisplay(2, (void *)str);	
		SleepMs(2000);

		package_num = DevStat.black_name_number  / 190;
		package_left = DevStat.black_name_number % 190;

		//�����������أ�ǰ����������190��������
		for( package_index=0; package_index<package_num; package_index++ )
		{
			memset(sendbuf, 0, sizeof(sendbuf));
			ch = (int)((package_index*100)/(package_num+1));
			sprintf((void *)str, "���ؽ���%2d%%", ch);
			EA_vDisplay(3, (void *)str);
			//SleepMs(500);
			sprintf((void *)sendbuf, "@START%02X%02X%02XWHUP006%02d%04d", 
					DevStat.equ_id[0], DevStat.equ_id[1], DevStat.equ_id[2],
					package_index, 190 * 5);

			ret = GPRS_Send_Receive((void *)sendbuf, 25);
			if( ret != ok )
			{
				EA_vCls();
				EA_vDisplay(4, "����������ʧ��!");
				SleepMs(2000);
				//memset(DevStat.black_version, 0, 7);
				DevStat.black_name_number = 0;
				//EA_ucBlkClean ((char *)blackFileName);
				DevStat.current_record_index = 0;
				return notok;
			}
		}

		if( ret == ok )
		{
			if( package_left > 0 )
			{
				memset(sendbuf,0x00,sizeof(sendbuf));
				sprintf((void *)sendbuf, "@START%02X%02X%02XWHUP006%02d%04d", 
						DevStat.equ_id[0], DevStat.equ_id[1], DevStat.equ_id[2],
					package_num, package_left*5);
				ret = GPRS_Send_Receive(sendbuf, 25);
				if( ret == ok )
				{
					EA_vDisplay(3, "      �������      ");	
					SleepMs(2000);		
				}
				else
				{
					EA_vDisplay(4, (void *)"   ����������ʧ��   ");
					//memset(DevStat.black_version,0,7);
					DevStat.black_name_number = 0;
					//EA_ucBlkClean((char *)blackFileName);
					DevStat.current_record_index = 0;
					GPRS_Close();
					return notok;
				}
			}
			else
			{
				EA_vDisplay(3, "      �������      ");
				SleepMs(2000);
			}
			DevStat.driver_is_working = TRUE;
			WriteParam();
		}
		if( ret != ok )
		{
			EA_vCls();
			EA_vDisplay(1, "    GPRS���ݴ���    ");
			EA_vDisplay(2, "  �������ͨѶʧ��  ");	
			EA_vDisplay(3, "  ������������ʹ��  ");
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

	GPRS_Close();
	return ok;
/*
		//���һ��
		if( package_left == 0 )
		{
			black_length = (190*package_num);

		}
		else if( package_left > 0 )  		
		{
			memset(sendbuf, 0x00, sizeof(sendbuf));
			sprintf((void *)sendbuf, "@START%02X%02X%02XWHUP006%02d%04d", DevStat.equ_id[0], 
					DevStat.equ_id[1], DevStat.equ_id[2], package_num, package_left * 5);

			memset(END, 0, sizeof(END));
			crc = Cal_CRC((unsigned char*)sendbuf, 25);	
			sprintf((char*)END, "%02X%02XEND\r", ((crc >> 8)& 0xff), (crc & 0xff));
			memcpy(sendbuf + 25, END, 8);
			sendbuf[33] = '\0';

			if( write(socketID, sendbuf, 33) == -1)
			{
				lcddisperr("TCP/IP����ʧ��");
				goto Err_whilte_list;
			}	

			memset(recbuf, 0x00, sizeof(recbuf));
			retlen = read(socketID, recbuf, sizeof(recbuf));
			if( retlen < 0)
			{
				lcddisperr("TCP/IP����ʧ��3");
				goto Err_whilte_list;
			}	
			recbuf[retlen] = '\0';

			if( memcmp(recbuf+12, "WHUP", 4) == 0 )      //���°�����
			{
//  			memcpy(pblk+950*package_num, recbuf+19, package_left * 5);
				Debugprintf((void *)recbuf);
		    }
			black_length = (190*package_num+package_left);
		}

		memset(sendbuf, 0x00, sizeof(sendbuf));
		sprintf((void *)sendbuf, "@START%02X%02X%02XUPDE000", DevStat.equ_id[0], 
				DevStat.equ_id[1], DevStat.equ_id[2]);

		memset(END, 0, sizeof(END));
		crc = Cal_CRC((unsigned char*)sendbuf, 19);	
		sprintf((char*)END, "%02X%02XEND\r", ((crc >> 8)& 0xff), (crc & 0xff));
		memcpy(sendbuf + 19, END, 8);
		sendbuf[27] = '\0';

		if( write(socketID, sendbuf, 27) == -1)
		{
			lcddisperr("TCP/IP����ʧ��1");
			goto Err_whilte_list;
		}	

		memset(recbuf, 0x00, sizeof(recbuf));
		retlen = read(socketID, recbuf, sizeof(recbuf));
		if( retlen < 0)
		{
			lcddisperr("TCP/IP����ʧ��2");
			goto Err_whilte_list;
		}	
		recbuf[retlen] = '\0';
		if( (memcmp(recbuf+12, "UPDE", 4) == 0) && (memcmp(recbuf+19, "OK", 2) == 0) )
		{
			lcddisperr("      �������!     ");
			DevStat.driver_is_working = TRUE;
			
			SleepMs(1000);

			//��հ������б�
			EA_ucBlkClean ((char *)blackFileName);
			//д�����ݿ�
			EA_ucBlkOpen((void *)&blackFileName, &uiOpenID);

//  		Debugprintf((void *)pblk);

			ret = EA_ucBlkAppendBase(uiOpenID, pblk, &black_length);
//  		ret = EA_ucBlkUpdate(uiOpenID, NULL, 0, pblk, &black_length);
			if ( ret != EM_bfs_SUCCESS )
			{
				EA_vDisplay(1, "�洢������ʧ��");
				EA_vDisplay(2, "ʵ������������%ld", black_length);
				EA_vDisplay(3, "������룺0x%02X", ret);
				SleepMs(2000);
				EA_ucBlkClose(uiOpenID);
				DevStat.black_name_number = 0;
	//  		free(pblk);
				return notok;
			}

			WriteParam();
			EA_ucBlkClose(uiOpenID);
			lcddisperr("      �������      ");
	//  	free(pblk);
			return ok;
		}

	}
	else if( DevStat.black_flag == 0 )
	{
		lcddisperr("������Ϊ���°汾");
		return ok;
	}
Err_whilte_list:

	EA_vCls();
	EA_vDisplay(1, "    GPRS���ݴ���    ");
	EA_vDisplay(2, "  �������ͨѶʧ��  ");	
	EA_vDisplay(3, "  ������������ʹ��  ");
	SleepMs(2000);
	DevStat.black_name_number = 0;
//  free(pblk);
    return notok;
*/
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
			return;
		}
		else
		{
			Get_Time(&ltime); 
			memcpy(&DevStat.binhand.work_datetime, &ltime, sizeof(BUS_TIME));

			memcpy(DevStat.binhand.acnt_id, DevStat.acnt_id, 2);
			memcpy(DevStat.binhand.equ_id, DevStat.equ_id, 3);
			memcpy(&DevStat.binhand.oper_id[0], DevStat.oper_cardno, 3);

			memset(DevStat.binhand.consume_cnt, 0, 16);
			memset(DevStat.binhand.consume_cap, 0, 16);
			memset(DevStat.binhand.consume_amt, 0, 16);

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
			TextHeadDisplayTimer();

			EA_vDisplay(1, "��ɨ���Ϳ�...");
			EA_vDisplay(4, "ǩ��            ����");
//			sprintf((void *)strbuf, "�������:%3d��", DevStat.binhand.consume_cnt);		
			EA_vDisplay(2, " ");
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
						EA_vDisp(4, 1, "ȷ��            ȡ��");	
						uikey = EA_uiInkey(0);
						switch(uikey)
						{
						   case EM_key_F1:
						   case EM_key_ENTER:
							    if( Logout() == ok )
								{
									EA_vCls();
									EA_vDisplay(1, "   ��ӭʹ�ñ��豸   ");
									EA_vDisplay(2, "      ǩ�˳ɹ�      ");
									EA_vDisplay(3, "   ����������˳�   ");
									(void)EA_uiInkey(3);
									GPRS_Close();
									return;
								}
								else
								{
									EA_vCls();
									EA_vDisplay(1, "     ǩ�˲��ɹ�     ");
									EA_vDisplay(2, "     ������ǩ��     ");
									EA_vDisplay(3, "   ����������˳�   ");
									(void)EA_uiInkey(3);
									GPRS_Close();
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
����ԭ�ͣ�TextHeadDisplayTimer(void)
����������������Ϊ�ڿ�ͷ��λ����ʾʱ��
������������
�������ƣ�	����/�����	����		����
-----------		-----------	------	   	-------
�� �� ֵ��ok  ?  notok
*****************************************************************/
void TextHeadDisplayTimer(void)
{
	BUS_TIME ltime;
	char buf[22];

	Get_Time(&ltime);
	sprintf((void *)buf, "%02X%02X-%02X-%02X %02X:%02X",
			ltime.century, ltime.year, ltime.month, ltime.day, ltime.hour, ltime.minute);
	EA_vTextOut(0, 0, EM_key_FONT8X16, 0, 1, 1, (char *)buf);
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




