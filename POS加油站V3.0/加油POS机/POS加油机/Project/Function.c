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
extern INT8U const    VERIFY_PIN[11];     //��֤����
extern INT8U const    READ_15_FILE[5];    //��ȡ15�ļ�����
extern INT8U const    WRITE_15_FILE[5];   //��ȡ15�ļ�����

extern	DevHandle  hSam1Handle;
extern	DevHandle  hCpuHandle;
extern  DevHandle hMifsHandle;      //M1��

extern const char parmFileName[];
extern const char blackFileName[];
extern const char currecFileName[];
extern const char conrecFileName[];
extern const char hisrecFileName[];
extern const char grayrecFileName[];

// *****************************************************************
// ���ܣ�		�������ý���
// *****************************************************************
INT8U  parm_set[10*20 +2] = {"1.�ͼ۵���          2.��Ʒ����          3.��¼��ѯ          4.�豸������        5.վ�������        6.�����޸�          7.�ش��ϴ�СƱ      8.IP��ַ����        9.�޸ļ���Ա������  10.�ͼ���Чʱ���趨 "};
INT8U  gas_set[4*20 +2]   = {"1.��Ȼ��            2.����93#           3.����97#           4.����              "}; 
INT8U  rec_query[2*20 +2] = {"1.�սᵥ��ѯ        2.���ʼ�¼��ѯ      "}; 
// *****************************************************************
// ���ܣ�		��Ϣ��ѯ����
// *****************************************************************

