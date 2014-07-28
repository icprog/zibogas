#ifndef _DB_H
#define _DB_H

#include "data_type.h"

/******************************************************/
//定义表
#define MAX_CLERK_NUM 10    //目前最大员工数量是10个
//表2:黑名单信息
typedef  struct {
	char black_card_no[5];
}BlackCardInfo;

//表3:交易记录
typedef  struct {
	INT8U record_info[34];
}CardInfoOne;

//表4:操作员名单
typedef struct 
{
	char id[2+1];
	char pw[8+1];
	unsigned int right;
} CLERK_STRUCT;

enum 
{
	ID_OPERATOR=1,
	ID_MANAGER=2,
	ID_SUPERVISOR=3
};
//定义票价按键对应关系
typedef struct	
{
	unsigned int	price;//票价
	unsigned char	key;//按键值
}PriceKey;

//记录票价车号()
typedef  struct {

	PriceKey			lstrPriceKey[10];
	unsigned    long	PosNo;	
}DevParam;

 //1.表1:司机号和上下班状态标志

typedef  struct {

	INT8U		cur_driver_number[4];
	INT8U		dirver_is_working;
	
}DriverParam;



//3.表3:交易记录2
typedef  struct {
	//unsigned char record_info[34];
	unsigned char record_info[83];
}CardInfoTwo;


// //表5:参数信息表
// typedef  struct {
// 	INT8U line_no[2];//线路号
// 	INT8U bus_no[3]; //车号
// 	INT32U normal_card_fare;  //普卡票价
// 	INT32U elementary_student_card_fare;//小学生卡票价
// 	INT32U high_student_card_fale;//中学生卡票价
// 	INT32U old_card_fare;	//老年卡票价
// 	INT32U family_card_fare;//家属卡
// 	INT32U employee_card_fare;//员工卡票价
// 	INT32U month_card_fare;//月票卡票价
// 	INT32U param_flag;	//参数标志位
// 	INT32U element_student_cnt;//小学生卡优惠次数
// 	INT32U high_student_cnt;//中学生卡优惠次数
// 	INT32U month_card_cnt;//月票卡优惠次数
// 	INT32U have_been_set;//是否设置卡设置过
// 	INT32U normal_interval;//普卡刷卡间隔
// 	INT32U favord_interval;//优惠卡刷卡间隔
//
// }ParamCardInfo;

//表6:记录指针信息表
typedef struct {
	INT32U	last_normal_gather_index;//补采起始索引地址
	INT32U  last_normal_record_count;//补采记录总数
	INT32U  cur_normal_gather_index;//当前记录起始索引地址
	INT32U	cur_normal_record_count;//当前记录总数
	INT8U	last_index_loca;		//补采表索引(主表为0，备表为1)
}RecordIndex;

//宏定义表索引变量
//#define  FIRSTTAB		0
//#define  SECONDETAB		1
//#define	 THIRDTAB		2
//#define	 FouthTAB		3
//#define  FifthTAB		4
//#define	 SixthTAB	    5

//#define  DRIVERPARAM	FIRSTTAB	//驾驶员状态表
//#define	 RECORDTABONE	SECONDETAB	//记录表一
//#define	 RECORDTABTWO	THIRDTAB  //记录表二
//#define  BLACKTAB		FouthTAB	//黑名单表
//#define  PARAMINFO		FifthTAB	//参数表
//#define	 RECORDINDEX	SixthTAB    //记录指针索引表


#define DB_PARAM	0
#define DB_BLACK	1
#define DB_REC		2
#define DB_REC_BAK	3
#define DB_REC_CONSUME	DB_REC_BAK

#define DB_REC_HISTORY	4
#define DB_GRAY_RECORD 5


//宏定义记录采集方式
//#define	 FROMRECORDONE		0			//采集记录表一中数据
//#define	 FROMRECORDTWO		1			//采集记录表二中数据
//#define	 FROMRECORDALL		2			//同时采集2个表中的数据

//function 
//void   init_db();
//INT8U   chk_sys_db_format(void);
//short SaveDevParam(DriverParam	*strDevParam);
//short QueryDevParam();


