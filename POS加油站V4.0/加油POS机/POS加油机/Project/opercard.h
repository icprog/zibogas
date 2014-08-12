/*****************************************************************
Copyright 2003 大连易达通有限公司
All rights riserved.

文件名	：bus.h
模块名称：定义了公用的一些define

当前版本：0.0.1
修改人	：许岩
完成日期：2004.09.02
 *****************************************************************/
#ifndef __OPERCARD_H__
#define __OPERCARD_H__
#include "data_type.h"


#define BUS_TYPE_NORMAL     8		//普通车
#define BUS_TYPE_LUXURY     6		//豪华车

//新增GPRS通讯状态码
#define AT_OK				0//成功
#define AT_ERR				1//失败
#define AT_TIMEOUT			2//超时
#define AT_QUIT				3//人为退出


#ifdef DEBUG
	#define FIVE_MINUTES	1			//刷卡时间兼隔
	#define FIVE_SECONDS	1			//刷卡时间兼隔
#else
	#define FIVE_MINUTES	300			//刷卡时间兼隔
//	#define FIVE_SECONDS	5			//刷卡时间兼隔
	#define FIVE_SECONDS	2			//刷卡时间兼隔
#endif

#ifndef USE_PDATA
	#define x_pdata xdata
#else
	#define x_pdata 
#endif

#ifndef LOCAL_VARIABLE_NOT_USE_IDATA
	#define LOCAL_V_TYPE 
#else
	#define LOCAL_V_TYPE
#endif
#define  idata

#define COM_BUFFER_SIZE 100

#define MAX_REC_NUM						2000
#define CPU_CARD 						0x08
//#define M1_CARD 						0x04
#define M1_CARD 						0x04
#define DISP_DELAY						600					//交易成功后，余额停留在屏幕上的时间
#define SINGLE_KEY_QUERY_DELAY			500					//单键查询时


//卡类型
//消费卡
#define CARDT_NORMAL        0x01    //普通卡
#define CARDT_STUDENT		0x02    //老年卡(半费)
#define CARDT_OLDMAN   		0x03    //老年长寿卡
#define CARDT_FREE			0x04    //职工卡
#define CARDT_MEMORY        0x05    //纪念卡
#define CARDT_DRIVER        0x06    //员工卡


 //管理卡
#define CARDT_SET           0x10    //线路票价车号设置卡
#define CARDT_GATHER        0x11    //数据采集卡
#define CARDT_SIGNED        0x12    //签点卡
#define CARDT_CHECKED       0x13    //检测卡
#define CARDT_INSPECTOR      0x18    //稽查卡


//证件类型
#define  IDENTIFY_TYPE 		0x01//1:身份证
#define  OLD_TYPE			0x02//2:老年证
#define  RETIRE_TYPE		0x03//3:离休证
#define  OFFICER_TYPE		0x04//4:军官证
#define	 RESIDENCE_TYPE		0x05//5:户口证
#define	 STUDENT_TYPE		0x06//6:学生证


//数据盒类型
#define POS_T_NORMAL	0x34			//正常采集类型
#define POS_T_ABNORMAL	0x36			//异常采集类型


//消费模式
#define NORMAL 0x00				//正常
#define SHORTCUT 0x01			//快捷

//////////////////////////////////////////////
//定义老年卡员工卡是否允许消费
//标志
//added by myron for dandong flow 2005.3.30
//////////////////////////////////////////////
#define OLDMAN_ENABLE_CONSUME	0x02	 //允许老年卡消费
#define DRIVER_ENABLE_CONSUME	0x01	 //允许员工卡消费

