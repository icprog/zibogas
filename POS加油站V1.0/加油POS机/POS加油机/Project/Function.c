/*****************************************************************
ģ�����ƣ�EH0218������
���ܸ�Ҫ����ģ����Ҫ��ɸ���Ӧ�ò��溯����д
��    �ߣ��������ķ�������޹�˾/�ڸ���
��    �ڣ�2006��06��13
�޸���ʷ��
����		�޸���		�޸�����
------		---------	-------------
*****************************************************************/

#include  "main.h"



//ϵͳ����
INT8U const   SuperPass[6] =    "889015";	//��������
INT8U         UserPass[6]  =    "000000";	//�û������޸ĵ�����


extern INT8U const    SELECT_USER_ADF2[7];
extern INT8U const    VERIFY_PIN[11];//��֤����
extern INT8U const    READ_15_FILE[5];//��ȡ15�ļ�����
extern INT8U const    WRITE_15_FILE[5];//��ȡ15�ļ�����

extern	DevHandle  hSam1Handle;
extern	DevHandle  hCpuHandle;

extern const char parmFileName[];
extern const char blackFileName[];
extern const char currecFileName[];
extern const char conrecFileName[];
extern const char hisrecFileName[];
extern const char grayrecFileName[];


// *****************************************************************
// ���ܣ�		�������ý���
// *****************************************************************
// const char  func_set[7*16 +2] =  {"1.����ʱ������  2.����������    3.�Զ���������  4.�Զ��ػ�����  5.��������      6.ϵͳ��ʼ��    7.���³���      "};
// const char  func_set[6*16 +2] =  {"1.����ʱ������  2.����������    3.�Զ���������  4.�Զ��ػ�����  5.��������      6.ϵͳ��ʼ��    "};
//INT8U  func_set[6*20 +2] =  {"1.����������    2.�Զ���������  3.�Զ��ػ�����  4.����            "};

// *****************************************************************
// ���ܣ�		�������ý���
// *****************************************************************
//INT8U  parm_set[13*16 +2] =  {"1.��������ʱ��  2.������·��    3.���ó���      4.������ѽ��  5.����ģʽ�趨  6.���꿨�ۿ��趨7.��ѿ��ۿ��趨8.��ѿ�Ʊ���趨9.Ա�����ۿ��趨10.��������     11.ˢ������ʱ�� 12.���Ʊ������ 13.ϵͳ��ʼ��   "}; 
//INT8U  parm_set[9*20 +2] =  {"1.��Ʒ����          2.��������          3.��Ʒ��������      4.�ָ���������      5.APN�����ѡ��     6.����������        7.���ͼ�¼�ϴ�      8.�豸ID����        9.վ�������        "};
INT8U  parm_set[8*20 +2] =  {"1.����������        2.��Ʒ����          3.���ͼ�¼�ϴ�      4.�豸������        5.վ�������        6.�����޸�          7.�ش��ϴ�СƱ      8.VPN�����ѡ��     "};
INT8U  gas_set[3*20 +2] =   {"1.��Ȼ��            2.����              3.����               "}; 

// *****************************************************************
// ���ܣ�		��Ϣ��ѯ����
// *****************************************************************
INT8U  Info_Query[3*20 +2] ={"1.ϵͳ������ѯ      2.��¼������ѯ      3.���׼�¼��ѯ      "};

// *****************************************************************
// ���ܣ�		����ģʽ
// *****************************************************************
// const char  Data_Trans[3*16 +2]={"1.����������    2.����ѯ      3.�����ϴ�      "};
//INT8U  Data_Trans[1*20 +2]={"1.��¼�ϴ�          "};//2.��¼�ɼ�      "};

// *****************************************************************
// ���ܣ�		֤������ѡ��
// *****************************************************************
// const char  Data_Trans[3*16 +2]={"1.����������    2.����ѯ      3.�����ϴ�      "};
INT8U  Id_Type[6*20 +2]={"1.���֤            2.����֤            3.����֤            4.����֤            5.����֤            6.ѧ��֤            "};
//1:���֤
//2:����֤
//3:����֤
//4:����֤
//5:����֤
//6:ѧ��֤

char ReceiveChar;//���յ�������

//volatile INT8U SndBuf[COM_BUFFER_SIZE];								//���ڷ��ͻ�����
//volatile INT8U RcvBuf[COM_BUFFER_SIZE];								//���ڽ��ջ�����

//ϵͳ��������
extern DEV_STAT     DevStat;					//�豸״̬
extern CARD_INFO    CardInfo;					//��Ƭ��Ϣ�����׼�¼�ɴ˱�������
extern ERROR_CARD_INFO ErrCardInfo;	//����״̬



// *****************************************************************
// ���ܣ�		OperPasswordCheck
// ˵��:		����Ա���������
//				��ϵͳ���г���������û�����
// ��ڲ�����	
// ���ڲ�����	Null
// ���ߣ�
// ����ֵ�� -1 �ػ� 						
// *****************************************************************
int OperPasswordCheck()//INT8U *strPassWord
{
	INT8U i = 0;
	INT8U input[50];
//	INT8U buf[50];

	EA_vCls();
	//EA_vTextOut(0, 0, EM_key_FONT8X16, 0, 1, 1, "   ����Ա��¼   ");
	//  sprintf((void *)buf, "���:%s", DevStat.pos_number);

//  sprintf((void *)buf, "%s%s%s%s%s%s", DevStat.card_password[0],DevStat.card_password[1],DevStat.card_password[2],
//  		DevStat.card_password[3], DevStat.card_password[4],DevStat.card_password[5]);
//  lcddisp(1, 1, (void *)buf);
//  (void)EA_uiInkey(0);
	lcddisp(1, 1, (void *)"     ����Ա��¼     ");
	lcddisp(2, 1, (void *)"��������:");
	strcpy((void *)input, "");
	EA_ucClrKeyBuf();
	for (;;)
	{
		i = EA_ucGetInputStr(2, 10, 20, EM_BIG_FONT | EM_MODE_NUMBER | EM_ALIGN_LEFT | EM_SHOW_PASSWORD
							 , 6, 6, 0, (void *)input);
		if ( i == EM_SUCCESS )
			break;
		if ( i == EM_ABOLISH )
			return -6;
	}

	if ( memcmp((void *)input, &DevStat.card_password[0],6) == 0 )
	{
		return ok;
	}
	else
	{
		EA_ucSetInverse(EM_lcd_INVON);
		lcddisp(4, 1, (void *)"      �������      ");
		EA_ucSetInverse(EM_lcd_INVOFF);
		//(void)EA_uiInkey(1);
		return notok;
	}
}


// *****************************************************************
// ���ܣ�		OperPasswordModify
// ˵��:		����Ա�������޸�
//				��ϵͳ���г���������û�����
// ��ڲ�����	
// ���ڲ�����	Null
// ���ߣ�
// ����ֵ�� -1 �ػ� 						
// *****************************************************************
int OperPasswordModify()
{

	int 		ret = 1;
	INT16U		k=0;
	INT8U       input[50];
	INT8U  		retstr[200]; 
	INT8U 		rcv_buf[100], temp_data[20];
	uint		rcv_length;
	uchar       i;
	INT8U            	oldpass[7],newpass[7],surepass[7]; 

	memset(oldpass, 0x00, sizeof(oldpass));
	memset(newpass, 0x00, sizeof(newpass));
	memset(surepass, 0x00, sizeof(surepass));

//  //����������
	EA_vCls();
	//EA_vTextOut(0, 0, EM_key_FONT8X16, 0, 1, 1, "    ����Ա�������޸�    ");
	EA_vDisplay(1, "   ����Ա�����޸�   ");
	EA_vDisplay(2, "����ԭ����:         ");
	strcpy((void *)input, (void *)"");
	(void)EA_ucClrKeyBuf();
	for (;;)
	{
		i = EA_ucGetInputStr(2, 12, 20, EM_BIG_FONT | EM_MODE_NUMBER | EM_ALIGN_LEFT | EM_SHOW_PASSWORD
							 , 6, 6, 0, (void *)input);
		if ( i == EM_SUCCESS )
			break;
		if ( i == EM_ABOLISH )
			return notok;
	}
	if ( memcmp((void *)input, &DevStat.card_password[0],6) == 0 )
	{
		EA_vDisplay(3,"����������:         ");
		(void)EA_ucClrKeyBuf();
		strcpy((void *)newpass, (void *)"");
		for (;;)
		{
			i = EA_ucGetInputStr(3, 12, 20, EM_BIG_FONT | EM_MODE_NUMBER | EM_ALIGN_LEFT | EM_SHOW_PASSWORD
								 , 6, 6, 0, (void *)newpass);
			if ( i == EM_SUCCESS )
				break;
			if ( i == EM_ABOLISH )
				return notok;
		}
		EA_vDisplay(4,"������һ��:         ");
		(void)EA_ucClrKeyBuf();
		strcpy((void *)surepass, (void *)"");
		for (;;)
		{
			i = EA_ucGetInputStr(4, 12, 20, EM_BIG_FONT | EM_MODE_NUMBER | EM_ALIGN_LEFT | EM_SHOW_PASSWORD
								 , 6, 6, 0, (void *)surepass);
			if ( i == EM_SUCCESS )
				break;
			if ( i == EM_ABOLISH )
				return notok;
		}
		if( memcmp(newpass, surepass, 6) == 0 )
		{
			lcddisperr("    �����޸ĳɹ�    ");
			strcpy((void *)DevStat.card_password, (void *)newpass);
			WriteParam();
			EA_uiInkeyMs(2000);
			return ok;
		}
		else
		{
			lcddisperr("    �����޸�ʧ��    ");
			EA_uiInkeyMs(2000);
			return notok;
		}
		//return ok;
	}
	else
	{
		lcddisperr("      �������      ");
		return notok;
	}

	Cpucard_Init();	
	//cpu����λ
	ret = Cpucard_power_on(rcv_buf, &rcv_length);
	if(ret)
	{
		Cpucard_power_off();
		return -1;//�޿�
	}	
	
	memset(rcv_buf,0,sizeof(rcv_buf));
	rcv_length = 0;
	
	//ѡ��1002Ŀ¼
	k = CmdPsam(hCpuHandle, (void *)SELECT_USER_ADF2, sizeof(SELECT_USER_ADF2) ,rcv_buf, &rcv_length);
	if ( k != ok )
	{
		EA_vCls();
		lcddisperr("    ѡ��Ŀ¼����    ");
		EA_uiInkeyMs(2000);
		Cpucard_power_off();
		return -2;
	}

	//��֤����
	k = CmdPsam(hCpuHandle, (void *)VERIFY_PIN, sizeof(VERIFY_PIN) ,rcv_buf, &rcv_length);
	if ( k != ok )
	{
		EA_vCls();
		EA_vDisplay(1,"    ��֤�������    ");
		EA_vDisplay(2,"    �Ǳ�ϵͳ��Ƭ    ");
		EA_vDisplay(3,"   ����������˳�   ");
		EA_uiInkeyMs(2000);
		Cpucard_power_off();
		return -3;
	}


	//��ȡ15�ļ����ݣ����ж���ʲô��Ƭ����
	//
	k = CmdPsam(hCpuHandle, (void *)READ_15_FILE, sizeof(READ_15_FILE) ,rcv_buf, &rcv_length);
	if ( k != ok )
	{
		EA_vCls();
		EA_vDisplay(1,"  ��ȡ��Ϣ�ļ�����  ");
		EA_vDisplay(2,"   ����������˳�   ");
		Cpucard_power_off();
		return -3;
	}

	memcpy(temp_data, rcv_buf, 0x24);

	for(k=0;k<3;k++)
	{
		retstr[k] = ((a_to_h(newpass[2*k]))<<4)|((a_to_h(newpass[2*k+1]))&0x0f);
	}	

	memcpy(temp_data+11, retstr, 3);

	for ( k=0; k<8; k++ )			  //������㲻�ɹ��������ѭ������8��
	{
		////////////////////////////////////////
		//���Ŀ�Ƭʹ������ʶ
		////////////////////////////////////////
		memcpy(retstr, WRITE_15_FILE, 5);		

		memcpy(&retstr[5], temp_data, 36);
		k = CmdPsam(hCpuHandle, retstr, 41, rcv_buf, &rcv_length);		//TAC�����ʼ��
		if ( k != ok )
			continue;
	}
	Cpucard_power_off();
	printf_debug((void *)"������ĳɹ�!");
	return ok;
}


// *****************************************************************
// ���ܣ�		PasswordCheck
// ˵��:		�������
//				��ϵͳ���г���������û�����
// ��ڲ�����	
// ���ڲ�����	Null
// ���ߣ�
// ����ֵ�� -1 �ػ� 						
// *****************************************************************
int PasswordCheck()
{
	uchar i = 0;
	INT8U input[50];

	EA_vCls();
	lcddisp(2, 1, (void *)"��������:");
	lcddisp(4, 1, (void *)"�밴<ȷ��>������");
	strcpy((void *)input, "");

	for ( ;; )
	{
		i = EA_ucGetInputStr(2, 10, 20, EM_BIG_FONT | EM_MODE_NUMBER | EM_ALIGN_LEFT | EM_SHOW_PASSWORD
							 , 6, 6, 0, (void *)input);
		if ( i == EM_SUCCESS )
			break;
		if ( i == EM_ABOLISH )
			return 2;
	}
	if ( memcmp((void *)input, &DevStat.user_passwd[0],6) == 0 )
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

}

