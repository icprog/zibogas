/******************************************************************************
Copyright 2012 	�����״�ͨ���Ӽ������޹�˾
All rights riserved.
�� �� ���� m1card.c
ģ�����ƣ� E550 ˢ���ӳ���
���ܸ�Ҫ�� ������
*****************************************************************/
#include "main.h"
#define  Debug_Com

extern DevHandle hCpuHandle;      //CPU��
extern DevHandle hMifsHandle;
extern DevHandle hSam1Handle;
extern DEV_STAT    DevStat;
ERROR_CARD_INFO ErrCardInfo;
CARD_INFO       CardInfo;
//GRAYRECORD  GrayRecord;
char send_buffer[1024];

#define _debug_m1card_      //������Ϣ��by frank ,��ʽ����ʱҪȥ��

#define MAX_REC_NUM			2000
extern INT8U     			KeySector[14][6];

//����CPU��ʹ�õı���
INT8U const    RATS[2]=                  {0xE0,0x00};
INT8U const    DESELECT[1]=              {0xC2};
INT8U const    SELECT_USER_ADF1[7]=      {0x00,0xA4,0x00,0x00,0x02,0x10,0x01};
INT8U const    SELECT_USER_ADF2[7]=      {0x00,0xA4,0x00,0x00,0x02,0x10,0x02};
INT8U const    GET_CHALLENGE[5]=         {0x00,0x84,0x00,0x00,0x04};
INT8U const    APPEND_RECORD[4]=         {0x04,0xE2,0x00,0xC8};		//C8���ܱ�ʶ0019�ļ�	2005.02.23	hwr
INT8U const    PURCHASE_INIT[5]=         {0x80,0x50,0x01,0x02,0x0B};						//���׳�ʼ��
INT8U const    PURCHASE[5]=              {0x80,0x54,0x01,0x00,0x0F};						//����
INT8U const    GET_PROVE_CODE[5]=        {0x80,0x5A,0x00,0x06,0x02};						//ȡ������֤��
INT8U const    GET_BALANCE[5]=           {0x80,0x5C,0x00,0x02,0x04};						//ȡ���
INT8U const    APPLICATION_LOCK[5]=      {0x84,0x1E,0x00,0x00,0x04};							//����
INT8U const    ADDRECORD[5]=             {0x04,0xd6,0x97,0x00,0x0f};						//д0017�ļ�
INT8U const    VERIFY_PIN[11]=			 {0x00,0x20,0x00,0x00,0x06,0x31,0x32,0x33,0x34,0x35,0x36};//��֤����
INT8U const    READ_15_FILE[5]=		 	 {0x00,0xB0,0x95,0x00,0x24};//��ȡ15�ļ�����
INT8U const    WRITE_15_FILE[5]=		 {0x00,0xD6,0x95,0x00,0x24};//��ȡ15�ļ�����

INT8U const    READ_16_FILE[5]=		 	 {0x00,0xB0,0x96,0x00,0x10};//��ȡ16�ļ�����
INT8U const    WRITE_16_FILE[5]=		 {0x00,0xD6,0x96,0x00,0x10};//��ȡ16�ļ�����

//����PSAM��ʹ�õı���
extern INT8U const    SELECT_SAM_ADF1[7];
extern INT8U const    SELECT_JDSAM_ADF1[7];

extern INT8U const    SELECT_SAM_ADF2[7];
extern INT8U const    SELECT_SAM_ADF3[7];
extern INT8U const    DES_INIT[5];
extern INT8U const    DES_CRYPT[4];
extern INT8U const    GET_MAC1[5];
extern INT8U const    VERIFY_MAC2[5];
extern INT8U const    GET_M1_KEY[5];
extern INT8U const    TAC_INIT[5];
extern INT8U const    TAC_JDINIT[5];

extern INT8U const    CAL_TAC_M1[5];
extern INT8U const    ReadPOSNumber[5];
extern INT8U const    DESCOMMUNICATION1[5];
extern INT8U const    DESCOMMUNICATION[4];
extern INT8U const    ISAM_CAL_KEY_INIT[5];
extern INT8U const    ISAM_CAL_KEY[5];

extern INT8U const   SELECT_SAM_3F01[7];		//ѡ1001Ŀ¼
extern INT8U const   GET_M1_FUZA_KEY[5];							//����M1��������Կ
extern INT8U const   TAC_FUZA_INIT[5];							//M1�����������У�TAC������ʼ��
extern INT8U const   CAL_TAC_M1_FUZA[5];							//M1�����׹����У�����TAC��
//INT8U const   code    GET_MI_FUZA_KEY_YINZI[8]= {0xD1,0xCC,0xCC,0xA8,0x00,0x00,0x00,0x00};                          //M1�����׹����У�����TAC��
extern INT8U const   GET_MI_FUZA_KEY_YINZI[8];                          //M1�����׹����У�����TAC��
extern INT8U const   FUZHA_TAC_INIT[5];							//M1�����������У�TAC������ʼ��
extern INT8U const   FUZHA_CAL_TAC_M1[5];	

extern const char 	parmFileName[];
extern const char 	blackFileName[];
extern const char 	currecFileName[];
extern const char 	conrecFileName[];
extern const char 	hisrecFileName[];
extern const char 	grayrecFileName[];

#define    PICC_DECREMENT   0
#define    PICC_INCREMENT   1
/*****************************************************************
 ����ԭ�ͣ�Get_Sector_PermissionA
 ����������ȡ�õ�N���Ķ�дȨ��
 ����������
 �������ƣ�	����/�����	����		����
 sector		����			INT8U		Ҫȡ��Ȩ�޵���
 key		����			INT8U *		Ҫȡ��Ȩ��������Կ
				
 ��  ��  ֵ��ok(0)-�ɹ�
				 notok(0xFF)-ʧ��
				   
 ��      ��	��������
 ��      �ڣ�2012-1-10
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
		20070207    yufuyong	���ӳ�ֵ��Կ��֤
*****************************************************************/
INT8U Get_Sector_PermissionA(INT8U sector, INT8U *key)
{
	uchar ucResult;

	ucResult = EA_ucMIFAuthToRam (hMifsHandle, sector, EM_mifs_KEYA, key);
	if (ucResult != EM_SUCCESS)
	{
		   /*  ������*/
		return notok;
	}
	else
	{
		// ͨ����֤
		return ok;
	}

}

/*****************************************************************
 ����ԭ�ͣ�Get_Sector_PermissionB
 ����������ȡ�õ�N���Ķ�дȨ��
 ����������
 �������ƣ�	����/�����	����		����
 sector		����			INT8U		Ҫȡ��Ȩ�޵���
 key		����			INT8U *		Ҫȡ��Ȩ��������Կ
				
 ��  ��  ֵ��ok(0)-�ɹ�
				 notok(0xFF)-ʧ��
				   
 ��      ��	��������
 ��      �ڣ�2012-12-10
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
		20070207    yufuyong	���ӳ�ֵ��Կ��֤
*****************************************************************/
INT8U Get_Sector_PermissionB(INT8U sector, INT8U *key)
{
	uchar ucResult;
	ucResult = EA_ucMIFAuthToRam(hMifsHandle, sector, EM_mifs_KEYB, key);
	if (ucResult != EM_SUCCESS)
	{
		/*  ������*/
		lcddisperr("��Կ��֤����!");
		return notok;
	}
	else
	{
		return ok;
	}
}
///******************************************************************************
// �������ƣ�M1CardConsumeProcess
// ����������M1�����ѽ�������
// ����������
// �������ƣ�	����/�����	����		����
// 
// ��  ��  ֵ��ok(0)-�ɹ�
//				 notok(0xFF)-ʧ��
//				 0xF0-����ˢ��
//				   
// ��      ��	������
// ��      �ڣ�2004-09-02
// �޸���ʷ��
//		����		�޸���		�޸�����
//		------		---------	-------------
//******************************************************************************/
//INT8U M1CardConsumeProcess(void)
//{
//	INT8U   LOCAL_V_TYPE  i = 0;
//	INT8U   LOCAL_V_TYPE  j = 0;
//	INT32U  LOCAL_V_TYPE  start_using_date = 0;				//��������
//	INT32U  LOCAL_V_TYPE  end_using_date = 0;				//��Ч��������
//	INT32U  LOCAL_V_TYPE  cur_date = 0;							//��ǰʱ��
//	INT32U  LOCAL_V_TYPE  temp_int32u = 0;
//	INT8S   LOCAL_V_TYPE  h = 0;
//	uint    LOCAL_V_TYPE  rcv_length;
//	INT16U  LOCAL_V_TYPE  k = 0;
//	INT8U   LOCAL_V_TYPE  temp_buf[50];
//	INT8U   LOCAL_V_TYPE  temp_rcv_buf[50];
//	INT8U   LOCAL_V_TYPE  temp_snd_buf[50];
//	INT8U   LOCAL_V_TYPE  block_buffer1[16] = {0};			//M1��һ��block�����ݻ���
//	INT16U  key ;
//
//	if(!ErrCardInfo.error_occured)
//	{
//		if(GetConsumeTradeAmt() != ok)
//		{
//			//EA_vCls();
////  		MC530Off();
////  		InitMC530(TYPE_A_ISO14443);
//			EA_ucMIFHalt(hMifsHandle);
//			return ok;
//		}
//	}	
//
////  MC530Off();
////  if( InitMC530(TYPE_A_ISO14443) != MI_OK)
////  	return notok;
//	
//	if(CardInit() != ok)
//		return notok;
//
//	
//
//	////////////////////////////////////////
//	//��������
//	////////////////////////////////////////
//	if ( ErrCardInfo.error_occured == TRUE )
//	{//�������룬PSAM�����ܸ�λ
//		h = memcmp(&CardInfo.csn[0], &ErrCardInfo.csn[0], 4);//����Ӳ�����űȽ�
//		if ( h != ok )
//		{
//// 			cardinfomation.return_card=1;
//// 			printf_debug("��ˢ����");
////  		(void)EA_vCls();
//			lcddisperr("��ˢ����");
//			Beep(1000);
//
//			return notok;//�����׳������صĿ�Ƭ���ǳ���
//		}
//	}
//	else
//	{
//		////////////////////////////////////////
//		//����ʱ��
//		////////////////////////////////////////
//		i = SaveCardInTime(&CardInfo);
//		if ( i != ok )
//			return notok;
//		////////////////////////////////////////
//		//PSAM����λ
//		////////////////////////////////////////
//		i = PsamRst();
//		if ( i != ok )
//		{
//			return notok;
//		}
//
//	}
//
//	////////////////////////////////////////
//	//��Sector 1 ��4 ����Ϣ
//	////////////////////////////////////////
//	memcpy(&KeySector[1][0], &CardInfo.csn[0], 4);
//	KeySector[1][4] = ~KeySector[1][0];
//	KeySector[1][5] = ~KeySector[1][1];
//
//	i = Get_Sector_PermissionA(1, &KeySector[1][0]);
//	if ( i != ok )
//		return notok;
//
//	i = EA_ucMIFRead(hMifsHandle, 4, block_buffer1);
//
//	if ( i != ok )
//		return notok;
//
//	if ( block_buffer1[12] != 0x01 )	  //�ж����ñ�־
//	{
//		//��δ���ô���
//		(void)EA_vCls();
//		EA_vDisp(3, 1, "     ��Ƭδ����!    ");         
//		Beep(1000);
////  	(void)delay_and_wait_key(3, 0, 0);
//		EA_uiInkey(3);
//		////////////////////////////////////////
//		//M1���ҿ�
//		////////////////////////////////////////
////  	j =EA_ucMIFHalt(hMifsHandle)();	//�ҿ�
//		EA_ucMIFHalt(hMifsHandle);
//		return ok;
//	}
//
//	//���濨��Ӧ��ϵ�к�
//	memcpy(&CardInfo._CARD_NUMBER.detail_card_number.city_code[0], block_buffer1, 2);	//���д���
//	memcpy(&CardInfo._CARD_NUMBER.detail_card_number.calling_code[0], block_buffer1+2, 2);	//���д���
//	memcpy(&CardInfo._CARD_NUMBER.detail_card_number.serial_number[0], &block_buffer1[4], 4);//������ˮ��
//	memcpy(&CardInfo.authentication_code[0], &block_buffer1[8], 4);//���濨��֤��
//	CardInfo.card_type = block_buffer1[13];	//������
//
//#ifdef DEBUG
//	TRACE232("read block 4 ok.\n\r");
//#endif
//
//
//	////////////////////////////////////////
//	//����ˢ�����ʱ��
//	////////////////////////////////////////
////	i=Cal_Interval();
////	if(i!=ok)
////		return notok;
//
//	////////////////////////////////////////
//	//��������
//	////////////////////////////////////////
//	if ( ErrCardInfo.error_occured == TRUE )
//	{//�������룬PSAM�����ܸ�λ
//		h = memcmp(&CardInfo._CARD_NUMBER.detail_card_number.serial_number[0], &ErrCardInfo.m1_card_number[0], 4);	//�������űȽ�
//		if ( h != ok )
//		{
//			return notok;//�����׳������صĿ�Ƭ���ǳ���
//		}
//	}
//
//// #ifdef USE_TEST_PSAM
//	////////////////////////////////////////
//	//PSAM��ѡ1001Ŀ¼
//	////////////////////////////////////////
//	k = CmdPsam(hSam1Handle, (void *)SELECT_SAM_ADF1, sizeof(SELECT_SAM_ADF1), 
//				temp_buf, &rcv_length);		//ѡ1001Ŀ¼
//	if ( k != ok )
//		return notok;
//// #else
//// 	////////////////////////////////////////
//// 	//PSAM��ѡ1003Ŀ¼
//// 	////////////////////////////////////////
//// 	k = CmdPsam(DevStat.cur_psam_channel, (void *)SELECT_SAM_ADF3, sizeof(SELECT_SAM_ADF3),
//// 				temp_buf, &rcv_length);		//ѡ1001Ŀ¼
//// 	if ( k != 0x9000 )
//// 		return notok;
//// #endif
//
//	////////////////////////////////////////
//	//PSAM���������Կ
//	////////////////////////////////////////
//	memcpy(temp_snd_buf, GET_M1_KEY, sizeof(GET_M1_KEY));//ͷ
//	memcpy(&temp_snd_buf[5], &CardInfo._CARD_NUMBER.detail_card_number.city_code[0], 2);//���д���
//	memcpy(&temp_snd_buf[7], &CardInfo.csn[0], 4);//CSNӲ������
//	memcpy(&temp_snd_buf[11], &CardInfo._CARD_NUMBER.detail_card_number.serial_number[2], 2);//��������ˮ�ź�2�ֽ�
//	memcpy(&temp_snd_buf[13], &CardInfo.authentication_code[0], 4);	//����֤��
//	temp_snd_buf[17] = 0x02;
//	temp_snd_buf[18] = 0x06;
//	temp_snd_buf[19] = 0x0a;
//	temp_snd_buf[20] = 0x0b;
//	temp_snd_buf[21] = 0x0c;//����sector2��6,10,11,12����Կ
//	k = CmdPsam(hSam1Handle, temp_snd_buf, 22, temp_rcv_buf, &rcv_length);//��MAC1
//
//	if ( k != ok )
//		return notok;
//	memcpy(&KeySector[2][0], &temp_rcv_buf[0], 6);	//������������Կ��2sector
//	memcpy(&KeySector[6][0], &temp_rcv_buf[6], 6);//������������Կ��6sector
//	memcpy(&KeySector[10][0], &temp_rcv_buf[12], 6);//������������Կ��10sector
//	memcpy(&KeySector[11][0], &temp_rcv_buf[18], 6);//������������Կ��11sector
//	memcpy(&KeySector[12][0], &temp_rcv_buf[24], 6);//������������Կ��12sector#ifdef DEBUG
////#ifdef DEBUG
////    TRACE232("psam cal key ok.\n\r");
////#endif
//
//
//	////////////////////////////////////////
//	//�������
//	////////////////////////////////////////
//	memcpy(&temp_int32u, &CardInfo._CARD_NUMBER.detail_card_number.serial_number[0], 4);
//	i = BlackName_Find(temp_int32u);	//�������ж�
//	if ( i == ok )
//	{	//�����Ǻ�������
////#ifdef DEBUG
////        TRACE232("find black name card!\n\r");
////#endif
//		i = Lock_M1_Card_Process();//����������������
//		return i;
//	}
//
//	////////////////////////////////////////
//	//��Sector 1 ��5 ����Ϣ
//	////////////////////////////////////////
//	i = EA_ucMIFRead(hMifsHandle, 5, block_buffer1);
//	if ( i != ok )
//		return notok;
//
//	memcpy(&start_using_date, block_buffer1, 4);//��������
//	memcpy(&end_using_date, block_buffer1+4, 4);//��Ч��������
//
//	//����Ч���ж�
//	memcpy((void *)&cur_date, (void *)&CardInfo.card_in_time.century, 4);//ȡ��ǰ��avoid warning
//	i = CheckPeriodOfValidity(start_using_date, cur_date, end_using_date);	//��Ч���ж�
//	if ( i != ok )
//	{//���ڴ�����������
//		//printf_debug("����Ч��");
//		EA_vDisp(4, 1, "  �˿�����,������?  ");
////  	if(select_affirm(4,"�˿�����,������?")!=0)
//		key = EA_uiInkey(0);
//		switch(key)
//		{
//		   case EM_key_ENTER:
//			   		Beep(500);
////  		j = EA_ucMIFHalt(hMifsHandle)();	//�ҿ�
////  				EA_ucMIFHalt(hMifsHandle);
//		//DispPurchaseResultDelay = DISP_DELAY; 			
//					break;
//		   case EM_key_CANCEL:
//				EA_ucMIFHalt(hMifsHandle);
//				return ok;
//		}
//	}
//
//
//	CardInfo.card_purchase_type = TT_NORMAL_CONSUME;//��ͨ����������
//	
//	////////////////////////////////////////
//	//���ֿ����Ӵ�������
//	////////////////////////////////////////
//
//	switch ( CardInfo.card_type )
//	{
//		case CARDT_NORMAL:		//��ͨ��
//		case CARDT_STUDENT:	//Сѧ����
//// 		case CARDT_STUDENT_B:	//��ѧ����
//// 		case CARDT_MONTH:		//��Ʊ��
//		case CARDT_MEMORY:		//���
//
//		case CARDT_OLDMAN:		//���꿨
//		case CARDT_FREE:	//������
//			i = M1_Card_Consume_ProcessA();
//			if ( i == ok )		  //�ۿ�ɹ�
//			{
//				CardInfo.card_purchase_type = TT_NORMAL_CONSUME;//��ͨ����������
//				
//				CalConsumeTAC();		//����TAC��
//				EA_ucMIFHalt(hMifsHandle);
//				
//				(void)StoreConsumeRecord(&CardInfo);			//�������Ѽ�¼
//
//				DisplayPurchaseResult(&CardInfo);	//��ʾ���׽��
//
//			//��¼���һ�εĽ�������
//			//	DevStat.last_trade_type = TT_NORMAL_CONSUME;
//				DevStat.gray_flag = FALSE;
//				DevStat.binhand.consume_cnt++;
//				DevStat.binhand.consume_amt += ((float)(CardInfo.fare))/100;
//				WriteParam();
//				
//				if(DevStat.mode==0x00)
//					print_consume_record();
//
//				//�ָ�״̬
//				memset(&CardInfo, 0x00, sizeof(CardInfo));		//�忨Ƭ������Ϣ
//				memset(&ErrCardInfo, 0x00, sizeof(ErrCardInfo));	//�������Ϣ
//				//DispPurchaseResultDelay = DISP_DELAY;
//				DevStat.purchase_again_req = FALSE; 				//������ˢ��������־
//				DevStat.voice_purchase_again_freq=FALSE;
//			//	   halt_req = TRUE;
//
//#ifdef  DEBUG
//				TRACE232("ICCardProcess over.\n\r");
//#endif
////			//�����ϴ�		
////				MC530Off();
////				i = GPRS_SEND_RECORD();
////				if(i == ok)
////				{
////					DevStat.have_data_not_send_flag = FALSE;
////					WriteParam();
////				}
//				
//			//		MC530Off();
//			//		GPRSProcess();
//				return ok;
//			}
//			break;
//
//		default:
//			printf_debug((void *)"������������");
//			(void)EA_ucMIFHalt(hMifsHandle);	//�ҿ�
//			return ok;
//	}
//
//	j = EA_ucMIFHalt(hMifsHandle);	//�ҿ�
//
//	if ( i != ok )
//		return i;
//
////     if((CardInfo.card_purchase_type==COMMON_CONSUME_TYPE)
////     ||(CardInfo.card_purchase_type==FAVOURABLE_CONSUME_TYPE)
////     ||(CardInfo.card_purchase_type==ZERO_CONSUME_TYPE))
////     {
////         //��3���������tac  modified by myron for dandong flow 2005.2.18
////         CalTAC();       //����TAC��
////     }
////     else
////     {
////     }
//
//	return ok;
//}
//