#define TT_NORMAL_CONSUME					0x06		//卡片消费
#define TT_NORMAL_CONSUME_MONEY				0x02		//卡片冲帐
#define TT_CREDIT_MONEY						0x01        //卡片充值
#define TT_CARD_SALE						0x04		//卡片售卡
//#define TT_SETTING				  	 0x10       	//线路票价设置卡
#define TT_FREE								0x07		//免费卡消费
#define TT_BLACKNAME_CONSUME_TYPE			0x11		//黑名单消费类型
#define TT_DRIVER_WORKING					0x12		//司机上班类型
#define TT_DRIVER_REST						0x13		//司机下班类型
#define TT_CHECK							0x14		//检测卡类型
#define TT_SIGN								0x15		//签点卡类型
#define TT_INSPECTOR						0x17		//稽查卡类型
#define TT_MANAGE							0x16		//其他管理卡类型

#define TT_GRAY_SALE						0x94		//售卡灰记录
#define TT_GRAY_CREDIT						0x91		//充值灰记录
#define TT_GRAY_FLUSH						0x92		//冲账灰记录

#define MAX_INPUT_LENGTH 10

#define		NO_KEY	(INT8U)0xFF


#define ChkStat(exp)      {	INT8U status = (exp); \
											if (status != ok)	 \
												return notok;	}

#define ChkStatPass(exp)      {	INT8U status = (exp); \
													if (status != ok)	 \
														return status;	}


#define ToggleWD()

#define resetpos() (*(void(*)())0)()

#define BeepInputErr()

/******************************************************************************
															部分struct定义部分
******************************************************************************/

//结算单
typedef struct _BANLANCE_IN_HAND_
{
	INT8U	acnt_id[2];        //商户ID
	INT8U	equ_id[3];         //设备ID
	INT8U	oper_id[3];		   //当班结算操作员

	BUS_TIME work_datetime;	   //上班时间
	BUS_TIME down_datetime;	   //下班时间
//这里应该分为四类取统计、记录日结结果
	INT8U   consume_kind[4];      //加油种类
	INT16U  consume_cnt[4];    //加油次数
	INT32U	consume_cap[4];    //加油体积
	INT32U	consume_amt[4];    //加油金额
}_BANLANCE_IN_HAND;

//设备状态，最重要的变量，如果增加变量，请在SysInit里进行初始化
typedef struct _DEV_STAT
{
	INT8U err_occured;					// == TRUE出错 ＝＝FALSE设备正常
	INT16U err_code;					//出错代码

	INT8U allow_upload_data;			//采集标志
	INT8U databox_cmd_rcvd;				//是否接收到数据盒的命令
	INT8U purchase_again_req;			//重新刷卡语音标志＝＝TRUE播放重新刷卡 ==FALSE不播放
	INT8U voice_purchase_again_freq;
	INT8U mode;                             
	//PSAM卡相关
	INT8U cur_psam_channel;				//当前使用的PSAM卡通道 = PSAM_PRI 或 ＝ PSAM_SLV
	INT8U key_version;					//PSAM卡密钥版本号
	INT8U pos_number[6];				//POS号，又名终端号，又名PSAM卡编号
	INT16U record_base_ptr;				//未上传记录指针起始指针(原p0+2) 

	INT32U record_number;				//未上传的记录条数(原p0+4)
	INT8U POS_type;						//盒类型
	INT8U card_password[6];				//操作员密码
	INT8U Gprs_On_Flag;                 //gprs通道是否可用标志
	INT8U black_flag;
	INT16U current_record_index;

	//以下是需要存储到flash中的数据
	INT8U   start;	
	INT8U   ServerIp[4];                  //服务器地址
	INT8U   ServerPort[2];                //服务器端口号
	INT8U   dev_trade_no[3];              //设备交易序号
	INT8U   oper_cardno[3];				//操作员卡号

	INT16U  send_total_num;				//当前传送记录序号
	INT8U   cur_driver_number[4];		//当班司机号，下班清0------>改成当班操作员号
	INT8U   driver_is_working;          //操作员登录标志
	INT16U  black_name_number;          //黑名单数量
	INT8U	black_version[7];           //版本号
	INT8U	bus_number[5];              //车牌号
	INT8S   user_passwd[7];			    //进入设置菜单的密码
	INT8U   samcompcode;			    //SAM卡厂商代码
	_BANLANCE_IN_HAND  binhand;         //结算单
										//
	INT8U   fill_mode;				    //油品标示0，气体 1，汽油93#，2.汽油97#，3.柴油 
	INT32U  price[4];                   //单价

/*--------------------20070131新增加的--------------------*/
	INT8U   acnt_id[2];                 //商户号编码
	INT8U   equ_id[3];                  //设备编码
	BUS_TIME effect_time;               //油价更新生效时间
	INT32U  price_backup[4];            //待更新的燃料单价
//  INT8U   update_price_done;			//1,表示已经更新过了，0，表示没有更新
	INT8U   request_all_rec;
	INT8U   end;
}DEV_STAT;


