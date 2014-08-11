/******************************************************************************
Copyright 2013 �����״�ͨ�������޹�˾
All rights riserved.

�ļ���	��main.c
ģ�����ƣ��Ͳ�����POS��������
���ܸ�Ҫ���Ͳ�����POS�������� 

��ǰ�汾��0.1.5 
    	  1���޸ļ�¼�Ĵ��䷽ʽ
    	  2������һ�������ļ���¼��ʽ
 
��ǰ�汾��0.1.3 
    	  1.�����ͼ۸�����Чʱ��
    	  2.�޸��ͼ�ʱ���ֶ�����Ҫ����������󣬲����޸�
    	  3.�ӷ�������������Чʱ���趨����
		  4.�ֶ��޸��ͼۺ󣬽���־��Ϣ�ϴ�	
��ǰ�汾��0.1.2  
    	  1.�����ͼ۸�����Чʱ��
    	  2.�޸��ͼ�ʱ���ֶ�����Ҫ����������󣬲����޸�
    	  3.�ӷ�������������Чʱ���趨����
		  4.�ֶ��޸��ͼۺ󣬽���־��Ϣ�ϴ�
 
��ǰ�汾��0.1.0  
��ǰ�汾��0.0.9 
    	  1.�����е�float���������ͣ�ȫ��ת��Ϊdouble����
    	  2.�޸��ϰ�ʱ�䱣��
    	  3.������λ�����ݼ��� 0.001
    	  4.�޸ĵ�¼ʱ���޷������bug
 
��ǰ�汾��0.0.7 
��ǰ�汾��0.0.6
�޸���	��������
������ڣ�2014.3.26
����˵����1���޸�һ��bug������Ա��½û��ˢ����һ��ȡ�����͵�½�ɹ� 
    	  2�����Ӽ����������������
    	  3��������������Ҳ�Σ���ʾ��
    	  4����¼��ѯ�е������ݲ�ѯ��ŵ��ң�����ʮ������ʾ��
    	  5����ݷ�ʽ�� ��ʾ��ǰ�ͼ�
    	  6���ش��ϴ�СƱҲ�ŵ���ݷ�ʽ��
    	  7���ֶ����복���Ǹ����棬Ҫ����ȡ������ȡ�����˵�

 
��ǰ�汾��0.0.3
�޸���	��������
������ڣ�2014.3.26
����˵����������һ���� �����̣�Ա�������·��Ŀ���ÿ��ˢ����ʱ��Ҫ��֤��������� 
 
��ǰ�汾��0.0.2
�޸���	��������
������ڣ�2014.2.26
����˵����
 
��ǰ�汾��0.0.1
�޸���	��������
������ڣ�2013.6.16
����˵����
 
******************************************************************************/

#include "main.h"

/**************************************************************************/

//#define	 _zb_debug_tran_

DEV_STAT    DevStat;				//�豸״̬
CARD_INFO 	CardInfo;				//��Ƭ��Ϣ�����׼�¼�ɴ˱�������
ERROR_CARD_INFO ErrCardInfo;	    //����״̬
const char *VERSION = "VER:0.1.5";  //�汾��
GPRS gprs;

//gprsͨѶ���
extern int socketID;
extern struct sockaddr_in s_add;

//����ṹ���壬����������ƺ���
DevHandle hRS232Handle;    //ͨ�ô���
DevHandle hDprinterHandle; //��ӡ��
DevHandle hSIMHandle;     //SIM��
DevHandle hCpuHandle;      //CPU��
DevHandle hSam1Handle;      //SAM1��
DevHandle hSam2Handle ;      //SAM2��
DevHandle hMifsHandle;     //M1��
DevHandle hWLMHandle;     //GPRSģ��
//DevHandle hApptimerHandle; //��ʱ������
//INT16U   Apptimer_Delay;
DevHandle hUSBDHandle;     //usb�ӿ�������������
ET_DISK_INFO ptDiskInfo;
ET_PART_INFO ptPartInfo;

extern const char parmFileName[] ;
extern const char blackFileName[] ;
extern const char currecFileName[]; 
extern const char conrecFileName[]; 
extern const char hisrecFileName[]; 
extern const char grayrecFileName[] ;
extern const char clerkFileName[] ;

//LERK_STRUCT clerk_list[MAX_CLERK_NUM];
extern INT8U rec_query[2*20 +2];
extern INT8U  kuaijie_set[5*20 +2];