//ϵͳ��������
extern DEV_STAT     DevStat;					//�豸״̬
extern CARD_INFO    CardInfo;					//��Ƭ��Ϣ�����׼�¼�ɴ˱�������
extern ERROR_CARD_INFO ErrCardInfo;	//����״̬
extern INT8U sendbuf[1024];
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
// ���ܣ�		PasswordCheck
// ˵��:		�������
//				��ϵͳ���г���������û�����
// ��ڲ�����	
// ���ڲ�����	Null
// ���ߣ�
// ����ֵ�� -1 �ػ� 						
// *****************************************************************
int PasswordCheck(void)
{
	uchar i = 0;
	INT8U input[22];

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
int PasswordSetting(void)
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
		for ( ;; )
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

	switch ( PasswordCheck() )
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
		   case 0://�ͼ�����
 			    price_Setting();
				break;	
		   case 1://��Ʒ����
 			    select_gas_oil_Setting();
				break;
		   case 2://���׼�¼��ѯ
			    query_note_and_record();
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
		   case 6:  //�ش��ϴ�СƱ
			    Reprint_sale_record();
			    break;
		   case 7://APN����
			   if(GPRS_Input_IP() == ok)
			   {
				   EA_vCls();
				   EA_vDisplay(3, "   IP��ַ�޸ĳɹ�   ");
				   EA_uiInkeyMs(2000);
			   }
			   break;
		   case 8:
			   oper_code_setting();
			   break;
		   case 9:
//  		   DateTime_Setting(void);
			   break;

		   case -1://�����ϼ��˵�
				return -1;

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
	char    buf[50];
	int     i;   

	EA_vCls();
	EA_vDisplay(1, (void *)"�豸ID:%02X%02X%02X",DevStat.equ_id[0], DevStat.equ_id[1], DevStat.equ_id[2]);
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

	printf_debug((void *)"   �豸ID���óɹ�   ");

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

	sprintf((void *)str, "վ���:%02X%02X", DevStat.acnt_id[0], DevStat.acnt_id[1]);

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

	printf_debug((void *)"   վ������óɹ�   ");

	return ok;
}
////////////////////////////////////////////////////////////
//APN�����ѡ��
////////////////////////////////////////////////////////////
INT8U SAMCompSelect(void)
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
// ���ܣ�		GPRS_Input_IP
// ˵��:		�ֹ�����IP��ַ
// ��ڲ�����	
// ���ڲ�����	Null
// ���ߣ�
// ����ֵ��	-1 �ػ�							
// *****************************************************************
//void GPRS_Input_IP(void)
//{
//    INT8U buf[16], IP_addr_1[16], i, j;
//    INT8U iTmp[4], iport;
//    INT8U IP_addr[16];
////	IP_addr[16] = "219.133.55.78", Comm_port[16] = "7455"
//
//IP_AGAIN:
//    memset(IP_addr, 0, sizeof(IP_addr));
//    memset(IP_addr_1, 0, sizeof(IP_addr));
//
//    EA_vCls();
//    (void)EA_ucClrKeyBuf();
//    j = 0;
//    EA_vDisplay(1, "ԭIP��ַΪ:");
//    EA_vDisplay(2, "��������IP��ַ:");
//
//    sprintf(IP_addr, "%03d.%03d.%03d.%03d",DevStat.ServerIp[0], DevStat.ServerIp[1],
//            DevStat.ServerIp[2], DevStat.ServerIp[3]);
//    EA_vDisplay(3, IP_addr);
//
//    memcpy(IP_addr_1, "   .   .   .   ", 15);
//    IP_addr_1[15] = '\0';
////  if (ReadNumeric_(IP_addr_1, 15, 0, 2) < 0)
////  	return;
//
//    for ( ;; )
//    {
//        i = EA_ucGetInputStr(3, 1, 20, EM_BIG_FONT | EM_MODE_NUMBER | EM_ALIGN_LEFT | EM_SHOW_PASSWORD
//                             , 7, 15, 0, (void *)IP_addr_1);
//        if ( i == EM_SUCCESS )
//            break;
//        if ( i == EM_ABOLISH )
//            return ;
//    }
//
//    for(i = 0; i < 4; i++)
//    {
//        memcpy(buf, IP_addr_1 + i * 4, 3);
//        buf[3] = '\0';
//        iTmp[i] = atoi(buf);
//        if(atoi(buf) > 255)
//        {
//            lcddisperr("IP��ַ��������");
//            goto IP_AGAIN;
//        }
//    }
//
//    for(i = 0; i < 4; i++)
//    {
//        DevStat.ServerIp[i] = iTmp[i];
//    }
////  DevStat.ServerPort[0]=0x90;
////  DevStat.ServerPort[1]=0x00;
//
//    memset(IP_addr, '\0', 16);
//    sprintf(IP_addr, "%d.%d.%d.%d", iTmp[0], iTmp[1], iTmp[2], iTmp[3]);
//    EA_vDisplay(3, IP_addr);
//
//PORT_AGAIN:
//    EA_vDisplay(4, "������˿ں�:");
//    memcpy(IP_addr_1, "     ", 5);
//    IP_addr_1[5] = '\0';
////  if (ReadNumeric_(IP_addr_1, 5, 24, 2) < 0)
////  	return;
//    for ( ;; )
//    {
//        i = EA_ucGetInputStr(13, 1, 20, EM_BIG_FONT | EM_MODE_NUMBER | EM_ALIGN_LEFT | EM_SHOW_PASSWORD
//                             , 1, 5, 0, (void *)IP_addr_1);
//        if ( i == EM_SUCCESS )
//            break;
//        if ( i == EM_ABOLISH )
//            return ;
//    }
//
//    iport = atoi(IP_addr_1);
//    if( iport > 65535 )
//    {
//        lcddisperr("�˿ں���������");
//        goto PORT_AGAIN;
//    }
////	memset(Comm_port, '\0', 16);
////	sprintf(Comm_port, "%d", iport);
//    DevStat.ServerPort[0] = (INT8U)(iport>>8);
//    DevStat.ServerPort[1] = (INT8U)iport;
//
//    WriteParam();
//}


