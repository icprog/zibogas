/*****************************************************************
ģ�����ƣ��˵�
���ܸ�Ҫ����������ΪE550�Ļ�������ƵĲ˵�����
��    �ߣ��������ķ�������޹�˾ ������
��    �ڣ�2013��06��23
�޸���ʷ��
����		�޸���		�޸�����
------		---------	-------------
*****************************************************************/

#include "main.h"

#define EN  1
#define CN  0


//ϵͳ��������
extern DEV_STAT     DevStat;					//�豸״̬
extern CARD_INFO    CardInfo;					//��Ƭ��Ϣ�����׼�¼�ɴ˱�������
extern const char   parmFileName[];
extern const char   blackFileName[];
extern const char   currecFileName[];
extern const char   conrecFileName[];
extern const char   hisrecFileName[];
extern const char   grayrecFileName[];

DevHandle hDprinterHandle;


// *****************************************************************
// ���ܣ�		CRCУ�������
// ��ڲ�����	data_to_cal_crc �����ַ���
//				plength �ַ�������
// ���ڲ�����	int crc 4�ֽ�CRCУ����
// ����:		
// ���ߣ�	    ������    
//  ����:      2013-6-23
// *****************************************************************
int Cal_CRC(uchar * data_to_cal_crc, int length)
{
	unsigned int crc;
	unsigned char i;

	crc = 0xFFFF;			//CRC��ʼֵ

	while ( length-- != 0 )
	{
		crc = crc ^ ((unsigned int)*data_to_cal_crc++ << 8);

		for ( i = 0; i < 8; ++i )
			if ( crc & 0x8000 )
				crc = (crc << 1) ^ 0x1021;
			else
				crc	= crc << 1;
	}

	return(crc & 0xFFFF);

}

/******************************************************************************
 �������ƣ�htoa
 ����������hexת��Ϊascii��
 ����������
 �������ƣ� ����/����� ����		����
				
 ��  ��  ֵ��ok(0)-�ɹ�
				notok(0xFF)-ʧ��
				   
 �� 	 �� ������
 �� 	 �ڣ�2005-04-07
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
******************************************************************************/
void htoa(void *des, INT8U *sour, int hex_len)
{
	int i = 0;
	char *dest = (char *)des;

	if ( hex_len <= 0 )
		return;

	for ( i=0; i<hex_len; i++ )
		sprintf((char *)dest + i * 2, "%02X", sour[i]);

	dest[hex_len * 2] = '\0';
}

void htoa2(void *des, INT8U *sour, int hex_len)
{
	int i = 0;
	char *dest = (char *)des;

	if ( hex_len <= 0 )
		return;

	for ( i=0; i<hex_len; i++ )
		sprintf((char *)dest + i * 2, "%02d", sour[i]);

	dest[hex_len * 2] = '\0';
}

/******************************************************************************
 �������ƣ�a_to_h
 ������������һ��0-9,a-f,A-F��ascii���룬ת��Ϊ16������
 �������ƣ� ����/����� ����        ����
 num        ����        INT8U           Ҫת��Ϊascii���������Ҫ��Χ0-9

 ��  ��  ֵ��ת�����16������

 ��      �� ������
 ��      �ڣ�2004-09-02
 �޸���ʷ��
		����        �޸���      �޸�����
		------      ---------   -------------
******************************************************************************/
INT8U a_to_h(INT8U num)
{
	if (num>='0' && num <= '9')
		return (num - 0x30);
	else if (num == 'A' || num== 'a')
		return 0x0A;
	else if (num == 'B' || num== 'b')
		return 0x0B;
	else if (num == 'C' || num== 'c') 
		return 0x0C;
	else if (num == 'D' || num== 'd')
		return 0x0D;
	else if (num == 'E' || num== 'e')
		return 0x0E;
	else
//      if (num == 0x0F)
		return 0x0F;
//	return (num - 0x30);
}

/*****************************************************************
����ԭ�ͣ�bcdhex
����������bcd��ת��Ϊhex��
����������
�������ƣ�	����/�����	����		����
-----------		-----------	------	   	-------
bcd				����			INT8U		�����bcd��

��  ��  ֵ��ת�����hex��
��      ��	������
��      �ڣ�2006-01-09
�޸���ʷ��
����		�޸���		�޸�����
------		---------	-------------
*****************************************************************/
INT8U bcdhex(INT8U bcd)
{
	return ((bcd >> 4) * 10 + (bcd & 0x0F));
}

/******************************************************************************
 �������ƣ�ascii
 ������������һ��0-9����ת��Ϊascii���룬�˺���Ҳ����չ����ͨѶ�У�ͨѶ���漰��һЩA-F�ķǷ����룬��Ϊ�Ϸ�
 �������ƣ�	����/�����	����		����
 num		����		INT8U			Ҫת��Ϊascii���������Ҫ��Χ0-9
 
 ��  ��  ֵ��ת�����ascii����
 
 ��      ��	������
 ��      �ڣ�2004-09-02
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
******************************************************************************/
INT8U ascii(INT8U num)
{
//     return (num + 0x30);
	if (num <= 9)
		return (num + 0x30);
	else if (num == 0x0A)
		return 'A';
	else if (num == 0x0B)
		return 'B';
	else if (num == 0x0C)
		return 'C';
	else if (num == 0x0D)
		return 'D';
	else if (num == 0x0E)
		return 'E';
	else
//      if (num == 0x0F)
		return 'F';
}

