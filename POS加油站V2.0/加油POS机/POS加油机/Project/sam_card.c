/******************************************************************************
Copyright 2004 �����ִ��߼�����չ���޹�˾
All rights riserved.

�ļ���	��sam_card.c
ģ�����ƣ�PSAM�����ƺ�ͨѶ����
���ܸ�Ҫ��

ȡ���汾��0.0.1
�޸���	������
������ڣ�2004.09.09
����˵����create

******************************************************************************/
#include "main.h"


//��PSAM���������õ���һЩcode����
INT8U const       ReadPOSNumber[5]=         {0x00,0xB0,0x96,0x00,0x06};     
INT8U const       GetChallenge[5]=          {0x00,0x84,0x00,0x00,0x04};		//ȡPOS��
INT8U const       SELECT_SAM_ADF1[7]=      {0x00,0xA4,0x00,0x00,0x02,0x10,0x01};		//ѡ1001Ŀ¼
INT8U const       SELECT_JDSAM_ADF1[7]=      {0x00,0xA4,0x00,0x0C,0x02,0x10,0x01};		//ѡ1001Ŀ¼

INT8U const       SELECT_SAM_ADF2[7]=      {0x00,0xA4,0x00,0x00,0x02,0x10,0x02};		//ѡ1002Ŀ¼
INT8U const       SELECT_SAM_ADF3[7]=      {0x00,0xA4,0x00,0x00,0x02,0x10,0x03};		//ѡ1003Ŀ¼
INT8U const       DES_INIT[5]=                     {0x80,0x1A,0x45,0x01,0x10};							//DES�����ʼ��
INT8U const      GetResponse[4]=                {0x00,0xC0,0x00,0x00};									//ȡ��Ӧ
INT8U const      DES_CRYPT[4]=                   {0x80,0xFA,0x05,0x00};									 //DES����
INT8U const      READ_SAM_KEY_NUM[5]=   {0x00,0xb0,0x97,0x00,0x19};						 //����Կ�汾��
INT8U const      GET_MAC1[5]=                     {0x80,0x70,0x00,0x00,0x24};						  //PSAM����MAC1
INT8U const      VERIFY_MAC2[5]=                {0x80,0x72,0x00,0x00,0x04};							//��֤MAC2
INT8U const      GET_M1_KEY[5]=                 {0x80,0xFC,0x01,0x01,0x11};							//����M1��������Կ
INT8U const      TAC_INIT[5]=                       {0x80,0x1A,0x44,0x01,0x10};							//M1�����������У�TAC������ʼ��
//INT8U const      TAC_INIT[5]=                       {0x80,0x1A,0x24,0x01,0x08};							//M1�����������У�TAC������ʼ��
INT8U const      TAC_JDINIT[5]=                       {0x80,0x1A,0x24,0x07,0x08};							//M1�����������У�TAC������ʼ��
INT8U const      CAL_TAC_M1[5]=                 {0x80,0xFA,0x01,0x00,0x18};							//M1�����׹����У�����TAC��
INT8U const      DESCOMMUNICATION1[5]=   {0x80,0xfa,0x05,0x00,0x20};						//CPU��д0017�ļ�
INT8U const      DESCOMMUNICATION[4]=   {0x80,0xfa,0x05,0x00};									//DES����
INT8U const		 ISAM_CAL_KEY_INIT[5]={0x80,0x1A,0x28,0x01,0x08};//isam��������Կ��ʼ��
INT8U const      ISAM_CAL_KEY[5]={0x80,0xFA,0x00,0x00,0x08};//ISAM�������ֵ��Կ



