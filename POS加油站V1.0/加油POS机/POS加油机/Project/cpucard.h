/*****************************************************************
Copyright 2003 大连现代高技术发展有限公司
All rights riserved.

文件名	：bus.h
模块名称：定义了公用的一些define

当前版本：0.0.1
修改人	：黄立国、许岩
完成日期：2004.09.02
 *****************************************************************/
#ifndef __CPUCARD_H__
#define __CPUCARD_H__
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


#define PREFERENTIAL_PRICE_CONSUMPTION 6				//交易类型为学生卡或月票卡优惠票价交易
#define TOTALRECORDNUM      			10200
// #define MAX_REC_NUM                     10200
#define MAX_REC_NUM						2000
#define BLACKNAMESTARTPAGE      	    300		//黑名单开始页
#define BLACKPAGENUM					300     //黑名单占用的页数
#define RECORDSTART         			600
#define RECORDPAGENUM         			3200
#define RECORDPERPAGE       			256
#define BYTESPERRECORD      			32
#define BYTESPERRECORDWITHCRC           34
#define COUNTER_MAX      				800
#define	FRAM_REC_NUM					16				//铁电中可以缓存16条纪录
#define	FRAM_REC_POS					1024			//铁电中纪录开始的位置

#define REC_ONE_POS						0x00		//第一条纪录的位置
#define REC_TWO_POS						0x00		//第二条纪录的位置
#define REC_THREE_POS					0x00		//第三条纪录的位置

#define CPU_CARD 						0x08
//#define M1_CARD 						0x04
#define M1_CARD 						0x02
#define DISP_DELAY_COUNTER_END		0x00
#define DISP_DELAY					600						//交易成功后，余额停留在屏幕上的时间
#define SINGLE_KEY_QUERY_DELAY		500					//单键查询时


#define LINE_NUM_HAVE_SET	0x01
#define BUS_NUM_HAVE_SET	0x02


//01	普通卡
//02	老年(半费)卡
//03	长寿卡
//05	纪念卡
//06	员工卡
//07	学生卡
//08	职工卡

//淄博公交卡片类型定义
//01：普通卡、
//02：学生卡、
//03：老年卡、
//04：免费卡（D卡）、
//05：纪念卡、
//06：员工卡

//普通卡
//学生卡
//老年卡
//免费卡（D卡）
//纪念卡
//员工卡

//卡类型
//消费卡
#define CARDT_NORMAL        0x01    //普通卡
#define CARDT_STUDENT		0x02    //老年卡(半费)
   
#define CARDT_OLDMAN   		0x03    //老年长寿卡
// #define CARDT_EMPLOYEE_A    0x04    //家属卡
#define CARDT_FREE			0x04    //职工卡
#define CARDT_MEMORY        0x05    //纪念卡
#define CARDT_DRIVER        0x06    //员工卡
 //#define CARDT_STUDENT     0x07    //学生卡
 //#define CARDT_FREE     0x08    //职工卡
 
 //管理卡
#define CARDT_SET           0x10    //线路票价车号设置卡
#define CARDT_GATHER        0x11    //数据采集卡
#define CARDT_SIGNED        0x12    //签点卡
#define CARDT_CHECKED       0x13    //检测卡
#define CARDT_INSPECTOR       0x18    //稽查卡


//证件类型
#define  IDENTIFY_TYPE 		0x01//1:身份证
#define  OLD_TYPE			0x02//2:老年证
#define  RETIRE_TYPE		0x03//3:离休证
#define  OFFICER_TYPE		0x04//4:军官证
#define	 RESIDENCE_TYPE		0x05//5:户口证
#define	 STUDENT_TYPE		0x06//6:学生证


// //消费卡
// #define CARDT_NORMAL		0x00	//普通卡
// #define CARDT_ADULT_MTH		0x02	//成人月票卡
// #define CARDT_STUDENT_MTH		0x03	//学生月票卡
// #define CARDT_EMPLOYEE_CHD	0x04	//员工子女卡
// #define CARDT_EMPLOYEE		0x05	//员工卡
// #define CARDT_MEMORY		0x06	//一次性卡
// #define CARDT_FRIEND			0x07	//友情卡
// //管理卡
// #define CARDT_DRIVER		0xA0	//司机卡
// #define CARDT_GATHER		0xA2	//数据采集卡
// #define CARDT_SET_NUM		0xA3	//线路车号设置卡
// #define CARDT_SET_FARE		0xA5	//票价设置卡
// #define CARDT_SIGNED_IN		0xA9	//入车调度卡
// #define CARDT_SIGNED_OUT	0xAA	//出车调度卡
// #define CARDT_MONTH_EN		0xAB	//月票有效卡
// #define CARDT_MONTH_DIS		0xAC	//月票无效卡

//数据盒类型
#define POS_T_NORMAL	0x34			//正常采集类型
#define POS_T_ABNORMAL	0x36			//异常采集类型


//定义时钟类型
#define RTC_3511    0x01
#define RTC_35180   0x02

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

//////////////////////////////////////////////
//定义交易类型
//added by myron for dandong flow 2005.2.18
//////////////////////////////////////////////
// #define TT_NORMAL_CONSUME_MONEY			0x01		//普通卡消费
// #define TT_ADULT_MTH_CONSUME_MONEY 		0x03		//成人月票卡消费钱包
// #define TT_STUDENT_MTH_CONSUME_MONEY 	0x05		//成人月票卡消费钱包
// #define TT_SIGN_IN						0x41		//入车调度卡消费类型
// #define TT_SIGN_OUT						0x42		//出车调度卡消费类型
// #define TT_DRIVER_WORKING				0x43		//司机上班类型
// #define TT_DRIVER_REST					0x44		//司机下班消费类型
// #define TT_GATHER						0x45		//采集卡消费类型
// #define TT_BLACKNAME_CONSUME_TYPE		0x13		//黑名单消费类型

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


// #define 	COMMON_CONSUME_TYPE			0x06		//普通消费类型
// #define 	FAVOURABLE_CONSUME_TYPE		0x08		//优惠消费类型
// #define 	ZERO_CONSUME_TYPE			0x09		//零消费类型
// #define 	BLACKNAME_CONSUME_TYPE		0X11		//黑名单消费类型
// #define 	DRIVER_WORKING_TYPE			0x12		//司机上班类型
// #define 	DRIVER_REST_TYPE			0x13		//司机下班消费类型
// #define 	CHECKED_CONSUME_TYPE		0x14		//检测消费类型
// #define 	SIGNED_CONSUME_TYPE			0x15		//签点消费类型
// #define 	INSPECTOR_CONSUME_TYPE		0x17		//稽查消费类型
// #define 	MANAGER_CONSUME_TYPE		0x16		//管理消费类型

