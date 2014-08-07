/******************************************************************************
Copyright 2005 �����״�ͨ�������޹�˾
All rights riserved.

�ļ���	��ydt.c
ģ�����ƣ�
���ܸ�Ҫ��

ȡ���汾��0.0.1
�޸���	������
������ڣ�2005.04.06
����˵����create

******************************************************************************/
#include "main.h"

extern DEV_STAT DevStat;
extern CARD_INFO CardInfo;
//extern char dbuf[];

INT32U uiblackLen = sizeof(BlackCardInfo);
INT32U uiparamclen = ( &DevStat.end - &DevStat.start );
INT32U uicurrecLen = sizeof(RECORD);
//INT32U uiconrecLen = sizeof(CONSUMERECORD);
INT32U uihisrecLen = sizeof(RECORD);
//INT32U uigrayrecLen = sizeof(GRAYRECORD);
INT32U clerkLen = sizeof(CLERK_STRUCT);


extern CLERK_STRUCT clerk_list[MAX_CLERK_NUM];


const char parmFileName[] = "param";
const char blackFileName[] = "black";
const char currecFileName[] = "currec";
const char conrecFileName[] = "conrec";
const char hisrecFileName[] = "hisrec";
const char grayrecFileName[] = "grayrec";
const char clerkFileName[] = "clerk";
/*****************************************************************
����ԭ�ͣ�Get_Time
������������ȡʱ�䣬��ȡ���Σ�Ϊ�˷�ֹż��һ�β��ɹ�
����������
�������ƣ�	����/�����	����		����
*time_data	���   _TIME	7���ֽڳ������ݴ�ͷ��β˳�������� �� �� �� ʱ �� ��
��  ��  ֵ��ok(0)-�ɹ� 
		notok(0xFF)��ʧ��
��      �ߣ�����
��      �ڣ�2005-04-12
�޸���ʷ��
����		�޸���		�޸�����
------		---------	-------------
*****************************************************************/
INT8U Get_Time(BUS_TIME *time_data)
{
	ET_DATETIME time1;

	(void)EA_ucGetDateTime(&time1);

	time_data->century = 0x20;
	time_data->year = hextobcd((INT8U)(time1.usYear % 2000));
	time_data->month = hextobcd(time1.ucMonth);
	time_data->day = hextobcd(time1.ucDay);
	time_data->hour = hextobcd(time1.ucHour);
	time_data->minute = hextobcd(time1.ucMinute);
	time_data->second = hextobcd(time1.ucSecond);

	CardInfo.card_begin_dt[0] = 0x20;
	CardInfo.card_begin_dt[1] = time_data->year;
	CardInfo.card_begin_dt[2] = time_data->month;
	CardInfo.card_begin_dt[3] = time_data->day;

	CardInfo.card_useful_dt[0] = 0x20;
	CardInfo.card_useful_dt[1] = (time_data->year + 50)%100;
	CardInfo.card_useful_dt[2] = time_data->month;
	CardInfo.card_useful_dt[3] = time_data->day;
	
	CardInfo.card_add_dt[0] = time_data->year;
	CardInfo.card_add_dt[1] = time_data->month;
	CardInfo.card_add_dt[2] = time_data->day;
	CardInfo.card_add_dt[3] = time_data->hour;
	return ok;
}

/*****************************************************************
����ԭ�ͣ�Modify_Time
�����������޸�ʱ�䣬�޸����Σ�Ϊ�˷�ֹż��һ�β��ɹ�
����������
�������ƣ�	����/�����	����		����
*time_data	����	_TIME	7���ֽڳ������ݴ�ͷ��β˳�������� �� �� �� ʱ �� ��
��  ��  ֵ��ok(0)-�ɹ� 
		notok(0xFF)��ʧ��
��      ��	������
��      �ڣ�2005-04-12
�޸���ʷ��
����		�޸���		�޸�����
------		---------	-------------
*****************************************************************/
INT8U Modify_Time(BUS_TIME const * const time_data)
{
	ET_DATETIME time1;

	time1.usYear = 2000 + (INT16U)bcdtohex(time_data->year);
	time1.ucMonth = bcdtohex(time_data->month);
	time1.ucDay = bcdtohex(time_data->day);
	time1.ucHour = bcdtohex(time_data->hour);
	time1.ucMinute = bcdtohex(time_data->minute);
	time1.ucSecond = bcdtohex(time_data->second);

	EA_ucSetDateTime(&time1, EM_SYSTEM_DATE | EM_SYSTEM_TIME);

	return ok;
}

uchar pf_exist(uchar *pucFileName, uchar *pucFileID)
{
	uchar ucret;
	ucret = EA_ucPFOpen((uchar *)pucFileName, (uchar *)pucFileID);
	if(ucret == EM_ffs_NOTEXIST)
		return FALSE;
	else if(ucret == EM_SUCCESS)
	{
		EA_ucPFClose((uchar)*pucFileID);
		return TRUE;
	}
	return FALSE;
}

uchar pf_open(uchar *pucFileName, uchar *pucFileID)
{
	uchar ucret;
	ucret = EA_ucPFOpen((uchar *)pucFileName, (uchar *)pucFileID);
	return ucret;
}

uchar pf_creat(uchar *pucFileName, uint uiRecLen)
{
	uchar ucret;
	ucret = EA_ucPFCreate((uchar *)pucFileName, uiRecLen);
	return ucret;
}

