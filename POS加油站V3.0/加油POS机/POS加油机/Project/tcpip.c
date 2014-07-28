
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
int pppn;    									//PPP链路号码
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
extern DEV_STAT 	DevStat;					//设备状态
extern INT8U  KeySector[14][6];					//M1卡个区密钥，每个区密钥为6字节
const unsigned char COM_END = '\r';				//命令结束字符

/******************************************************************************
 函数名称：GPRS_Close
 功能描述：GPRS关闭
 参数名称：输入/输出？	类型		描述
输入		    
//命令码说明:
				
 返  回  值：无
 
 作      者	：YFY
 日      期：2007-08-08
 修改历史：
		日期		修改人		修改描述
******************************************************************************/	
void GPRS_Close(void)
{
	EA_vCls();
	EA_vDisp(2, 1, "  GPRS模块正在关闭  ");
	EA_vDisp(3, 1, "    请稍候......    ");
	//DevStat.Gprs_On_Flag = 0;
	close_gprs_module();
//  (void)EA_uiInkey(1);
}

/******************************************************************************
 函数名称：FindStr
 功能描述：查找指蹲址串
 参数名称：输入/输出？	类型		描述
输入		    data	数据
				
 返  回  值：无
 
 作      者	：YFY
 日      期：2007-05-2
 修改历史：
		日期		修改人		修改描述
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
			if(index_num-i < 20)//数据报长度错误
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
			if(ret != 1)//验证CRC是否正确
			{
				sprintf((void *)str, "ret:%d,len:%d", ret, DataLength);
				lcddisperr((void *)str);
				return -2;
			}
			
			if(memcmp(RevBuff+19, "ER", 2) == 0)
			{
				lcddisperr("数据传输有问题");
				return -3;//数据传输有问题
			}
			if((memcmp(RevBuff+12, "TIME", 4)==0))//更新时间
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

			if( (memcmp(RevBuff + 12, "WHNM", 4) == 0) ) //查询白名单数量
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

			if( (memcmp(RevBuff+12, "UPYJ", 4) == 0) ) //油价表
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

			if( memcmp(RevBuff+12, "WHUP", 4) == 0 )      //更新白名单
			{
				memcpy(temp_data, RevBuff+16, 3);
				temp_data[3] = 0;	
				black_length = atoi(temp_data);				
				black_length = black_length / 5;         //每5个字节（ASCII）为一组车牌号
				EA_ucBlkOpen((void *)&blackFileName, &uiOpenID);

#ifdef _whitle_debug_
	char DISPBUF[100];
	Debugprintf((char *)RevBuff+19);
	sprintf(DISPBUF, "%04d", black_length);
	Debugprintf((char *)DISPBUF);
#endif
				tolanum += black_length; 
				sprintf((void *)str, "已下载条数%04lu", tolanum);
				EA_vDisplay(3, (void *)str);

				ret = EA_ucBlkAppendBase(uiOpenID, RevBuff+19, &black_length); 
				if( ret != EM_SUCCESS)
				{
					lcddisperr("白名单写入失败");
					EA_ucBlkClose(uiOpenID);
					return 0;
				}
				EA_ucBlkClose(uiOpenID);
				return 1;
			}

			if(memcmp(RevBuff+19, "OK", 2)==0)    //数据传输成功
				return 1;
		}
	}
	if(i == index_num)
		return -1;
	return 1;
}

/******************************************************************************
 函数名称：GPRS_TCP_Send
 功能描述：gprs发发送字符串
 参数名称：输入/输出？	类型		描述
 输入		    data	数据
    			length   数据长度
				timeout  超时限制
 返  回  值：无
 
 作      者	：YFY/刘及华
 日      期：2012-12-12
 修改历史：
		日期		修改人		修改描述
******************************************************************************/
INT8U GPRS_TCP_Send(unsigned char *Data, int length, unsigned short timeout)
{
	 size_t sndlen = 0;

	 timeout = timeout;
	 sndlen = (size_t)length;

	 if( write(socketID, Data, sndlen) == -1)
	 {
		 lcddisperr("发送时间失败");
		 return notok;
	 }
	 lcddisperr("发送时间成功");
     return ok;
}