//存储在FLASH中的关键数据的偏移量
#define	TYPEA_PARAM_START_POS			0
#define	TYPEB_PARAM_START_POS			100
#define	TYPEC_PARAM_START_POS			200
#define	TYPEA_PARAM_SIZE				100
#define	TYPEB_PARAM_SIZE				100
#define	TYPEC_PARAM_SIZE				100
//A类变量--需要频繁修改的，中用前100页（0--99）。
#define RECORD_NUMBER                   	TYPEA_PARAM_START_POS     	   //未上传的记录条数(原p0+4)              2字
//B类变量--修改比较频繁，每个变量分配两个页，总共占用100页（100--199）。
#define RECORD_BASE_PTR                 	(TYPEB_PARAM_START_POS)      //0－9保留     未上传记录指针起始指针(原p0+2)     2字节
#define LAST_RECORD_BASE_PTR       		(RECORD_BASE_PTR + 2)        //正常补采记录指针起始指针                2字节
#define LAST_RECORD_NUMBER          	  (LAST_RECORD_BASE_PTR +2)  //正常补采的记录条数                         2字节
#define CUR_DRIVER_NUMBER               	(LAST_RECORD_NUMBER +2)    //当班司机号(原p0+620)                    2字节
//#define driver_is_working                	(CUR_DRIVER_NUMBER +2)       //上下班标志(原p0+200)                    1字节
#define TIME_MODIFY_TIMES                  (driver_is_working +2)       //修改时间次数(原p0+687)                 1字节
#define BLACK_NAME_NUMBER				  (TIME_MODIFY_TIMES +2)			//黑名单数量			2字节

#define TYPEA_PARA_POSITION				(BLACK_NAME_NUMBER + 2)			//A参数存放的位置，范围0-99
//原C类变量，放弃使用C类变量，归到B里
#define LINE_NUMBER                           (TYPEA_PARA_POSITION + 2)                                         //线路号(原p0+48)                           2字节
#define BUS_NUMBER                          (LINE_NUMBER + 2)                  //车号(压缩BCD码,原p0+450)            4字节
#define NORMAL_CARD_FARE				(BUS_NUMBER + 2)				  //普通卡票价
#define STUDENT_CARD_FARE			   (NORMAL_CARD_FARE + 2)		//A种学生卡票价
#define OLDMAN_CARD_FARE				(STUDENT_CARD_FARE + 2)  //老年卡票价
#define EMPLOYEE_CARD_FARE				 (OLDMAN_CARD_FARE + 2)	//职工卡票价
#define DRIVER_CARD_FARE				 (EMPLOYEE_CARD_FARE + 2)		 //员工卡票价
//modified by myron for dandong flow 2005.2.21
//#define FAVORC_CARD_FARE   			 (DRIVER_CARD_FARE + 2)			 //C种优惠卡票价
//#define FAVORD_CARD_FARE   			 (FAVORC_CARD_FARE + 2)			 //D种优惠卡票价
//#define HAVE_BEEN_SET                      (FAVORD_CARD_FARE + 2)      //是否刷设置卡标志(原p0+638)           1字节

#define HAVE_BEEN_SET                      (DRIVER_CARD_FARE + 2)      //是否刷设置卡标志(原p0+638)           1字节
#define NVRAM_CYCLED			(HAVE_BEEN_SET + 2)		    //FLASH中存放记录是否翻过页 == TRUE 翻过 else 没翻过
//////////////////////////////////////////////
//added by myron 2005.2.17 for dandong device
//////////////////////////////////////////////
#define CONSUME_CARD_KINDS	(NVRAM_CYCLED+2)//消费卡种类 1字节
#define AM_STARTTIME_OLDMAN		(CONSUME_CARD_KINDS+2)//老年卡上午禁止乘车开始时间 2字节
#define AM_STOPTIME_OLDMAN		(AM_STARTTIME_OLDMAN+2)//老年卡上午禁止乘车停止时间 2字节
#define PM_STARTTIME_OLDMAN		(AM_STOPTIME_OLDMAN+2)//老年卡下午禁止乘车开始时间 2字节
#define PM_STOPTIME_OLDMAN		(PM_STARTTIME_OLDMAN+2)//老年卡下午禁止乘车停止时间 2字节
//////////////////////////////////////////////
//参数标志位:
//bit0--员工卡是否扣款标志位0:不扣款
//bit1--老年卡是否扣款标志位0:不扣款
//bit2--语音提示标志位0:请投币1:请买票
//其余位保留
//added by myron 2005.2.17
//////////////////////////////////////////////
#define PARAM_FLAG		(PM_STOPTIME_OLDMAN+2)//  1字节
//////////////////////////////////////////////
//定义刷卡间隔时间
//////////////////////////////////////////////
#define NORMAL_INTERVAL		(PARAM_FLAG+2)//  1字节
#define FAVORED_INTERVAL		(NORMAL_INTERVAL+2)//  1字节


#define		NO_KEY	(INT8U)0xFF


#define ChkStat(exp)      {	INT8U status = (exp); \
											if (status != ok)	 \
												return notok;	}

#define ChkStatPass(exp)      {	INT8U status = (exp); \
													if (status != ok)	 \
														return status;	}

//flash写入函数专用，如果函数返回不是OK，则返回写入失败
#define ChkFLWrStat(exp)      {	INT8U status = (exp); \
											if (status != ok)	 \
												return FL_WR_ERR;	}

//检查 WriteVariableToNVRAM() 和 ReadVariableFromNVRAM() 的返回参数
#define ChkNVStat(exp)      {	INT8U status = (exp); \
													if (status != ok)	 \
													{	DevStat.err_occured = TRUE;	\
															if ( (DevStat.err_code & (INT16U)NVRAM_ERROR) == 0 )	\
																DevStat.err_code += (INT16U)NVRAM_ERROR;		}	}

#ifdef CPU_CARD_DEBUG
	#define ADDQUEUE
	#define AddQueue(exp)	{	if ( (pDBCode - CPUDBCode) < sizeof(CPUDBCode) )	\
														*(pDBCode++) =(exp);	}
#endif

#ifdef M1_CARD_DEBUG
	#ifndef ADDQUEUE
		#define AddQueue(exp)	{	if ( (pDBCode - CPUDBCode) < sizeof(CPUDBCode) )	\
														*(pDBCode++) =(exp);	}
	#endif
#endif


#define ToggleWD()

#define resetpos() (*(void(*)())0)()

#define BeepInputErr()

/******************************************************************************
															部分struct定义部分
******************************************************************************/


//老年卡禁止乘车时间结构added by myron 2005.2.17
typedef struct _FORBIDDENTIME
{
	INT8U am_start[2];
	INT8U am_stop[2];
	INT8U pm_start[2];
	INT8U pm_stop[2];
}FORBIDDEN_TIME;

////////////////////////////////////////////
//增加管理员编号联合结构变量
//added by myron for dandong flow 2005.2.21
/////////////////////////////////////////////
typedef union _MANAGER_NUMBER_
{
	INT8U setter_number[3];     //设置员编号
	INT8U gather_number[3];     //采集员编号
	INT8U checker_number[3];    //监测员编号
	INT8U signer_number[3];     //签点员编号
}_MANAGER_NUMBER;


typedef struct _ID_TYPE_
{
	INT8U id_type;              //证件类型
	INT8U id_name[8];           //证件名称
}_ID_TYPE;