// *****************************************************************
// ���ܣ�		Price_Setting
// ˵��:		���͵�������
// ��ڲ�����	��
// ���ڲ�����	Null
// ���ߣ�    frank
// ����ֵ��	NULL							
// *****************************************************************
void price_Setting(void)
{
	int     ret;
	char    input[22], buf[22];
	uchar   i;
	double  temp_price;
	INT8U   price_menu[2*20 + 2] = {"1.�Զ�              2.�ֶ�              "};
	INT16U  key = EM_key_NOHIT;
	INT32U  tmp_price[4] = {0, 0, 0, 0};
	//uint    sendbuf_len = 0;

	memset(buf, 0, sizeof(buf));
	memcpy(tmp_price, (INT8U *)&DevStat.price[0], 16);

	ret = browse_menu(0, (unsigned char*)"", price_menu, TRUE);
	switch ( ret )
	{
	    case 0:	// �Զ�����

			if(download_price_table() == ok)
			{
				//lcddisperr("�����ͼ۳ɹ�");
				GPRS_Close();
			}
			else
			{
				;
			}
			break;

		case 1:	// �ֶ�����
			EA_ucClrKeyBuf();
			switch ( PasswordCheck() )
			{
				case notok:
					printf_debug((void *)"�������");
					return ;
				case 2:
					return ;
				case ok:
					EA_ucClrKeyBuf();
					break;
				default:
					return ;
			}
			//��ǰ�ͼ���ʾ
			EA_vCls();
			EA_ucClrKeyBuf();
			strcpy((void *)input, "");
reboot:
			EA_vDisplay(1, "��Ȼ�� :%2.2fԪ/����", tmp_price[0]/100.0);
			EA_vDisplay(2, "����93#:%2.2fԪ/�� ",  tmp_price[1]/100.0);
			EA_vDisplay(3, "����97#:%2.2fԪ/�� ",  tmp_price[2]/100.0);
			EA_vDisplay(4, "��  �� :%2.2fԪ/�� ",  tmp_price[3]/100.0);
			EA_vDisplay(5, "�����ּ�����Ӧ���ͼ�");

			key = EA_uiInkey(0);
			switch ( key )
			{
			    case EM_key_1:
					EA_vCls();
					EA_ucClrKeyBuf();
					EA_vDisplay(1, "��Ȼ�� :%2.2fԪ/����", tmp_price[0]/100.0);
					EA_vDisplay(2, "������ :%2.2fԪ/����", 00.00);
					for ( ;; )
					{
						i = get_input_str(2, 11, 1, 6, (void *)input);
						if ( i == EM_SUCCESS )
							break;
						if ( i == EM_ABOLISH )
							break ;
					}

					sscanf((void *)input, "%lf", &temp_price);
					tmp_price[0] = temp_price * 100 + 0.001;
				   	break;

				case EM_key_2:
					EA_vCls();
					EA_ucClrKeyBuf();
					EA_vDisplay(1, "����93#:%2.2fԪ/�� ", tmp_price[1]/100.0);
					EA_vDisplay(2, "������ :%2.2fԪ/�� ", 00.00);
					for ( ;; )
					{
						i = get_input_str(2, 11, 1, 6, (void *)input);
						if ( i == EM_SUCCESS )
							break;
						if ( i == EM_ABOLISH )
							break ;
					}
					sscanf((void *)input, "%lf", &temp_price);
					tmp_price[1] = temp_price * 100 + 0.001;
				    break;

				case EM_key_3:
					EA_vCls();
					EA_ucClrKeyBuf();
					EA_vDisplay(1, "����97#:%2.2fԪ/�� ", tmp_price[2]/100.0);
					EA_vDisplay(2, "������ :%2.2fԪ/�� ", 00.00);
					for ( ;; )
					{
						i = get_input_str(2, 11, 1, 6, (void *)input);
						if ( i == EM_SUCCESS )
							break;
						if ( i == EM_ABOLISH )
							break ;
					}

					sscanf((void *)input, "%lf", &temp_price);

					tmp_price[2] = temp_price * 100 + 0.001;
					
				   break;

				case EM_key_4:
					EA_vCls();
					EA_ucClrKeyBuf();
					EA_vDisplay(1, "��  �� :%2.2fԪ/�� ", tmp_price[3]/100.0);
					EA_vDisplay(2, "������ :%2.2fԪ/�� ", 00.00);

					for ( ;; )
					{
						i = get_input_str(2, 11, 1, 6, (void *)input);
						if ( i == EM_SUCCESS )
							break;
						if ( i == EM_ABOLISH )
							break;
					}

					sscanf((void *)input, "%lf", &temp_price);
					tmp_price[3] = temp_price * 100 + 0.001;

				     break;

				case EM_key_ENTER:
					
					ret = upload_price_log(tmp_price);
					if(ret != ok)
					{
						lcddisperr("�����ͼ�ʧ��");
						GPRS_Close();
						break;
					}

					DevStat.price[0] = tmp_price[0];
					DevStat.price[1] = tmp_price[1];
					DevStat.price[2] = tmp_price[2];
					DevStat.price[3] = tmp_price[3];

					lcddisperr("�����ͼ۳ɹ�");
					WriteParam();
					GPRS_Close();
					return;
				default: 
					break;
				case EM_key_EXIT:
					return;
				
			}
			goto reboot;
		default: 
			return ;
		//	break;
	}
	return ;
}
// *****************************************************************
// ���ܣ�		Information_Query
// ˵��:		�������ò˵�
// ��ڲ�����	
// ���ڲ�����	Null
// ���ߣ�
// ����ֵ��	-1 �ػ�							
// *****************************************************************
// *****************************************************************
// ���ܣ�		Datetime_Setting
// ˵��:		����ʱ������
// ��ڲ�����	
// ���ڲ�����	Null
// ���ߣ�
// ����ֵ��							
// *****************************************************************
int oper_code_setting(void)
{
	uchar 	  i = 0;
	INT8U	  input1[22], input2[22];					
	INT8U     block_buffer1[16] = {0};			//M1��һ��block�����ݻ���
	INT8U     block_buffer2[16] = {0};			//M1��һ��block�����ݻ���
	INT16U    uckey = 0;
	uint      uikey = 0;
	INT8U     KeySector[2][6];	

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
		return ok;
	}

	memcpy(&CardInfo.driver_number[0], &block_buffer1[1], 3);							 //����Ա����																				 //
