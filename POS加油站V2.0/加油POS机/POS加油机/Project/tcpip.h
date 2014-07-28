#ifndef  _TCPIP_H_
#define  _TCPIP_H_

#include "data_type.h"


//delay unit
#define TEN_SECOND		    640	//ten seconds
#define NINE_SECOND		    576
#define EIGHT_SECOND		512
#define SEVEN_SECOND		448
#define SIX_SECOND		    384
#define FIVE_SECOND		    320
#define FOUR_SECOND		    256
#define THREE_SECOND		192
#define TWO_SECOND		    128
#define ONE_SECOND		    64
#define HALF_SECOND		    32
#define	QUARTER_SECOND		16
#define	EIGHTH_SECOND		8
#define	SIXTEENTH_SECOND	4	//sixteenth second

#define  GPRS_SEND_RETRY 10

//network registration status
#define NET_STAT_NOT_SEARCH	    0
#define NET_STAT_REG_HOME	    1
#define	NET_STAT_SEARCHING	    2
#define NET_STAT_REG_DENIED	    3
#define	NET_STAT_UNKNOWN	    4
#define	NET_STAT_REG_ROAMING	5



#define serv_port 9001
//SIM100/SIM300 TCP/IP status
#define SIM_STATUS_IP_INITIAL		0	//Initializing state
#define SIM_STATUS_IP_START		    1	//starting state
#define SIM_STATUS_IP_IND		    2  	//activating PDP context/establishing CSD connection
#define	SIM_STATUS_IP_GPRSACT		3	//PDP context activated/CSD connection established
#define SIM_STATUS_IP_STATUS		4	//local IP address got
#define SIM_STATUS_IP_CONNECTING	5	//connecing
#define SIM_STATUS_IP_CLOSE		    6	//connection closed
#define SIM_STATUS_IP_CONNECT_OK	7	//TCP/UDP connection established
#define SIM_STATUS_IP_CONFIGURE     8
#define SIM_STATUS_IP_ERROR         9   //

//Possible Values of SIM_errno
#define SIM_ERROR_INIT			801	//initial value of SIM_errno
#define SIM_ERROR_NO			802	//can find the error number in the message

//Possible Values of GPRS_errno
#define GPRS_ERROR_INIT			810	//initial value of GPRS_errno
#define GPRS_ERROR_NEVER		812	//Unknow error
#define GPRS_SIM_ERROR			813	//SIM300 error
#define GPRS_UART_OVERFLOW		814	//UART error

#define GPRS_NULL_POINTER		820	//NULL pointer parameter
#define GPRS_INVALID_INT		821	//Integer parameter error

#define GPRS_NO_ANSWER			830	//The module can't respons to command
#define GPRS_REV_TIMEOUT		831	//no receive data overtime
#define GPRS_ERR_SETUP			840	//module initialize error
#define GPRS_NO_SIM			    841	//not be able detect the SIM card
#define GPRS_NEED_SIM_PIN		842	//need input SIM PIN

#define GPRS_LINK_PARAM			854	//SIM300:the link parameters error
#define GPRS_LINK_ERROR			855	//SIM300:create link error
#define GPRS_SEND_ERROR			857	//SIM300:tcp send error
#define GPRS_REV_FORMAT			858	//SIM300:tcp receive error
#define GPRS_SIM_PARAM			859	//SIM300:command format error

#define GPRS_UART_ERROR			900	//module serial port error


uchar SIM_power_on(void);
int SIM_power_off(void);
void close_gprs_module (void);
INT8U  ConnectServer(void);
INT8U GPRS_Send_Receive(INT8U *senddata,int  sendlength);

//short RetrieveDateTime(void);
//short GPRS_TCP_Shut(void);


/*****************************************************************
 函数原型：open_gprs_module 
 功能描述：GPRS开启
 参数描述： 无
 参数名称：	输入/输出？	类型		描述
			无
				
 返  回  值：ok ?? notok			   
 作      者	：刘及华
 日      期：2012-12-12
 修改历史：
	  	日期		修改人		修改描述
*****************************************************************/
uchar open_gprs_module (void);

//void Modify_GPRS_CLK(void);
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
//char GPRS_TCP_Send(unsigned char *Data,int length,unsigned short timeout);
/******************************************************************************
 函数名称：GPRS_TCP_Receive
 功能描述：gprs接收字符串
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
INT8U GPRS_TCP_Receive(unsigned char *Data,int *length,unsigned short timeout);

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
INT8U GPRS_TCP_Send(unsigned char *Data,int length,unsigned short timeout);

void GPRS_Close(void);
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
int   FindStr(unsigned char * data, int index_num);
//unsigned long IpStringToUlong(char *ip_addr);
#endif
