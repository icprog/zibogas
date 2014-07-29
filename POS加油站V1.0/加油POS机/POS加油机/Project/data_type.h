/******************************************************************************
Copyright 2005 �����״�ͨ�������޹�˾
All rights riserved.

�ļ���  ��data_type.h
ģ�����ƣ����������е���������

��ǰ�汾��0.0.1
�޸���	������
������ڣ�2005.04.06
����˵����create
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
*                    ������������޹ص���������
**********************************************************************************************************/

typedef unsigned char  BOOLEAN;                 /* ��������                                 */
typedef unsigned char  INT8U;                   /* �޷���8λ���ͱ���                        */
typedef signed   char  INT8S;                   /* �з���8λ���ͱ���                        */
typedef unsigned short INT16U;                  /* �޷���16λ���ͱ���                       */
typedef signed   short INT16S;                  /* �з���16λ���ͱ���                       */
typedef unsigned int   INT32U;                  /* �޷���32λ���ͱ���                       */
typedef signed   int   INT32S;                  /* �з���32λ���ͱ���                       */
typedef float          FP32;                    /* �����ȸ�������32λ���ȣ�                 */
typedef double         FP64;                    /* ˫���ȸ�������64λ���ȣ�                 */

typedef INT32U         OS_STK;                  /* ��ջ��32λ���                           */


typedef unsigned char  	uint8;                   /* �޷���8λ���ͱ���                        */
typedef signed   char  	int8;                    /* �з���8λ���ͱ���                        */
typedef unsigned short 	uint16;                  /* �޷���16λ���ͱ���                       */
typedef signed   short 	int16;                   /* �з���16λ���ͱ���                       */
typedef unsigned int   	uint32;                  /* �޷���32λ���ͱ���                       */
typedef signed   int   	int32;                   /* �з���32λ���ͱ���                       */
typedef float          	fp32;                    /* �����ȸ�������32λ���ȣ�                 */
typedef double         	fp64;                    /* ˫���ȸ�������64λ���ȣ�                 */

 
//���ػ�ʱ��ṹ
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
