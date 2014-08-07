/******************************************************************************
Copyright 2012 	�����״�ͨ���Ӽ������޹�˾
All rights riserved.
�� �� ���� m1card.c
ģ�����ƣ� E550 ˢ���ӳ���
���ܸ�Ҫ�� ������
*****************************************************************/
#include "main.h"
//#define  Debug_Com
//#define _debug_m1card_      //������Ϣ��by frank ,��ʽ����ʱҪȥ��
//#define  _zb_debug_sd_
//#define  _zb_debug_tran_

extern DevHandle hCpuHandle;      //CPU��
extern DevHandle hMifsHandle;
extern DevHandle hSam1Handle;
extern ET_PART_INFO ptPartInfo;
extern DEV_STAT    DevStat;
ERROR_CARD_INFO ErrCardInfo;
extern CARD_INFO   CardInfo;
extern ET_DISK_INFO ptDiskInfo;
char send_buffer[1024];



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
extern INT8U const   GET_M1_FUZA_KEY[5];		//����M1��������Կ
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
 sector		����		INT8U		Ҫȡ��Ȩ�޵���
 key		����		INT8U *		Ҫȡ��Ȩ��������Կ
				
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
		//lcddisperr("��Կ��֤����!");
		return notok;
	}
	else
	{
		return ok;
	}
}