// *****************************************************************
// ���ܣ�		PasswordSetting
// ˵��:		�û���������
//				��ϵͳ���г���������û�����
// ��ڲ�����	
// ���ڲ�����	Null
// ���ߣ�
// ����ֵ�� -1 �ػ� 						
// *****************************************************************
int PasswordSetting()
{
//	int             ret=0;
	char            oldpass[7],newpass[7],surepass[7]; 
	uchar i = 0;
	INT8U input[50];

	memset(oldpass, 0x00, sizeof(oldpass));
	memset(newpass, 0x00, sizeof(newpass));
	memset(surepass, 0x00, sizeof(surepass));

	//����������
	EA_vCls();
//  EA_vTextOut(0, 0, EM_key_FONT8X16, 0, 1, 1, "     �û���������     ");
	EA_vDisplay(1, "    �û���������    ");
	EA_vDisplay(2, "����ԭ����:         ");
	(void)EA_ucClrKeyBuf();
	strcpy((void *)input, "");
	for (;;)
	{
		i = EA_ucGetInputStr(2, 12, 20, EM_BIG_FONT | EM_MODE_NUMBER | EM_ALIGN_LEFT | EM_SHOW_PASSWORD
							 , 6, 6, 0, (void *)input);
		if ( i == EM_SUCCESS )
			break;
		if ( i == EM_ABOLISH )
			return notok;
	}
	if ( memcmp((void *)input, &DevStat.user_passwd[0],6) == 0 )
	{
		EA_vDisplay(3,"����������:         ");
		(void)EA_ucClrKeyBuf();
		strcpy((void *)newpass, "");
		for (;;)
		{
			i = EA_ucGetInputStr(3, 12, 20, EM_BIG_FONT | EM_MODE_NUMBER | EM_ALIGN_LEFT | EM_SHOW_PASSWORD
								 , 6, 6, 0, (void *)newpass);
			if ( i == EM_SUCCESS )
				break;
			if ( i == EM_ABOLISH )
				return notok;
		}
		EA_vDisplay(4,"������һ��:         ");
		(void)EA_ucClrKeyBuf();
		strcpy((void *)surepass, "");
		for (;;)
		{
			i = EA_ucGetInputStr(4, 12, 20, EM_BIG_FONT | EM_MODE_NUMBER | EM_ALIGN_LEFT | EM_SHOW_PASSWORD
								 , 6, 6, 0, (void *)surepass);
			if ( i == EM_SUCCESS )
				break;
			if ( i == EM_ABOLISH )
				return notok;
			if(EA_uiInkeyMs(10) == EM_key_CANCEL)
				return notok;
		}
		if( memcmp(newpass, surepass, 6) == 0 )
		{
			lcddisperr("    �����޸ĳɹ�    ");
			strcpy((void *)DevStat.user_passwd, (void *)newpass);
			WriteParam();
//  		EA_uiInkeyMs(500);
			return ok;
		}
		else
		{
			lcddisperr("    �����޸�ʧ��    ");
//  		EA_uiInkeyMs(500);
			return notok;
		}
		return ok;
	}
	return notok;
}

// *****************************************************************
// ���ܣ�		Func_Setting
// ˵��:		�������ò˵�
//              �˲˵���Ҫ���뱣��;��ϵͳ���г���������û�����
// ��ڲ�����	
// ���ڲ�����	Null
// ���ߣ�
// ����ֵ��	-1 �ػ�							
// *****************************************************************
//int Func_Setting()
//{
//    uchar ret;
//    for ( ; ; )
//    {
//        ret = browse_menu(0, (void *)"", func_set, TRUE );
//        switch ( ret )
//        {
//            case 0://����������
////  			ret = KeySound_Setting();
//                break;
//
//            case 1://�Զ���������
////  			ret = BackLight_Setting();
//                break;
//
//            case 2://�Զ���������
////  			ret = PowerOff_Setting();
//                break;
//
//            case 3://����
//                EA_ucClrKeyBuf();
//                EA_vDisp(1, 1, "��ӭʹ�ñ��豸");
//                EA_vDisp(2, 1, "�����״�ͨ����");
//                EA_vDisp(3, 1, "�绰:         ");
//                EA_vDisp(4, 1, "              ");
//                (void)EA_uiInkey(5);
//                break;
//            case 4:
//                return -1;
//
//            case 5:
//                return -5;
//
//            default:
//                break;
//        }
//    }
//}
//
// *****************************************************************
// ���ܣ�		Parm_Setting
// ˵��:		�������ò˵�
// ��ڲ�����	
// ���ڲ�����	Null
// ���ߣ�
// ����ֵ��	-1 �ػ�						
// *****************************************************************
int Parm_Setting(void)
{
	int  ret;

	EA_ucClrKeyBuf();

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

	while(1)
	{

		ret = browse_menu(0, (void *)"", parm_set, TRUE );
		switch ( ret )
		{
		   case 0://����������
				download_whiltelist();				
				break;	
		   case 1://��Ʒ����
 			    select_gas_oil_Setting();
				break;
		   case 2://��¼�ϴ�
			    Gprs_Upload_data();
				//GetTradeAmt();
				break;
		   case 3://�����豸ID��
				SetID();
				break;
		   case 4:  //����վ���
				SetUserID(); 
				break;
		   case 5:  //�����޸�
				PasswordSetting();
				break;
		   case 6:  //ϵͳ��ʼ�� 
			    Reprint_sale_record();
			    break;
		   case 7://APN����
			   if(SAMCompSelect() == ok)
			   {
				   EA_vCls();
				   EA_vDisplay(3, "  APN������޸ĳɹ� ");
				   EA_vDisplay(4, "   ϵͳ���Զ�����   ");
				   EA_uiInkeyMs(2000);
				   System_reset();
			   }
			   break;
//  	   case 7:
//  		   //ϵͳ��ʼ��
//  			InitParam();
//  			break;
//  		case 8:
//   			DateTime_Setting();
//  			break;
			case -1:
//  			Power_Down();			//	�ػ�
				return -1;

			case -5://�����ϼ��˵�
				return -5;

			default:
				break;
		}
	}
}

// *****************************************************************
// ���ܣ�		void Reprint_sale_record( void )
// ˵��:		���´�ӡ�ϴν���СƱ
// ��ڲ�����	��
// ���ڲ�����	Null
// ���ߣ�  frank
// ����ֵ����						
// *****************************************************************
void Reprint_sale_record( void )
{
	print_sale_record();
}
// *****************************************************************
// ���ܣ�		SetID
// ˵��:		�ֹ�����ID��
// ��ڲ�����	
// ���ڲ�����	Null
// ���ߣ�
// ����ֵ��	-1 �ػ�							
// *****************************************************************
unsigned char SetID(void)
{
	char    buf[50];//�����ַ��������
	int     i;   
//  char    str[50];//�����ַ��������

//  sprintf((void *)str, "�豸ID:%02x%02x%02x", DevStat.equ_id[0], DevStat.equ_id[1], DevStat.equ_id[2]);

	EA_vCls();
//  EA_vDisplay(1, (void *)str);
	EA_vDisplay(1, (void *)"�豸ID:%02x%02x%02x",DevStat.equ_id[0], DevStat.equ_id[1], DevStat.equ_id[2]);
	EA_vDisplay(2, (void *)"�����豸ID:");
	EA_vDisplay(3, (void *)"�밴<ȷ��>��ȷ��");
	strcpy((void *)buf, "");
	for ( ;; )
	{
		i = EA_ucGetInputStr(2, 12, 20, EM_BIG_FONT | EM_MODE_NUMBER | EM_ALIGN_LEFT | EM_SHOW_ORIGINALLY
							 , 6, 6, 0, (void *)buf);
		if ( i == EM_SUCCESS )
			break;
		if ( i == EM_ABOLISH )
			return notok;
	}
	
	DevStat.equ_id[0] = (ascii_to_hex(buf[0]) << 4) | ascii_to_hex(buf[1]);
	DevStat.equ_id[1] = (ascii_to_hex(buf[2]) << 4) | ascii_to_hex(buf[3]);
	DevStat.equ_id[2] = (ascii_to_hex(buf[4]) << 4) | ascii_to_hex(buf[5]);

	WriteParam();

	printf_debug((void *)"�豸ID���óɹ�");

	return ok;
}

// *****************************************************************
// ���ܣ�		SetUserID
// ˵��:		�ֹ������̺�
// ��ڲ�����	
// ���ڲ�����	Null
// ���ߣ�
// ����ֵ��	-1 �ػ�							
// *****************************************************************
unsigned char SetUserID(void)
{
	char    buf[50];//�����ַ��������
	int     i;      
	char    str[50];//�����ַ�������� 

	sprintf((void *)str, "վ���:%02x%02x", DevStat.acnt_id[0], DevStat.acnt_id[1]);

	EA_vCls();
	EA_vDisplay(1, (void *)str);
	EA_vDisplay(2, (void *)"����վ���:");
	EA_vDisplay(3, (void *)"�밴<ȷ��>��ȷ��");
	strcpy((void *)buf, "");

	for ( ;; )
	{
		i = EA_ucGetInputStr(2, 12, 20, EM_BIG_FONT | EM_MODE_NUMBER | EM_ALIGN_LEFT | EM_SHOW_ORIGINALLY
							 , 4, 4, 0, (void *)buf);
		if ( i == EM_SUCCESS )
			break;
		if ( i == EM_ABOLISH )
			return notok;
	}
	
	DevStat.acnt_id[0] = (ascii_to_hex(buf[0]) << 4) | ascii_to_hex(buf[1]);
	DevStat.acnt_id[1] = (ascii_to_hex(buf[2]) << 4) | ascii_to_hex(buf[3]);

	WriteParam();

	printf_debug((void *)"վ������óɹ�");

	return ok;
}
///******************************************************************************
// �������ƣ�GetTradeAmt
// ������������ý��׽��
// ����������
// �������ƣ�����/�����  ����        ����
//
// ��  ��  ֵ��    ok(0)-�ɹ�
//                 notok(1)-ʧ��
//
// ��      �� ���ڸ���
// ��      �ڣ�2007-01-30
// �޸���ʷ��
//        ����        �޸���      �޸�����
//******************************************************************************/
//INT8U GetTradeAmt(void)
//{
//    INT8U   ret=0,flag=FALSE;
//    INT32U  LOCAL_V_TYPE  temp_int32u = 0;
//    INT8U   LOCAL_V_TYPE  temp_buf[50];
//    INT8U                 strbuf[50];
//    int                   key;
//////////////////////////////////////////
////�����ֵ���
//////////////////////////////////////////
//
////  init_console(CONSOLE_CHINESE);
//
//    EA_vCls();
//
//    flag = FALSE;
//    EA_vDisplay(1, "�����뽻�׽��");
//    EA_vDisplay(2, "��λ(��)");
//
//    temp_int32u = get_numeric(4,4,4,0);
//    if(temp_int32u<= 0)
//        temp_int32u = 0;
//
//    while (!flag)
//    {
//        //if(temp_int32u > DevStat.max_price_limit || temp_int32u==0 || temp_int32u<3000)//credit amt must >30 and <max_price_limit
//        if(temp_int32u>10000)//credit amt must >30 and <max_price_limit
//        {
//            //clr_scr();
//            //clr_scr();
//            DispStr_CE(0, 6, "���׽��������Ч", DISP_POSITION|DISP_CLRLINE);
//            Beep(1000);
//        }
//        else
//        {
//        //  clr_scr();
//        //  Clean_KEY_FIFO();
//            flag = TRUE;
//            DevStat.fare = temp_int32u;
//            WriteParam();
//            break;
//        //  sprintf(strbuf, "���:%6.2fԪ", CardInfo.fare/100);
//        //  if(select_affirm(4, strbuf)==0)
//        //      break;
//
//        }
//
////�ж������˳�������
//        ret = KEY_read();
//        if(ret ==EXIT_KEY_COMM)
//        {
//            Beep(2000);
//            delay_n_ms(200);
//            init_console(CONSOLE_CHINESE);
//            KEY_init(KEY_BEEP_KEY|KEY_AUTO_EL_ON|KEY_FIFO_MODE);//�������������ͱ���
//            (void)sm_setting(MSK_KEY_UP|MSK_KEY_DOWN); //��ֹ�����»�̧����ʱ����Ӧ
//            KEY_beep_mask_set(0xFFFfffff);
//            KEY_EL_mask_set(0xFFFfffff);
//            Clean_KEY_FIFO();
//            return notok;
//        }
//        if(ret == EXIT_KEY_POWER)
//        {
//            Halt_HD6305();
//        }
////		else
////		{
////			flag = FALSE;
////		}
//
////��������
////		clr_scr();
//        DispStr_CE(0, 6, "", DISP_POSITION|DISP_CLRLINE);
//        DispStr_CE(0, 0, "�����뽻�׽��", DISP_POSITION|DISP_CLRLINE);
//        temp_int32u = 0;
//        temp_int32u = get_numeric(4,4,4,0);
//        if(temp_int32u < 0)
//            temp_int32u = 0;
//
//    }
//
//    init_console(CONSOLE_CHINESE);
//    KEY_init(KEY_BEEP_KEY|KEY_AUTO_EL_ON|KEY_FIFO_MODE);//�������������ͱ���
//    (void)sm_setting(MSK_KEY_UP|MSK_KEY_DOWN); //��ֹ�����»�̧����ʱ����Ӧ
//    KEY_beep_mask_set(0xFFFfffff);
//    KEY_EL_mask_set(0xFFFfffff);
//    Clean_KEY_FIFO();
//
//    return ok;
//}
////////////////////////////////////////////////////////////
//APN�����ѡ��
////////////////////////////////////////////////////////////
INT8U SAMCompSelect()
{
	int ret;
	INT8U APN_menu[2*20 + 2] = {"1.�ƶ�����          2.�ƶ�ר��          "};
//	DevStat.samcompcode=1;
	ret = browse_menu(0, (unsigned char*)"", APN_menu, TRUE);
	switch ( ret )
	{
		case 0:	// �ƶ�����
			DevStat.samcompcode = 0;
			WriteParam();
			break;

		case 1:	// �ƶ�ר��
			DevStat.samcompcode = 1;
			WriteParam();
			break;

		default: 
			return notok;
		//	break;
	}
	return ok;
}
// *****************************************************************
// ���ܣ�		Information_Query
// ˵��:		�������ò˵�
// ��ڲ�����	
// ���ڲ�����	Null
// ���ߣ�
// ����ֵ��	-1 �ػ�							
// *****************************************************************
int Information_Query()
{

	int             ret;

	for ( ; ; )
	{
		ret = browse_menu(0, (void *)"", Info_Query, TRUE );
		switch ( ret )
		{
			case 0://������ѯ
				ret = Parm_Query();
// 				return ret;
				break;

			case 1://��¼������ѯ
				ret = Record_Query();
// 				return ret;
				break;

			case 2://��Ƭ��ѯ
				ret = Card_Query();
// 				return ret;
				break;

			case -1://EXIT_KEY_POWER,EXIT_AUTO_QUIT
				//Halt_EH0218(POWER_DELAY);			//	�ػ�
				return -1;
				break;

			case -5://�����ϼ��˵�
				return -5;
				break;

			default:
				break;
		}
	}
}