//CARD_INFO中用到的struct
typedef struct _CARD_NUM
{
	INT8U city_code[2];	            //城市代码
	INT8U calling_code[2];          //行业代码
	INT8U serial_number[4];		    //序列号
}CARD_NUM;

//卡片信息，交易记录由此变量生成
typedef struct _CARD_INFO
{
	union _CARD_NUMBER_
	{
		INT8U card_number[8];  //卡号
		CARD_NUM detail_card_number;
	}_CARD_NUMBER;

	BUS_TIME card_in_time;			//交易时间
	INT8U   card_begin_dt[4];       //卡片启用日期yyyymmdd
	INT8U   card_useful_dt[4];      //卡片有效期yyyymmdd
	INT8U   card_add_dt[4];         //卡片加款时间yymmddhh
	
	INT8U  card_type;				//卡类型
	INT8U  fuel_type;				//加油型0，天然气，1，柴油 2，汽油
	INT32U capacity;				//加油（量）单位L
	INT32U price;					//加油单价
	INT32U fare;					//加油金额
	INT32U cur_fare;				//当前应扣金额，CPU卡出错使用

	INT8U init_purchase_serial_num[2];		//初始的卡内消费交易序号
	INT8U purchase_serial_num[3];			//初始的卡内交易序号+1

	INT32U init_balance;					//原余额
	INT32U balance;							//交易后余额
	INT8U card_purchase_type;				//交易类型
	INT8U tac[4];							//TAC码
	INT8U mac2[4];							//CPU卡返回的MAC2

	INT8U bus_number[6];                    //车牌号码
	INT8U csn[4];							//硬件卡号
	INT8U authentication_code[4];			//卡认证码

	INT8U card_random_num[4];				//CPU卡随机数
	INT8U cpu_card_key_version;				//CPU卡密钥版本号
	INT8U arithmetic_sign;					//算法标识

	INT32U psam_purchase_serial_num;	    //PSAM卡交易序号
	INT8U mac1[4];							//PSAM卡计算出的MAC1

	//关于司机卡的上下班操作和员工卡
	INT8U drv_card_type;					//司机卡类型

	/////////////////////////////////////////////
	//优惠卡相应信息
	/////////////////////////////////////////////
	INT16U init_time_balance;               //原次数余额
	INT16U time_balance;                    //交易后次数余额

	INT8U station_number[2];                //站点编号
	INT8U checked_flag;                     //检测标志


	INT8U driver_number[3];			        //司机号

/*--------------20070131新增加的用户卡档案信息--------------------*/
	INT8U acnt_id[3];                       //结算中心编码
	INT8U spot_id[3];                       //售卡点编码
	INT8U equ_id[3];                        //设备编码
	INT8U key_type;                         //密钥类型
	INT8U pay_type;                         //付款方式
	INT8U deposit_amt[2];                   //押金
	INT8U isam_no[6];                       //ISAM卡卡号
	INT8U card_status;                      //卡片状态
	INT8U name[8];                          //持卡人姓名
	INT8U sex;                              //持卡人性别
	INT8U id_type;                          //持卡人证件类型
	INT8U id_no[18];                        //持卡人证件号码
	INT8U dev_trade_no[2];                  //设备交易序号
	INT8U credit_rec_pos;                   //圈存记录保存地址
	INT8U blockcreditrec[16];	            //出错卡重入是把此buf写回卡内

	INT8U card_record_point;				//消费记录区指针
	
}CARD_INFO;