/******************************************************************************
 �������ƣ�M1CardProcess
 ����������M1����������
 ����������
 �������ƣ�	����/�����	����		����
 
 ��  ��  ֵ��ok(0)-�ɹ�
				 notok(0xFF)-ʧ��
				 0xF0-����ˢ��
				   
 ��      ��	��������
 ��      �ڣ�2014-02-22
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
******************************************************************************/
INT8U M1CardProcess(void)
{
	INT8U     i = 0;					
	uint      rcv_length;
	INT16U    k = 0;
	INT8U     temp_rcv_buf[50];
	INT8U     temp_snd_buf[50];
	INT8U     block_buffer1[16] = {0};			//M1��һ��block�����ݻ���


#ifndef Debug_M1card

	 i = PsamRst();
	 if ( i != ok )
	 {
		 lcddisperr((char *)"PSAM������");
		 return notok;
	 }
	
	////////////////////////////////////////
	//����ʱ��
	////////////////////////////////////////
	i = SaveCardInTime(&CardInfo);
	if ( i != ok )
	{
		lcddisperr("ʱ�����");
		return notok;
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
		lcddisperr((char *)"������ˢ��1");
		return notok;
	}
		
	i = EA_ucMIFRead(hMifsHandle, 4, block_buffer1);
	if ( i != ok )
	{
		lcddisperr((char *)"������ˢ��2");
		return notok;
	}
		
	if ( block_buffer1[12] != 0x01 )	  //�ж����ñ�־
	{
		//��δ���ô���
		lcddisperr((char *)"��Ƭδ����");
		Beeperr();
		return notok;
	}

	//���濨��Ӧ��ϵ�к�
	memcpy(&CardInfo._CARD_NUMBER.detail_card_number.city_code[0], block_buffer1, 2);   //���д���
	memcpy(&CardInfo._CARD_NUMBER.detail_card_number.calling_code[0], block_buffer1 + 2, 2);  //��ҵ����
	memcpy(&CardInfo._CARD_NUMBER.detail_card_number.serial_number[0], &block_buffer1[4], 4); //������ˮ��
	memcpy(&CardInfo.authentication_code[0], &block_buffer1[8], 4); //���濨��֤��
	CardInfo.card_type = block_buffer1[13]; //������

#ifdef _debug_m1card_
	EA_vDisplay(3, "���Ϳ�����:%02x", block_buffer1[12]);
	SleepMs(2000);
#endif									    


	////////////////////////////////////////
	//PSAM��ѡ1003Ŀ¼
	////////////////////////////////////////
	k = CmdPsam(hSam1Handle, (INT8U *)SELECT_SAM_ADF1, sizeof(SELECT_SAM_ADF1),
				temp_rcv_buf, &rcv_length);     //ѡ1001Ŀ¼
	if ( k != ok )
	{
		lcddisperr("PSAM��ѡ3F01Ŀ¼����");
		return notok;
	}

	////////////////////////////////////////
	//PSAM���������Կ
	////////////////////////////////////////
	memcpy(temp_snd_buf, (INT8U *)GET_M1_KEY, sizeof(GET_M1_KEY)); //ͷ
	memcpy(&temp_snd_buf[5], &CardInfo._CARD_NUMBER.detail_card_number.city_code[0], 2); //���д���
	memcpy(&temp_snd_buf[7], &CardInfo.csn[0], 4); //CSNӲ������
	memcpy(&temp_snd_buf[11], &CardInfo._CARD_NUMBER.detail_card_number.serial_number[2], 2); //��������ˮ�ź�2�ֽ�
	memcpy(&temp_snd_buf[13], &CardInfo.authentication_code[0], 4); //����֤��
	temp_snd_buf[17] = 0x02;
	temp_snd_buf[18] = 0x06;
	temp_snd_buf[19] = 0x0a;
	temp_snd_buf[20] = 0x0b;
	temp_snd_buf[21] = 0x0c; //����sector2��6,10,11,12����Կ
	k = CmdPsam(hSam1Handle, temp_snd_buf, 22, temp_rcv_buf, &rcv_length); //��MAC1
	if ( k != ok )
	{
		lcddisperr("��MAC1����");
		return notok;
	}
	memcpy(&KeySector[2][0], &temp_rcv_buf[0], 6);   //������������Կ��2sector
	memcpy(&KeySector[6][0], &temp_rcv_buf[6], 6);   //������������Կ��6sector
	memcpy(&KeySector[10][0], &temp_rcv_buf[12], 6); //������������Կ��10sector
	memcpy(&KeySector[11][0], &temp_rcv_buf[18], 6); //������������Կ��11sector
	memcpy(&KeySector[12][0], &temp_rcv_buf[24], 6); //������������Կ��12sector
	

    ////////////////////////////////////////
	//��Sector 11 ��44 ����Ϣʱ�����Ϣ
	//////////////////////////////////////
	i = Get_Sector_PermissionA(11, &KeySector[11][0]);
	if ( i != ok )
	{
		lcddisperr((char *)"������ˢ��3");
		return notok;
	}

	i = EA_ucMIFRead(hMifsHandle, 44, block_buffer1);
	if ( i != ok )
	{
		lcddisperr((char *)"������ˢ��4");
		return notok;
	}

	memcpy(&CardInfo.driver_number[0], &block_buffer1[3], 3);//Ա���� ����

//  memcpy(&CardInfo.authentication_code[0], &block_buffer1[8], 4);						//���濨��֤��

	//////////////////////////////////////////////////////////////
	//��������˿���û�б������ӿ���õ�������
	//////////////////////////////////////////////////////////////
#ifdef Debug_Com
	sprintf(send_buffer, "\r\n CSN:%02X%02X%02X%02X\r\n",
			CardInfo.csn[0], CardInfo.csn[1], CardInfo.csn[2], CardInfo.csn[3]);
	Debugprintf(send_buffer);
	sprintf(send_buffer, "\r\n code:%02X%02X%02X%02X\r\n",
			CardInfo.authentication_code[0], CardInfo.authentication_code[1],
			CardInfo.authentication_code[2], CardInfo.authentication_code[3]);
	Debugprintf(send_buffer);
	sprintf(send_buffer, "\r\n DRIVER:%02X%02X%02X\r\n",
			CardInfo.driver_number[0], CardInfo.driver_number[1], 
			CardInfo.driver_number[2]);
	Debugprintf(send_buffer);

	sprintf(send_buffer, "\r\n bus_num:%s\r\n",
			(char *)CardInfo.bus_number);
	Debugprintf(send_buffer);
#endif

//  //////////////////////////////////////////////////////////////
//  //�鿴�˿� ��Կ�Ƿ���ȷ��
//  //////////////////////////////////////////////////////////////
//  i = Cal_Keyval();
//  if( i !=ok )
//  {
//  	lcddisperr("����δ���!");
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
			//CalTAC();		  //����TAC��
			EA_ucMIFHalt(hMifsHandle);                          //�ҿ�
			StoreRecord(&CardInfo);			   			        //�������Ѽ�¼	
			print_sale_record();                                //��ӡ���ν�������
			//�ָ�״̬
			memset(&CardInfo, 0x00, sizeof(CardInfo));			//�忨Ƭ������Ϣ
			memset(&ErrCardInfo, 0x00, sizeof(ErrCardInfo));	//�������Ϣ
			DevStat.purchase_again_req = FALSE;					//������ˢ��������־
			DevStat.voice_purchase_again_freq = FALSE;
			return ok;
		}
	}

	lcddisperr("����δ���!");
	EA_ucMIFHalt(hMifsHandle);                          //�ҿ�
	return i;
}
/*****************************************************************
����ԭ�ͣ�trans_to_int32u
������������4�ֽ�����ת��ΪINT32U����
����������
�������ƣ�	����/����� ����		����
buf			����		INT8U *		��Ҫ����ת�����ַ���
			
��	��	 ֵ��ת����� 
 
��		 �ߣ�����
��		 �ڣ�2003-11-27
�޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
*****************************************************************/
INT32U trans_to_int32u(INT8U *buf)
{
	INT32U i = 0;

	i = (INT32U)buf[0] * 0x1000000 + (INT32U)buf[1] * 0x10000 + (INT32U)buf[2] * 0x100 + (INT32U)buf[3];
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
	INT8U  i = 0;
	INT8U  record[57];
	INT32U temp_int32u = 0;
	uchar  buf[100];
	int    buf_len = 0;

	RECORD * prec = (RECORD*)record;
	
	memset(record, 0x00, sizeof(record));
	memset(buf, 0, sizeof(buf));

	//////////////////////////////////////////
	//��֯���׼�¼��ʽ����ͬ����
	//////////////////////////////////////////

	temp_int32u = query_rec_num();
	if(temp_int32u >= 2000)
	{
		lcddisperr("�뾡�콫��¼�ϴ�");
		temp_int32u = 2000;
	}

	memcpy((INT8U *)&prec->pos_purchase_serial_num[0], (INT8U *)&cardinfo->purchase_serial_num[0], 3);
	var_bcd2asc(buf+buf_len, prec->pos_purchase_serial_num, 3);
	buf_len+=6;
#ifdef _zb_debug_tran_
	scrShowMsgInfo((char *)"num", buf+buf_len-6, 6, ORG_ASCII);
#endif
	//���ƺ�
    memcpy((INT8U *)&prec->bus_number[0], (INT8U *)&cardinfo->bus_number[0], 5);				
//  var_bcd2asc(buf+buf_len, &cardinfo->bus_number[0], 5);
	memcpy(buf+buf_len, &prec->bus_number[0], 5);
	buf_len+=5;
#ifdef _zb_debug_tran_
		scrShowMsgInfo((char *)"bus_num", buf+buf_len-5, 5, ORG_ASCII);
#endif
	//��������4�ֽں�ʱ�� 3�ֽ�
	memcpy((BUS_TIME *)&prec->card_in_time, (BUS_TIME *)&cardinfo->card_in_time, 7);		
	var_bcd2asc(buf+buf_len, (uchar *)&prec->card_in_time, 7);
	buf_len+=14;
#ifdef _zb_debug_tran_
		scrShowMsgInfo((char *)"date", buf+buf_len-14, 14, ORG_ASCII);
#endif
	//���ͽ��
//  memset(tmpbuf, 0, 10);                                                  
//  packInt(tmpbuf, cardinfo->fare);
	memcpy(&prec->fare, &cardinfo->fare, 4);
//  var_bcd2asc(buf+buf_len, (uchar *)&cardinfo->fare, 4);
//  buf_len+=8;
	sprintf((char *)(buf+buf_len), (char *)"%08d", cardinfo->fare);
	buf_len+=8;
#ifdef _zb_debug_tran_
		scrShowMsgInfo((char *)"fare", buf+buf_len-8, 8, ORG_ASCII);
#endif	
//  memset(tmpbuf, 0, 10);	                                                //��������
//  packInt(tmpbuf, cardinfo->capacity);
	memcpy(&prec->capacity, &cardinfo->capacity, 4);
//  var_bcd2asc(buf+buf_len, (uchar *)&prec->capacity, 4);
	sprintf((char *)(buf+buf_len), (char *)"%08d", prec->capacity);
	buf_len+=8;
#ifdef _zb_debug_tran_
		scrShowMsgInfo((char *)"cap", buf+buf_len-8, 8, ORG_ASCII);
#endif	
																		
	memcpy(&prec->driver_no[0], (INT8U *)(&cardinfo->driver_number[0]), 3); //Ա��������
	var_bcd2asc(buf+buf_len, &prec->driver_no[0], 3);
	buf_len+=6;
#ifdef _zb_debug_tran_
		scrShowMsgInfo((char *)"driver_no", buf+buf_len-6, 6, ORG_ASCII);
#endif

	memcpy(&prec->acnt_id[0], &DevStat.acnt_id[0], 2);                     //վ���
	var_bcd2asc(buf+buf_len, prec->acnt_id, 2);
	buf_len+=4;
#ifdef _zb_debug_tran_
		scrShowMsgInfo((char *)"acnt_id", buf+buf_len-4, 4, ORG_ASCII);
#endif

	memcpy(&prec->equ_id[0], &DevStat.equ_id[0], 3);                      //�豸���
	var_bcd2asc(buf+buf_len, prec->equ_id, 3);
	buf_len+=6;
#ifdef _zb_debug_tran_
		scrShowMsgInfo((char *)"equ_id", buf+buf_len-6, 6, ORG_ASCII);
#endif

	memcpy(&prec->fuel_type, &DevStat.fill_mode, 1);                        //��Ʒ
	var_bcd2asc(buf+buf_len, &prec->fuel_type, 1);
	buf_len+=2;

#ifdef _zb_debug_tran_
		scrShowMsgInfo((char *)"fuel_type", buf+buf_len-2, 2, ORG_ASCII);
#endif	
																		
	memcpy((uchar *)&prec->price, (uchar *)&DevStat.price[DevStat.fill_mode], 4); //����
	var_bcd2asc(buf+buf_len, (uchar *)&prec->price, 4);
	sprintf((void *)(buf+buf_len), "%08d", prec->price);
	buf_len+=8;
#ifdef _zb_debug_tran_
		scrShowMsgInfo((char *)"fuel_price", buf+buf_len-8, 8, ORG_ASCII);
#endif
																		
	memcpy(&prec->oper_id[0], &DevStat.oper_cardno[0], 3);            //����Ա��
	var_bcd2asc(buf+buf_len, &prec->oper_id[0], 3);
	buf_len+=6;
#ifdef _zb_debug_tran_
		scrShowMsgInfo((char *)"oper_id", buf+buf_len-6, 6, ORG_ASCII);
#endif

//  crc = Cal_CRC(record, sizeof(record));
//	Debugprintf((char *)buf);

	i = StoreRectoSD(prec);
	if( i == notok )
	{
		lcddisperr("���ν���δ����");
		Beeperr();
		return notok;
	}

	i = SaveCardRecord(prec);	
	if( i == notok)
	{
		lcddisperr("���ν��ײ��ɹ�");
		Beeperr();
		return notok;
	}
	
	i = prec->fuel_type;

	if( i <= 3 )
	{
		DevStat.binhand.consume_cnt[i] ++;
		DevStat.binhand.consume_cap[i] += prec->capacity;
		DevStat.binhand.consume_amt[i] += prec->fare;
	}

	WriteParam();
	
	return ok;
}
/////////////////////////////////////////////////
//
/////////////////////////////////////////////////
INT8U StoreRectoSD(RECORD * prec)
{
	FILE *fp = NULL; 
	uchar buf[100];
	char  path[50];
	int   buf_len = 0;
	long  length;

	memset(buf, 0, sizeof(buf));
	memset(path, 0, sizeof(path));

//  frank -- 2014,4,8�ã�����ȥ���Ե�ʱ��Ҫ�޸Ļ��������������洢��
//  htoa(buf, (void *)&prec->card_in_time, 6);
//  sprintf(path, "%s/%s", ptPartInfo.acHome, buf);
	sprintf(path, "%s/%s", ptPartInfo.acHome, "backup.txt");

	if (pfexist(path) == FALSE) {
		fp = fopen(path, "w+");
	} else {
		fp = fopen(path, "r+");
	}

	fseek(fp, 0L, SEEK_END);
	length=ftell(fp);

	if(length >= ptDiskInfo.ullSize - 100)
	{
		lcddisperr("SD���Ѿ��洢��!");
		Beeperr();
		return notok;
	}

#ifdef _zb_debug_sd_
		Debugprintf((char *)"<<<<<<<<<<<<<_debug_sd_<<<<<<<<<<<<\n");
#endif
	//4B �̻���
	var_bcd2asc(buf+buf_len, prec->acnt_id, 2);
	buf_len+=4;
#ifdef _zb_debug_sd_
		scrShowMsgInfo((char *)"acnt_id", buf+buf_len-4, 4, ORG_ASCII);
#endif
	//6B �豸���
	var_bcd2asc(buf+buf_len, prec->equ_id, 3);
	buf_len+=6;
#ifdef _zb_debug_sd_
		scrShowMsgInfo((char *)"equ_id", buf+buf_len-6, 6, ORG_ASCII);
#endif
	//6B ����Ա��
	var_bcd2asc(buf+buf_len, &prec->oper_id[0], 3);
	buf_len+=6;
#ifdef _zb_debug_sd_
		scrShowMsgInfo((char *)"oper_id", buf+buf_len-6, 6, ORG_ASCII);
#endif
	//6B ������ź�
	var_bcd2asc(buf+buf_len, prec->pos_purchase_serial_num, 3);
	buf_len+=6;
#ifdef _zb_debug_sd_
	scrShowMsgInfo((char *)"num", buf+buf_len-6, 6, ORG_ASCII);
#endif
//5B ����
	memcpy(buf+buf_len, &prec->bus_number[0], 5);
//  var_bcd2asc(buf+buf_len, &prec->bus_number[0], 5);
	buf_len+=5;
#ifdef _zb_debug_sd_
		scrShowMsgInfo((char *)"bus_num", buf+buf_len-5, 5, ORG_ASCII);
#endif
	//6B ����Ա��
	var_bcd2asc(buf+buf_len, &prec->driver_no[0], 3);
 buf_len+=6;
#ifdef _zb_debug_sd_
	 scrShowMsgInfo((char *)"driver_no", buf+buf_len-6, 6, ORG_ASCII);
#endif
	//1B ��������
	var_bcd2asc(buf+buf_len, &prec->fuel_type, 1);
	buf_len+=2;
#ifdef _zb_debug_sd_
		scrShowMsgInfo((char *)"fuel_type", buf+buf_len-2, 2, ORG_ASCII);
#endif	
	//8B ���͵���
	sprintf((char *)(buf+buf_len), "%08d", prec->price);
	buf_len+=8;
#ifdef _zb_debug_sd_
		scrShowMsgInfo((char *)"fuel_price", buf+buf_len-8, 8, ORG_ASCII);
#endif
		//8B ���ͽ��
//  var_bcd2asc(buf+buf_len, (uchar *)&prec->fare, 4);
	sprintf((char *)(buf+buf_len), "%08d", prec->fare);
	buf_len+=8;
#ifdef _zb_debug_sd_
		scrShowMsgInfo((char *)"fare", buf+buf_len-8, 8, ORG_ASCII);
#endif	

//  var_bcd2asc(buf+buf_len, (uchar *)&prec->capacity, 4);
		//8B ��������
	sprintf((char *)(buf+buf_len), "%08d", prec->capacity);
	buf_len+=8;
#ifdef _zb_debug_sd_
		scrShowMsgInfo((char *)"cap", buf+buf_len-8, 8, ORG_ASCII);
#endif	
//14B ����ʱ��
	var_bcd2asc(buf+buf_len, (uchar *)&prec->card_in_time, 7);
	buf_len+=14;
#ifdef _zb_debug_sd_
		scrShowMsgInfo((char *)"date", buf+buf_len-14, 14, ORG_ASCII);
#endif

	strcat((char *)buf, "\r\n");
	if (fwrite(buf, strlen((void *)buf), 1, fp) == FALSE)	
	{
		pfclose(fp);
		lcddisperr("дSD�ļ�ʧ��");
		return notok;
	}

//  memset(buf, 0, sizeof(buf));
//  if(pfread(fp, (char *)buf, sizeof(RECORD)) == FALSE)
//  {
//  	pfclose(fp);
//  	lcddisperr("��SD�ļ�ʧ��");
//  	return notok;
//  }

//#ifdef _debug_sd_
//    Debugprintf((char *)buf);
//#endif

	pfclose(fp);
	return ok;
}