/*****************************************************************
����ԭ�ͣ�LowBatteryDisplay
����������������Ϊ��ص�����ʾ
������������
�������ƣ�	����/�����	����		����
 ��  �ߣ�   ������
 ��  �ڣ�   2012-11-24
 �޸���ʷ��
		����        �޸���      �޸�����
�� �� ֵ����
*****************************************************************/
INT8U LowBatteryDisplay(void)
{
	ET_PowerInfo PowerInfo;
//  ET_PowerType ucType;
//  ET_PowerCapLevel ucCapability;

	if( EA_ucGetPowerInfo(&PowerInfo) != EM_SUCCESS )
	{
		lcddisperr("���������");
		return notok;
	}

	if( PowerInfo.ucType == EM_power_TYPE_BATTERY )
	{
		if( PowerInfo.ucCapLevel == EM_power_CAPACITY_LEVEL_LOW )
		{
			lcddisperr("��ص���������");
			return notok;
		}
		else if( PowerInfo.ucCapLevel == EM_power_CAPACITY_LEVEL_CRITICAL )
		{
			lcddisp(1, 1, (void *)"��ص�������ϵͳ�޷�");
			lcddisp(2, 1, (void *)"   �������������   ");
			(void)EA_uiInkey(1);
			return notok;
		}
	}
	return ok;
}
/*****************************************************************
����ԭ�ͣ�Debugprintf
����������������Ϊ������Ϣ���
������������
�������ƣ�	����/�����	����		����
 ��  �ߣ�   ������
 ��  �ڣ�   2012-11-24
 �޸���ʷ��
		����        �޸���      �޸�����
�� �� ֵ����
*****************************************************************/
void Debugprintf(const char *fmt, ...)
{
//  int n;
//  char *arg;
//  va_list arg = (va_list)((char*)(&fmt) + 4);
//  n = vsprintf(sprint_buf, fmt, arg);
	EA_ucWriteDevice(hRS232Handle, strlen(fmt), 0, (char *)fmt);
}


/*****************************************************************
 ����ԭ�ͣ�main()
 ����������������
 ����������
 ����ֵ��   ��
 ��  �ߣ�   ������
 ��  �ڣ�   2012-11-24
 �޸���ʷ��
		����        �޸���      �޸�����
		------      ---------   -------------
*****************************************************************/
int main(int argc, char **argv)
{	
	INT8U   ret = 0;
	char    mainflag = 1;
	uchar   uckey;
	uint 	uikey;

	(void)EA_ucSetStopFlag( EM_DISABLE_STOP );  //������ϵͳ����˯��״̬��

reboot:
	ret = System_Init();      //ϵͳ��ʼ��,����PSAM��������GPRS��ʼ��
	if( ret != ok )		 
	{
		lcddisperr("ϵͳ��ʼ��ʧ��!");
		goto reboot;
	}

	Beep(1000);

	//(void)EA_ucSetStopFlag( EM_ENABLE_STOP );  //����ϵͳ����˯��״̬��
	for ( ;; )
	{

		if( mainflag == 1 )
		{
			DisplayWelcomeMenu();
			mainflag = 0;
		}

		uckey = EA_ucKBHit();
		if(uckey == EM_key_HIT)       //�а���
		{
			uikey = EA_uiInkey(0);    //��ȡ������ֵ
			switch ( uikey )
			{
				case EM_key_EXIT:     //�ػ�
				{
					mainflag = 1;
					goto shutdown;
					break;
				}

				case EM_key_F3:
				{
					Login();          //��¼����                    	
					mainflag = 1;
					break;
				}
				case EM_key_F1:	
				{
					Parm_Setting();   //��������
					mainflag = 1;
					break;
				}
				case EM_key_1:
				{
					gprs_test();     //�������Ӳ���
					mainflag = 1;
					break;
				}
				case EM_key_F2:
				{
					kuaijiefangshi(); //��ݷ�ʽ
//  				Gprs_Upload_data();      
//  				GPRS_Close();
					mainflag = 1;
					break;
				}
				default:
					break;
			}
		}
	}
shutdown:
	Power_Down();
}


void kuaijiefangshi(void)
{
	INT8S ret = 0;
	while(1)
	{

		ret = browse_menu(0, (void *)"", kuaijie_set, TRUE );
		switch ( ret )
		{
		   case 0://����������
				download_whiltelist();	
				break;
		   case 1: //�����ϴ�
			    Gprs_Upload_data();     
				break;	
		   case 2: //��ǰ�ͼ۲�ѯ
			    query_oil_price();     
			    break;	
		   case 3: //�ش��ϴ�СƱ
			    Reprint_sale_record();     
				break;	
		   case 4: //�ֶ����복��
			    man_set_busnum();     
				break;	
		   case -1:
			   return;
		   default:
				break;
		}
	}
	return ;
}