//出错卡状态
typedef struct _ERROR_CARD_INFO
{
	INT8U error_occured;						//==TRUE，扣款发生错误，==FALSE，扣款正确
	INT8U sale_error_occured;					//==TRUE，扣款发生错误，==FALSE，扣款正确

	INT32U init_balance;						//原余额
	INT32U init_cpu_card_balance;
	/////////////////////////////////////////
	//added by myron for dandong flow 2005.2.22
	/////////////////////////////////////////
	INT16U init_time_balance;	//原次数余额	
	//cpu
	INT8U cpu_card_number[8];			        //CPU卡号
	INT8U purchase_serial_num[2];		        //交易序号(加过1)
	//m1
	INT8U block24_buf_to_restore[16];	        //出错卡重入是把此buf写回卡内
	INT8U m1_card_number[4];				    //M1卡卡号
	INT8U csn[4];								//出错卡硬件卡号
	INT8U trade_type;                           //交易类型add by yfy 20070206
}ERROR_CARD_INFO;

//交易记录

typedef  struct  _RECORD
{
	INT8U acnt_id[2];                //结算中心编码
	INT8U equ_id[3];                 //设备编码
	INT8U oper_id[3];                //操作员号
	INT32U price;                    //油品单价
	INT8U card_serial_number[8];     //卡的发行流水号	8字节
	INT8U card_type;                 //卡片类型  1字节
	INT8U card_purchase_type;        //交易类型	1字节
                                     

	INT8U pos_purchase_serial_num[3];//终端交易序号3字节
	INT8U bus_number[5];             //汽车牌号	5字节
	INT8U driver_no[3];              //员工卡卡号
    INT32U fare;                     //消费金额   4字节
	INT32U capacity;                 //加油体积  4字节
	INT8U fuel_type;                 //燃料品种:0.天然气，1.汽油93，2.汽油97，3.柴油
	BUS_TIME card_in_time;           //交易时间	7字节
									 

	INT8U purchase_serial_num[2];    //初始的卡内交易序号+1	2字节
	INT8U tac[4];                    //TAC码	4字节
	INT16U crc;
}   RECORD;

typedef struct _GPRS
{
	INT16U record_base_ptr;                 //未上传记录指针起始指针(原p0+2)
	INT16U record_number;                   //未上传的记录条数(原p0+4)

	INT8U have_driver_rec;                  //是否是司机上下班记录
	INT16U snd_len;                         //发送长度
	INT16U rcv_len;                         //接收长度

	INT8U have_drec;						//是否有未上传的drec，是=TRUE 否=FALSE
	INT8U update_time;						//是否准备与服务器同步时间，是=TRUE 否=FALSE
	INT8U driver_is_working;				//上下班标志

	INT16U package_num;						//包数
	INT16U last_packgae_rec_num;			//最后一包的记录数

}GPRS;

//GPRS gprs;

typedef struct _PREVCARDINFO
{
	INT8U csn[4];	
	BUS_TIME card_in_time;         	//交易时间	
}PREVCARDINFO;



#define resetpos() (*(void(*)())0)()


/******************************************************************************
 函数名称：SaveCardInTime
 功能描述：入卡时保存时间
 参数描述：
 参数名称：	输入/输出？	类型		描述
 card_info	输入		CARD_INFO *		准备要保存时间的变量的指针
 
 返  回  值：ok(0)-成功
				 notok(0xFF)-失败
				   
 作      者	：许岩
 日      期：2004-09-02
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
******************************************************************************/
INT8U SaveCardInTime(CARD_INFO *card_info);

