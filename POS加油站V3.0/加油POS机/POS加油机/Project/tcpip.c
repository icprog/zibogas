
#include "main.h"
//#define _debug_tcp_
//#define  _whitle_debug_


extern DevHandle hSIMHandle;
extern DevHandle hWLMHandle;
extern uchar blackFileName[];
//extern DevHandle hApptimerHandle;

int  tmpch;
int  socketID;
//EA_TestPara gtPara;             				//local para
int pppn;    									//PPP��·����
/*para*/
typedef struct
{
    ET_PPP_PARA    stPara ;     //only for gprs&cdma
    char    acUser[80] ;        //username
    char    acPwd[80] ;         //password
    int     optionId ;    		//option file Id

}EA_TestPara;
EA_TestPara gtPara;
struct sockaddr_in s_add;
extern DEV_STAT 	DevStat;					//�豸״̬
extern INT8U  KeySector[14][6];					//M1��������Կ��ÿ������ԿΪ6�ֽ�
const unsigned char COM_END = '\r';				//��������ַ�

/******************************************************************************
 �������ƣ�GPRS_Close
 ����������GPRS�ر�
 �������ƣ�����/�����	����		����
����		    
//������˵��:
				
 ��  ��  ֵ����
 
 ��      ��	��YFY
 ��      �ڣ�2007-08-08
 �޸���ʷ��
		����		�޸���		�޸�����
******************************************************************************/	
void GPRS_Close(void)
{
	EA_vCls();
	EA_vDisp(2, 1, "  GPRSģ�����ڹر�  ");
	EA_vDisp(3, 1, "    ���Ժ�......    ");
	//DevStat.Gprs_On_Flag = 0;
	close_gprs_module();
//  (void)EA_uiInkey(1);
}