// *****************************************************************
// ���ܣ�		query_oil_price
// ˵��:		��ѯ��ǰ�ͼ�
// ��ڲ�����	
// ���ڲ�����	Null
// ���ߣ�	frank
// ����ֵ��		Null		
// *****************************************************************
void query_oil_price(void)
{
	//��ǰ�ͼ���ʾ
	EA_vCls();
	EA_vDisplay(2, "��Ȼ�� :%2.2fԪ/����", DevStat.price[0]/100.0);
	EA_vDisplay(3, "����93#:%2.2fԪ/�� ", DevStat.price[1]/100.0);
	EA_vDisplay(4, "����97#:%2.2fԪ/�� ", DevStat.price[2]/100.0);
	EA_vDisplay(5, "��  �� :%2.2fԪ/�� ", DevStat.price[3]/100.0);

	//��������˳�
	(void)EA_uiInkey(0);
	return;
}
// *****************************************************************
// ���ܣ�		man_set_busnum
// ˵��:		�ֶ����복��
// ��ڲ�����	
// ���ڲ�����	Null
// ���ߣ�
// ����ֵ��		Null		
// *****************************************************************
void man_set_busnum(void)
{
	INT8U i = 0;
	INT8U input1[20];
	uchar  uckey;
	uint   uikey;
	uchar ret;
	uchar     temp_uc[5];
//  if ( strcmp((void *)&DevStat.bus_number[0], "12345") == 0 )
//  {
//  	goto set_bus_number;
//  }

	EA_vCls();
	lcddisp(2, 1, (void *)"���복��:           ");
	strcpy((void *)input1, "");

	for ( ;; )
	{
		i = EA_ucGetInputStr(2, 10, 20, EM_BIG_FONT | EM_MODE_CHAR | EM_ALIGN_LEFT | EM_SHOW_ORIGINALLY
							 , 5, 5, 0, (void *)input1); //���������޸�ֻ���������ֺ���ĸ
		if ( i == EM_SUCCESS )
			break;
		if ( i == EM_ABOLISH )
			return;      //do nothing
	}

	memcpy((void *)&CardInfo.bus_number[0], (void *)input1, 5);

	WriteParam();

	EA_ucSetInverse(EM_lcd_INVON);
	lcddisp(3, 1, (void *)"      ����ɹ�      ");
	EA_ucSetInverse(EM_lcd_INVOFF);
	SleepMs(1500);

RESELECT:
	

	memcpy(temp_uc, &CardInfo.bus_number[0], 5);

	i = BlackName_Find((char *)temp_uc);	//�������ж�
	if ( i != ok )	 						//���ǰ�����
	{
		lcddisperr("�ÿ����ڰ�������");
		Beeperr();
		return;
	}

	EA_vCls();
	EA_vDisplay(1, "��  �� :³C-%s", CardInfo.bus_number);

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
				return ;
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

	return;
}
// *****************************************************************
// ���ܣ�		query_note_record
// ˵��:		��ѯ�ս�СƱ
// ��ڲ�����	
// ���ڲ�����	Null
// ���ߣ�
// ����ֵ��	-1 �ػ�				
// *****************************************************************
int query_note_and_record(void)
{
	uchar ret ;
	ret = browse_menu(0, (void *)"", rec_query, TRUE );
	switch ( ret )
	{
	   case 0://�սᵥ��ѯ
			Note_Query();				
			break;
	   case 1://���ʼ�¼��ѯ
			Record_Query();
			break;
	   default:
		    break;
	}
	return ok;
}


/******************************************************************************
 �������ƣ�print_work_note_history
 ������������ӡ�ս�����ʷƱ��
 �������ƣ�����/�����	����		����
 ����		
 
 ��  ��  ֵ��
 
 ��      ��	���ڸ���
 ��      �ڣ�2004-09-02
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
******************************************************************************/
void print_work_note_his(INT8U * rec)
{
	uchar       ucRet;
	INT8U		RecBuff[70];
	INT8U		strbuff[50];
	INT8U		time[7+2];


	_BANLANCE_IN_HAND * prec = (_BANLANCE_IN_HAND *)rec;

	EA_vCls();
	
	EA_vDisplay(1, "  ��ʷ�ս��ʵ���ӡ  ");//(int qx, int qy, char * ceStr, int xyFlag)
	EA_vDisplay(2, "       ��ȴ�...    ");//(int qx, int qy, char * ceStr, int xyFlag)
	SleepMs(1500);

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
///��ӡͷ
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char *)"�Ͳ��й���������˾\r\n");
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char *)"�ս��ʵ���ʷ��¼\r\n");
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char *)"------------------------------\r\n");
//���Ѵ���
	sprintf((void *)strbuff, "��Ȼ���������� ��%6d��\r\n", prec->consume_cnt[0]);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char *)strbuff);
	sprintf((void *)strbuff, "����93#���ʹ�����%6d��\r\n", prec->consume_cnt[1]);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char *)strbuff);
	sprintf((void *)strbuff, "����97#���ʹ�����%6d��\r\n", prec->consume_cnt[2]);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char *)strbuff);
	sprintf((void *)strbuff, "���ͼ��ʹ���   ��%6d��\r\n", prec->consume_cnt[3]);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char *)strbuff);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char *)"------------------------------\r\n");
//�������
	sprintf((void *)strbuff, "��Ȼ����� ��%6.2lfԪ\r\n", (double)(prec->consume_cap[0])/100);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char *)strbuff);
	sprintf((void *)strbuff, "����93#�����%6.2lfԪ\r\n", (double)(prec->consume_cap[1])/100);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char *)strbuff);
	sprintf((void *)strbuff, "����97#�����%6.2lfԪ\r\n", (double)(prec->consume_cap[2])/100);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char *)strbuff);
	sprintf((void *)strbuff, "�� �� �� ����%6.2lfԪ\r\n", (double)(prec->consume_cap[3])/100);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char *)strbuff);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char *)"------------------------------\r\n");
//���ѽ��
	sprintf((void *)strbuff, "��Ȼ����� ��%6.2lfԪ\r\n", (double)(prec->consume_amt[0])/100);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char *)strbuff);
	sprintf((void *)strbuff, "����93#��%6.2lfԪ\r\n", (double)(prec->consume_amt[1])/100);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char *)strbuff);
	sprintf((void *)strbuff, "����97#��%6.2lfԪ\r\n", (double)(prec->consume_amt[2])/100);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char *)strbuff);
	sprintf((void *)strbuff, "�� �� �� �%6.2lfԪ\r\n", (double)(prec->consume_amt[3])/100);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char *)strbuff);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char *)"------------------------------\r\n");
	sprintf((void *)strbuff, "��  ��  �� ��%6.2lfԪ\r\n", (double)(prec->consume_amt[0]+prec->consume_amt[1]+prec->consume_amt[2]
															+prec->consume_amt[3])/100);

	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char *)strbuff);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char *)"------------------------------\r\n");