// // *****************************************************************
// // ���ܣ�		CardConsumeProcess
// // ˵��:		��Ƭ��������
// // ��ڲ�����
// // ���ڲ�����	Null
// // ���ߣ�
// // ����ֵ��
// // *****************************************************************
// int CardConsumeProcess()
// {
// 	int     ret = 0;
// 	char    strbuf[20], str[20];
// 	float   fare = 0;
//
//
// 	EA_ucClrKeyBuf();
//
// 	CardInfo.fare = 100;//��������
//
// 	RCX_Init(TYPE_A_ISO14443);
//
// 	//��ʾ��ˢ��
// 	DispStr_CE(0, 3, "����10����ˢ��", DISP_CENTER);
//
// 	SPT_set(640);
//
// 	while ( SPT_read() )
// 	{
// 		ret = SPT_read()%64;
// 		if ( ret == 0 )
// 		{
// 			EA_ucClrKeyBuf();
// 			sprintf(strbuf, "����%d����ˢ��", SPT_read()/64);
// 			DispStr_CE(0, 3, strbuf, DISP_CENTER);
// 		}
// 		ret = ICCardProcess();
// 		if ( ret == 0 )
// 			break;
// 	}
//
// 	RCX_Close();
//
// 	ICC_deactivate_current_sock();
// 	ICC_interface_power(ICC_POWER_OFF);
// 	return 0;
// }

// *****************************************************************
// ���ܣ�		Datetime_Setting
// ˵��:		����ʱ������
// ��ڲ�����	
// ���ڲ�����	Null
// ���ߣ�
// ����ֵ��							
// *****************************************************************
int DateTime_Setting(void)
{
//  modify_date();
	TimeModify();
	EA_uiInkeyMs(100);
	EA_ucClrKeyBuf();
	return 1;
}
// *****************************************************************
// ���ܣ�		select_gas_oil_Setting(void)
// ˵��:		��Ʒѡ��  -- ���ͻ�������
// ��ڲ�����	
// ���ڲ�����	Null
// ���ߣ�		������
// ����ֵ��	    OK or NOTOK					
// *****************************************************************
int select_gas_oil_Setting(void)
{
	int uiRet;
	uiRet = browse_menu(0, (void *)"", gas_set, TRUE );
	switch ( uiRet )
	{
	   case 0:
		   DevStat.fill_mode = 0;
		   break;
	   case 1:
		   DevStat.fill_mode = 1;
		   break;
	   case 2:
		   DevStat.fill_mode = 2;
		   break;
	   default:
		   break;
	}
	WriteParam();
	return ok;
}

//// *****************************************************************
//// ���ܣ�		Parm_Query
//// ˵��:		������ѯ
//// ��ڲ�����
//// ���ڲ�����	Null
//// ���ߣ�
//// ����ֵ��
//// *****************************************************************
//int Parm_Query()
//{
//    char buf[50];
////	char temp_buf[4];
//    FP64            balance = 0.0;
//    EA_vCls();
//    strcpy((void *)buf, "�豸���: ");
////  temp_buf[0] = DevStat.equ_id[2];
////  temp_buf[1] = DevStat.equ_id[1];
////  temp_buf[2] = DevStat.equ_id[0];
//    htoa(&buf[10], DevStat.equ_id, sizeof(DevStat.equ_id)); //by liujihua
////  htoa(&buf[8], temp_buf, 3);
//
//    EA_vDisp(1,1,buf);
//
//    strcpy(buf, "PSAM����: ");
//    htoa(&buf[10], &(DevStat.pos_number[0]), 4);   //by liujihua
////  htoa(&buf[8], &DevStat.pos_number[3], 1);
////  htoa(&buf[10], &DevStat.pos_number[2], 1);
////  htoa(&buf[12], &DevStat.pos_number[1], 1);
////  htoa(&buf[14], &DevStat.pos_number[0], 1);
//
//    EA_vDisp(2,1,buf);
//
//    strcpy(buf, "����Ա��: ");
//
//    htoa(&buf[10], &DevStat.cur_driver_number[1], 3);  //by liujihua
////  buf[8] = DevStat.cur_driver_number[2];
////  buf[9] = DevStat.cur_driver_number[1];
////  buf[10] = DevStat.cur_driver_number[0];
//
//
//
//    EA_vDisp(3,1,buf);
//    balance = (FP64)(DevStat.oper_card_amt / 100);
//    sprintf((void *)buf, "�������: %7.2fԪ", balance);
//    EA_vDisp(4,1,buf);
//    EA_uiInkey(10);
//    return ok;
//}
//
// *****************************************************************
// ���ܣ�		Record_Query
// ˵��:		��¼��ѯ
// ��ڲ�����	
// ���ڲ�����	Null
// ���ߣ�
// ����ֵ��							
// *****************************************************************
int Record_Query()
{
	INT16U key;
	INT16U Num_rec;
	INT16U Num_black;
	INT16U Num_gray;
	uint   uiRetNum;
	uchar  ucOpenID = 0;
	EA_vCls();
	EA_ucClrKeyBuf();
//////////////////////////////////////////////////////
	if ( EA_ucPFOpen((uchar *)currecFileName, &ucOpenID) != EM_ffs_SUCCESS )
	{
		lcddisperr("�򿪼�¼�ļ�ʧ��");
		(void)EA_ucPFClose(ucOpenID);
		return notok;
	}
	if ( EA_ucPFInfo(ucOpenID, EM_ffs_VALIDREC, &uiRetNum) != EM_ffs_SUCCESS )
	{
		lcddisperr("��ȡ��¼�ļ�ʧ��");
		(void)EA_ucPFClose(ucOpenID);
		return notok;
	}
	Num_rec = uiRetNum;
	(void)EA_ucPFClose(ucOpenID);
//////////////////////////////////////////////////////////
	if ( EA_ucPFOpen((uchar *)blackFileName, &ucOpenID) != EM_ffs_SUCCESS )
	{
		lcddisperr("�򿪼�¼�ļ�ʧ��");
		(void)EA_ucPFClose(ucOpenID);
		return notok;
	}
	if ( EA_ucPFInfo(ucOpenID, EM_ffs_VALIDREC, &uiRetNum) != EM_ffs_SUCCESS )
	{
		lcddisperr("��ȡ��¼�ļ�ʧ��");
		(void)EA_ucPFClose(ucOpenID);
		return notok;
	}
	Num_black = uiRetNum;
	(void)EA_ucPFClose(ucOpenID);
////////////////////////////////////////////////////////
	if ( EA_ucPFOpen((uchar *)conrecFileName, &ucOpenID) != EM_ffs_SUCCESS )
	{
		lcddisperr("�򿪼�¼�ļ�ʧ��");
		(void)EA_ucPFClose(ucOpenID);
		return notok;
	}
	if ( EA_ucPFInfo(ucOpenID, EM_ffs_VALIDREC, &uiRetNum) != EM_ffs_SUCCESS )
	{
		lcddisperr("��ȡ��¼�ļ�ʧ��");
		(void)EA_ucPFClose(ucOpenID);
		return notok;
	}
	Num_gray = uiRetNum;
	(void)EA_ucPFClose(ucOpenID);
//////////////////////////////////////////////////////////

	EA_vDisplay(1, "ϵͳ�洢��¼��Ϣ��ѯ");
	EA_vDisplay(2, "���׼�¼: %6d   ��", Num_rec);
	EA_vDisplay(3, "�� �� ��: %6d   ��", Num_black);
	EA_vDisplay(4, "���Ѽ�¼: %6d   ��", Num_gray);
	key = EA_uiInkey(0);
	if( key == EM_key_CANCEL)
	{
		return ok;
	}
	return ok;
}
//
//// *****************************************************************
//// ���ܣ�		AnalyAndDisplay
//// ˵��:		��ѯ��¼����ʵ�ֽ��׼�¼
//// ��ڲ�����
//// ���ڲ�����	Null
//// ���ߣ�
//// ����ֵ��
//// *****************************************************************
//void AnalyAndDisplay(RECORD * strTradeInfo)
//{
//    char  strbuf[50];
//    float   fBuf = 0;
//    INT8U   LOCAL_V_TYPE  temp_buf[100];
//    RECORD * prec = (RECORD *)strTradeInfo;
//
//    EA_ucClrKeyBuf();
//
//    //��ʾ��Ƭ��Ϣ
//    EA_vCls();
//    htoa(temp_buf, &prec->card_serial_number[4], 4);
//    strcpy((void *)strbuf, (void *)"����:   ");
//    strcat((void *)strbuf, (void *)temp_buf);
//    EA_vDisp(1,1,strbuf);
///*
//    if (prec->card_purchase_type == 0x18)           //�Ƿ�Ϊ˾�����ϰ�׷�Ӽ�¼
//    {
//        GetCardType(0xF0, temp);        //������Ƭ������Ϣ
//    }
//    else
//    {
//        GetCardType(prec->card_type, temp);     //������Ƭ������Ϣ
//    }
//    sprintf(strbuf, "����: %s", temp);
//    DispStr_CE(0,2, strbuf, DISP_POSITION);
//*/
//
//    switch ( prec->card_purchase_type)
//    {
//        case TT_NORMAL_CONSUME_MONEY:
//            sprintf((void *)strbuf, "ģʽ:����   02%d02%d02%d", prec->card_in_time.hour,prec->card_in_time.minute,prec->card_in_time.second);//������Ƭ������Ϣ
//            EA_vDisp(2,1, strbuf);
//
//            fBuf    = (float)(prec->fare[0] * 65536 + prec->fare[1]*256 + prec->fare[2])/100;
//            sprintf((void *)strbuf, "����:     %5.2fԪ   ", fBuf);
//            EA_vDisp(3,1, strbuf);
//            fBuf    = (float)(prec->balance[0]*65536 + prec->balance[1]*256 + prec->balance[2])/100;
//            sprintf((void *)strbuf, "���:     %5.2fԪ   ", fBuf);
//            EA_vDisp(4,1, strbuf);
//            break;
//        case TT_CARD_SALE:
//            //EA_vDisp(2, 1,          "ģʽ:           �ۿ�");
//            sprintf((void *)strbuf, "ģʽ:�ۿ�   02%d02%d02%d", prec->card_in_time.hour,prec->card_in_time.minute,prec->card_in_time.second);//������Ƭ������Ϣ
//            EA_vDisp(2,1, strbuf);
//            fBuf    = (float)(prec->deposit_amt[0]*256 + prec->deposit_amt[1])/100;
//            sprintf((void *)strbuf, "Ѻ��:     %5.2fԪ   ", fBuf);
//            EA_vDisp(3,1, strbuf);
//            break;
//        case TT_CREDIT_MONEY:
//            //EA_vDisp(2,1,           "ģʽ:           ��ֵ");
//            sprintf((void *)strbuf, "ģʽ:��ֵ   02%d02%d02%d", prec->card_in_time.hour,prec->card_in_time.minute,prec->card_in_time.second);//������Ƭ������Ϣ
//            EA_vDisp(2,1, strbuf);
//            fBuf    = (float)(prec->fare[0]*65536 + prec->fare[1]*256 + prec->fare[2])/100;
//            sprintf((void *)strbuf, "��ֵ:     %5.2fԪ   ", fBuf);
//            EA_vDisp(3,1, strbuf);
//            fBuf    = (float)(prec->balance[0]*65536 + prec->balance[1]*256 + prec->balance[2])/100;
//            sprintf((void *)strbuf, "���:     %5.2fԪ   ", fBuf);
//            EA_vDisp(4,1, strbuf);
//            break;
//        default:
//            EA_vDisp(2,1,           "ģʽ:           ����");
//            EA_vDisp(4,1,           "    ��������˳�    ");
//            break;
//
//    }
//}