//结算单
typedef struct _BANLANCE_IN_HAND_
{
	INT8U	acnt_id[2];        //商户ID
	INT8U	equ_id[3];         //设备ID
	INT8U	oper_id[3];		   //当班结算操作员

	BUS_TIME work_datetime;	   //上班时间
	BUS_TIME down_datetime;	   //下班时间

	INT16U  consume_cnt;       // 加油次数
	INT32U	consume_amt;       //加油体积
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
	INT8U oper_cardno[5];				//操作员卡号
	INT16U record_base_ptr;				//未上传记录指针起始指针(原p0+2) 

	INT32U record_number;				//未上传的记录条数(原p0+4)
	INT8U POS_type;						//盒类型
	INT8U card_password[6];				//操作员密码
	INT8U Gprs_On_Flag;                 //gprs通道是否可用标志
	INT8U black_flag;
	INT16U current_record_index;

	//以下是需要存储到flash中的数据
	INT8U start;	
	INT8U ServerIp[4];                  //服务器地址
	INT8U ServerPort[2];                //服务器端口号
	INT8U dev_trade_no[2];              //设备交易序号

	INT16U  send_total_num;				//当前传送记录序号
	INT8U   cur_driver_number[4];		//当班司机号，下班清0------>改成当班操作员号
	INT8U   driver_is_working;
	INT16U  black_name_number;          //黑名单数量
	INT8U	black_version[7];           //版本号
	INT8U	bus_number[5];              //车牌号
	INT8S   user_passwd[7];			    //进入设置菜单的密码
	INT8U   fill_mode;				    //油品标示0，气体 1，柴油 2，汽油
	INT8U   samcompcode;			        //SAM卡厂商代码
	_BANLANCE_IN_HAND  binhand;         //结算单
	INT32U fare;			            //油价

/*--------------------20070131新增加的--------------------*/
	INT8U   acnt_id[2];                   //商户号编码
	INT8U   spot_id[2];                   //加油机编码
	INT8U   equ_id[3];                    //设备编码
	INT8U   isam_no[6];                   //ISAM卡卡号

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
		INT8U card_number[8];//卡号
		CARD_NUM detail_card_number;
	}_CARD_NUMBER;

	BUS_TIME card_in_time;			//交易时间
	INT8U   card_begin_dt[4];       //卡片启用日期yyyymmdd
	INT8U   card_useful_dt[4];      //卡片有效期yyyymmdd
	INT8U   card_add_dt[4];         //卡片加款时间yymmddhh
	
	INT8U  card_type;				//卡类型
	INT8U  fuel_type;				//加油型0，天然气，1，柴油 2，汽油
	INT32U fare;					//加油（量）单位
	INT32U cur_fare;				//当前应扣金额，CPU卡出错使用

	INT8U init_purchase_serial_num[2];		//初始的卡内消费交易序号
	INT8U purchase_serial_num[2];			//初始的卡内交易序号+1
//	INT8U init_credit_serial_num[2];		//初始的卡内充值交易序号
//	INT8U credit_serial_num[2];			    //卡内交易序号+1

	INT32U init_balance;					//原余额
	INT32U balance;							//交易后余额
	INT8U card_purchase_type;				//交易类型
	INT8U tac[4];							//TAC码
	INT8U mac2[4];							//CPU卡返回的MAC2

	INT8U bus_number[5];                    //车牌号码
	INT8U csn[4];							//硬件卡号
	INT8U authentication_code[4];			//卡认证码

	INT8U card_random_num[4];				//CPU卡随机数
	INT8U cpu_card_key_version;				//CPU卡密钥版本号
	INT8U arithmetic_sign;					//算法标识

	INT32U psam_purchase_serial_num;	    //PSAM卡交易序号
	INT8U mac1[4];							//PSAM卡计算出的MAC1

	//关于司机卡的上下班操作和员工卡
//	INT8U quit_work;						//卡片交易过程中，实现了下班操作
//	INT8U begin_work;						 //卡片交易过程中，实现了上班操作
	INT8U drv_card_type;					//司机卡类型

	/////////////////////////////////////////////
	//优惠卡相应信息
	/////////////////////////////////////////////
	INT16U init_time_balance;               //原次数余额
	INT16U time_balance;                    //交易后次数余额

	INT8U station_number[2];                //站点编号
	INT8U checked_flag;                     //检测标志
	_MANAGER_NUMBER manager_number;         //管理员编号 

	INT8U driver_number[4];			        //司机号

/*--------------20070131新增加的用户卡档案信息--------------------*/
	INT8U acnt_id[3];                       //结算中心编码
	INT8U spot_id[3];                       //售卡点编码
	INT8U equ_id[3];                        //设备编码
	INT8U key_type;                         //密钥类型
	INT8U pay_type;                         //付款方式
	INT8U deposit_amt[2];                   //押金
	INT8U isam_no[6];                       //ISAM卡卡号
//	INT8U oper_id[6];                       //操作员号
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
	INT8U spot_id[2];                //售卡点编码
	INT8U equ_id[3];                 //设备编码
	INT8U driver_id[4];              //操作员号
	INT8U isam_no[6];                //ISAM卡卡号
   	INT8U card_type;                 //卡片类型  1字节
	INT8U fuel_type;                 //燃料品种0,天然气，1，柴油，2，汽油
	INT8U card_purchase_type;        //交易类型	1字节
	INT8U pos_purchase_serial_num[3];//终端交易序号3字节
	INT8U card_serial_number[8];     //发行流水号	8字节
	INT8U bus_number[5];             //汽车牌号	5字节
	INT8U balance[3];                //交易后余额	3字节
	INT8U fare[3];                   //消费金额3字节
	BUS_TIME card_in_time;           //交易时间	7字节
	INT8U purchase_serial_num[2];    //初始的卡内交易序号+1	2字节
	INT8U tac[4];                    //TAC码	4字节
	INT16U crc;
}   RECORD;


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

/******************************************************************************
 函数名称：CPUGetCardTypeAndADF2
 功能描述：取得CPU卡的卡号和卡类型
 参数描述：
 参数名称：	输入/输出？	类型		描述
 card_type	输出		INT8U *		返回的CPU卡类型
 adf2_buf	输出	INT8U *	返回的1002目录的内容
 
 返  回  值：ok(0)-成功
				 notok(0xFF)-失败
				   
 作      者	：许岩
 日      期：2004-09-02
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
******************************************************************************/
//INT8U CPUGetCardTypeAndADF2(INT8U * card_type, INT8U * adf2_buf);

//丹东cpu卡 的相应信息都在 ADF1(1001目录中)  2005.02.22  hwr
/******************************************************************************
 函数名称：CPUCardInfoProcess
 功能描述：对CPU卡的信息进行处理, 包括黑名单处理, 设置卡判断, 启用标志,启用日期, 有效期判断
 参数描述：
 参数名称：	输入/输出？	类型		描述
 card_type	输出		INT8U *		返回的CPU卡类型
 
 返  回  值：ok(0)-成功
				 notok(0xFF)-失败（）
				 0x3c	黑名单交易完成标志
				   
 作      者	：许岩
 日      期：2004-09-02
 修改历史：
		日期		修改人		修改描述
	2005.02.22	    hwr  	       从原来CPU_Card_Consume_Process 中摘出一部分, 再加些修改
******************************************************************************/
//INT8U CPUCardInfoProcess(INT8U * ADF1_buf);

// *****************************************************************
// 功能：		蜂鸣
// 入口参数：	i=0:蜂鸣"嘀"一声
//				i=1:蜂鸣"嘀嘀嘀"三声
// 出口参数：	Null
// 作者：
// 函数出处：EH0218---COMMON.C例子程序
// *****************************************************************
void Beep(int i);

void Beeperr(void);
/******************************************************************************
 函数名称：SetBell
 功能描述：控制蜂鸣器
 参数描述：
 参数名称：	输入/输出？	类型		描述
 bell_switch	输入		INT8U	ON-响，OFF-不响
				
 返  回  值：无
				   
 作      者	：许岩
 日      期：2004-09-02
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
******************************************************************************/
//void SetBell(INT8U bell_switch);