/******************************************************************************
 �������ƣ�print_record
 ������������ӡ��ֵ��¼
 �������ƣ�����/�����	����		����
 ����		
 
 ��  ��  ֵ��
 
 ��      ��	���ڸ���
 ��      �ڣ�2004-09-02
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
******************************************************************************/
void print_record (RECORD *prec)
{
	INT8U       RecBuff[70];
	INT8U       strbuff[50];
	double		fBuf = 0;
	uchar       ucRet;


	EA_vCls();
	EA_vDisp(1, 1, "   ��ӡ��ʷ���׼�   ");
	EA_vDisp(2, 1, "      ��ȴ�...     ");
	EA_uiInkeyMs(500);

	ucRet = EA_ucGetPrinterStatus(hDprinterHandle);      //��ӡǰ�����жϴ�ӡ����״̬
	if( ucRet == EM_prn_PAPERENDED )
	{
		EA_vCls();
		EA_vDisplay(1, "   ȱֽ,��ӡʧ��!  ");
		EA_vDisplay(2, "   �����ֽ�ź�    ");
		EA_vDisplay(3, "   ��ȷ�������    ");
		EA_uiInkey(0);
	}
	if( ucRet == EM_prn_OVERHEAT )
	{
		EA_vDisplay(2, "    ��о�¶ȹ���    ");
		EA_vDisplay(3, "    ���Ժ��ٴ�ӡ     ");
		EA_uiInkeyMs(500);
		return;
	}
	if( ucRet == EM_prn_LOWVOL )
	{
		EA_vDisplay(2, "    ��ص�������    ");
		EA_vDisplay(3, "   ������ٴ�ӡ   ");
		EA_uiInkeyMs(500);
		return;
	}

	memset(strbuff, 0 , sizeof(strbuff));
	sprintf((char*)strbuff, "   �Ͳ��й���������˾\n");
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	sprintf((char*)strbuff, "   ��ʷ���׼�¼\n");
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);


	sprintf((char*)strbuff, "----------------------\n");
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	//����
	htoa(RecBuff, &prec->bus_number[0], 3);
	//memcpy(RecBuff, &prec->bus_number[0], 5);
	//strcpy((void *)RecBuff, (void *)&prec->bus_number[0]);
	sprintf((char*)strbuff, "   ��    �ţ�³C-%s\n", RecBuff);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);

	//ȼ��Ʒ��
	if(prec->fuel_type == 0)
	{
	sprintf((char*)strbuff, "   ȼ    �ϣ���Ȼ��\n");
	}
	else if(prec->fuel_type == 1)
	{
	sprintf((char*)strbuff, "   ȼ    �ϣ�����93#\n");
	}
	else if(prec->fuel_type == 2)
	{
	sprintf((char*)strbuff, "   ȼ    �ϣ�����97#\n");
	}
	else if(prec->fuel_type == 3)
	{
	sprintf((char*)strbuff, "   ȼ    �ϣ�����\n");
	}
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);


	//��������
	if( prec->fuel_type == 0 )
	{
	fBuf= (double)(prec->capacity)/100;
	sprintf((char*)strbuff, "   ���������%6.2lf����\n", fBuf);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	}
	else
	{
	fBuf= (double)(prec->capacity)/100;
	sprintf((char*)strbuff, "   ���������%6.2lf��\n", fBuf);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	}

	//�������
	fBuf= (double)(prec->fare)/100;
	sprintf((char*)strbuff, "   ���ѽ�%6.2lfԪ\n", fBuf);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);

	//��Ʒ����
	fBuf = (double)(prec->price)/100.0;
	sprintf((char*)strbuff, "   ��Ʒ���ۣ�%6.2lfԪ\n", fBuf);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);

	//Ա��������
	memcpy(RecBuff, &prec->driver_no[0], 4);
	sprintf((char*)strbuff, "   Ա �� �ţ�%02X%02X%02X%02X\n", RecBuff[0], RecBuff[1], RecBuff[2], RecBuff[3]);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);

	//�������
	sprintf((char*)strbuff, "   ������ţ�%08d\n", prec->pos_purchase_serial_num[0]*65535+prec->pos_purchase_serial_num[1]*256+prec->pos_purchase_serial_num[2]);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	sprintf((char*)strbuff, "----------------------\n");
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);

	//�����豸���
	htoa(RecBuff, &prec->equ_id[0], 3);
	sprintf((char*)strbuff, "   �豸��ţ�%s\n", RecBuff);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	htoa(RecBuff, &prec->acnt_id[0], 2);
	sprintf((char*)strbuff, "   վ���ţ�%s\n", RecBuff);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);

	//����Ա��
	htoa(RecBuff, &prec->oper_id[0], 1);
	sprintf((char*)strbuff, "   ����Ա�ţ�%s\n", RecBuff);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);

	//��������
	memcpy(RecBuff, (INT8U*)&prec->card_in_time, 7);
	sprintf((char*)strbuff, "   ����ʱ�䣺\r\n");
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	sprintf((char*)strbuff, "   %02X%02X-%02X-%02X %02X:%02X:%02X\r\n", 
	RecBuff[0],RecBuff[1],RecBuff[2],RecBuff[3],RecBuff[4],RecBuff[5],RecBuff[6]);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);

	sprintf((char*)strbuff, "----------------------\n");
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	// ��ӡ�����û�ҳ����
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)"   ��ע");
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)"\n\n\n\n");

	ucRet = EA_ucLinePrinter(hDprinterHandle, 0, "\f");
	if ( ucRet == EM_SUCCESS )
	{
		EA_vDisplay(2, "    ���δ�ӡ����!   ");
		EA_uiInkeyMs(500);
	}

}


