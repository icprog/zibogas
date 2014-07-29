/******************************************************************************
Copyright 2013 �����״�ͨ�������޹�˾
All rights riserved.

�ļ���	��main.c
ģ�����ƣ��Ͳ�����POS��������
���ܸ�Ҫ���Ͳ�����POS�������� 
 

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
#define 	MENU_COUNT		4
#define		Error			0
#define     PowerOff		-1

//#define DEBUG
//�������ÿ���˾�����Ƿ�ˢ����־
#define     ICON_A			0x40//��ˢ���ÿ�
#define		ICON_B			0x20//��ˢ˾����
//#define		DEBUG
// *****************************************************************
// ���ܣ�		���˵�����
// *****************************************************************
//INT8U  main_menu[4*16+1] = {"1.��������      2.��Ϣ��ѯ      3.���ݴ���      4.����Ա�����޸�"};

DEV_STAT    DevStat;				//�豸״̬
//GRAYRECORD GrayRecord;			//�Ҽ�¼
CARD_INFO CardInfo;					//��Ƭ��Ϣ�����׼�¼�ɴ˱�������
ERROR_CARD_INFO ErrCardInfo;	    //����״̬
const char *VERSION = "0.0.2";     //�汾��
char dbuf[100];

extern int socketID;
//INT16U   Apptimer_Delay;
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
//Devhandle hRS232Handle;     //usb�ӿ�������������

extern const char parmFileName[] ;
extern const char blackFileName[] ;
extern const char currecFileName[]; 
extern const char conrecFileName[]; 
extern const char hisrecFileName[]; 
extern const char grayrecFileName[] ;
extern const char clerkFileName[] ;

CLERK_STRUCT clerk_list[MAX_CLERK_NUM];

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
//	ET_PowerType ucType;
//	ET_PowerCapLevel ucCapability;

	if( EA_ucGetPowerInfo(&PowerInfo) != EM_SUCCESS )
	{
		lcddisperr("batt check failed.");
		return notok;
	}

	if( PowerInfo.ucType == EM_power_TYPE_BATTERY )
	{
		if( PowerInfo.ucCapLevel == EM_power_CAPACITY_LEVEL_LOW )
		{
			lcddisperr(" ��ص��������� ");
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

void Debugprintf(const char *fmt, ...)
{

//  int n;
	//char *arg;

//  va_list arg = (va_list)((char*)(&fmt) + 4);

//  n = vsprintf(sprint_buf, fmt, arg);


	EA_ucWriteDevice(hRS232Handle, strlen(fmt), 0, (char *)fmt);
}


/*****************************************************************
 ����ԭ�ͣ�Menu_Sure_or_Cancel
 ����������ѡ��ȷ�ϻ�����ȡ�����棬ֻ������ѡ�
 ����������

 ����ֵ��  -1������0

 ��  �ߣ�   ������
 ��  �ڣ�   2012-11-24
 �޸���ʷ��
		����        �޸���      �޸�����
		------      ---------   -------------
*****************************************************************/
//INT8U Menu_Sure_or_Cancel(const char * menu_line )
//{
//    INT16U key = 0;
//    int  disp_pos = 0;
//    int  menu_pos = 0;
//    int  m = 0, h = 0, i = 0;
//    INT8U  menu_text[2][22 * 2] =
//    {
//        "ȷ��                ",
//        "ȡ��                "
//    };
//    EA_vCls();
//    EA_vTextOut(0, 0, EM_key_FONT8X16, 0, 1, 1, (char *)menu_line);
//
//    for (;;)
//    {
//        key = EA_uiInkey(0);
//        switch ( key )
//        {
//           case EM_key_INTERPUNCTION:           //��
//               disp_pos--;
//               if ( disp_pos < 0 )
//                   disp_pos = 0;
//               menu_pos--;
//               if ( menu_pos < 0 )
//                   menu_pos = 0;
//
//               EA_vCls();   //�����Ļ
//               EA_ucSetInverse(EM_lcd_INVON);
//               lcddisp(disp_pos + 1, 1, &menu_text[menu_pos][0]);       //��ʾ��ǰ�˵�������
//               EA_ucSetInverse(EM_lcd_INVOFF);
//
//               m = 1;
//               for ( i = disp_pos + 1; i <= 3; i++ )
//               {
//                   lcddisp(i + 1, 1, &menu_text[menu_pos + m][0]);
//                   m++;
//               }
//
//               break;
//
//           case EM_key_LETTER:                  //��
//               disp_pos++;
//               if ( disp_pos > 2 )
//                   disp_pos = 2;
//               menu_pos++;
//               if ( menu_pos > 2 )
//                   menu_pos = 2;
//
//
//               EA_vCls();   //�����Ļ
//               EA_ucSetInverse(EM_lcd_INVON);
//               lcddisp(disp_pos + 1, 1, &menu_text[menu_pos][0]);       //��ʾ��ǰ�˵�������
//               EA_ucSetInverse(EM_lcd_INVOFF);
//
//               m = 1;
//               for ( i = disp_pos + 1; i <= 3; i++ )
//               {
//                   lcddisp(i + 1, 1, &menu_text[menu_pos + m][0]);
//                   m++;
//               }
//               m = 1;
//               for ( h = disp_pos - 1; h >= 0; h-- )
//               {
//                   lcddisp(h + 1, 1, &menu_text[menu_pos - m][0]);
//                   m++;
//               }
//
//               break;
//
//           case EM_key_ENTER:
//           if( menu_pos == 1 )
//               return -1;
//           else                   //ȷ��
//               return menu_pos;
//               break;
//
//           case EM_key_CANCEL:
//                   return -1;
//                   break;
//
//           default:
//               break;
//        }
//    }
//}
/*****************************************************************
 ����ԭ�ͣ�Debug_string(char *str)
 �������������ڵ����������
 ����������str��ӡ���ַ���

 ����ֵ��   ��

 ��  �ߣ�   ������
 ��  �ڣ�   2013-06-24
 �޸���ʷ��
		����        �޸���      �޸�����
		------      ---------   -------------
*****************************************************************/
//void Debug_string(char *str)
//{
//	EA_ucWriteDevice(hUSBDHandle, sizeof(str), 0, (void *)str);
//}