//�ϰ�ʱ��	
	memcpy(time, (INT8U*)&prec->work_datetime, 7);
	sprintf((void *)strbuff, "�ϰ�ʱ�䣺\r\n");
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	sprintf((void *)strbuff, "%02X%02X-%02X-%02X %02X:%02X:%02X\r\n", time[0],time[1],time[2],time[3],time[4],time[5],time[6]);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);	
//�°�ʱ��
	memcpy(time, (INT8U*)&prec->down_datetime,7);
	sprintf((void *)strbuff, "�°�ʱ�䣺\r\n");
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);
	sprintf((void *)strbuff, "%02X%02X-%02X-%02X %02X:%02X:%02X\r\n", time[0],time[1],time[2],time[3],time[4],time[5],time[6]);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char*)strbuff);	

//���з���
	strcpy((void *)strbuff, "---------------------------------- \r\r\r");
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char *)strbuff);
	
//�豸���
	htoa(RecBuff, &prec->equ_id[0], 3);
	sprintf((void *)strbuff, "�豸���  ��%s\r\n", RecBuff);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char *)strbuff);
//�̻���
	htoa(RecBuff, &prec->acnt_id[0], 2);
	sprintf((void *)strbuff, "վ  ��  �ţ�%s\r\n", RecBuff);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char *)strbuff);
//����Ա��
	htoa(RecBuff, &prec->oper_id[0], 1);
	sprintf((void *)strbuff, "����Ա���ţ�%s\r\n", RecBuff);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char *)strbuff);
	// ��ӡ�����û�ҳ����
	ucRet = EA_ucLinePrinter(hDprinterHandle, 0, "\f");
	if ( ucRet == EM_SUCCESS )
	{
		EA_vDisplay(2, "���δ�ӡ����!");
		Beep(1000);
		EA_uiInkeyMs(2000);
	}
		
}


// *****************************************************************
// ���ܣ�		AnalyAndDisplayNote
// ˵��:		��ѯ�ս������ʾ�ս���Ϣ
// ��ڲ�����	
// ���ڲ�����	Null
// ���ߣ�
// ����ֵ��							
// *****************************************************************
void AnalyAndDisplayNote(INT8U * strTradeInfo)
{
	INT8U	time[7+2]; 
	char    strbuff[30];
	INT8U   temp[30];

	_BANLANCE_IN_HAND * prec = (_BANLANCE_IN_HAND *)strTradeInfo;

	EA_vCls();


	sprintf(strbuff, "���ʹ���:%7d��", (prec->consume_cnt[0]+prec->consume_cnt[1]+prec->consume_cnt[2]+prec->consume_cnt[3]));
	EA_vDisplay(1, strbuff);

//���ѽ��
	sprintf(strbuff, "���ͽ��:%6.2lfԪ", (double)(prec->consume_amt[0]+prec->consume_amt[1]+prec->consume_amt[2]+prec->consume_amt[3])/100.0);
	EA_vDisplay(2, strbuff);

//�ϰ�ʱ��	
	memcpy(time, (INT8U*)&prec->work_datetime, 6);
	htoa(temp, time, 6);
	sprintf(strbuff, "�ϰ�ʱ��:%s", temp);
	EA_vDisplay(4, strbuff);
	
//����Ա
	memcpy(strbuff, (INT8U*)&prec->oper_id, 3);
	htoa((void *)temp, (void *)strbuff, 3);
	sprintf(strbuff, "�� �� Ա:%s",temp);
	EA_vDisplay(3, strbuff);
}