// *****************************************************************
// ���ܣ�		Card_Query
// ˵��:		��Ƭ��Ϣ��ѯ
// ��ڲ�����	
// ���ڲ�����	Null
// ���ߣ�
// ����ֵ��							
// *****************************************************************
int Card_Query()
{
	int         count;
	long        lKeyRet = 255;  
	uint		uiRetCnd;
	uchar       ucOpenID = 0;
//	uchar   	ucRet;
	uint		uiReadCnt;
	uchar 		i = 0;	
	INT8U 		p[60];

	RECORD *prec = (RECORD *)p;
	count = 0 ;//���һ��

	(void)EA_ucClrKeyBuf();
	i = EA_ucPFOpen( (uchar *)currecFileName, &ucOpenID);
	if ( i != EM_ffs_SUCCESS )
	{
		(void)EA_ucPFClose(ucOpenID);
		lcddisperr("�򿪼�¼�ļ�ʧ��");
		return notok;
	}
	// ��ȡ��Ч��Ϣ����
	(void)EA_ucPFInfo(ucOpenID, EM_ffs_VALIDREC, &uiReadCnt);	
	if ( uiReadCnt == 0 )//��¼Ϊ�գ�ֱ������¼�¼����
	{
		printf_debug((void *)"��¼Ϊ��");
		(void)EA_ucPFClose(ucOpenID);
		return notok;
	}
	count = uiReadCnt;
	i = EA_ucPFReadRec(ucOpenID, uiReadCnt, 1, &uiRetCnd, p);
	if ( i != EM_ffs_SUCCESS )
	{
		(void)EA_ucPFClose(ucOpenID);
		lcddisperr("����¼�ļ�ʧ��");
		return notok;
	}
	(void)AnalyAndDisplay(prec);
	(void)EA_ucPFClose(ucOpenID);
	for ( ;; )
	{
		lKeyRet = EA_uiInkey(0);
		if ( (lKeyRet == EM_key_CANCEL) || (lKeyRet == EM_key_CLEAR) )			//�˳�
		{
			return 0;
		}
		switch ( lKeyRet )
		{
			case EM_key_INTERPUNCTION:		//	up
				count --;
				if ( count <= 0 )//��¼����
				{
					(void)EA_ucClrKeyBuf();
					EA_vCls();
					count = 0;
					EA_vDisp(4, 1, "     û�и���ļ�¼     ");
					EA_uiInkey(1);
					return 0;
				}
				break;

			case EM_key_LETTER:		//	down
				count ++;
				if ( count > uiReadCnt )
				{
					(void)EA_ucClrKeyBuf();
					EA_vCls();
					count = 0;
					EA_vDisp(4, 1, "    ���һ����¼    ");
					EA_uiInkey(1);
					return 0;
				}
				break;

			default:
				break;
		}//end switch
		i = EA_ucPFOpen( (uchar *)currecFileName, &ucOpenID);
		if ( i != EM_ffs_SUCCESS )
		{
			(void)EA_ucPFClose(ucOpenID);
			lcddisperr("�򿪼�¼�ļ�ʧ��");
			return notok;
		}
		i = EA_ucPFReadRec(ucOpenID, count, 1, &uiRetCnd, p);
		if ( i != EM_ffs_SUCCESS )
		{
			(void)EA_ucPFClose(ucOpenID);
			lcddisperr("����¼�ļ�ʧ��");
			return notok;
		}
		AnalyAndDisplay(prec);
		(void)EA_ucPFClose(ucOpenID);
	}//end while

}

// *****************************************************************
// ���ܣ�		ͨѶ����
// ��ʽ:        @START + COMMAND + LENGTH + DATA + CRC + END
// ˵��:		������ݲɼ������������ء�ʱ����µȲ���
// 				������ִ��󣬴��󷵻�Ϊ@START + E + REASON
// ��ڲ�����	
// ���ڲ�����	Null
// ���ߣ�		
// ����ֵ��		-1:�ػ�					
// *****************************************************************
//int CommunicationDeal()
//{
//
//    typ_msg_word        msg;
//    typ_UART_stat_word  Usw;
//    char                length;//�������ݳ���
//
//    INT32U rec_num;
//    char        flag;
//
//    RcvdLength = 0;
//    //ClrUART();//��մ��ڻ�����
//
//    while ( 1 )
//    {
//        //SetAutoPowerOffTime(9999);            //ͨѶ��ֹ����
//        msg.s_word = sys_msg(SM_STAY_AWAKE);
//        if ( msg.bits.comm_data )
//        {
//            ReceiveChar = (char)UART_get_char();
//
//#ifdef DEBUG
//            TRACE(chtostr(ReceiveChar));
//#endif
//
//            if ( RcvdLength == 0 && !(ReceiveChar == 0x20))// || ReceiveChar == '@') )
//            {
//                continue;               //��ͷ����Ϊ0x02��'@'
//            }
//            else
//            {
//            }
//
//            RcvBuf[RcvdLength] = ReceiveChar;
//            RcvdLength++;
//
//            if ( RcvdLength >= COM_BUFFER_SIZE )
//            {
//                RcvdLength = 0;             //���ȳ���
//                return LENGTH_ERR;
//            }
//            if((RcvdLength==NeedRcvLength) && (NeedRcvLength!=0))
//            {
//                return ok;
//            }
//
//            if( (NeedRcvLength==0) && (RcvdLength == RcvBuf[2] + 4 ))
//            {
//                return ok;
//            }
//
//        }
//
//        if ( msg.bits.key_available )
//        {
//            switch ( KEY_read() )
//            {
//                case EXIT_KEY_COMM:
//                case EXIT_KEY_F1:
//                    CloseCom();
//                    //SetAutoPowerOffTime(DevStat.poweroff_time);       //�ָ�����
//                    return OPER_CANCLE;
//
//                case EXIT_KEY_POWER:
//                    CloseCom();
//                    //SetAutoPowerOffTime(DevStat.poweroff_time);       //�ָ�����
//                    return OPER_SLEEP;
//                default:
//                    break;
//            }
//        }
//    }
//
//}

// *****************************************************************
// ���ܣ�		DataGather
// ˵��:		���ݲɼ�
// ��ڲ�����	
// ���ڲ�����	Null
// ���ߣ�		
// ����ֵ��		-1:�ػ�					
// *****************************************************************
//int DataGather()
//{
//    typ_msg_word        msg;
//    typ_UART_stat_word  Usw;
//    char                length;//�������ݳ���
//
//    INT32U rec_num;
//// 	INT8U *p;
//    char        flag;
//    RECORD *prec;
//    uchar ucResult;
//
//// 	//�ɼ�����¼�ж�
//// 	rec_num = DB_count_records(DB_REC);
//// 	if(rec_num == 0)			// û�м�¼
//// 	{
//// 		DispStr_CE(0,1, "δˢ�ɼ���!", DISP_CENTER | DISP_CLRSCR);
//// 		DispStr_CE(0,3, "�뷵����������", DISP_CENTER);
//// 		DispStr_CE(0,5, "ˢ�ɼ���", DISP_CENTER);
//// 		Beep(1000);
//// 		SleepMs(1000);
//// 		return notok;
//// 	}
//// 	else
//// 	{
//// 		//�ж����һ����¼�Ƿ�Ϊ�ɼ�����¼
//// 		prec = (RECORD *)DB_jump_to_record(DB_REC, rec_num - 1, &flag);
//// 		if (prec->card_purchase_type != TT_GATHER)
//// 		{
//// 			DispStr_CE(0,1, "δˢ�ɼ���!", DISP_CENTER | DISP_CLRSCR);
//// 			DispStr_CE(0,3, "�뷵����������", DISP_CENTER);
//// 			DispStr_CE(0,5, "ˢ�ɼ���", DISP_CENTER);
//// 			Beep(1000);
//// 			SleepMs(1000);
//// 			return notok;
//// 		}
//// 	}
//    //(void)EA_ucClrKeyBuf();
//    //DispStr_CE(0,0, "ͨѶ��...", DISP_CENTER);
//    EA_vCls();
//    EA_vDisplay(1, "ͨѶ��...");
//    //InitCom();//��ʼ��RS232
//    //  ��RS232�豸
//    ucResult = EA_ucOpenDevice("COM1", EM_io_RDWR, &hRS232Handle);
//    if ( ucResult != EM_SUCCESS )
//    {
//        lcddisperr("�򿪴���ʧ��");
//        return;
//    }
//    RcvdLength = 0;
//
//    while ( 1 )
//    {
//        //SetAutoPowerOffTime(9999);            //ͨѶ��ֹ����
//        msg.s_word = sys_msg(SM_STAY_AWAKE);
//        if ( msg.bits.comm_data )
//        {
//            ReceiveChar = (char)armgetch();
//
//#ifdef DEBUG
//            TRACE(chtostr(ReceiveChar));
//#endif
//
//            if ( RcvdLength == 0 && !(ReceiveChar == 0x02 || ReceiveChar == '@') )
//            {
//                continue;       //��ͷ����Ϊ0x02��'@'
//            }
//            else
//            {
//            }
//
//            RcvBuf[RcvdLength] = ReceiveChar;
//            RcvdLength++;
//
//            if ( RcvdLength >= COM_BUFFER_SIZE )
//            {
//                RcvdLength = 0;             //���ȳ���
//                return;
//            }
//
//            if ( RcvdLength == RcvBuf[2] + 4 )
//            {
//                CmdProcess();
//            }
//
//// 			if ( RcvdLength==2 )//��ʱ���յ����������򳤶��ֽ�
//// 			{
//// 				RcvBuf[RcvdLength] = ReceiveChar;
//// 				length = ReceiveChar;
//// 			}
//// 			else
//// 			{
//// 				if(RcvdLength == length+14)//@START + COMMAD + LENGTH + DATA + CRC1 + CRC2 + \END
//// 				{
//// 					CommunicationDeal();//ͨѶ����
//// 					RcvdLength = 0;
//// 				}
//// 				else
//// 				{
//// 					RcvBuf[RcvdLength++] = ReceiveChar;
//// 				}
//// 			}
//        }
//
//        if ( msg.bits.key_available )
//        {
//            switch ( KEY_read() )
//            {
//                case EXIT_KEY_COMM:
//                case EXIT_KEY_F1:
//                    CloseCom();
//                    //SetAutoPowerOffTime(DevStat.poweroff_time);           //�ָ�����
//                    return 1;
//
//                case EXIT_KEY_POWER:
//                    CloseCom();
//                    //SetAutoPowerOffTime(DevStat.poweroff_time);           //�ָ�����
//                    return -1;
//
//                default:
//                    break;
//            }
//        }
//    }
//
//    CloseCom();
//
//
//    return 1;
//}