///******************************************************************************
// �������ƣ�print_consume_record
// ������������ӡ��ֵ��¼
// �������ƣ�����/�����  ����        ����
// ����
//
// ��  ��  ֵ��
//
// ��      �� ���ڸ���
// ��      �ڣ�2004-09-02
// �޸���ʷ��
//        ����        �޸���      �޸�����
//        ------      ---------   -------------
//******************************************************************************/
//void print_consume_record (void)
//{
//
//    int         count;
////	CardInfoOne 	*strCardInfo;
//    INT8U       RecBuff[70];
//    INT8U       strbuff[50];
//    INT8U  rec[34];
//    CONSUMERECORD* prec=(CONSUMERECORD*)rec ;
//    double   fBuf = 0;
//    uchar  ucOpenID = 0;
//    uchar  ucRet;
//    uint   uiReadCnt;
//    uint   uiRetCnd;
//
//    EA_vCls();
//    //����¼��Ч����
//    if ( EA_ucPFOpen( (uchar *)conrecFileName, &ucOpenID) != EM_ffs_SUCCESS )
//    {
//        lcddisperr("�����Ѽ�¼ʧ��");
//        EA_ucPFClose(ucOpenID);
//        return;
//    }
//    // ��ȡʣ���д��¼��
//    (void)EA_ucPFInfo(ucOpenID, EM_ffs_VALIDREC, &uiReadCnt);
//    if( uiReadCnt == 0)
//    {
//        lcddisperr("û�����Ѽ�¼��Ϣ");
//        EA_ucPFClose(ucOpenID);
//        return;//��ʾû�м�¼
//    }
//    count = uiReadCnt;//���һ��
////  prec = (RECORD *)DB_jump_to_record(DB_REC,count,&flag);//count -1 :�����¼���
//    ucRet = EA_ucPFReadRec(ucOpenID, count, 1, &uiRetCnd, rec);
//    if(ucRet != EM_SUCCESS)
//    {
//        EA_ucPFClose(ucOpenID);
//        return;
//    }
//    EA_ucPFClose(ucOpenID);
////  DispStr_CE(0, 0, "��ӡ��¼ ", DISP_CENTER);//(int qx, int qy, char * ceStr, int xyFlag)
////  DispStr_CE(0, 4, "��ȴ�... ...", DISP_CENTER);//(int qx, int qy, char * ceStr, int xyFlag)
//    EA_vDisp(1, 1, "��ӡ���Ѽ�¼");
//    EA_vDisp(2, 1, "��ȴ�...");
//    EA_uiInkeyMs(500);
//    //mtclprn_init_page(pagebuff, PageSize);
//    // �ɽ��д�ӡ��һЩ��������
//
////  ucRet = EA_ucSetPrinterFormat(hDprinterHandle, EM_prn_XSPACE, 0);
////  if ( ucRet != EM_SUCCESS )
////  {
////  	//  	(void)EA_ucCloseDevice(&hDprinterHandle);
////  	lcddisperr("��ӡ������ʧ��!");
////  	return;
////  }
////  ucRet =  EA_ucSetPrinterFormat(hDprinterHandle, EM_prn_YSPACE, 6);
////  if ( ucRet != EM_SUCCESS )
////  {
////  	//  	(void)EA_ucCloseDevice(&hDprinterHandle);
////  	lcddisperr("��ӡ������ʧ��!");
////  	return;
////  }
////
////  //  ucRet = EA_ucSetPrinterFormat(hDprinterHandle, EM_prn_PAGESPACE, 578);
////  ucRet = EA_ucSetPrinterFormat(hDprinterHandle, EM_prn_PAGESPACE , 600);
////  if ( ucRet != EM_SUCCESS )
////  {
////  	//  	(void)EA_ucCloseDevice(&hDprinterHandle);
////  	lcddisperr("��ӡ������ʧ��!");
////  	return;
////  }
////
////  ucRet = EA_ucSetPrinterFormat(hDprinterHandle, EM_prn_ASCII_DOTTYPE, EM_prn_ASCIIDOT24X12);
////  if ( ucRet != EM_SUCCESS )
////  {
////  	//  	(void)EA_ucCloseDevice(&hDprinterHandle);
////  	lcddisperr("��ӡ������ʧ��!");
////  	return;
////  }
////  ucRet = EA_ucSetPrinterFormat(hDprinterHandle, EM_prn_ASCII_PRNTYPE, EM_prn_ASCIIPRN1X1);
////  if ( ucRet != EM_SUCCESS )
////  {
////  	//  	(void)EA_ucCloseDevice(&hDprinterHandle);
////  	lcddisperr("��ӡ������ʧ��!");
////  	return;
////  }
////  ucRet = EA_ucSetPrinterFormat(hDprinterHandle, EM_prn_HZ_DOTTYPE, EM_prn_HZDOT24X24);
////  if ( ucRet != EM_SUCCESS )
////  {
////  	//  	(void)EA_ucCloseDevice(&hDprinterHandle);
////  	lcddisperr("��ӡ������ʧ��!");
////  	return;
////  }
////
////  ucRet = EA_ucSetPrinterFormat(hDprinterHandle, EM_prn_HZ_PRNTYPE, EM_prn_HZPRN1X1);
////  if ( ucRet != EM_SUCCESS )
////  {
////  	//  	(void)EA_ucCloseDevice(&hDprinterHandle);
////  	lcddisperr("��ӡ������ʧ��!");
////  	return;
////  }
////  ucRet = EA_ucSetPrinterFormat(hDprinterHandle, EM_prn_PRNGRAY, 4);
////  if ( ucRet != EM_SUCCESS )
////  {
////  	//  	(void)EA_ucCloseDevice(&hDprinterHandle);
////  	lcddisperr("��ӡ������ʧ��!");
////  	return;
////  }
//
//    ucRet = EA_ucGetPrinterStatus(hDprinterHandle);      //��ӡǰ�����жϴ�ӡ����״̬
//    if( ucRet == EM_prn_PAPERENDED )
//    {
//        lcddisperr("   ȱֽ,��ӡʧ��!  ");
//        return;
//    }
//    if( ucRet == EM_prn_OVERHEAT )
//    {
//        EA_vDisplay(2, "    ��о�¶ȹ���    ");
//        EA_vDisplay(3, "    ���Ժ��ٴ�ӡ     ");
//        EA_uiInkeyMs(500);
//        return;
//    }
//    if( ucRet == EM_prn_LOWVOL )
//    {
//        EA_vDisplay(2, "    ��ص�������    ");
//        EA_vDisplay(3, "   ������ٴ�ӡ   ");
//        EA_uiInkeyMs(500);
//        return;
//    }
//    (void)EA_ucPrinterAutoFeeding(hDprinterHandle);
//    memset(RecBuff, 0, sizeof(RecBuff));
//
//    sprintf((char*)strbuff, "��ӭʹ���Ͳ�����IC��\n");
//    (void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
//    sprintf((char*)strbuff, "www.zbbus.com\n");
//    (void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
////��˾��ַ
//    sprintf((char*)strbuff, "-----------\n");
//    (void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
////����
//    htoa(RecBuff,&prec->card_serial_number[4], 4);//���д���2�ֽ�+��ҵ����2�ֽ�+��������ˮ��4�ֽ�
//    sprintf((char*)strbuff, "��    �ţ�%s\n", RecBuff);
//    (void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
//
////��Ƭ����
//    GetCardType(prec->card_type, RecBuff);
//    sprintf((char*)strbuff, "��Ƭ����: %s\n", RecBuff);
//    (void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
//
////��Ч��
//
////Ѻ����߹�����
//
////��������
////	htoa(RecBuff,&prec->card_purchase_type, 1);//���д���2�ֽ�+��ҵ����2�ֽ�+��������ˮ��4�ֽ�
////	sprintf((char*)strbuff, "\x1Bw0\x1BW1�������ͣ�%s\r\r", RecBuff);
////	mtclprn_charline_print(strbuff);
//
//    switch ( prec->card_purchase_type)
//    {
//        case TT_NORMAL_CONSUME_MONEY:                   //������Ƭ������Ϣ
//            strcpy((char*)RecBuff,"    ����");
//            break;
//        case TT_CARD_SALE:
//            strcpy((char*)RecBuff,"    �ۿ�");
//            break;
//        case TT_CREDIT_MONEY:
//            strcpy((char*)RecBuff,"    ��ֵ");
//            break;
//        case TT_NORMAL_CONSUME:
//            strcpy((char*)RecBuff,"    ����");
//            break;
//        default:
//            strcpy((char*)RecBuff,"    ����");
//            break;
//
//    }
//    sprintf((char*)strbuff, "�������ͣ�%s\n", RecBuff);
//    (void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
//
////Ӳ������
////	mtclprn_charline_print("\x1Bw0\x1BW1 Ӳ�����ţ�\r");
////	mtclprn_charline_print("\x1Bw1\x1BW0  ABCDEFGH\r\r");
//
////���׽��
//    fBuf= (double)(prec->fare[0]*65536 + prec->fare[1]*256 + prec->fare[2])/100;
//    sprintf((char*)strbuff, "���׽�%6.2fԪ\n", fBuf);
//    (void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
//
////��Ƭ���
//    fBuf= (double)(prec->balance[0]*65536 + prec->balance[1]*256 + prec->balance[2])/100;
//    sprintf((char*)strbuff, "��Ƭ��%6.2fԪ\n", fBuf);
//    (void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
//
////��Ƭ�������
////	htoa(RecBuff, &prec->purchase_serial_num[0], 2);
////	sprintf((char*)strbuff, "\x1Bw0\x1BW1 ������ţ�%d\r\r", RecBuff);
//    sprintf((char*)strbuff, "�������:%8d\n", prec->purchase_serial_num[0]*256 + prec->purchase_serial_num[1]);
//    (void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
//
//
////������֤�룭��TAC��
////	mtclprn_charline_print("\x1Bw0\x1BW1 ������֤�룺\r");
//    htoa(RecBuff, &prec->tac[0], 4);
//    sprintf((char*)strbuff, "���״��룺%s\n", RecBuff);
//    (void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
//
//    sprintf((char*)strbuff, "-----------\n");
//    (void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
//
////�����豸���
//    htoa(RecBuff, &DevStat.acnt_id[0], 2);
//    htoa(RecBuff+4, &DevStat.equ_id[0], 3);
//    sprintf((char*)strbuff, "\x1Bw1\x1BW1�����豸��%s\r\r", RecBuff);
//    (void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
//
////����Ա��
////	mtclprn_charline_print("\x1Bw0\x1BW1 ����Ա�ţ�\r");
//    htoa(RecBuff, &DevStat.cur_driver_number[1], 3);           //by liujihua
//    sprintf((char*)strbuff, "����Ա�ţ�%s\n", RecBuff);
//    (void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
//
////��������
////	mtclprn_charline_print("\x1Bw0\x1BW1 �������ڣ�\r");
//    htoa(RecBuff, (INT8U*)&prec->card_in_time, 7);
//    sprintf((char*)strbuff, "����ʱ�䣺%s\n", RecBuff);
//    (void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
///*
////�˿�ǩ��
////	mtclprn_charline_print("\x1Bw0\x1BW1 �������ڣ�\r");
////	htoa(RecBuff, (INT8U*)&prec->card_in_time, 7);
//    strcpy(strbuff, "\x1Bw1\x1BW1�˿�ǩ����______________\r\r");
//    mtclprn_charline_print(strbuff);
//
//
////��ʾ
////	mtclprn_charline_print("\x1Bw0\x1BW1 �������ڣ�\r");
//    strcpy(strbuff, "\r\x1Bw1\x1BW1  �� �����Ʊ��ܴ�Ʊ���� \r\r");
//    mtclprn_charline_print(strbuff);
//*/
//
////  mtclprn_charline_print("\x1Bw0\x1BW0\r\r\r\r\r\r");
////
////  mtcl_prn_power_on();
////
////  rcode = mtclprn_print_page(pagebuff, total_dotlines);
////
////  mtcl_prn_power_off();
//
////	delay_n_ms(2000);
////	delay_n_ms(200);
//
////	init_console(CONSOLE_CHINESE);
////	KEY_init(KEY_BEEP_KEY|KEY_AUTO_EL_ON|KEY_FIFO_MODE);//�������������ͱ���
////	(void)sm_setting(MSK_KEY_UP|MSK_KEY_DOWN); //��ֹ�����»�̧����ʱ����Ӧ
////	EA_ucClrKeyBuf();
////	printf_debug("2");
////	delay_and_wait_key(30,0,0);
//    // ��ӡ�����û�ҳ����
//    ucRet = EA_ucLinePrinter(hDprinterHandle, 0, "\f");
//    if ( ucRet == EM_SUCCESS )
//    {
//        EA_vDisplay(2, "���δ�ӡ����!");
//        EA_uiInkeyMs(500);
//    }
//
//}