/******************************************************************************
 �������ƣ�M1CardProcess
 ����������M1����������
 ����������
 �������ƣ�	����/�����	����		����
 
 ��  ��  ֵ��ok(0)-�ɹ�
				 notok(0xFF)-ʧ��
				 0xF0-����ˢ��
				   
 ��      ��	������
 ��      �ڣ�2004-09-02
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
******************************************************************************/
INT8U M1CardProcess(void)
{
	INT8U     i = 0;
	INT32U    start_using_date = 0;				//��������
	INT32U    end_using_date = 0;				//��Ч��������
	INT32U    cur_date = 0;						//��ǰʱ��
	INT32U    temp_int32u = 0;
	INT8S     h = 0;
	uint      rcv_length;
	INT16U    k = 0;
	INT8U     temp_rcv_buf[50];
	INT8U     temp_snd_buf[50];
	INT8U     block_buffer1[16] = {0};			//M1��һ��block�����ݻ���

#ifndef Debug_M1card
	////////////////////////////////////////
	//��������
	////////////////////////////////////////
	if ( ErrCardInfo.error_occured == TRUE )
	{//�������룬PSAM�����ܸ�λ
		h = memcmp(&CardInfo.csn[0], &ErrCardInfo.csn[0], 4);//����Ӳ�����űȽ�
		if ( h != ok )
		{
			lcddisperr("��ˢ����");
			Beeperr();
			return notok;//�����׳������صĿ�Ƭ���ǳ���
		}
	}
	else
	{
		////////////////////////////////////////
		//����ʱ��
		////////////////////////////////////////
		i = SaveCardInTime(&CardInfo);
		if ( i != ok )
		{
			lcddisperr("ʱ�����");
			return notok;
		}
			
	}

	 i = PsamRst();
	 if ( i != ok )
	 {
		 lcddisperr((char *)"PSAM������");
		 return notok;
	 }
	
	////////////////////////////////////////
	//��Sector 1 ��4 ����Ϣ
	////////////////////////////////////////
	memcpy(&KeySector[1][0], &CardInfo.csn[0], 4);
	KeySector[1][4] = ~KeySector[1][0];
	KeySector[1][5] = ~KeySector[1][1];
	memset(&KeySector[1][0],0xFF,6);

	i = Get_Sector_PermissionA(1, &KeySector[1][0]);
	if ( i != ok )
	{
		lcddisperr((char *)"��Կ1������ʧ��");
		return notok;
	}
		
	i = EA_ucMIFRead(hMifsHandle, 4, block_buffer1);
	if ( i != ok )
	{
		lcddisperr((char *)"��Կ2������ʧ��");
		return notok;
	}
		

	if ( block_buffer1[12] != 0x02 )	  //�ж����ñ�־
	{
		//��δ���ô���
		lcddisperr((char *)"��Ƭδ����");
		Beeperr();
		return ok;
	}

	//���濨��Ӧ��ϵ�к�
	memcpy(&CardInfo._CARD_NUMBER.detail_card_number.city_code[0], block_buffer1, 2);	//���д���
	memcpy(&CardInfo._CARD_NUMBER.detail_card_number.calling_code[0], block_buffer1+2, 2);	//���д���
//  memcpy(&CardInfo._CARD_NUMBER.detail_card_number.serial_number[0], &block_buffer1[4], 4);//������ˮ��
	memcpy(&CardInfo.authentication_code[0], &block_buffer1[8], 4);//���濨��֤��
	CardInfo.card_type = block_buffer1[13];	//������
#ifdef _debug_m1card_
	EA_vDisplay(3, "���Ϳ�����:%02x", CardInfo.card_type);
	SleepMs(2000);
#endif
//  if (CardInfo.card_type != 0x00)                 //ֻ������ͨ������debug  by frank
//  {
//  	lcddisperr((void *)"������ͨ���Ϳ�!");
//  	return notok;
//  }

		////////////////////////////////////////
	//��Sector 1 ��5 ����Ϣʱ�����Ϣ
	////////////////////////////////////////
	i = EA_ucMIFRead(hMifsHandle, 5, block_buffer1);
	if ( i != ok )
	{
//  	lcddisperr((char *)"��Կ3������ʧ��");
		return notok;
	}

	memcpy(&start_using_date, block_buffer1+8, 4);//��������
	memcpy(&end_using_date, block_buffer1+4, 4);//��Ч��������
	memcpy(&cur_date, &CardInfo.card_in_time, 4);//ȡ��ǰ����

	if ( cur_date < start_using_date )
	{
		//��δ���ô���
		lcddisperr("��Ƭδ����!");         
		Beeperr();
		return notok;
	}
	////////////////////////////////////////
	//PSAM��ѡ3F01Ŀ¼
	////////////////////////////////////////
	k = CmdPsam(hSam1Handle, (INT8U *)SELECT_SAM_3F01, sizeof(SELECT_SAM_3F01), 
				temp_rcv_buf, &rcv_length);		//ѡ3F01Ŀ¼
	if ( k != ok )
	{
//  	lcddisperr("PSAM��ѡ3F01Ŀ¼����");
		return notok;
	}
	////////////////////////////////////////
	//PSAM���������Կ
	////////////////////////////////////////
	memcpy(temp_snd_buf, (INT8U *)GET_M1_FUZA_KEY, sizeof(GET_M1_FUZA_KEY));//ͷ
	memcpy(&temp_snd_buf[5], &CardInfo._CARD_NUMBER.detail_card_number.city_code[0], 2);//���д���
	memcpy(&temp_snd_buf[7], &CardInfo.csn[0], 4);//CSNӲ������
	temp_snd_buf[11]=0x00;
	temp_snd_buf[12]=0x00;
	memcpy(&temp_snd_buf[13], &CardInfo.authentication_code[0], 4);	//����֤��
	temp_snd_buf[17] = 0x02;
	temp_snd_buf[18] = 0x03;
	temp_snd_buf[19] = 0x04;
	temp_snd_buf[20] = 0x05;
	temp_snd_buf[21] = 0x06;//����sector2��6,8,11,12����Կ
	memcpy(&temp_snd_buf[22], (INT8U *)GET_MI_FUZA_KEY_YINZI, sizeof(GET_MI_FUZA_KEY_YINZI));//ͷ
	k = CmdPsam(hSam1Handle, temp_snd_buf, 30, temp_rcv_buf, &rcv_length);//��MAC1
	if ( k != ok )
	{
//  	lcddisperr("PSAM����MAC1����!");
		return notok;
	}
	
	memcpy(&KeySector[2][0], &temp_rcv_buf[0], 6);	//������������Կ��2sector
	memcpy(&KeySector[3][0], &temp_rcv_buf[6], 6);  //������������Կ��6sector
	memcpy(&KeySector[4][0], &temp_rcv_buf[12], 6); //������������Կ��10sector
	memcpy(&KeySector[5][0], &temp_rcv_buf[18], 6); //������������Կ��11sector
	memcpy(&KeySector[6][0], &temp_rcv_buf[24], 6); //������������Կ��12sector

	memcpy(temp_snd_buf, (INT8U *)GET_M1_FUZA_KEY, sizeof(GET_M1_FUZA_KEY));//ͷ
	temp_snd_buf[4]=0x15;
	memcpy(&temp_snd_buf[5], &CardInfo._CARD_NUMBER.detail_card_number.city_code[0], 2);//���д���
	memcpy(&temp_snd_buf[7], &CardInfo.csn[0], 4);//CSNӲ������
	temp_snd_buf[11]=0x00;
	temp_snd_buf[12]=0x00;
	memcpy(&temp_snd_buf[13], &CardInfo.authentication_code[0], 4);	//����֤��
	temp_snd_buf[17] = 0x08;
	memcpy(&temp_snd_buf[18], (INT8U *)GET_MI_FUZA_KEY_YINZI, sizeof(GET_MI_FUZA_KEY_YINZI));//ͷ
	k = CmdPsam(hSam1Handle, temp_snd_buf, 26, temp_rcv_buf, &rcv_length);//��MAC1
	if ( k != ok )
	{
//  	lcddisperr("PSAM����MAC2����!");
		return notok;
	}
	memcpy(&KeySector[8][0], &temp_rcv_buf[0], 6);	//������������Կ��2sector
	//��Կ�������

	//������
	i = Get_Sector_PermissionA(8, &KeySector[8][0]);
	if ( i != ok )
	{
//  	lcddisperr((char *)"��Կ4������ʧ��");
		return notok;
	}
	i = EA_ucMIFRead(hMifsHandle, 33, block_buffer1);
	if ( i != ok )
	{
//  	lcddisperr((char *)"��Կ5������ʧ��");
		return notok;
	}
	memcpy(&CardInfo._CARD_NUMBER.detail_card_number.calling_code[0], block_buffer1+4, 2);//��ҵ����
	CardInfo._CARD_NUMBER.detail_card_number.calling_code[0]=0x31;
	memcpy(&CardInfo._CARD_NUMBER.detail_card_number.serial_number[0], &block_buffer1[6], 4);//������ˮ��


	////////////////////////////////////////
	//��������
	////////////////////////////////////////
	if ( ErrCardInfo.error_occured == TRUE )//��������
	{
		h = memcmp(&CardInfo._CARD_NUMBER.detail_card_number.serial_number[0], &ErrCardInfo.m1_card_number[0], 4);	//�������űȽ�
		if ( h != ok )
		{
			lcddisperr("��ˢ����!");
			return notok;//�����׳������صĿ�Ƭ���ǳ���
		}
	}

	//////////////////////////////////////////////////////////////
	//��������˿���û�б������ӿ���õ�������
	//////////////////////////////////////////////////////////////
#ifdef Debug_Com
	sprintf(send_buffer, "\r\n����Ӳ����:%02X%02X%02X%02X\r\n",
			CardInfo.csn[0], CardInfo.csn[1], CardInfo.csn[2], CardInfo.csn[3]);
	Debugprintf(send_buffer);
	sprintf(send_buffer, "\r\n�������к�:%02X%02X%02X%02X\r\n",
			CardInfo._CARD_NUMBER.detail_card_number.serial_number[0], CardInfo._CARD_NUMBER.detail_card_number.serial_number[1],
				CardInfo._CARD_NUMBER.detail_card_number.serial_number[2], CardInfo._CARD_NUMBER.detail_card_number.serial_number[3]);
	Debugprintf(send_buffer);
#endif
	memcpy(&temp_int32u, &CardInfo._CARD_NUMBER.detail_card_number.serial_number[0], 4);
	i = BlackName_Find(temp_int32u);	//�������ж�
	if ( i != ok )	 					//���ǰ�����
	{
		lcddisperr("��Ч��!");
		Beeperr();
		return i;
	}

	lcddisperr("��Ч��!");
	
	//////////////////////////////////////////////////////////////
	//�鿴�˿������Ƿ���ˢ��
	//////////////////////////////////////////////////////////////
//  i = Cal_Interval();
//  if( i !=ok )
//  {
//  	lcddisperr("�˿������Ѽӹ���!");
//  	Beeperr();
//  	return i;
//  }

	CardInfo.card_purchase_type = TT_NORMAL_CONSUME;    		//��ͨ����������

#endif

	i = M1_Card_Consume_Process();
	if ( i == ok )		    //�ۿ�ɹ�
	{    
		if ( DisplayPurchaseResult(&CardInfo) == ok )  			//��ʾ���׽��
		{
			CalTAC();		  //����TAC��
			EA_ucMIFHalt(hMifsHandle);
			(void)StoreRecord(&CardInfo);			   			//�������Ѽ�¼
			//��ӡ���ν�������
			print_sale_record();
			//�ָ�״̬
			memset(&CardInfo, 0x00, sizeof(CardInfo));			//�忨Ƭ������Ϣ
			memset(&ErrCardInfo, 0x00, sizeof(ErrCardInfo));	//�������Ϣ
			//DispPurchaseResultDelay = DISP_DELAY;
			DevStat.purchase_again_req = FALSE;					//������ˢ��������־
			DevStat.voice_purchase_again_freq = FALSE;
			return ok;
		}
	}

	lcddisperr("����δ���!");

	return i;
}