/******************************************************************************
 函数名称：Gather_Card_Process
 功能描述：采集卡处理流程
 参数描述：
 参数名称：	输入/输出？	类型		描述
	INT8U const * const  gather_number	采集员编号
 
 返  回  值：ok(0)-成功
				 notok(0xFF)-失败
				   
 作      者	：许岩
 日      期：2004-09-02
 修改历史：
		日期		修改人		修改描述
		2005.03.01	HWR	-------------
******************************************************************************/
//INT8U Gather_Card_Process(INT8U const * const  gather_number);



// /******************************************************************************
//  函数名称：WriteVariableToNVRAM
//  功能描述：向NVRAM中存储数据，此函数与Important_Save功能重复，加此函数是为了移植用
//  参数描述：
//  参数名称： 输入/输出？ 类型        描述
// variable        输入        void *      要写入的变量
// offset          输入        INT16U const    此变量的偏移地址
// size            输入        INT8U       此变量的长度，sizeof(variable)
//
//  返  回  值：ok(0)-成功
//                  notok(0xFF)-失败
//                  0xFE-参数错误
//
//  作      者 ：许岩
//  日      期：2004-09-02
//  修改历史：
//         日期        修改人      修改描述
//         ------      ---------   -------------
// ******************************************************************************/
// INT8U WriteVariableToNVRAM(void *variable, INT16U const offset, INT8U const size);
//
//
//
// /******************************************************************************
//  函数名称：ReadVariableFromNVRAM
//  功能描述：从NVRAM中读出数据，此函数与Important_Restore功能重复，加此函数是为了移植用
//  参数描述：
//  参数名称： 输入/输出？ 类型        描述
// variable        输出        void *      要读取的变量
// offset          输入        INT16U const    此变量的偏移地址
// size            输入        INT8U       此变量的长度，sizeof(variable)
//
//  返  回  值：ok(0)-成功
//                  notok(0xFF)-失败
//                  0xFE-参数错误
//
//  作      者 ：许岩
//  日      期：2004-09-02
//  修改历史：
//         日期        修改人      修改描述
//         ------      ---------   -------------
// ******************************************************************************/
// INT8U ReadVariableFromNVRAM(void *variable, INT16U const offset, INT8U const size);
//
//
//
//  /*****************************************************************
//  函数原型： Important_Save()
//  功能描述： 把输入的数据写入第0页p0+n和p0+1024+n和p0+2048+n，并确保写入正确
//  参数描述： 参数名称        输入/输出？     类型            描述
//                  data_to_write   输入        INT8U*  要写入的数据
//                  offset              输入        INT8U*  要保护的数据在NVRAM中的偏移量0xC000+offset
//                  data_length     输入        INT8U   要写入的数据的长度  ，要求小于33
//
//  返回值：   ok(0)-写入成功；
//                 notok(1)-写入失败；
//                 0x02-data_length长度不对
//                 0x03-数据超出存储区范围
//
//  作  者：   大连现代/许岩
//  日  期：   2004-09-16
//  修改历史：
//         日期        修改人      修改描述
//         ------      ---------   -------------
// *****************************************************************/
// INT8U Important_Save(INT8U *data_to_write, INT16U const offset , INT8U const data_length);
//
//
//
// /*****************************************************************
//  函数原型： Important_Restore()
//  功能描述： 把第0页p0+n,p0+1024+n和p0+2048+n的数据读到data_to_read，确保数据读出正确
//  参数描述： 参数名称        输入/输出？     类型            描述
//
//                  data_to_read    输出       INT8U*   移植与兼容用，无意义
//                  ram_adr         输入        INT16U  要读取的数据在NVRAM中的偏移量0xC000+offset
//                  data_length     输入       INT8U    要对比的数据的长度  ，要求小于33
//
//  返回值：   ok(0)-写入成功；
//                 notok(1)-写入失败；
//                 0x02-data_length长度不对
//                 0x03-数据超出存储区范围
//
//  作  者：   大连现代/许岩
//  日  期：   2004-10-01
//  修改历史：
//         日期        修改人      修改描述
//         ------      ---------   -------------
// *****************************************************************/
// INT8U Important_Restore(INT8U *data_to_read, INT16U const offset , INT8U const data_length);



/*****************************************************************
 函数原型：	Fatal_NVRAM_Err()
 功能描述：ReadVariableFromNVRAM中3区数据不一致处理，显示内部参数错误(NVRAM错误E??)
 参数描述：	参数名称		输入/输出？		类型			描述
				 
 返回值：	无
 
 作  者：	大连现代/许岩
 日  期：	2004-09-17
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
*****************************************************************/
//void Fatal_NVRAM_Err(void);



/*****************************************************************
 函数原型：	Fatal_FLASH_Err()
 功能描述：致命的FLASH错误
 参数描述：	参数名称		输入/输出？		类型			描述
 pri_slv		输入		INT8U const		主FLASH还是从FLASH，＝PRI 或 ＝SLV
 
 返回值：	无
 
 作  者：	大连现代/许岩
 日  期：	2004-09-17
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
*****************************************************************/
//void Fatal_FLASH_Err(INT8U const pri_slv);


/*****************************************************************
 函数原型：Get_Sector_Permission
 功能描述：取得第N区的读写权限
 参数描述：
 参数名称：	输入/输出？	类型		描述
 sector		输入			INT8U		要取得权限的区
 key		输入			INT8U *		要取得权限区的密钥
 				
 返  回  值：ok(0)-成功
				 notok(0xFF)-失败
				   
 作      者	：黄立国、许岩
 日      期：2003-12-19
 修改历史：
	  	日期		修改人		修改描述
	  	------		---------	-------------
*****************************************************************/
//INT8U Get_Sector_PermissionA(INT8U sector, INT8U *key);

/*****************************************************************
 函数原型：Get_Sector_PermissionB
 功能描述：取得第N区的读写权限
 参数描述：
 参数名称：	输入/输出？	类型		描述
 sector		输入			INT8U		要取得权限的区
 key		输入			INT8U *		要取得权限区的密钥
				
 返  回  值：ok(0)-成功
				 notok(0xFF)-失败
				   
 作      者	：黄立国、许岩
 日      期：2003-12-19
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
		20070207    yufuyong	增加充值密钥验证
*****************************************************************/
//INT8U Get_Sector_PermissionB(INT8U sector, INT8U *key);

/******************************************************************************
 函数名称：AppendCPUCardRecord
 功能描述：向CPU管理卡内写记录
 参数描述：
 参数名称：	输入/输出？	类型		描述
 
 返  回  值：ok(0)-成功
				 notok(0xFF)-失败
				   
 作      者	：许岩
 日      期：2004-09-20
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
******************************************************************************/
//INT8U AppendCPUCardRecord(void);



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
 函数名称：Driver_Card_Process
 功能描述：司机卡处理流程
 参数描述：
 参数名称：	输入/输出？	类型		描述
 ADF2_buf	输入	INT8U *	1002目录的内容
 
 返  回  值：ok(0)-成功
				 notok(0xFF)-失败
				   
 作      者	：许岩
 日      期：2004-09-02
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
******************************************************************************/
//INT8U Driver_Card_Process(INT8U const * const ADF2_buf);