// *****************************************************************
// 功能：		蜂鸣
// 入口参数：	i=0:蜂鸣"嘀"一声
//				i=1:蜂鸣"嘀嘀嘀"三声
// 出口参数：	Null
// 作者：
// 函数出处：EH0218---COMMON.C例子程序
// *****************************************************************
void Beep(int i);
/*****************************************************************
函数原型：TextHeadDisplayTimer(void)
功能描述：本函数为在开头的位置显示时间
参数描述：无
参数名称：	输入/输出？	类型		描述
-----------		-----------	------	   	-------
返 回 值：ok  ?  notok
*****************************************************************/
void TextHeadDisplayTimer(void);

// *****************************************************************
// 功能：		蜂鸣
// 入口参数：	i=0:蜂鸣"嘀"一声
//				i=1:蜂鸣"嘀嘀嘀"三声
// 出口参数：	Null
// 作者：
// 函数出处：EH0218---COMMON.C例子程序
// *****************************************************************
void Beeperr(void);

/******************************************************************************
 函数名称：QuitWorkProcess
 功能描述：下班操作
 参数描述：
 参数名称：	输入/输出？	类型		描述
 
 返  回  值：无
				   
 作      者	：许岩
 日      期：2004-09-20
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
******************************************************************************/
void QuitWorkProcess(void);


/******************************************************************************
 函数名称：CheckPeriodOfValidity
 功能描述：有效期判断
 参数描述：
 参数名称：	输入/输出？	类型		描述
 start			输入		INT32U		启用日期
 current		输入		INT32U		当前日期
 end			输入		INT32U		有效截止日期
 
 返  回  值：ok(0)-成功
				 notok(0xFF)-失败(过期)
				   
 作      者	：许岩
 日      期：2004-09-20
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
******************************************************************************/
INT8U CheckPeriodOfValidity(INT32U start, INT32U current, INT32U end);

void printf_debug(char * buf);


/*****************************************************************
函数原型：Sec6_Block_Is_Legal
 功能描述：判断一个区(16个字节)的数据是否正确，判断最后4字节是否为18E718E7
 参数描述：
 参数名称：	输入/输出？	类型		描述
 data_to_verify	输入	const INT8U *		要验证的数组的起始指针
 				
 返  回  值：ok(0)-验证成功		
				notok(0xff)-验证失败
 作      者	：许岩
 日      期：2003-10-14
 修改历史：
	  	日期		修改人		修改描述
	  	------		---------	-------------
*****************************************************************/
INT8U Sec6_Block_Is_Legal(const INT8U * data_to_verify);


/*****************************************************************
函数原型：Purse_Block_Is_Right
 功能描述：判断钱包区(16个字节)的数据是否正确
 参数描述：
 参数名称：	输入/输出？	类型		描述
 data_to_verify	输入	const INT8U *		要验证的数组的起始指针
				
 返  回  值：ok(0)-验证成功		notok(0xff)-验证失败
 作      者	：黄立国、许岩
 日      期：2003-10-14
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
*****************************************************************/
INT8U Purse_Block_Is_Right(INT8U * data_to_verify);



/*****************************************************************
函数原型：BlockToCash
 功能描述：判断钱包区的余额格式->INT32U的数据
 参数描述：
 参数名称：	输入/输出？	类型		描述
 buf			输入		const INT8U *		钱包区的余额格式
				
 返  回  值：INT32U的钱
 作      者	：许岩
 日      期：2004-09-29
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
*****************************************************************/
INT32U BlockToCash(const INT8U *buf);



/*****************************************************************
函数原型：BlockToCash
 功能描述：INT32U的数据->判断钱包区的余额格式
 参数描述：
 参数名称：	输入/输出？	类型		描述
 cash		输入		INT32U		要扣的款
				
 返  回  值：指向扣款block格式的指针
 作      者	：许岩
 日      期：2004-09-29
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
*****************************************************************/
unsigned long CashToBlock(INT32U cash);



/*****************************************************************
函数原型：CalTAC
 功能描述：计算TAC码，M1卡交易流程专用
 参数描述：
 参数名称：	输入/输出？	类型		描述
				
 返  回  值：无
 作      者	：许岩
 日      期：2004-10-08
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
*****************************************************************/
void CalTAC(void);