/*****************************************************************
 ����ԭ�ͣ�main()
 ����������������
 ����������

 ����ֵ��   ��

 ��  �ߣ�   ����/������
 ��  �ڣ�   2012-11-24
 �޸���ʷ��
		����        �޸���      �޸�����
		------      ---------   -------------
*****************************************************************/
int main(void)
{	
	INT8U   ret = 0;
	char    mainflag = 1;
	INT8S   buf[30];
	uchar   uckey;
	uint 	uikey;
	BUS_TIME ltime;

	(void)EA_ucSetStopFlag( EM_DISABLE_STOP );  //������ϵͳ����˯��״̬��

reboot:
	ret = System_Init();     //ϵͳ��ʼ��,����PSAM��������GPRS��ʼ��
	if( ret != ok )		 
	{
		lcddisperr("ϵͳ��ʼ��ʧ��!");
		goto reboot;
	}

	Beep(1000);

	for ( ;; )
	{
		Get_Time(&ltime);
		sprintf((void *)buf, "  %02X%02X-%02X-%02X %02X:%02X",
				ltime.century, ltime.year, ltime.month, ltime.day, ltime.hour, ltime.minute);
//  	EA_vTextOut(0, 0, EM_key_FONT8X16, 0, 1, 1, (char *)buf);
		EA_vDisplay(3, (void *)buf);
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
					Login();                  //��¼����                    	
					mainflag = 1;
					break;
				}
				case EM_key_F1:	
				{
					Parm_Setting();          //��������
					mainflag = 1;
					break;
				}
				case EM_key_1:
				{
					GPRS_TEST();             //�������Ӳ���
					mainflag = 1;
					break;
				}
				case EM_key_F2:
				{
//  				QUERY_NOTE_RECORD();
					Gprs_Upload_data();      //�����ϴ�
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

// *****************************************************************
// ���ܣ�		QUERY_NOTE_RECORD
// ˵��:		��ѯ�ս�СƱ
// ��ڲ�����	
// ���ڲ�����	Null
// ���ߣ�
// ����ֵ��	-1 �ػ�				
// *****************************************************************
int QUERY_NOTE_RECORD(void)
{
	int         count;
	INT32U      RecordCount = 0;
	uchar       uckey; 
	uint        uikey; 
	uchar 		ucOpenID=0;
	INT8U 		rec[60];

	memset(rec, 0, sizeof(rec));

	if ( EA_ucPFOpen( (uchar *)hisrecFileName, &ucOpenID) != EM_ffs_SUCCESS )
	{
		lcddisperr("���ս��ļ�ʧ��");
		return 0;
	}
	// ��ȡ��Ч��Ϣ����
	(void)EA_ucPFInfo(ucOpenID, EM_ffs_VALIDREC, &RecordCount);
	if( RecordCount == 0)
	{
		lcddisperr("û���ս���Ϣ");
		(void)EA_ucPFClose(ucOpenID);
		return 0;//��ʾû�м�¼
	}
	count = RecordCount;//���һ��		

	if(EA_ucPFReadRec(ucOpenID, count, 1, &RecordCount, rec) != EM_SUCCESS)
	{
		(void)EA_ucPFClose(ucOpenID);
		lcddisperr((void *)"��ȡ������");
		return 0;
	}

	AnalyAndDisplayNote(rec);

	while ( 1 )
	{
		uckey = EA_ucKBHit();
		if(uckey == EM_key_HIT)    //�а���
		{
			uikey = EA_uiInkey(0);    //��ȡ������ֵ
			if ( (uikey == EM_key_EXIT) || (uikey == EM_key_CANCEL) )			//�˳�
			{
				(void)EA_ucPFClose(ucOpenID);
				return 1;
			}
			switch ( uikey )
			{
				case EM_key_UP:	//	��[2]�������ϲ�����������Ļ���Ϲ���һ��
					count --;
					if ( count < 0 )//��¼����
					{
						lcddisperr("���һ��");
						count ++;
					}
					break;

				case EM_key_DOWN:	//	��[8]�������²�����������Ļ���¹���һ��
					count ++;
					if ( count >= RecordCount )
					{
						lcddisperr("���һ��");
						count --;
					}
					break;
				case EM_key_ENTER:
					break;
				default:
					break;
			}

			if ( (uikey==EM_key_UP) || (uikey==EM_key_DOWN)  )
			{
				if(EA_ucPFReadRec(ucOpenID, count, 1, &RecordCount, rec) != EM_SUCCESS)
				{
					(void)EA_ucPFClose(ucOpenID);
					lcddisperr((void *)"��ȡ������");
					return 0;
				}
				AnalyAndDisplayNote(rec);
			}

			if(uikey == EM_key_ENTER)
			{
				print_work_note_his(rec);
				(void)EA_ucPFClose(ucOpenID);
				return 1;
			}			
		}
	}
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
void print_work_note_his (INT8U * rec)
{
	uchar       ucRet;
	INT8U		RecBuff[70];
	INT8U		strbuff[50];
	INT8U		time[7+2];


	_BANLANCE_IN_HAND * prec = (_BANLANCE_IN_HAND *)rec;

	EA_vCls();
	
	EA_vDisplay(1, "    �ս��ʵ���ӡ    ");//(int qx, int qy, char * ceStr, int xyFlag)
	EA_vDisplay(2, "       ��ȴ�...    ");//(int qx, int qy, char * ceStr, int xyFlag)
	SleepMs(1500);

	memset(RecBuff, 0, sizeof(RecBuff));
///��ӡͷ
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char *)"�ս��ʵ���ʷ��¼\r\n");
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char *)"------------------------------\r\n");
//���Ѵ���
	sprintf((void *)strbuff, "���ʹ�����%6d��\r\n", prec->consume_cnt);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char *)strbuff);