uchar pf_read(uchar ucFileID, uint uiStartRec, uint uiRecCntR, uint *puiRecCntW, uchar *pucString)
{
	return(EA_ucPFReadRec(ucFileID, uiStartRec, uiRecCntR, (uint *)puiRecCntW, (uchar *)pucString));
}

uchar pf_close(uchar ucFileID)
{
	return( EA_ucPFClose((uchar)ucFileID) );
}

uchar pf_write(uchar ucFileID, uint uiRecID, uchar *pucString)
{
	return( EA_ucPFWriteRec((uchar)ucFileID, (uint)uiRecID, (uchar *)pucString) );
}
/*****************************************************************
����ԭ�ͣ�CheckTimeFormat
�������������ʱ���ʽ�Ƿ�Ϸ�
����������
�������ƣ�	����/�����	����		����
time		����  			const _TIME *	7���ֽڳ������ݴ�ͷ��β˳�������� �� �� �� ʱ �� ��

��  ��  ֵ��ok(0)-�ɹ� 
		notok(0xFF)��ʧ��
��      ��	������
��      �ڣ�2003-05-12
�޸���ʷ��
����		�޸���		�޸�����
------		---------	-------------
*****************************************************************/
INT8U CheckTimeFormat(const BUS_TIME *time)
{
	INT8U temp;

	temp = time->month;
	if ( temp >= 0x0a )       //�·�Ӧ��1��12֮��
	{
		if ( temp >= 0x10 && temp <= 0x12 )
		{
			;
		}
		else
		{
			return notok;
		}
	}

	temp = time->day;
	if ( temp > 0x31 )                   //��Ӧ��1-31֮��
		return notok;
	temp = temp & 0x0F;
	if ( temp > 0x09 )
		return notok;

	temp = time->hour;
	if ( temp > 0x23 )                                   //СʱӦ��0-23֮��
		return notok;
	temp = temp & 0x0F;
	if ( temp > 0x09 )
		return notok;

	temp = time->minute;
	if ( temp > 0x59 )                                   //����Ӧ��0-59֮��
		return notok;
	temp = temp & 0x0F;
	if ( temp > 0x09 )
		return notok;

	temp = time->second;
	if ( temp > 0x59 )                                    //��Ӧ��0-59֮��
		return notok;
	temp = temp & 0x0F;
	if ( temp > 0x09 )
		return notok;

	return ok;
}

/******************************************************************************
 �������ƣ�ascii_to_hex
 ������������һ��0-9��ascii���룬ת��Ϊ16������
 �������ƣ� ����/����� ����        ����
 num        ����        INT8U           Ҫת��Ϊascii���������Ҫ��Χ0-9 a-f A-F

 ��  ��  ֵ��ת�����16������

 ��      �� ������
 ��      �ڣ�2004-09-02
 �޸���ʷ��
	����        �޸���      �޸�����
	------      ---------   -------------
******************************************************************************/
INT8U ascii_to_hex(INT8U num)
{
	//  ToggleWD();

	// 	return (num - 0x30);
	if ( num <= '9' )
		return ( num - 0x30 );
	else if ( num == 'a' || num == 'A' )
		return 0x0A;
	else if ( num == 'b' || num == 'B' )
		return 0x0B;
	else if ( num == 'c' || num == 'C' )
		return 0x0C;
	else if ( num == 'd' || num == 'D' )
		return 0x0D;
	else if ( num == 'e' || num == 'E' )
		return 0x0E;
	else
		return 0x0F;
}

/*****************************************************************
����ԭ�ͣ�bcdtohex
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
INT8U bcdtohex(INT8U bcd)
{
	return ( (bcd >> 4) * 10 + (bcd & 0x0F) );
}

/******************************************************************************
 �������ƣ�hextobcd
 ������������һ��16��������ת��Ϊbcd��
 �������ƣ� ����/����� ����        ����
 num        ����        INT8U           Ҫת��Ϊbcd�����

 ��  ��  ֵ��ת�����bcd��

 ��      �� ������
 ��      �ڣ�2004-09-02
 �޸���ʷ��
	����        �޸���      �޸�����
	------      ---------   -------------
******************************************************************************/
INT8U hextobcd(INT8U num)
{
	return ( ((num / 10) << 4) | (num % 10) );
}

/******************************************************************************
 �������ƣ�atoh
 ����������asciiת��Ϊhex��
 ����������
 �������ƣ� ����/����� ����		����
		
 ��  ��  ֵ����
		   
 �� 	 �� ������
 �� 	 �ڣ�2005-04-07
 �޸���ʷ��
	����		�޸���		�޸�����
	------		---------	-------------
******************************************************************************/
void atoh(void *des, INT8U *sour, int ascii_len)
{
	int i = 0;
	char *dest = (char *)des;

	//  ToggleWD();

	if ( ascii_len <= 0 )
		return;

	for ( i = 0; i < ascii_len; i++ )
	{
		if ( i % 2 == 0 )
			continue;

		dest[i / 2] = (ascii_to_hex(sour[i - 1]) << 4) | ascii_to_hex(sour[i]);
	}
}

/******************************************************************************
 �������ƣ�ascii_comm
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
INT8U ascii_comm(INT8U num)
{
	return ( num + 0x30 );
}

/******************************************************************************
 �������ƣ�SleepMs
 ������������ʱ������11.0592M*4����ר�ã���mem����
 �������ƣ�	����/�����	����		����
 _ms	����	INT16U				Ҫ��ʱ�ı��� 1ms*?
 
 ��  ��  ֵ����
 
 ��      �ߣ�����
 ��      �ڣ�2004-09-02
 �޸���ʷ��
	����		�޸���		�޸�����
	------		---------	-------------
******************************************************************************/
void SleepMs(INT16U _ms)
{
	EA_uiInkeyMs(_ms);
	EA_ucClrKeyBuf();
	return;
}