//  memcpy(&CardInfo.authentication_code[0], &block_buffer1[4], 4);						 //����Ա����
	CardInfo.card_type = block_buffer1[11];											     //����

	if (CardInfo.card_type != 0xBB)                 //ֻ������ͨ������debug  by frank
	{
		lcddisperr((void *)"���Ǽ���Ա��!");
		return notok;
	}

	EA_vCls();
	EA_vDisplay(1, "�� �� Ա:%02X%02X%02X", CardInfo.driver_number[0],CardInfo.driver_number[1],CardInfo.driver_number[2]);
	EA_vDisplay(2, "����������:");
	EA_vDisplay(3, "������һ��:");
	EA_vDisplay(4, "   �����ƶ���Ƭ   ");
	strcpy((void *)input1, "");
	strcpy((void *)input2, "");

	for ( ;; )
	{
		i = EA_ucGetInputStr(2, 12, 20, EM_BIG_FONT | EM_MODE_NUMBER | EM_ALIGN_LEFT | EM_SHOW_PASSWORD
							 , 8, 8, 0, (void *)input1);
		if ( i == EM_SUCCESS )
			break;
		if ( i == EM_ABOLISH )
			return 2;
	}
	//������һ��
	for ( ;; )
	{
		i = EA_ucGetInputStr(3, 12, 20, EM_BIG_FONT | EM_MODE_NUMBER | EM_ALIGN_LEFT | EM_SHOW_PASSWORD
							 , 8, 8, 0, (void *)input2);
		if ( i == EM_SUCCESS )
			break;
		if ( i == EM_ABOLISH )
			return 2;
	}

	if ( memcmp((void *)input1, input2, 8) == 0 )
	{
		//lcddisperr("�����޸ĳɹ�");
		//return ok;
	}
	else
	{
		EA_ucSetInverse(EM_lcd_INVON);
		lcddisp(4, 1, (void *)"  �����������벻ͬ  ");
		EA_ucSetInverse(EM_lcd_INVOFF);
		(void)EA_uiInkey(1);
		return notok;
	} 