/*****************************************************************
函数原型：  query_rec_num(void)
 功能描述：查询记录条数
 参数描述：
 参数名称：	输入/输出？	类型		描述
				
 返  回  值：无
 作      者	：刘及华
 日      期：2004-10-08
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
*****************************************************************/
uint query_rec_num(void);
/*****************************************************************
函数原型：CalConsumeTAC
 功能描述：计算TAC码，M1卡交易流程专用
 参数描述：
 参数名称：	输入/输出？	类型		描述
				
 返  回  值：无
 作      者	：许岩
 日      期：2004-10-08
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
*****************************************************************/
void CalConsumeTAC(void);

/*****************************************************************
 函数原型：	SingleFlashBoot
 功能描述：单FLASH启动，用于一块FLASH失效，采集另一块FLASH的数据
 参数描述：	参数名称		输入/输出？		类型			描述
 pri_slv		输入		INT8U const		由主FLASH还是从FLASH启动，＝PRI 或 ＝SLV
 
 返回值：	无
 
 作  者：	大连现代/许岩
 日  期：	2005-01-31
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
*****************************************************************/
uchar  Sector6_Process(void);
/*****************************************************************
函数原型：DisplayPurchaseResult
功能描述：显示交易结果(扣款成功后)
参数描述：
参数名称：	输入/输出？	类型		描述
cardinfo	输入			const CARD_INFO*	交易完毕后的卡片的所有信息

返  回  值：无
				
作      者	：许岩
日      期：2004-09-23
修改历史：
日期		修改人		修改描述
2005.2.21		myron	                   in function 
*****************************************************************/
int DisplayPurchaseResult(const CARD_INFO *cardinfo);


// *****************************************************************
// 功能：在屏幕上输出字符
// 入口参数：	lb->显示输出字符
// 出口参数：	Null
// 作者：
// 函数出处：EH0218---COMMON.C例子程序
// *****************************************************************
char HextoBcd(char sour);


// *****************************************************************
// 功能：检索卡片类型信息
// 入口参数：	lkeyvalue->按键值
// 出口参数：	Null
// 作者：
// 函数出处：EH0218---COMMON.C例子程序
// *****************************************************************
void GetCardType(INT8U  cardtype, INT8U * temp);

/******************************************************************************
 函数名称：CPUCardQuery
 功能描述：CPU卡信息查询
 参数描述：
 参数名称：	输入/输出？	类型		描述
 
 返  回  值：ok(0)-成功
				 notok(0xFF)-失败
				   
 作      者	：许岩
 日      期：2004-09-02
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
******************************************************************************/
INT8U QueryNormal(void);


INT8U Logout(void);

/******************************************************************************
 函数名称：CardFlush
 功能描述：售卡交易流程
 参数描述：
 参数名称：输入/输出？	类型		描述
 
 返  回  值：    ok(0)-成功
				 notok(0xFF)-失败
				 0xFE-没找到卡
				 0xF0-重新刷卡
				   
 作      者	：于福勇
 日      期：2007-01-30
 修改历史：
		日期		修改人		修改描述
******************************************************************************/
INT8U CardFlush(void);

/******************************************************************************
 函数名称：download_whiltelist(void)
 功能描述：下载最新的白名单
 参数名称：无
 输入		   无
				
 返  回  值：无
 
 作      者	：刘及华
 日      期：2013-06-2
 修改历史：
		日期		修改人		修改描述
******************************************************************************/
INT8U download_whiltelist(void);
/******************************************************************************
 函数名称：download_whiltelist(void)
 功能描述：下载最新的白名单
 参数名称：无
 输入		   无
				
 返  回  值：无
 
 作      者	：刘及华
 日      期：2013-06-2
 修改历史：
		日期		修改人		修改描述
******************************************************************************/
INT8U download_price_table(void);

INT8U Is_timeup(void);

#endif