// *****************************************************************
// ���ܣ�		RecordGather
// ˵��:		�ɼ������ֳ�����
// ��ڲ�����	
// ���ڲ�����	Null
// ���ߣ�		
// ����ֵ��		-1:�ػ�					
// *****************************************************************
//int RecordGather()
//{
//
//    typ_msg_word        msg;
//    typ_UART_stat_word  Usw;
//    char                length;//�������ݳ���
//
//    INT32U rec_num;
//    INT32U rec_no;//��¼���
//    INT16U crc;
//    INT8U record[83];
//    INT32U  black_num;//����������
//    INT32U  black_no;//���������
//// 	INT8U *p;
//    char        flag;
//    INT8U *p;
//    int    ret=0,i=0,j=0;
//
////  (void)EA_ucClrKeyBuf();
//    DispStr_CE(0,0, "ͨѶ��...", DISP_CENTER);
//
//    InitCom();//��ʼ��RS232
//
//    RcvdLength = 0;
//
////////////////////////////////////
////��ȡ����
/////////////////////////////////////
//
//
////��ȡ��¼����
//
//    SndBuf[0]=0x02;
//    SndBuf[1]=0x00;
//    SndBuf[2]=10;
//    SndBuf[3]='Z';
//    SndBuf[4]='N';
//    SndBuf[5]='I';
//    SndBuf[6]=0x00;
//    for(i=0;i<6;i++)
//        SndBuf[7+i]=0xFF;
//    Rs232BCCWrite(SndBuf, 13);
//    NeedRcvLength = 35;
//    ret = CommunicationDeal();
//    if(ret != 0)
//    {
//        CloseCom();
//        return ret;
//    }
//
//    rec_no = 0;
////	err_num = 0;
//    NeedRcvLength = 0;
//
//    while(1)
//    {
//        SndBuf[0]=0x02;
//        SndBuf[1]=0x00;
//        SndBuf[2]=4;
//        SndBuf[3]='I';
//        SndBuf[4]=0x00;//��ʱ�̶�Ϊ0x01
//        SndBuf[5]=(INT8U)(rec_no>>8);
//        SndBuf[6]=(INT8U)(rec_no&0xff);
//        Rs232BCCWrite(SndBuf, 7);
//        ret = CommunicationDeal();
//        if(ret != 0)
//        {
//            CloseCom();
//            return ret;
//        }
//
//        if(RcvBuf[0]==0x20 && RcvBuf[1]==0x00 && RcvBuf[3] == 'E')//û�м�¼��
//            break;
//        if(RcvBuf[0]==0x20 && RcvBuf[1]==0x00 && RcvBuf[3] == 'I')//�м�¼
//        {
//
//            //�����¼��������
//            memcpy(record, (INT8U *)(RcvBuf+6), 57);
//            crc = Cal_CRC(record, 57);
//            record[57] = ((crc >> 8)& 0xff);//CRC1
//            record[58] = (crc & 0xff);//CRC2
//            ret = SaveCardRecord(record);
//            if ( ret != ok )
//            {
//#ifdef DEBUG
//                TRACE232("Store record fail.\n\r");
//#endif
//                (void)EA_ucClrKeyBuf();
//                DispStr_CE(0,0, "���ݿⱣ��ʧ��!", DISP_CENTER);
//
//                CloseCom();
//                return notok;
//            }
//
//            rec_no++;//��¼ָ������
//        }
//
//    }
//
////////////////////////////////////
////���غ�����
/////////////////////////////////////
//    SndBuf[0]=0x02;
//    SndBuf[1]=0x00;
//    SndBuf[2]=0x01;
//    SndBuf[3]='J';
//    Rs232BCCWrite(SndBuf, 4);
//    ret = CommunicationDeal();
//    if(ret != 0)
//    {
//        CloseCom();
//        (void)EA_ucClrKeyBuf();
//        DispStr_CE(0,0, "�������سɹ�!", DISP_CENTER);
//        DispStr_CE(0,4, "���������ʧ��!", DISP_CENTER);
//        return ret;
//    }
//    black_num = DB_count_records(DB_BLACK);
//
//    black_no = 0;
//    while(1)
//    {
//        if(black_num == black_no)
//            break;
//        SndBuf[0]=0x02;
//        SndBuf[1]=0x00;
//        SndBuf[2]=0x05;
//        SndBuf[3]='G';
//        p = (INT8U *)DB_jump_to_record(DB_BLACK, black_no, &flag);  //ȡ��¼
//        memcpy((void *)&SndBuf[4], p,4);
//
//        Rs232BCCWrite(SndBuf, 4 + 5);
//
//        ret = CommunicationDeal();
//        if(ret != 0)
//        {
//            CloseCom();
//            return ret;
//        }
//
//        if(RcvBuf[0]==0x20 && RcvBuf[1]==0x00 && RcvBuf[3] == 'G')//���������³ɹ�
//        {
//
//            black_no++;//��¼ָ������
//        }
//
//    }
//
////////////////////////////////////
////�������ݺ�ʱ��
/////////////////////////////////////
//
//    (void)EA_ucClrKeyBuf();
//    DispStr_CE(0,0, "�ɼ��ɹ�!", DISP_CENTER);
//    delay_n_ms(200);
//    CloseCom();
//
//
//    return 1;
//}

/*****************************************************************
����ԭ�ͣ�chtostr
������������һ��'1'ת��Ϊ"1"
����������
�������ƣ�	����/�����	����		����
-----------		-----------	------	   	-------
disp_buf		����			INT8U		����Ĳ���

��  ��  ֵ����
��      ��	������
��      �ڣ�2005-08-20
�޸���ʷ��
����		�޸���		�޸�����
------		---------	-------------
*****************************************************************/
INT8U *chtostr(INT8U disp_buf)
{
	static INT8U buf[2];
	buf[0] = disp_buf;
	buf[1] = '\0';
	return buf;
}