/******************************************************************************
 函数名称：BeginWorkProcess
 功能描述：上班操作
 参数描述：
 参数名称：	输入/输出？	类型		描述
 ADF2_buf	输入	INT8U *	1002目录的内容
 
 返  回  值：无
				   
 作      者	：许岩
 日      期：2004-09-20
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
******************************************************************************/
void BeginWorkProcess(INT8U const * const ADF2_buf);



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



/******************************************************************************
 函数名称：GetFare
 功能描述：根据卡类型取票价
 参数描述：
 参数名称：	输入/输出？	类型		描述
 card_type			输入		INT8U		卡类型
 
 返  回  值：票价
   
 作      者	：许岩
 日      期：2004-09-20
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
******************************************************************************/
//INT32U GetFare(INT8U card_type);



/******************************************************************************
 函数名称：M1CardProcess
 功能描述：M1卡交易流程
 参数描述：
 参数名称：	输入/输出？	类型		描述
 
 返  回  值：ok(0)-成功
				 notok(0xFF)-失败
				 0xF0-重新刷卡
				   
 作      者	：许岩
 日      期：2004-09-02
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
******************************************************************************/
//INT8U M1CardProcess(void);
/*****************************************************************
函数原型：FuncMenu
功能描述：本函数为功能菜单显示
参数描述：无
参数名称：	输入/输出？	类型		描述
-----------		-----------	------	   	-------
返 回 值：无
*****************************************************************/
void  FuncMenu(void);

void printf_debug(char * buf);
/******************************************************************************
 函数名称：StoreConsumeRecord
 功能描述：交易记录存储函数
 参数名称：	输入/输出？	类型		描述
 cardinfo	输入			CARD_INFO*	交易完毕后的卡片的所有信息
				
 返  回  值：无
 
 作      者	：许岩
 日      期：2004-09-23
 修改历史：
		日期		修改人		修改描述
		2005.2.18		myron	                     in function 
******************************************************************************/
//INT8U StoreConsumeRecord(CARD_INFO *cardinfo);

/******************************************************************************
 函数名称：Setting_Card_Process
 功能描述：设置卡处理流程
 参数描述：
 参数名称：	输入/输出？	类型		描述
 ADF2_buf	输入	INT8U *	1002目录的内容
 
 返  回  值：ok(0)-成功
				 notok(0xFF)-失败
				   
 作      者	：许岩
 日      期：2004-09-02
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
******************************************************************************/
//INT8U Setting_Card_Process(INT8U const * const ADF2_buf);



/******************************************************************************
 函数名称：Attemper_Card_Process
 功能描述：调度卡处理流程
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
//INT8U Attemper_Card_Process(void);
/******************************************************************************
 函数名称：Attemper_Card_Process
 功能描述：调度卡处理流程
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
//INT8U Student_Card_Process(void);


/******************************************************************************
 函数名称：Set_Bus_Number_Process
 功能描述：车号设定卡处理流程
 参数描述：
 参数名称：	输入/输出？	类型		描述
 
 返  回  值：
				   
 作      者	：许岩
 日      期：2004-09-02
 修改历史：
		日期		修改人		修改描述
		2005.2.21		myron			
******************************************************************************/
//void Set_Bus_Number_Process(void);



/*****************************************************************
 函数原型：M1_Card_Consume_Process
 功能描述：M1消费卡扣款流程
 参数描述：
 参数名称：	输入/输出？	类型		描述
 				
 				
 返  回  值：	ok(0)-消费成功		
 				0xFF-消费失败
 				//0xFC-不是本系统卡
				//0xFD-有卡出错，但没有回该卡
				0xF0-重新刷卡
 作      者	：许岩
 日      期：2004-09-27
 修改历史：
	  	日期		修改人		修改描述
	  	------		---------	-------------
*****************************************************************/
//INT8U M1_Card_Consume_Process(void);



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
函数原型：Get_Record
功能描述：取指定地址的记录

参数描述：
参数名称：	输入/输出？	类型		描述
-----------		-----------	------	   	-------
 rec_buf		输出	INT8U *	取回记录存放在此
 start_position	输入	INT16U		p0+2，即NewPtr，即DevStat.record_base_ptr
 rec_pointer	输入	INT16U		要取记录的指针，即DevStat.record_number

返  回  值：无
作      者	：黄立国、许岩
日      期：2004-10-10
修改历史：
日期		修改人		修改描述
------		---------	-------------
*****************************************************************/
//void Get_Record(INT8U *l_rec_buf, INT16U start_position, INT16U rec_pointer);



/*****************************************************************
函数原型：Get_RecordSingleFL
功能描述：取指定地址的记录，单FLASH启动专用

参数描述：
参数名称：	输入/输出？	类型		描述
-----------		-----------	------	   	-------
 pri_slv		输入		INT8U const		由主FLASH还是从FLASH启动，＝PRI 或 ＝SLV
 rec_buf		输出	INT8U *	取回记录存放在此
 start_position	输入	INT16U		p0+2，即NewPtr，即DevStat.record_base_ptr
 rec_pointer	输入	INT16U		要取记录的指针，即DevStat.record_number

返  回  值：无
作      者	：黄立国、许岩
日      期：2004-10-10
修改历史：
日期		修改人		修改描述
------		---------	-------------
*****************************************************************/
//void Get_RecordSingleFL(INT8U const pri_slv, INT8U *l_rec_buf, INT16U start_position, INT16U rec_pointer);



/*****************************************************************
函数原型：BlackName_Find
 功能描述：查找黑名单函数
 参数描述：
 参数名称：	输入/输出？	类型		描述
		card_number 		输入		INT32U			要查找的卡号，4字节
				
 返  回  值：	ok(0)-找到，是黑名单
				notok(0xFF)-没找到，不是黑名单
 
 作      者	：许岩
 日      期：2003-12-19
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
*****************************************************************/
//INT8U BlackName_Find(INT32U card_number);



/*****************************************************************
 函数原型：Lock_M1_Card_Process
 功能描述：M1卡黑名单在中，锁卡处理流程
 参数描述：
 参数名称：	输入/输出？	类型		描述
 				
 				
 返  回  值：	ok(0)-消费成功		
 				0xFF-消费失败
 作      者	：许岩
 日      期：2003-12-19
 修改历史：
	  	日期		修改人		修改描述
	  	------		---------	-------------
*****************************************************************/
//INT8U Lock_M1_Card_Process(void);



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
//void SingleFlashBoot(INT8U const pri_slv);


/******************************************************************************
 函数名称：M1_Driver_Card_Process
 功能描述：M1司机卡处理流程
 参数描述：
 参数名称：	输入/输出？	类型		描述
 无输入 	 输出状态码

 
 返  回  值：ok(0)-成功
			 notok(0xFF)-失败
				   
 作      者	：myron
 日      期：2005-02-17
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
******************************************************************************/
//INT8U M1_Driver_Card_Process(void);
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
/******************************************************************************
 函数名称：M1_Set_Card_Process
 功能描述：M1票价车号线路设置卡流程
 参数描述：
 参数名称：	输入/输出？	类型		描述
无输入		输出状态码
 
 返  回  值：ok(0)-成功
			 notok(0xFF)-失败
				   
 作      者	：myron
 日      期：2005-02-16
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
******************************************************************************/
//INT8U M1_Set_Card_Process(void);