FILE * pfopen(char *filepath, char *mode)
{
	return (fopen(filepath, mode));
}

size_t pfread(FILE *fh, char *dest, long num)
{
	return (fread((char *)dest, sizeof(char), num, fh));
}

//////////////////////////////////////////////////////////////////////////
//
size_t pfwrite(FILE *fh, char *src, long num)
{
	return (fwrite((char *)src, sizeof(char), num, fh));
}

void pfseek(FILE *fh, long seek, int mode)
{
	fseek(fh, seek, mode);
}
//////////////////////////////////////////////////////////////////////////
//
void pfclose(FILE *fh)
{
	fclose(fh);
}
//�ļ������򷵻�true;
int pfexist(char *filename)
{
	return (access(filename, 0) == 0); 
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

/******************************************************************************
 �������ƣ�INT8U Cal_Keyval(void)
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
INT8U Cal_Keyval(void)
{
	uchar i = 0;
	INT8U input[22];
	char tmp_key[10];

	memset(tmp_key, 0, sizeof(tmp_key));
	EA_vCls();
	EA_vDisplay(1, "�� �� Ա:%02X%02X%02X", CardInfo.driver_number[0],CardInfo.driver_number[1],CardInfo.driver_number[2]);
	EA_vDisplay(2, "��������:");
	EA_vDisplay(4, "�밴<ȷ��>������");
	strcpy((void *)input, "");

	sprintf(tmp_key, "%02X%02X%02X%02X", CardInfo.authentication_code[0], CardInfo.authentication_code[1]
			,CardInfo.authentication_code[2], CardInfo.authentication_code[3]);

	for ( ;; )
	{
		i = EA_ucGetInputStr(2, 10, 20, EM_BIG_FONT | EM_MODE_NUMBER | EM_ALIGN_LEFT | EM_SHOW_PASSWORD
							 , 8, 8, 0, (void *)input);
		if ( i == EM_SUCCESS )
			break;
		if ( i == EM_ABOLISH )
			return 2;
	}
	if ( memcmp((void *)input, tmp_key, 4) == 0 )
	{
		return ok;
	}
	else
	{
		EA_ucSetInverse(EM_lcd_INVON);
		lcddisp(4, 1, (void *)"      �������      ");
		EA_ucSetInverse(EM_lcd_INVOFF);
		(void)EA_uiInkey(1);
		return notok;
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
	INT8U p[67];
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

	memcpy(l_rec_buf, p, 67);
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
	INT32U  temp_int32u;
	uchar   i;
	double   temp_cap = 0.00;
	double   temp_fare = 0.00;

reboot:
	memset(buf, 0, sizeof(buf));

	//���Ѽ�¼������
	EA_vCls();
	EA_ucClrKeyBuf();
	Beep(1000);

	EA_vDisplay(1, "��  �� :³C-%s", CardInfo.bus_number);

	EA_ucSetInverse(EM_lcd_INVOFF);

	switch( DevStat.fill_mode )
	{
	   case 0:
		   EA_vDisplay(2, "��Ȼ�� :      ����  ");
		   EA_vDisplay(3, "��  �� : %3.2fԪ/����", DevStat.price[0]/100.0);
		   CardInfo.price = DevStat.price[0];
		   break;
	   case 1:
		   EA_vDisplay(2, "����93#:       ��(L)");
		   EA_vDisplay(3, "��  �� : %3.2f", DevStat.price[1]/100.0);
		   CardInfo.price = DevStat.price[1];
		   break;
	   case 2:
		   EA_vDisplay(2, "����97#:       ��(L)");
		   EA_vDisplay(3, "��  �� : %3.2f", DevStat.price[2]/100.0);
		   CardInfo.price = DevStat.price[2];
		   break;
	   case 3:
		   EA_vDisplay(2, "��  �� :       ��(L)");
		   EA_vDisplay(3, "��  �� : %3.2f", DevStat.price[3]/100.0);
		   CardInfo.price = DevStat.price[3];
		   break;
	   default:
		   break;
	}
		   EA_vDisplay(4, "Ա���� :%02X%02X%02X", CardInfo.driver_number[0], CardInfo.driver_number[1]
					   , CardInfo.driver_number[2]);


	strcpy((void *)input, "");

	for ( ;; )
	{
		i = get_input_str(2, 9, 1, 6, (void *)input);
		if ( i == EM_SUCCESS )
			break;
		if ( i == EM_ABOLISH )
			return notok;
	}

	sscanf((void *)input, "%lf", &temp_cap);
	
	if( DevStat.fill_mode == 0 )
	{
		sprintf((void *)buf, "������:%4.2lf����", temp_cap);
	}
	else
	{                        
		sprintf((void *)buf, "������:%4.2lf��  ", temp_cap);
	}

//  CardInfo.capacity = (INT32U)(temp_cap * 100.0);
	CardInfo.capacity = (INT32U)((temp_cap + 0.0001) * 100.0);
	temp_fare = (temp_cap * (CardInfo.price/100.0) * 100 + 0.5) / 100.0;
	CardInfo.fare = temp_fare * 100;//��������

	lcddisp(2, 1, (void *)buf);


	if ( (CardInfo.capacity < 1) || (CardInfo.capacity > 100000 ) )
	{
		//���㴦��
		lcddisperr((char *)"�������������!");
		goto reboot;
	}
	CardInfo.card_purchase_type = TT_NORMAL_CONSUME;		 //��ͨ����������
	//�����豸�������	
	temp_int32u = ((INT32U)DevStat.dev_trade_no[0] << 16) | ((INT16U)DevStat.dev_trade_no[1] << 8) | ((INT16U)DevStat.dev_trade_no[2]); 	//��ʼ�������
	if ( temp_int32u >= 0xF423F )   //���Ѵ���999999
		temp_int32u = 0;
	else
		temp_int32u++;

	DevStat.dev_trade_no[0] = (INT8U)(temp_int32u / 0x10000); //+1��Ľ������
	DevStat.dev_trade_no[1] = (INT8U)(temp_int32u % 0x10000 / 0x100); //+1��Ľ������
	DevStat.dev_trade_no[2] = (INT8U)(temp_int32u % 0x100);

	memcpy((INT8U *)&CardInfo.purchase_serial_num[0], (INT8U *)&DevStat.dev_trade_no[0], 3);

	WriteParam();

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
				   
 ��      ��	��������
 ��      �ڣ�2014-09-02
 �޸���ʷ��
		����		�޸���		�޸�����
		2005.2.18		myron	                   in function 
******************************************************************************/
INT8U ICCardProcess(void)
{
	INT8U     i = 0;  
	INT8U     block_buffer1[16] = {0};			//M1��һ��block�����ݻ���
	char      tmp_buf[10];
	uchar     temp_uc[5];

	memset(tmp_buf, 0, 10);

	i = CardInit();

	if( i != ok )
	{
		EA_ucMIFHalt(hMifsHandle);  //�ҿ�
		return notok;
	}

	////////////////////////////////////////
	//����ʱ��
	////////////////////////////////////////
	i = SaveCardInTime(&CardInfo);
	if ( i != ok )
	{
		lcddisperr("ʱ�����");
		return notok;
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
	//Debugprintf((char *)block_buffer1);	

	if ( block_buffer1[10] != 0x01 )	  //�ж����ñ�־
	{
		//��δ���ô���
		lcddisperr((char *)"��Ƭδ����");
		Beeperr();
		return notok;
	}

	//�����Ա��
	memcpy(CardInfo.mac1, &block_buffer1[0], 4);	
//  Debugprintf(CardInfo.bus_number);						 		//���Ա��
	//���ƺ�
	memcpy(CardInfo.bus_number, &block_buffer1[4], 5);	
	CardInfo.bus_number[5] = '\0';

	//���濨����
//  CardInfo.card_type = ;													 //������
//  EA_vDisplay(1, (void *)"����������:%02X", CardInfo.card_type);
//  SleepMs(2000);

#ifdef _debug_m1card_
	EA_vDisplay(3, "���Ϳ�����:%02X", CardInfo.card_type);
	SleepMs(2000);
#endif

	if ( block_buffer1[11] != 0xAA )                 //ֻ������ͨ������debug  by frank
	{
		lcddisperr((void *)"���ǳ�����!");
		return notok;
	}

	memcpy(temp_uc, &CardInfo.bus_number[0], 5);

	i = BlackName_Find((char *)temp_uc);	//�������ж�
	if ( i != ok )	 						//���ǰ�����
	{
		lcddisperr("�ÿ����ڰ�������");
		Beeperr();
		return i;
	}


//    //////////////////////////////////////////////////////////////
//    //�鿴�˿������Ƿ���ˢ��
//    //////////////////////////////////////////////////////////////
//    i = Cal_Interval();
//    if( i != ok )
//    {
//        lcddisperr("�˳������Ѽӹ���!");
//        Beeperr();
////  	return i;
//    }

	EA_vCls();
	EA_vDisplay(1, "��  �� :³C-%s", CardInfo.bus_number);
	EA_ucMIFHalt(hMifsHandle);  //�ҿ�
	//SleepMs(1000);
	EA_vBeepMs(2000);

	return ok;

}

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
		lcddisperr("M1����ȡ������ʧ��");
		return notok;
	}

	CardInfo.csn[3] = (INT8U)(ulSnr / 0x1000000);
	CardInfo.csn[2] = (INT8U)((ulSnr % 0x1000000) / 0x10000);
	CardInfo.csn[1] = (INT8U)((ulSnr % 0x10000) / 0x100);
	CardInfo.csn[0] = (INT8U)(ulSnr % 0x100);

	kind_of_card = usICType;

#ifdef _debug_m1card_
	EA_vDisplay(3, "������Ϊ:%02X", kind_of_card);
	SleepMs(2000);
#endif

	if ( kind_of_card != M1_CARD )
	{
		lcddisperr("����M1��");   //frank -- �ڳ���ǰ������
		return notok;
	}
	//ȡ��Ӳ������
	return ok;
}