/******************************************************************************
 �������ƣ�print_sale_record
 ������������ӡ�ۿ���¼
 �������ƣ�����/�����	����		����
 ����		
 
 ��  ��  ֵ��
 
 ��      ��	���ڸ���
 ��      �ڣ�2004-09-02
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
******************************************************************************/
void print_sale_record(void)
{
	int 		count;
    INT8U		RecBuff[70];
	INT8U		strbuff[50];
	INT8U       rec[57];

	RECORD * prec = (RECORD*)rec;
	double	fBuf = 0.00;
	uchar   ucOpenID = 0;
	uchar   ucRet;
	uint    uiReadCnt;
	uint    uiRetCnd;

	memset(rec, 0, sizeof(rec));

	EA_vCls();
	//����¼��Ч����
	if ( EA_ucPFOpen( (uchar *)currecFileName, &ucOpenID) != EM_ffs_SUCCESS )
	{
		lcddisperr("�򿪼�¼�ļ�ʧ��");
		EA_ucPFClose(ucOpenID);
		return;
	}
	// ��ȡʣ���д��¼��
	(void)EA_ucPFInfo(ucOpenID, EM_ffs_VALIDREC, &uiReadCnt);
	if( uiReadCnt == 0)
	{
		lcddisperr("    û�м�¼��Ϣ    ");
		EA_ucPFClose(ucOpenID);
		return;//��ʾû�м�¼
	}
	count = uiReadCnt;//���һ��		
	ucRet = EA_ucPFReadRec(ucOpenID, count, 1, &uiRetCnd, rec);
	if(ucRet != EM_SUCCESS)
	{
		EA_ucPFClose(ucOpenID);
		return;
	}
	(void)EA_ucPFClose(ucOpenID);
	EA_vDisp(1, 1, "  ��ӡ���μ��ͼ�¼  ");
	EA_vDisp(2, 1, "      ��ȴ�...     ");
	EA_uiInkeyMs(500);

	ucRet = EA_ucGetPrinterStatus(hDprinterHandle);      //��ӡǰ�����жϴ�ӡ����״̬
	if( ucRet == EM_prn_PAPERENDED )
	{
		EA_vCls();
		EA_vDisplay(1, "   ȱֽ,��ӡʧ��!  ");
		EA_vDisplay(2, "   �����ֽ�ź�    ");
		EA_vDisplay(3, "   ��ȷ�������    ");
		EA_uiInkey(0);
	}
	if( ucRet == EM_prn_OVERHEAT )
	{
		EA_vDisplay(2, "    ��о�¶ȹ���    ");
		EA_vDisplay(3, "    ���Ժ��ٴ�ӡ     ");
		EA_uiInkeyMs(500);
		return;
	}
	if( ucRet == EM_prn_LOWVOL )
	{
		EA_vDisplay(2, "    ��ص�������    ");
		EA_vDisplay(3, "   ������ٴ�ӡ   ");
		EA_uiInkeyMs(500);
		return;
	}

	memset(strbuff, 0 , sizeof(strbuff));
	memset(RecBuff, 0 , sizeof(RecBuff));

	(void)EA_ucPrinterAutoFeeding(hDprinterHandle);
///��ӡͷ
	sprintf((char*)strbuff, "   �Ͳ��й���������˾\n");
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	sprintf((char*)strbuff, "   �� �� ��\n");
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	sprintf((char*)strbuff, "----------------------\n");
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
//����
	//htoa(RecBuff, &prec->bus_number[0], 3);
	memcpy(RecBuff, &prec->bus_number[0], 5);
	//strcpy((void *)RecBuff, (void *)&prec->bus_number[0]);
	sprintf((char*)strbuff, "   ��    �ţ�³C-%s\n", RecBuff);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
//ȼ��Ʒ��
//	htoa(RecBuff, &prec->bus_number[0], 5);
//  memcpy(RecBuff, &prec->fuel_type, 1);
	if(prec->fuel_type == 0)
	{
		sprintf((char*)strbuff, "   ȼ    �ϣ���Ȼ��\n");
	}
	else if(prec->fuel_type == 1)
	{
		sprintf((char*)strbuff, "   ȼ    �ϣ�����93#\n");
	}
	else if(prec->fuel_type == 2)
	{
		sprintf((char*)strbuff, "   ȼ    �ϣ�����97#\n");
	}
	else if(prec->fuel_type == 3)
	{
		sprintf((char*)strbuff, "   ȼ    �ϣ�����\n");
	}
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);