/******************************************************************************
 �������ƣ�lcddisp
 ������������ʾ����
 �������ƣ�	����/�����	����		����
 line		����		INT16U		�� 1~4
 col		����		INT16U		�� 1~20
 str		����		INT8U *		Ҫ��ʾ������
 
 ��  ��  ֵ����
 
 ��      �ߣ�����
 ��      �ڣ�2012-10-29
 �޸���ʷ��
	����		�޸���		�޸�����
	------		---------	-------------
******************************************************************************/
void lcddisp(INT16U line, INT16U col, const INT8U *str)
{
	EA_vDisp((int)line, (int)col, (char *)str);
}

/******************************************************************************
 �������ƣ�browse_menu
 �����������˵�����
 �������ƣ�	����/�����	����		����
 startline	����		INT8U		�˵���ʼ��ʾ�� 1~4
 title		����		INT8U *		�˵���ʾ, ������8������, �ڵ�һ����ʾ
 p_menu		����		INT8U *		�˵���ÿ��10�����֣��������ո�  
 allow_exit	����		INT8U		�Ƿ�����ȡ�����˳�TRUE=���� FALSE=������ 
 
 ��  ��  ֵ��ѡ��Ĳ˵���  0Ϊ��һ��
		-1 = ȡ���� 
 
 ��      �ߣ�����
 ��      �ڣ�2012-10-29
 �޸���ʷ��
	����		�޸���		�޸�����
	------		---------	-------------
******************************************************************************/
INT8S browse_menu(INT8U startline, INT8U *title, INT8U *p_menu, INT8U allow_exit)
{
	INT16U len = 0;
	INT16U menu_lines = 0;
	INT16U i = 0;
#define MAX_MENU_LINES 100
	INT8U menu_text[MAX_MENU_LINES][22];
	//  INT16U j = 0;
	INT16U key = EM_key_NOHIT;
	struct _MENU_PARAM
	{
		INT16S disp_pos;
		INT16S menu_pos;
	}lmenu;
	INT16S h = 0;
	INT16U disp_pos_max = 0;
	INT16S m = 0;
	//  INT8S buf[30];


	startline = startline;              //avoid warning

	EA_vCls();
	EA_ucSetInverse(EM_lcd_INVOFF);

	//��ʾtitle
	EA_vTextOut(0, 0, EM_key_FONT8X16, 0, 1, 1, (char *)title);

	//�˵����ֳ�ʼ��
	len = strlen((char *)p_menu);
	menu_lines = len / 20;
	if ( menu_lines > MAX_MENU_LINES )
		menu_lines = MAX_MENU_LINES;
	//  if ( menu_lines <= 1 )
	//  	return 0;
	if ( menu_lines == 0 )
		return -1;
	memset((void *)&menu_text[0][0], 0x00, sizeof(menu_text));
	for ( i = 0; i < menu_lines; i++ )
	{
		memcpy((void *)&menu_text[i][0], (void *)&p_menu[i * 20], 20);
	}


	if ( menu_lines < 4 )
		disp_pos_max = menu_lines - 1;
	else
		disp_pos_max = 3;

	lmenu.disp_pos = 0;
	lmenu.menu_pos = 0;

	EA_vCls();   //�����Ļ
	EA_ucSetInverse(EM_lcd_INVON);
	lcddisp(lmenu.disp_pos + 1, 1, &menu_text[lmenu.menu_pos][0]);       //��ʾ��ǰ�˵�������
	EA_ucSetInverse(EM_lcd_INVOFF);

	m = 1;
	for ( i = lmenu.disp_pos + 1; i <= 3; i++ )
	{
		lcddisp(i + 1, 1, &menu_text[lmenu.menu_pos + m][0]);
		m++;
	}
	m = 1;
	for ( h = lmenu.disp_pos - 1; h >= 0; h-- )
	{
		lcddisp(h + 1, 1, &menu_text[lmenu.menu_pos - m][0]);
		m++;
	}

	for (;;)
	{
		key = EA_uiInkey(0);
		switch ( key )
		{
		   case EM_key_INTERPUNCTION:           //��
			   lmenu.disp_pos--;
			   if ( lmenu.disp_pos < 0 )
				   lmenu.disp_pos = 0;
			   lmenu.menu_pos--;
			   if ( lmenu.menu_pos < 0 )
				   lmenu.menu_pos = 0;


			   EA_vCls();   //�����Ļ
			   EA_ucSetInverse(EM_lcd_INVON);
			   lcddisp(lmenu.disp_pos + 1, 1, &menu_text[lmenu.menu_pos][0]);       //��ʾ��ǰ�˵�������
			   EA_ucSetInverse(EM_lcd_INVOFF);

			   m = 1;
			   for ( i = lmenu.disp_pos + 1; i <= 3; i++ )
			   {
				   lcddisp(i + 1, 1, &menu_text[lmenu.menu_pos + m][0]);
				   m++;
			   }
			   m = 1;
			   for ( h = lmenu.disp_pos - 1; h >= 0; h-- )
			   {
				   lcddisp(h + 1, 1, &menu_text[lmenu.menu_pos - m][0]);
				   m++;
			   }

			   break;

		   case EM_key_LETTER:                  //��
			   lmenu.disp_pos++;
			   if ( lmenu.disp_pos > disp_pos_max )
				   lmenu.disp_pos = disp_pos_max;
			   lmenu.menu_pos++;
			   if ( lmenu.menu_pos > menu_lines - 1 )
				   lmenu.menu_pos = menu_lines - 1;


			   EA_vCls();   //�����Ļ
			   EA_ucSetInverse(EM_lcd_INVON);
			   lcddisp(lmenu.disp_pos + 1, 1, &menu_text[lmenu.menu_pos][0]);       //��ʾ��ǰ�˵�������
			   EA_ucSetInverse(EM_lcd_INVOFF);

			   m = 1;
			   for ( i = lmenu.disp_pos + 1; i <= 3; i++ )
			   {
				   lcddisp(i + 1, 1, &menu_text[lmenu.menu_pos + m][0]);
				   m++;
			   }
			   m = 1;
			   for ( h = lmenu.disp_pos - 1; h >= 0; h-- )
			   {
				   lcddisp(h + 1, 1, &menu_text[lmenu.menu_pos - m][0]);
				   m++;
			   }

			   break;

		   case EM_key_ENTER:                   //ȷ��
			   return lmenu.menu_pos;

		   case EM_key_1:
			   if ( menu_lines >= 1 )
				   return 0;
			   else
				   break;

		   case EM_key_2:
			   if ( menu_lines >= 2 )
				   return 1;
			   else
				   break;

		   case EM_key_3:
			   if ( menu_lines >= 3 )
				   return 2;
			   else
				   break;

		   case EM_key_4:
			   if ( menu_lines >= 4 )
				   return 3;
			   else
				   break;

		   case EM_key_5:
			   if ( menu_lines >= 5 )
				   return 4;
			   else
				   break;

		   case EM_key_6:
			   if ( menu_lines >= 6 )
				   return 5;
			   else
				   break;

		   case EM_key_7:
			   if ( menu_lines >= 7 )
				   return 6;
			   else
				   break;

		   case EM_key_8:
			   if ( menu_lines >= 8 )
				   return 7;
			   else
				   break;

		   case EM_key_9:
			   if ( menu_lines >= 9 )
				   return 8;
			   else
				   break;


		   case EM_key_CANCEL:
			   if ( allow_exit == TRUE )
				   return -1;
			   else
				   break;

		   default:
			   break;
		}

	}
	return 0;
}