/******************************************************************************
 函数名称：GPRS_TCP_Receive
 功能描述：gprs发发送字符串
 参数名称：输入/输出？	类型		描述
 输入		    data	数据
    			length   数据长度
				timeout  超时限制
 返  回  值：无
 
 作      者	：YFY/刘及华
 日      期：2012-12-12
 修改历史：
		日期		修改人		修改描述
******************************************************************************/
INT8U GPRS_TCP_Receive(unsigned char *Data, int *length,unsigned short timeout)
{
	size_t Rcvlen = 0;
	timeout = timeout;

	Rcvlen = (size_t)length;

	if(read( socketID, Data, Rcvlen ) == -1)
	{
		lcddisperr("接收失败");
		return notok;
	}
	lcddisperr("接收成功");
	return ok;
}

/*****************************************************************
 *	函数：GPRS_Send_Receive()
 *	功能：发送和接收GPRS数据
 *	输入参数：senddata--数据域；
 		      sendlength--数据长度;
 *	输出参数：无
 *	返回值：0--成功, ff--失败  
    		2--重新尝试连接
    		3--信号若，更换位置后再次连接
 *	说明：	正常关闭返回  
 *		   关闭前将清空接收缓冲区 	      
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
	
	//连接服务器
	if( ConnectServer() != ok )
	{
		close(socketID);
		lcddisperr("无法连接服务器");
		goto Err_sockoff;
	}

	crc = Cal_CRC((unsigned char*)sendbuf, sendlength);	
	sprintf((char*)STR, "%02X%02XEND\r", ((crc >> 8)& 0xff), (crc & 0xff));
	memcpy(sendbuf+sendlength, STR, 8);
	len = sendlength + 8;
	sendbuf[len] = '\0';

	if( write(socketID, sendbuf, len) == -1)
	{
		lcddisperr("TCP/IP发送失败");
		goto Err_sockoff;
	}	

	memset(recbuf, 0x00, sizeof(recbuf));

	for( i = 0; i < 500; i ++ )   //增加延时等待10秒钟，超时退出
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
		lcddisperr("TCP/IP接收失败");
		goto Err_sockoff;
	}
	recbuf[retlen] = '\0';

	ret = FindStr(recbuf, retlen);        //数据检测分包
	if(ret != 1)
	{
//  lcddisperr("数据格式校验错误");
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
 函数原型：open_gprs_module 
 功能描述：GPRS初始化并建立其连接
 参数描述： 无
 参数名称：	输入/输出？	类型		描述
			无
				
 返  回  值：ok ?? notok			   
 作      者	：刘及华
 日      期：2012-12-12
 修改历史：
	  	日期		修改人		修改描述
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
	EA_vDisplay(2, "     拨号初始化...  ");
//  EA_uiInkeyMs(500);
	//开机后，要求至少设置一次路由。对于只有一种通信方式时，才可以按下面值进行设置。若多种通信方式，请参考通讯通用库说明
	memset(&tRouteCFG, 0, sizeof(ET_ROUTE_CONFIG));

	strcpy( tRouteCFG.Destination, "0.0.0.0");
	strcpy( tRouteCFG.GateWay, "0.0.0.0");
	strcpy( tRouteCFG.Mask, "0.0.0.0");

	ucResult = EA_ucCommuExpand( (char *)"/dev/WLM", EM_CMD_SETROUTE, &tRouteCFG);
	if ( ucResult != EM_SUCCESS )
	{
		EA_vDisplay( 2, " 路由配置:%02X ", ucResult );
//  	(void)EA_uiInkeyMs(2000);
	}

    ucResult = EA_ucInitPpp( (char *)"/dev/WLM", &pppn );        //Init WirelessModule
    if ( ucResult != EM_SUCCESS )
    {
		//EA_vCls();
		//EA_vDisplay( 2, "   GPRS检测中...   ");
		//(void)EA_uiInkeyMs(1000);
		(void)EA_ucGetLinkState( pppn, PROTO_CONNECT_EVENT, &ucStatus );
		if( ucStatus == EM_ppp_SUCCESS )    //呼叫成功
		{
			DevStat.Gprs_On_Flag = 1;
			//EA_vCls();
			//EA_vDisplay( 3, "    网络连接成功    ");
			//(void)EA_uiInkeyMs(1000);
			return ok;
		}
	    EA_vDisplay( 4, "    拨号失败:%02X   ", ucResult );
		(void)EA_uiInkeyMs(2000);
        goto offdial;
	}

	EA_vCls();
    //EA_vDisplay( 2, "    PPP链路号:%02X    ", pppn );
	//(void)EA_uiInkeyMs(1000);

	//辅助设置，以及用户名，密码设置，建议预留设置界面

	memset(&gtPara, 0, sizeof(EA_TestPara));

	strcpy( gtPara.acUser, "");
	strcpy( gtPara.acPwd, "");
	gtPara.optionId = 0;
    strcpy(gtPara.stPara.acCenter , "ATD*99***1#");
//    if( DevStat.samcompcode == 1)
//    {
//        lcddisperr("移动专网");
////  	strcpy(gtPara.stPara.acPduContext , "AT+CGDCONT=1,\"IP\",\"qtd.zb.sd\"");//一卡通专网
//        strcpy(gtPara.stPara.acPduContext , "AT+CGDCONT=1,\"IP\",\"gjgs.zb.sd\"");//公交公司专网
//    }
//    else if( DevStat.samcompcode == 0)
//    {
//        lcddisperr("移动公网");
//        strcpy(gtPara.stPara.acPduContext , "AT+CGDCONT=1,\"IP\",\"CMNET\"");
//    }
	if( DevStat.samcompcode == 1)
	{
//  	lcddisperr("联通专网");
//  	strcpy(gtPara.stPara.acPduContext , "AT+CGDCONT=1,\"IP\",\"qtd.zb.sd\"");//一卡通专网
		strcpy(gtPara.stPara.acPduContext , "AT+CGDCONT=1,\"IP\",\"zbbus.sd\"");//公交公司专网
//  	strcpy(gtPara.stPara.acPduContext , "AT+CSTT=\"zbbus.sd\"");
	}                                          
	else if( DevStat.samcompcode == 0)
	{
//  	lcddisperr("联通公网");
		strcpy(gtPara.stPara.acPduContext , "AT+CGDCONT=1,\"IP\",\"uninet\"");
	}
	gtPara.stPara.uiSeconds = 0;    //可进行设置

	//设置用户名，密码
	ucResult = EA_ucAuthenPara( pppn , gtPara.acUser , gtPara.acPwd );            //input username and password

	if ( ucResult != EM_SUCCESS )
	{
		EA_vDisplay( 2, "    帐号设置错误:%02X   ", ucRet );
		(void)EA_uiInkeyMs(2000);
		goto offdial ;
	}

	EA_vCls() ;
    EA_vDisplay( 2, "    启动呼叫中...   " );
	(void)EA_uiInkeyMs(1000);
	//开始拨号
    ucResult = EA_ucCallPpp( pppn , &gtPara.stPara );         //start to dial
    if ( ucResult != EM_SUCCESS  )
    {
        EA_vDisplay( 2, "   启动呼叫失败:%02X  ", ucRet );
        (void)EA_uiInkeyMs(1000);
        goto offdial;
    }
	//等待拨号的结果，建议超时30-60s, 可以比 gtPara.stPara.uiSeconds 长几秒
	for( i = 80; i > 0; i-- )                 //60秒内循环查询状态数值
	{
		(void)EA_ucGetLinkState( pppn, PROTO_CONNECT_EVENT, &ucStatus );
		if( ucStatus == EM_ppp_SUCCESS )    //呼叫成功
		{
			DevStat.Gprs_On_Flag = 1;
			EA_vCls();
			EA_vDisplay( 3, "    网络连接成功    ");
			(void)EA_uiInkeyMs(2000);
			return ok;
		}
		else 
		{                                     //连接失败
			 EA_vDisplay( 2, "    网络连接中...   ");
			 (void)EA_vDelayMs(2000);
		}
	}
	//返回IP地址
	ucResult = EA_ucGetLocalDynIp( pppn , (char *)vIPBuf) ;
	if( ucResult == EM_SUCCESS )
	{

		EA_vDisplay(2, (char *)vIPBuf);
		SleepMs(2000);
		//(void)EA_uiInkey(0);
		return ok;
	}
	//挂断函数，超时时间内失败，建议再执行一次挂断
offdial:
    ucResult = EA_ucDialOffPpp( pppn ) ;           //start to hangup
    if ( ucResult != EM_SUCCESS )
    {
        EA_vDisplay( 4, "   启动挂断失败:%x  ", ucResult );
        EA_uiInkeyMs(2000);
        goto offdial;
    }
//等待挂断的结果
    for( i = 10; i > 0; i-- )
	{
		 (void)EA_ucGetLinkState( pppn, PROTO_TERM_EVENT, &ucStatus );
		 if( ucStatus == EM_SUCCESS )    //成功挂断
		 {
			 DevStat.Gprs_On_Flag = 0;
			 return notok;
		 }
		 else
		 {                                
			 lcddisperr("网络断开中...");
		 }
	}
	return notok;
}
/*****************************************************************
 函数原型：  close_gprs_module
 功能描述：GPRS关闭
 参数描述： 无
 参数名称：	输入/输出？	类型		描述
			无
				
 返  回  值：无				   
 作      者	：刘及华
 日      期：2012-12-12
 修改历史：
	  	日期		修改人		修改描述
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
        EA_vDisplay( 4, "   启动挂断失败:%x  ", ucRet );
        (void)EA_uiInkeyMs(2000);
        goto offdial;
    }
	for( i = 10; i > 0; i-- )
    {
        EA_ucGetLinkState( pppn , PROTO_TERM_EVENT , &ucStatus ) ;
		if( ucStatus == EM_SUCCESS )    //成功挂断
		{
			DevStat.Gprs_On_Flag = 0;
			return;
		}
		else 
		{                              //没有挂断EM_ppp_Doing 
			lcddisperr("网络断开中...");
		}
    }
	if(SIM_power_off() < 0) 
	{
		lcddisperr("关闭GPRS模块失败");	
	}
	else
	{
		lcddisperr("GPRS模块已关闭");	
		DevStat.Gprs_On_Flag = 0;
	}
}
/*****************************************************************
函数原型：SIM_power_on
 功能描述：给SIM卡上电
 参数描述：
 参数名称：	输入/输出？	类型		描述
				
 返  回  值：	ok  or  notok
 作      者	：刘及华
 日      期：2012-12-12
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
*****************************************************************/
uchar SIM_power_on(void) 
{
	uchar result;
	uchar prot;
	uint  len;
	uchar aucAtrData[50];

	EA_vCls();
	EA_vDisp(2, 1, "    检测SIM卡...     ");
	(void)EA_uiInkeyMs(2000);

	// 初始化IC卡，5V、EMV2000规范
	result= EA_ucICInitModule(hSIMHandle, EM_ic_VOLDEFAULT, EM_ic_EMVMODE);
	if (result != EM_SUCCESS)
	{
		 lcddisperr("SIM卡供电初始化失败");
		 goto releaseOperate;
	}
	// 等待卡插入
	while(EA_ucICCardIn(hSIMHandle) != EM_SUCCESS)
	{
		lcddisperr("请插入SIM卡");
		//EA_uiInkeyMs(500);
	}
	// 给卡上电
	result = EA_ucICPowerUp(hSIMHandle, &len, aucAtrData, &prot);
	if  (result != EM_SUCCESS)
	{
		lcddisperr("SIM卡上电失败");
		goto releaseOperate;
	}
	return ok;
releaseOperate: 
	// 该卡座暂时不使用，释放其控制权
	result = EA_ucCloseDevice(&hSIMHandle);
	return notok;
}
/*****************************************************************
函数原型：SIM_power_off
 功能描述：给SIM卡下电
 参数描述：
 参数名称：	输入/输出？	类型		描述
				
 返  回  值：无
 作      者	：刘及华
 日      期：2012-12-12
 修改历史：
		日期		修改人		修改描述
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


///*设置心跳*/
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
 函数名称：ConnectServer
 功能描述：连接服务器
 参数名称：输入/输出？	类型		描述