/******************************************************************************
 �������ƣ�FindStr
 ��������������ָ��ַ����
 �������ƣ�����/�����	����		����
����		    data	����
				
 ��  ��  ֵ����
 
 ��      ��	��YFY
 ��      �ڣ�2007-05-2
 �޸���ʷ��
		����		�޸���		�޸�����
******************************************************************************/
int FindStr(unsigned char * data, int index_num)
{
	INT8U  	 i=0, k=0;
	INT8U    str[22];
	int		 DataLength = 0, ret = 0;
	INT8U    input_buffer[20+2];
	BUS_TIME LOCAL_V_TYPE time;

	char    temp_data[200];
	char  	RevBuff[1024];
	ulong  	black_length=0;
	ulong  	tolanum=0;
	uint 	uiOpenID = 0;
	INT16U  black_number;  
	unsigned char   tmp_kind;
	uchar   tmp_price[8]; 

	memset(temp_data, 0x00, sizeof(temp_data));

#ifdef  _debug_tcp_
		Debugprintf((char *)data);
#endif

	for(i=0;i<index_num;i++)
	{
		if((data[i] == '@')&&(data[i+1]=='S')&&(data[i+2]=='T')&&(data[i+3]=='A')&&(data[i+4]=='R'))
		{
			if(index_num-i < 20)//���ݱ����ȴ���
			{
				lcddisperr((void *)"-1");
				return -1;
			}
			else
			{
				DataLength = index_num-i;
				memcpy(RevBuff, data+i, DataLength);
				RevBuff[DataLength] = 0;		
			}

			ret = PackageCheck(RevBuff, DataLength);
			if(ret != 1)//��֤CRC�Ƿ���ȷ
			{
				sprintf((void *)str, "ret:%d,len:%d", ret, DataLength);
				lcddisperr((void *)str);
				return -2;
			}
			
			if(memcmp(RevBuff+19, "ER", 2) == 0)
			{
				lcddisperr("���ݴ���������");
				return -3;//���ݴ���������
			}
			if((memcmp(RevBuff+12, "TIME", 4)==0))//����ʱ��
			{		
				for(k=0; k<7; k++)
					input_buffer[k] = ((a_to_h(RevBuff[2*k+19]))<<4)|((a_to_h(RevBuff[2*k+1+19]))&0x0f);
				memcpy((INT8U*)&time, input_buffer, 7);
				if(CheckTimeFormat(&time) != ok)
					return 1;
				Modify_Time(&time);
//  			Modify_GPRS_CLK();
				return 1;							
			}

			if( (memcmp(RevBuff + 12, "WHNM", 4) == 0) ) //��ѯ����������
			{			
				if( memcmp(RevBuff+16, "005", 3) == 0 )  
				{
					black_number = 	(INT16U)(a_to_h(RevBuff[19]))*1000 + 
									(INT16U)(a_to_h(RevBuff[20]))*100 +
									(INT16U)(a_to_h(RevBuff[21]))*10 + 
									(INT16U)(a_to_h(RevBuff[22])); 

					if((memcmp(&RevBuff[23], "N", 1) == 0) && 
					   (DevStat.black_name_number == black_number))
					{
						DevStat.black_flag = 0;
					}
					else
					{
						DevStat.black_flag = 1;
						DevStat.black_name_number = black_number;
					}
					return 1;
				}
				return 0;
			}

			if( (memcmp(RevBuff+12, "UPYJ", 4) == 0) ) //�ͼ۱�
			{			
				if( memcmp(RevBuff+16,"024",3) == 0 ) 
				{
					for( k = 0; k < 4; k ++ )
					{
						//DevStat.fill_mode = (a_to_h(RevBuff[5*k+19]));
						var_asc2bcd((uchar *)&tmp_kind, (uchar *)&RevBuff[6*k+19], 2);	
						switch( tmp_kind )	
						{
						   case 0:  
							   DevStat.price[0] = a_to_h(RevBuff[21]) * 1000+a_to_h(RevBuff[22])* 100+
							   a_to_h(RevBuff[23]) * 10+a_to_h(RevBuff[24]) ;
							   break;
						   case 1:
							   var_asc2bcd((uchar *)&tmp_price, (uchar *)&RevBuff[31], 8);

							   DevStat.price[1] =a_to_h(RevBuff[27]) * 1000+a_to_h(RevBuff[28])* 100+
							   a_to_h(RevBuff[29]) * 10+a_to_h(RevBuff[30]) ;
							   break;
						   case 2:
							   var_asc2bcd((uchar *)&tmp_price, (uchar *)&RevBuff[41], 8);
							
							   DevStat.price[2] = a_to_h(RevBuff[33]) * 1000+a_to_h(RevBuff[34])* 100+
							   a_to_h(RevBuff[35]) * 10+a_to_h(RevBuff[36]) ;
							   break;
						   case 3:
							   var_asc2bcd((uchar *)&tmp_price, (uchar *)&RevBuff[51], 8);
							  
							   DevStat.price[3] = a_to_h(RevBuff[39]) * 1000+a_to_h(RevBuff[40])* 100+
							   a_to_h(RevBuff[41]) * 10+a_to_h(RevBuff[42]) ;
							   break;
						   default:
							   break;

						}
//  				    DevStat.price[k]  = (INT16U)(a_to_h(RevBuff[10*k+19 + 1]))*1000+
//  										(INT16U)(a_to_h(RevBuff[10*k+19 + 2]))*100+
//  										(INT16U)(a_to_h(RevBuff[10*k+19 + 3]))*10+
//  										(INT16U)(a_to_h(RevBuff[10*k+19 + 4]));
					}
					//WriteParam();
					return 1;
				}
				return 0;			
			}

			if( memcmp(RevBuff+12, "WHUP", 4) == 0 )      //���°�����
			{
				memcpy(temp_data, RevBuff+16, 3);
				temp_data[3] = 0;	
				black_length = atoi(temp_data);				
				black_length = black_length / 5;         //ÿ5���ֽڣ�ASCII��Ϊһ�鳵�ƺ�
				EA_ucBlkOpen((void *)&blackFileName, &uiOpenID);

#ifdef _whitle_debug_
	char DISPBUF[100];
	Debugprintf((char *)RevBuff+19);
	sprintf(DISPBUF, "%04d", black_length);
	Debugprintf((char *)DISPBUF);
#endif
				tolanum += black_length; 
				sprintf((void *)str, "����������%04lu", tolanum);
				EA_vDisplay(3, (void *)str);

				ret = EA_ucBlkAppendBase(uiOpenID, RevBuff+19, &black_length); 
				if( ret != EM_SUCCESS)
				{
					lcddisperr("������д��ʧ��");
					EA_ucBlkClose(uiOpenID);
					return 0;
				}
				EA_ucBlkClose(uiOpenID);
				return 1;
			}

			if(memcmp(RevBuff+19, "OK", 2)==0)    //���ݴ���ɹ�
				return 1;
		}
	}
	if(i == index_num)
		return -1;
	return 1;
}