/******************************************************************************
 �������ƣ�StoreRecord
 �������������׼�¼�洢����
 �������ƣ�	����/�����	����		����
cardinfo	����			CARD_INFO*	������Ϻ�Ŀ�Ƭ��������Ϣ
				
 ��  ��  ֵ����
 
 ��      ��	������
 ��      �ڣ�2004-09-23
 �޸���ʷ��
		����		�޸���		�޸�����
		2005.2.18		myron	                     in function 
******************************************************************************/
INT8U StoreRecord(CARD_INFO *cardinfo)
{
	INT8U LOCAL_V_TYPE i = 0;
	INT8U record[60];
	INT32U temp_int32u = 0;
//	INT8S temp[100];
	INT16U crc = 0;

	RECORD * prec = (RECORD*)record;
	
	memset(record, 0x00, sizeof(record));


	//////////////////////////////////////////
	//��֯���׼�¼��ʽ����ͬ����
	//////////////////////////////////////////
	prec->card_type = cardinfo->card_type;//������1�ֽ�
	prec->card_purchase_type = cardinfo->card_purchase_type;//�������� 1�ֽ�	

	temp_int32u = query_rec_num();
//  temp_int32u = DevStat.record_number;
	prec->pos_purchase_serial_num[0] = (INT8U)(temp_int32u >> 16);	//�ն˻��������
	prec->pos_purchase_serial_num[1] = (INT8U)(temp_int32u >> 8);
	prec->pos_purchase_serial_num[2] = (INT8U)temp_int32u;

//  memcpy(&prec->card_serial_number[0], &cardinfo->_CARD_NUMBER.card_number[0], 8);//���д���2�ֽ�+��ҵ����2�ֽ�+��������ˮ��4�ֽ�
    memcpy(&prec->bus_number[0], &cardinfo->bus_number[0], 5);//���д���2�ֽ�+��ҵ����2�ֽ�+��������ˮ��4�ֽ�
	memcpy((void *)&prec->card_in_time, &cardinfo->card_in_time, 7);//��������4�ֽں�ʱ�� 3�ֽ�
	memcpy(&prec->tac[0], &cardinfo->tac[0], 4);//TAC�� 4�ֽ�

//  memcpy(&prec->balance[0], ((INT8U *)(&cardinfo->balance))+1, 3); //���׺���� 3�ֽ�
	memcpy(&prec->fare[0], (INT8U *)(&cardinfo->fare), 3);	    //������� 3�ֽ�
	memcpy(&prec->purchase_serial_num[0], ((INT8U *)(&cardinfo->purchase_serial_num[0])), 2); //�û����ѻ�������� 2�ֽ�

	memcpy(&prec->acnt_id[0], &DevStat.acnt_id[0], 2);
//  memcpy(&prec->spot_id[0], &DevStat.spot_id[0], 2);
	memcpy(&prec->equ_id[0], &DevStat.equ_id[0], 3);
	memcpy(&prec->fuel_type, &DevStat.fill_mode, 1);

	memcpy(&prec->isam_no[0], &DevStat.isam_no[0], 6);
	memcpy(&prec->driver_id[0], &DevStat.cur_driver_number[0],4);

	crc = Cal_CRC(record, 49);  

	record[49] = ((crc >> 8)& 0xff);//CRC1
	record[50] = (crc & 0xff);//CRC2

	i = SaveCardRecord(record);		

	DevStat.binhand.consume_cnt++;
	DevStat.binhand.consume_amt+=cardinfo->fare;

	WriteParam();
	
	return ok;
}
/******************************************************************************
 �������ƣ�void Cal_Interval()
 ����������
 ����������
 �������ƣ�	����/�����	����		����
 none
 
 ��  ��  ֵ��ok or notok
   
 ��      ��	��myron
 ��      �ڣ�2005.4.21
 �޸���ʷ��
		����		�޸���		�޸�����

******************************************************************************/
INT8U Cal_Interval(void)
{
     INT16U LOCAL_V_TYPE k = 0;
     INT8U  temp_buf[55];
     INT8S LOCAL_V_TYPE h = 0;

	 DevStat.record_number = query_rec_num();
     ////////////////////////////////////////
     //����ˢ�����ʱ��
     ////////////////////////////////////////
     for(k=0; k<DevStat.record_number; k++)
     {
         Get_Record(temp_buf, 0, (DevStat.record_number - k)); //ȡ�����n����¼
         h=memcmp(&CardInfo.card_in_time, &temp_buf[36], 4);   //�Ƚ�ʱ��
         if(h == ok)
         {//�� ���� ���
             
             h=memcmp(&CardInfo._CARD_NUMBER.card_number[0], &temp_buf[22], 8);
             if(h==ok)
             {//�������
                 return notok;
             }
             else
             {//������������ѯ
                 ;
             }
         }
         else
         {//���һ������
             break;
         }
     }
	 return ok;
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
void Get_Record(INT8U *l_rec_buf, INT16U start_position, INT16U rec_pointer)
{
	INT8U p[60];
	uchar ucOpenID = 0;
	uint  uiRetNum = 0;

	memset(p, 0, sizeof(p));

	if ( EA_ucPFOpen((uchar *)currecFileName, &ucOpenID) != EM_ffs_SUCCESS )
	{
		lcddisperr("�򿪼�¼�ļ�ʧ��");
		return ;
	}
	if ( EA_ucPFReadRec(ucOpenID, rec_pointer, 1, &uiRetNum, p) != EM_ffs_SUCCESS )
	{
		lcddisperr("��ȡ��¼�ļ�ʧ��");
		return ;
	}

	(void)EA_ucPFClose(ucOpenID);

	memcpy(l_rec_buf, p, 49);
}
/******************************************************************************
 �������ƣ�M1CardQuery
 ����������M1����Ϣ��ѯ
 ����������
 �������ƣ�	����/�����	����		����
 
 ��  ��  ֵ��ok(0)-�ɹ�
				 notok(0xFF)-ʧ��
				   
 ��      ��	������
 ��      �ڣ�2004-09-02
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
******************************************************************************/
INT8U M1CardQuery(void)
{
	INT8U   LOCAL_V_TYPE  i = 0;
	INT32U  LOCAL_V_TYPE  start_using_date = 0;				//��������
	INT32U  LOCAL_V_TYPE  end_using_date = 0;				//��Ч��������
	INT32U  LOCAL_V_TYPE  cur_date = 0;							//��ǰʱ��
	uint    LOCAL_V_TYPE  rcv_length;
	INT16U  LOCAL_V_TYPE  k = 0;
	INT8U   LOCAL_V_TYPE  temp_rcv_buf[50];
	INT8U   LOCAL_V_TYPE  temp_snd_buf[50];
	INT8U   LOCAL_V_TYPE  block_buffer1[16] = {0};			//M1��һ��block�����ݻ���
	
//  if(CardInit() != ok)
//  {
//  	return notok;
//  }

	////////////////////////////////////////
	//��Sector 1 ��4 ����Ϣ
	////////////////////////////////////////
	memcpy(&KeySector[1][0], &CardInfo.csn[0], 4);
	KeySector[1][4] = ~KeySector[1][0];
	KeySector[1][5] = ~KeySector[1][1];

	i = Get_Sector_PermissionA(1, &KeySector[1][0]);
	if ( i != ok )
	{
//  	lcddisperr("��Կ��֤����!");
		EA_ucMIFHalt (hMifsHandle);	//�ҿ�
		return notok;
	}
			
	i = EA_ucMIFRead(hMifsHandle, 4, block_buffer1);
	if ( i != ok )
	{
//  	lcddisperr("��4���ļ�����!");
		EA_ucMIFHalt (hMifsHandle);	//�ҿ�
		return notok;
	}

	if ( block_buffer1[12] != 0x01 )	  //�ж����ñ�־
	{
		//��δ���ô���
//  	Beep(500);
		lcddisperr("��Ƭδ����");
		Beeperr();
		EA_ucMIFHalt(hMifsHandle);
		return ok;
	}
	//���濨��Ӧ��ϵ�к�
	memcpy(&CardInfo._CARD_NUMBER.detail_card_number.city_code[0], block_buffer1, 2);	//���д���
	memcpy(&CardInfo._CARD_NUMBER.detail_card_number.calling_code[0], block_buffer1+2, 2);	//���д���
// 	DevStat.cur_psam_channel=block_buffer1[2];
	memcpy(&CardInfo._CARD_NUMBER.detail_card_number.serial_number[0], &block_buffer1[4], 4);//������ˮ��
	memcpy(&CardInfo.authentication_code[0], &block_buffer1[8], 4);//���濨��֤��
	CardInfo.card_type = block_buffer1[13];	//������

	////////////////////////////////////////
	//��Sector 1 ��5 ����Ϣ
	////////////////////////////////////////
	i = EA_ucMIFRead(hMifsHandle, 5, block_buffer1);
	if ( i != ok )
	{
//  	lcddisperr("��5������");
		EA_ucMIFHalt (hMifsHandle);	//�ҿ�
		return notok;
	}

	memcpy(&start_using_date, block_buffer1, 4);//��������
	memcpy(&end_using_date, block_buffer1+4, 4);//��Ч��������

	////////////////////////////////////////
	//�ж���Ч��
	////////////////////////////////////////

	//����Ч���ж�
	memcpy((void *)&cur_date, (void *)&CardInfo.card_in_time.century, 4);//ȡ��ǰ��avoid warning
	i = CheckPeriodOfValidity(start_using_date, cur_date, end_using_date);	//��Ч���ж�
	if ( i != ok )
	{	//���ڴ�����������
		lcddisperr("�˿��Ѿ�����");
		EA_ucMIFHalt (hMifsHandle);	//�ҿ�
		return ok;
	}

	////////////////////////////////////////
	//PSAM���������Կ
	////////////////////////////////////////
	memcpy(temp_snd_buf, GET_M1_KEY, sizeof(GET_M1_KEY));//ͷ
	memcpy(&temp_snd_buf[5], &CardInfo._CARD_NUMBER.detail_card_number.city_code[0], 2);//���д���
	memcpy(&temp_snd_buf[7], &CardInfo.csn[0], 4);//CSNӲ������
	memcpy(&temp_snd_buf[11], &CardInfo._CARD_NUMBER.detail_card_number.serial_number[2], 2);//��������ˮ�ź�2�ֽ�
	memcpy(&temp_snd_buf[13], &CardInfo.authentication_code[0], 4);	//����֤��
	temp_snd_buf[17] = 0x02;
	temp_snd_buf[18] = 0x06;
	temp_snd_buf[19] = 0x0a;
	temp_snd_buf[20] = 0x0b;
	temp_snd_buf[21] = 0x0c;//����sector2��6,10,11,12����Կ

	k = CmdPsam(hSam1Handle, temp_snd_buf, 22, temp_rcv_buf, &rcv_length);//��MAC1
	if ( k != ok )
	{
		lcddisperr("������Կʧ��");
		return notok;
	}

	memcpy(&KeySector[2][0], &temp_rcv_buf[0], 6);	//������������Կ��2sector
	memcpy(&KeySector[6][0], &temp_rcv_buf[6], 6);//������������Կ��6sector
	memcpy(&KeySector[10][0], &temp_rcv_buf[12], 6);//������������Կ��10sector
	memcpy(&KeySector[11][0], &temp_rcv_buf[18], 6);//������������Կ��11sector
	memcpy(&KeySector[12][0], &temp_rcv_buf[24], 6);//������������Կ��12sector

	////////////////////////////////////////
	//��֤Sector 6 ��Կ
	////////////////////////////////////////
	i = Get_Sector_PermissionA(6, &KeySector[6][0]);
	if ( i != ok )
	{
		EA_ucMIFHalt (hMifsHandle);	//�ҿ�
		return notok;
	}
	
	////////////////////////////////////////
	//�жϴ˿��Ƿ��Ѿ��ú�������־
	////////////////////////////////////////
	if ( block_buffer1[7] == 0x04 )
	{
		lcddisperr("�Ƿ���");
		(void)EA_ucMIFHalt(hMifsHandle);	
		return ok;
	}
	switch ( CardInfo.card_type )
	{
		case CARDT_NORMAL:
		case CARDT_STUDENT:
		case CARDT_OLDMAN:
		case CARDT_FREE:
		case CARDT_MEMORY:
		case CARDT_DRIVER:
			i = QueryNormal();
			(void)EA_ucMIFHalt(hMifsHandle);
			return i;
		default:
			EA_vDisp(1,1,"�豸��֧�ִ˿�!");
			(void)EA_ucMIFHalt(hMifsHandle);
			return ok;
	}

}
/******************************************************************************
 �������ƣ�M1_Driver_Card_Process
 ����������M1˾������������
 ����������
 �������ƣ� ����/����� ����        ����
 ������      ���״̬��


 ��  ��  ֵ��ok(0)-�ɹ�
			 notok(0xFF)-ʧ��

 ��      �� ��myron
 ��      �ڣ�2005-02-17
 �޸���ʷ��
		����        �޸���      �޸�����
		------      ---------   -------------
******************************************************************************/
INT8U M1_Driver_Card_Process(void)
{
	return ok;
}

/* *****************************************************************************
 �������ƣ�M1_Set_Card_Process
 ����������M1Ʊ�۳�����·���ÿ�����
 ����������
 �������ƣ�	����/�����	����		����
������		���״̬��
 
 ��  ��  ֵ��ok(0)-�ɹ�
			 notok(0xFF)-ʧ��
				   
 ��      ��	��myron
 ��      �ڣ�2005-02-16
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
***************************************************************************** */
INT8U M1_Set_Card_Process(void)
{
	return ok;
}

/******************************************************************************
 �������ƣ�M1_Checked_Card_Process
 ����������M1��⿨��������
 ����������
 �������ƣ�	����/�����	����		����
������		���״̬��
 
 ��  ��  ֵ��ok(0)-�ɹ�
			 notok(0xFF)-ʧ��
				   
 ��      ��	��myron
 ��      �ڣ�2005-02-16
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
******************************************************************************/
INT8U M1_Checked_Card_Process()
{
/*	
	INT8U LOCAL_V_TYPE  i = 0;
	INT8U LOCAL_V_TYPE  j = 0;
	INT8U LOCAL_V_TYPE  m = 0;
//	INT16U LOCAL_V_TYPE k = 0;
	INT16U LOCAL_V_TYPE temp_int16u;
	INT8S LOCAL_V_TYPE g = 0;
//	INT8S LOCAL_V_TYPE h = 0;

	INT8U  block_buffer1[16] = {0};
	INT8U  block_buffer2[16] = {0};

	////////////////////////////////////////
	//��֤Sector 6 ��Կ
	////////////////////////////////////////
	i=Sector6_Process();
	if ( i!=ok )
		return notok;

// 	////////////////////////////////////////////////////////
// 	//�����ж�
// 	////////////////////////////////////////////////////////
// 	if ( ErrCardInfo.error_occured == TRUE )  //��֤�˽�����ſɶϲ����أ��������Ϊ���ݿ�������֮һ
// 	{
// 		//��������
// 		memcpy(&ErrCardInfo.block24_buf_to_restore[12], "\x18\xE7\x18\xE7", 4);		//ǿ����Ϊ0x18E7
// 		i = CardMFCWrite16Bytes(24, &ErrCardInfo.block24_buf_to_restore[0]);
// 		if ( i != ok )
// 			return notok;
//
// 		i = CardMFCWrite16Bytes(25, &ErrCardInfo.block24_buf_to_restore[0]);
// 		if ( i != ok )
// 			return notok;
// 	}
// 	else
// 	{		//��һ���뿨����
// 		////////////////////////////////////////
// 		//���ڽ��׼���������
// 		////////////////////////////////////////
// 		memcpy(&CardInfo.init_purchase_serial_num[0], block_buffer1+2, 2);				//��ʼ�������
// 		memcpy(&temp_int16u, block_buffer1+2, 2);														//��ʼ�������
// 		if ( temp_int16u >= 0xFFFF )
// 			temp_int16u = 0;
// 		else
// 			temp_int16u++;
// 		memcpy(&CardInfo.purchase_serial_num[0], &temp_int16u, 2);						//+1��Ľ������
//
// 		memcpy(&ErrCardInfo.block24_buf_to_restore[0], block_buffer1, 16);				//��24��Ӧ��д�������
// 		memcpy(&ErrCardInfo.block24_buf_to_restore[2], &CardInfo.purchase_serial_num[0], 2);
// 		memcpy(&ErrCardInfo.block24_buf_to_restore[12], "\x18\xE7\x18\xE7", 4);		//ǿ����Ϊ0x18E7
// 		i = CardMFCWrite16Bytes(24, &ErrCardInfo.block24_buf_to_restore[0]);
// 		if ( i != ok )
// 			return notok;
//
// 		i = CardMFCWrite16Bytes(25, &ErrCardInfo.block24_buf_to_restore[0]);
// 		if ( i != ok )
// 			return notok;
//
// 	}
	CardInfo.psam_purchase_serial_num = 0;//PSAM�������������
//	CardInfo.card_purchase_type= 0x10;

	////////////////////////////////////////
	//��֤Sector 11 ��Կ
	////////////////////////////////////////
	i = Get_Sector_PermissionA(11, &KeySector[11][0]);
	if ( i != ok )
		return notok;

	i = CardMFCRead16Bytes(44, block_buffer1);
	if ( i != ok )
		return notok;

	CardInfo.checked_flag= block_buffer1[3];//�������־
	memcpy(&CardInfo.manager_number.checker_number[0],block_buffer1,sizeof(CardInfo.manager_number.checker_number));//������Ա���
	CardInfo.card_purchase_type=TT_CHECK;
*/
	return ok;
}

/******************************************************************************
 �������ƣ�M1_Inspector_Card_Process
 ����������M1���鿨��������
 ����������
 �������ƣ�	����/�����	����		����
������		���״̬��
 
 ��  ��  ֵ��ok(0)-�ɹ�
			 notok(0xFF)-ʧ��
				   
 ��      ��	��myron
 ��      �ڣ�2005-02-16
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
******************************************************************************/
INT8U M1_Inspector_Card_Process(void)
{
	return ok;
}

/******************************************************************************
 �������ƣ�M1_Signed_Card_Process
 ����������M1ǩ�㿨��������
 ����������
 �������ƣ� ����/����� ����		����
������		���״̬��
 
 ��  ��  ֵ��ok(0)-�ɹ�
			 notok(0xFF)-ʧ��
				   
 �� 	 �� ��myron
 �� 	 �ڣ�2005-02-16
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
******************************************************************************/
INT8U M1_Signed_Card_Process()
{
/*	
	INT8U LOCAL_V_TYPE  i = 0;
	INT8U LOCAL_V_TYPE  j = 0;
	INT8U LOCAL_V_TYPE  m = 0;
	INT16U LOCAL_V_TYPE temp_int16u;
	INT8S LOCAL_V_TYPE g = 0;
	INT8U block_buffer1[16] = {0};
	INT8U block_buffer2[16] = {0};

	////////////////////////////////////////
	//��֤Sector 6 ��Կ
	////////////////////////////////////////
	i=Sector6_Process();
	if ( i!=ok )
		return notok;

// 	////////////////////////////////////////////////////////
// 	//�����ж�
// 	////////////////////////////////////////////////////////
// 	if ( ErrCardInfo.error_occured == TRUE )  //��֤�˽�����ſɶϲ����أ��������Ϊ���ݿ�������֮һ
// 	{
// 		//��������
// 		memcpy(&ErrCardInfo.block24_buf_to_restore[12], "\x18\xE7\x18\xE7", 4);		//ǿ����Ϊ0x18E7
// 		i = CardMFCWrite16Bytes(24, &ErrCardInfo.block24_buf_to_restore[0]);
// 		if ( i != ok )
// 			return notok;
//
// 		i = CardMFCWrite16Bytes(25, &ErrCardInfo.block24_buf_to_restore[0]);
// 		if ( i != ok )
// 			return notok;
// 	}
// 	else
// 	{		//��һ���뿨����
// 		////////////////////////////////////////
// 		//���ڽ��׼���������
// 		////////////////////////////////////////
// 		memcpy(&CardInfo.init_purchase_serial_num[0], block_buffer1+2, 2);				//��ʼ�������
// 		memcpy(&temp_int16u, block_buffer1+2, 2);														//��ʼ�������
// 		if ( temp_int16u >= 0xFFFF )
// 			temp_int16u = 0;
// 		else
// 			temp_int16u++;
// 		memcpy(&CardInfo.purchase_serial_num[0], &temp_int16u, 2);						//+1��Ľ������
//
// 		memcpy(&ErrCardInfo.block24_buf_to_restore[0], block_buffer1, 16);				//��24��Ӧ��д�������
// 		memcpy(&ErrCardInfo.block24_buf_to_restore[2], &CardInfo.purchase_serial_num[0], 2);
// 		memcpy(&ErrCardInfo.block24_buf_to_restore[12], "\x18\xE7\x18\xE7", 4);		//ǿ����Ϊ0x18E7
// 		i = CardMFCWrite16Bytes(24, &ErrCardInfo.block24_buf_to_restore[0]);
// 		if ( i != ok )
// 			return notok;
//
// 		i = CardMFCWrite16Bytes(25, &ErrCardInfo.block24_buf_to_restore[0]);
// 		if ( i != ok )
// 			return notok;
//
// 	}
	CardInfo.psam_purchase_serial_num = 0;//PSAM�������������
//	CardInfo.card_purchase_type= 0x10;

	////////////////////////////////////////
	//��֤Sector 11 ��Կ
	////////////////////////////////////////
	i = Get_Sector_PermissionA(11, &KeySector[11][0]);
	if ( i != ok )
		return notok;

	i = CardMFCRead16Bytes(44, block_buffer1);
	if ( i != ok )
		return notok;

	memcpy(&CardInfo.manager_number.signer_number[0],block_buffer1,sizeof(CardInfo.manager_number.signer_number));//����ǩ��Ա���
	memcpy(&CardInfo.station_number[0],block_buffer1+5,sizeof(CardInfo.station_number));//����վ����
	CardInfo.card_purchase_type = TT_SIGN;
*/
	return ok;
}

/******************************************************************************
 �������ƣ�M1_Gather_Card_Process
 ����������M1�ɼ�����������
 ����������
 �������ƣ�	����/�����	����		����
������		���״̬��
 
 ��  ��  ֵ��ok(0)-�ɹ�
			 notok(0xFF)-ʧ��
				   
 ��      ��	��myron
 ��      �ڣ�2005-02-16
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
******************************************************************************/
INT8U M1_Gather_Card_Process()
{
/*	
	INT8U LOCAL_V_TYPE  i = 0;
	INT8U LOCAL_V_TYPE  j = 0;
	INT8U LOCAL_V_TYPE  m = 0;
	INT16U LOCAL_V_TYPE temp_int16u;
	INT8S LOCAL_V_TYPE g = 0;
	INT8U block_buffer1[16] = {0};
	INT8U block_buffer2[16] = {0};

	////////////////////////////////////////
	//��֤Sector 6 ��Կ
	////////////////////////////////////////
	i = Get_Sector_PermissionA(6, &KeySector[6][0]);
	if ( i != ok )
		return notok;

	i = CardMFCRead16Bytes(24, block_buffer1);
	if ( i != ok )
		return notok;

	////////////////////////////////////////
	//�жϴ˿��Ƿ��Ѿ��ú�������־
	////////////////////////////////////////
	if ( block_buffer1[6] == 0x04 )
	{
		printf_debug("�Ƿ���");
		return notok;
	}

//     i = CardMFCRead16Bytes(25, block_buffer2);
//     if ( i != ok )
//         return notok;

	if ( DevStat.driver_is_working == TRUE )
	{
// 		DevStat.driver_is_working = FALSE;
// 		CardInfo.card_purchase_type = TT_DRIVER_REST;
// 		WriteParam();
		QuitWorkProcess();
		return ok;
	}

	CardInfo.psam_purchase_serial_num = 0;										//PSAM�������������

	////////////////////////////////////////
	//��֤Sector 11 ��Կ
	////////////////////////////////////////
	i = Get_Sector_PermissionA(11, &KeySector[11][0]);
	if ( i != ok )
		return notok;

	i = CardMFCRead16Bytes(44, block_buffer1);
	if ( i != ok )
		return notok;

	DevStat.allow_upload_data = TRUE;//����ɼ�
	memcpy(&CardInfo.manager_number.gather_number[0],block_buffer1,sizeof(CardInfo.manager_number.gather_number));//����ɼ�Ա���
	CardInfo.card_purchase_type = TT_MANAGE;
*/
	return ok;
}

/******************************************************************************
 �������ƣ�M1_Favor_Card_Process()
 ����������M1�ɼ�����������
 ����������
 �������ƣ�	����/�����	����		����
������		���״̬��
 
 ��  ��  ֵ��ok(0)-�ɹ�
			 notok(0xFF)-ʧ��
				   
 ��      ��	��myron
 ��      �ڣ�2005-02-22
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
******************************************************************************/
// INT8U M1_Favor_Card_Process()
// {
//     INT8U LOCAL_V_TYPE  i = 0;
//     INT8U LOCAL_V_TYPE  j = 0;
//     INT8U LOCAL_V_TYPE  m = 0;
//     INT16U LOCAL_V_TYPE temp_int16u;
//     INT8S LOCAL_V_TYPE g = 0;
//     INT8U block_buffer1[16] = {0};
//     INT8U block_buffer2[16] = {0};
//     INT16U LOCAL_V_TYPE temp_balance;
//     ////////////////////////////////////////
//     //��֤Sector 6 ��Կ
//     ////////////////////////////////////////
//     i=Sector6_Process();
//     if ( i!=ok )
//         return notok;
//
//     ////////////////////////////////////////////////////////
//     //�����ж�
//     ////////////////////////////////////////////////////////
//     if ( ErrCardInfo.error_occured == TRUE )  //��֤�˽�����ſɶϲ����أ��������Ϊ���ݿ�������֮һ
//     {
//         //��������
//         memcpy(&ErrCardInfo.block24_buf_to_restore[12], "\x18\xE7\x18\xE7", 4);     //ǿ����Ϊ0x18E7
//         i = CardMFCWrite16Bytes(24, &ErrCardInfo.block24_buf_to_restore[0]);
//         if ( i != ok )
//             return notok;
//
//         i = CardMFCWrite16Bytes(25, &ErrCardInfo.block24_buf_to_restore[0]);
//         if ( i != ok )
//             return notok;
//     }
//     else
//     {       //��һ���뿨����
//         ////////////////////////////////////////
//         //���ڽ��׼���������
//         ////////////////////////////////////////
//         memcpy(&CardInfo.init_purchase_serial_num[0], block_buffer1+2, 2);              //��ʼ�������
//         memcpy(&temp_int16u, block_buffer1+2, 2);                                                       //��ʼ�������
//         if ( temp_int16u >= 0xFFFF )
//             temp_int16u = 0;
//         else
//             temp_int16u++;
//         memcpy(&CardInfo.purchase_serial_num[0], &temp_int16u, 2);                      //+1��Ľ������
//
//         memcpy(&ErrCardInfo.block24_buf_to_restore[0], block_buffer1, 16);              //��24��Ӧ��д�������
//         memcpy(&ErrCardInfo.block24_buf_to_restore[2], &CardInfo.purchase_serial_num[0], 2);
//         memcpy(&ErrCardInfo.block24_buf_to_restore[12], "\x18\xE7\x18\xE7", 4);     //ǿ����Ϊ0x18E7
//         i = CardMFCWrite16Bytes(24, &ErrCardInfo.block24_buf_to_restore[0]);
//         if ( i != ok )
//             return notok;
//
//         i = CardMFCWrite16Bytes(25, &ErrCardInfo.block24_buf_to_restore[0]);
//         if ( i != ok )
//             return notok;
//
//     }
//     CardInfo.psam_purchase_serial_num = 0;//PSAM�������������
//     //  CardInfo.card_purchase_type= 0x10;
//     //add hu
//     ////////////////////////////////////////
//     //���ݿ�����������ѽ��
//     ////////////////////////////////////////
//     CardInfo.fare = GetFare(CardInfo.card_type);
//     //���Ǯ�����
//     i=Check_Money();
//     if ( i!=ok )
//         return notok;
//     //������
//     ////////////////////////////////////////
//     //��֤Sector 10 ��Կ
//     ////////////////////////////////////////
//     i = Get_Sector_PermissionA(10, &KeySector[10][0]);
//     if ( i != ok )
//         return notok;
//
//     i = CardMFCRead16Bytes(41, block_buffer1);
//     if ( i != ok )
//         return notok;
//
//     i = CardMFCRead16Bytes(42, block_buffer2);
//     if ( i != ok )
//         return notok;
//
//     i = Favor_Block_Is_Right(block_buffer1);
//     j = Favor_Block_Is_Right(block_buffer2);
//     if ( (i == ok) && (j == ok) )
//     {
//         //�������Ϸ��������������Ƿ�һ��
//         g = memcmp(block_buffer1, block_buffer2, 16);
//         if ( g == ok )
//         {
//             //����һ�£�ʲôҲ����
//             ;
//         }
//         else
//         {
//             //������һ�£���41��Ϊ��
//             m = CardMFCWrite16Bytes(42,block_buffer1);
//             if ( m != ok )
//                 return notok;
//         }
//
//     }
//     else if ( i == ok )
//     {
//         // 41���Ϸ���41->42
//         m = CardMFCWrite16Bytes(42,block_buffer1);
//         if ( m != ok )
//             return notok;
//     }
//     else if ( j == ok )
//     {
//         // 42���Ϸ���42->41
//         m = CardMFCWrite16Bytes(41,block_buffer2);
//         if ( m != ok )
//             return notok;
//         memcpy(block_buffer1, block_buffer2, 16);//�õ����ݴ���block_buffer1���±ߴ���������block_buffer1
//     }
//     else
//     {
//         //���������Ϸ����˳�
//         return notok;
//     }
//     i = memcmp(block_buffer1,&CardInfo.card_in_time.year, 3);       //p0+119������λ
//     //ʱ����ȣ��۳�����
//     if ( i == 0 )
//     {
//         //��ȣ���һ��
//         memcpy(&temp_balance, &block_buffer1[3], 2);
//         if ( temp_balance == 0 )
//         {
//             //�Żݴ����ѿ۹⣬������ͨ������
//             CardInfo.card_type= CARDT_NORMAL;
//             memcpy(&block_buffer1[5],&CardInfo.card_in_time, 7);    //д�뵱(��)�ν�������ʱ��
//             memcpy(&block_buffer1[12], "\x29\xD6\x29\xD6", 4);//ǿ����Ϊ0x29d6
//             /////////////////////////////////////////
//             //�۴�������
//             /////////////////////////////////////////
//             i = CardMFCWrite16Bytes(41,block_buffer1);
//             if ( i != ok )
//             {//�۴�����������¼�������ź�ԭ�������
//                 memcpy(&ErrCardInfo.m1_card_number[0], &CardInfo._CARD_NUMBER.detail_card_number.serial_number[0], 4);  //�������M1������
//                 memcpy(&ErrCardInfo.csn[0], &CardInfo.csn[0], 4);               //�������M1��Ӳ������
//                 ErrCardInfo.init_time_balance = CardInfo.init_time_balance;                 //�������M1��ԭ�������
//                 ErrCardInfo.error_occured = TRUE;
//                 DevStat.purchase_again_req = TRUE;      //��������ˢ������
//                 DevStat.voice_purchase_again_freq=100;
//                 return notok;
//             }
//             i = CardMFCWrite16Bytes(42,block_buffer1);
//             if ( i != ok )
//             {   //�ۿ��������¼�������ź�ԭ���
//                 memcpy(&ErrCardInfo.m1_card_number[0], &CardInfo._CARD_NUMBER.detail_card_number.serial_number[0], 4);  //�������M1������
//                 memcpy(&ErrCardInfo.csn[0], &CardInfo.csn[0], 4);               //�������M1��Ӳ������
//                 ErrCardInfo.init_time_balance = CardInfo.init_time_balance;                 //�������M1��ԭ���
//                 ErrCardInfo.error_occured = TRUE;
//                 DevStat.purchase_again_req = TRUE;      //��������ˢ������
//                 DevStat.voice_purchase_again_freq=100;
//
//                 return notok;
//             }
//         }
//         else
//         {
//             //�����Żݴ���
//             temp_balance--;
//
// //          g_xunPreferential_Price_Num = preferential_times;
//
//             memcpy(&block_buffer1[3], &temp_balance, 2);        //�Żݴ���--��д��
//
//             memcpy(&block_buffer1[5], &CardInfo.card_in_time, 7);   //д�뵱(��)�ν�������ʱ��
//             memcpy(&block_buffer1[12], "\x29\xD6\x29\xD6", 4);//ǿ����Ϊ0x29d6
//             /////////////////////////////////////////
//             //�۴�������
//             /////////////////////////////////////////
//             i = CardMFCWrite16Bytes(41,block_buffer1);
//             if ( i != ok )
//             {//�۴�����������¼�������ź�ԭ�������
//                 memcpy(&ErrCardInfo.m1_card_number[0], &CardInfo._CARD_NUMBER.detail_card_number.serial_number[0], 4);  //�������M1������
//                 memcpy(&ErrCardInfo.csn[0], &CardInfo.csn[0], 4);               //�������M1��Ӳ������
//                 ErrCardInfo.init_time_balance = CardInfo.init_time_balance;                 //�������M1��ԭ�������
//                 ErrCardInfo.error_occured = TRUE;
//                 DevStat.purchase_again_req = TRUE;      //��������ˢ������
//                 DevStat.voice_purchase_again_freq=100;
//                 return notok;
//             }
//             i = CardMFCWrite16Bytes(42,block_buffer1);
//             if ( i != ok )
//             {   //�ۿ��������¼�������ź�ԭ���
//                 memcpy(&ErrCardInfo.m1_card_number[0], &CardInfo._CARD_NUMBER.detail_card_number.serial_number[0], 4);  //�������M1������
//                 memcpy(&ErrCardInfo.csn[0], &CardInfo.csn[0], 4);               //�������M1��Ӳ������
//                 ErrCardInfo.init_time_balance = CardInfo.init_time_balance;                 //�������M1��ԭ���
//                 ErrCardInfo.error_occured = TRUE;
//                 DevStat.purchase_again_req = TRUE;      //��������ˢ������
//                 DevStat.voice_purchase_again_freq=100;
//
//                 return notok;
//             }
//         }
//     }
//     else
//     {
//         //����ȣ����´���
//         memcpy(&block_buffer1[0], &CardInfo.card_in_time.year, 3);      //������Ч����
//         switch ( CardInfo.card_type )
//         {
//             case 0x02:      //Сѧ����
//                 memcpy(&temp_balance, &DevStat.student_a_card_count, 2);
//                 temp_balance--;
//                 memcpy(&block_buffer1[3], &temp_balance, 2);        //�Żݴ���--��д��
//                 break;
//
//             case 0x08:      //��ѧ����
//                 memcpy(&temp_balance, &DevStat.student_b_card_count, 2);
//                 temp_balance--;
//                 memcpy(&block_buffer1[3], &temp_balance, 2);        //�Żݴ���--��д��
//                 break;
//
//             case 0x07:      //��Ʊ��
//                 memcpy(&temp_balance, &DevStat.emplory_b, 2);
//                 temp_balance--;
//                 memcpy(&block_buffer1[3], &temp_balance, 2);        //�Żݴ���--��д��
//                 break;
//
//             default:
//                 return 0xFF;            //�����Ӧ�ò�����
//                 break;
//         }
//
//         //g_xunPreferential_Price_Num = preferential_times;
//         memcpy(&block_buffer1[5], &CardInfo.card_in_time, 7);   //д�뵱(��)�ν�������ʱ��
//         memcpy(&block_buffer1[12], "\x29\xD6\x29\xD6", 4);//ǿ����Ϊ0x29d6
//         /////////////////////////////////////////
//         //�۴�������
//         /////////////////////////////////////////
//         i = CardMFCWrite16Bytes(41,block_buffer1);
//         if ( i != ok )
//         {//�۴�����������¼�������ź�ԭ�������
//             memcpy(&ErrCardInfo.m1_card_number[0], &CardInfo._CARD_NUMBER.detail_card_number.serial_number[0], 4);  //�������M1������
//             memcpy(&ErrCardInfo.csn[0], &CardInfo.csn[0], 4);               //�������M1��Ӳ������
//             ErrCardInfo.init_time_balance = CardInfo.init_time_balance;                 //�������M1��ԭ�������
//             ErrCardInfo.error_occured = TRUE;
//             DevStat.purchase_again_req = TRUE;      //��������ˢ������
//             DevStat.voice_purchase_again_freq=100;
//             return notok;
//         }
//         i = CardMFCWrite16Bytes(42,block_buffer1);
//         if ( i != ok )
//         {   //�ۿ��������¼�������ź�ԭ���
//             memcpy(&ErrCardInfo.m1_card_number[0], &CardInfo._CARD_NUMBER.detail_card_number.serial_number[0], 4);  //�������M1������
//             memcpy(&ErrCardInfo.csn[0], &CardInfo.csn[0], 4);               //�������M1��Ӳ������
//             ErrCardInfo.init_time_balance = CardInfo.init_time_balance;                 //�������M1��ԭ���
//             ErrCardInfo.error_occured = TRUE;
//             DevStat.purchase_again_req = TRUE;      //��������ˢ������
//             DevStat.voice_purchase_again_freq=100;
//
//             return notok;
//         }
//         return 0xFD;
//     }
//     //���Żݽ���Ǯ
//     i=M1_Card_Consume_Process();
//     if ( i!=ok )
//         return notok;
// 	return ok;
// }

INT8U CardMFCValueWithTransfer(int flag, int s_block, int d_block, ulong num)
{
	INT8U ucResult;

	if(flag == 0)//decrement
	{
		ucResult = EA_ucMIFDecrement (hMifsHandle, s_block,num);
		if (ucResult != EM_SUCCESS)
		{
			return notok;
		}
		/* ���������14�����ݱ��ݵ���13���У���������13��Ҳ��Ϊ��ֵ�飬��ֵΪ125*/
		ucResult = EA_ucMIFTransfer (hMifsHandle, d_block);
		if (ucResult != EM_SUCCESS)
		{
			return notok;
		}
	}
	if(flag == 1)//increment
	{
		ucResult = EA_ucMIFIncrement(hMifsHandle, s_block,num);
		if (ucResult != EM_SUCCESS)
		{
			return notok;
		}
		/* ���������14�����ݱ��ݵ���13���У���������13��Ҳ��Ϊ��ֵ�飬��ֵΪ125*/
		ucResult = EA_ucMIFTransfer (hMifsHandle, d_block);
		if (ucResult != EM_SUCCESS)
		{
			return notok;
		}
	}
	return ok;
}
/******************************************************************************
 �������ƣ�Cal_BCC
 �����������Խ��յ������ݽ���BCCУ��
 ����������
 �������ƣ�	����/�����	����		����
 buf		����		const INT8U*		Ҫ����BCC��������ݻ�����
 len		����		INT16U		����
				
 ��  ��  ֵ�����������BCCֵ
				   
 ��      ��	������
 ��      �ڣ�2004-09-02
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
******************************************************************************/
INT8U Cal_BCC_Hex(const INT8U *buf, INT16U len)
{
	INT16U LOCAL_V_TYPE i = 0;
	INT8U LOCAL_V_TYPE bcc = 0;

	for ( i=0; i<len; i++ )
	{
		bcc ^= buf[i];
	}

	return bcc;
}
/*****************************************************************
 ����ԭ�ͣ�M1_Card_Consume_Process
 ����������M1���ѿ��ۿ�����
 ����������
 �������ƣ�	����/�����	����		����
				
				
 ��  ��  ֵ��	ok(0)-���ѳɹ�		
				0xFF-����ʧ��
				//0xFC-���Ǳ�ϵͳ��
				//0xFD-�п�������û�лظÿ�
				0xF0-����ˢ��
 ��      ��	������
 ��      �ڣ�2004-09-27
 �޸���ʷ��
		����		�޸���		�޸�����
		2005.2.21		myron			in function
*****************************************************************/
INT8U M1_Card_Consume_Process(void)
{
	char    input[22], buf[22];
	INT16U  LOCAL_V_TYPE    temp_int16u;
	uchar   i;
	float   temp_fare;

	memset(buf, 0, sizeof(buf));

	//���Ѽ�¼������
	EA_vCls();
	EA_ucClrKeyBuf();
	Beep(2500);
	strcpy((void *)&CardInfo.bus_number[0], "12345");
	EA_vDisplay(1, "��  ��:³C-%s", &CardInfo.bus_number[0] );

	EA_ucSetInverse(EM_lcd_INVOFF);

	if( DevStat.fill_mode == 0 )
	{
		EA_vDisplay(2, "��Ȼ��:        ��(L)");
	}
	else if( DevStat.fill_mode == 1 )
	{
		EA_vDisplay(2, "������:        ��(L)");
	}
	else if( DevStat.fill_mode == 2 )
	{
		EA_vDisplay(2, "������:        ��(L)");
	}
	else
	{
		return notok;
	}
	strcpy((void *)input, "");

	for ( ;; )
	{
		i = get_input_str(2, 8, 1, 6, (void *)input);
		if ( i == EM_SUCCESS )
			break;
		if ( i == EM_ABOLISH )
			return notok;
	}

	sscanf((void *)input, "%f", &temp_fare);
	
	if( DevStat.fill_mode == 0 )
	{
		sprintf((void *)buf, "������:%4.2f��(L)", temp_fare);
	}
	else
	{
		sprintf((void *)buf, "������:%4.2f��(L)", temp_fare);
	}

	CardInfo.fare = temp_fare * 100;

	lcddisp(2, 1, (void *)buf);

//  EA_ucSetInverse(EM_lcd_INVOFF);

	if ( (CardInfo.fare < 1) || (CardInfo.fare > 50000 ) )
	{
		//���㴦��
		lcddisperr((char *)"�������������!");
		memset(&CardInfo, 0x00, sizeof(CardInfo));      	//�忨Ƭ������Ϣ
		memset(&ErrCardInfo, 0x00, sizeof(ErrCardInfo));    //�������Ϣ
		DevStat.purchase_again_req = FALSE;					//������ˢ��������־
		DevStat.voice_purchase_again_freq = 0;
		return notok;
	}
//  CardInfo.balance = CardInfo.init_balance - CardInfo.fare;//���潻�׺����
	CardInfo.card_purchase_type = TT_NORMAL_CONSUME;		 //��ͨ����������
	//�����豸�������	
	temp_int16u = ((INT16U)DevStat.dev_trade_no[0] << 8) | (INT16U)DevStat.dev_trade_no[1]; 	//��ʼ�������
	if ( temp_int16u >= 0xFFFF )
		temp_int16u = 0;
	else
		temp_int16u++;
	DevStat.dev_trade_no[0] = (INT8U)(temp_int16u >> 8);						//+1��Ľ������
	DevStat.dev_trade_no[1] = (INT8U)temp_int16u;

	WriteParam();

	return ok;
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
INT8U Fuzha_Sec6_Block_Is_Legal(const INT8U * data_to_verify)
{
	INT8U LOCAL_V_TYPE i = 0;
	INT8U LOCAL_V_TYPE bcc = 0;

//  ToggleWD();

	if (memcmp(data_to_verify, "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF", 16) == 0)
		return notok;

	if (memcmp(data_to_verify, "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 16) == 0)
			return notok;

	for ( i=0; i<15; i++ )
	{
		bcc ^= *(data_to_verify + i);
	}
	if ( bcc==*(data_to_verify + 15) )
	{
		return ok;
	}
	else
	{
		return notok;
	}
}
///*****************************************************************
// ����ԭ�ͣ�M1_Card_Consume_ProcessA
// ����������M1���ѿ��ۿ�����
// ����������
// �������ƣ�	����/�����	����		����
//				
//				
// ��  ��  ֵ��	ok(0)-���ѳɹ�		
//				0xFF-����ʧ��
//				//0xFC-���Ǳ�ϵͳ��
//				//0xFD-�п�������û�лظÿ�
//				0xF0-����ˢ��
// ��      ��	������
// ��      �ڣ�2004-09-27
// �޸���ʷ��
//		����		�޸���		�޸�����
//		2005.2.21		myron			in function
//*****************************************************************/
//INT8U M1_Card_Consume_ProcessA(void)
//{
//	
//	INT8U   LOCAL_V_TYPE    i = 0;
//	INT8U   LOCAL_V_TYPE    j = 0;
//	INT8U   LOCAL_V_TYPE    m = 0;
//	INT16U  LOCAL_V_TYPE    temp_int16u;
//	INT8S   LOCAL_V_TYPE    g = 0;
//	INT8U   LOCAL_V_TYPE    block_buffer1[16] = {0};
//	INT8U   LOCAL_V_TYPE    block_buffer2[16] = {0};
//	INT8U   LOCAL_V_TYPE    block9[16];
//	INT8U   LOCAL_V_TYPE    block10[16];
//	float   zero=0;
//	//float temp_data;
//	//INT8U 	const  			zero[4] = {0,0,0,0};
//	INT32U  LOCAL_V_TYPE    temp_balance;
////     INT8U   LOCAL_V_TYPE    zero_balance_counter ;
//
//
//
//	////////////////////////////////////////
//	//��֤Sector 6 ��Կ
//	////////////////////////////////////////
////  if(CardInit() != ok)
////  	return notok;
//	
//	//i = Get_Sector_PermissionA(6, &KeySector[6][0]);
//	i = Get_Sector_PermissionA(6, &KeySector[6][0]);
//	if ( i != ok )
//		return notok;
//
//	i = EA_ucMIFRead(hMifsHandle, 24, block_buffer1);
//	if ( i != ok )
//		return notok;
//
//	i = EA_ucMIFRead(hMifsHandle, 25, block_buffer2);
//	if ( i != ok )
//		return notok;
//
//	i = Sec6_Block_Is_Legal(block_buffer1);
//	j = Sec6_Block_Is_Legal(block_buffer2);
//	if ( (i == ok) && (j == ok) )
//	{
//		//�������Ϸ��������������Ƿ�һ��
//		g = memcmp(block_buffer1, block_buffer2, 16);
//		if ( g == ok )
//		{
//			//����һ�£�ʲôҲ����
//			;
//		}
//		else
//		{
//			//������һ�£���24��Ϊ��
//			m = EA_ucMIFWrite(hMifsHandle, 25, block_buffer1);
//			if ( m != ok )
//				return notok;
//		}
//
//	}
//	else if ( i == ok )
//	{
//		//24���Ϸ���24->25
//		m = EA_ucMIFWrite(hMifsHandle, 25, block_buffer1);
//		if ( m != ok )
//			return notok;
//	}
//	else if ( j == ok )
//	{
//		//25���Ϸ���25->24
//		m = EA_ucMIFWrite(hMifsHandle, 24, block_buffer2);
//		if ( m != ok )
//			return notok;
//		memcpy(block_buffer1, block_buffer2, 16);//�õ����ݴ���block_buffer1���±ߴ���������block_buffer1
//	}
//	else
//	{
//		//���������Ϸ����˳�
//		return notok;
//	}
//
//	////////////////////////////////////////
//	//�жϴ˿��Ƿ��Ѿ��ú�������־
//	////////////////////////////////////////
//	if ( block_buffer1[7] == 0x04 )
//	{
//		printf_debug((char*)"�Ƿ���");
//		return notok;
//	}
//
//
//	CardInfo.credit_rec_pos = block_buffer1[8];
//		
//	if(CardInfo.credit_rec_pos == 0x00 ||CardInfo.credit_rec_pos == 0xFF)
//		CardInfo.credit_rec_pos = 0x0C;
//	else
//		CardInfo.credit_rec_pos ++;
//
//	if(CardInfo.credit_rec_pos==0x0f ||CardInfo.credit_rec_pos==0x13 )
//		CardInfo.credit_rec_pos ++;
//	if(CardInfo.credit_rec_pos>=0x17)
//		CardInfo.credit_rec_pos=0x0C;
//
//	////////////////////////////////////////////////////////
//	//�����ж�
//	////////////////////////////////////////////////////////
//	if ( ErrCardInfo.error_occured == TRUE )							  //��֤�˽�����ſɶϲ����أ��������Ϊ���ݿ�������֮һ
//	{
//		//��������
//		memcpy(&ErrCardInfo.block24_buf_to_restore[12], "\x18\xE7\x18\xE7", 4);		//ǿ����Ϊ0x18E7
//		i = EA_ucMIFWrite(hMifsHandle, 24, &ErrCardInfo.block24_buf_to_restore[0]);
//		if ( i != ok )
//			return notok;
//
//		i = EA_ucMIFWrite(hMifsHandle, 25, &ErrCardInfo.block24_buf_to_restore[0]);
//		if ( i != ok )
//			return notok;
//	}
//	else
//	{		//��һ���뿨���� 
//		////////////////////////////////////////
//		//���ڽ��׼���������
//		////////////////////////////////////////
//		CardInfo.init_purchase_serial_num[0] = block_buffer1[0];				//��ʼ�������
//		CardInfo.init_purchase_serial_num[1] = block_buffer1[1];
//		temp_int16u = ((INT16U)block_buffer1[0] << 8) | (INT16U)block_buffer1[1];				//��ʼ�������
//		if ( temp_int16u >= 0xFFFF )
//			temp_int16u = 0;
//		else
//			temp_int16u++;
////  	EA_vDisplay(1, "%02x", block_buffer1[0]);
////  	EA_vDisplay(2, "%02x", block_buffer1[1]);
////  	EA_vDisplay(2, "%02d", temp_int16u);
////  	EA_uiInkey(0);
//
//		CardInfo.purchase_serial_num[0] = (INT8U)(temp_int16u / 0x100 );						//+1��Ľ������
//		CardInfo.purchase_serial_num[1] = (INT8U)(temp_int16u % 0x100 );
//
//		memcpy(&ErrCardInfo.block24_buf_to_restore[0], block_buffer1, 16);				//��24��Ӧ��д�������
//		memcpy(&ErrCardInfo.block24_buf_to_restore[0], &CardInfo.purchase_serial_num[0], 2);
//		memcpy(&ErrCardInfo.block24_buf_to_restore[12], "\x18\xE7\x18\xE7", 4);		//ǿ����Ϊ0x18E7
//		i = EA_ucMIFWrite(hMifsHandle, 24, &ErrCardInfo.block24_buf_to_restore[0]);
//		if ( i != ok )
//			return notok;
//
//		i = EA_ucMIFWrite(hMifsHandle, 25, &ErrCardInfo.block24_buf_to_restore[0]);
//		if ( i != ok )
//			return notok;
//
//	}
//
//	CardInfo.psam_purchase_serial_num = 0;										//PSAM�������������
//
//	////////////////////////////////////////
//	//��֤Sector 2 ��Կ
//	////////////////////////////////////////
//	if(CardInit() !=ok)
//		return notok;
//	
//	//i = Get_Sector_PermissionA(2, &KeySector[2][0]);
//	i = Get_Sector_PermissionA(2, &KeySector[2][0]);
//	if ( i != ok )
//		return notok;
//
//	i =EA_ucMIFRead(hMifsHandle, 9, block9);
//	if ( i != ok )
//		return notok;
//
//	i = EA_ucMIFRead(hMifsHandle, 10, block10);
//	if ( i != ok )
//		return notok;
//
//	i = Purse_Block_Is_Right(block9);
//	j = Purse_Block_Is_Right(block10);
//	if ( (i == ok) && (j == ok) )
//	{	//�������Ϸ��������������Ƿ�һ��
//		g = memcmp(block9, block10, 16);
//		if ( g == ok )
//		{
//			;			//����һ�£�ʲôҲ����
//		}
//		else
//		{
//			//������һ�£���9��Ϊ��
//			m = CardMFCValueWithTransfer(PICC_DECREMENT, 9, 10,zero);	  //������һ�£���9��Ϊ��
//			if ( m != ok )
//				return notok;
//		}
//	}
//	else if ( i == ok )
//	{
////         m = Write(10, block9);              //9���Ϸ���9->10
//		m = CardMFCValueWithTransfer(PICC_DECREMENT, 9, 10, zero);	   //9���Ϸ���9->10
//		if ( m != ok )
//			return notok;
//	}
//	else if ( j == ok )
//	{
////         m = Write(9, block10);          //10���Ϸ���10->9
//		m = CardMFCValueWithTransfer(PICC_DECREMENT, 10, 9, zero);	  //10���Ϸ���10->9
//		if ( m != ok )
//			return notok;
//		memcpy(block9, block10, 16);		//�õ����ݴ���block9�±ߴ���������block9
//	}
//	else
//	{
//		return notok;		//���������Ϸ����˳�
//	}
//
//
//
//	////////////////////////////////////////
//	//�����ۿ��
//	////////////////////////////////////////
//	if ( ErrCardInfo.error_occured == TRUE )
//	{	//�����ؿ����������������һ�����Ѿ��۹����ˣ���һ���ǻ�û�۹���
//		temp_balance = BlockToCash(block9);
//		if ( temp_balance == ErrCardInfo.init_balance )
//		{	//�ۿ�û�ɹ�
//			//���½����ߵ�һ�οۿ��
//		}
//		else
//		{	//�ۿ�ɹ�(�������������Ҳ��ۿ�ɹ�)
//			CardInfo.balance = temp_balance;//���潻�׺����
//
//			//if ( (DevStat.oldman_free == TRUE && CardInfo.card_type == CARDT_OLDMAN)
//			//	 || (DevStat.employee_a_free == TRUE && CardInfo.card_type == CARDT_FREE)
//			//	 || (DevStat.driver_free == TRUE && CardInfo.card_type == CARDT_DRIVER) )
//			//{
//			//	CardInfo.card_purchase_type = TT_FREE;	//��ѿ�����
//			//}
//			
//			//else
//			//{
//				CardInfo.card_purchase_type = TT_NORMAL_CONSUME_MONEY;//��ͨ����������
//			//}
//
//			return ok;
//		}
//	}
//
//	////////////////////////////////////////
//	//��һ���뿨�ۿ��
//	////////////////////////////////////////
//	if ( ErrCardInfo.error_occured != TRUE )			//ֻ���ڵ�һ���뿨ʱ���ű����ʼ���Ժ��״򲻶�
//	{
//		CardInfo.init_balance = BlockToCash(block9);
//	}
//
//
//	if ( CardInfo.init_balance < CardInfo.fare )
//	{
//		//���㴦��
//		printf_debug((char *)"�������ֵ");
//		memset(&CardInfo, 0x00, sizeof(CardInfo));		//�忨Ƭ������Ϣ		
//		memset(&ErrCardInfo, 0x00, sizeof(ErrCardInfo));	//�������Ϣ
//		DevStat.purchase_again_req = FALSE;//������ˢ��������־
//		DevStat.voice_purchase_again_freq=0;
//		return notok;
//	}
//
//	if( ErrCardInfo.error_occured != TRUE)
//		StoreGrayRecord();
//
//// 	SleepMs(500);
////	i = CardMFCValueWithTransfer(PICC_DECREMENT, 9, 9,CashToBlock(CardInfo.fare));
//	i = CardMFCValueWithTransfer(PICC_DECREMENT, 9, 9,CardInfo.fare);
//
//	if ( i != ok )
//	{	//�ۿ��������¼�������ź�ԭ���
//		memcpy(&ErrCardInfo.m1_card_number[0], &CardInfo._CARD_NUMBER.detail_card_number.serial_number[0], 4);	//�������M1������
//		memcpy(&ErrCardInfo.csn[0], &CardInfo.csn[0], 4);				//�������M1��Ӳ������
//		ErrCardInfo.init_balance = CardInfo.init_balance;					//�������M1��ԭ���
//		ErrCardInfo.error_occured = TRUE;
//		DevStat.purchase_again_req = TRUE;		//��������ˢ������
//		DevStat.voice_purchase_again_freq=100;
//// 		return notok;
//		return 0xF0;
//	}
//	i = CardMFCValueWithTransfer(PICC_DECREMENT, 9, 10, 0);
//	if ( i != ok )
//	{	//�ۿ��������¼�������ź�ԭ���
//		memcpy(&ErrCardInfo.m1_card_number[0], &CardInfo._CARD_NUMBER.detail_card_number.serial_number[0], 4);	//�������M1������
//		memcpy(&ErrCardInfo.csn[0], &CardInfo.csn[0], 4);				//�������M1��Ӳ������
//		ErrCardInfo.init_balance = CardInfo.init_balance;					//�������M1��ԭ���
//		ErrCardInfo.error_occured = TRUE;
//		DevStat.purchase_again_req = TRUE;		//��������ˢ������
//		DevStat.voice_purchase_again_freq=100;
//// 		return notok;
//		return 0xF0;
//	}
//
//	CardInfo.balance = CardInfo.init_balance - CardInfo.fare;//���潻�׺����
////	CardInfo.card_purchase_type = TT_NORMAL_CONSUME_MONEY;	//���ʽ�������
//	CardInfo.card_purchase_type = TT_NORMAL_CONSUME_MONEY;//��ͨ����������
//
////�����豸�������
//	
//	temp_int16u = ((INT16U)DevStat.dev_trade_no[0] << 8) | (INT16U)DevStat.dev_trade_no[1]; 			//��ʼ�������
//	if ( temp_int16u >= 0xFFFF )
//		temp_int16u = 0;
//	else
//		temp_int16u++;
//	DevStat.dev_trade_no[0] = (INT8U)(temp_int16u / 0x100);						//+1��Ľ������
//	DevStat.dev_trade_no[1] = (INT8U)(temp_int16u % 0x100);
//	WriteParam();
//
//	return ok;
//}
/*****************************************************************
 ����ԭ�ͣ�Lock_M1_Card_Process
 ����������M1�����������У�������������
 ����������
 �������ƣ�	����/�����	����		����
				
				
 ��  ��  ֵ��	ok(0)-���ѳɹ�		
				0xFF-����ʧ��
 ��      ��	������
 ��      �ڣ�2003-12-19
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
*****************************************************************/
INT8U Lock_M1_Card_Process(void)
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
	if(CardInit() != ok)
	{
		EA_ucMIFHalt(hMifsHandle);	//�ҿ�
		return notok;
	}
	
	i = Get_Sector_PermissionB(6, &KeySector[6][0]);
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
			;//����һ�£�ʲôҲ����
		}
		else
		{
			//������һ�£���24��Ϊ��
			m = CardMFCWrite16Bytes(25,block_buffer1);
			if ( m != ok )
				return notok;
		}

	}
	else if ( i == ok )
	{
		//24���Ϸ���24->25
		m = CardMFCWrite16Bytes(25,block_buffer1);
		if ( m != ok )
			return notok;
	}
	else if ( j == ok )
	{
		//25���Ϸ���25->24
		m = CardMFCWrite16Bytes(24,block_buffer2);
		if ( m != ok )
			return notok;
		memcpy(block_buffer1, block_buffer2, 16);		//�õ����ݴ���block_buffer1���±ߴ���������block_buffer1
	}
	else
	{
		return notok;		//���������Ϸ����˳�
	}

	////////////////////////////////////////
	//�жϴ˿��Ƿ��Ѿ��ú�������־
	////////////////////////////////////////
	if ( block_buffer1[7] == 0x04 )
	{
//  	Beep(500);
		printf_debug((char *)"�Ƿ���");
		Beeperr();	
		return notok;
	}

	////////////////////////////////////////
	//����������־λдΪ0x04
	////////////////////////////////////////
	block_buffer1[7] = 0x04;
	i = CardMFCWrite16Bytes(24, block_buffer1);
	if ( i != ok )
		return notok;

	i = CardMFCWrite16Bytes(25, block_buffer1);

	
	CardInfo.psam_purchase_serial_num = 0;//PSAM�������������
	CardInfo.balance = 0;//���׺��������
	CardInfo.fare = 0;//���׽������
	memset(&CardInfo.purchase_serial_num[0], 0x00, 2);//���׼���������  
	memset(&CardInfo.tac[0], 0x00, 4);		//tac������
	CardInfo.card_purchase_type = TT_BLACKNAME_CONSUME_TYPE;	//��������Ϊ����������
	(void)StoreRecord(&CardInfo);			//�����ۿ���¼
	////////////////////////////////////////
	//M1���ҿ�
	////////////////////////////////////////
	j = EA_ucMIFHalt(hMifsHandle);	//�ҿ�

	return ok;
}

/******************************************************************************
 �������ƣ�ICCardProcess
 ��������������������
 ����������
 �������ƣ�	����/�����	����		����
 
 ��  ��  ֵ��ok(0)-�ɹ�
				 notok(0xFF)-ʧ��
				 0xFE-û�ҵ���
				 0xF0-����ˢ��
				   
 ��      ��	������
 ��      �ڣ�2004-09-02
 �޸���ʷ��
		����		�޸���		�޸�����
		2005.2.18		myron	                   in function 
******************************************************************************/
INT8U ICCardProcess(void)
{
//     INT8U LOCAL_V_TYPE  j =0;
	INT8U LOCAL_V_TYPE  i = 0;  
	unsigned short  ICType;
	ulong ICSnr;
	uint  puiSize;

	i = EA_ucMIFRequest(hMifsHandle, EM_mifs_IDLE, &ICType);
	if ( i != EM_SUCCESS )
	{
		return 0xFE;
	}
	i = EA_ucMIFAntiColl(hMifsHandle,&ICSnr);
	if ( i != EM_SUCCESS )
	{
		return notok;
	}
	EA_ucMIFSelect(hMifsHandle,ICSnr, &puiSize);
	if ( i != EM_SUCCESS )
	{
		return notok;
	}

	i = M1CardProcess();
	if ( i != ok )
		return i;

	memset(&CardInfo, 0x00, sizeof(CardInfo));		//�忨Ƭ������Ϣ
	memset(&ErrCardInfo, 0x00, sizeof(ErrCardInfo));	//�������Ϣ
	DevStat.purchase_again_req = FALSE;					//������ˢ��������־
	DevStat.voice_purchase_again_freq=FALSE;
	return ok;
}
//
///*****************************************************************
// ����ԭ�ͣ�M1_Card_Credit_Process
// ����������M1���ѿ���ֵ����
// ����������
// �������ƣ� ����/����� ����        ����
//
//
// ��  ��  ֵ��   ok(0)-��ֵ�ɹ�
//                0xFF-��ֵʧ��
//                //0xFC-���Ǳ�ϵͳ��
//                //0xFD-�п�������û�лظÿ�
//                0xF0-����ˢ��
// ��      �� ������
// ��      �ڣ�2004-09-27
// �޸���ʷ��
//        ����        �޸���      �޸�����
//        2005.2.21       myron           in function
//*****************************************************************/
//INT8U M1_Card_Credit_Process(void)
//{
//    INT8U   LOCAL_V_TYPE    i = 0;
//    INT8U   LOCAL_V_TYPE    j = 0;
//    INT8U   LOCAL_V_TYPE    m = 0;
//    INT16U  LOCAL_V_TYPE    temp_int16u;
//    INT8S   LOCAL_V_TYPE    g = 0;
//    INT8U   LOCAL_V_TYPE    block_buffer1[16] = {0};
//    INT8U   LOCAL_V_TYPE    block_buffer2[16] = {0};
//    INT8U   LOCAL_V_TYPE    block9[16];
//    INT8U   LOCAL_V_TYPE    block10[16];
//    ulong   zero = 0;
//    INT32U  LOCAL_V_TYPE    temp_balance;
//    int     ret = 0;
//
////	uchar   ucResult;
//
//    ////////////////////////////////////////
//    //��֤Sector 6 ��Կ
//    ////////////////////////////////////////
//
//
////  if( CardInit() != ok )
////  {
////  	EA_ucMIFHalt(hMifsHandle);	//�ҿ�
////  	return notok;
////  }
//
//    i = Get_Sector_PermissionB( 6, &KeySector[6][0] );
//    if ( i != ok )
//    {
//        lcddisperr("no 1!");
//        return notok;
//    }
//
//    i = EA_ucMIFRead( hMifsHandle, 24, block_buffer1 );
//    if ( i != ok )
//    {
//        lcddisperr("no 2!");
//        return notok;
//    }
//    i = EA_ucMIFRead(hMifsHandle, 25, block_buffer2);
//    if ( i != ok )
//    {
//        lcddisperr("no 3!");
//        return notok;
//    }
//
//    i = Sec6_Block_Is_Legal(block_buffer1);
//    j = Sec6_Block_Is_Legal(block_buffer2);
//
//    if ( ( i == ok ) && ( j == ok ) )
//    {
//        //�������Ϸ��������������Ƿ�һ��
//        g = memcmp(block_buffer1, block_buffer2, 16);
//        if ( g == ok )
//        {
//            //����һ�£�ʲôҲ����
//            ;
//        }
//        else
//        {
//            //������һ�£���24��Ϊ��
////  		m = CardMFCWrite16Bytes(25,block_buffer1);
//            m = EA_ucMIFWrite( hMifsHandle, 25, block_buffer1 );
//            if ( m != ok )
//                return notok;
//        }
//
//    }
//    else if ( i == ok )
//    {
//        //24���Ϸ���24->25
//        m = EA_ucMIFWrite( hMifsHandle, 25, block_buffer1 );
//        if ( m != ok )
//            return notok;
//    }
//    else if ( j == ok )
//    {
//        //25���Ϸ���25->24
//        m = EA_ucMIFWrite( hMifsHandle, 24, block_buffer2 );
//        if ( m != ok )
//            return notok;
//        memcpy(block_buffer1, block_buffer2, 16);  //�õ����ݴ���block_buffer1���±ߴ���������block_buffer1
//    }
//    else
//    {
//        //���������Ϸ����˳�
//        EA_ucMIFHalt(hMifsHandle);  //�ҿ�
//        return notok;
//    }
//
//    ////////////////////////////////////////
//    //�жϴ˿��Ƿ��Ѿ��ú�������־
//    ////////////////////////////////////////
//    if ( block_buffer1[7] == 0x04 )
//    {
//        lcddisperr("�Ƿ���");
////  	EA_ucMIFHalt();	//�ҿ�
//        (void)EA_ucMIFHalt(hMifsHandle);
//        return notok;
//    }
//
//    CardInfo.credit_rec_pos = block_buffer1[8];
//
//    if(CardInfo.credit_rec_pos == 0x00 ||CardInfo.credit_rec_pos == 0xFF)
//        CardInfo.credit_rec_pos = 0x0C;
//    else
//        CardInfo.credit_rec_pos ++;
//
//    if(CardInfo.credit_rec_pos == 0x0f ||CardInfo.credit_rec_pos == 0x13 )
//        CardInfo.credit_rec_pos ++;
//    if(CardInfo.credit_rec_pos >= 0x17)
//        CardInfo.credit_rec_pos = 0x0C;
//
//
//    CardInfo.card_purchase_type = TT_CREDIT_MONEY;  //��ֵ��������
//    CardInfo.balance = CardInfo.init_balance + CardInfo.fare;//���潻�׺����
//
//    ////////////////////////////////////////////////////////
//    //�����ж�
//    ////////////////////////////////////////////////////////
//    if ( ErrCardInfo.error_occured == TRUE )                              //��֤�˽�����ſɶϲ����أ��������Ϊ���ݿ�������֮һ
//    {
//        //��������
//        memcpy(&ErrCardInfo.block24_buf_to_restore[12], "\x18\xE7\x18\xE7", 4);     //ǿ����Ϊ0x18E7
//        i = EA_ucMIFWrite(hMifsHandle, 24, &ErrCardInfo.block24_buf_to_restore[0]);
//        if ( i != ok )
//            return notok;
//
//        i = EA_ucMIFWrite(hMifsHandle, 25, &ErrCardInfo.block24_buf_to_restore[0]);
//        if ( i != ok )
//            return notok;
//    }
//    else
//    {
//        //��һ���뿨����
//        ////////////////////////////////////////
//        //���ڽ��׼���������
//        ////////////////////////////////////////
//        CardInfo.init_purchase_serial_num[0] = block_buffer1[0];                //��ʼ�������
//        CardInfo.init_purchase_serial_num[1] = block_buffer1[1];
//        temp_int16u = ((INT16U)block_buffer1[0] << 8) | (INT16U)block_buffer1[1];               //��ʼ�������
//        if ( temp_int16u >= 0xFFFF )
//            temp_int16u = 0;
//        else
//            temp_int16u ++;
//        CardInfo.purchase_serial_num[0] = (INT8U)(temp_int16u / 0x100);                     //+1��Ľ������
//        CardInfo.purchase_serial_num[1] = (INT8U)(temp_int16u % 0x100);
//
//        memcpy(&ErrCardInfo.block24_buf_to_restore[0], block_buffer1, 16);              //��24��Ӧ��д�������
//        memcpy(&ErrCardInfo.block24_buf_to_restore[0], &CardInfo.purchase_serial_num[0], 2);
//        ErrCardInfo.block24_buf_to_restore[8] = CardInfo.credit_rec_pos;
//        memcpy(&ErrCardInfo.block24_buf_to_restore[12], "\x18\xE7\x18\xE7", 4);     //ǿ����Ϊ0x18E7
//        i = EA_ucMIFWrite(hMifsHandle, 24, &ErrCardInfo.block24_buf_to_restore[0]);
//        if ( i != ok )
//            return notok;
//
//        i = EA_ucMIFWrite(hMifsHandle, 25, &ErrCardInfo.block24_buf_to_restore[0]);
//        if ( i != ok )
//            return notok;
//    }
//
////�����豸�������
//
//    temp_int16u = ((INT16U)DevStat.dev_trade_no[0] << 8) | (INT16U)DevStat.dev_trade_no[1];             //��ʼ�������
//    if ( temp_int16u >= 0xFFFF )
//        temp_int16u = 0;
//    else
//        temp_int16u++;
//    DevStat.dev_trade_no[0] = (INT8U)(temp_int16u / 0x100 );                        //+1��Ľ������
//    DevStat.dev_trade_no[1] = (INT8U)(temp_int16u % 0x100 );
//    WriteParam();
//
//
//    memcpy(&CardInfo.dev_trade_no[0],&DevStat.dev_trade_no[0], 2);              //PSAM�������������
//
//    CardInfo.psam_purchase_serial_num = 0;
//
//
//    ////////////////////////////////////////
//    //��֤Sector 2 ��Կ
//    ////////////////////////////////////////
//
////  if( CardInit() != ok )
////  {
////  	return notok;
////  }
//
//    i = Get_Sector_PermissionB(2, &KeySector[2][0]);
//    if ( i != ok )
//    {
//        lcddisperr("no 3");
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
//        g = memcmp( block9, block10, 16 );
//        if ( g == ok )
//        {
//            ;           //����һ�£�ʲôҲ����
//        }
//        else
//        {
//            m = CardMFCValueWithTransfer(1, 9, 10, zero);
//            if ( m != ok )
//                return notok;
//        }
//    }
//    else if ( i == ok )
//    {
//        m = CardMFCValueWithTransfer(1, 9, 10, zero);      //9���Ϸ���9->10
//        if ( m != ok )
//            return notok;
//    }
//    else if ( j == ok )
//    {
//        m = CardMFCValueWithTransfer(1, 10, 9, zero);     //10���Ϸ���10->9
//        if ( m != ok )
//            return notok;
//
//        memcpy(block9, block10, 16);        //�õ����ݴ���block9�±ߴ���������block9
//    }
//    else
//    {
//        EA_ucMIFHalt(hMifsHandle);  //�ҿ�
//        return notok;       //���������Ϸ����˳�
//    }
//
//    ////////////////////////////////////////
//    //�����ۿ��
//    ////////////////////////////////////////
//    if ( ErrCardInfo.error_occured == TRUE )
//    {   //�����ؿ����������������һ�����Ѿ��۹����ˣ���һ���ǻ�û�۹���
//        temp_balance = BlockToCash(block9);
//        if ( temp_balance == ErrCardInfo.init_balance )
//        {   //�ۿ�û�ɹ�
//            //���½����ߵ�һ�οۿ��
//            //�жϲ���Ա�󿨽���Ƿ����仯
//        }
//        else
//        {   //�ۿ�ɹ�(�������������Ҳ��ۿ�ɹ�)
//            CardInfo.balance = temp_balance;//���潻�׺����
//            CardInfo.card_purchase_type = TT_CREDIT_MONEY;  //��ѿ�����
//
////			if ( (DevStat.oldman_free == TRUE && CardInfo.card_type == CARDT_OLDMAN)
////				 || (DevStat.employee_a_free == TRUE && CardInfo.card_type == CARDT_FREE)
////				 || (DevStat.driver_free == TRUE && CardInfo.card_type == CARDT_DRIVER) )
////			{
////				CardInfo.card_purchase_type = TT_FREE;	//��ѿ�����
////			}
////			else
////			{
////				CardInfo.card_purchase_type = TT_NORMAL_CONSUME_MONEY;//��ͨ����������
////			}
//
//            return ok;
//        }
//    }
//
//    ////////////////////////////////////////
//    //��һ���뿨�ۿ��
//    ////////////////////////////////////////
//    if ( ErrCardInfo.error_occured == FALSE )           //ֻ���ڵ�һ���뿨ʱ���ű����ʼ���Ժ��״򲻶�
//    {
//        CardInfo.init_balance = BlockToCash(block9);
//    }
//
//    if ( ( CardInfo.init_balance + CardInfo.fare ) > DevStat.max_card_limit)
//    {
//
//        //���㴦��
//        lcddisperr("��������");
//        memset(&CardInfo, 0x00, sizeof(CardInfo));      //�忨Ƭ������Ϣ
//        memset(&ErrCardInfo, 0x00, sizeof(ErrCardInfo));    //�������Ϣ
//        DevStat.purchase_again_req = FALSE;//������ˢ��������־
//        DevStat.voice_purchase_again_freq = 0;
//        EA_ucMIFHalt(hMifsHandle);  //�ҿ�
//        return notok;
//    }
//
//    if ( ErrCardInfo.error_occured == FALSE )           //ֻ���ڵ�һ���뿨ʱ���ű����ʼ���Ժ��״򲻶�
//    {
//        StoreGrayRecord();
//    }
//    //�۳�����Ա���ڽ��
////  Cpucard_Init();
//    ret = OperTradeProcess(0, CardInfo.fare);
//    if(ret != ok )
//    {
//        Cpucard_power_off();
//        ErrCardInfo.error_occured = TRUE;
//        lcddisperr("�󿨴����ܽ���");
////  	MC530Off();
////  	InitMC530(TYPE_A_ISO14443);
//        EA_ucMIFHalt(hMifsHandle);  //�ҿ�
//        return 0xF0;
//    }
////  Cpucard_power_off();
////	}
//
//// 	SleepMs(500);
////	i = EA_ucMIFTransfer(PICC_INCREMENT, 9, 9,CardInfo.fare);
////  i = EA_ucMIFIncrement(hMifsHandle, 9, CardInfo.fare);
////  i = EA_ucMIFTransfer(hMifsHandle, 9);
//    i = CardMFCValueWithTransfer( 1, 9, 9, CardInfo.fare );
//    if ( i != ok )
//    {   //�ۿ��������¼�������ź�ԭ���
//        memcpy(&ErrCardInfo.m1_card_number[0], &CardInfo._CARD_NUMBER.detail_card_number.serial_number[0], 4);  //�������M1������
//        memcpy(&ErrCardInfo.csn[0], &CardInfo.csn[0], 4);               //�������M1��Ӳ������
//        ErrCardInfo.init_balance = CardInfo.init_balance;                   //�������M1��ԭ���
//        ErrCardInfo.error_occured = TRUE;
//        DevStat.purchase_again_req = TRUE;      //��������ˢ������
//        DevStat.voice_purchase_again_freq = 100;
//// 		return notok;
////		printf_debug("222222222222");
//        EA_ucMIFHalt(hMifsHandle);  //�ҿ�
//        return 0xF0;
//    }
////  i = CardMFCValueWithTransfer(PICC_INCREMENT, 9, 10, 0);
////  i = EA_ucMIFIncrement(hMifsHandle, 9, 0);
////  i = EA_ucMIFTransfer(hMifsHandle, 10);
//    i = CardMFCValueWithTransfer( 1, 9, 10, 0 );
//    if ( i != ok )
//    {   //�ۿ��������¼�������ź�ԭ���
//        memcpy(&ErrCardInfo.m1_card_number[0], &CardInfo._CARD_NUMBER.detail_card_number.serial_number[0], 4);  //�������M1������
//        memcpy(&ErrCardInfo.csn[0], &CardInfo.csn[0], 4);               //�������M1��Ӳ������
//        ErrCardInfo.init_balance = CardInfo.init_balance;                   //�������M1��ԭ���
//        ErrCardInfo.error_occured = TRUE;
//        DevStat.purchase_again_req = TRUE;      //��������ˢ������
//        DevStat.voice_purchase_again_freq = 100;
//// 		return notok;
////		printf_debug("111111");
//        EA_ucMIFHalt(hMifsHandle);  //�ҿ�
//        return 0xF0;
//    }
//
//    CardInfo.card_purchase_type = TT_CREDIT_MONEY;  //��ֵ��������
//    CardInfo.balance = CardInfo.init_balance + CardInfo.fare;//���潻�׺����
//
////	printf_debug("3333333");
//
//    ////////////////////////////////////////
//    //��֤Sector 3.4.5 ��Կ��д���׼�¼
//    ////////////////////////////////////////
////  if(CardInit() != ok)
////  	return ok;
//
//    m = (CardInfo.credit_rec_pos -12 )/4  + 3;//������
//    i = Get_Sector_PermissionB(m, &KeySector[m][0]);
//    if ( i != ok )
//        return ok;
//    memcpy(&CardInfo.blockcreditrec[0], &CardInfo.card_in_time.year, 5);//yymmddhhmi
//    CardInfo.blockcreditrec[5] = (CardInfo.init_balance / 0x10000);//��Ƭ���
//    CardInfo.blockcreditrec[6] = (CardInfo.init_balance % 0x10000 / 0x100);//��Ƭ���
//    CardInfo.blockcreditrec[7] = (CardInfo.init_balance % 0x100);//��Ƭ���
//
//    CardInfo.blockcreditrec[8] = ( CardInfo.fare / 0x10000 );//���׽��
//    CardInfo.blockcreditrec[9] = ( CardInfo.fare % 0x10000 / 0x100);//���׽��
//    CardInfo.blockcreditrec[10] = ( CardInfo.fare % 0x100);//���׽��
//
//    memcpy(&CardInfo.blockcreditrec[11],&DevStat.equ_id[1],2);//�豸��
//    memcpy(&CardInfo.blockcreditrec[13], &DevStat.cur_driver_number[1],3);//����Ա��        //by liujihua
//
//    i = EA_ucMIFWrite(hMifsHandle, CardInfo.credit_rec_pos, &CardInfo.blockcreditrec[0]);
//    //��ֵ�����
//    CardInfo.balance = CardInfo.init_balance + CardInfo.fare;//���潻�׺����
//
////	if ( (DevStat.oldman_free == TRUE && CardInfo.card_type == CARDT_OLDMAN)
////		 || (DevStat.employee_a_free == TRUE && CardInfo.card_type == CARDT_FREE)
////		 || (DevStat.driver_free == TRUE && CardInfo.card_type == CARDT_DRIVER) )
////	{
////		CardInfo.card_purchase_type = TT_FREE;	//��ѿ�����
////	}
////	else
////	{
////		CardInfo.card_purchase_type = TT_NORMAL_CONSUME_MONEY;//��ͨ����������
////	}
//    lcddisperr("��ֵ�ɹ�!");
//    return ok;
//}
//
/******************************************************************************
 �������ƣ�CardInit
 ������������Ƭ��ʼ��,����ҿ�����
 ����������
 �������ƣ�����/�����	����		����
 
 ��  ��  ֵ��    ok(0)-�ɹ�
				 notok(0xFF)-ʧ��
				 0xFE-û�ҵ���
				 0xF0-����ˢ��
				   
 ��      ��	���ڸ���/������
 ��      �ڣ�2012-12-10
 �޸���ʷ��
		����		�޸���		�޸�����
******************************************************************************/
INT8U CardInit(void)
{
	uchar	ucResult;
	ushort	usICType;
	ulong	ulSnr;
	INT8U   kind_of_card;
	uint	uiSize;

	ucResult = EA_ucMIFRequest(hMifsHandle, EM_mifs_IDLE, &usICType);
	if (ucResult != EM_SUCCESS)
	{
//  	lcddisperr("M1����ȡ����ʧ��");
		return notok;
	}

	ucResult = EA_ucMIFAntiColl(hMifsHandle, &ulSnr);
	if (ucResult != EM_SUCCESS)
	{
//  	lcddisperr("M1����ȡ����ʧ��");
		return notok;
	}

	ucResult = EA_ucMIFSelect(hMifsHandle, ulSnr, &uiSize);
	if (ucResult != EM_SUCCESS)
	{
//  	lcddisperr("M1����ȡ������ʧ��");
		return notok;
	}

	CardInfo.csn[3] = (INT8U)(ulSnr / 0x1000000);
	CardInfo.csn[2] = (INT8U)((ulSnr % 0x1000000) / 0x10000);
	CardInfo.csn[1] = (INT8U)((ulSnr % 0x10000) / 0x100);
	CardInfo.csn[0] = (INT8U)(ulSnr % 0x100);

	kind_of_card = usICType;

//#ifdef _debug_m1card_
//    EA_vDisplay(3, "������Ϊ:%02x", kind_of_card);
//    SleepMs(2000);
//#endif

//  if ( kind_of_card != M1_CARD )
//  {
//  	lcddisperr("����M1��");   //frank -- �ڳ���ǰ������
//  	return notok;
//  }
	//ȡ��Ӳ������
	return ok;
}