//  i = Get_Sector_PermissionA(1, &KeySector[1][0]);
//  if ( i != ok )
//  {
//  	lcddisperr((char *)"д��Կ��֤ʧ��");
//  	return notok;
//  }
	var_asc2bcd(&block_buffer1[4], &input1[0], 8);
//  block_buffer1[11] = 0xBB;
//  block_buffer1[5] = a_to_h(input1[1]);
//  block_buffer1[6] = a_to_h(input1[2]);
//  block_buffer1[7] = a_to_h(input1[3]);

	memcpy(block_buffer2, block_buffer1, 16);

	i = Sec0_Block_Is_Legal(block_buffer2);
	if ( (i == ok) )
	{
	    if(EA_ucMIFWrite (hMifsHandle, 4, block_buffer2) == ok)
		lcddisperr("�����޸ĳɹ�");
		EA_ucMIFHalt(hMifsHandle);	//�ҿ�
		return ok;
	}
	lcddisperr("�����޸�ʧ��");
	EA_ucMIFHalt(hMifsHandle);	//�ҿ�
	return notok;
}


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
	   case 3:
		   DevStat.fill_mode = 3;
		   break;
	   default:
		   break;
	}

	WriteParam();

	return ok;
}

// *****************************************************************
// ���ܣ�		Record_Query
// ˵��:		��¼��ѯ
// ��ڲ�����	
// ���ڲ�����	Null
// ���ߣ�
// ����ֵ��							
// *****************************************************************
int Record_Query(void)
{
	int         num;
	INT32U      RecordCount = 0;
	INT32U      Count = 0;
	uchar       uckey; 
	uint        uikey; 
	uchar 		ucOpenID=0;
	INT8U 		rec[57];

	EA_vCls();
	EA_ucClrKeyBuf();

	memset(rec, 0, sizeof(rec));
////////////////////////////////////////////////////////
//    if ( EA_ucPFOpen((uchar *)currecFileName, &ucOpenID) != EM_ffs_SUCCESS )
//    {
//        lcddisperr("�򿪼�¼�ļ�ʧ��");
//        (void)EA_ucPFClose(ucOpenID);
//        return notok;
//    }
//    if ( EA_ucPFInfo(ucOpenID, EM_ffs_VALIDREC, &uiRetNum) != EM_ffs_SUCCESS )
//    {
//        lcddisperr("��ȡ��¼�ļ�ʧ��");
//        (void)EA_ucPFClose(ucOpenID);
//        return notok;
//    }
//
//    Num_rec = uiRetNum;
//    (void)EA_ucPFClose(ucOpenID);
//
//    EA_vDisplay(1, "ϵͳ�洢��¼��Ϣ��ѯ");
//    EA_vDisplay(2, "���׼�¼��:%6d��", Num_rec);

	if ( EA_ucPFOpen( (uchar *)hisrecFileName, &ucOpenID) != EM_ffs_SUCCESS )
	{
		lcddisperr("����ʷ�ļ��ļ�ʧ��");
		return 0;
	}
	// ��ȡ��Ч��Ϣ����
	(void)EA_ucPFInfo(ucOpenID, EM_ffs_VALIDREC, &RecordCount);
	if( RecordCount == 0)
	{
		lcddisperr("û�н�����Ϣ");
		(void)EA_ucPFClose(ucOpenID);
		return 0;
	}

	num = RecordCount;//���һ��		

	if(EA_ucPFReadRec(ucOpenID, num, 1, &Count, rec) != EM_SUCCESS)
	{
		(void)EA_ucPFClose(ucOpenID);
		lcddisperr((void *)"��ȡ������");
		return 0;
	}

	AnalyAndDisplay((RECORD *)rec);   //���ͼ�¼��ѯ

	while (1)
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
				case EM_key_LETTER:	                //	�Ϲ���һ��
					num --;
					if ( num <= 0 )                 //��¼����
					{
						lcddisperr("�Ѿ��ǵ�һ��");
						num = 1;
					}
					break;

				case EM_key_INTERPUNCTION:	        //	�¹���һ��
					num ++;
					if ( num >= RecordCount+1 )
					{
						lcddisperr("�Ѿ������һ��");
						num = RecordCount;
					}
					break;
				case EM_key_ENTER:
					break;
				default:
					break;
			}

			if ( (uikey == EM_key_INTERPUNCTION) || (uikey == EM_key_LETTER)  )
			{
				if(EA_ucPFReadRec(ucOpenID, num, 1, &Count, rec) != EM_SUCCESS)
				{
					(void)EA_ucPFClose(ucOpenID);
					lcddisperr((void *)"��ȡ������");
					return 0;
				}
				AnalyAndDisplay((RECORD *)rec);
			}

			if(uikey == EM_key_ENTER)
			{
				print_record((RECORD *)rec);
				(void)EA_ucPFClose(ucOpenID);
				return 1;
			}			
		}
	}
}