//���ѽ��
	sprintf((void *)strbuff, "���������%6.2fԪ\r\n", (float)(prec->consume_amt)/100);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char *)strbuff);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char *)"------------------------------\r\n");
//�ϰ�ʱ��	
	memcpy(time, (INT8U*)&prec->work_datetime, 7);
	htoa(RecBuff, time, 7);
	sprintf((void *)strbuff, "�ϰ�ʱ�䣺%s\r\n", RecBuff);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char *)strbuff);
	
//�°�ʱ��
	memcpy(time, (INT8U*)&prec->down_datetime,7);
	htoa(RecBuff, time, 7);

	sprintf((void *)strbuff, "�°�ʱ�䣺%s\r\n",RecBuff);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char *)strbuff);

//���з���
	strcpy((void *)strbuff, "---------------------------------- \r\r\r");
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char *)strbuff);
	
//�豸���
	htoa(RecBuff+4, &prec->equ_id[0], 3);
	sprintf((void *)strbuff, "�豸��ţ�%s\r\n", RecBuff);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char *)strbuff);
//�̻���
	htoa(RecBuff, &prec->acnt_id[0], 2);
	sprintf((void *)strbuff, "վ��ţ�%s\r\n", RecBuff);
	(void)EA_ucLinePrinter(hDprinterHandle, 0, (char *)strbuff);