/******************************************************************************
 �������ƣ�GPRS_TCP_Send
 ����������gprs�������ַ���
 �������ƣ�����/�����	����		����
 ����		    data	����
    			length   ���ݳ���
				timeout  ��ʱ����
 ��  ��  ֵ����
 
 ��      ��	��YFY/������
 ��      �ڣ�2012-12-12
 �޸���ʷ��
		����		�޸���		�޸�����
******************************************************************************/
INT8U GPRS_TCP_Send(unsigned char *Data, int length, unsigned short timeout)
{
	 size_t sndlen = 0;

	 timeout = timeout;
	 sndlen = (size_t)length;

	 if( write(socketID, Data, sndlen) == -1)
	 {
		 lcddisperr("����ʱ��ʧ��");
		 return notok;
	 }
	 lcddisperr("����ʱ��ɹ�");
     return ok;
}

/******************************************************************************
 �������ƣ�GPRS_TCP_Receive
 ����������gprs�������ַ���
 �������ƣ�����/�����	����		����
 ����		    data	����
    			length   ���ݳ���
				timeout  ��ʱ����
 ��  ��  ֵ����
 
 ��      ��	��YFY/������
 ��      �ڣ�2012-12-12
 �޸���ʷ��
		����		�޸���		�޸�����
******************************************************************************/
INT8U GPRS_TCP_Receive(unsigned char *Data, int *length,unsigned short timeout)
{
	size_t Rcvlen = 0;
	timeout = timeout;

	Rcvlen = (size_t)length;

	if(read( socketID, Data, Rcvlen ) == -1)
	{
		lcddisperr("����ʧ��");
		return notok;
	}
	lcddisperr("���ճɹ�");
	return ok;
}

/*****************************************************************
 *	������GPRS_Send_Receive()
 *	���ܣ����ͺͽ���GPRS����
 *	���������senddata--������
 		      sendlength--���ݳ���;
 *	�����������
 *	����ֵ��0--�ɹ�, ff--ʧ��  
    		2--���³�������
    		3--�ź���������λ�ú��ٴ�����
 *	˵����	�����رշ���  
 *		   �ر�ǰ����ս��ջ����� 	      
 *****************************************************************/
INT8U GPRS_Send_Receive(INT8U *senddata, int  sendlength)
{
	INT8U	STR[10], sendbuf[1024], recbuf[1024];
	INT16U	crc = 0;
	char	ret;
	int		retlen = 0;	
	int		len = 0;
	int i = 0;	

	memset(STR, 0, sizeof(STR));
	memcpy(sendbuf, senddata, sendlength);
	sendbuf[sendlength] = '\0';

//  EA_uiControlAppTimer(hApptimerHandle, EM_rtos_DISABLE_TIMER);
	
	//���ӷ�����
	if( ConnectServer() != ok )
	{
		close(socketID);
		lcddisperr("�޷����ӷ�����");
		goto Err_sockoff;
	}

	crc = Cal_CRC((unsigned char*)sendbuf, sendlength);	
	sprintf((char*)STR, "%02X%02XEND\r", ((crc >> 8)& 0xff), (crc & 0xff));
	memcpy(sendbuf+sendlength, STR, 8);
	len = sendlength + 8;
	sendbuf[len] = '\0';

	if( write(socketID, sendbuf, len) == -1)
	{
		lcddisperr("TCP/IP����ʧ��");
		goto Err_sockoff;
	}	

	memset(recbuf, 0x00, sizeof(recbuf));

	for( i = 0; i < 500; i ++ )   //������ʱ�ȴ�10���ӣ���ʱ�˳�
	{
		retlen = read( socketID, recbuf, sizeof(recbuf) );
		if( retlen > 0 )
		{
			break;
		}	
		EA_vDelayMs(20);
	}
	if(i >= 500)
	{
		lcddisperr("TCP/IP����ʧ��");
		goto Err_sockoff;
	}
	recbuf[retlen] = '\0';

	ret = FindStr(recbuf, retlen);        //���ݼ��ְ�
	if(ret != 1)
	{
//  lcddisperr("���ݸ�ʽУ�����");
		goto Err_sockoff;
	}
	close(socketID);
//  EA_uiControlAppTimer(hApptimerHandle, EM_rtos_ENABLE_TIMER);
	return ok;

Err_sockoff:
	close(socketID);
//  EA_uiControlAppTimer(hApptimerHandle, EM_rtos_ENABLE_TIMER);
	return notok;
}