//��� SHENGH:   ������   ��˭ �ҵ���2014�Ǽǽ����飬   ����2012-1244�ں��ϣ�  u�����ٴ��ڼ�û�����������ӣ����������
//// *****************************************************************���ڣ� ��ַ��  �绰��
//// ���ܣ�		AnalyAndDisplay
//// ˵��:		��ѯ��¼����ʵ�ֽ��׼�¼
//// ��ڲ�����
//// ���ڲ�����	Null
//// ���ߣ�
//// ����ֵ��
//// *****************************************************************
void AnalyAndDisplay(RECORD * strTradeInfo)
{
	char     strbuf[22];
	double    fBuf = 0;
	INT8U    temp_buf[22];
	RECORD * prec = (RECORD *)strTradeInfo;

	memset(strbuf, 0, sizeof(strbuf));

	EA_ucClrKeyBuf();

	//��ʾ��Ƭ��Ϣ
	EA_vCls();
	memcpy(temp_buf, &prec->bus_number[0], 5);
	strcpy((void *)strbuf, (void *)"³C-");
	strcat((void *)strbuf, (void *)temp_buf);
	EA_vDisp(1, 1, strbuf);

	switch ( prec->fuel_type )
	{
		case 0:
			sprintf((void *)strbuf, "��ȼ��");//������Ƭ������Ϣ
			EA_vDisp(1,12, strbuf);

			fBuf    = (double)(prec->fare)/100.0;
			sprintf((void *)strbuf, "��ϸ:%4.2lfԪ", fBuf);
			EA_vDisp(2,1, strbuf);
			fBuf    = (double)(prec->capacity)/100.0;
			sprintf((void *)strbuf, "%3.2lf��", fBuf);
			EA_vDisp(2,14, strbuf);
			break;
		case 1:
			sprintf((void *)strbuf, "����93#");//������Ƭ������Ϣ
			EA_vDisp(1,12, strbuf);

			fBuf    = (double)(prec->fare)/100.0;
			sprintf((void *)strbuf, "��ϸ:%4.2lfԪ", fBuf);
			EA_vDisp(2,1, strbuf);
			fBuf    = (double)(prec->capacity)/100.0;
			sprintf((void *)strbuf, "%3.2lf��", fBuf);
			EA_vDisp(2,14, strbuf);
			break;
		case 2:
			sprintf((void *)strbuf, "����97#");//������Ƭ������Ϣ
			EA_vDisp(1,12, strbuf);

			fBuf    = (double)(prec->fare)/100.0;
			sprintf((void *)strbuf, "��ϸ:%4.2lfԪ", fBuf);
			EA_vDisp(2,1, strbuf);
			fBuf    = (double)(prec->capacity)/100.0;
			sprintf((void *)strbuf, "%3.2lf��", fBuf);
			EA_vDisp(2,14, strbuf);
			break;
		case 3:
			sprintf((void *)strbuf, "����");//������Ƭ������Ϣ
			EA_vDisp(1,12, strbuf);

			fBuf    = (double)(prec->fare)/100.0;
			sprintf((void *)strbuf, "��ϸ:%4.2lfԪ", fBuf);
			EA_vDisp(2,1, strbuf);
			fBuf    = (double)(prec->capacity)/100.0;
			sprintf((void *)strbuf, "%3.2lf��", fBuf);
			EA_vDisp(2,14, strbuf);
			break;

		default:
			break;

	}
	htoa(temp_buf, (void *)&prec->card_in_time, 6);
	strcpy((void *)strbuf, (void *)"����:");
	strcat((void *)strbuf, (void *)temp_buf);
	EA_vDisp(3, 1, strbuf);

	htoa2(temp_buf, (void *)&prec->pos_purchase_serial_num[0], 3);

	strcpy((void *)strbuf, (void *)"�������:");
	strcat((void *)strbuf, (void *)temp_buf);
	EA_vDisp(4, 1, strbuf);

}