/******************************************************************************
 �������ƣ�lcddisperr
 ����������Һ������ʾ����
 �������ƣ�	����/�����	����		����
 str		����		char *		Ҫ��ʾ������
 
 ��  ��  ֵ����
 
 ��      �ߣ�����
 ��      �ڣ�2012-10-29
 �޸���ʷ��
	����		�޸���		�޸�����
	------		---------	-------------
******************************************************************************/
void lcddisperr(const char *str)
{
	INT16U key = EM_key_NOHIT;
	uchar  len=0;

	len = strlen((void *)str);
	if( len <= 20 )
	{
		EA_vCls();
		EA_vDisp(3, (20 -len)/2 + 1, (char *)str);
	}
	else if( len > 20 )
	{
		EA_vCls();
		EA_vDisp(3, 1, (char *)str);
	}

	key = EA_uiInkeyMs(1200);
//  (void)EA_uiInkeyMs(500);
	EA_vDisp(3, 1, "                      ");
}

/*****************************************************************
 ����ԭ�ͣ�param_factory_default()
 ���������������ָ���������
 ����������	
 �������ƣ�	����/�����	����		����
 level		����		INT8U		��ʼ���ĵȼ�0-����ʼ����¼�Ͳ����ļ� 1-��¼�Ͳ����ļ�����ʼ��
				 
 ����ֵ��	��
 
 ��  �ߣ�	����
 ��  �ڣ�	2004-09-17
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
*****************************************************************/
void param_factory_default(INT8U level)
{
	//����5����¼�ļ������������������������׼�¼��
	//��ʷ��¼��Ա������
	INT8U i = 0;
	//���²�����
	 i = EA_ucPFCreate((uchar *)parmFileName, uiparamclen);  
	 if ( i == EM_ffs_FILEEXIST )          //�ļ��Ѿ�����
	 {
		 if ( EA_ucPFDel((uchar *)parmFileName) != EM_ffs_SUCCESS )
		 {
			 lcddisperr("ɾ��������ʧ��");
		 }
		 if ( EA_ucPFCreate((uchar *)parmFileName, uiparamclen) == EM_ffs_SUCCESS )
		 {
			 lcddisperr("���´���������");
		 }
	 }
	 else if ( i == EM_ffs_FILEFULL )
	 {
		 lcddisperr("��������������");
	 }
	 else if ( i == EM_ffs_SUCCESS )
	 {
		 lcddisperr("����������ɹ�");
	 }
	 else
	 {
		 lcddisperr("����������ʧ��");
	 }
//
//   //Ա����½�ļ�
//   i = EA_ucPFCreate((uchar *)clerkFileName, sizeof(clerk_list));
//   if ( i == EM_ffs_FILEEXIST )          //�ļ��Ѿ�����
//   {
//  	 if ( EA_ucPFDel((uchar *)clerkFileName) != EM_ffs_SUCCESS )
//  	 {
//  		 lcddisperr("ɾ��Ա����ʧ��");
//  	 }
//  	 if ( EA_ucPFCreate((uchar *)clerkFileName, sizeof(clerk_list)) == EM_ffs_SUCCESS )
//  	 {
//  		 lcddisperr("���´���Ա����");
//  	 }
//   }
//   else if ( i == EM_ffs_FILEFULL )
//   {
//  	 lcddisperr("����Ա��������");
//   }
//   else if ( i == EM_ffs_SUCCESS )
//   {
//  	 lcddisperr("����Ա����ɹ�");
//   }
//   else
//   {
//  	 lcddisperr("����Ա����ʧ��");
//   }
//
//  for (i=0; i<MAX_CLERK_NUM; i++)
//  {
//  	clerk_list[i].id[0] = '0';
//  	clerk_list[i].id[1] = '0'+i;
//  	clerk_list[i].right = ID_OPERATOR;
//  	memset(clerk_list[i].pw,'0'+i,4);
//  }
//
//  zbClerkSaveFile();

	memset((void *)&DevStat, 0x00, sizeof(DevStat));
	strcpy((void *)&DevStat.user_passwd[0], "000000");
 	
	DevStat.driver_is_working = FALSE;
	memset(&DevStat.dev_trade_no[0], 0, 2);
	DevStat.mode = 0x00;
	DevStat.samcompcode = 1;
	DevStat.Gprs_On_Flag = 0;
	DevStat.effect_time.hour = 0x07;
	memset(&DevStat.binhand, 0x00, sizeof(DevStat.binhand));

	WriteParam();

	if ( level == 0 )
	{
		return;
	}
	else if ( level == 1 )
	{
		lcddisperr("���̸�ʽ����...");
		i = EA_ucPFFormat();       //���̸�ʽ��
		if( i != EM_ffs_SUCCESS)
		{
			lcddisperr("���̸�ʽ��ʧ��");
		}
		//���´����������ļ�
		i = EA_ucBlkCreate((char *)blackFileName, uiblackLen, 2000);
		if ( i == EM_bfs_FILE_EXIST )          //�ļ��Ѿ�����
		{
			if (  EA_ucBlkDelete((char *)blackFileName) != EM_SUCCESS )
			{
				lcddisperr("ɾ���������ļ�ʧ��");
			}
			if (EA_ucBlkCreate((char *)blackFileName, uiblackLen, 2000) == EM_SUCCESS )
			{
				lcddisperr("���´���������");
			}
		}
		else if ( i == EM_bfs_NO_SPACE )
		{
			lcddisperr("��������������");
		}
		else if ( i == EM_ffs_SUCCESS )
		{
			lcddisperr("�����������ɹ�");
		}
		else
		{
			lcddisperr("����������ʧ��");
		}
		//���´�����¼�ļ�
		 i = EA_ucPFCreateLoop((uchar *)currecFileName, uicurrecLen, 2000);  //1000��ѭ����¼;
		 if ( i == EM_ffs_FILEEXIST )          //�ļ��Ѿ�����
		 {
			 if ( EA_ucPFDel((uchar *)currecFileName) != EM_ffs_SUCCESS )
			 {
				 lcddisperr("ɾ�����Ѽ�¼ʧ��");
			 }
			 if ( EA_ucPFCreateLoop((uchar *)currecFileName, uicurrecLen, 2000) == EM_ffs_SUCCESS )
			 {
				 lcddisperr("�ؽ����Ѽ�¼");
			 }
		 }
		 else if ( i == EM_ffs_FILEFULL )
		 {
			 lcddisperr("�������Ѽ�¼������");
		 }
		 else if ( i == EM_ffs_SUCCESS )
		 {
			 lcddisperr("�������Ѽ�¼�ɹ�");         
		 }
		 else
		 {
			 lcddisperr("�������Ѽ�¼ʧ��");
		 }
		 //���´�����ʷ��¼�ļ�
		 i = EA_ucPFCreateLoop((uchar *)hisrecFileName, uihisrecLen, 10000);
		 if ( i == EM_ffs_FILEEXIST )          //�ļ��Ѿ�����
		 {
			 if ( EA_ucPFDel((uchar *)hisrecFileName) != EM_ffs_SUCCESS )
			 {
				 lcddisperr("ɾ����ʷ��¼�ļ�ʧ��");
			 }
			 if ( EA_ucPFCreateLoop((uchar *)hisrecFileName, uihisrecLen, 10000) == EM_ffs_SUCCESS )
			 {
				 lcddisperr("�ؽ���ʷ��¼");
			 }
		 }
		 else if ( i == EM_ffs_FILEFULL )
		 {
			 lcddisperr("������ʷ��¼������");
		 }
		 else if ( i == EM_ffs_SUCCESS )
		 {
			 lcddisperr("������ʷ��¼�ɹ�");
		 }
		 else
		 {
			 lcddisperr("������ʷ��¼ʧ��");
		 }
		 //�����ս��ļ�
		 i = EA_ucPFCreateLoop((uchar *)conrecFileName, uicurrecLen, 800);  //1000��ѭ����¼;
		 if ( i == EM_ffs_FILEEXIST )          //�ļ��Ѿ�����
		 {
			 if ( EA_ucPFDel((uchar *)conrecFileName) != EM_ffs_SUCCESS )
			 {
				 lcddisperr("ɾ���ս��ļ�ʧ��");
			 }
			 if ( EA_ucPFCreateLoop((uchar *)conrecFileName, uicurrecLen, 800) == EM_ffs_SUCCESS )
			 {
				 lcddisperr("�ؽ��ս��ļ���¼");
			 }
		 }
		 else if ( i == EM_ffs_FILEFULL )
		 {
			 lcddisperr("�����ս�������");
		 }
		 else if ( i == EM_ffs_SUCCESS )
		 {
			 lcddisperr("�����ս�ɹ�");         
		 }
		 else
		 {
			 lcddisperr("�����ս�ʧ��");
		 }
	}

	return;
}

