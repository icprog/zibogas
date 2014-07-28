/******************************************************************************
Copyright 2005 大连易达通电子有限公司
All rights riserved.

文件名  ：data_type.h
模块名称：定义了所有的数据类型

当前版本：0.0.1
修改人	：许岩
完成日期：2005.04.06
升级说明：create
******************************************************************************/
#ifndef __DATA_TYPE_H__
#define __DATA_TYPE_H__

//#include "OS_CPU.H"
//#include "main.h"
#ifndef NULL
#define NULL    ((void *)0)
#endif

#ifndef FALSE
#define FALSE   (0)
#endif

#ifndef TRUE
#define TRUE    (1)
#endif

typedef unsigned char  BYTE;
typedef unsigned short WORD;
typedef unsigned long  DWORD;
typedef unsigned int   BOOL;


/**********************************************************************************************************
*                    定义与编译器无关的数据类型
**********************************************************************************************************/

typedef unsigned char  BOOLEAN;                 /* 布尔变量                                 */
typedef unsigned char  INT8U;                   /* 无符号8位整型变量                        */
typedef signed   char  INT8S;                   /* 有符号8位整型变量                        */
typedef unsigned short INT16U;                  /* 无符号16位整型变量                       */
typedef signed   short INT16S;                  /* 有符号16位整型变量                       */
typedef unsigned int   INT32U;                  /* 无符号32位整型变量                       */
typedef signed   int   INT32S;                  /* 有符号32位整型变量                       */
typedef float          FP32;                    /* 单精度浮点数（32位长度）                 */
typedef double         FP64;                    /* 双精度浮点数（64位长度）                 */

typedef INT32U         OS_STK;                  /* 堆栈是32位宽度                           */


typedef unsigned char  	uint8;                   /* 无符号8位整型变量                        */
typedef signed   char  	int8;                    /* 有符号8位整型变量                        */
typedef unsigned short 	uint16;                  /* 无符号16位整型变量                       */
typedef signed   short 	int16;                   /* 有符号16位整型变量                       */
typedef unsigned int   	uint32;                  /* 无符号32位整型变量                       */
typedef signed   int   	int32;                   /* 有符号32位整型变量                       */
typedef float          	fp32;                    /* 单精度浮点数（32位长度）                 */
typedef double         	fp64;                    /* 双精度浮点数（64位长度）                 */

 
//车载机时间结构
//__packed typedef struct _TIME_
typedef struct _TIME_
{
	INT8U century;
	INT8U year;
	INT8U month;
	INT8U day;
	INT8U hour;
	INT8U minute;
	INT8U second;
}BUS_TIME;


#endif