/******************************************************************************
 函数名称：M1_Gather_Card_Process
 功能描述：M1采集卡处理流程
 参数描述：
 参数名称：	输入/输出？	类型		描述
无输入		输出状态码
 
 返  回  值：ok(0)-成功
			 notok(0xFF)-失败
				   
 作      者	：myron
 日      期：2005-02-16
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
******************************************************************************/
//INT8U M1_Gather_Card_Process(void);

/******************************************************************************
 函数名称：M1_Checked_Card_Process
 功能描述：M1检测卡处理流程
 参数描述：
 参数名称：	输入/输出？	类型		描述
无输入		输出状态码
 
 返  回  值：ok(0)-成功
			 notok(0xFF)-失败
				   
 作      者	：myron
 日      期：2005-02-16
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
******************************************************************************/
//INT8U M1_Checked_Card_Process(void);

/******************************************************************************
 函数名称：M1_Signed_Card_Process
 功能描述：M1签点卡处理流程
 参数描述：
 参数名称： 输入/输出？ 类型		描述
无输入		输出状态码
 
 返  回  值：ok(0)-成功
			 notok(0xFF)-失败
				   
 作 	 者 ：myron
 日 	 期：2005-02-16
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
******************************************************************************/
//INT8U M1_Signed_Card_Process(void);

/*****************************************************************
函数原型：Favor_Block_Is_Right
 功能描述：判断钱包区(16个字节)的数据是否正确
 参数描述：
 参数名称：	输入/输出？	类型		描述
 data_to_verify	输入	const INT8U *		要验证的数组的起始指针
				
 返  回  值：ok(0)-验证成功		notok(0xff)-验证失败
 作      者	：黄立国、许岩,myron
 日      期：2005.2.22
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
*****************************************************************/
//INT8U Favor_Block_Is_Right(const INT8U * data_to_verify);
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
//INT8U Check_Money(void);
/******************************************************************************
 函数名称：M1_Favor_Card_Process()
 功能描述：M1采集卡处理流程
 参数描述：
 参数名称：	输入/输出？	类型		描述
无输入		输出状态码
 
 返  回  值：ok(0)-成功
			 notok(0xFF)-失败
				   
 作      者	：myron
 日      期：2005-02-22
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
******************************************************************************/
//INT8U M1_Favor_Card_Process(void);


// *****************************************************************
// 功能：在屏幕上输出字符
// 入口参数：	lb->显示输出字符
// 出口参数：	Null
// 作者：
// 函数出处：EH0218---COMMON.C例子程序
// *****************************************************************
char HextoBcd(char sour);

/******************************************************************************
 函数名称：StoreRecord
 功能描述：交易记录存储函数
 参数名称：	输入/输出？	类型		描述
cardinfo	输入			CARD_INFO*	交易完毕后的卡片的所有信息
				
 返  回  值：无
 
 作      者	：许岩
 日      期：2004-09-23
 修改历史：
		日期		修改人		修改描述
		2005.2.18		myron	                     in function 
******************************************************************************/
//INT8U StoreRecord(CARD_INFO *cardinfo);

// *****************************************************************
// 功能：直接按键消费
// 入口参数：	lkeyvalue->按键值
// 出口参数：	Null
// 作者：
// 函数出处：EH0218---COMMON.C例子程序
// *****************************************************************
//char DirectConsume(long lkeyvalue);

/******************************************************************************
 函数名称：M1CardQuery
 功能描述：M1卡信息查询
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
//INT8U M1CardQuery(void);

// *****************************************************************
// 功能：检索卡片类型信息
// 入口参数：	lkeyvalue->按键值
// 出口参数：	Null
// 作者：
// 函数出处：EH0218---COMMON.C例子程序
// *****************************************************************
void GetCardType(INT8U  cardtype, INT8U * temp);



/*****************************************************************
函数原型：TRACE
功能描述：调试输出，类似printf，在屏幕上循环显示
参数描述：无
参数名称：	输入/输出？	类型		描述
-----------		-----------	------	   	-------
返 回 值：有
*****************************************************************/
//void TRACE(char * fmt, ...);
//
//void TRACE232(char * fmt, ...);



/******************************************************************************
 函数名称：htoa
 功能描述：hex转换为ascii码
 参数描述：
 参数名称： 输入/输出？ 类型		描述
				
 返  回  值：ok(0)-成功
			   	notok(0xFF)-失败
				   
 作 	 者 ：许岩
 日 	 期：2005-04-07
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
******************************************************************************/
//void htoa(void *des, INT8U *sour, int hex_len);



/******************************************************************************
 函数名称：CardQuery
 功能描述：卡信息查询
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
INT8U CardQuery(void);



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
//INT8U CPUCardQuery(void);



/******************************************************************************
 函数名称：EA_ucClrKeyBuf
 功能描述：读空键值
 参数描述：
 参数名称： 输入/输出？ 类型		描述
				
 返  回  值：无
				   
 作 	 者 ：许岩
 日 	 期：2005-04-07
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
******************************************************************************/
//void EA_ucClrKeyBuf(void);



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



/******************************************************************************
 函数名称：QueryFriend
 功能描述：友情卡、一次性卡信息查询
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
//INT8U QueryFriend(void);



// *****************************************************************
// 功能：		延时i毫秒
// 入口参数：	i->延时时间
// 出口参数：	Null
// 作者：
// 函数出处：EH0218---COMMON.C例子程序
// *****************************************************************
//void EA_uiInkeyMs(short i);



/*****************************************************************
 函数原型：OverProcess
 功能描述：M1消费卡扣款流程收尾
 参数描述：
 参数名称：	输入/输出？	类型		描述
				
				
 返  回  值：	ok(0)-消费成功		
				0xFF-消费失败
				//0xFC-不是本系统卡
				//0xFD-有卡出错，但没有回该卡
 作      者	：许岩
 日      期：2004-09-27
 修改历史：
		日期		修改人		修改描述
*****************************************************************/
//INT8U OverProcess(void);



/******************************************************************************
 函数名称：monitor
 功能描述：监控函数
 参数名称：	输入/输出？	类型		描述
				
 返  回  值：无
 
 作      者	：许岩
 日      期：2004-09-02
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
******************************************************************************/
//void monitor(void);



/******************************************************************************
 函数名称：CPU_Driver_Card_Process
 功能描述：CPU司机卡处理流程
 参数描述：
 参数名称：	输入/输出？	类型		描述
 无输入 	 输出状态码

 
 返  回  值：ok(0)-成功
			 notok(0xFF)-失败
				   
 作      者	：myron
 日      期：2005-02-17
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
******************************************************************************/
//INT8U CPU_Driver_Card_Process(void);



/******************************************************************************
 函数名称：Get_Passwd
 功能描述：验证密码进入设置初始化数据库状态
 参数描述：
 参数名称：	输入/输出？	类型		描述
				
 返  回  值：ok(0)-验证成功
				 notok(0xFF)-验证失败

 作      者	：许岩
 日      期：2005-05-20
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
******************************************************************************/
//INT8U Get_Passwd(void);



/******************************************************************************
函数名称：get_key
功能描述：得到一个按键，死等，只接受1234567890
参数描述：无

返  回  值	：该键值
作      者	：张坛
日      期：2005-04-08
修改历史：
日期		修改人		修改描述
------		---------	---------
******************************************************************************/
//INT8U get_key(void);