/******************************************************************************
 �������ƣ�WriteParam
 �����������������
 ����������
 �������ƣ� ����/����� ����        ����

 ��  ��  ֵ����

 ��      �� ������/������
 ��      �ڣ�2012-11-24
 �޸���ʷ��
		����        �޸���      �޸�����
		------      ---------   -------------
******************************************************************************/
void WriteParam(void)
{
	INT8U ucOpenID = 0;
	//�򿪲����ļ�
	if ( EA_ucPFOpen((uchar *)parmFileName, &ucOpenID) != EM_SUCCESS )
	{
		lcddisperr("�򿪲�����ʧ��");
		return;
	}	
	if ( EA_ucPFWriteRec(ucOpenID, 0, &DevStat.start) != EM_SUCCESS )
	{
		lcddisperr("��Ӳ�����ʧ��");
		return;
	}

	if ( EA_ucPFClose(ucOpenID) != EM_SUCCESS )
	{
		lcddisperr("�رղ�����ʧ��");
		return;
	}
	
}

/******************************************************************************
 �������ƣ�ReadParam
 ������������ȡ����
 ����������
 �������ƣ� ����/����� ����        ����

 ��  ��  ֵ��ok ? notok

 ��      �� ������/������
 ��      �ڣ�2012-11-24
 �޸���ʷ��
		����        �޸���      �޸�����
		------      ---------   -------------
******************************************************************************/
INT8U ReadParam(void)
{
	uchar ucOpenID = 0;
	uint  uiReadCnt = 0;
	uchar i;
	uint  uiRetNum;
	// �򿪲������ļ�
	if ( EA_ucPFOpen((INT8U *)parmFileName, &ucOpenID) != EM_SUCCESS )
	{
		(void)EA_ucPFClose(ucOpenID);
		return notok;
	}
	//��ȡ���ݣ����һ����Ч��������Ϣ
	i = EA_ucPFInfo(ucOpenID, EM_ffs_VALIDREC, &uiReadCnt);
	if ( i != EM_ffs_SUCCESS )
	{
		(void)EA_ucPFClose(ucOpenID);
		return notok;
	}
	if( uiReadCnt == 0)
	{
		lcddisperr("������Ϊ��");
		(void)EA_ucPFClose(ucOpenID);
		return notok;
	}
	if ( EA_ucPFReadRec(ucOpenID, uiReadCnt, 1, &uiRetNum, &DevStat.start ) != EM_SUCCESS )
	{
		(void)EA_ucPFClose(ucOpenID);
		return notok;
	}
	//�ر��ļ�
	(void)EA_ucPFClose(ucOpenID);
	return ok;
}