//��������
	if( prec->fuel_type == 0 )
	{
		fBuf= (double)(prec->capacity/100.00);
		sprintf((char*)strbuff, "   ���������%6.2lf����\n", fBuf);
		(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	}
	else
	{
		fBuf= (double)(prec->capacity/100.00);
		sprintf((char*)strbuff, "   ���������%6.2lf��\n", fBuf);
		(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	}

//�������
	fBuf= (double)(prec->fare)/100;
	sprintf((char*)strbuff, "   ���ѽ�%6.2lfԪ\n", fBuf);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);

//��Ʒ����
	fBuf= (double)(prec->price)/100;
	sprintf((char*)strbuff, "   ��Ʒ���ۣ�%6.2lfԪ\n", fBuf);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	

//Ա��������
	memcpy(RecBuff, &prec->driver_no[0], 3);
	sprintf((char*)strbuff, "   Ա �� �ţ�%02X%02X%02X\n", RecBuff[0], RecBuff[1], RecBuff[2]);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	
//�������
	sprintf((char*)strbuff, "   ������ţ�%08d\n", prec->pos_purchase_serial_num[0]*65535+prec->pos_purchase_serial_num[1]*256+prec->pos_purchase_serial_num[2]);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);

	sprintf((char*)strbuff, "----------------------\n");
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);