/******************************************************************************
 函数名称：trans_key
 功能描述：转换键值到ascii码
 参数描述：
 参数名称： 输入/输出？ 类型		描述
	key		输入		long		delay_and_wait_key的返回值，只接受0-9

 返  回  值：ascii码的键值
				   
 作 	 者 ：许岩
 日 	 期：2005-04-07
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
******************************************************************************/
//INT8U trans_key(long key);



/******************************************************************************
 函数名称：ascii_to_hex
 功能描述：把一个0-9的ascii代码，转化为16进制数
 参数名称： 输入/输出？ 类型        描述
 num        输入        INT8U           要转化为ascii代码的数，要求范围0-9

 返  回  值：转化后的16进制数

 作      者 ：许岩
 日      期：2004-09-02
 修改历史：
		日期        修改人      修改描述
		------      ---------   -------------
******************************************************************************/
//INT8U ascii_to_hex(INT8U num);



/******************************************************************************
 函数名称：CPU_Card_Consume_Process
 功能描述：CPU普通卡处理流程
 参数描述：
 参数名称：	输入/输出？	类型		描述
 无输入 	 输出状态码

 
 返  回  值：ok(0)-成功
			 notok(0xFF)-失败
			 0xF0-重新刷卡
				   
 作      者	：许岩
 日      期：2005-02-17
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
******************************************************************************/
//INT8U CPU_Card_Consume_Process(void);



// *****************************************************************
// 功能：检索卡片类型信息
// 入口参数：	lkeyvalue->按键值
// 出口参数：	Null
// 作者：
// 函数出处：EH0218---COMMON.C例子程序
// *****************************************************************
//void GetTradeType(INT8U  cardtype, INT8U * temp);



/*****************************************************************
函数原型：bcdhex
功能描述：bcd码转换为hex码
参数描述：
参数名称：	输入/输出？	类型		描述
-----------		-----------	------	   	-------
bcd				输入			INT8U		输入的bcd码

返  回  值：转换后的hex码
作      者	：许岩
日      期：2006-01-09
修改历史：
日期		修改人		修改描述
------		---------	-------------
*****************************************************************/
//INT8U bcdhex(INT8U bcd);



/*****************************************************************
函数原型：CPUCard_Application_Lock
 功能描述：锁CPU卡
 参数描述：
 参数名称：	输入/输出？	类型		描述
				
 返  回  值：	ok(0)-成功		
				notok (0xFF)(-1)-失败或卡片已经被锁
 作      者	：许岩
 日      期：2004-10-11
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
*****************************************************************/
//INT8U CPUCard_Application_Lock(void);



// *****************************************************************
// 功能：		浏览菜单
// 入口参数：	startline->起始行
//				p_menu->菜单
//				p_cPtr->当前行
//				p_lPtr->在菜单中所处的位置
// 出口参数：	Null
// 返回:		
// 作者：	
// 函数出处：EH0218---COMMON.C例子程序
// *****************************************************************
//short browse_menu(int startline,unsigned char *p_menu,int *p_cPtr,int *p_lPtr );


/******************************************************************************
 函数名称：ascii
 功能描述：把一个0-9的数转化为ascii代码，此函数也可扩展用于通讯中，通讯中涉及到一些A-F的非法输入，认为合法
 参数名称：	输入/输出？	类型		描述
 num		输入		INT8U			要转化为ascii代码的数，要求范围0-9
 
 返  回  值：转化后的ascii代码
 
 作      者	：许岩
 日      期：2004-09-02
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
******************************************************************************/
//INT8U ascii(INT8U num);



/******************************************************************************
 函数名称：M1_Inspector_Card_Process
 功能描述：M1稽查卡处理流程
 参数描述：
 参数名称：	输入/输出？	类型		描述
无输入		输出状态码
 
 返  回  值：ok(0)-成功
			 notok(0xFF)-失败
				   
 作      者	：myron
 日      期：2005-02-16
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
******************************************************************************/
//INT8U M1_Inspector_Card_Process(void);



/******************************************************************************
 函数名称：monitor_normal
 功能描述：正常消费模式监控函数
 参数名称：	输入/输出？	类型		描述
				
 返  回  值：无
 
 作      者	：许岩
 日      期：2004-09-02
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
******************************************************************************/
void monitor_normal(void);



// *****************************************************************
// 功能：直接按键消费
// 入口参数：	无
// 出口参数：	Null
// 作者：
// 函数出处：EH0218---COMMON.C例子程序
// *****************************************************************
INT8U NormalConsume(INT32U fare_in);



// *****************************************************************
// 功能：		PHoldTime_Setting
// 说明:		刷卡持续时间设置
// 入口参数：	
// 出口参数：	Null
// 作者：
// 返回值：		-1:关机					
// *****************************************************************
int PHoldTime_Setting(void);



// *****************************************************************
// 功能：		MaxPriceLimitSet
// 说明:		设置最大消费金额
// 入口参数：	
// 出口参数：	Null
// 作者：
// 返回值：							
// *****************************************************************
INT8U MaxPriceLimitSet(void);

/*****************************************************************
函数原型：CPU_SET_PROCESS
功能描述：本函数为CPU设置卡处理过程
参数描述：无
参数名称：	输入/输出？	类型		描述
-----------		-----------	------	   	-------
返 回 值：无
*****************************************************************/
int CPU_SET_PROCESS(INT8U * data);


/*****************************************************************
函数原型：CPU_OPER_PROCESS
功能描述：本函数为CPU操作员卡处理过程
参数描述：无
参数名称：	输入/输出？	类型		描述
-----------		-----------	------	   	-------
返 回 值：     -1:上一个操作员没有签退
 		       -2:操作员密码错误
 		       -3:卡片已经过有效期
 		       -4:卡片已经停用
 		       -5: 没有可用的有效次数 
 		       1:  成功
*****************************************************************/
int CPU_OPER_PROCESS(INT8U * data);

/*****************************************************************
函数原型：Login
功能描述：本函数为操作员登陆窗口
参数描述：无
参数名称：	输入/输出？	类型		描述
-----------		-----------	------	   	-------
返 回 值：	-1:无卡
			  -2:选择目录错误
			  -3:口令验证错误
			  -4:

*****************************************************************/
int CpuProcess(void);


// *****************************************************************
// 功能：		Login
// 说明:		cpu卡登陆
// 入口参数：	
// 出口参数：	Null
// 作者：
// 返回值：							
// *****************************************************************
void Login(void);


/******************************************************************************
 函数名称：CardInit
 功能描述：卡片初始化,完成找卡工作
 参数描述：
 参数名称：输入/输出？	类型		描述
 
 返  回  值：    ok(0)-成功
				 notok(0xFF)-失败
				 0xFE-没找到卡
				 0xF0-重新刷卡
				   
 作      者	：于福勇/刘及华
 日      期：2012-12-10
 修改历史：
		日期		修改人		修改描述
******************************************************************************/
//INT8U CardInit(void);

/******************************************************************************
 函数名称：SetCardUseDT
 功能描述：设置卡片有效期
 参数描述：
 参数名称：输入/输出？	类型		描述
 
 返  回  值：    ok(0)-成功
				 notok(1)-失败
				 				   
 作      者	：于福勇
 日      期：2007-01-30
 修改历史：
		日期		修改人		修改描述
******************************************************************************/
INT8U SetCardUseDT(void);