//����ͨ��
INT8U const       SELECT_SAM_3F01[7]=       {0x00,0xA4,0x00,0x00,0x02,0x3F,0x01};		//ѡ1001Ŀ¼
INT8U const       GET_M1_FUZA_KEY[5]=       {0x80,0xFC,0x0E,0x0D,0x19};							//����M1��������Կ
INT8U const       TAC_FUZA_INIT[5]=         {0x80,0x1A,0x44,0x01,0x10};							//M1�����������У�TAC������ʼ��
INT8U const       CAL_TAC_M1_FUZA[5]=       {0x80,0xFA,0x01,0x00,0x18};							//M1�����׹����У�����TAC��
//INT8U const   code    GET_MI_FUZA_KEY_YINZI[8]= {0xD1,0xCC,0xCC,0xA8,0x00,0x00,0x00,0x00};                          //M1�����׹����У�����TAC��
INT8U const       GET_MI_FUZA_KEY_YINZI[8]= {0xD7,0xCD,0xB2,0xA9,0x00,0x00,0x00,0x00};                          //M1�����׹����У�����TAC��
INT8U const       FUZHA_TAC_INIT[5]=        {0x80,0x1A,0x48,0x0A,0x10};							//M1�����������У�TAC������ʼ��
INT8U const       FUZHA_CAL_TAC_M1[5]=      {0x80,0xFA,0x05,0x00,30};							//M1�����׹����У�����TAC��





//extern volatile INT8U idata OutPortStat;					//273��չ�����״̬�Ĵ���
extern DEV_STAT     DevStat;					//�豸״̬
extern DevHandle hSam1Handle;      //SAM1��
extern DevHandle hSam2Handle ;     //SAM2��
extern CARD_INFO    CardInfo;	

/******************************************************************************
 �������ƣ�CmdPsam
 ����������ʹ��T=0Э����PSAM������ͨѶ������̨��д��������0x6C(���ͳ��Ȳ���)�Ĵ���
 ����������
 �������ƣ�	����/�����	����		����
 psam_handle		����	PSAM���ľ��
 snd_buf			����	INT8U *			���ͻ�����
 snd_length			����	INT8U			Ҫ��������ĳ���
 rcv_buf			���	INT8U *			PSAM���ͻص��ֽڻ�����
 rcv_length			���	INT8U *			PSAM���ͻص��ֽڳ���
				
 ��  ��  ֵ��ok ?  notok
				   
 ��      ��	������/������
 ��      �ڣ�2012-12-02
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
******************************************************************************/
INT8U CmdPsam(DevHandle Cardhandle, INT8U  * const snd_buf, uint const snd_length,
			   INT8U *rcv_buf, uint *rcv_length)
{
	uchar result;
//  (void)EA_ucSetStopFlag( EM_DISABLE_STOP );   //���������� ״̬
	result = EA_ucICExchangeAPDU(Cardhandle, snd_length, snd_buf, rcv_length, rcv_buf);
	if( result != EM_SUCCESS )
	{
//  	EA_vCls();
//  	EA_vDisplay(2, "�������ͣ�%x", result);
//  	EA_uiInkey(1);
		return notok;
	}
//  (void)EA_ucSetStopFlag( EM_ENABLE_STOP );   //���������� ״̬
	return ok;
}