输入		    
//命令码说明:
				
 返  回  值：无
 
 作      者	：刘及华
 日      期：2012-12-12
 修改历史：
		日期		修改人		修改描述
******************************************************************************/
INT8U  ConnectServer(void)
{
	int  ret,i;
	int	 keepalive = 1;
	struct timeval tm;
	int keepIdle = 1;//1s
	int keepInterval = 2;//1.5s
	int keepCount = 10;//20次
	tm.tv_sec  = 2;
	tm.tv_usec = 0;
	char IP_addr[256];

	//EA_vCls();
	
	if((socketID = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		lcddisperr("    SOCK建立失败    ");
		return notok;
	}
	//发送时限
	ret = setsockopt(socketID, SOL_SOCKET, SO_SNDTIMEO, (char *)&tm, sizeof(struct timeval));
	if( ret == -1)
	{
		lcddisperr("   配置OPT发送失败  ");
		return notok;
	}
	//接收时限
	ret = setsockopt(socketID, SOL_SOCKET, SO_RCVTIMEO, (char *)&tm, sizeof(struct timeval));
	if( ret == -1)
	{
		lcddisperr("   配置OPT接收失败  ");
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
//  s_add.sin_addr.s_addr = inet_addr("192.168.2.7");    //一卡通网址
//  s_add.sin_addr.s_addr = inet_addr("192.168.2.17");		//公交公司网址
	s_add.sin_port = htons(5555);

	bzero(&(s_add.sin_zero), 8);

	EA_vDisp(2, 1, "    连接服务器...   ");

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
		lcddisperr("   连接服务器失败  ");
		return notok;
	}
	return ok;
}