// *****************************************************************
// ���ܣ�		Note_Query
// ˵��:		�սᵥ��ѯ
// ��ڲ�����	
// ���ڲ�����	Null
// ���ߣ�
// ����ֵ��							
// *****************************************************************
int Note_Query(void)
{
	int         num;
	INT32U      RecordCount = 0;
	INT32U      Count = 0;
	uchar       uckey; 
	uint        uikey; 
	uchar 		ucOpenID=0;
	INT8U 		rec[100];

	EA_vCls();
	EA_ucClrKeyBuf();

	memset(rec, 0, sizeof(rec));

	if ( EA_ucPFOpen( (uchar *)conrecFileName, &ucOpenID) != EM_ffs_SUCCESS )
	{
		lcddisperr("����ʷ�ļ��ļ�ʧ��");
		return 0;
	}
	// ��ȡ��Ч��Ϣ����
	(void)EA_ucPFInfo(ucOpenID, EM_ffs_VALIDREC, &RecordCount);
	if( RecordCount == 0)
	{
		lcddisperr("û���ս���Ϣ");
		(void)EA_ucPFClose(ucOpenID);
		return 0;
	}

	num = RecordCount;//���һ��		

	if(EA_ucPFReadRec(ucOpenID, num, 1, &Count, rec) != EM_SUCCESS)
	{
		(void)EA_ucPFClose(ucOpenID);
		lcddisperr((void *)"��ȡ������");
		return 0;
	}

	AnalyAndDisplayNote((INT8U *)rec);   //���ͼ�¼��ѯ

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
				case EM_key_LETTER:	                //	�Ϲ���һ��
					num --;
					if ( num <= 0 )                 //��¼����
					{
						lcddisperr("�Ѿ��ǵ�һ��");
						num = 1;
					}
					break;

				case EM_key_INTERPUNCTION:	        //	�¹���һ��
					num ++;
					if ( num >= RecordCount+1 )
					{
						lcddisperr("�Ѿ������һ��");
						num = RecordCount;
					}
					break;
				case EM_key_ENTER:
					break;
				default:
					break;
			}

			if ( (uikey == EM_key_INTERPUNCTION) || (uikey == EM_key_LETTER)  )
			{
				if(EA_ucPFReadRec(ucOpenID, num, 1, &Count, rec) != EM_SUCCESS)
				{
					(void)EA_ucPFClose(ucOpenID);
					lcddisperr((void *)"��ȡ������");
					return 0;
				}
				AnalyAndDisplayNote((INT8U *)rec);
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