/******************************************************************************
 �������ƣ�CmdProcess
 ���������������ݺе�ͨѶ��������ʹ���
 ����������
 �������ƣ�	����/�����	����		����
				
 ��  ��  ֵ����
 ��      ��	������
 ��      �ڣ�2004-09-02
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
******************************************************************************/
//void CmdProcess(void)
//{
//
////	INT8U LOCAL_V_TYPE i = 0;
////     INT8U j = 0;
////     INT8U LOCAL_V_TYPE m = 0;
//    static INT16U LOCAL_V_TYPE cur_trans_rec_num = 0;               //��ǰ���ͼ�¼��ָ��
//    static INT8U LOCAL_V_TYPE rec_have_snd = 0;                         //�����ͳ�һ����¼ʱ����TRUE�����յ�ȷ��ʱ��FLASE����֤�˼�¼�Ĳ�����
////	INT16U LOCAL_V_TYPE data_len = 0;										//�����򳤶�
//    BUS_TIME time;
////	typ_RTC_date_rec    date;
////	typ_RTC_time_rec    time2;
//    INT32U temp_int32u = 0;
//    INT32U black_ptr = 0;
////	INT16U temp_int16u = 0;
//    static INT32U rec_num = 0;
//    static INT32U db_rec_num = 0;
//    static INT32U db_rec_bak_num = 0;
//    static INT32U db_rec_history_num = 0;
//    INT32U cur_trans_rec_num = 0;               //��ǰ���ͼ�¼��ָ��
//    INT8U *p;
//    char        flag=1;
////	INT32U rec_ptr;
//    INT8U buf[40];
//
//
//    switch ( RcvBuf[3] )
//    {
//        ////////////////////////////////////////
//        //������ݺ��Ƿ���ڻ�ɾ�����ݺ�
//        ////////////////////////////////////////
//        case  'O':      //ŷ
//        case  'Y':      // ɾ�����ݺ�(Del Box)
//            SetBell(ON);    //��������
//
//            SndBuf[0] = 0x20;
//            SndBuf[1] = 0x00;
//            SndBuf[2] = 0x01;
//            SndBuf[3] = 'O';        //ŷ
//
//            Rs232BCCWrite(SndBuf, 4);
//
//            SetBell(OFF);   //�ط�����
//            break;
//
//            ////////////////////////////////////////
//            // �����ݺ�����
//            ////////////////////////////////////////
//        case 'Q':
//            SetBell(ON);    //��������
//
//            memcpy((void *)SndBuf, "\x20\x00\x05Q\x35\x00\x00\x00", 8);
//            SndBuf[8] = DevStat.POS_type;
//
//            Rs232BCCWrite(SndBuf, 9);
//
//            SetBell(OFF);   //�ط�����
//            break;
//
//            ////////////////////////////////////////
//            // �����ݺ�ʱ��
//            ////////////////////////////////////////
//        case 'A':
//            SetBell(ON);    //��������
//            (void)Get_Time(&time);
//
//            memset((void *)SndBuf, 0x00, sizeof(SndBuf));
//            memcpy((void *)SndBuf, "\x20\x00", 2);
//            SndBuf[2] = DevStat.POS_type;
//            memcpy((void *)&SndBuf[19], &time, sizeof(time));
//            //memcpy((void *)&SndBuf[26], &DevStat.bus_number[1], 2);
//            //memcpy((void *)&SndBuf[28], &DevStat.line_number[0], 2);
//            memcpy((void *)&SndBuf[26], &DevStat.equ_id[1], 2);
//            memcpy((void *)&SndBuf[28], &DevStat.acnt_id[0], 2);
//            Rs232BCCWrite(SndBuf, 30);
//
//            SetBell(OFF);   //�ط�����
//            break;
//
//            ////////////////////////////////////////
//            // �������ݺ�����(������Ĺػ����Զ����)
//            ////////////////////////////////////////
//        case  'D':
//            SetBell(ON);    //��������
//            DevStat.POS_type = POS_T_ABNORMAL;
//
//            memcpy((void *)SndBuf, (void *)RcvBuf, 7);
//            SndBuf[0] = 0x20;
//            SndBuf[1] = 0x00;
//            Rs232BCCWrite(SndBuf, 7);
//            SetBell(OFF);   //�ط�����
//            break;
//
//        case  'K':
//            SetBell(ON);    //��������
//
//            if ( RcvBuf[4] == POS_T_NORMAL )              //�������Ͳ����
//            {
//                DevStat.POS_type = POS_T_NORMAL;
//
//                memcpy((void *)SndBuf, (void *)RcvBuf, 7);
//                SndBuf[0] = 0x20;
//                SndBuf[1] = 0x00;
//                Rs232BCCWrite(SndBuf, 7);
//            }
//
//            SetBell(OFF);   //�ط�����
//            break;
//
//            ////////////////////////////////////////
//            // ���ݺ�ʱ�����(change time from computer)
//            ////////////////////////////////////////
//        case  'T':
//            SetBell(ON);    //��������
//
//            date.fields.year = bcdhex(RcvBuf[4]) * 100 + bcdhex(RcvBuf[5]);
//            date.fields.month = bcdhex(RcvBuf[6]);
//            date.fields.day = bcdhex(RcvBuf[7]);
//
//            time2.fields.hour = bcdhex(RcvBuf[8]);
//            time2.fields.minute = bcdhex(RcvBuf[9]);
//            time2.fields.second = bcdhex(RcvBuf[10]);
//
//            // �޸ĵ�ǰϵͳʱ��
//            (void)RTC_set_date(date.l_word);
//            (void)RTC_set_time(time2.l_word);
//
//            if(time_okay_3511() == 0)
//            init_3511();
//
//            if(time_okay_3511() != 0)
//            {
//                date.l_word = RTC_read_date();
//                time2.l_word    = RTC_read_time();
//                set_all_time_for_3511(date, time2);
//            }
//
//            memcpy((void *)SndBuf, (void *)RcvBuf, 12);
//            SndBuf[0] = 0x20;
//            SndBuf[1] = 0x00;
//            Rs232BCCWrite(SndBuf, 12);
//
//            SetBell(OFF);   //�ط�����
//            break;
//
//            ////////////////////////////////////////
//            // ��ѯ���ݺ������ļ�����
//            ////////////////////////////////////////
//        case 'R':
//            SetBell(ON);    //��������
//
//            memset((void *)SndBuf, 0x00, sizeof(SndBuf));
//            memcpy((void *)SndBuf, "\x20\x00", 2);
//
//            if ( DevStat.POS_type == POS_T_NORMAL && DB_count_records(DB_REC) == 0 )          //�����У�û�м�¼
//            {
//                SndBuf[2] = 0x00;           //�ļ�����
//                SndBuf[3] = 0x35;       //???
//                SndBuf[5] = DevStat.POS_type;
//                SndBuf[15] = DevStat.POS_type;
//                SndBuf[16] = 0x00;          //�ļ�����
//                Rs232BCCWrite(SndBuf, 17);
//                SetBell(OFF);   //�ط�����
//                break;
//            }
//
//            SndBuf[2] = 0x01;           //�ļ�����
//            SndBuf[3] = 0x35;       //???
//            SndBuf[5] = DevStat.POS_type;
//            SndBuf[15] = DevStat.POS_type;
//            SndBuf[16] = 0x01;          //�ļ�����
//
//            // ���ļ�����ͨ�����ݻ�����
//            //����ǰ�����ļ��������ļ��м�¼����
//            switch ( DevStat.POS_type )
//            {
//                case POS_T_NORMAL:          //�����ɼ�
//                    temp_int32u = DB_count_records(DB_REC);
//                    SndBuf[17 + 15] = (INT8U)(temp_int32u >> 8);            //��¼����
//                    SndBuf[17 + 16] = (INT8U)temp_int32u;
//                    break;
//
//                case POS_T_ABNORMAL:            //�쳣�ɼ�
//                    temp_int32u = DB_count_records(DB_REC) + DB_count_records(DB_REC_BAK)
//                                  + DB_count_records(DB_REC_HISTORY);
//                    SndBuf[17 + 15] = (INT8U)(temp_int32u >> 8);            //��¼����
//                    SndBuf[17 + 16] = (INT8U)temp_int32u;
//                    break;
//
//            }
//
//            (void)Get_Time(&time);
//
//            SndBuf[17] = 0x01;                              // POS���ļ����
//            SndBuf[18] = DevStat.POS_type;          // ���ݺ�����
//            //SndBuf[19] = DevStat.line_number[0];  //��·��
//            //SndBuf[20] = DevStat.line_number[1];
//            //SndBuf[21] = DevStat.bus_number[0];       //����
//            //SndBuf[22] = DevStat.bus_number[1];
//            SndBuf[19] = DevStat.acnt_id[0];    //��·��
//            SndBuf[20] = DevStat.acnt_id[1];
//            SndBuf[21] = DevStat.equ_id[0];     //����
//            SndBuf[22] = DevStat.equ_id[1];
//            SndBuf[23] = 0x00;                                  // ����Ա��
//            SndBuf[24] = 0x00;                                  // ����Ա��
//
//            memcpy((void *)&SndBuf[25], (void *)&time, 7);          //����ʱ��
//
//            memcpy((void *)&SndBuf[34], (void *)&DevStat.pos_number[0], 6);         //pos��
////			SndBuf[40] = DevStat.bus_number[2];		// �������λ
//            SndBuf[40] = DevStat.equ_id[2];     // �������λ
//
//
//            Rs232BCCWrite(SndBuf, 41);
//
//            SetBell(OFF);   //�ط�����
//            break;
//
//
//            ////////////////////////////////////////
//            // ��ȡ���ݺ���ָ����ŵĺ�������Ϣ
//            ////////////////////////////////////////
//        case 'X':
//            SetBell(ON);    //��������
//            temp_int32u = DB_count_records(DB_BLACK);
//            black_ptr = ((INT32U)RcvBuf[4] << 8) + (INT32U)RcvBuf[5];           //Ҫ��ȡ�ĺ���������
//
//            if ( black_ptr >= temp_int32u )         //Ҫȡ�ĺ������������ں���������
//            {
//                memset((void *)SndBuf, 0x00, sizeof(SndBuf));
//                memcpy((void *)SndBuf, "\x02\x00", 2);
//            }
//            else
//            {
//                memset((void *)SndBuf, 0x00, sizeof(SndBuf));
//                memcpy((void *)SndBuf, "\x20\x00", 2);
//                GetBlackName((void *)&SndBuf[5], (INT16U)black_ptr);
//            }
//
//            SndBuf[2] = 'X';
//            SndBuf[3] = (INT8U)temp_int32u;
//            SndBuf[4] = (INT8U)(temp_int32u >> 8);
//
//            Rs232BCCWrite(SndBuf, 9);
//
//            SetBell(OFF);   //�ط�����
//            break;
//
//            ////////////////////////////////////////
//            // ��պ�����(Clear BlackName)
//            ////////////////////////////////////////
//        case  'J':
//            SetBell(ON);    //��������
//
//            DispStr_CE(0,2,"���غ�����",DISP_CENTER|DISP_CLRSCR);
//
//            // �����������
//            (void)DB_erase_filesys(DB_BLACK);
//
//            memcpy((void *)SndBuf, (void *)RcvBuf, 7);
//            SndBuf[0] = 0x20;
//            SndBuf[1] = 0x00;
//            Rs232BCCWrite(SndBuf, 4);
//
//            SetBell(OFF);   //�ط�����
//            break;
//
//            ////////////////////////////////////////
//            // ���غ�����(Add blackname)
//            ////////////////////////////////////////
//        case 'G':
//            DB_add_record(DB_BLACK, &RcvBuf[4]);
//
//            memcpy((void *)SndBuf, (void *)RcvBuf, 9);
//            SndBuf[0] = 0x20;
//            SndBuf[1] = 0x00;
//            Rs232BCCWrite(SndBuf, 9);
//
//            break;
//
//            ////////////////////////////////////////
//            // �����ɼ�ָ���ļ�
//            ////////////////////////////////////////
//        case 'Z':
//            DispStr_CE(0,2,"��¼������",DISP_CENTER|DISP_CLRSCR);
//
//            memcpy((void *)SndBuf, (void *)RcvBuf, 9);
//            SndBuf[0] = 0x20;
//            SndBuf[1] = 0x00;
//
//            SndBuf[13] = RcvBuf[6] + 1;
//
//            //SndBuf[15] = DevStat.line_number[0];  //��·��
//            //SndBuf[16] = DevStat.line_number[1];
//            //SndBuf[17] = DevStat.bus_number[0];       //����
//            //SndBuf[18] = DevStat.bus_number[1];
//            SndBuf[15] = DevStat.acnt_id[0];    //��·��
//            SndBuf[16] = DevStat.acnt_id[1];
//            SndBuf[17] = DevStat.equ_id[0];     //����
//            SndBuf[18] = DevStat.equ_id[1];
//            SndBuf[19] = 0x00;                                  // ����Ա��
//            SndBuf[20] = 0x00;                                  // ����Ա��
//
//            (void)Get_Time(&time);
//            memcpy((void *)&SndBuf[21], (void *)&time, 7);          //����ʱ��
//
//            memcpy((void *)&SndBuf[28], (void *)&DevStat.pos_number[0], 6);         //pos��
//            //SndBuf[34] = DevStat.bus_number[2];       // �������λ
//            SndBuf[18] = DevStat.equ_id[2];
//
//// 			rec_num = DB_count_records(DB_REC);
//
//            switch ( DevStat.POS_type )
//            {
//                case POS_T_NORMAL:          //�����ɼ�
//                    rec_num = DB_count_records(DB_REC);
//                    break;
//
//                case POS_T_ABNORMAL:            //�쳣�ɼ�
//                    db_rec_num = DB_count_records(DB_REC);
//                    db_rec_bak_num = DB_count_records(DB_REC_BAK);
//                    db_rec_history_num = DB_count_records(DB_REC_HISTORY);
//                    rec_num = db_rec_num + db_rec_bak_num + db_rec_history_num;
//                    break;
//
//            }
//
//            Rs232BCCWrite(SndBuf, 35);
//            break;
//
//            ////////////////////////////////////////
//            // ��¼�ϴ�
//            ////////////////////////////////////////
//        case    'I':
//            cur_trans_rec_num = ((INT32U)RcvBuf[5] << 8) + (INT32U)RcvBuf[6];
//
//            sprintf((void *)buf,"����%5.2f%%", (FP32)cur_trans_rec_num / (FP32)rec_num * 100.0);
//            EA_vDisp(4,1,buf);
//
//            if ( cur_trans_rec_num >= rec_num )           //��¼ȫ���������
//            {
//                EA_vDisp(2,1,"��¼�������");
//
//                SndBuf[0] = 0x20;
//                SndBuf[1] = 0x00;
//                SndBuf[2] = 0x03;           //���ݰ�����
//                SndBuf[3] = 'E';
//                SndBuf[4] = RcvBuf[5];      //��¼����
//                SndBuf[5] = RcvBuf[6];
//
//                //Rs232BCCWrite(SndBuf, 7);
//                break;
//            }
//
//            SndBuf[0] = 0x20;
//            SndBuf[1] = 0x00;
//            SndBuf[2] = 57 + 3;         //���ݰ�����
//            SndBuf[3] = 'I';
//            SndBuf[4] = RcvBuf[5];      //��¼����
//            SndBuf[5] = RcvBuf[6];
//
//            switch ( DevStat.POS_type )
//            {
//                case POS_T_NORMAL:
//                default:
//                    p = (INT8U *)DB_jump_to_record(DB_REC, cur_trans_rec_num, &flag);   //ȡ��¼
//                    //memcpy((void *)&SndBuf[6], p, 32);
//
//                    //Rs232BCCWrite(SndBuf, 32 + 7);
//                    memcpy((void *)&SndBuf[6], p, 57);
//
//                    Rs232BCCWrite(SndBuf, 57 + 7);
//                    break;
//
//                case POS_T_ABNORMAL:        //�쳣����
//                    if (cur_trans_rec_num < db_rec_num)
//                    {
//                        p = (INT8U *)DB_jump_to_record(DB_REC, cur_trans_rec_num, &flag);   //ȡ��¼
//                    }
//                    else if (cur_trans_rec_num < db_rec_bak_num)
//                    {
//                        p = (INT8U *)DB_jump_to_record(DB_REC, cur_trans_rec_num - db_rec_num, &flag);  //ȡ��¼
//                    }
//                    else
//                    {
//                        p = (INT8U *)DB_jump_to_record(DB_REC, cur_trans_rec_num - db_rec_num - db_rec_bak_num, &flag); //ȡ��¼
//                    }
//
//                    //memcpy((void *)&SndBuf[6], p, 32);
//
//                    //Rs232BCCWrite(SndBuf, 32 + 7);
//                    memcpy((void *)&SndBuf[6], p, 57);
//
//                    Rs232BCCWrite(SndBuf, 57 + 7);
//                    break;
//            }
//            break;
//
//            ////////////////////////////////////////
//            // ������ݺУ��ڴ���Ϊ�ɼ�������
//            ////////////////////////////////////////
//        case  'F':
//// 			SetBell(ON);	//��������
//
//            memcpy((void *)SndBuf, (void *)RcvBuf, 4);
//            SndBuf[0] = 0x20;
//            SndBuf[1] = 0x00;
//
//            Rs232BCCWrite(SndBuf, 4);
//
//            if ( DB_count_records(DB_REC_BAK) >
//                 DB_capacity(DB_REC_HISTORY) - DB_count_records(DB_REC_HISTORY) )
//            {   //��ʷ��¼���ݿ����������ˣ����
//                DispStr_CE(0,2,"�����ʷ���ݿ�",DISP_CENTER|DISP_CLRSCR);
//                (void)DB_erase_filesys(DB_REC_HISTORY);
//            }
//
//            //db_rec_bak -> db_rec_history
//            DispStr_CE(0,2,"д��ʷ���ݿ�",DISP_CENTER|DISP_CLRSCR);
//            rec_num = DB_count_records(DB_REC_BAK);
//            for ( temp_int32u = 0; temp_int32u < rec_num; temp_int32u++ )
//            {
//                DB_add_record( DB_REC_HISTORY, DB_jump_to_record(DB_REC_BAK, temp_int32u, &flag) );
//                sprintf(buf,"����%5.2f%%", ((FP32)temp_int32u + 1.0) / (FP32)rec_num * 100.0);
//                DispStr_CE(0,4,buf,DISP_CENTER|DISP_CLRSCR);
//            }
//
//            //db_rec -> db_rec_bak
//            DispStr_CE(0,2,"д�����������ݿ�",DISP_CENTER|DISP_CLRSCR);
//            (void)DB_erase_filesys(DB_REC_BAK);
//            rec_num = DB_count_records(DB_REC);
//            for ( temp_int32u = 0; temp_int32u < rec_num; temp_int32u++ )
//            {
//                DB_add_record( DB_REC_BAK, DB_jump_to_record(DB_REC, temp_int32u, &flag) );
//                sprintf(buf,"����%5.2f%%", ((FP32)temp_int32u + 1.0) / (FP32)rec_num * 100.0);
//                DispStr_CE(0,4,buf,DISP_CENTER|DISP_CLRSCR);
//            }
//
//            DispStr_CE(0,2,"��������ɼ����ݿ�",DISP_CENTER|DISP_CLRSCR);
//            (void)DB_erase_filesys(DB_REC);
//
//            DispStr_CE(0,2,"���",DISP_CENTER|DISP_CLRSCR);
//            Beep(100);
//            SleepMs(100);
//            Beep(100);
//
//            break;
//
//
//
//        default:    //do nothing
//            break;
//    }
//
//// 	DevStat.databox_cmd_rcvd = FALSE;
//    RcvdLength = 0;                         //����ջ�����
//    ClrUART();
//// 	i = SBUF;			//����ջ�����
//// 	RI = 0;
//// 	EnableSerialINT();
//
//
//}
//
//INT8U InitParam(void)
//{
//
////  SELE_INLINE KS_Setting;
////	int ret;
//    INT16U key;
////  EA_ucClrKeyBuf();
//    EA_vDisp(1, 1, "   ϵͳ���ָ�����   ");
//    EA_vDisp(2, 1, "  ��״̬, ���м�¼  ");
//    EA_vDisp(3, 1, "  �Ͳ���ȫ�������  ");
//    EA_vDisp(4, 1, "ȷ��            ȡ��");
//
//
////    KS_Setting.count = 2;   //�˵�����
////    KS_Setting.line = 6;    //�˵��к�
//////	KS_Setting.enable46 = 1; //ʹ��4��6�����������ʹ��4��6�����滬��
//////	KS_Setting.flagch = 0;	 //��ǰѡ����ı�־�ַ�������
////    KS_Setting.defsele = 0; // Ĭ��ѡ�񡰹رա�
////    KS_Setting.autoexit = 30; // 30���ް����Զ��˳�
////
////    KS_Setting.selepstr[0].xx = 1;  //��һ�˵�λ��
////    strcpy(KS_Setting.selepstr[0].pstr, "��" );
////    KS_Setting.selepstr[1].xx = 8;  //�ڶ��˵�λ��
////    strcpy( KS_Setting.selepstr[1].pstr, "��" );
////
////    KS_Setting.qEvent = EXIT_KEY_F1 | EXIT_KEY_COMM|EXIT_KEY_POWER; // ��[C]����Դ�����ް����Զ��˳�
//
////ret = select_in_line( &KS_Setting );
//    key = EA_uiInkey(0);
//    switch ( key )
//    {
////  	case EM_key_ENTER:	// ��
////  		return notok;
//        case EM_key_ENTER:  // ��
////  		EA_ucClrKeyBuf();
//            EA_vDisp(1, 1, "ϵͳ���ڳ�ʼ��");
//            EA_vDisp(2, 1, "��ȴ�....");
//
////  		init_db();
////  		//��ʼ������
////		   InitImportantParameter();
//
////  		(void)EA_vCls();
////			DispStr_CE(0, 2, "ϵͳ��ʼ�����", DISP_CENTER);
//// 			DispStr_CE(0, 4, "�����Զ��ػ�", DISP_CENTER);
////			DispStr_CE(0, 4, "�����Զ�����", DISP_CENTER);
////			Beep(1000);
//// 			(void)delay_and_wait_key(5,0,0);
////			SleepMs(1000);
////			resetpos();
////  		_start();
//            param_factory_default(1);
//            return ok;
//        default:
//            return notok;
//    }
//
//}