// int   GetTypeBParamfromFlash(ParamCardInfo	*strParamInfo);//查询参数表信息
// int   SaveTypeBParamtoFlash(ParamCardInfo *pSaveParam);//保存参数表信息
short QueryCardTradeInfo(unsigned char * card_no, unsigned char *card_record);//查询卡片交易信息
INT8U SaveCardRecord(RECORD * CardRecord);//保存卡片交易记录
INT8U StoreRectoSD(RECORD* prec);
//char  SaveRecordIndex();//保存记录指针
//char  QueryRecordIndex(long *lBaseIndex, long* lRecordCount);//查询记录指针信息
//int   QueryLastRecordIndex(long *lBaseIndex, long* lRecordCount, char *RecordLocal);//查询最后一次采集记录指针信息


/******************************************************************************
 函数名称：StoreRecord
 功能描述：交易记录存储函数
 参数名称：	输入/输出？	类型		描述
cardinfo	输入			CARD_INFO*	交易完毕后的卡片的所有信息
				
 返  回  值：无
 
 作      者	：许岩/刘及华
 日      期：2012-12-12
 修改历史：
		日期		修改人		修改描述
		2005.2.18		myron	                     in function 
******************************************************************************/
//INT8U StoreRecord(CARD_INFO *cardinfo);
/******************************************************************************
 函数名称：StoreConsumeRecord
 功能描述：交易记录存储函数
 参数名称：	输入/输出？	类型		描述
 cardinfo	输入			CARD_INFO*	交易完毕后的卡片的所有信息
				
 返  回  值：无
 
 作      者	：许岩/刘及华
 日      期：2012-12-12
 修改历史：
		日期		修改人		修改描述
		2005.2.18		myron	                     in function 
******************************************************************************/
INT8U StoreConsumeRecord(CARD_INFO *cardinfo);
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
INT8U StoreGrayRecord(void);
/******************************************************************************
 函数名称：WriteParam
 功能描述：保存参数
 参数描述：
 参数名称：	输入/输出？	类型		描述
 
 返  回  值：无
				   
 作      者	：许岩
 日      期：2006-08-02
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
******************************************************************************/
//void WriteParam(void);



/******************************************************************************
 函数名称：ReadParam
 功能描述：读取参数
 参数描述：
 参数名称： 输入/输出？ 类型        描述

 返  回  值：无

 作      者 ：许岩
 日      期：2006-08-02
 修改历史：
		日期        修改人      修改描述
		------      ---------   -------------
******************************************************************************/
//void ReadParam(void);



/******************************************************************************
 函数名称：InitImportantParameter
 功能描述：初始化关键参数
 参数描述：
 参数名称：	输入/输出？	类型		描述
				
 返  回  值：无

 作      者	：许岩
 日      期：2004-09-02
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
******************************************************************************/
void InitImportantParameter(void);



/******************************************************************************
 函数名称：GetBlackName
 功能描述：得到指定条号的黑名单，只读主区
 参数名称：	输入/输出？	类型		描述
 black			输出		INT8U*		返回的黑名单卡号
 black_num	输入	INT16U		要读取黑名单的条号0-7999
 
 返  回  值：无
 
 作      者	：xuhongliang
 日      期：2004-12-08
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
******************************************************************************/
void GetBlackName(INT8U *black, INT16U black_num);



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
INT8U BlackName_Find(char *card_number);


/******************************************************************************
 函数名称：ReadGrayRecord
 功能描述：读取灰记录
 参数描述：
 参数名称：	输入/输出？	类型		描述
 
 返  回  值：无
				   
 作      者	：许岩
 日      期：2006-08-02
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
******************************************************************************/
INT8U ReadGrayRecord(void);

/******************************************************************************
 函数名称：WriteGrayRecord
 功能描述：保存灰记录
 参数描述：
 参数名称：	输入/输出？	类型		描述
 
 返  回  值：无
				   
 作      者	：许岩
 日      期：2006-08-02
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
******************************************************************************/
INT8U WriteGrayRecord(void);

INT8U SaveCardConsumeRecord(unsigned char * CardRecord);

#endif  