//�����豸���
	htoa(RecBuff, &prec->equ_id[0], 3);
	sprintf((char*)strbuff, "   �豸��ţ�%s\n", RecBuff);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	htoa(RecBuff, &prec->acnt_id[0], 2);
	sprintf((char*)strbuff, "   վ���ţ�%s\n", RecBuff);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);

//����Ա��

	memcpy(RecBuff, &prec->oper_id[0], 3);
	sprintf((char*)strbuff, "   ����Ա�ţ�%02X%02X%02X\n", RecBuff[0], RecBuff[1], RecBuff[2]);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);

//��������
//  htoa(RecBuff, (INT8U*)&prec->card_in_time, 7);
	memcpy(RecBuff, (INT8U*)&prec->card_in_time, 7);
	sprintf((char*)strbuff, "   ����ʱ�䣺\r\n");
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	sprintf((char*)strbuff, "   %02X%02X-%02X-%02X %02X:%02X:%02X\r\n", 
			RecBuff[0],RecBuff[1],RecBuff[2],RecBuff[3],RecBuff[4],RecBuff[5],RecBuff[6]);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);

	sprintf((char*)strbuff, "----------------------\n");
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	// ��ӡ�����û�ҳ����
	sprintf((char*)strbuff, "\n\n\n");
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
//////////////////////////////////////////////////////////////////////////////////////////////////////////
////�̻�
//////////////////////////////////////////////////////////////////////////////////////////////////////////
	SleepMs(2500);

	memset(strbuff, 0 , sizeof(strbuff));
	memset(RecBuff, 0 , sizeof(RecBuff));
	sprintf((char*)strbuff, "   �Ͳ��й���������˾\n");
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	sprintf((char*)strbuff, "   �� �� ��\n");
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);


	sprintf((char*)strbuff, "----------------------\n");
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
//����
//  htoa(RecBuff, &prec->bus_number[0], 3);
	memcpy(RecBuff, &prec->bus_number[0], 5);
	//strcpy((void *)RecBuff, (void *)&prec->bus_number[0]);
	sprintf((char*)strbuff, "   ��    �ţ�³C-%s\n", RecBuff);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);

//ȼ��Ʒ��
	if(prec->fuel_type == 0)
	{
		sprintf((char*)strbuff, "   ȼ    �ϣ���Ȼ��\n");
	}
	else if(prec->fuel_type == 1)
	{
		sprintf((char*)strbuff, "   ȼ    �ϣ�����93#\n");
	}
	else if(prec->fuel_type == 2)
	{
		sprintf((char*)strbuff, "   ȼ    �ϣ�����97#\n");
	}
	else if(prec->fuel_type == 3)
	{
		sprintf((char*)strbuff, "   ȼ    �ϣ�����\n");
	}
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);


//��������
	if( prec->fuel_type == 0 )
	{
		fBuf= (double)(prec->capacity)/100;
		sprintf((char*)strbuff, "   ���������%6.2lf����\n", fBuf);
		(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	}
	else
	{
		fBuf= (double)(prec->capacity)/100;
		sprintf((char*)strbuff, "   ���������%6.2lf��\n", fBuf);
		(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	}

//�������
	fBuf= (double)(prec->fare)/100;
	sprintf((char*)strbuff, "   ���ѽ�%6.2lfԪ\n", fBuf);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);

//��Ʒ����
	fBuf = (double)(prec->price)/100.0;
	sprintf((char*)strbuff, "   ��Ʒ���ۣ�%6.2lfԪ\n", fBuf);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);

//Ա��������
	memcpy(RecBuff, &prec->driver_no[0], 3);
	sprintf((char*)strbuff, "   Ա �� �ţ�%02X%02X%02X\n", RecBuff[0], RecBuff[1], RecBuff[2]);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);

//�������
	sprintf((char*)strbuff, "   ������ţ�%08d\n", prec->pos_purchase_serial_num[0]*65535+prec->pos_purchase_serial_num[1]*256+prec->pos_purchase_serial_num[2]);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	sprintf((char*)strbuff, "----------------------\n");
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);

//�����豸���
	htoa(RecBuff, &prec->equ_id[0], 3);
	sprintf((char*)strbuff, "   �豸��ţ�%s\n", RecBuff);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	htoa(RecBuff, &prec->acnt_id[0], 2);
	sprintf((char*)strbuff, "   վ���ţ�%s\n", RecBuff);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);