// *****************************************************************
// ���ܣ�		OldManSetting
// ˵��:		���꿨�ۿ�����
// ��ڲ�����	
// ���ڲ�����	Null
// ���ߣ�
// ����ֵ��							
// *****************************************************************
INT8U OldManSetting(void)
{
	(void)EA_ucClrKeyBuf();
	EA_vDisp(1,1,"���꿨�ڱ���·");
	EA_vDisp(2,1,"��������Ѵ���");
	EA_vDisp(3,1,"�������ñ�����");
	EA_vDisp(4,1,"����������˳�");

//  delay_and_wait_key(5, 0, 0);
	EA_uiInkey(5);
	
	return 1;


	
/*	SELE_INLINE KS_Setting;
	int ret;
	

	DispStr_CE(0,0,"���꿨�ۿ��趨",DISP_CENTER|DISP_CLRSCR);

	KS_Setting.count = 2;	//�˵�����
	KS_Setting.line = 2;	//�˵��к�
	KS_Setting.enable46 = 1; //ʹ��4��6�����������ʹ��4��6�����滬��
	KS_Setting.flagch = 0;	 //��ǰѡ����ı�־�ַ�������
	if (DevStat.oldman_free == TRUE)
		KS_Setting.defsele = 1;	// Ĭ��ѡ�񡰲��ۿ 
	else
		KS_Setting.defsele = 0;	// Ĭ��ѡ�񡰿ۿ 

	KS_Setting.autoexit = 30; // 30���ް����Զ��˳�

	KS_Setting.selepstr[0].xx = 25;	//��һ�˵�λ��
	strcpy(KS_Setting.selepstr[0].pstr, "�ۿ�" ); 
	KS_Setting.selepstr[1].xx = 72;	//�ڶ��˵�λ��
	strcpy( KS_Setting.selepstr[1].pstr, "���ۿ�" ); 

	KS_Setting.qEvent = EXIT_KEY_F1 | EXIT_KEY_CLR|EXIT_KEY_POWER|EXIT_AUTO_QUIT; // ��[C]����Դ�����ް����Զ��˳� 
	ret = select_in_line( &KS_Setting ); 
	switch ( ret )
	{
		case 0:	// ѡ��ۿ�� 
			DevStat.oldman_free = FALSE;
			WriteParam();
			break; 

		case 1:	// ѡ�񲻿ۿ��
			DevStat.oldman_free = TRUE;
			WriteParam();
			break; 

		default: 
			if ( KS_Setting.qEvent = EXIT_KEY_F1 ) // ��[F1]���˳� 
				return 1;
			else if ( KS_Setting.qEvent = EXIT_KEY_POWER ) // ����Դ���˳� 
				return -1;
			else if ( KS_Setting.qEvent = EXIT_AUTO_QUIT ) // 30���ް����Զ��˳�
				return 1;
			break;
	}

	return 1;
*/
}


// *****************************************************************
// ���ܣ�		EmployeeASetting
// ˵��:		�������ۿ�����
// ��ڲ�����	
// ���ڲ�����	Null
// ���ߣ�
// ����ֵ��							
// *****************************************************************
//INT8U EmployeeASetting(void)
//{
/*
	SELE_INLINE KS_Setting;
	int ret;

	DispStr_CE(0,0,"��ѿ��ۿ��趨",DISP_CENTER|DISP_CLRSCR);

	KS_Setting.count = 2;	//�˵�����
	KS_Setting.line = 2;	//�˵��к�
	KS_Setting.enable46 = 1; //ʹ��4��6�����������ʹ��4��6�����滬��
	KS_Setting.flagch = 0;	 //��ǰѡ����ı�־�ַ�������
	if (DevStat.employee_a_free == TRUE)
		KS_Setting.defsele = 1;	// Ĭ��ѡ�񡰲��ۿ 
	else
		KS_Setting.defsele = 0;	// Ĭ��ѡ�񡰿ۿ 

	KS_Setting.autoexit = 30; // 30���ް����Զ��˳�

	KS_Setting.selepstr[0].xx = 25;	//��һ�˵�λ��
	strcpy(KS_Setting.selepstr[0].pstr, "�ۿ�" ); 
	KS_Setting.selepstr[1].xx = 72;	//�ڶ��˵�λ��
	strcpy( KS_Setting.selepstr[1].pstr, "���ۿ�" ); 

	KS_Setting.qEvent = EXIT_KEY_F1 | EXIT_KEY_CLR|EXIT_KEY_POWER|EXIT_AUTO_QUIT; // ��[C]����Դ�����ް����Զ��˳� 
	ret = select_in_line( &KS_Setting ); 
	switch ( ret )
	{
		case 0:	// ѡ��ۿ�� 
			DevStat.employee_a_free = FALSE;
			WriteParam();
			break; 

		case 1:	// ѡ�񲻿ۿ��
			DevStat.employee_a_free = TRUE;
			WriteParam();
			break; 

		default: 
			if ( KS_Setting.qEvent = EXIT_KEY_F1 ) // ��[F1]���˳� 
				return 1;
			else if ( KS_Setting.qEvent = EXIT_KEY_POWER ) // ����Դ���˳� 
				return -1;
			else if ( KS_Setting.qEvent = EXIT_AUTO_QUIT ) // 30���ް����Զ��˳�
				return 1;
			break;
	}
*/
//    return 1;
//}

// *****************************************************************
// ���ܣ�		FreePriceSetting
// ˵��:		��ѿ�Ʊ������
// ��ڲ�����	
// ���ڲ�����	Null
// ���ߣ�
// ����ֵ��							
// *****************************************************************
//INT8U FreePriceSetting(void)
//{
/*	
	INT8U i = 0;
//	INT8U  PriceSettingMenu[10*16 +2] =  {0};
	INT8U buf[100];
	int 			ret;
//	int 			cPtr=0;
//	int 			lPtr=0;
	INT8U disp_buf[10];
	INT8U key = NO_KEY;
	const INT8U position[6][2] =								   //��λ��ʾ��λ��
	{
		{4 * 8, 3},
		{5 * 8, 3},
		{6 * 8, 3},
		{7 * 8, 3},
		{9 * 8, 3},
		{10 * 8, 3},
	};
	INT8S cur_ptr = 0;

	ToggleWD();

	DispStr_CE(0, 1, "��ѿ�Ʊ������", DISP_CLRSCR | DISP_CENTER);

	sprintf(buf, "    %07.2fԪ   ", (FP64)DevStat.free_price/ 100.0);
	DispStr_CE(0, 3, buf, DISP_POSITION);


	disp_buf[0] = ascii(DevStat.free_price / 100000);						//ǧλ
	disp_buf[1] = ascii((DevStat.free_price % 100000) / 10000);	//��λ
	disp_buf[2] = ascii((DevStat.free_price % 10000) / 1000);		//ʮ
	disp_buf[3] = ascii((DevStat.free_price % 1000) / 100);			//��
	disp_buf[4] = ascii((DevStat.free_price % 100) / 10);				//ë
	disp_buf[5] = ascii(DevStat.free_price % 10);							//��

	cur_ptr = 0;
	DispStr_CE(position[cur_ptr][0], position[cur_ptr][1], chtostr(disp_buf[0]), DISP_POSITION | DISP_REVERSE);


	for ( ;; )
	{
		do
		{				  //only accept 0-9,KEY_CANCEL, KEY_ENTER
			key = get_key();
		}while ( !( (key>='0'&&key<='9') || (key==EXIT_KEY_F1) || (key==EXIT_KEY_COMM) || (key==EXIT_KEY_ENT) ) );

		switch ( key )
		{
			case EXIT_KEY_COMM:
				break;

			case EXIT_KEY_F1:
				return notok;			//���޸�ʱ�䣬����

			case EXIT_KEY_ENT:
					DevStat.free_price = (INT32U)ascii_to_hex(disp_buf[0]) * 100000 + (INT32U)ascii_to_hex(disp_buf[1]) * 10000 
							+ (INT32U)ascii_to_hex(disp_buf[2]) * 1000 + (INT32U)ascii_to_hex(disp_buf[3]) * 100 
							+ (INT32U)ascii_to_hex(disp_buf[4]) * 10 + (INT32U)ascii_to_hex(disp_buf[5]);
					WriteParam();
					return ok;

			default:			//���ּ�����
				disp_buf[cur_ptr] = key;
				DispStr_CE(position[cur_ptr][0], position[cur_ptr][1], chtostr(disp_buf[cur_ptr]), DISP_POSITION);
				cur_ptr++;
				if ( cur_ptr >= 6 )
				{
					cur_ptr = 0;
				}
				DispStr_CE(position[cur_ptr][0], position[cur_ptr][1], chtostr(disp_buf[cur_ptr]), DISP_POSITION | DISP_REVERSE);
//				DispStr_CE(position[cur_ptr][0], position[cur_ptr][1], "_", DISP_POSITION);
				break;
		}
	}

*/
//    return ok;
//}

// *****************************************************************
// ���ܣ�		DriverSetting
// ˵��:		Ա�����ۿ�����
// ��ڲ�����	
// ���ڲ�����	Null
// ���ߣ�
// ����ֵ��							
// *****************************************************************
INT8U DriverSetting(void)
{
	(void)EA_ucClrKeyBuf();
	EA_vDisp(1,1,"Ա�����ڱ���·");
	EA_vDisp(2,1,"��������Ѵ���");
	EA_vDisp(3,1,"�������ñ�����");
	EA_vDisp(4,1,"����������˳�");

//  delay_and_wait_key(5, 0, 0);
	EA_uiInkey(5);
	
	return 1;

/*	
	SELE_INLINE KS_Setting;
	int ret;

	DispStr_CE(0,0,"Ա�����ۿ��趨",DISP_CENTER|DISP_CLRSCR);

	KS_Setting.count = 2;	//�˵�����
	KS_Setting.line = 2;	//�˵��к�
	KS_Setting.enable46 = 1; //ʹ��4��6�����������ʹ��4��6�����滬��
	KS_Setting.flagch = 0;	 //��ǰѡ����ı�־�ַ�������
	if (DevStat.driver_free == TRUE)
		KS_Setting.defsele = 1;	// Ĭ��ѡ�񡰲��ۿ 
	else
		KS_Setting.defsele = 0;	// Ĭ��ѡ�񡰿ۿ 

	KS_Setting.autoexit = 30; // 30���ް����Զ��˳�

	KS_Setting.selepstr[0].xx = 25;	//��һ�˵�λ��
	strcpy(KS_Setting.selepstr[0].pstr, "�ۿ�" ); 
	KS_Setting.selepstr[1].xx = 72;	//�ڶ��˵�λ��
	strcpy( KS_Setting.selepstr[1].pstr, "���ۿ�" ); 

	KS_Setting.qEvent = EXIT_KEY_F1 | EXIT_KEY_CLR|EXIT_KEY_POWER|EXIT_AUTO_QUIT; // ��[C]����Դ�����ް����Զ��˳� 
	ret = select_in_line( &KS_Setting ); 
	switch ( ret )
	{
		case 0:	// ѡ��ۿ�� 
			DevStat.driver_free = FALSE;
			WriteParam();
			break; 

		case 1:	// ѡ�񲻿ۿ��
			DevStat.driver_free = TRUE;
			WriteParam();
			break; 

		default: 
			if ( KS_Setting.qEvent = EXIT_KEY_F1 ) // ��[F1]���˳� 
				return 1;
			else if ( KS_Setting.qEvent = EXIT_KEY_POWER ) // ����Դ���˳� 
				return -1;
			else if ( KS_Setting.qEvent = EXIT_AUTO_QUIT ) // 30���ް����Զ��˳�
				return 1;
			break;
	}

	return 1;
*/
}


// *****************************************************************
// ���ܣ�		ModeSetting
// ˵��:		����ģʽ����
// ��ڲ�����	
// ���ڲ�����	Null
// ���ߣ�
// ����ֵ��							
// *****************************************************************

//INT8U ModeSetting(void)
//{
/*	SELE_INLINE KS_Setting;
	int ret;

	DispStr_CE(0,0,"����ģʽ�趨",DISP_CENTER|DISP_CLRSCR);

	KS_Setting.count = 2;	//�˵�����
	KS_Setting.line = 2;	//�˵��к�
	KS_Setting.enable46 = 1; //ʹ��4��6�����������ʹ��4��6�����滬��
	KS_Setting.flagch = 0;	 //��ǰѡ����ı�־�ַ�������
	if (DevStat.mode == NORMAL)
		KS_Setting.defsele = 0;	// ѡ�������� 
	else
		KS_Setting.defsele = 1;	// ѡ�񡰿�ݡ� 

	KS_Setting.autoexit = 30; // 30���ް����Զ��˳�

	KS_Setting.selepstr[0].xx = 25;	//��һ�˵�λ��
	strcpy(KS_Setting.selepstr[0].pstr, "����" ); 
	KS_Setting.selepstr[1].xx = 72;	//�ڶ��˵�λ��
	strcpy( KS_Setting.selepstr[1].pstr, "���" ); 

	KS_Setting.qEvent = EXIT_KEY_F1 | EXIT_KEY_CLR|EXIT_KEY_POWER|EXIT_AUTO_QUIT; // ��[C]����Դ�����ް����Զ��˳� 
	ret = select_in_line( &KS_Setting ); 
	switch ( ret )
	{
		case 0:	// ѡ���������� 
			DevStat.mode = NORMAL;
			WriteParam();
			break; 

		case 1:	// ѡ���ݴ���
			DevStat.mode = SHORTCUT;
			WriteParam();
			break; 

		default: 
			if ( KS_Setting.qEvent = EXIT_KEY_F1 ) // ��[F1]���˳� 
				return 1;
			else if ( KS_Setting.qEvent = EXIT_KEY_POWER ) // ����Դ���˳� 
				return -1;
			//else if ( KS_Setting.qEvent = AUTO_QUIT ) // 30���ް����Զ��˳�
			//	return 1;
			break;
	}
*/
//    return 1;
//}