/*****************************************************************
 ����ԭ�ͣ�open_gprs_module 
 ����������GPRS��ʼ��������������
 ���������� ��
 �������ƣ�	����/�����	����		����
			��
				
 ��  ��  ֵ��ok ?? notok			   
 ��      ��	��������
 ��      �ڣ�2012-12-12
 �޸���ʷ��
	  	����		�޸���		�޸�����
*****************************************************************/
uchar open_gprs_module(void)
{
	uchar ucResult = 0;
	uchar ucRet = 0;
	ET_ROUTE_CONFIG tRouteCFG;      //route config
	uchar ucStatus = 0; 
	INT8U i = 0;
	char vIPBuf[128];

	EA_vCls();
	EA_vDisplay(2, "     ���ų�ʼ��...  ");
//  EA_uiInkeyMs(500);
	//������Ҫ����������һ��·�ɡ�����ֻ��һ��ͨ�ŷ�ʽʱ���ſ��԰�����ֵ�������á�������ͨ�ŷ�ʽ����ο�ͨѶͨ�ÿ�˵��
	memset(&tRouteCFG, 0, sizeof(ET_ROUTE_CONFIG));

	strcpy( tRouteCFG.Destination, "0.0.0.0");
	strcpy( tRouteCFG.GateWay, "0.0.0.0");
	strcpy( tRouteCFG.Mask, "0.0.0.0");

	ucResult = EA_ucCommuExpand( (char *)"/dev/WLM", EM_CMD_SETROUTE, &tRouteCFG);
	if ( ucResult != EM_SUCCESS )
	{
		EA_vDisplay( 2, " ·������:%02X ", ucResult );
//  	(void)EA_uiInkeyMs(2000);
	}

    ucResult = EA_ucInitPpp( (char *)"/dev/WLM", &pppn );        //Init WirelessModule
    if ( ucResult != EM_SUCCESS )
    {
		//EA_vCls();
		//EA_vDisplay( 2, "   GPRS�����...   ");
		//(void)EA_uiInkeyMs(1000);
		(void)EA_ucGetLinkState( pppn, PROTO_CONNECT_EVENT, &ucStatus );
		if( ucStatus == EM_ppp_SUCCESS )    //���гɹ�
		{
			DevStat.Gprs_On_Flag = 1;
			//EA_vCls();
			//EA_vDisplay( 3, "    �������ӳɹ�    ");
			//(void)EA_uiInkeyMs(1000);
			return ok;
		}
	    EA_vDisplay( 4, "    ����ʧ��:%02X   ", ucResult );
		(void)EA_uiInkeyMs(2000);
        goto offdial;
	}

	EA_vCls();
    //EA_vDisplay( 2, "    PPP��·��:%02X    ", pppn );
	//(void)EA_uiInkeyMs(1000);

	//�������ã��Լ��û������������ã�����Ԥ�����ý���

	memset(&gtPara, 0, sizeof(EA_TestPara));

	strcpy( gtPara.acUser, "");
	strcpy( gtPara.acPwd, "");
	gtPara.optionId = 0;
    strcpy(gtPara.stPara.acCenter , "ATD*99***1#");
//    if( DevStat.samcompcode == 1)
//    {
//        lcddisperr("�ƶ�ר��");
////  	strcpy(gtPara.stPara.acPduContext , "AT+CGDCONT=1,\"IP\",\"qtd.zb.sd\"");//һ��ͨר��
//        strcpy(gtPara.stPara.acPduContext , "AT+CGDCONT=1,\"IP\",\"gjgs.zb.sd\"");//������˾ר��
//    }
//    else if( DevStat.samcompcode == 0)
//    {
//        lcddisperr("�ƶ�����");
//        strcpy(gtPara.stPara.acPduContext , "AT+CGDCONT=1,\"IP\",\"CMNET\"");
//    }
	if( DevStat.samcompcode == 1)
	{
//  	lcddisperr("��ͨר��");
//  	strcpy(gtPara.stPara.acPduContext , "AT+CGDCONT=1,\"IP\",\"qtd.zb.sd\"");//һ��ͨר��
		strcpy(gtPara.stPara.acPduContext , "AT+CGDCONT=1,\"IP\",\"zbbus.sd\"");//������˾ר��
//  	strcpy(gtPara.stPara.acPduContext , "AT+CSTT=\"zbbus.sd\"");
	}                                          
	else if( DevStat.samcompcode == 0)
	{
//  	lcddisperr("��ͨ����");
		strcpy(gtPara.stPara.acPduContext , "AT+CGDCONT=1,\"IP\",\"uninet\"");
	}
	gtPara.stPara.uiSeconds = 0;    //�ɽ�������

	//�����û���������
	ucResult = EA_ucAuthenPara( pppn , gtPara.acUser , gtPara.acPwd );            //input username and password

	if ( ucResult != EM_SUCCESS )
	{
		EA_vDisplay( 2, "    �ʺ����ô���:%02X   ", ucRet );
		(void)EA_uiInkeyMs(2000);
		goto offdial ;
	}

	EA_vCls() ;
    EA_vDisplay( 2, "    ����������...   " );
	(void)EA_uiInkeyMs(1000);
	//��ʼ����
    ucResult = EA_ucCallPpp( pppn , &gtPara.stPara );         //start to dial
    if ( ucResult != EM_SUCCESS  )
    {
        EA_vDisplay( 2, "   ��������ʧ��:%02X  ", ucRet );
        (void)EA_uiInkeyMs(1000);
        goto offdial;
    }
	//�ȴ����ŵĽ�������鳬ʱ30-60s, ���Ա� gtPara.stPara.uiSeconds ������
	for( i = 80; i > 0; i-- )                 //60����ѭ����ѯ״̬��ֵ
	{
		(void)EA_ucGetLinkState( pppn, PROTO_CONNECT_EVENT, &ucStatus );
		if( ucStatus == EM_ppp_SUCCESS )    //���гɹ�
		{
			DevStat.Gprs_On_Flag = 1;
			EA_vCls();
			EA_vDisplay( 3, "    �������ӳɹ�    ");
			(void)EA_uiInkeyMs(2000);
			return ok;
		}
		else 
		{                                     //����ʧ��
			 EA_vDisplay( 2, "    ����������...   ");
			 (void)EA_vDelayMs(2000);
		}
	}
	//����IP��ַ
	ucResult = EA_ucGetLocalDynIp( pppn , (char *)vIPBuf) ;
	if( ucResult == EM_SUCCESS )
	{

		EA_vDisplay(2, (char *)vIPBuf);
		SleepMs(2000);
		//(void)EA_uiInkey(0);
		return ok;
	}
	//�ҶϺ�������ʱʱ����ʧ�ܣ�������ִ��һ�ιҶ�
offdial:
    ucResult = EA_ucDialOffPpp( pppn ) ;           //start to hangup
    if ( ucResult != EM_SUCCESS )
    {
        EA_vDisplay( 4, "   �����Ҷ�ʧ��:%x  ", ucResult );
        EA_uiInkeyMs(2000);
        goto offdial;
    }
//�ȴ��ҶϵĽ��
    for( i = 10; i > 0; i-- )
	{
		 (void)EA_ucGetLinkState( pppn, PROTO_TERM_EVENT, &ucStatus );
		 if( ucStatus == EM_SUCCESS )    //�ɹ��Ҷ�
		 {
			 DevStat.Gprs_On_Flag = 0;
			 return notok;
		 }
		 else
		 {                                
			 lcddisperr("����Ͽ���...");
		 }
	}
	return notok;
}
/*****************************************************************
 ����ԭ�ͣ�  close_gprs_module
 ����������GPRS�ر�
 ���������� ��
 �������ƣ�	����/�����	����		����
			��
				
 ��  ��  ֵ����				   
 ��      ��	��������
 ��      �ڣ�2012-12-12
 �޸���ʷ��
	  	����		�޸���		�޸�����
*****************************************************************/
void close_gprs_module(void) 
{
	uchar ucStatus = 0;
	int i = 0;
	uchar ucRet = 0;

offdial:
    ucRet = EA_ucDialOffPpp( pppn ) ;           //start to hangup
    if ( ucRet != EM_SUCCESS  )
    {
        EA_vDisplay( 4, "   �����Ҷ�ʧ��:%x  ", ucRet );
        (void)EA_uiInkeyMs(2000);
        goto offdial;
    }
	for( i = 10; i > 0; i-- )
    {
        EA_ucGetLinkState( pppn , PROTO_TERM_EVENT , &ucStatus ) ;
		if( ucStatus == EM_SUCCESS )    //�ɹ��Ҷ�
		{
			DevStat.Gprs_On_Flag = 0;
			return;
		}
		else 
		{                              //û�йҶ�EM_ppp_Doing 
			lcddisperr("����Ͽ���...");
		}
    }
	if(SIM_power_off() < 0) 
	{
		lcddisperr("�ر�GPRSģ��ʧ��");	
	}
	else
	{
		lcddisperr("GPRSģ���ѹر�");	
		DevStat.Gprs_On_Flag = 0;
	}
}
/*****************************************************************
����ԭ�ͣ�SIM_power_on
 ������������SIM���ϵ�
 ����������
 �������ƣ�	����/�����	����		����
				
 ��  ��  ֵ��	ok  or  notok
 ��      ��	��������
 ��      �ڣ�2012-12-12
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
*****************************************************************/
uchar SIM_power_on(void) 
{
	uchar result;
	uchar prot;
	uint  len;
	uchar aucAtrData[50];

	EA_vCls();
	EA_vDisp(2, 1, "    ���SIM��...     ");
	(void)EA_uiInkeyMs(2000);

	// ��ʼ��IC����5V��EMV2000�淶
	result= EA_ucICInitModule(hSIMHandle, EM_ic_VOLDEFAULT, EM_ic_EMVMODE);
	if (result != EM_SUCCESS)
	{
		 lcddisperr("SIM�������ʼ��ʧ��");
		 goto releaseOperate;
	}
	// �ȴ�������
	while(EA_ucICCardIn(hSIMHandle) != EM_SUCCESS)
	{
		lcddisperr("�����SIM��");
		//EA_uiInkeyMs(500);
	}
	// �����ϵ�
	result = EA_ucICPowerUp(hSIMHandle, &len, aucAtrData, &prot);
	if  (result != EM_SUCCESS)
	{
		lcddisperr("SIM���ϵ�ʧ��");
		goto releaseOperate;
	}
	return ok;
releaseOperate: 
	// �ÿ�����ʱ��ʹ�ã��ͷ������Ȩ
	result = EA_ucCloseDevice(&hSIMHandle);
	return notok;
}
/*****************************************************************
����ԭ�ͣ�SIM_power_off
 ������������SIM���µ�
 ����������
 �������ƣ�	����/�����	����		����
				
 ��  ��  ֵ����
 ��      ��	��������
 ��      �ڣ�2012-12-12
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
*****************************************************************/
int SIM_power_off(void)
{
	uchar ucRet;
	ucRet = EA_ucICPowerDown(hSIMHandle);
	if( ucRet != EM_SUCCESS )
	{
		return -1;
	}
	ucRet = EA_ucCloseDevice(&hSIMHandle);
 	if( ucRet != EM_SUCCESS )	
	{
		return -1;
	}	
	return 0;
}