//����Ա��
	memcpy(RecBuff, &prec->oper_id[0], 3);
	sprintf((char*)strbuff, "   ����Ա�ţ�%02X%02X%02X\n", RecBuff[0], RecBuff[1], RecBuff[2]);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);

//��������
	memcpy(RecBuff, (INT8U*)&prec->card_in_time, 7);
	sprintf((char*)strbuff, "   ����ʱ�䣺\r\n");
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	sprintf((char*)strbuff, "   %02X%02X-%02X-%02X %02X:%02X:%02X\r\n", 
			RecBuff[0],RecBuff[1],RecBuff[2],RecBuff[3],RecBuff[4], RecBuff[5],RecBuff[6]);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);

	sprintf((char*)strbuff, "----------------------\n");
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	// ��ӡ�����û�ҳ����
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)"   ��ע");
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)"\n\n\n\n");
	ucRet = EA_ucLinePrinter(hDprinterHandle, 0, "\f");

	if ( ucRet == EM_SUCCESS )
	{
		EA_vDisplay(2, "    ���δ�ӡ����!   ");
		EA_uiInkeyMs(500);
	}
	
}


// *****************************************************************
// ���ܣ�		GPRS_Input_IP
// ˵��:		�ֹ�����IP��ַ
// ��ڲ�����	
// ���ڲ�����	Null
// ���ߣ�
// ����ֵ��	-1 �ػ�							
// *****************************************************************
INT8U GPRS_Input_IP(void)
{

	INT8U IP_addr[20];
	INT8U input[20];
	int   IP[4],i, j;

	EA_vCls();
	EA_ucClrKeyBuf();

	//���������޸Ĺ���
	switch (PasswordCheck())
	{
		case notok:
			printf_debug((void *)"�������");
			return 1;
		case 2:
			return 1;
		case ok:
			EA_ucClrKeyBuf();
			break;
		default:
			return 1;
	}

	j = 0;

	EA_vCls();
	EA_vDisp(1, 1, "ԭ��������ַΪ:");
	EA_vDisp(3, 1, "�������·�������ַ:");
	sprintf((char *)IP_addr, "%d.%d.%d.%d", DevStat.ServerIp[0], DevStat.ServerIp[1], DevStat.ServerIp[2], DevStat.ServerIp[3]);
	EA_vDisp(2, 1, (char *)IP_addr);

	EA_vDisplay(4, "   .   .   .      ");
//  IP_addr_1[15] = '\0';
//  EA_vDisp(4, 1, (void *)IP_addr_1);
	for ( ;; )
	{
		i = EA_ucGetInputStr(4, 1, 20, EM_BIG_FONT | EM_MODE_CHAR | EM_ALIGN_LEFT | EM_SHOW_ORIGINALLY
							 , 7, 15, 0, (char *)input);
		if ( i == EM_SUCCESS )
			break;
		if ( i == EM_ABOLISH )
			return notok;
	}

	//if (ReadNumeric_(IP_addr_1, 15, 0, 2) < 0) return;
//  sprintf((uint *)input, "%03d.%03d.%03d.%03d", (void *)&IP_addr_1);
	if(strlen((void *)input) < 7)
	{
		lcddisperr("IP��ַ�����ʽ����");
		return notok;
	}
	EA_vDisp(4, 1, (void *)input);
	sscanf((void *)input, "%d.%d.%d.%d", &IP[0], &IP[1], &IP[2], &IP[3]);
	for(i = 0; i < 4; i++)
	{
		if( (IP[i] > 255) || (IP[i] < 0) )
		{
			lcddisperr("IP��ַ�����ʽ��");
			return notok;
		}
	}

	for(i=0;i<4;i++)
	{
		DevStat.ServerIp[i] = IP[i];
	}
	DevStat.ServerPort[0]=0x55;
	DevStat.ServerPort[1]=0x55;
//	memset(IP_addr, '\0', 16);
//	sprintf(IP_addr, "%d.%d.%d.%d", iTmp[0], iTmp[1], iTmp[2], iTmp[3]);
 /*
PORT_AGAIN:
	DispStr_CE(0, 0, "������˿ں�:", DISP_POSITION | DISP_CLRSCR);
	memcpy(IP_addr_1, "     ", 5);
	IP_addr_1[5] = '\0';
	if (ReadNumeric_(IP_addr_1, 5, 24, 2) < 0) return;
	iport = atoi(IP_addr_1);
	if( iport > 65535 )
	{
//		DispStr_CE(0, 6, "�˿ں���������", DISP_POSITION);
//		PressAnyKey();
		printf_debug("");
		goto PORT_AGAIN;
	}
//	memset(Comm_port, '\0', 16);
//	sprintf(Comm_port, "%d", iport);
	DevStat.ServerPort[0] = (INT8U)(iport>>8);
	DevStat.ServerPort[1] = (INT8U)iport;
*/
	WriteParam();
	return ok;
}