/*****************************************************************
 ����ԭ�ͣ�Cal_Crc
 ��������������һ�����ݵ�CRCֵ������CCITT�㷨
 ����������
 �������ƣ�	����/�����	����		����
 data_to_cal_crc	����	INT8U const *		Ҫ����CRC������������ʼָ��
 len				 		����	INT16U				Ҫ����CRC���������ĳ���

 ��  ��  ֵ��uint�͵�CRCֵ(2�ֽ�)
 ��      ��	������
 ��      �ڣ�2004-02-04
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
*****************************************************************/
INT16U Cal_Crc(INT8U const *data_to_cal_crc, INT16U len)
{
	INT16U crc;
	INT8U i;

	crc = 0xFFFF;           //CRC��ʼֵ

	while ( len-- != 0 )
	{
		crc = crc ^ ((INT16U) * data_to_cal_crc++ << 8);

		for ( i = 0; i < 8; ++i )
			if ( crc & 0x8000 )
				crc = (crc << 1) ^ 0x1021;
			else
				crc	= crc << 1;
	}

	return crc;
}


/******************************************************************************
 �������ƣ�browse_menu_time
 �����������˵�����������ʾʱ��ģ����˵�ר��
 �������ƣ�	����/�����	����		����
 startline	����		INT8U		�˵���ʼ��ʾ�� 1~4
 title		����		INT8U *		�˵���ʾ, ������8������, �ڵ�һ����ʾ
 p_menu		����		INT8U *		�˵���ÿ��10�����֣��������ո�  
 allow_exit	����		INT8U		�Ƿ�����ȡ�����˳�TRUE=���� FALSE=������ 
 
 ��  ��  ֵ��ѡ��Ĳ˵���  0Ϊ��һ��
		-1 = ȡ���� 
 
 ��      �ߣ�����
 ��      �ڣ�2012-10-29
 �޸���ʷ��
	����		�޸���		�޸�����
	------		---------	-------------
******************************************************************************/
INT8S browse_menu_time(INT8U startline, INT8U *title, INT8U *p_menu, INT8U allow_exit)
{
	INT16U len = 0;
	INT16U menu_lines = 0;
	INT16U i = 0;
#define MAX_MENU_LINES 100
	INT8U menu_text[MAX_MENU_LINES][22];
	//  INT16U j = 0;
	INT16U key = EM_key_NOHIT;
	struct _MENU_PARAM
	{
		INT16S disp_pos;
		INT16S menu_pos;
	}lmenu;
	INT16S h = 0;
	INT16U disp_pos_max = 0;
	INT16S m = 0;
	INT8S buf[30];
	BUS_TIME ltime;


	startline = startline;              //avoid warning

	EA_vCls();
	EA_ucSetInverse(EM_lcd_INVOFF);

	//  //��ʾtitle
	//  EA_vTextOut(0, 0, EM_key_FONT8X16, 0, 1, 1, (char *)title);
	//2012-11-16 14:25
	Get_Time(&ltime);
	sprintf((void *)buf, "%02X%02X-%02X-%02X %02X:%02X",
			ltime.century, ltime.year, ltime.month, ltime.day, ltime.hour, ltime.minute);
	EA_vTextOut(0, 0, EM_key_FONT8X16, 0, 1, 1, (char *)buf);

	//�˵����ֳ�ʼ��
	len = strlen((char *)p_menu);
	menu_lines = len / 20;
	if ( menu_lines > MAX_MENU_LINES )
		menu_lines = MAX_MENU_LINES;
	//  if ( menu_lines <= 1 )
	//  	return 0;
	if ( menu_lines == 0 )
		return -1;
	memset((void *)&menu_text[0][0], 0x00, sizeof(menu_text));
	for ( i = 0; i < menu_lines; i++ )
	{
		memcpy((void *)&menu_text[i][0], (void *)&p_menu[i * 20], 20);
	}


	if ( menu_lines < 4 )
		disp_pos_max = menu_lines - 1;
	else
		disp_pos_max = 3;
	//  EA_ucSetInverse(EM_lcd_INVON);
	//  lcddisp(1, 1, &menu_text[0][0]);
	//  EA_ucSetInverse(EM_lcd_INVOFF);
	//  for ( i = 1; i < disp_pos_max; i++ )
	//  {
	//  	lcddisp(i + 1, 1, &menu_text[i][0]);
	//  }

	lmenu.disp_pos = 0;
	lmenu.menu_pos = 0;

	EA_vCls();   //�����Ļ
	EA_ucSetInverse(EM_lcd_INVON);
	lcddisp(lmenu.disp_pos + 1, 1, &menu_text[lmenu.menu_pos][0]);       //��ʾ��ǰ�˵�������
	EA_ucSetInverse(EM_lcd_INVOFF);

	m = 1;
	for ( i = lmenu.disp_pos + 1; i <= 3; i++ )
	{
		lcddisp(i + 1, 1, &menu_text[lmenu.menu_pos + m][0]);
		m++;
	}
	m = 1;
	for ( h = lmenu.disp_pos - 1; h >= 0; h-- )
	{
		lcddisp(h + 1, 1, &menu_text[lmenu.menu_pos - m][0]);
		m++;
	}


	for (;;)
	{
		Get_Time(&ltime);
		sprintf((void *)buf, "%02X%02X-%02X-%02X %02X:%02X",
				ltime.century, ltime.year, ltime.month, ltime.day, ltime.hour, ltime.minute);
		EA_vTextOut(0, 0, EM_key_FONT8X16, 0, 1, 1, (char *)buf);

		key = EA_uiInkeyMs(2000);
		switch ( key )
		{
		   case EM_key_INTERPUNCTION:           //��
			   lmenu.disp_pos--;
			   if ( lmenu.disp_pos < 0 )
				   lmenu.disp_pos = 0;
			   lmenu.menu_pos--;
			   if ( lmenu.menu_pos < 0 )
				   lmenu.menu_pos = 0;


			   EA_vCls();   //�����Ļ
			   EA_ucSetInverse(EM_lcd_INVON);
			   lcddisp(lmenu.disp_pos + 1, 1, &menu_text[lmenu.menu_pos][0]);       //��ʾ��ǰ�˵�������
			   EA_ucSetInverse(EM_lcd_INVOFF);

			   m = 1;
			   for ( i = lmenu.disp_pos + 1; i <= 3; i++ )
			   {
				   lcddisp(i + 1, 1, &menu_text[lmenu.menu_pos + m][0]);
				   m++;
			   }
			   m = 1;
			   for ( h = lmenu.disp_pos - 1; h >= 0; h-- )
			   {
				   lcddisp(h + 1, 1, &menu_text[lmenu.menu_pos - m][0]);
				   m++;
			   }

			   break;

		   case EM_key_LETTER:                  //��
			   lmenu.disp_pos++;
			   if ( lmenu.disp_pos > disp_pos_max )
				   lmenu.disp_pos = disp_pos_max;
			   lmenu.menu_pos++;
			   if ( lmenu.menu_pos > menu_lines - 1 )
				   lmenu.menu_pos = menu_lines - 1;


			   EA_vCls();   //�����Ļ
			   EA_ucSetInverse(EM_lcd_INVON);
			   lcddisp(lmenu.disp_pos + 1, 1, &menu_text[lmenu.menu_pos][0]);       //��ʾ��ǰ�˵�������
			   EA_ucSetInverse(EM_lcd_INVOFF);

			   m = 1;
			   for ( i = lmenu.disp_pos + 1; i <= 3; i++ )
			   {
				   lcddisp(i + 1, 1, &menu_text[lmenu.menu_pos + m][0]);
				   m++;
			   }
			   m = 1;
			   for ( h = lmenu.disp_pos - 1; h >= 0; h-- )
			   {
				   lcddisp(h + 1, 1, &menu_text[lmenu.menu_pos - m][0]);
				   m++;
			   }
			   //  		   for ( h = lmenu.disp_pos - 1; h >= 0; h-- )
			   //  		   {
			   //  			   lcddisp(h + 1, 1, &menu_text[lmenu.menu_pos - (lmenu.disp_pos - h)][0]);
			   //  		   }


			   break;

		   case EM_key_ENTER:                   //ȷ��
			   return lmenu.menu_pos;
			   //  		   //debug
			   //  		   sprintf((void *)buf, "%d", lmenu.menu_pos + 1);
			   //  		   EA_vTextOut(0, 0, EM_key_FONT8X16, 0, 1, 1, (void *)buf);
			   //  		   break;
			   //  		   //debug end


		   case EM_key_1:
			   if ( menu_lines >= 1 )
				   return 0;
			   else
				   break;

		   case EM_key_2:
			   if ( menu_lines >= 2 )
				   return 1;
			   else
				   break;

		   case EM_key_3:
			   if ( menu_lines >= 3 )
				   return 2;
			   else
				   break;

		   case EM_key_4:
			   if ( menu_lines >= 4 )
				   return 3;
			   else
				   break;

		   case EM_key_5:
			   if ( menu_lines >= 5 )
				   return 4;
			   else
				   break;

		   case EM_key_6:
			   if ( menu_lines >= 6 )
				   return 5;
			   else
				   break;

		   case EM_key_7:
			   if ( menu_lines >= 7 )
				   return 6;
			   else
				   break;

		   case EM_key_8:
			   if ( menu_lines >= 8 )
				   return 7;
			   else
				   break;

		   case EM_key_9:
			   if ( menu_lines >= 9 )
				   return 8;
			   else
				   break;


		   case EM_key_CANCEL:
			   if ( allow_exit == TRUE )
				   return -1;
			   else
				   break;

		   default:
			   break;
		}

	}

	return 0;
}

