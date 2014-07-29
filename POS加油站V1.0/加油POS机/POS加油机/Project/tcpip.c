#include "main.h"



extern DEV_STAT 	DevStat;					//�豸״̬
//short GPRS_errno = GPRS_ERROR_INIT;		//ȫ�ִ���ָʾ����
extern INT8U  KeySector[14][6];								//M1��������Կ��ÿ������ԿΪ6�ֽ�
const unsigned char COM_END = '\r';		//��������ַ�
int  tmpch;
int  socketID;

//char  Response[4096];
//char  RecvHeadStr1[20],RecvHeadStr2[20];
extern DevHandle hSIMHandle;
extern DevHandle hWLMHandle;
//extern DevHandle hApptimerHandle;
extern uchar blackFileName[];
//EA_TestPara gtPara;             //local para
int pppn;    //PPP��·����
/*para*/
typedef struct
{
    ET_PPP_PARA    stPara ;     //only for gprs&cdma
    char    acUser[80] ;        //username
    char    acPwd[80] ;         //password
    int     optionId ;    //option file Id

}EA_TestPara;
EA_TestPara gtPara;
struct sockaddr_in s_add;
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
	DevStat.Gprs_On_Flag = 0;
	(void)EA_uiInkey(1);
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
	INT8U  	 i=0, j=0,k=0;
	INT8U    str[200];
	int		 DataLength = 0,ret = 0;
	INT8U    input_buffer[20];
	BUS_TIME LOCAL_V_TYPE time;

	char    temp_data[200], temp_black[1024];
	char  	RevBuff[1024];
	ulong	ulAppend = 1;
	ulong  	black_length=0;
	INT8U	black_name[1024];
	uint 	uiOpenID = 0;
	uchar   black_version[7+2];
	INT16U  black_number;   

	for(i=0;i<index_num;i++)
	{
		if((data[i] == '@') && (data[i+1]=='S') &&(data[i+2]=='T')&&(data[i+3]=='A')&&(data[i+4]=='R'))
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
				sprintf((void *)str,"ret:%d,len%d",ret,DataLength);
				lcddisperr((void *)str);
				return -2;
			}
			
			if(memcmp(RevBuff+19,"ER", 2) == 0)
			{
				lcddisperr("���ݲ���������");
				return -3;//���ݴ���������
			}
			if((memcmp(RevBuff+12, "TIME", 4)==0))//����ʱ��
			{		
				for(k=0;k<7;k++)
					input_buffer[k] = ((a_to_h(RevBuff[2*k+19]))<<4)|((a_to_h(RevBuff[2*k+1+19]))&0x0f);
				memcpy((INT8U*)&time, input_buffer, 7);
				if(CheckTimeFormat(&time)!=ok)
					return 1;
				Modify_Time(&time);
//  			Modify_GPRS_CLK();
				return 1;							
			}
			
			if((memcmp(RevBuff+12, "CALK", 4)==0))//������Կ
			{
				for(k=0;k<36;k++)
				{
					temp_data[k] = ((a_to_h(RevBuff[2*k+19]))<<4)|((a_to_h(RevBuff[2*k+1+19]))&0x0f);
				}
				for(j=0;j<6;j++)
				{
					memcpy(&KeySector[j+1][0], temp_data+6*j,6);
				//	sprintf(str, "%02x%02x%02x%02x%02x%02x", KeySector[j+1][0],KeySector[j+1][1],KeySector[j+1][2],KeySector[j+1][3],KeySector[j+1][4],KeySector[j+1][5]);
				//	printf_debug(str);
				}
				return 1;
			}

			if( (memcmp(RevBuff+12, "WHTV", 4) == 0) ) //����Ա�����
			{			
				if( memcmp(RevBuff+16,"020",3) == 0 )  //�µİ������汾��
				{
					for(k=0;k<7;k++)
					{
						black_version[k] = ((a_to_h(RevBuff[2*k+19]))<<4)|((a_to_h(RevBuff[2*k+1+19]))&0x0f);
					}
					
					    black_number = (INT16U)(a_to_h(RevBuff[33]))*100000+ 
											    (INT16U)(a_to_h(RevBuff[34]))*10000 +
											    (INT16U)(a_to_h(RevBuff[35]))*1000+ 
											    (INT16U)(a_to_h(RevBuff[36]))*100+ 
											    (INT16U)(a_to_h(RevBuff[37]))*10+ 
											    (INT16U)(a_to_h(RevBuff[38])); 

					if((memcmp(DevStat.black_version, black_version, 7) == 0) && 
					   (DevStat.black_name_number == black_number))
					{
						DevStat.black_flag = 0;
					}
					else
					{
						DevStat.black_flag = 1;
						DevStat.black_name_number = black_number;
						memcpy(DevStat.black_version, black_version, 7);
					}
					return 1;
				}
				return 0;
				
			}

			if( memcmp(RevBuff+12, "WHTD", 4)==0 )//������
			{
				memcpy(temp_data,RevBuff+16,3);
				temp_data[3] = 0;	
				black_length = atoi(temp_data);//(RevBuff[16]-0x30)*100+(RevBuff[17]-0x30)*10+(RevBuff[18]-0x30);				
				memcpy(temp_black, RevBuff+19, black_length);
				temp_black[black_length] = 0;			
				black_length = black_length/10;
				EA_ucBlkOpen((void *)&blackFileName, &uiOpenID);

				for(k=0;k<black_length;k++)
				{
					for(j=0;j<4;j++)
						black_name[j] = ((a_to_h(temp_black[2*(j+1)+k*10]))<<4)|((a_to_h(temp_black[2*(j+1)+1+k*10]))&0x0f);
					black_name[4] = 0;
//  				DB_add_record( DB_BLACK, black_name);
					EA_ucBlkUpdate(uiOpenID, NULL, 0, black_name, &ulAppend);
				}
//  			EA_ucBlkUpdate(uiOpenID, NULL, 0, black_name, &black_length);
				EA_ucBlkClose(uiOpenID);
				return 1;
			}
			if(memcmp(RevBuff+19,"OK", 2)==0)//���ݴ���ɹ�
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
 *	������GPRS_SEND_Receive()
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
INT8U GPRS_SEND_Receive(INT8U *senddata,int  sendlength)
{
	INT8U				STR[256],sendbuf[1024], recbuf[1024];
	INT16U				crc = 0;
	char				ret;
	int					retlen = 0;	
	int					len = 0;	

	memcpy(sendbuf, senddata, sendlength);
	sendbuf[sendlength] = 0;

//  EA_uiControlAppTimer(hApptimerHandle, EM_rtos_DISABLE_TIMER);
	
	//���ӷ�����
	if( ConnectServer() != ok )
	{
		close(socketID);
		lcddisperr("�޷����ӷ�����");
		goto Err_sockoff;
	}

	crc = Cal_CRC((unsigned char*)sendbuf, sendlength);	
	sprintf((char*)STR,"%02X%02XEND\r",((crc >> 8)& 0xff),(crc & 0xff));
	memcpy(sendbuf+sendlength,STR, 8);
	len = sendlength + 8;
	sendbuf[len] = 0;

	if( write(socketID, sendbuf, len) == -1)
	{
		lcddisperr("����ʧ��");
		goto Err_sockoff;
	}	

	memset(recbuf,0x00,sizeof(recbuf));
	retlen = read( socketID, recbuf, sizeof(recbuf) );
	if( retlen < 0)
	{
		lcddisperr("����ʧ��");
		goto Err_sockoff;
	}
		
	recbuf[retlen] = 0;
	ret = FindStr(recbuf, retlen);
	if(ret != 1)
	{
//  	lcddisperr("���ݸ�ʽУ�����");
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
 ����ԭ�ͣ�Open_GPRS_Module 
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
uchar Open_GPRS_Module(void)
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
		EA_vDisplay( 2, " ·������:%02x ", ucResult );
//  	(void)EA_uiInkeyMs(2000);
	}

    ucResult = EA_ucInitPpp( (char *)"/dev/WLM", &pppn );        //Init WirelessModule
    if ( ucResult != EM_SUCCESS )
    {
		EA_vCls();
		EA_vDisplay( 2, "   GPRS�����...   ");
		(void)EA_uiInkeyMs(1000);
		(void)EA_ucGetLinkState( pppn, PROTO_CONNECT_EVENT, &ucStatus );
		if( ucStatus == EM_ppp_SUCCESS )    //���гɹ�
		{
			DevStat.Gprs_On_Flag = 1;
			EA_vCls();
			EA_vDisplay( 3, "    �������ӳɹ�    ");
			(void)EA_uiInkeyMs(1000);
			return ok;
		}
	    EA_vDisplay( 4, "    ����ʧ��:%02x   ", ucResult );
		(void)EA_uiInkeyMs(2000);
        goto offdial;
	}

	EA_vCls();
    EA_vDisplay( 2, "    PPP��·��:%02x    ", pppn );
	(void)EA_uiInkeyMs(1000);

	//�������ã��Լ��û������������ã�����Ԥ�����ý���

	memset(&gtPara, 0, sizeof(EA_TestPara));

	strcpy( gtPara.acUser, "");
	strcpy( gtPara.acPwd, "");
	gtPara.optionId = 0;
    strcpy(gtPara.stPara.acCenter , "ATD*99***1#");
	if( DevStat.samcompcode == 1)
	{
		lcddisperr("�ƶ�ר��");
		strcpy(gtPara.stPara.acPduContext , "AT+CGDCONT=1,\"IP\",\"qtd.zb.sd\"");
	}
	else if( DevStat.samcompcode == 0)
	{
		lcddisperr("�ƶ�����");
		strcpy(gtPara.stPara.acPduContext , "AT+CGDCONT=1,\"IP\",\"CMNET\"");
	}
	gtPara.stPara.uiSeconds = 0;    //�ɽ�������

	//�����û���������
	ucResult = EA_ucAuthenPara( pppn , gtPara.acUser , gtPara.acPwd );            //input username and password

	if ( ucResult != EM_SUCCESS )
	{
		EA_vDisplay( 2, "    �ʺ����ô���:%02x   ", ucRet );
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
        EA_vDisplay( 2, "   ��������ʧ��:%02x  ", ucRet );
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
		lcddisperr((char *)vIPBuf);
//  	(void)EA_uiInkey(0);
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
 ����ԭ�ͣ�  Close_GPRS_Module
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
void Close_GPRS_Module(void) 
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
	DevStat.Gprs_On_Flag = 0;
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
void SIM_power_off(void)
{
	uchar ucRet;
	ucRet = EA_ucICPowerDown(hSIMHandle);
	if( ucRet == EM_SUCCESS )
	{

	}
	ucRet = EA_ucCloseDevice(&hSIMHandle);
 	if( ucRet == EM_SUCCESS )	
	{

	}	
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


///*
//����:����ͨ������
//����:
//����ֵ:
//1 ----------------------- ����ʧ��
//0 ------------------------ ���ӳɹ�
//�޸���ʷ:
//
//*/
//int GPRS_LINK(unsigned char *ip_addr,
//               unsigned char *ip_port)
//{
//    int i = 0,ret =0,j=0;
//
//    char buf[256],str[20];
///*
//    if(GPRS_INIT()!=ok)
//    {
//        return notok;
//    }
//*/
//    memset(buf, 0, sizeof(buf));
//
//    sprintf(buf, "AT+MIPOPEN=1,,\"%s\",%s,0\r", ip_addr, ip_port);
//
//    for(i=0;i<10;i++)
//    {
//        uart_str(buf);
//
//        EA_uiInkeyMs(4000);
//
//        ret=UART_wait_str2(15*64 ,-1, "+MIPOPEN: 1,1\r", "+MIPSTAT: 1,1");
//
//        if ( ret ==1 )
//        {
//            GPRS_Set_HeartBeat();
//            DispStr_CE(0, 6, "    ���ӳɹ�    ", DISP_CENTER);
//            return ok;
//        }
//    }
//
//    return notok;
//}

//
//INT8U  ConnectServer()
//{
//    unsigned short send_port;
//    char    *hostaddr, *senddata;
//    int snd_len;
//    int timeout;
//
//    struct  sockaddr_in sin;
//    int i, sock;
//    int on=1;
//
//    if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
//    {
//        //ErrorLog(ERROR, "socket err! errno[%d]", errno);
//        lcddisperr("SOCKET����");
//        return notok;
//    }
//
//    /* set socket options */
//    if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int)) < 0)
//    {
//        close(sock);
//        ErrorLog(ERROR, "setsockopt err! errno[ %d]", errno);
//        return( FAIL );
//    }
//
//    bzero(&sin, sizeof(sin));
//    sin.sin_family = AF_INET;
//    sin.sin_addr.s_addr = IpStringToUlong(hostaddr);
//    sin.sin_port = htons(send_port);
//    signal( SIGALRM, catch1 );
//
//    if( setjmp( position1 ) != 0 )
//    {
//        close( sock );
//        ErrorLog( ERROR, "setjmp err! errno[%d] timeout[%ld]", errno, timeout );
//        return( FAIL );
//    }
//    alarm( timeout );
//
//    if( connect(sock, (struct sockaddr *)&sin, sizeof(struct sockaddr_in)) < 0)     {
//        alarm( 0 );
//        close( sock );
//        ErrorLog(ERROR, "connect %s %d err! errno[%d]", hostaddr, send_port, errno);
//        return( FAIL );
//    }
//
//        i = write( sock, senddata, snd_len );
//    if( i != snd_len )
//    {
//        alarm( 0 );
//        close( sock );
//        ErrorLog( ERROR, "write fail ret[%ld] snd_len[%ld] errno[%d]", i, snd_len, errno);
//        return( FAIL );
//    }
//    alarm( 0 );
//
//    return( sock );
//}

//unsigned long IpStringToUlong(char *ip_addr)
//{
//    unsigned long l, m, n;
//    int i, j, k, len;
//    char buffer[10];
//
//    m = 0L;
//    l = 1L;
//    k = 0;
//    len = strlen(ip_addr);
//    for( i = 0; i < 4; i++, l = l*256L ){
//        memset(buffer, 0, 5);
//        for( j = 0;; j++ ){
//            if( (j+k) >= len || ip_addr[j+k] == '.' ){
//                k += j+1;
//                break;
//            }
//            buffer[j] = ip_addr[k+j];
//        }
//        n = atol(buffer);
//        if( n < 0L || n > 256L )
//            return notok;
//
//        m += n*l;
//    }
//
//    return m;
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
//	char IP_addr[256];

	EA_vCls();
	
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

//  sprintf((char *)IP_addr, "%03d.%03d.%03d.%03d", DevStat.ServerIp[0],DevStat.ServerIp[1],DevStat.ServerIp[2],DevStat.ServerIp[3]);
//  s_add.sin_addr.s_addr = inet_addr((char *)IP_addr);//"192.168.1.102");   123.185.210.253
	s_add.sin_addr.s_addr = inet_addr("172.168.22.100");   //123.185.210.253
	s_add.sin_port = htons(9001);

	bzero(&(s_add.sin_zero),8);

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