/*****************************************************************
����ԭ�ͣ�AppTimerISR
������������ʱ������
����������������������ʱ��
�������ƣ�	����/�����	����		����
-----------		-----------	------	   	------- 
��	  �ߣ� ������ 
�� �� ֵ����
*****************************************************************/
void AppTimerISR()
{
	int ret = 0;

	if((socketID = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
//  	lcddisperr("SOCK����ʧ��");
		return;
	}

	ret = connect(socketID,(struct sockaddr_in *)(&s_add), sizeof(struct sockaddr_in));
	if( ret == -1 )
	{
		return;
	}
	if( write(socketID, (void *)&"0000", 4) == -1)
	{
//  	lcddisperr("����ʧ��");
//  	goto Err_sockoff;
	}
	close(socketID);
	return;
}
/*****************************************************************
����ԭ�ͣ�DisplayWelcomeMenu
����������������Ϊ��ʾ��ӭ����
������������
�������ƣ�	����/�����	����		����
-----------		-----------	------	   	-------
�� �� ֵ����
*****************************************************************/
void DisplayWelcomeMenu(void)
{

	EA_vCls();
	EA_vDrawIcons();

	EA_ucSetInverse(EM_lcd_INVOFF);
	EA_vDisp(1, 1, " �Ͳ��й���������˾ ");
	EA_vDisp(2, 1, "    �ֳ�POS���ͻ�   ");
	EA_vDisp(3, 6, (void *)VERSION);


	if ( DevStat.record_number >= MAX_REC_NUM )
	{
		EA_vDisp(3, 1, "  ��¼����, ���ϴ�  ");
	}

	EA_vDisp(4, 1,     "����    ���    ��½");
	return ;
}

/*****************************************************************
����ԭ�ͣ�SystemCheck
����������������Ϊϵͳ���
������������
�������ƣ�	����/�����	����		����
-----------		-----------	------	   	------- 
��    �ߣ� ������ 
��	  �ڣ� 2012-11-28
�� �� ֵ����
			 1:�ɹ�
			 0:ʧ��
*****************************************************************/
INT8U SystemCheck(void)
{
	INT8U   ret = 0;

	EA_vCls();
//  EA_vDisp(2, 1, "  PSAM�������...   ");
//  (void)EA_uiInkeyMs(2000);
	ret = SamCheck();
	if ( ret != ok )
	{
		EA_vCls();
		lcddisp(1, 1, (void *)"   PSAM��������   ");
		lcddisp(2, 1, (void *)"     ������     ");
		(void)EA_uiInkeyMs(2000);
		return notok;
	}
//  lcddisperr(" PSAM�����ɹ�!");
	return ok;
}
/*****************************************************************
 ����ԭ�ͣ�System_Init()
 ���������������ȳ�ʼ��
 ����������

 ����ֵ��   OK ?  NOTOK

 ��  �ߣ�   ����/������
 ��  �ڣ�   2012-11-24
 �޸���ʷ��
		����        �޸���      �޸�����
		------      ---------   -------------
*****************************************************************/
INT8U System_Init(void)
{
	
	INT8U   i = 0;
	INT16U  key = EM_key_NOHIT;

	EA_iSetMode(0);    //��ʾģʽ����

	EA_vCls();
	EA_ucSetInverse(EM_lcd_INVOFF);
	EA_vDisp(2, 1, "   ϵͳ��ʼ����...  ");
	key = EA_uiInkeyMs(1500);

	if ( key == EM_key_F3 )
	{
		format_process();    		  //���Խ���ϵͳ��ʽ������
	}

	memset((void *)&DevStat, 0x00, sizeof(DevStat));

	i = ReadParam();          
	if ( i != ok )
	{
		param_factory_default(0);     //��ʽ����Ҫ����DEV.STATE
	}

	DevStat.POS_type = POS_T_NORMAL;  //������Ϊ�����ɼ�

	//��Ҫ�������ֳ�ʼ������
	appfile_Init();

	DevStat.record_number = query_rec_num();

	memset((void *)&CardInfo, 0x00, sizeof(CardInfo));        //���������Ϣ
	memset((void *)&ErrCardInfo, 0x00, sizeof(ErrCardInfo));  //������󿨵���Ϣ

	//����Ӧ�����һ������������������е��豸�ľ���ṹ!!!!
	i = Device_Handle_Init();
	if( i != ok)
	{
		return notok;
	}

	if( LowBatteryDisplay() != ok )     //��ص������
	{	
		lcddisperr("��ص������ʧ��");
		return notok;
	}
	if( SystemCheck() != ok )          //PSAMϵͳ���
	{
		lcddisperr("PSAM�����ʧ��");
		return notok;
	}
	return ok;
}
/*****************************************************************
 ����ԭ�ͣ�appfile_Init(void)
 �����������û��ļ�ϵͳ��ʼ��
 ����������	
				 
 ����ֵ��	��
 
 ��  �ߣ� ������
 ��  �ڣ�	2012-11-24
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
*****************************************************************/
void appfile_Init(void)
{
	INT8U ret;
	//����Ա��½
	//zbClerkFileInit();
	//���°�����
	ret = download_whiltelist();
	if(ret == notok)
	{
		EA_vDisplay(2, "���°�����ʧ��!");
		EA_vDisplay(3, "���¼���ֶ�����");
		Beeperr();
	}
	//���±�۱�
	ret = download_price_table();
	if(ret == notok)
	{
		EA_vDisplay(2, "�����ͼ�ʧ��!");
		EA_vDisplay(3, "���¼���ֶ�����");
		Beeperr();
	}
	GPRS_Close();   //����Ϊ���³���֮��Ҫ���̹ر�GPRSģ��
}


/*****************************************************************
 ����ԭ�ͣ�zbClerkFileInit(void)
 ��������������Ա�ļ���ʼ��
 ����������	
				 
 ����ֵ��	OK ? NOTOK
 
 ��  �ߣ� ������
 ��  �ڣ�	2012-11-24
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
*****************************************************************/
char zbClerkFileInit(void)
{
	char upd=1;
	uchar i;
	uchar ucOpenID;
	uchar ucRet;

	if (pf_exist((void *)clerkFileName, &ucOpenID)==FALSE)
	{
		ucRet = pf_creat((void *)clerkFileName, sizeof(CLERK_STRUCT));
		upd=1;
	}
	else
	{
		ucRet = pf_close(ucOpenID);
		upd=0;
	}
	
	memset(&clerk_list, 0, sizeof(clerk_list));

	if (upd) 
	{
		for (i=0; i<MAX_CLERK_NUM; i++)
		{
  			clerk_list[i].id[0] = '0';
	  		clerk_list[i].id[1] = '0'+i;
//			memset(clerk_list[i].id, '0'+i, 2);
			clerk_list[i].right = ID_OPERATOR;
			memset(clerk_list[i].pw,'0'+i,4);
		}

		//set the default
//		memset(clerk_list[9].id,'8',2);
//		memcpy(clerk_list[9].pw,"22446868",8);
//		clerk_list[9].right=ID_SUPERVISOR;

		//////////////////////////////////////////////////////////////////////////
//		memset(clerk_list[0].id,'0',2);
//		memset(clerk_list[0].pw,'0',6);
//		clerk_list[0].right=ID_MANAGER;

		//////////////////////////////////////////////////////////////////////////
//		strcpy((void *)&clerk_list[1].id[0], "01");
//		memset(clerk_list[1].pw,'1',4);
//		clerk_list[1].right=ID_OPERATOR;

		zbClerkSaveFile();
	}
	
	zbClerkReadFile();

	return ok;
}

/*****************************************************************
 ����ԭ�ͣ�zbClerkSaveFile(void)
 ��������������Ա�ļ���ʼ��
 ����������	
				 
 ����ֵ��	OK ? NOTOK
 
 ��  �ߣ� ������
 ��  �ڣ�	2012-11-24
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
*****************************************************************/
char zbClerkSaveFile(void)
{
//	uchar ucret;
	uchar ucOpenID;

	pf_open((void *)clerkFileName, &ucOpenID);

	pf_write(ucOpenID, 0, (uchar *)clerk_list);

	pf_close(ucOpenID);

	return TRUE;
}

/*****************************************************************
 ����ԭ�ͣ�zbClerkReadFile(void)
 ��������������Ա�ļ���ʼ��
 ����������	
				 
 ����ֵ��	OK ? NOTOK
 
 ��  �ߣ� ������
 ��  �ڣ�	2012-11-24
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
*****************************************************************/
char zbClerkReadFile(void)
{
	uchar ucOpenID;
	uint  uiRecnum;

	pf_open((void *)clerkFileName, &ucOpenID);

	pf_read(ucOpenID, 1, 1, &uiRecnum, (uchar *)clerk_list);
	
	pf_close(ucOpenID);

	return TRUE;
}
/*****************************************************************
 ����ԭ�ͣ�Device_Handle_Init()
 �����������豸���о����ʼ��
 ����������	
				 
 ����ֵ��	OK ? NOTOK
 
 ��  �ߣ� ������
 ��  �ڣ�	2012-11-24
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
*****************************************************************/
INT8U Device_Handle_Init(void)
{
	uchar  ucRet;
	uint   cap_sd;

	if (EA_ucOpenDevice("RFCARD", EM_mifs_TYPEA, &hMifsHandle) != EM_SUCCESS)
	{
		lcddisperr("��������ʼ��ʧ��");
		return notok;
	}
	if ( EA_ucOpenDevice("PRINT", EM_io_REALMODE, &hDprinterHandle) != EM_SUCCESS )
	{
		lcddisperr("��ô�ӡ�������ʧ��!");
		return notok;
	}
	if ( EA_ucOpenDevice("USBD", EM_io_REALMODE, &hUSBDHandle) != EM_SUCCESS )
	{
		lcddisperr("��ô�ӡ�������ʧ��!");
		return notok;
	}
//  if ( EA_ucOpenDevice("WLM", 0, &hWLMHandle) != EM_SUCCESS )
//  {
//  	lcddisperr("���GPRS���ʧ��!");
//  	return notok;
//  }
///////////////////////////////////////////////��ӡ�����ò���
	ucRet =  EA_ucInitPrinter(hDprinterHandle);
	if ( ucRet != EM_SUCCESS )
	{
		EA_vDisplay(2, "��ʼ����ӡ��оʧ��!");
		return notok;
	}
	ucRet = EA_ucSetPrinterFormat(hDprinterHandle, EM_prn_XSPACE, 0);
	if ( ucRet != EM_SUCCESS )
	{
		lcddisperr("��ӡ�ַ��������ʧ��");
		return notok;
	}
	ucRet =  EA_ucSetPrinterFormat(hDprinterHandle, EM_prn_YSPACE, 8);
	if ( ucRet != EM_SUCCESS )
	{
		lcddisperr("��ӡ�о�����ʧ��");
		return notok;
	}

	//  ucRet = EA_ucSetPrinterFormat(hDprinterHandle, EM_prn_PAGESPACE, 578);
	ucRet = EA_ucSetPrinterFormat(hDprinterHandle, EM_prn_PAGESPACE , 180);
	if ( ucRet != EM_SUCCESS )
	{
		lcddisperr("��ӡҳ��������ʧ��");
		return notok;
	}

	ucRet = EA_ucSetPrinterFormat(hDprinterHandle, EM_prn_ASCII_DOTTYPE, EM_prn_ASCIIDOT24X12); 
	if ( ucRet != EM_SUCCESS )
	{
		lcddisperr("��ӡASCII�ַ�����ʧ��");
		return notok;
	}
	ucRet = EA_ucSetPrinterFormat(hDprinterHandle, EM_prn_ASCII_PRNTYPE, EM_prn_ASCIIPRN1X1);
	if ( ucRet != EM_SUCCESS )
	{
		lcddisperr("��ӡASCII�ַ���Сʧ��");
		return notok;
	}
	ucRet = EA_ucSetPrinterFormat(hDprinterHandle, EM_prn_HZ_DOTTYPE, EM_prn_HZDOT24X24);
	if ( ucRet != EM_SUCCESS )
	{
		lcddisperr("��ӡ��������ʧ��!");
		return notok;
	}

	ucRet = EA_ucSetPrinterFormat(hDprinterHandle, EM_prn_HZ_PRNTYPE, EM_prn_HZPRN1X1);
	if ( ucRet != EM_SUCCESS )
	{
		lcddisperr("��ӡ���ִ�С����ʧ��");
		return notok;
	}	

	ucRet = EA_ucSetPrinterFormat(hDprinterHandle, EM_prn_PRNGRAY, 4);
	if ( ucRet != EM_SUCCESS )
	{
		lcddisperr("��ӡ�Ҷ�����ʧ��");
		return notok;
	}
/////////////////////////////////////////////////////////////////////////////��ʼ��ʱ��ֻ����һ��
	if (EA_ucOpenDevice("SAM1CARD", 0, &hSam1Handle) != EM_SUCCESS)
	{
		lcddisperr("PSAM����ʼ��ʧ��");
		return notok;
	}
//  if (EA_ucOpenDevice("USERCARD", 0, &hCpuHandle) != EM_SUCCESS)
//  {
//  	lcddisperr("CPU����ʼ��ʧ��");
//  	return notok;
//  }
//  if (EA_ucICInitModule(hCpuHandle, EM_ic_VOLDEFAULT, EM_ic_TCRMODE) != EM_SUCCESS)
//  {
//      lcddisperr("����CPU�淶ʧ��");
//   	return notok;
//  }
    ucRet = EA_ucOpenDevice("USBD", 0, &hRS232Handle);
	if ( ucRet != EM_SUCCESS )
	{
		(void)EA_ucCloseDevice(&hRS232Handle);
		lcddisperr("COM�ڳ�ʼ��ʧ��!");
		return notok;
	}
	ucRet = EA_ucInitRS232(hRS232Handle, EM_232_115200BPS, EM_232_NOPAR, EM_232_8DBS);
	if ( ucRet != EM_SUCCESS )
	{
		(void)EA_ucCloseDevice(&hRS232Handle);
		lcddisperr("COM������ʧ��!");
		return notok;
	}
//��ȡSD����Ϣ
	ucRet = EA_ucGetDiskInfo(EM_DISK_SDCARD, &ptDiskInfo);
	if ( ucRet != EM_SUCCESS )
	{
		lcddisperr("�����SD��!");
		return notok;
	}
	cap_sd = ptDiskInfo.ullSize / 0x100000;
	EA_vDisplay(3, "  SD����������%dM", cap_sd);
	SleepMs(2000);
	if(cap_sd < 1)
	{
		lcddisperr("�뼰ʱ����SD������!");
		Beeperr();
	}
//  // ����ռ����洢���̵ķ����б�
//  ptPartList = EA_pvMalloc(ptDiskInfo.uiPartNum * sizeof(ptPartList));
//  if (!ptPartList)
//  {
//  	lcddisperr("����SD��������ʧ��!");
//  	return notok;
//  }
//  // ��ȡ���̵ķ����б�
//  ucRet = EA_ucGetPartList(EM_DISK_SDCARD, ptDiskInfo.uiPartNum, &uiRealPartNum, ptPartList);
//  if (ucRet != EM_SUCCESS)
//  {
//  	EA_vFree(ptPartList);
//  	lcddisperr("��ȡSD���б�ʧ��!");
//  	return notok;
//  }
//
//  EA_vDisplay(1, "������Ŀ:%d", ptDiskInfo.uiPartNum);
//  SleepMs(3000);

	ucRet = EA_ucGetPartInfo(EM_DISK_SDCARD, 0, &ptPartInfo);
	if (ucRet != EM_SUCCESS)
	{
		lcddisperr("��ȡSD��������Ϣʧ��!");
		return notok;
	}

	return ok;
}
/******************************************************************************
 �������ƣ�TimeFormatChk
 ������������ʱ�ӣ���ʱ�Ӹ�ʽ�Ƿ���ȷ
 ����������
 �������ƣ�	����/�����	����		����
				
 ��  ��  ֵ��ok(0)-�Լ�ɹ�
				 notok(0xFF)-�Լ�ʧ��
				   
 ��      ��	������
 ��      �ڣ�2004-09-02
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
******************************************************************************/
INT8U TimeFormatChk(void)
{
	INT8U i = 0;
	BUS_TIME ltime;

	i = Get_Time(&ltime);
	if ( i != ok )
	{
		return notok;
	}

	i = CheckTimeFormat(&ltime);
	return i;
}

/******************************************************************************
 �������ƣ�TimeModify
 ����������Ҫ������ʱ�䣬�����޸�
 ����������
 �������ƣ�	����/�����	����		����
				
 ��  ��  ֵ����
				   
 ��      ��	������
 ��      �ڣ�2004-09-02
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
******************************************************************************/
void TimeModify(void)
{
	INT8U i = 0;
	INT8U input[50];
	INT8U buf[50];
	BUS_TIME ltime;


	for ( ;; )
	{
		EA_vCls();
//  	Get_Time(&ltime);
		EA_vDisp(1, 1, "��ǰ��Чʱ��:");
		sprintf((void *)buf, "       %02X:%02X        ", DevStat.effect_time.hour, DevStat.effect_time.minute);
		EA_vDisp(2, 1, (void *)buf);
		EA_vDisp(3, 1, "�������µ�ʱ��:");
		(void)EA_ucClrKeyBuf();
		strcpy((void *)input, "");

		i = EA_ucGetInputStr(4, 8, 20, EM_BIG_FONT | EM_MODE_NUMBER | EM_ALIGN_LEFT | EM_SHOW_ORIGINALLY
		, 2, 2, 0, (void *)input);
		if ( i == EM_ABOLISH )
			return;
		if ( i != EM_SUCCESS )
			continue;
		
		i = EA_ucGetInputStr(4, 11, 20, EM_BIG_FONT | EM_MODE_NUMBER | EM_ALIGN_LEFT | EM_SHOW_ORIGINALLY
		, 2, 2, 0, (void *)input);
		if ( i == EM_ABOLISH )
			return;
		if ( i != EM_SUCCESS )
			continue;
		//  	sprintf(dbuf, "%s", input);
		//  	EA_vDisp(3, 1, dbuf);
		//  	sprintf(dbuf, "result:%02X", i);
		//  	EA_vDisp(4, 1, dbuf);

		ltime.hour = (ascii_to_hex(input[8]) << 4) | ascii_to_hex(input[9]);
		ltime.minute = (ascii_to_hex(input[11]) << 4) | ascii_to_hex(input[12]);

		if ( ltime.hour > 0x23 )       //СʱӦ��0-23֮��
			return ;


		
		if ( ltime.minute > 0x59 )                                   //����Ӧ��0-59֮��
			return ;


		if ( (ltime.hour > 0x23) || (ltime.minute > 0x59) )
		{
			EA_vCls();
			EA_vDisp(4, 1, "ʱ���ʽ����");
			SleepMs(1500);
			continue;
		}

		DevStat.effect_time.hour = ltime.hour;
		DevStat.effect_time.minute = ltime.minute;
		WriteParam();

		EA_vCls();
		EA_vDisp(4, 1, "��Чʱ���޸ĳɹ�");
		SleepMs(1500);
		break;
	}
}


/******************************************************************************
 �������ƣ�format_process
 ������������ʽ������
 ����������
 �������ƣ�	����/�����	����		����
				
 ��  ��  ֵ����
				   
 ��      ��	������/������
 ��      �ڣ�2012-11-24
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
******************************************************************************/
void format_process(void)
{
	INT8U i = 0;
	INT8U input[10];
	INT8U passwd[10] = "22884646";

	EA_vCls();
	(void)EA_vDisp(1, 1, (void *)"     ϵͳ��ʽ��     ");
	(void)EA_vDisp(2, 1, (void *)"��������:");
	(void)EA_ucClrKeyBuf();
	strcpy((void *)input, "");

	for ( ;; )
	{
		i = EA_ucGetInputStr(2, 10, 20, EM_BIG_FONT | EM_MODE_NUMBER | EM_ALIGN_LEFT | EM_SHOW_PASSWORD
							 , 8, 8, 0, (void *)input);
		if ( i == EM_SUCCESS )
			break;
		if ( i == EM_ABOLISH )
			return;
	}

	if ( strcmp((void *)input, (void *)passwd) == 0 )
	{
		lcddisperr("ϵͳ���ڸ�ʽ��...");
		pf_format();
	}
	else
	{
		EA_ucSetInverse(EM_lcd_INVON);
		lcddisp(4, 1, (void *)"�������!");
		EA_ucSetInverse(EM_lcd_INVOFF);
		return;
	}
	//��ʼ������
	param_factory_default(1);

}
/******************************************************************************
 �������ƣ�Power_Down
 ���������������ֳֻ�
 �������ƣ�	����/�����	����		����
 ����		
 
 ��  ��  ֵ��
 
 ��      ��	���ڸ���/������
 ��      �ڣ�2012-11-27

 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
******************************************************************************/
void Power_Down(void)
{
//  uint uisleep_delay;   //������Ϊȫ�ֱ���
//  EA_ucSetStopFlag(EM_ENABLE_STOP);  //��������˯��״̬��
	lcddisperr("ϵͳ���ڹػ�...");
//  uisleep_delay = 2;
	close_gprs_module();
//  EA_vSetSaveResTime(uisleep_delay);
	EA_vSystemManage (EM_SHUTDOWN);
	
//  (void)EA_uiInkey(0);
}
/******************************************************************************
 �������ƣ�Power_Down
 ���������������ֳֻ�
 �������ƣ�	����/�����	����		����
 ����		
 
 ��  ��  ֵ��
 
 ��      ��	���ڸ���/������
 ��      �ڣ�2012-11-27

 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
******************************************************************************/
void System_reset(void)
{
//  uint uisleep_delay;   //������Ϊȫ�ֱ���
//  EA_ucSetStopFlag(EM_ENABLE_STOP);  //��������˯��״̬��
	lcddisperr("ϵͳ��������...");
//  uisleep_delay = 2;
	close_gprs_module();
//  EA_vSetSaveResTime(uisleep_delay);
	EA_vSystemManage (EM_RESET);
	
	//(void)EA_uiInkey(0);
}