// *****************************************************************
// ���ܣ�		PHoldTime_Setting
// ˵��:		ˢ������ʱ������
// ��ڲ�����	
// ���ڲ�����	Null
// ���ߣ�
// ����ֵ��		-1:�ػ�					
// *****************************************************************
//int PHoldTime_Setting()
//{
/*
	char strbuf[50];
	int ret;
	long ll_input=0;
	int cPtr = 0;
	int lPtr = 0;
	GETSTR_PARAM gs;
// 	char    buf[50];//�����ַ��������

	EA_ucClrKeyBuf();

// 	DispStr_CE(0,0,"�Ƽ�����ʱ��Ϊ60",DISP_CENTER|DISP_CLRSCR);
	DispStr_CE(0,2,"ˢ������ʱ��(��)",DISP_CENTER);
	DispStr_CE(0,4,"��Χ:1 - 255��",DISP_CENTER);
//     sprintf(strbuf,"%d",30);

	sprintf(strbuf, "%d", DevStat.purchase_hold_time);

	gs.qx = 0;		//��ʾ�����ʾ����
	gs.qy = 6;

	gs.alpha = 0;	//����ģʽ��־�Ƿ�����������ĸ�����ţ�Ϊ0������������ĸ�����ţ���Ϊ0������������ĸ������
	gs.sFont = 0;	//��־��ʾ�����С��Ϊ 0 ��ʾʹ��7x9�Ĵ����壬 ��Ϊ 0 ���ʾʹ��5x7��С����
	gs.pPrompt = "����ʱ��:";
	gs.csChar = '_';
	gs.pwChar = 0;
	gs.nullEnable = 0;
// 	gs.spChar = ',';
	gs.spChar = 0;
	gs.pStr = strbuf;			//	��������ַ��洢�ڻ�����strbuf��
	gs.maxNum = 3;
	gs.minNum = 1;
	gs.minValue = 1;
	gs.maxValue = 255;
	if (DevStat.purchase_hold_time < 10)
	{
		gs.retNum = 1;			//��ʼ��ʾ1λ
	}
	else if (DevStat.purchase_hold_time < 100)
	{
		gs.retNum = 2;			//��ʼ��ʾ2λ
	}
	else if (DevStat.purchase_hold_time < 1000)
	{
		gs.retNum = 3;			//��ʼ��ʾ3λ
	}
	else if (DevStat.purchase_hold_time < 10000)
	{
		gs.retNum = 4;			//��ʼ��ʾ4λ
	}
	else
	{
		gs.retNum = 5;			//��ʼ��ʾ5λ
	}
	gs.qEvent = KEY_F1 | KEY_POWER; // Ĭ��F1�˳�
// 	sprintf(strbuf,"%d",30);

	ret = huGetStr_E(&gs);
	switch ( ret )
	{
		case 0:	//��ȷ���롣 
			DevStat.purchase_hold_time = atol(strbuf);
			WriteParam();
			return 1;

		case 1:// ������ַ���
			DispStr_CE(0,3,"��������",DISP_CENTER|DISP_CLRSCR);
			Beep(500);
			return -1;

		case -1://�¼��˳����ַ���δ��ȷ���룩��
			break;

		default:
			break;          
	}
*/
//    return 1;
//}

// *****************************************************************
// ���ܣ�		MaxPriceLimitSet
// ˵��:		����������ѽ��
// ��ڲ�����	
// ���ڲ�����	Null
// ���ߣ�
// ����ֵ��							
// *****************************************************************
//INT8U MaxPriceLimitSet(void)
//{
//    INT8U i = 0;
//// 	INT8U  PriceSettingMenu[10*16 +2] =  {0};
//    INT8U buf[100];
//    int             ret;
//// 	int             cPtr=0;
//// 	int             lPtr=0;
//    INT8U disp_buf[10];
//    INT8U key = NO_KEY;
//    const INT8U position[6][2] =                                   //��λ��ʾ��λ��
//    {
//        {4 * 8, 3},
//        {5 * 8, 3},
//        {6 * 8, 3},
//        {7 * 8, 3},
//        {9 * 8, 3},
//        {10 * 8, 3},
//    };
//    INT8S cur_ptr = 0;
//
//    ToggleWD();
//
//    DispStr_CE(0, 1, "������ѽ������", DISP_CLRSCR | DISP_CENTER);
//
//    sprintf(buf, "    %07.2fԪ   ", (FP64)DevStat.max_price_limit /100.0);
//    DispStr_CE(0, 3, buf, DISP_POSITION);
//
//
//    disp_buf[0] = ascii(DevStat.max_price_limit / 100000);                      //ǧλ
//    disp_buf[1] = ascii((DevStat.max_price_limit % 100000) / 10000);    //��λ
//    disp_buf[2] = ascii((DevStat.max_price_limit % 10000) / 1000);      //ʮ
//    disp_buf[3] = ascii((DevStat.max_price_limit % 1000) / 100);            //��
//    disp_buf[4] = ascii((DevStat.max_price_limit % 100) / 10);              //ë
//    disp_buf[5] = ascii(DevStat.max_price_limit % 10);                          //��
//
//    cur_ptr = 0;
//    DispStr_CE(position[cur_ptr][0], position[cur_ptr][1], chtostr(disp_buf[0]), DISP_POSITION | DISP_REVERSE);
//
//
//    for ( ;; )
//    {
//        do
//        {                 //only accept 0-9,KEY_CANCEL, KEY_ENTER
//            key = get_key();
//        }while ( !( (key>='0'&&key<='9') || (key==EXIT_KEY_F1) || (key==EXIT_KEY_COMM) || (key==EXIT_KEY_ENT) ) );
//
//        switch ( key )
//        {
//            case EXIT_KEY_COMM:
//                break;
//
//            case EXIT_KEY_F1:
//                return notok;           //���޸�ʱ�䣬����
//
//            case EXIT_KEY_ENT:
//                    DevStat.max_price_limit = (INT32U)ascii_to_hex(disp_buf[0]) * 100000 + (INT32U)ascii_to_hex(disp_buf[1]) * 10000
//                            + (INT32U)ascii_to_hex(disp_buf[2]) * 1000 + (INT32U)ascii_to_hex(disp_buf[3]) * 100
//                            + (INT32U)ascii_to_hex(disp_buf[4]) * 10 + (INT32U)ascii_to_hex(disp_buf[5]);
//                    WriteParam();
//                    return ok;
//
//            default:            //���ּ�����
//                disp_buf[cur_ptr] = key;
//                DispStr_CE(position[cur_ptr][0], position[cur_ptr][1], chtostr(disp_buf[cur_ptr]), DISP_POSITION);
//                cur_ptr++;
//                if ( cur_ptr >= 6 )
//                {
//                    cur_ptr = 0;
//                }
//                DispStr_CE(position[cur_ptr][0], position[cur_ptr][1], chtostr(disp_buf[cur_ptr]), DISP_POSITION | DISP_REVERSE);
//// 				DispStr_CE(position[cur_ptr][0], position[cur_ptr][1], "_", DISP_POSITION);
//                break;
//        }
//    }
//
//
//    return ok;
//}


// *****************************************************************
// ���ܣ�		PrintModeSelect
// ˵��:		��ӡģʽѡ��
// ��ڲ�����	
// ���ڲ�����	Null
// ���ߣ�
// ����ֵ��							
// *****************************************************************
int PrintModeSelect()
{

//	SELE_INLINE KS_Setting;
	int ret;
	INT8U Print_menus[2*20 + 2] = {"1.�Զ�              2.�ֶ�              "};
	

//  EA_vCls();
//  EA_vDisplay(1,"��ӡģʽѡ��");
	ret = browse_menu(0, (unsigned char*)"", Print_menus, TRUE);
	switch ( ret )
	{
		case 0: // ѡ���Զ�
			DevStat.mode = 0x00;
			WriteParam();
			break;

		case 1: // ѡ���ֶ�
			DevStat.mode = 0x01;
			WriteParam();
			break;

		default:
			return notok;
			break;
	}
	return ok;

}


// *****************************************************************
// ���ܣ�		InitParam
// ˵��:		�ָ���������
// ��ڲ�����	
// ���ڲ�����	Null
// ���ߣ�
// ����ֵ��							
// *****************************************************************
INT8U InitParam()
{
//	int ret;
	INT16U  key;  
		 
	EA_ucClrKeyBuf();
	EA_vCls();
	EA_vDisplay(1, "   ϵͳ���ָ�����   ");
	EA_vDisplay(2, "   ��״̬,���м�¼  ");
	EA_vDisplay(3, "  �Ͳ���ȫ�������  ");
	EA_vDisplay(4, "F1-ȷ��    ����-ȡ��");
	key = EA_uiInkey(0);

	switch ( key )
	{
		case EM_key_F1:	// ��
			EA_ucClrKeyBuf();
			EA_vCls();
			EA_vDisplay(1, "  ϵͳ���ڳ�ʼ��   ");
			EA_vDisplay(2, "     ��ȴ�....    ");
			EA_uiInkeyMs(2000);
			//��ʼ������
//  		format_process();
			param_factory_default(1);

			(void)EA_vCls();
			EA_vDisplay(1, "   ϵͳ��ʼ�����   ");
			EA_vDisplay(2, "    �����Զ��ػ�    ");
			EA_vDisplay(3, "    �����Զ�����    ");
			EA_uiInkeyMs(2000);
			Beep(1000);
			System_reset();
//  		Power_Down();
//  		return ok;
		default: 
			return notok;
	}



}



// *****************************************************************
// ���ܣ�		Sex_Select
// ˵��:		�Ա�ѡ��
// ��ڲ�����	
// ���ڲ�����	Null
// ���ߣ�
// ����ֵ��							
// *****************************************************************
//int Sex_Select()
//{
//
//    SELE_INLINE KS_Setting;
//    int ret;
//
//    EA_vCls();
//
//    DispStr_CE(0,0,"�Ա�ѡ��",DISP_CENTER|DISP_CLRSCR);
//
//    KS_Setting.count = 2;   //�˵�����
//    KS_Setting.line = 4;    //�˵��к�
//    //KS_Setting.enable46 = 1; //ʹ��4��6�����������ʹ��4��6�����滬��
//    //KS_Setting.flagch = 0;     //��ǰѡ����ı�־�ַ�������
//
//    KS_Setting.defsele = 0; // Ĭ��ѡ�񡰿�����
//    KS_Setting.autoexit = DevStat.poweroff_time; // 30���ް����Զ��˳�
//
//    KS_Setting.selepstr[0].xx = 2;  //��һ�˵�λ��
//    strcpy(KS_Setting.selepstr[0].pstr, "��" );
//    KS_Setting.selepstr[1].xx = 10; //�ڶ��˵�λ��
//    strcpy( KS_Setting.selepstr[1].pstr, "Ů" );
//
//    CardInfo.sex = 0;
//
//    KS_Setting.qEvent = EXIT_KEY_F1 | EXIT_KEY_COMM|EXIT_KEY_POWER; // ��[C]����Դ�����ް����Զ��˳�
//    ret = select_in_line( &KS_Setting );
//    switch ( ret )
//    {
//        case 0: // ѡ����
//            CardInfo.sex = 0x00;
//            break;
//
//        case 1: // ѡ��Ů
//            CardInfo.sex = 0x01;
//            break;
//
//        default:
//            return ret;
//            break;
//    }
//
//    return 1;
//
//}


// *****************************************************************
// ���ܣ�		Id_Select
// ˵��:		֤������ѡ��
// ��ڲ�����	
// ���ڲ�����	Null
// ���ߣ�
// ����ֵ��	-1 �ػ�							
// *****************************************************************
int Id_Select()
{
	int             ret;
	CardInfo.id_type = 1;

	//1:���֤
	//2:����֤
	//3:����֤
	//4:����֤
	//5:����֤
	//6:ѧ��֤
	for ( ; ; )
	{
		ret = browse_menu(0, (void *)"    ֤������    ", Id_Type, TRUE);
		switch ( ret )
		{
			case 0://���֤
			case EM_key_1:
				CardInfo.id_type = 1;
				return 0;
				break;

			case 1://����֤
			case EM_key_2:
				CardInfo.id_type = 2;
				return 1;
				break;

			case 2://����֤
			case EM_key_3:			
				CardInfo.id_type = 3;
				return 2;
				break;
			case 3://����֤
			case EM_key_4:			
				CardInfo.id_type = 4;
				return 4;
				break;
			case 4://����֤
			case EM_key_5:			
				CardInfo.id_type = 5;
				return 5;
				break;
			case 5://ѧ��֤
			case EM_key_6:			
				CardInfo.id_type = 6;
				return 6;
				break;
			case -1://EXIT_KEY_POWER,EXIT_AUTO_QUIT
//  			Halt_HD6305();
				return -1;

			case -5://�����ϼ��˵�
				return -5;

			default:
				break;
		}
	}
}