/******************************************************************************
 �������ƣ�print_work_note
 ������������ӡ�ս���Ʊ��
 �������ƣ�����/�����	����		����
 ����		
 
 ��  ��  ֵ��
 
 ��      ��	���ڸ���
 ��      �ڣ�2004-09-02
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
******************************************************************************/
void print_work_note(void)
{
	INT8U		RecBuff[70];
	INT8U		strbuff[50];
	INT8U		time[7+2];
	uchar     	ucRet;

	EA_vCls();
	EA_vDisplay(1,"    �ս��ʵ���ӡ    ");
	EA_vDisplay(2,"      ��ȴ�...     ");
	SleepMs(1000);

	memset(RecBuff, 0, sizeof(RecBuff));

	ucRet = EA_ucGetPrinterStatus(hDprinterHandle);      //��ӡǰ�����жϴ�ӡ����״̬
	if( ucRet == EM_prn_PAPERENDED )
	{
		 EA_vCls();
		EA_vDisplay(1, "   ȱֽ,��ӡʧ��!  ");
		EA_vDisplay(2, "   �����ֽ�ź�    ");
		EA_vDisplay(3, "   ��ȷ�������    ");
		EA_uiInkey(0);
	}
	if( ucRet == EM_prn_OVERHEAT )
	{
		 EA_vDisplay(2, "    ��о�¶ȹ���    ");
		 EA_vDisplay(3, "    ���Ժ��ٴ�ӡ     ");
		 EA_uiInkeyMs(500);
		 return;
	}
	if( ucRet == EM_prn_LOWVOL )
	{
	 EA_vDisplay(2, "    ��ص�������    ");
	 EA_vDisplay(3, "   ������ٴ�ӡ   ");
	 EA_uiInkeyMs(500);
	 return;
	}

	(void)EA_ucPrinterAutoFeeding(hDprinterHandle);

	sprintf((char*)strbuff, "  �Ͳ��й���������˾\n");
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	sprintf((char*)strbuff, "  �ս��ʵ�\n");
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);


	sprintf((char*)strbuff, "-----------------------------------------\n");
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	
//���Ѵ���
	sprintf((char*)strbuff, "  ��Ȼ���������� ��%6d��\r\n", DevStat.binhand.consume_cnt[0]);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	sprintf((char*)strbuff, "  ����93#���ʹ�����%6d��\r\n", DevStat.binhand.consume_cnt[1]);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	sprintf((char*)strbuff, "  ����97#���ʹ�����%6d��\r\n", DevStat.binhand.consume_cnt[2]);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	sprintf((char*)strbuff, "  ���ͼ��ʹ���   ��%6d��\r\n", DevStat.binhand.consume_cnt[3]);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	sprintf((char*)strbuff, "-----------------------------------------\n");
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
//�������
	sprintf((char*)strbuff, "  ��Ȼ����� ��%6.2lf����\r\n", (double)((DevStat.binhand.consume_cap[0])/100.0));
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	sprintf((char*)strbuff, "  ����93#�����%6.2lf��\r\n", (double)((DevStat.binhand.consume_cap[1])/100.0));
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	sprintf((char*)strbuff, "  ����97#�����%6.2lf��\r\n", (double)((DevStat.binhand.consume_cap[2])/100.0));
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	sprintf((char*)strbuff, "  �� �� �� ����%6.2lf��\r\n", (double)((DevStat.binhand.consume_cap[3])/100.0));
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	sprintf((char*)strbuff, "-----------------------------------------\n");
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	//���ѽ��
	sprintf((char*)strbuff, "  ��Ȼ����� ��%6.2lfԪ\r\n", (double)((DevStat.binhand.consume_amt[0])/100.0));
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	sprintf((char*)strbuff, "  ����93#��%6.2lfԪ\r\n", (double)((DevStat.binhand.consume_amt[1])/100.0));
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	sprintf((char*)strbuff, "  ����97#��%6.2lfԪ\r\n", (double)((DevStat.binhand.consume_amt[2])/100.0));
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	sprintf((char*)strbuff, "  �� �� �� �%6.2lfԪ\r\n", (double)((DevStat.binhand.consume_amt[3])/100.0));
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	sprintf((char*)strbuff, "-----------------------------------------\n");
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);

	sprintf((char*)strbuff, "  ��  ��  �� ��%6.2lfԪ\r\n", (double)((DevStat.binhand.consume_amt[0]+DevStat.binhand.consume_amt[1]+
														   DevStat.binhand.consume_amt[2]+DevStat.binhand.consume_amt[3])/100.0));
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
//�ϰ�ʱ��	
	memcpy(time, (INT8U*)&DevStat.binhand.work_datetime, 7);
	sprintf((void *)strbuff, "�ϰ�ʱ�䣺\r\n");
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	sprintf((void *)strbuff, "%02X%02X-%02X-%02X %02X:%02X:%02X\r\n", time[0],time[1],time[2],time[3],time[4],time[5],time[6]);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);	
//�°�ʱ��
	memcpy(time, (INT8U*)&DevStat.binhand.down_datetime, 7);
	sprintf((void *)strbuff, "�°�ʱ�䣺\r\n");
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	sprintf((void *)strbuff, "%02X%02X-%02X-%02X %02X:%02X:%02X\r\n", time[0],time[1],time[2],time[3],time[4],time[5],time[6]);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);	


//���з���
	sprintf((char*)strbuff, "-----------------------------------------\n");
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char *)strbuff);
	
//�豸���
	htoa(RecBuff, &DevStat.equ_id[0], 3);
	sprintf((char*)strbuff, "  �� �� �� �ţ�%s\r\n", RecBuff);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char *)strbuff);
//�̻���
	htoa(RecBuff, &DevStat.acnt_id[0], 2);	
	sprintf((char*)strbuff, "  վ �� �� �ţ�%s\r\n", RecBuff);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char *)strbuff);
//����Ա��
	htoa(RecBuff, &DevStat.cur_driver_number[0], 1);	   // by liujihua
	sprintf((char*)strbuff, "  ����Ա���� ��%s\r\n", RecBuff);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);

	// ��ӡ�����û�ҳ����
	ucRet = EA_ucLinePrinter(hDprinterHandle, 0, "\f");
	if ( ucRet == EM_SUCCESS )
	{
		EA_vDisplay(2, "    ���δ�ӡ����!   ");
		EA_uiInkeyMs(2000);
	}
	return ;
}