//����Ա��
	htoa(RecBuff, &prec->oper_id[0], 2);
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
//	float   fBuf = 0;
//	INT8U   LOCAL_V_TYPE  temp_buf[100];
	_BANLANCE_IN_HAND * prec = (_BANLANCE_IN_HAND *)strTradeInfo;

	EA_vCls();


	sprintf(strbuff, "���ʹ���:%7d��", prec->consume_cnt);
	EA_vDisplay(1, strbuff);

//���ѽ��
	sprintf(strbuff, "�������:%6.2fԪ", (float)(prec->consume_amt)/100);
	EA_vDisplay(2, strbuff);

//�ϰ�ʱ��	
	memcpy(time, (INT8U*)&prec->work_datetime,7);
	htoa(temp, time, 7);
	sprintf(strbuff, "�ϰࣺ%s", temp);
	EA_vDisplay(3, strbuff);
	
//�°�ʱ��
	memcpy(time, (INT8U*)&prec->down_datetime,7);
	htoa(temp, time, 7);
	sprintf(strbuff, "�°ࣺ%s",temp);
	EA_vDisplay(4, strbuff);
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
	//lcddisperr("��ʱ��ʾ");
//  GPRS_SEND_Receive((void *)&"0000", 4);
	int ret = 0;
//  if()
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
	BUS_TIME  ltime;
	INT8U buf[22];

	EA_vCls();
	EA_ucSetInverse(EM_lcd_INVOFF);
	EA_vDisp(1, 1, " �Ͳ��й���������˾ ");
	EA_vDisp(2, 1, "    �ֳ�POS���ͻ�   ");

	Get_Time(&ltime);
	sprintf((void *)buf, "  %02X%02X-%02X-%02X %02X:%02X",
			ltime.century, ltime.year, ltime.month, ltime.day, ltime.hour, ltime.minute);
	EA_vDisplay(3, (void *)buf);

	if ( DevStat.record_number >= MAX_REC_NUM )
	{
		EA_vDisp(3, 1, "  ��¼����, ���ϴ�  ");
	}

	EA_vDisp(4, 1,     "F1���� F2�ϴ� F3��½");
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
INT8U SystemCheck()
{
	INT8U   ret = 0;
	//SAM�����
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

	EA_vCls();
	EA_ucSetInverse(EM_lcd_INVOFF);
	EA_vDisp(1, 1, "    �汾��: ");
	EA_vDisp(1, 13, VERSION);
	SleepMs(2000);
	EA_vCls();
	EA_vDisp(2, 1, "   ϵͳ��ʼ����...  ");
	key = EA_uiInkeyMs(2000);

	if ( key == EM_key_F3 )
	{
		format_process();    		  //���Խ���ϵͳ��ʽ������
	}
	//��Ҫ�������ֳ�ʼ������
	appfile_Init();

	memset((void *)&DevStat, 0x00, sizeof(DevStat));

	i = ReadParam();  
	        
	if ( i != ok )
	{
		param_factory_default(0);     //��ʽ����Ҫ����DEV.STATE
	}

	DevStat.POS_type = POS_T_NORMAL;  //������Ϊ�����ɼ�


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
	//����Ա��½
	zbClerkFileInit();
	//���°�����
//  download_whiltelist();

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

//  if (ucRet!=EM_SUCCESS)
//  {
//  	lcddisperr((void *)"Ա��������ʼ��ʧ��");
//  	return notok;
//  }
	
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
//  if ( EA_ucOpenDevice("USBD", EM_io_REALMODE, &hUSBDHandle) != EM_SUCCESS )
//  {
//  	lcddisperr("��ô�ӡ�������ʧ��!");
//  	return notok;
//  }
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
		//  	(void)EA_ucCloseDevice(&hDprinterHandle);
		lcddisperr("��ӡ������ʧ��!");
		return notok;
	}
	ucRet =  EA_ucSetPrinterFormat(hDprinterHandle, EM_prn_YSPACE, 6);
	if ( ucRet != EM_SUCCESS )
	{
		//  	(void)EA_ucCloseDevice(&hDprinterHandle);
		lcddisperr("��ӡ������ʧ��!");
		return notok;
	}

	//  ucRet = EA_ucSetPrinterFormat(hDprinterHandle, EM_prn_PAGESPACE, 578);
	ucRet = EA_ucSetPrinterFormat(hDprinterHandle, EM_prn_PAGESPACE , 220);
	if ( ucRet != EM_SUCCESS )
	{
		//  	(void)EA_ucCloseDevice(&hDprinterHandle);
		lcddisperr("��ӡ������ʧ��!");
		return notok;
	}

	ucRet = EA_ucSetPrinterFormat(hDprinterHandle, EM_prn_ASCII_DOTTYPE, EM_prn_ASCIIDOT24X12);
	if ( ucRet != EM_SUCCESS )
	{
		//  	(void)EA_ucCloseDevice(&hDprinterHandle);
		lcddisperr("��ӡ������ʧ��!");
		return notok;
	}
	ucRet = EA_ucSetPrinterFormat(hDprinterHandle, EM_prn_ASCII_PRNTYPE, EM_prn_ASCIIPRN1X1);
	if ( ucRet != EM_SUCCESS )
	{
		//  	(void)EA_ucCloseDevice(&hDprinterHandle);
		lcddisperr("��ӡ������ʧ��!");
		return notok;
	}
	ucRet = EA_ucSetPrinterFormat(hDprinterHandle, EM_prn_HZ_DOTTYPE, EM_prn_HZDOT24X24);
	if ( ucRet != EM_SUCCESS )
	{
		//  	(void)EA_ucCloseDevice(&hDprinterHandle);
		lcddisperr("��ӡ������ʧ��!");
		return notok;
	}

	ucRet = EA_ucSetPrinterFormat(hDprinterHandle, EM_prn_HZ_PRNTYPE, EM_prn_HZPRN1X1);
	if ( ucRet != EM_SUCCESS )
	{
		//  	(void)EA_ucCloseDevice(&hDprinterHandle);
		lcddisperr("��ӡ������ʧ��!");
		return notok;
	}	
	ucRet = EA_ucSetPrinterFormat(hDprinterHandle, EM_prn_PRNGRAY, 4);
	if ( ucRet != EM_SUCCESS )
	{
		//  	(void)EA_ucCloseDevice(&hDprinterHandle);
		lcddisperr("��ӡ������ʧ��!");
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
		lcddisperr("COM������ʧ��!");
		return notok;
	}
	EA_ucInitRS232(hRS232Handle, EM_232_115200BPS, EM_232_NOPAR, EM_232_8DBS);

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

	for (;;)
	{
		EA_vCls();
		Get_Time(&ltime);
		EA_vDisp(1, 1, "��ǰʱ��:");
		sprintf((void *)buf, "%02X%02X-%02X-%02X-%02X:%02X:%02X", ltime.century, ltime.year, ltime.month,
				ltime.day, ltime.hour, ltime.minute, ltime.second);
		EA_vDisp(2, 1, (void *)buf);
		EA_vDisp(3, 1, "�������µ�ʱ��:");
		(void)EA_ucClrKeyBuf();
		strcpy((void *)input, "");

		i = EA_ucGetInputStr(4, 1, 20, EM_BIG_FONT | EM_MODE_NUMBER | EM_ALIGN_LEFT | EM_SHOW_ORIGINALLY
							 , 14, 14, 0, (void *)input);
		//  	sprintf(dbuf, "%s", input);
		//  	EA_vDisp(3, 1, dbuf);
		//  	sprintf(dbuf, "result:%02X", i);
		//  	EA_vDisp(4, 1, dbuf);
		if ( i == EM_ABOLISH )
			return;
		if ( i != EM_SUCCESS )
			continue;
		ltime.century = (ascii_to_hex(input[0]) << 4) | ascii_to_hex(input[1]);
		ltime.year = (ascii_to_hex(input[2]) << 4) | ascii_to_hex(input[3]);
		ltime.month = (ascii_to_hex(input[4]) << 4) | ascii_to_hex(input[5]);
		ltime.day = (ascii_to_hex(input[6]) << 4) | ascii_to_hex(input[7]);
		ltime.hour = (ascii_to_hex(input[8]) << 4) | ascii_to_hex(input[9]);
		ltime.minute = (ascii_to_hex(input[10]) << 4) | ascii_to_hex(input[11]);
		ltime.second = (ascii_to_hex(input[12]) << 4) | ascii_to_hex(input[13]);

		i = CheckTimeFormat(&ltime);
		if ( i != ok )
		{
			EA_vCls();
			EA_vDisp(4, 1, "ʱ���ʽ����");
			SleepMs(1500);
			//  		EA_vCls();
			continue;
		}

		Modify_Time(&ltime);
		EA_vCls();
		EA_vDisp(4, 1, "ʱ���޸ĳɹ�");
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
	INT8U input[50];
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
	Close_GPRS_Module();
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
	Close_GPRS_Module();
//  EA_vSetSaveResTime(uisleep_delay);
	EA_vSystemManage (EM_RESET);
	
	//(void)EA_uiInkey(0);
}