/******************************************************************************
 �������ƣ�PsamRst
 ������������λPSAM��
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
INT8U PsamRst(void)
{
	INT8U rcv_buf[100];
	uint  rcv_length;
	INT8U ret;
	uint  len;
	uchar aucATR[100];
	uchar prot;

	(void)EA_ucICPowerDown(hSam1Handle);
	(void)EA_ucICPowerUp(hSam1Handle, &len, aucATR, &prot);
//  (void)EA_ucSetStopFlag(EM_DISABLE_STOP);   //������������ ״̬
	////////////////////////////////////////
	//PSAM��ȡPOS��
	////////////////////////////////////////
	memset(rcv_buf, 0, sizeof(rcv_buf));
	ret = CmdPsam(hSam1Handle, (INT8U *)ReadPOSNumber, 5, rcv_buf, &rcv_length);       //ȡPOS��
	if (ret != ok )
	{
		(void)EA_ucSetStopFlag(EM_ENABLE_STOP);   //���������� ״̬
		return notok;
	}
	memcpy(&DevStat.pos_number[0], rcv_buf, 6);     //��POS��
	WriteParam();
//  (void)EA_ucSetStopFlag(EM_ENABLE_STOP);   //���������� ״̬
	return ok;

}

/******************************************************************************
 �������ƣ�SamCheck
 �������������SAM��������ȡ16�ļ�
 ����������
 �������ƣ�	����/�����	����		����
 ��    ��:  ��
				
 ��  ��  ֵ��ok(0)-�ɹ�
				 notok(0xFF)-ʧ��
				   
 ��      ��	������/������
 ��      �ڣ�2012-11-28
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
******************************************************************************/
INT8U SamCheck(void)
{
	uchar result = 0;
	uchar prot = 0;
	uint  len = 0;
	uchar aucDataOut[100];
	uchar aucAtrData[50];
	ET_CPUCOM  apducomm;
	ET_CPURESP apduresp;

	result = EA_ucICInitModule ( hSam1Handle, EM_ic_VOL5, EM_ic_384MODE );  //EM_ic_384MODE
	if (result != EM_SUCCESS)
	{
//  		EA_vDisplay(2, 1, "��������Ϊ:%d", i);
//  		break;
		lcddisperr("  PSAM����ʼ��ʧ��  ");
		return notok;
	}
	// �����ϵ�
	//(void)EA_uiInkey(0);
	result = EA_ucICPowerUp(hSam1Handle, &len, aucAtrData, &prot);
	if( result !=  EM_SUCCESS )
	{
		  lcddisperr("   PSAM���ϵ�ʧ��   ");
//  	  (void)EA_vDisplay(2, "Э������:%02X", prot);
//  	  (void)EA_vDisplay(3, "Э������:%02X", result);
//  	  (void)EA_vDelay(5);
     	  goto releaseOperate;
	}
//  (void)EA_ucSetStopFlag(EM_DISABLE_STOP);   //������������ ״̬
	// ִ��ѡ���ļ�����
	apducomm.CLA=0x00;
	apducomm.INS=0xa4;
	apducomm.P1=0x00;
	apducomm.P2=0x00;
	apducomm.Lc=0x02;
	apducomm.Le=0x00;
	apducomm.LeFlag=0x01;
	result = EA_ucICInComing (hSam1Handle , &apducomm, (uchar*)"\x3f\x00",&apduresp);
	if  ((result != EM_SUCCESS) && (result != EM_ic_SWDIFF) && (result != EM_ic_FORRESP))
	{
		   // ��ʾ����, ����������
		  goto releaseOperate;
	}

	// ִ��ȡ���������
	apducomm.CLA=0x00;
	apducomm.INS=0x84;
	apducomm.P1=0x00;
	apducomm.P2=0x00;
	apducomm.Lc=0x00;
	apducomm.Le=0x08;
	apducomm.LeFlag=0x01;
	apduresp.LengthOut=0x00;
	apduresp.SW1=0x00;
	apduresp.SW2=0x00;
	memset(aucDataOut,0x00,100);
	result = EA_ucICOutGoing(hSam1Handle , &apducomm, &apduresp, aucDataOut);
	if  ((result != EM_SUCCESS) && (result != EM_ic_SWDIFF))
	{
		   lcddisperr("PSAM�����ݶ�дʧ��");
		   goto releaseOperate;
	}

//  (void)EA_ucSetStopFlag(EM_ENABLE_STOP);   //���������� ״̬
	result = EA_ucICPowerDown(hSam1Handle);
	return ok;

releaseOperate: 
	result = EA_ucICPowerDown(hSam1Handle);
	if  (result != EM_SUCCESS)
	{
		   // ��ʾ����, ����������
		   goto releaseOperate;
	}
//  (void)EA_ucSetStopFlag(EM_ENABLE_STOP);   //���������� ״̬
	 // �ÿ�����ʱ��ʹ�ã��ͷ������Ȩ
	result = EA_ucCloseDevice(&hSam1Handle);
	// ��ʾ����, ��������
	return notok;
}