/******************************************************************************
 函数名称：GetConsumeTradeAmt
 功能描述：获得交易金额
 参数描述：
 参数名称：输入/输出？	类型		描述
 
 返  回  值：    ok(0)-成功
				 notok(1)-失败
				 				   
 作      者	：于福勇
 日      期：2007-01-30
 修改历史：
		日期		修改人		修改描述
******************************************************************************/
INT8U GetConsumeTradeAmt(void);

/******************************************************************************
 函数名称：GetTradeAmt
 功能描述：获得交易金额
 参数描述：
 参数名称：输入/输出？	类型		描述
 
 返  回  值：    ok(0)-成功
				 notok(1)-失败
				 				   
 作      者	：于福勇
 日      期：2007-01-30
 修改历史：
		日期		修改人		修改描述
******************************************************************************/
INT8U GetTradeAmt(void);



/******************************************************************************
 函数名称：GetTradeInfo
 功能描述：获得持卡人交易信息
 参数描述：
 参数名称：输入/输出？	类型		描述
 
 返  回  值：    ok(0)-成功
				 notok(1)-失败
				 				   
 作      者	：于福勇
 日      期：2007-01-30
 修改历史：
		日期		修改人		修改描述
******************************************************************************/
INT8U GetTradeInfo(void);


/******************************************************************************
 函数名称：GetPersonInfo
 功能描述：获得持卡人信息
 参数描述：
 参数名称：输入/输出？	类型		描述
 
 返  回  值：    ok(0)-成功
				 notok(0xFF)-失败
				 				   
 作      者	：于福勇
 日      期：2007-01-30
 修改历史：
		日期		修改人		修改描述
******************************************************************************/
INT8U GetPersonInfo(void);

/*****************************************************************
 函数原型：M1_Card_Credit_Process
 功能描述：M1消费卡充值流程
 参数描述：
 参数名称：	输入/输出？	类型		描述
				
				
 返  回  值：	ok(0)-充值成功		
				0xFF-充值失败
				//0xFC-不是本系统卡
				//0xFD-有卡出错，但没有回该卡
				0xF0-重新刷卡
 作      者	：许岩
 日      期：2004-09-27
 修改历史：
		日期		修改人		修改描述
		2005.2.21		myron			in function
*****************************************************************/
//INT8U M1_Card_Credit_Process(void);

/*****************************************************************
函数原型：OperCardCheck
功能描述：本函数为检测是否有操作员卡在，
				//并判断是否为同一张卡
参数描述：无
参数名称：	输入/输出？	类型		描述
-----------		-----------	------	   	-------
返 回 值：	-1:无卡
			  -2:选择目录错误
			  -3:口令验证错误
			  -4:

*****************************************************************/
int OperCardCheck(void);

/******************************************************************************
 函数名称：M1CardProcess
 功能描述：M1卡交易流程
 参数描述：
 参数名称：	输入/输出？	类型		描述
 
 返  回  值：ok(0)-成功
				 notok(0xFF)-失败
				 0xF0-重新刷卡
				   
 作      者	：许岩
 日      期：2004-09-02
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
******************************************************************************/
INT8U CheckCardCredit(void);

/*****************************************************************
函数原型：CpuDecrement
功能描述：本函数为操作员卡扣款
参数描述：无
参数名称：	输入tFlag---交易类型:0---扣款交易，1:充值交易
///输出？	类型		描述
-----------		-----------	------	   	-------
返 回 值：	-1:无卡
			  -2:选择目录错误
			  -3:口令验证错误
			  -4:

*****************************************************************/
int CpuDecrement(INT8U * data, INT32U trade_amt);

/*****************************************************************
函数原型：CpuIncrement
功能描述：本函数为操作员卡扣款
参数描述：无
参数名称：	输入tFlag---交易类型:0---扣款交易，1:充值交易
///输出？	类型		描述
-----------		-----------	------	   	-------
返 回 值：	-1:无卡
			  -2:选择目录错误
			  -3:口令验证错误
			  -4:

*****************************************************************/
int CpuIncrement(INT8U * data, INT32U trade_amt);
/*****************************************************************
函数原型：INT8U Logout(void)
功能描述：本函数为签退功能
参数描述：无
参数名称：	输入/输出？	类型		描述
-----------		-----------	------	   	-------
返 回 值：ok  ?  notok
*****************************************************************/
INT8U Logout(void);

/*****************************************************************
函数原型：OperTradeProcess
功能描述：本函数为操作员卡交易函数
参数描述：无
参数名称：	输入tFlag---交易类型:0---扣款交易，1:充值交易
///输出？	类型		描述
-----------		-----------	------	   	-------
返 回 值：	-1:无卡
			  -2:选择目录错误
			  -3:口令验证错误
			  -4:

*****************************************************************/
int OperTradeProcess(int tFlag, INT32U trade_amt);

/*****************************************************************
函数原型：Cpucard_Init
功能描述：本函数初始化SAM OR CPU卡通道
参数描述：无
参数名称：	输入/输出？	类型		描述
-----------		-----------	------	   	-------
返 回 值：无
*****************************************************************/

INT8U Cpucard_Init(void);

/******************************************************************************
 函数名称：Cpucard_power_on
 功能描述：复位CPU卡
 参数描述：
 参数名称：	输入/输出？	类型		描述  
   
 返  回  值：ok(0)-成功
			 notok(0xFF)-失败
				   
 作      者	：刘及华
 日      期：2012-12-12
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
******************************************************************************/
INT8U Cpucard_power_on(INT8U *prcvbuf, uint *prcvLen);

/*****************************************************************
函数原型：Cpucard_power_off
功能描述：本函数关闭通道
参数描述：无
参数名称：	输入/输出？	类型		描述
-----------		-----------	------	   	-------
返 回 值：无
*****************************************************************/
void Cpucard_power_off(void);

/*****************************************************************
函数原型：CPU_OPER_DOWN
功能描述：本函数为CPU操作员卡下班处理
参数描述：无
参数名称：	输入/输出？	类型		描述
-----------		-----------	------	   	-------
返 回 值：   无
*****************************************************************/
void CPU_OPER_DOWN(void);



/******************************************************************************
 函数名称：GrayCardDeal
 功能描述：卡片补偿操作
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
//INT8U GrayCardDeal(void);

/*****************************************************************
函数原型：CharToCash
 功能描述：判断钱包区的余额->INT32U的数据
 参数描述：
 参数名称：	输入/输出？	类型		描述
 buf			输入		const INT8U *		要验证的数组的起始指针
				
 返  回  值：INT32U的钱
 作      者	：许岩
 日      期：2004-09-29
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
*****************************************************************/
INT32U CharToCash(const INT8U *buf);





/******************************************************************************
 函数名称：CardCreditAmt
 功能描述：充值交易流程
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
INT8U CardCreditAmt(void);

/******************************************************************************
 函数名称：CardSale
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
INT8U CardSale(void);
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

//INT8U M1_Card_Consume_ProcessA(void);
/******************************************************************************
 函数名称：StoreGrayRecord
 功能描述：灰记录存储函数
 参数名称：	输入/输出？	类型		描述
				
 返  回  值：无
 
 作      者	：于福勇
 日      期：2007-04-29
 修改历史：
		日期		修改人		修改描述
******************************************************************************/
//INT8U StoreGrayRecord(void);
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
uchar download_whiltelist(void);



#endif