///*��������*/
//void GPRS_Set_HeartBeat()
//{
//    unsigned char ret,retryTime=0;
//
//    return;
//
//    RetryHeartBeat:
//        EA_uiInkeyMs(100);
//        ret=Check_CPIN_CGREG_IPCALL_For_G600(6,0);
//        if (ret != AT_OK)
//        {
//                retryTime++;
//                if(retryTime<3)
//                    goto RetryHeartBeat;
//        }
//}


/******************************************************************************
 �������ƣ�ConnectServer
 �������������ӷ�����
 �������ƣ�����/�����	����		����
����		    
//������˵��:
				
 ��  ��  ֵ����
 
 ��      ��	��������
 ��      �ڣ�2012-12-12
 �޸���ʷ��
		����		�޸���		�޸�����
******************************************************************************/
INT8U  ConnectServer(void)
{
	int  ret,i;
	int	 keepalive = 1;
	struct timeval tm;
	int keepIdle = 1;//1s
	int keepInterval = 2;//1.5s
	int keepCount = 10;//20��
	tm.tv_sec  = 2;
	tm.tv_usec = 0;
	char IP_addr[256];

	//EA_vCls();
	
	if((socketID = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		lcddisperr("    SOCK����ʧ��    ");
		return notok;
	}
	//����ʱ��
	ret = setsockopt(socketID, SOL_SOCKET, SO_SNDTIMEO, (char *)&tm, sizeof(struct timeval));
	if( ret == -1)
	{
		lcddisperr("   ����OPT����ʧ��  ");
		return notok;
	}
	//����ʱ��
	ret = setsockopt(socketID, SOL_SOCKET, SO_RCVTIMEO, (char *)&tm, sizeof(struct timeval));
	if( ret == -1)
	{
		lcddisperr("   ����OPT����ʧ��  ");
		return notok;
	}

	ret = setsockopt(socketID, SOL_SOCKET, SO_KEEPALIVE, (int *)&keepalive, sizeof(keepalive));
	ret = setsockopt(socketID, SOL_TCP,  TCP_KEEPIDLE, (void *)&keepIdle, sizeof(keepIdle));
	ret = setsockopt(socketID,SOL_TCP, TCP_KEEPINTVL, (void *)&keepInterval, sizeof(keepInterval));
	ret = setsockopt(socketID,SOL_TCP, TCP_KEEPCNT, (void *)&keepCount, sizeof(keepCount));

	memset(&s_add, 0, sizeof(s_add));
	s_add.sin_family = AF_INET;

	sprintf((char *)IP_addr, "%d.%d.%d.%d", DevStat.ServerIp[0],DevStat.ServerIp[1],DevStat.ServerIp[2],DevStat.ServerIp[3]);
	s_add.sin_addr.s_addr = inet_addr((char *)IP_addr);//"192.168.1.102");   123.185.210.253
//  lcddisperr((char *)IP_addr);
//  s_add.sin_addr.s_addr = inet_addr("192.168.2.7");    //һ��ͨ��ַ
//  s_add.sin_addr.s_addr = inet_addr("192.168.2.17");		//������˾��ַ
	s_add.sin_port = htons(5555);

	bzero(&(s_add.sin_zero), 8);

	EA_vDisp(2, 1, "    ���ӷ�����...   ");

	for( i = 0; i < 8 ; i ++)
	{
		ret = connect(socketID,(struct sockaddr_in *)(&s_add), sizeof(struct sockaddr_in));	
		if( ret == 0)
		{
			return ok;
		}
	}

	if((ret<0)||(i>=8))
	{
		lcddisperr("   ���ӷ�����ʧ��  ");
		return notok;
	}
	return ok;
}