/******************************************************************************
 �������ƣ�get_input_str
 ������������ȡ�ַ���
 �������ƣ�	����/�����	����		����
 iStartLine	����		int			������ʼ��
 iStartCol	����		int			������ʼ��
 uiMinText	����		uint		�˵���ÿ��10�����֣��������ո�  
 uiMaxText	����		uint		�Ƿ�����ȡ�����˳�TRUE=���� FALSE=������ 
 
 ��  ��  ֵ��ѡ��Ĳ˵���  0Ϊ��һ��
		-1 = ȡ���� 
 
 ��      �ߣ�����
 ��      �ڣ�2012-10-29
 �޸���ʷ��
	����		�޸���		�޸�����
	------		---------	-------------
******************************************************************************/
INT8U get_input_str(int iStartLine, int iStartCol, uint uiMinText, uint uiMaxText, char *pcString)
{
	INT16U key = EM_key_NOHIT;
	char buf[100];
	char clr_buf[100];
	INT32U i = 0;

	(void)EA_ucClrKeyBuf();
	strcpy(clr_buf, "");
	for ( i = 0; i < uiMaxText; i++ )
	{
		strcat(clr_buf, " ");
	}

	strcpy(buf, "");

	for (;;)
	{
		lcddisp(iStartLine, iStartCol, (void *)buf);
		key = EA_uiInkey(0);
		switch ( key )
		{
		   case EM_key_0:
			   if ( strlen(buf) < uiMaxText )
				   strcat(buf, "0");
			   break;

		   case EM_key_1:
			   if ( strlen(buf) < uiMaxText )
				   strcat(buf, "1");
			   break;

		   case EM_key_2:
			   if ( strlen(buf) < uiMaxText )
				   strcat(buf, "2");
			   break;

		   case EM_key_3:
			   if ( strlen(buf) < uiMaxText )
				   strcat(buf, "3");
			   break;

		   case EM_key_4:
			   if ( strlen(buf) < uiMaxText )
				   strcat(buf, "4");
			   break;

		   case EM_key_5:
			   if ( strlen(buf) < uiMaxText )
				   strcat(buf, "5");
			   break;

		   case EM_key_6:
			   if ( strlen(buf) < uiMaxText )
				   strcat(buf, "6");
			   break;

		   case EM_key_7:
			   if ( strlen(buf) < uiMaxText )
				   strcat(buf, "7");
			   break;

		   case EM_key_8:
			   if ( strlen(buf) < uiMaxText )
				   strcat(buf, "8");
			   break;

		   case EM_key_9:
			   if ( strlen(buf) < uiMaxText )
				   strcat(buf, "9");
			   break;

		   case EM_key_INTERPUNCTION:           //��
			   if ( strlen(buf) < uiMaxText )
			   {
				   if ( strstr(buf, ".") == NULL )
					   strcat(buf, ".");
			   }
			   break;

		   case EM_key_CLEAR:
			   if ( strlen(buf) > 0 )
			   {
				   buf[strlen(buf) - 1] = '\0';
				   lcddisp(iStartLine, iStartCol, (void *)clr_buf);
			   }
			   break;

		   case EM_key_CANCEL:
			   return EM_ABOLISH;

		   case EM_key_ENTER:                   //ȷ��
			   if ( strlen(buf) < uiMinText )
			   {
				   break;
			   }
			   else
			   {
				   strcpy(pcString, buf);
				   return EM_SUCCESS;
			   }

		   case EM_key_F1:
			   return EM_key_F1;

		   case EM_key_F2:
			   return EM_key_F2;

		   default:
			   break;
		}

	}
}

