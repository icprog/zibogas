/*****************************************************************
Copyright 2003 �����ִ��߼�����չ���޹�˾
All rights riserved.

�ļ���	��bus.h
ģ�����ƣ������˹��õ�һЩdefine

��ǰ�汾��0.0.1
�޸���	��������������
������ڣ�2004.09.02
 *****************************************************************/
#ifndef __CPUCARD_H__
#define __CPUCARD_H__
#include "data_type.h"


#define BUS_TYPE_NORMAL     8		//��ͨ��
#define BUS_TYPE_LUXURY     6		//������

//����GPRSͨѶ״̬��
#define AT_OK				0//�ɹ�
#define AT_ERR				1//ʧ��
#define AT_TIMEOUT			2//��ʱ
#define AT_QUIT				3//��Ϊ�˳�


#ifdef DEBUG
	#define FIVE_MINUTES	1			//ˢ��ʱ����
	#define FIVE_SECONDS	1			//ˢ��ʱ����
#else
	#define FIVE_MINUTES	300			//ˢ��ʱ����
//	#define FIVE_SECONDS	5			//ˢ��ʱ����
	#define FIVE_SECONDS	2			//ˢ��ʱ����
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


#define PREFERENTIAL_PRICE_CONSUMPTION 6				//��������Ϊѧ��������Ʊ���Ż�Ʊ�۽���
#define TOTALRECORDNUM      			10200
// #define MAX_REC_NUM                     10200
#define MAX_REC_NUM						2000
#define BLACKNAMESTARTPAGE      	    300		//��������ʼҳ
#define BLACKPAGENUM					300     //������ռ�õ�ҳ��
#define RECORDSTART         			600
#define RECORDPAGENUM         			3200
#define RECORDPERPAGE       			256
#define BYTESPERRECORD      			32
#define BYTESPERRECORDWITHCRC           34
#define COUNTER_MAX      				800
#define	FRAM_REC_NUM					16				//�����п��Ի���16����¼
#define	FRAM_REC_POS					1024			//�����м�¼��ʼ��λ��

#define REC_ONE_POS						0x00		//��һ����¼��λ��
#define REC_TWO_POS						0x00		//�ڶ�����¼��λ��
#define REC_THREE_POS					0x00		//��������¼��λ��

#define CPU_CARD 						0x08
//#define M1_CARD 						0x04
#define M1_CARD 						0x02
#define DISP_DELAY_COUNTER_END		0x00
#define DISP_DELAY					600						//���׳ɹ������ͣ������Ļ�ϵ�ʱ��
#define SINGLE_KEY_QUERY_DELAY		500					//������ѯʱ


#define LINE_NUM_HAVE_SET	0x01
#define BUS_NUM_HAVE_SET	0x02


//01	��ͨ��
//02	����(���)��
//03	���ٿ�
//05	���
//06	Ա����
//07	ѧ����
//08	ְ����

//�Ͳ�������Ƭ���Ͷ���
//01����ͨ����
//02��ѧ������
//03�����꿨��
//04����ѿ���D������
//05�������
//06��Ա����

//��ͨ��
//ѧ����
//���꿨
//��ѿ���D����
//���
//Ա����

//������
//���ѿ�
#define CARDT_NORMAL        0x01    //��ͨ��
#define CARDT_STUDENT		0x02    //���꿨(���)
   
#define CARDT_OLDMAN   		0x03    //���곤�ٿ�
// #define CARDT_EMPLOYEE_A    0x04    //������
#define CARDT_FREE			0x04    //ְ����
#define CARDT_MEMORY        0x05    //���
#define CARDT_DRIVER        0x06    //Ա����
 //#define CARDT_STUDENT     0x07    //ѧ����
 //#define CARDT_FREE     0x08    //ְ����
 
 //����
#define CARDT_SET           0x10    //��·Ʊ�۳������ÿ�
#define CARDT_GATHER        0x11    //���ݲɼ���
#define CARDT_SIGNED        0x12    //ǩ�㿨
#define CARDT_CHECKED       0x13    //��⿨
#define CARDT_INSPECTOR       0x18    //���鿨


//֤������
#define  IDENTIFY_TYPE 		0x01//1:���֤
#define  OLD_TYPE			0x02//2:����֤
#define  RETIRE_TYPE		0x03//3:����֤
#define  OFFICER_TYPE		0x04//4:����֤
#define	 RESIDENCE_TYPE		0x05//5:����֤
#define	 STUDENT_TYPE		0x06//6:ѧ��֤


// //���ѿ�
// #define CARDT_NORMAL		0x00	//��ͨ��
// #define CARDT_ADULT_MTH		0x02	//������Ʊ��
// #define CARDT_STUDENT_MTH		0x03	//ѧ����Ʊ��
// #define CARDT_EMPLOYEE_CHD	0x04	//Ա����Ů��
// #define CARDT_EMPLOYEE		0x05	//Ա����
// #define CARDT_MEMORY		0x06	//һ���Կ�
// #define CARDT_FRIEND			0x07	//���鿨
// //����
// #define CARDT_DRIVER		0xA0	//˾����
// #define CARDT_GATHER		0xA2	//���ݲɼ���
// #define CARDT_SET_NUM		0xA3	//��·�������ÿ�
// #define CARDT_SET_FARE		0xA5	//Ʊ�����ÿ�
// #define CARDT_SIGNED_IN		0xA9	//�복���ȿ�
// #define CARDT_SIGNED_OUT	0xAA	//�������ȿ�
// #define CARDT_MONTH_EN		0xAB	//��Ʊ��Ч��
// #define CARDT_MONTH_DIS		0xAC	//��Ʊ��Ч��

//���ݺ�����
#define POS_T_NORMAL	0x34			//�����ɼ�����
#define POS_T_ABNORMAL	0x36			//�쳣�ɼ�����


//����ʱ������
#define RTC_3511    0x01
#define RTC_35180   0x02

//����ģʽ
#define NORMAL 0x00				//����
#define SHORTCUT 0x01			//���

//////////////////////////////////////////////
//�������꿨Ա�����Ƿ���������
//��־
//added by myron for dandong flow 2005.3.30
//////////////////////////////////////////////
#define OLDMAN_ENABLE_CONSUME	0x02	 //�������꿨����
#define DRIVER_ENABLE_CONSUME	0x01	 //����Ա��������

//////////////////////////////////////////////
//���彻������
//added by myron for dandong flow 2005.2.18
//////////////////////////////////////////////
// #define TT_NORMAL_CONSUME_MONEY			0x01		//��ͨ������
// #define TT_ADULT_MTH_CONSUME_MONEY 		0x03		//������Ʊ������Ǯ��
// #define TT_STUDENT_MTH_CONSUME_MONEY 	0x05		//������Ʊ������Ǯ��
// #define TT_SIGN_IN						0x41		//�복���ȿ���������
// #define TT_SIGN_OUT						0x42		//�������ȿ���������
// #define TT_DRIVER_WORKING				0x43		//˾���ϰ�����
// #define TT_DRIVER_REST					0x44		//˾���°���������
// #define TT_GATHER						0x45		//�ɼ�����������
// #define TT_BLACKNAME_CONSUME_TYPE		0x13		//��������������

#define TT_NORMAL_CONSUME					0x06		//��Ƭ����

#define TT_NORMAL_CONSUME_MONEY				0x02		//��Ƭ����
#define TT_CREDIT_MONEY						0x01        //��Ƭ��ֵ
#define TT_CARD_SALE						0x04		//��Ƭ�ۿ�
//#define TT_SETTING				  	 0x10       	//��·Ʊ�����ÿ�
#define TT_FREE								0x07		//��ѿ�����
#define TT_BLACKNAME_CONSUME_TYPE			0x11		//��������������
#define TT_DRIVER_WORKING					0x12		//˾���ϰ�����
#define TT_DRIVER_REST						0x13		//˾���°�����
#define TT_CHECK							0x14		//��⿨����
#define TT_SIGN								0x15		//ǩ�㿨����
#define TT_INSPECTOR						0x17		//���鿨����
#define TT_MANAGE							0x16		//������������

#define TT_GRAY_SALE						0x94		//�ۿ��Ҽ�¼
#define TT_GRAY_CREDIT						0x91		//��ֵ�Ҽ�¼
#define TT_GRAY_FLUSH						0x92		//���˻Ҽ�¼

#define MAX_INPUT_LENGTH 10


// #define 	COMMON_CONSUME_TYPE			0x06		//��ͨ��������
// #define 	FAVOURABLE_CONSUME_TYPE		0x08		//�Ż���������
// #define 	ZERO_CONSUME_TYPE			0x09		//����������
// #define 	BLACKNAME_CONSUME_TYPE		0X11		//��������������
// #define 	DRIVER_WORKING_TYPE			0x12		//˾���ϰ�����
// #define 	DRIVER_REST_TYPE			0x13		//˾���°���������
// #define 	CHECKED_CONSUME_TYPE		0x14		//�����������
// #define 	SIGNED_CONSUME_TYPE			0x15		//ǩ����������
// #define 	INSPECTOR_CONSUME_TYPE		0x17		//������������
// #define 	MANAGER_CONSUME_TYPE		0x16		//������������

//�洢��FLASH�еĹؼ����ݵ�ƫ����
#define	TYPEA_PARAM_START_POS			0
#define	TYPEB_PARAM_START_POS			100
#define	TYPEC_PARAM_START_POS			200
#define	TYPEA_PARAM_SIZE				100
#define	TYPEB_PARAM_SIZE				100
#define	TYPEC_PARAM_SIZE				100
//A�����--��ҪƵ���޸ĵģ�����ǰ100ҳ��0--99����
#define RECORD_NUMBER                   	TYPEA_PARAM_START_POS     	   //δ�ϴ��ļ�¼����(ԭp0+4)              2��
//B�����--�޸ıȽ�Ƶ����ÿ��������������ҳ���ܹ�ռ��100ҳ��100--199����
#define RECORD_BASE_PTR                 	(TYPEB_PARAM_START_POS)      //0��9����     δ�ϴ���¼ָ����ʼָ��(ԭp0+2)     2�ֽ�
#define LAST_RECORD_BASE_PTR       		(RECORD_BASE_PTR + 2)        //�������ɼ�¼ָ����ʼָ��                2�ֽ�
#define LAST_RECORD_NUMBER          	  (LAST_RECORD_BASE_PTR +2)  //�������ɵļ�¼����                         2�ֽ�
#define CUR_DRIVER_NUMBER               	(LAST_RECORD_NUMBER +2)    //����˾����(ԭp0+620)                    2�ֽ�
//#define driver_is_working                	(CUR_DRIVER_NUMBER +2)       //���°��־(ԭp0+200)                    1�ֽ�
#define TIME_MODIFY_TIMES                  (driver_is_working +2)       //�޸�ʱ�����(ԭp0+687)                 1�ֽ�
#define BLACK_NAME_NUMBER				  (TIME_MODIFY_TIMES +2)			//����������			2�ֽ�

#define TYPEA_PARA_POSITION				(BLACK_NAME_NUMBER + 2)			//A������ŵ�λ�ã���Χ0-99
//ԭC�����������ʹ��C��������鵽B��
#define LINE_NUMBER                           (TYPEA_PARA_POSITION + 2)                                         //��·��(ԭp0+48)                           2�ֽ�
#define BUS_NUMBER                          (LINE_NUMBER + 2)                  //����(ѹ��BCD��,ԭp0+450)            4�ֽ�
#define NORMAL_CARD_FARE				(BUS_NUMBER + 2)				  //��ͨ��Ʊ��
#define STUDENT_CARD_FARE			   (NORMAL_CARD_FARE + 2)		//A��ѧ����Ʊ��
#define OLDMAN_CARD_FARE				(STUDENT_CARD_FARE + 2)  //���꿨Ʊ��
#define EMPLOYEE_CARD_FARE				 (OLDMAN_CARD_FARE + 2)	//ְ����Ʊ��
#define DRIVER_CARD_FARE				 (EMPLOYEE_CARD_FARE + 2)		 //Ա����Ʊ��
//modified by myron for dandong flow 2005.2.21
//#define FAVORC_CARD_FARE   			 (DRIVER_CARD_FARE + 2)			 //C���Żݿ�Ʊ��
//#define FAVORD_CARD_FARE   			 (FAVORC_CARD_FARE + 2)			 //D���Żݿ�Ʊ��
//#define HAVE_BEEN_SET                      (FAVORD_CARD_FARE + 2)      //�Ƿ�ˢ���ÿ���־(ԭp0+638)           1�ֽ�

#define HAVE_BEEN_SET                      (DRIVER_CARD_FARE + 2)      //�Ƿ�ˢ���ÿ���־(ԭp0+638)           1�ֽ�
#define NVRAM_CYCLED			(HAVE_BEEN_SET + 2)		    //FLASH�д�ż�¼�Ƿ񷭹�ҳ == TRUE ���� else û����
//////////////////////////////////////////////
//added by myron 2005.2.17 for dandong device
//////////////////////////////////////////////
#define CONSUME_CARD_KINDS	(NVRAM_CYCLED+2)//���ѿ����� 1�ֽ�
#define AM_STARTTIME_OLDMAN		(CONSUME_CARD_KINDS+2)//���꿨�����ֹ�˳���ʼʱ�� 2�ֽ�
#define AM_STOPTIME_OLDMAN		(AM_STARTTIME_OLDMAN+2)//���꿨�����ֹ�˳�ֹͣʱ�� 2�ֽ�
#define PM_STARTTIME_OLDMAN		(AM_STOPTIME_OLDMAN+2)//���꿨�����ֹ�˳���ʼʱ�� 2�ֽ�
#define PM_STOPTIME_OLDMAN		(PM_STARTTIME_OLDMAN+2)//���꿨�����ֹ�˳�ֹͣʱ�� 2�ֽ�
//////////////////////////////////////////////
//������־λ:
//bit0--Ա�����Ƿ�ۿ��־λ0:���ۿ�
//bit1--���꿨�Ƿ�ۿ��־λ0:���ۿ�
//bit2--������ʾ��־λ0:��Ͷ��1:����Ʊ
//����λ����
//added by myron 2005.2.17
//////////////////////////////////////////////
#define PARAM_FLAG		(PM_STOPTIME_OLDMAN+2)//  1�ֽ�
//////////////////////////////////////////////
//����ˢ�����ʱ��
//////////////////////////////////////////////
#define NORMAL_INTERVAL		(PARAM_FLAG+2)//  1�ֽ�
#define FAVORED_INTERVAL		(NORMAL_INTERVAL+2)//  1�ֽ�


#define		NO_KEY	(INT8U)0xFF


#define ChkStat(exp)      {	INT8U status = (exp); \
											if (status != ok)	 \
												return notok;	}

#define ChkStatPass(exp)      {	INT8U status = (exp); \
													if (status != ok)	 \
														return status;	}

//flashд�뺯��ר�ã�����������ز���OK���򷵻�д��ʧ��
#define ChkFLWrStat(exp)      {	INT8U status = (exp); \
											if (status != ok)	 \
												return FL_WR_ERR;	}

//��� WriteVariableToNVRAM() �� ReadVariableFromNVRAM() �ķ��ز���
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
															����struct���岿��
******************************************************************************/


//���꿨��ֹ�˳�ʱ��ṹadded by myron 2005.2.17
typedef struct _FORBIDDENTIME
{
	INT8U am_start[2];
	INT8U am_stop[2];
	INT8U pm_start[2];
	INT8U pm_stop[2];
}FORBIDDEN_TIME;

////////////////////////////////////////////
//���ӹ���Ա������Ͻṹ����
//added by myron for dandong flow 2005.2.21
/////////////////////////////////////////////
typedef union _MANAGER_NUMBER_
{
	INT8U setter_number[3];     //����Ա���
	INT8U gather_number[3];     //�ɼ�Ա���
	INT8U checker_number[3];    //���Ա���
	INT8U signer_number[3];     //ǩ��Ա���
}_MANAGER_NUMBER;


typedef struct _ID_TYPE_
{
	INT8U id_type;              //֤������
	INT8U id_name[8];           //֤������
}_ID_TYPE;

//���㵥
typedef struct _BANLANCE_IN_HAND_
{
	INT8U	acnt_id[2];        //�̻�ID
	INT8U	equ_id[3];         //�豸ID
	INT8U	oper_id[3];		   //����������Ա

	BUS_TIME work_datetime;	   //�ϰ�ʱ��
	BUS_TIME down_datetime;	   //�°�ʱ��

	INT16U  consume_cnt;       // ���ʹ���
	INT32U	consume_amt;       //�������
}_BANLANCE_IN_HAND;

//�豸״̬������Ҫ�ı�����������ӱ���������SysInit����г�ʼ��
typedef struct _DEV_STAT
{
	INT8U err_occured;					// == TRUE���� ����FALSE�豸����
	INT16U err_code;					//�������

	INT8U allow_upload_data;			//�ɼ���־
	INT8U databox_cmd_rcvd;				//�Ƿ���յ����ݺе�����
	INT8U purchase_again_req;			//����ˢ��������־����TRUE��������ˢ�� ==FALSE������
	INT8U voice_purchase_again_freq;
	INT8U mode;                             
	//PSAM�����
	INT8U cur_psam_channel;				//��ǰʹ�õ�PSAM��ͨ�� = PSAM_PRI �� �� PSAM_SLV
	INT8U key_version;					//PSAM����Կ�汾��
	INT8U pos_number[6];				//POS�ţ������ն˺ţ�����PSAM�����
	INT8U oper_cardno[5];				//����Ա����
	INT16U record_base_ptr;				//δ�ϴ���¼ָ����ʼָ��(ԭp0+2) 

	INT32U record_number;				//δ�ϴ��ļ�¼����(ԭp0+4)
	INT8U POS_type;						//������
	INT8U card_password[6];				//����Ա����
	INT8U Gprs_On_Flag;                 //gprsͨ���Ƿ���ñ�־
	INT8U black_flag;
	INT16U current_record_index;

	//��������Ҫ�洢��flash�е�����
	INT8U start;	
	INT8U ServerIp[4];                  //��������ַ
	INT8U ServerPort[2];                //�������˿ں�
	INT8U dev_trade_no[2];              //�豸�������

	INT16U  send_total_num;				//��ǰ���ͼ�¼���
	INT8U   cur_driver_number[4];		//����˾���ţ��°���0------>�ĳɵ������Ա��
	INT8U   driver_is_working;
	INT16U  black_name_number;          //����������
	INT8U	black_version[7];           //�汾��
	INT8U	bus_number[5];              //���ƺ�
	INT8S   user_passwd[7];			    //�������ò˵�������
	INT8U   fill_mode;				    //��Ʒ��ʾ0������ 1������ 2������
	INT8U   samcompcode;			        //SAM�����̴���
	_BANLANCE_IN_HAND  binhand;         //���㵥
	INT32U fare;			            //�ͼ�

/*--------------------20070131�����ӵ�--------------------*/
	INT8U   acnt_id[2];                   //�̻��ű���
	INT8U   spot_id[2];                   //���ͻ�����
	INT8U   equ_id[3];                    //�豸����
	INT8U   isam_no[6];                   //ISAM������

	INT8U   end;
}DEV_STAT;


//CARD_INFO���õ���struct
typedef struct _CARD_NUM
{
	INT8U city_code[2];	            //���д���
	INT8U calling_code[2];          //��ҵ����
	INT8U serial_number[4];		    //���к�
}CARD_NUM;

//��Ƭ��Ϣ�����׼�¼�ɴ˱�������
typedef struct _CARD_INFO
{
	union _CARD_NUMBER_
	{
		INT8U card_number[8];//����
		CARD_NUM detail_card_number;
	}_CARD_NUMBER;

	BUS_TIME card_in_time;			//����ʱ��
	INT8U   card_begin_dt[4];       //��Ƭ��������yyyymmdd
	INT8U   card_useful_dt[4];      //��Ƭ��Ч��yyyymmdd
	INT8U   card_add_dt[4];         //��Ƭ�ӿ�ʱ��yymmddhh
	
	INT8U  card_type;				//������
	INT8U  fuel_type;				//������0����Ȼ����1������ 2������
	INT32U fare;					//���ͣ�������λ
	INT32U cur_fare;				//��ǰӦ�۽�CPU������ʹ��

	INT8U init_purchase_serial_num[2];		//��ʼ�Ŀ������ѽ������
	INT8U purchase_serial_num[2];			//��ʼ�Ŀ��ڽ������+1
//	INT8U init_credit_serial_num[2];		//��ʼ�Ŀ��ڳ�ֵ�������
//	INT8U credit_serial_num[2];			    //���ڽ������+1

	INT32U init_balance;					//ԭ���
	INT32U balance;							//���׺����
	INT8U card_purchase_type;				//��������
	INT8U tac[4];							//TAC��
	INT8U mac2[4];							//CPU�����ص�MAC2

	INT8U bus_number[5];                    //���ƺ���
	INT8U csn[4];							//Ӳ������
	INT8U authentication_code[4];			//����֤��

	INT8U card_random_num[4];				//CPU�������
	INT8U cpu_card_key_version;				//CPU����Կ�汾��
	INT8U arithmetic_sign;					//�㷨��ʶ

	INT32U psam_purchase_serial_num;	    //PSAM���������
	INT8U mac1[4];							//PSAM���������MAC1

	//����˾���������°������Ա����
//	INT8U quit_work;						//��Ƭ���׹����У�ʵ�����°����
//	INT8U begin_work;						 //��Ƭ���׹����У�ʵ�����ϰ����
	INT8U drv_card_type;					//˾��������

	/////////////////////////////////////////////
	//�Żݿ���Ӧ��Ϣ
	/////////////////////////////////////////////
	INT16U init_time_balance;               //ԭ�������
	INT16U time_balance;                    //���׺�������

	INT8U station_number[2];                //վ����
	INT8U checked_flag;                     //����־
	_MANAGER_NUMBER manager_number;         //����Ա��� 

	INT8U driver_number[4];			        //˾����

/*--------------20070131�����ӵ��û���������Ϣ--------------------*/
	INT8U acnt_id[3];                       //�������ı���
	INT8U spot_id[3];                       //�ۿ������
	INT8U equ_id[3];                        //�豸����
	INT8U key_type;                         //��Կ����
	INT8U pay_type;                         //���ʽ
	INT8U deposit_amt[2];                   //Ѻ��
	INT8U isam_no[6];                       //ISAM������
//	INT8U oper_id[6];                       //����Ա��
	INT8U card_status;                      //��Ƭ״̬
	INT8U name[8];                          //�ֿ�������
	INT8U sex;                              //�ֿ����Ա�
	INT8U id_type;                          //�ֿ���֤������
	INT8U id_no[18];                        //�ֿ���֤������
	INT8U dev_trade_no[2];                  //�豸�������
	INT8U credit_rec_pos;                   //Ȧ���¼�����ַ
	INT8U blockcreditrec[16];	            //���������ǰѴ�bufд�ؿ���

	INT8U card_record_point;				//���Ѽ�¼��ָ��
	
}CARD_INFO;

//����״̬
typedef struct _ERROR_CARD_INFO
{
	INT8U error_occured;						//==TRUE���ۿ������==FALSE���ۿ���ȷ
	INT8U sale_error_occured;					//==TRUE���ۿ������==FALSE���ۿ���ȷ

	INT32U init_balance;						//ԭ���
	INT32U init_cpu_card_balance;
	/////////////////////////////////////////
	//added by myron for dandong flow 2005.2.22
	/////////////////////////////////////////
	INT16U init_time_balance;	//ԭ�������	
	//cpu
	INT8U cpu_card_number[8];			        //CPU����
	INT8U purchase_serial_num[2];		        //�������(�ӹ�1)
	//m1
	INT8U block24_buf_to_restore[16];	        //���������ǰѴ�bufд�ؿ���
	INT8U m1_card_number[4];				    //M1������
	INT8U csn[4];								//����Ӳ������
	INT8U trade_type;                           //��������add by yfy 20070206
}ERROR_CARD_INFO;

//���׼�¼

typedef  struct  _RECORD
{
	INT8U acnt_id[2];                //�������ı���
	INT8U spot_id[2];                //�ۿ������
	INT8U equ_id[3];                 //�豸����
	INT8U driver_id[4];              //����Ա��
	INT8U isam_no[6];                //ISAM������
   	INT8U card_type;                 //��Ƭ����  1�ֽ�
	INT8U fuel_type;                 //ȼ��Ʒ��0,��Ȼ����1�����ͣ�2������
	INT8U card_purchase_type;        //��������	1�ֽ�
	INT8U pos_purchase_serial_num[3];//�ն˽������3�ֽ�
	INT8U card_serial_number[8];     //������ˮ��	8�ֽ�
	INT8U bus_number[5];             //�����ƺ�	5�ֽ�
	INT8U balance[3];                //���׺����	3�ֽ�
	INT8U fare[3];                   //���ѽ��3�ֽ�
	BUS_TIME card_in_time;           //����ʱ��	7�ֽ�
	INT8U purchase_serial_num[2];    //��ʼ�Ŀ��ڽ������+1	2�ֽ�
	INT8U tac[4];                    //TAC��	4�ֽ�
	INT16U crc;
}   RECORD;


typedef struct _PREVCARDINFO
{
	INT8U csn[4];	
	BUS_TIME card_in_time;         	//����ʱ��	
}PREVCARDINFO;



#define resetpos() (*(void(*)())0)()


/******************************************************************************
 �������ƣ�SaveCardInTime
 �����������뿨ʱ����ʱ��
 ����������
 �������ƣ�	����/�����	����		����
 card_info	����		CARD_INFO *		׼��Ҫ����ʱ��ı�����ָ��
 
 ��  ��  ֵ��ok(0)-�ɹ�
				 notok(0xFF)-ʧ��
				   
 ��      ��	������
 ��      �ڣ�2004-09-02
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
******************************************************************************/
INT8U SaveCardInTime(CARD_INFO *card_info);

/******************************************************************************
 �������ƣ�CPUGetCardTypeAndADF2
 ����������ȡ��CPU���Ŀ��źͿ�����
 ����������
 �������ƣ�	����/�����	����		����
 card_type	���		INT8U *		���ص�CPU������
 adf2_buf	���	INT8U *	���ص�1002Ŀ¼������
 
 ��  ��  ֵ��ok(0)-�ɹ�
				 notok(0xFF)-ʧ��
				   
 ��      ��	������
 ��      �ڣ�2004-09-02
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
******************************************************************************/
//INT8U CPUGetCardTypeAndADF2(INT8U * card_type, INT8U * adf2_buf);

//����cpu�� ����Ӧ��Ϣ���� ADF1(1001Ŀ¼��)  2005.02.22  hwr
/******************************************************************************
 �������ƣ�CPUCardInfoProcess
 ������������CPU������Ϣ���д���, ��������������, ���ÿ��ж�, ���ñ�־,��������, ��Ч���ж�
 ����������
 �������ƣ�	����/�����	����		����
 card_type	���		INT8U *		���ص�CPU������
 
 ��  ��  ֵ��ok(0)-�ɹ�
				 notok(0xFF)-ʧ�ܣ���
				 0x3c	������������ɱ�־
				   
 ��      ��	������
 ��      �ڣ�2004-09-02
 �޸���ʷ��
		����		�޸���		�޸�����
	2005.02.22	    hwr  	       ��ԭ��CPU_Card_Consume_Process ��ժ��һ����, �ټ�Щ�޸�
******************************************************************************/
//INT8U CPUCardInfoProcess(INT8U * ADF1_buf);

// *****************************************************************
// ���ܣ�		����
// ��ڲ�����	i=0:����"��"һ��
//				i=1:����"������"����
// ���ڲ�����	Null
// ���ߣ�
// ����������EH0218---COMMON.C���ӳ���
// *****************************************************************
void Beep(int i);

void Beeperr(void);
/******************************************************************************
 �������ƣ�SetBell
 �������������Ʒ�����
 ����������
 �������ƣ�	����/�����	����		����
 bell_switch	����		INT8U	ON-�죬OFF-����
				
 ��  ��  ֵ����
				   
 ��      ��	������
 ��      �ڣ�2004-09-02
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
******************************************************************************/
//void SetBell(INT8U bell_switch);

/******************************************************************************
 �������ƣ�Gather_Card_Process
 �����������ɼ�����������
 ����������
 �������ƣ�	����/�����	����		����
	INT8U const * const  gather_number	�ɼ�Ա���
 
 ��  ��  ֵ��ok(0)-�ɹ�
				 notok(0xFF)-ʧ��
				   
 ��      ��	������
 ��      �ڣ�2004-09-02
 �޸���ʷ��
		����		�޸���		�޸�����
		2005.03.01	HWR	-------------
******************************************************************************/
//INT8U Gather_Card_Process(INT8U const * const  gather_number);



// /******************************************************************************
//  �������ƣ�WriteVariableToNVRAM
//  ������������NVRAM�д洢���ݣ��˺�����Important_Save�����ظ����Ӵ˺�����Ϊ����ֲ��
//  ����������
//  �������ƣ� ����/����� ����        ����
// variable        ����        void *      Ҫд��ı���
// offset          ����        INT16U const    �˱�����ƫ�Ƶ�ַ
// size            ����        INT8U       �˱����ĳ��ȣ�sizeof(variable)
//
//  ��  ��  ֵ��ok(0)-�ɹ�
//                  notok(0xFF)-ʧ��
//                  0xFE-��������
//
//  ��      �� ������
//  ��      �ڣ�2004-09-02
//  �޸���ʷ��
//         ����        �޸���      �޸�����
//         ------      ---------   -------------
// ******************************************************************************/
// INT8U WriteVariableToNVRAM(void *variable, INT16U const offset, INT8U const size);
//
//
//
// /******************************************************************************
//  �������ƣ�ReadVariableFromNVRAM
//  ������������NVRAM�ж������ݣ��˺�����Important_Restore�����ظ����Ӵ˺�����Ϊ����ֲ��
//  ����������
//  �������ƣ� ����/����� ����        ����
// variable        ���        void *      Ҫ��ȡ�ı���
// offset          ����        INT16U const    �˱�����ƫ�Ƶ�ַ
// size            ����        INT8U       �˱����ĳ��ȣ�sizeof(variable)
//
//  ��  ��  ֵ��ok(0)-�ɹ�
//                  notok(0xFF)-ʧ��
//                  0xFE-��������
//
//  ��      �� ������
//  ��      �ڣ�2004-09-02
//  �޸���ʷ��
//         ����        �޸���      �޸�����
//         ------      ---------   -------------
// ******************************************************************************/
// INT8U ReadVariableFromNVRAM(void *variable, INT16U const offset, INT8U const size);
//
//
//
//  /*****************************************************************
//  ����ԭ�ͣ� Important_Save()
//  ���������� �����������д���0ҳp0+n��p0+1024+n��p0+2048+n����ȷ��д����ȷ
//  ���������� ��������        ����/�����     ����            ����
//                  data_to_write   ����        INT8U*  Ҫд�������
//                  offset              ����        INT8U*  Ҫ������������NVRAM�е�ƫ����0xC000+offset
//                  data_length     ����        INT8U   Ҫд������ݵĳ���  ��Ҫ��С��33
//
//  ����ֵ��   ok(0)-д��ɹ���
//                 notok(1)-д��ʧ�ܣ�
//                 0x02-data_length���Ȳ���
//                 0x03-���ݳ����洢����Χ
//
//  ��  �ߣ�   �����ִ�/����
//  ��  �ڣ�   2004-09-16
//  �޸���ʷ��
//         ����        �޸���      �޸�����
//         ------      ---------   -------------
// *****************************************************************/
// INT8U Important_Save(INT8U *data_to_write, INT16U const offset , INT8U const data_length);
//
//
//
// /*****************************************************************
//  ����ԭ�ͣ� Important_Restore()
//  ���������� �ѵ�0ҳp0+n,p0+1024+n��p0+2048+n�����ݶ���data_to_read��ȷ�����ݶ�����ȷ
//  ���������� ��������        ����/�����     ����            ����
//
//                  data_to_read    ���       INT8U*   ��ֲ������ã�������
//                  ram_adr         ����        INT16U  Ҫ��ȡ��������NVRAM�е�ƫ����0xC000+offset
//                  data_length     ����       INT8U    Ҫ�Աȵ����ݵĳ���  ��Ҫ��С��33
//
//  ����ֵ��   ok(0)-д��ɹ���
//                 notok(1)-д��ʧ�ܣ�
//                 0x02-data_length���Ȳ���
//                 0x03-���ݳ����洢����Χ
//
//  ��  �ߣ�   �����ִ�/����
//  ��  �ڣ�   2004-10-01
//  �޸���ʷ��
//         ����        �޸���      �޸�����
//         ------      ---------   -------------
// *****************************************************************/
// INT8U Important_Restore(INT8U *data_to_read, INT16U const offset , INT8U const data_length);



/*****************************************************************
 ����ԭ�ͣ�	Fatal_NVRAM_Err()
 ����������ReadVariableFromNVRAM��3�����ݲ�һ�´�����ʾ�ڲ���������(NVRAM����E??)
 ����������	��������		����/�����		����			����
				 
 ����ֵ��	��
 
 ��  �ߣ�	�����ִ�/����
 ��  �ڣ�	2004-09-17
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
*****************************************************************/
//void Fatal_NVRAM_Err(void);



/*****************************************************************
 ����ԭ�ͣ�	Fatal_FLASH_Err()
 ����������������FLASH����
 ����������	��������		����/�����		����			����
 pri_slv		����		INT8U const		��FLASH���Ǵ�FLASH����PRI �� ��SLV
 
 ����ֵ��	��
 
 ��  �ߣ�	�����ִ�/����
 ��  �ڣ�	2004-09-17
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
*****************************************************************/
//void Fatal_FLASH_Err(INT8U const pri_slv);


/*****************************************************************
 ����ԭ�ͣ�Get_Sector_Permission
 ����������ȡ�õ�N���Ķ�дȨ��
 ����������
 �������ƣ�	����/�����	����		����
 sector		����			INT8U		Ҫȡ��Ȩ�޵���
 key		����			INT8U *		Ҫȡ��Ȩ��������Կ
 				
 ��  ��  ֵ��ok(0)-�ɹ�
				 notok(0xFF)-ʧ��
				   
 ��      ��	��������������
 ��      �ڣ�2003-12-19
 �޸���ʷ��
	  	����		�޸���		�޸�����
	  	------		---------	-------------
*****************************************************************/
//INT8U Get_Sector_PermissionA(INT8U sector, INT8U *key);

/*****************************************************************
 ����ԭ�ͣ�Get_Sector_PermissionB
 ����������ȡ�õ�N���Ķ�дȨ��
 ����������
 �������ƣ�	����/�����	����		����
 sector		����			INT8U		Ҫȡ��Ȩ�޵���
 key		����			INT8U *		Ҫȡ��Ȩ��������Կ
				
 ��  ��  ֵ��ok(0)-�ɹ�
				 notok(0xFF)-ʧ��
				   
 ��      ��	��������������
 ��      �ڣ�2003-12-19
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
		20070207    yufuyong	���ӳ�ֵ��Կ��֤
*****************************************************************/
//INT8U Get_Sector_PermissionB(INT8U sector, INT8U *key);

/******************************************************************************
 �������ƣ�AppendCPUCardRecord
 ������������CPU������д��¼
 ����������
 �������ƣ�	����/�����	����		����
 
 ��  ��  ֵ��ok(0)-�ɹ�
				 notok(0xFF)-ʧ��
				   
 ��      ��	������
 ��      �ڣ�2004-09-20
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
******************************************************************************/
//INT8U AppendCPUCardRecord(void);



/******************************************************************************
 �������ƣ�QuitWorkProcess
 �����������°����
 ����������
 �������ƣ�	����/�����	����		����
 
 ��  ��  ֵ����
				   
 ��      ��	������
 ��      �ڣ�2004-09-20
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
******************************************************************************/
void QuitWorkProcess(void);



/******************************************************************************
 �������ƣ�Driver_Card_Process
 ����������˾������������
 ����������
 �������ƣ�	����/�����	����		����
 ADF2_buf	����	INT8U *	1002Ŀ¼������
 
 ��  ��  ֵ��ok(0)-�ɹ�
				 notok(0xFF)-ʧ��
				   
 ��      ��	������
 ��      �ڣ�2004-09-02
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
******************************************************************************/
//INT8U Driver_Card_Process(INT8U const * const ADF2_buf);



/******************************************************************************
 �������ƣ�BeginWorkProcess
 �����������ϰ����
 ����������
 �������ƣ�	����/�����	����		����
 ADF2_buf	����	INT8U *	1002Ŀ¼������
 
 ��  ��  ֵ����
				   
 ��      ��	������
 ��      �ڣ�2004-09-20
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
******************************************************************************/
void BeginWorkProcess(INT8U const * const ADF2_buf);



/******************************************************************************
 �������ƣ�CheckPeriodOfValidity
 ������������Ч���ж�
 ����������
 �������ƣ�	����/�����	����		����
 start			����		INT32U		��������
 current		����		INT32U		��ǰ����
 end			����		INT32U		��Ч��ֹ����
 
 ��  ��  ֵ��ok(0)-�ɹ�
				 notok(0xFF)-ʧ��(����)
				   
 ��      ��	������
 ��      �ڣ�2004-09-20
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
******************************************************************************/
INT8U CheckPeriodOfValidity(INT32U start, INT32U current, INT32U end);



/******************************************************************************
 �������ƣ�GetFare
 �������������ݿ�����ȡƱ��
 ����������
 �������ƣ�	����/�����	����		����
 card_type			����		INT8U		������
 
 ��  ��  ֵ��Ʊ��
   
 ��      ��	������
 ��      �ڣ�2004-09-20
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
******************************************************************************/
//INT32U GetFare(INT8U card_type);



/******************************************************************************
 �������ƣ�M1CardProcess
 ����������M1����������
 ����������
 �������ƣ�	����/�����	����		����
 
 ��  ��  ֵ��ok(0)-�ɹ�
				 notok(0xFF)-ʧ��
				 0xF0-����ˢ��
				   
 ��      ��	������
 ��      �ڣ�2004-09-02
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
******************************************************************************/
//INT8U M1CardProcess(void);
/*****************************************************************
����ԭ�ͣ�FuncMenu
����������������Ϊ���ܲ˵���ʾ
������������
�������ƣ�	����/�����	����		����
-----------		-----------	------	   	-------
�� �� ֵ����
*****************************************************************/
void  FuncMenu(void);

void printf_debug(char * buf);
/******************************************************************************
 �������ƣ�StoreConsumeRecord
 �������������׼�¼�洢����
 �������ƣ�	����/�����	����		����
 cardinfo	����			CARD_INFO*	������Ϻ�Ŀ�Ƭ��������Ϣ
				
 ��  ��  ֵ����
 
 ��      ��	������
 ��      �ڣ�2004-09-23
 �޸���ʷ��
		����		�޸���		�޸�����
		2005.2.18		myron	                     in function 
******************************************************************************/
//INT8U StoreConsumeRecord(CARD_INFO *cardinfo);

/******************************************************************************
 �������ƣ�Setting_Card_Process
 �������������ÿ���������
 ����������
 �������ƣ�	����/�����	����		����
 ADF2_buf	����	INT8U *	1002Ŀ¼������
 
 ��  ��  ֵ��ok(0)-�ɹ�
				 notok(0xFF)-ʧ��
				   
 ��      ��	������
 ��      �ڣ�2004-09-02
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
******************************************************************************/
//INT8U Setting_Card_Process(INT8U const * const ADF2_buf);



/******************************************************************************
 �������ƣ�Attemper_Card_Process
 �������������ȿ���������
 ����������
 �������ƣ�	����/�����	����		����
 
 ��  ��  ֵ��ok(0)-�ɹ�
				 notok(0xFF)-ʧ��
				   
 ��      ��	������
 ��      �ڣ�2004-09-02
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
******************************************************************************/
//INT8U Attemper_Card_Process(void);
/******************************************************************************
 �������ƣ�Attemper_Card_Process
 �������������ȿ���������
 ����������
 �������ƣ�	����/�����	����		����
 
 ��  ��  ֵ��ok(0)-�ɹ�
				 notok(0xFF)-ʧ��
				   
 ��      ��	������
 ��      �ڣ�2004-09-02
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
******************************************************************************/
//INT8U Student_Card_Process(void);


/******************************************************************************
 �������ƣ�Set_Bus_Number_Process
 ���������������趨����������
 ����������
 �������ƣ�	����/�����	����		����
 
 ��  ��  ֵ��
				   
 ��      ��	������
 ��      �ڣ�2004-09-02
 �޸���ʷ��
		����		�޸���		�޸�����
		2005.2.21		myron			
******************************************************************************/
//void Set_Bus_Number_Process(void);



/*****************************************************************
 ����ԭ�ͣ�M1_Card_Consume_Process
 ����������M1���ѿ��ۿ�����
 ����������
 �������ƣ�	����/�����	����		����
 				
 				
 ��  ��  ֵ��	ok(0)-���ѳɹ�		
 				0xFF-����ʧ��
 				//0xFC-���Ǳ�ϵͳ��
				//0xFD-�п�������û�лظÿ�
				0xF0-����ˢ��
 ��      ��	������
 ��      �ڣ�2004-09-27
 �޸���ʷ��
	  	����		�޸���		�޸�����
	  	------		---------	-------------
*****************************************************************/
//INT8U M1_Card_Consume_Process(void);



/*****************************************************************
����ԭ�ͣ�Sec6_Block_Is_Legal
 �����������ж�һ����(16���ֽ�)�������Ƿ���ȷ���ж����4�ֽ��Ƿ�Ϊ18E718E7
 ����������
 �������ƣ�	����/�����	����		����
 data_to_verify	����	const INT8U *		Ҫ��֤���������ʼָ��
 				
 ��  ��  ֵ��ok(0)-��֤�ɹ�		
				notok(0xff)-��֤ʧ��
 ��      ��	������
 ��      �ڣ�2003-10-14
 �޸���ʷ��
	  	����		�޸���		�޸�����
	  	------		---------	-------------
*****************************************************************/
INT8U Sec6_Block_Is_Legal(const INT8U * data_to_verify);


/*****************************************************************
����ԭ�ͣ�Purse_Block_Is_Right
 �����������ж�Ǯ����(16���ֽ�)�������Ƿ���ȷ
 ����������
 �������ƣ�	����/�����	����		����
 data_to_verify	����	const INT8U *		Ҫ��֤���������ʼָ��
				
 ��  ��  ֵ��ok(0)-��֤�ɹ�		notok(0xff)-��֤ʧ��
 ��      ��	��������������
 ��      �ڣ�2003-10-14
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
*****************************************************************/
INT8U Purse_Block_Is_Right(INT8U * data_to_verify);



/*****************************************************************
����ԭ�ͣ�BlockToCash
 �����������ж�Ǯ����������ʽ->INT32U������
 ����������
 �������ƣ�	����/�����	����		����
 buf			����		const INT8U *		Ǯ����������ʽ
				
 ��  ��  ֵ��INT32U��Ǯ
 ��      ��	������
 ��      �ڣ�2004-09-29
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
*****************************************************************/
INT32U BlockToCash(const INT8U *buf);



/*****************************************************************
����ԭ�ͣ�BlockToCash
 ����������INT32U������->�ж�Ǯ����������ʽ
 ����������
 �������ƣ�	����/�����	����		����
 cash		����		INT32U		Ҫ�۵Ŀ�
				
 ��  ��  ֵ��ָ��ۿ�block��ʽ��ָ��
 ��      ��	������
 ��      �ڣ�2004-09-29
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
*****************************************************************/
unsigned long CashToBlock(INT32U cash);



/*****************************************************************
����ԭ�ͣ�CalTAC
 ��������������TAC�룬M1����������ר��
 ����������
 �������ƣ�	����/�����	����		����
				
 ��  ��  ֵ����
 ��      ��	������
 ��      �ڣ�2004-10-08
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
*****************************************************************/
void CalTAC(void);



/*****************************************************************
����ԭ�ͣ�Get_Record
����������ȡָ����ַ�ļ�¼

����������
�������ƣ�	����/�����	����		����
-----------		-----------	------	   	-------
 rec_buf		���	INT8U *	ȡ�ؼ�¼����ڴ�
 start_position	����	INT16U		p0+2����NewPtr����DevStat.record_base_ptr
 rec_pointer	����	INT16U		Ҫȡ��¼��ָ�룬��DevStat.record_number

��  ��  ֵ����
��      ��	��������������
��      �ڣ�2004-10-10
�޸���ʷ��
����		�޸���		�޸�����
------		---------	-------------
*****************************************************************/
//void Get_Record(INT8U *l_rec_buf, INT16U start_position, INT16U rec_pointer);



/*****************************************************************
����ԭ�ͣ�Get_RecordSingleFL
����������ȡָ����ַ�ļ�¼����FLASH����ר��

����������
�������ƣ�	����/�����	����		����
-----------		-----------	------	   	-------
 pri_slv		����		INT8U const		����FLASH���Ǵ�FLASH��������PRI �� ��SLV
 rec_buf		���	INT8U *	ȡ�ؼ�¼����ڴ�
 start_position	����	INT16U		p0+2����NewPtr����DevStat.record_base_ptr
 rec_pointer	����	INT16U		Ҫȡ��¼��ָ�룬��DevStat.record_number

��  ��  ֵ����
��      ��	��������������
��      �ڣ�2004-10-10
�޸���ʷ��
����		�޸���		�޸�����
------		---------	-------------
*****************************************************************/
//void Get_RecordSingleFL(INT8U const pri_slv, INT8U *l_rec_buf, INT16U start_position, INT16U rec_pointer);



/*****************************************************************
����ԭ�ͣ�BlackName_Find
 �������������Һ���������
 ����������
 �������ƣ�	����/�����	����		����
		card_number 		����		INT32U			Ҫ���ҵĿ��ţ�4�ֽ�
				
 ��  ��  ֵ��	ok(0)-�ҵ����Ǻ�����
				notok(0xFF)-û�ҵ������Ǻ�����
 
 ��      ��	������
 ��      �ڣ�2003-12-19
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
*****************************************************************/
//INT8U BlackName_Find(INT32U card_number);



/*****************************************************************
 ����ԭ�ͣ�Lock_M1_Card_Process
 ����������M1�����������У�������������
 ����������
 �������ƣ�	����/�����	����		����
 				
 				
 ��  ��  ֵ��	ok(0)-���ѳɹ�		
 				0xFF-����ʧ��
 ��      ��	������
 ��      �ڣ�2003-12-19
 �޸���ʷ��
	  	����		�޸���		�޸�����
	  	------		---------	-------------
*****************************************************************/
//INT8U Lock_M1_Card_Process(void);



/*****************************************************************
 ����ԭ�ͣ�	SingleFlashBoot
 ������������FLASH����������һ��FLASHʧЧ���ɼ���һ��FLASH������
 ����������	��������		����/�����		����			����
 pri_slv		����		INT8U const		����FLASH���Ǵ�FLASH��������PRI �� ��SLV
 
 ����ֵ��	��
 
 ��  �ߣ�	�����ִ�/����
 ��  �ڣ�	2005-01-31
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
*****************************************************************/
//void SingleFlashBoot(INT8U const pri_slv);


/******************************************************************************
 �������ƣ�M1_Driver_Card_Process
 ����������M1˾������������
 ����������
 �������ƣ�	����/�����	����		����
 ������ 	 ���״̬��

 
 ��  ��  ֵ��ok(0)-�ɹ�
			 notok(0xFF)-ʧ��
				   
 ��      ��	��myron
 ��      �ڣ�2005-02-17
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
******************************************************************************/
//INT8U M1_Driver_Card_Process(void);
/*****************************************************************
����ԭ�ͣ�  query_rec_num(void)
 ������������ѯ��¼����
 ����������
 �������ƣ�	����/�����	����		����
				
 ��  ��  ֵ����
 ��      ��	��������
 ��      �ڣ�2004-10-08
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
*****************************************************************/
uint query_rec_num(void);
/*****************************************************************
����ԭ�ͣ�CalConsumeTAC
 ��������������TAC�룬M1����������ר��
 ����������
 �������ƣ�	����/�����	����		����
				
 ��  ��  ֵ����
 ��      ��	������
 ��      �ڣ�2004-10-08
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
*****************************************************************/
void CalConsumeTAC(void);
/******************************************************************************
 �������ƣ�M1_Set_Card_Process
 ����������M1Ʊ�۳�����·���ÿ�����
 ����������
 �������ƣ�	����/�����	����		����
������		���״̬��
 
 ��  ��  ֵ��ok(0)-�ɹ�
			 notok(0xFF)-ʧ��
				   
 ��      ��	��myron
 ��      �ڣ�2005-02-16
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
******************************************************************************/
//INT8U M1_Set_Card_Process(void);

/******************************************************************************
 �������ƣ�M1_Gather_Card_Process
 ����������M1�ɼ�����������
 ����������
 �������ƣ�	����/�����	����		����
������		���״̬��
 
 ��  ��  ֵ��ok(0)-�ɹ�
			 notok(0xFF)-ʧ��
				   
 ��      ��	��myron
 ��      �ڣ�2005-02-16
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
******************************************************************************/
//INT8U M1_Gather_Card_Process(void);

/******************************************************************************
 �������ƣ�M1_Checked_Card_Process
 ����������M1��⿨��������
 ����������
 �������ƣ�	����/�����	����		����
������		���״̬��
 
 ��  ��  ֵ��ok(0)-�ɹ�
			 notok(0xFF)-ʧ��
				   
 ��      ��	��myron
 ��      �ڣ�2005-02-16
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
******************************************************************************/
//INT8U M1_Checked_Card_Process(void);

/******************************************************************************
 �������ƣ�M1_Signed_Card_Process
 ����������M1ǩ�㿨��������
 ����������
 �������ƣ� ����/����� ����		����
������		���״̬��
 
 ��  ��  ֵ��ok(0)-�ɹ�
			 notok(0xFF)-ʧ��
				   
 �� 	 �� ��myron
 �� 	 �ڣ�2005-02-16
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
******************************************************************************/
//INT8U M1_Signed_Card_Process(void);

/*****************************************************************
����ԭ�ͣ�Favor_Block_Is_Right
 �����������ж�Ǯ����(16���ֽ�)�������Ƿ���ȷ
 ����������
 �������ƣ�	����/�����	����		����
 data_to_verify	����	const INT8U *		Ҫ��֤���������ʼָ��
				
 ��  ��  ֵ��ok(0)-��֤�ɹ�		notok(0xff)-��֤ʧ��
 ��      ��	��������������,myron
 ��      �ڣ�2005.2.22
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
*****************************************************************/
//INT8U Favor_Block_Is_Right(const INT8U * data_to_verify);
/*****************************************************************
 ����ԭ�ͣ�	SingleFlashBoot
 ������������FLASH����������һ��FLASHʧЧ���ɼ���һ��FLASH������
 ����������	��������		����/�����		����			����
 pri_slv		����		INT8U const		����FLASH���Ǵ�FLASH��������PRI �� ��SLV
 
 ����ֵ��	��
 
 ��  �ߣ�	�����ִ�/����
 ��  �ڣ�	2005-01-31
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
*****************************************************************/
uchar  Sector6_Process(void);
/*****************************************************************
����ԭ�ͣ�DisplayPurchaseResult
������������ʾ���׽��(�ۿ�ɹ���)
����������
�������ƣ�	����/�����	����		����
cardinfo	����			const CARD_INFO*	������Ϻ�Ŀ�Ƭ��������Ϣ

��  ��  ֵ����
				
��      ��	������
��      �ڣ�2004-09-23
�޸���ʷ��
����		�޸���		�޸�����
2005.2.21		myron	                   in function 
*****************************************************************/
int DisplayPurchaseResult(const CARD_INFO *cardinfo);
/*****************************************************************
����ԭ�ͣ�DisplayPurchaseResult
������������ʾ���׽��(�ۿ�ɹ���)
����������
�������ƣ�	����/�����	����		����
cardinfo	����			const CARD_INFO*	������Ϻ�Ŀ�Ƭ��������Ϣ

��  ��  ֵ����
				
��      ��	������
��      �ڣ�2004-09-23
�޸���ʷ��
����		�޸���		�޸�����
2005.2.21		myron	                   in function 
*****************************************************************/
//INT8U Check_Money(void);
/******************************************************************************
 �������ƣ�M1_Favor_Card_Process()
 ����������M1�ɼ�����������
 ����������
 �������ƣ�	����/�����	����		����
������		���״̬��
 
 ��  ��  ֵ��ok(0)-�ɹ�
			 notok(0xFF)-ʧ��
				   
 ��      ��	��myron
 ��      �ڣ�2005-02-22
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
******************************************************************************/
//INT8U M1_Favor_Card_Process(void);


// *****************************************************************
// ���ܣ�����Ļ������ַ�
// ��ڲ�����	lb->��ʾ����ַ�
// ���ڲ�����	Null
// ���ߣ�
// ����������EH0218---COMMON.C���ӳ���
// *****************************************************************
char HextoBcd(char sour);

/******************************************************************************
 �������ƣ�StoreRecord
 �������������׼�¼�洢����
 �������ƣ�	����/�����	����		����
cardinfo	����			CARD_INFO*	������Ϻ�Ŀ�Ƭ��������Ϣ
				
 ��  ��  ֵ����
 
 ��      ��	������
 ��      �ڣ�2004-09-23
 �޸���ʷ��
		����		�޸���		�޸�����
		2005.2.18		myron	                     in function 
******************************************************************************/
//INT8U StoreRecord(CARD_INFO *cardinfo);

// *****************************************************************
// ���ܣ�ֱ�Ӱ�������
// ��ڲ�����	lkeyvalue->����ֵ
// ���ڲ�����	Null
// ���ߣ�
// ����������EH0218---COMMON.C���ӳ���
// *****************************************************************
//char DirectConsume(long lkeyvalue);

/******************************************************************************
 �������ƣ�M1CardQuery
 ����������M1����Ϣ��ѯ
 ����������
 �������ƣ�	����/�����	����		����
 
 ��  ��  ֵ��ok(0)-�ɹ�
				 notok(0xFF)-ʧ��
				   
 ��      ��	������
 ��      �ڣ�2004-09-02
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
******************************************************************************/
//INT8U M1CardQuery(void);

// *****************************************************************
// ���ܣ�������Ƭ������Ϣ
// ��ڲ�����	lkeyvalue->����ֵ
// ���ڲ�����	Null
// ���ߣ�
// ����������EH0218---COMMON.C���ӳ���
// *****************************************************************
void GetCardType(INT8U  cardtype, INT8U * temp);



/*****************************************************************
����ԭ�ͣ�TRACE
�����������������������printf������Ļ��ѭ����ʾ
������������
�������ƣ�	����/�����	����		����
-----------		-----------	------	   	-------
�� �� ֵ����
*****************************************************************/
//void TRACE(char * fmt, ...);
//
//void TRACE232(char * fmt, ...);



/******************************************************************************
 �������ƣ�htoa
 ����������hexת��Ϊascii��
 ����������
 �������ƣ� ����/����� ����		����
				
 ��  ��  ֵ��ok(0)-�ɹ�
			   	notok(0xFF)-ʧ��
				   
 �� 	 �� ������
 �� 	 �ڣ�2005-04-07
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
******************************************************************************/
//void htoa(void *des, INT8U *sour, int hex_len);



/******************************************************************************
 �������ƣ�CardQuery
 ��������������Ϣ��ѯ
 ����������
 �������ƣ�	����/�����	����		����
 
 ��  ��  ֵ��ok(0)-�ɹ�
				 notok(0xFF)-ʧ��
				   
 ��      ��	������
 ��      �ڣ�2004-09-02
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
******************************************************************************/
INT8U CardQuery(void);



/******************************************************************************
 �������ƣ�CPUCardQuery
 ����������CPU����Ϣ��ѯ
 ����������
 �������ƣ�	����/�����	����		����
 
 ��  ��  ֵ��ok(0)-�ɹ�
				 notok(0xFF)-ʧ��
				   
 ��      ��	������
 ��      �ڣ�2004-09-02
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
******************************************************************************/
//INT8U CPUCardQuery(void);



/******************************************************************************
 �������ƣ�EA_ucClrKeyBuf
 �������������ռ�ֵ
 ����������
 �������ƣ� ����/����� ����		����
				
 ��  ��  ֵ����
				   
 �� 	 �� ������
 �� 	 �ڣ�2005-04-07
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
******************************************************************************/
//void EA_ucClrKeyBuf(void);



/******************************************************************************
 �������ƣ�CPUCardQuery
 ����������CPU����Ϣ��ѯ
 ����������
 �������ƣ�	����/�����	����		����
 
 ��  ��  ֵ��ok(0)-�ɹ�
				 notok(0xFF)-ʧ��
				   
 ��      ��	������
 ��      �ڣ�2004-09-02
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
******************************************************************************/
INT8U QueryNormal(void);



/******************************************************************************
 �������ƣ�QueryFriend
 �������������鿨��һ���Կ���Ϣ��ѯ
 ����������
 �������ƣ�	����/�����	����		����
 
 ��  ��  ֵ��ok(0)-�ɹ�
				 notok(0xFF)-ʧ��
				   
 ��      ��	������
 ��      �ڣ�2004-09-02
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
******************************************************************************/
//INT8U QueryFriend(void);



// *****************************************************************
// ���ܣ�		��ʱi����
// ��ڲ�����	i->��ʱʱ��
// ���ڲ�����	Null
// ���ߣ�
// ����������EH0218---COMMON.C���ӳ���
// *****************************************************************
//void EA_uiInkeyMs(short i);



/*****************************************************************
 ����ԭ�ͣ�OverProcess
 ����������M1���ѿ��ۿ�������β
 ����������
 �������ƣ�	����/�����	����		����
				
				
 ��  ��  ֵ��	ok(0)-���ѳɹ�		
				0xFF-����ʧ��
				//0xFC-���Ǳ�ϵͳ��
				//0xFD-�п�������û�лظÿ�
 ��      ��	������
 ��      �ڣ�2004-09-27
 �޸���ʷ��
		����		�޸���		�޸�����
*****************************************************************/
//INT8U OverProcess(void);



/******************************************************************************
 �������ƣ�monitor
 ������������غ���
 �������ƣ�	����/�����	����		����
				
 ��  ��  ֵ����
 
 ��      ��	������
 ��      �ڣ�2004-09-02
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
******************************************************************************/
//void monitor(void);



/******************************************************************************
 �������ƣ�CPU_Driver_Card_Process
 ����������CPU˾������������
 ����������
 �������ƣ�	����/�����	����		����
 ������ 	 ���״̬��

 
 ��  ��  ֵ��ok(0)-�ɹ�
			 notok(0xFF)-ʧ��
				   
 ��      ��	��myron
 ��      �ڣ�2005-02-17
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
******************************************************************************/
//INT8U CPU_Driver_Card_Process(void);



/******************************************************************************
 �������ƣ�Get_Passwd
 ������������֤����������ó�ʼ�����ݿ�״̬
 ����������
 �������ƣ�	����/�����	����		����
				
 ��  ��  ֵ��ok(0)-��֤�ɹ�
				 notok(0xFF)-��֤ʧ��

 ��      ��	������
 ��      �ڣ�2005-05-20
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
******************************************************************************/
//INT8U Get_Passwd(void);



/******************************************************************************
�������ƣ�get_key
�����������õ�һ�����������ȣ�ֻ����1234567890
������������

��  ��  ֵ	���ü�ֵ
��      ��	����̳
��      �ڣ�2005-04-08
�޸���ʷ��
����		�޸���		�޸�����
------		---------	---------
******************************************************************************/
//INT8U get_key(void);



/******************************************************************************
 �������ƣ�trans_key
 ����������ת����ֵ��ascii��
 ����������
 �������ƣ� ����/����� ����		����
	key		����		long		delay_and_wait_key�ķ���ֵ��ֻ����0-9

 ��  ��  ֵ��ascii��ļ�ֵ
				   
 �� 	 �� ������
 �� 	 �ڣ�2005-04-07
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
******************************************************************************/
//INT8U trans_key(long key);



/******************************************************************************
 �������ƣ�ascii_to_hex
 ������������һ��0-9��ascii���룬ת��Ϊ16������
 �������ƣ� ����/����� ����        ����
 num        ����        INT8U           Ҫת��Ϊascii���������Ҫ��Χ0-9

 ��  ��  ֵ��ת�����16������

 ��      �� ������
 ��      �ڣ�2004-09-02
 �޸���ʷ��
		����        �޸���      �޸�����
		------      ---------   -------------
******************************************************************************/
//INT8U ascii_to_hex(INT8U num);



/******************************************************************************
 �������ƣ�CPU_Card_Consume_Process
 ����������CPU��ͨ����������
 ����������
 �������ƣ�	����/�����	����		����
 ������ 	 ���״̬��

 
 ��  ��  ֵ��ok(0)-�ɹ�
			 notok(0xFF)-ʧ��
			 0xF0-����ˢ��
				   
 ��      ��	������
 ��      �ڣ�2005-02-17
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
******************************************************************************/
//INT8U CPU_Card_Consume_Process(void);



// *****************************************************************
// ���ܣ�������Ƭ������Ϣ
// ��ڲ�����	lkeyvalue->����ֵ
// ���ڲ�����	Null
// ���ߣ�
// ����������EH0218---COMMON.C���ӳ���
// *****************************************************************
//void GetTradeType(INT8U  cardtype, INT8U * temp);



/*****************************************************************
����ԭ�ͣ�bcdhex
����������bcd��ת��Ϊhex��
����������
�������ƣ�	����/�����	����		����
-----------		-----------	------	   	-------
bcd				����			INT8U		�����bcd��

��  ��  ֵ��ת�����hex��
��      ��	������
��      �ڣ�2006-01-09
�޸���ʷ��
����		�޸���		�޸�����
------		---------	-------------
*****************************************************************/
//INT8U bcdhex(INT8U bcd);



/*****************************************************************
����ԭ�ͣ�CPUCard_Application_Lock
 ������������CPU��
 ����������
 �������ƣ�	����/�����	����		����
				
 ��  ��  ֵ��	ok(0)-�ɹ�		
				notok (0xFF)(-1)-ʧ�ܻ�Ƭ�Ѿ�����
 ��      ��	������
 ��      �ڣ�2004-10-11
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
*****************************************************************/
//INT8U CPUCard_Application_Lock(void);



// *****************************************************************
// ���ܣ�		����˵�
// ��ڲ�����	startline->��ʼ��
//				p_menu->�˵�
//				p_cPtr->��ǰ��
//				p_lPtr->�ڲ˵���������λ��
// ���ڲ�����	Null
// ����:		
// ���ߣ�	
// ����������EH0218---COMMON.C���ӳ���
// *****************************************************************
//short browse_menu(int startline,unsigned char *p_menu,int *p_cPtr,int *p_lPtr );


/******************************************************************************
 �������ƣ�ascii
 ������������һ��0-9����ת��Ϊascii���룬�˺���Ҳ����չ����ͨѶ�У�ͨѶ���漰��һЩA-F�ķǷ����룬��Ϊ�Ϸ�
 �������ƣ�	����/�����	����		����
 num		����		INT8U			Ҫת��Ϊascii���������Ҫ��Χ0-9
 
 ��  ��  ֵ��ת�����ascii����
 
 ��      ��	������
 ��      �ڣ�2004-09-02
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
******************************************************************************/
//INT8U ascii(INT8U num);



/******************************************************************************
 �������ƣ�M1_Inspector_Card_Process
 ����������M1���鿨��������
 ����������
 �������ƣ�	����/�����	����		����
������		���״̬��
 
 ��  ��  ֵ��ok(0)-�ɹ�
			 notok(0xFF)-ʧ��
				   
 ��      ��	��myron
 ��      �ڣ�2005-02-16
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
******************************************************************************/
//INT8U M1_Inspector_Card_Process(void);



/******************************************************************************
 �������ƣ�monitor_normal
 ������������������ģʽ��غ���
 �������ƣ�	����/�����	����		����
				
 ��  ��  ֵ����
 
 ��      ��	������
 ��      �ڣ�2004-09-02
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
******************************************************************************/
void monitor_normal(void);



// *****************************************************************
// ���ܣ�ֱ�Ӱ�������
// ��ڲ�����	��
// ���ڲ�����	Null
// ���ߣ�
// ����������EH0218---COMMON.C���ӳ���
// *****************************************************************
INT8U NormalConsume(INT32U fare_in);



// *****************************************************************
// ���ܣ�		PHoldTime_Setting
// ˵��:		ˢ������ʱ������
// ��ڲ�����	
// ���ڲ�����	Null
// ���ߣ�
// ����ֵ��		-1:�ػ�					
// *****************************************************************
int PHoldTime_Setting(void);



// *****************************************************************
// ���ܣ�		MaxPriceLimitSet
// ˵��:		����������ѽ��
// ��ڲ�����	
// ���ڲ�����	Null
// ���ߣ�
// ����ֵ��							
// *****************************************************************
INT8U MaxPriceLimitSet(void);

/*****************************************************************
����ԭ�ͣ�CPU_SET_PROCESS
����������������ΪCPU���ÿ��������
������������
�������ƣ�	����/�����	����		����
-----------		-----------	------	   	-------
�� �� ֵ����
*****************************************************************/
int CPU_SET_PROCESS(INT8U * data);


/*****************************************************************
����ԭ�ͣ�CPU_OPER_PROCESS
����������������ΪCPU����Ա���������
������������
�������ƣ�	����/�����	����		����
-----------		-----------	------	   	-------
�� �� ֵ��     -1:��һ������Աû��ǩ��
 		       -2:����Ա�������
 		       -3:��Ƭ�Ѿ�����Ч��
 		       -4:��Ƭ�Ѿ�ͣ��
 		       -5: û�п��õ���Ч���� 
 		       1:  �ɹ�
*****************************************************************/
int CPU_OPER_PROCESS(INT8U * data);

/*****************************************************************
����ԭ�ͣ�Login
����������������Ϊ����Ա��½����
������������
�������ƣ�	����/�����	����		����
-----------		-----------	------	   	-------
�� �� ֵ��	-1:�޿�
			  -2:ѡ��Ŀ¼����
			  -3:������֤����
			  -4:

*****************************************************************/
int CpuProcess(void);


// *****************************************************************
// ���ܣ�		Login
// ˵��:		cpu����½
// ��ڲ�����	
// ���ڲ�����	Null
// ���ߣ�
// ����ֵ��							
// *****************************************************************
void Login(void);


/******************************************************************************
 �������ƣ�CardInit
 ������������Ƭ��ʼ��,����ҿ�����
 ����������
 �������ƣ�����/�����	����		����
 
 ��  ��  ֵ��    ok(0)-�ɹ�
				 notok(0xFF)-ʧ��
				 0xFE-û�ҵ���
				 0xF0-����ˢ��
				   
 ��      ��	���ڸ���/������
 ��      �ڣ�2012-12-10
 �޸���ʷ��
		����		�޸���		�޸�����
******************************************************************************/
//INT8U CardInit(void);

/******************************************************************************
 �������ƣ�SetCardUseDT
 �������������ÿ�Ƭ��Ч��
 ����������
 �������ƣ�����/�����	����		����
 
 ��  ��  ֵ��    ok(0)-�ɹ�
				 notok(1)-ʧ��
				 				   
 ��      ��	���ڸ���
 ��      �ڣ�2007-01-30
 �޸���ʷ��
		����		�޸���		�޸�����
******************************************************************************/
INT8U SetCardUseDT(void);

/******************************************************************************
 �������ƣ�GetConsumeTradeAmt
 ������������ý��׽��
 ����������
 �������ƣ�����/�����	����		����
 
 ��  ��  ֵ��    ok(0)-�ɹ�
				 notok(1)-ʧ��
				 				   
 ��      ��	���ڸ���
 ��      �ڣ�2007-01-30
 �޸���ʷ��
		����		�޸���		�޸�����
******************************************************************************/
INT8U GetConsumeTradeAmt(void);

/******************************************************************************
 �������ƣ�GetTradeAmt
 ������������ý��׽��
 ����������
 �������ƣ�����/�����	����		����
 
 ��  ��  ֵ��    ok(0)-�ɹ�
				 notok(1)-ʧ��
				 				   
 ��      ��	���ڸ���
 ��      �ڣ�2007-01-30
 �޸���ʷ��
		����		�޸���		�޸�����
******************************************************************************/
INT8U GetTradeAmt(void);



/******************************************************************************
 �������ƣ�GetTradeInfo
 ������������óֿ��˽�����Ϣ
 ����������
 �������ƣ�����/�����	����		����
 
 ��  ��  ֵ��    ok(0)-�ɹ�
				 notok(1)-ʧ��
				 				   
 ��      ��	���ڸ���
 ��      �ڣ�2007-01-30
 �޸���ʷ��
		����		�޸���		�޸�����
******************************************************************************/
INT8U GetTradeInfo(void);


/******************************************************************************
 �������ƣ�GetPersonInfo
 ������������óֿ�����Ϣ
 ����������
 �������ƣ�����/�����	����		����
 
 ��  ��  ֵ��    ok(0)-�ɹ�
				 notok(0xFF)-ʧ��
				 				   
 ��      ��	���ڸ���
 ��      �ڣ�2007-01-30
 �޸���ʷ��
		����		�޸���		�޸�����
******************************************************************************/
INT8U GetPersonInfo(void);

/*****************************************************************
 ����ԭ�ͣ�M1_Card_Credit_Process
 ����������M1���ѿ���ֵ����
 ����������
 �������ƣ�	����/�����	����		����
				
				
 ��  ��  ֵ��	ok(0)-��ֵ�ɹ�		
				0xFF-��ֵʧ��
				//0xFC-���Ǳ�ϵͳ��
				//0xFD-�п�������û�лظÿ�
				0xF0-����ˢ��
 ��      ��	������
 ��      �ڣ�2004-09-27
 �޸���ʷ��
		����		�޸���		�޸�����
		2005.2.21		myron			in function
*****************************************************************/
//INT8U M1_Card_Credit_Process(void);

/*****************************************************************
����ԭ�ͣ�OperCardCheck
����������������Ϊ����Ƿ��в���Ա���ڣ�
				//���ж��Ƿ�Ϊͬһ�ſ�
������������
�������ƣ�	����/�����	����		����
-----------		-----------	------	   	-------
�� �� ֵ��	-1:�޿�
			  -2:ѡ��Ŀ¼����
			  -3:������֤����
			  -4:

*****************************************************************/
int OperCardCheck(void);

/******************************************************************************
 �������ƣ�M1CardProcess
 ����������M1����������
 ����������
 �������ƣ�	����/�����	����		����
 
 ��  ��  ֵ��ok(0)-�ɹ�
				 notok(0xFF)-ʧ��
				 0xF0-����ˢ��
				   
 ��      ��	������
 ��      �ڣ�2004-09-02
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
******************************************************************************/
INT8U CheckCardCredit(void);

/*****************************************************************
����ԭ�ͣ�CpuDecrement
����������������Ϊ����Ա���ۿ�
������������
�������ƣ�	����tFlag---��������:0---�ۿ�ף�1:��ֵ����
///�����	����		����
-----------		-----------	------	   	-------
�� �� ֵ��	-1:�޿�
			  -2:ѡ��Ŀ¼����
			  -3:������֤����
			  -4:

*****************************************************************/
int CpuDecrement(INT8U * data, INT32U trade_amt);

/*****************************************************************
����ԭ�ͣ�CpuIncrement
����������������Ϊ����Ա���ۿ�
������������
�������ƣ�	����tFlag---��������:0---�ۿ�ף�1:��ֵ����
///�����	����		����
-----------		-----------	------	   	-------
�� �� ֵ��	-1:�޿�
			  -2:ѡ��Ŀ¼����
			  -3:������֤����
			  -4:

*****************************************************************/
int CpuIncrement(INT8U * data, INT32U trade_amt);
/*****************************************************************
����ԭ�ͣ�INT8U Logout(void)
����������������Ϊǩ�˹���
������������
�������ƣ�	����/�����	����		����
-----------		-----------	------	   	-------
�� �� ֵ��ok  ?  notok
*****************************************************************/
INT8U Logout(void);

/*****************************************************************
����ԭ�ͣ�OperTradeProcess
����������������Ϊ����Ա�����׺���
������������
�������ƣ�	����tFlag---��������:0---�ۿ�ף�1:��ֵ����
///�����	����		����
-----------		-----------	------	   	-------
�� �� ֵ��	-1:�޿�
			  -2:ѡ��Ŀ¼����
			  -3:������֤����
			  -4:

*****************************************************************/
int OperTradeProcess(int tFlag, INT32U trade_amt);

/*****************************************************************
����ԭ�ͣ�Cpucard_Init
������������������ʼ��SAM OR CPU��ͨ��
������������
�������ƣ�	����/�����	����		����
-----------		-----------	------	   	-------
�� �� ֵ����
*****************************************************************/

INT8U Cpucard_Init(void);

/******************************************************************************
 �������ƣ�Cpucard_power_on
 ������������λCPU��
 ����������
 �������ƣ�	����/�����	����		����  
   
 ��  ��  ֵ��ok(0)-�ɹ�
			 notok(0xFF)-ʧ��
				   
 ��      ��	��������
 ��      �ڣ�2012-12-12
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
******************************************************************************/
INT8U Cpucard_power_on(INT8U *prcvbuf, uint *prcvLen);

/*****************************************************************
����ԭ�ͣ�Cpucard_power_off
�����������������ر�ͨ��
������������
�������ƣ�	����/�����	����		����
-----------		-----------	------	   	-------
�� �� ֵ����
*****************************************************************/
void Cpucard_power_off(void);

/*****************************************************************
����ԭ�ͣ�CPU_OPER_DOWN
����������������ΪCPU����Ա���°ദ��
������������
�������ƣ�	����/�����	����		����
-----------		-----------	------	   	-------
�� �� ֵ��   ��
*****************************************************************/
void CPU_OPER_DOWN(void);



/******************************************************************************
 �������ƣ�GrayCardDeal
 ������������Ƭ��������
 ����������
 �������ƣ�����/�����	����		����
 
 ��  ��  ֵ��    ok(0)-�ɹ�
				 notok(0xFF)-ʧ��
				 0xFE-û�ҵ���
				 0xF0-����ˢ��
				   
 ��      ��	���ڸ���
 ��      �ڣ�2007-01-30
 �޸���ʷ��
		����		�޸���		�޸�����
******************************************************************************/
//INT8U GrayCardDeal(void);

/*****************************************************************
����ԭ�ͣ�CharToCash
 �����������ж�Ǯ���������->INT32U������
 ����������
 �������ƣ�	����/�����	����		����
 buf			����		const INT8U *		Ҫ��֤���������ʼָ��
				
 ��  ��  ֵ��INT32U��Ǯ
 ��      ��	������
 ��      �ڣ�2004-09-29
 �޸���ʷ��
		����		�޸���		�޸�����
		------		---------	-------------
*****************************************************************/
INT32U CharToCash(const INT8U *buf);





/******************************************************************************
 �������ƣ�CardCreditAmt
 ������������ֵ��������
 ����������
 �������ƣ�����/�����	����		����
 
 ��  ��  ֵ��    ok(0)-�ɹ�
				 notok(0xFF)-ʧ��
				 0xFE-û�ҵ���
				 0xF0-����ˢ��
				   
 ��      ��	���ڸ���
 ��      �ڣ�2007-01-30
 �޸���ʷ��
		����		�޸���		�޸�����
******************************************************************************/
INT8U CardCreditAmt(void);

/******************************************************************************
 �������ƣ�CardSale
 �����������ۿ���������
 ����������
 �������ƣ�����/�����	����		����
 
 ��  ��  ֵ��    ok(0)-�ɹ�
				 notok(0xFF)-ʧ��
				 0xFE-û�ҵ���
				 0xF0-����ˢ��
				   
 ��      ��	���ڸ���
 ��      �ڣ�2007-01-30
 �޸���ʷ��
		����		�޸���		�޸�����
******************************************************************************/
INT8U CardSale(void);
/******************************************************************************
 �������ƣ�CardFlush
 �����������ۿ���������
 ����������
 �������ƣ�����/�����	����		����
 
 ��  ��  ֵ��    ok(0)-�ɹ�
				 notok(0xFF)-ʧ��
				 0xFE-û�ҵ���
				 0xF0-����ˢ��
				   
 ��      ��	���ڸ���
 ��      �ڣ�2007-01-30
 �޸���ʷ��
		����		�޸���		�޸�����
******************************************************************************/
INT8U CardFlush(void);

//INT8U M1_Card_Consume_ProcessA(void);
/******************************************************************************
 �������ƣ�StoreGrayRecord
 �����������Ҽ�¼�洢����
 �������ƣ�	����/�����	����		����
				
 ��  ��  ֵ����
 
 ��      ��	���ڸ���
 ��      �ڣ�2007-04-29
 �޸���ʷ��
		����		�޸���		�޸�����
******************************************************************************/
//INT8U StoreGrayRecord(void);
/******************************************************************************
 �������ƣ�download_whiltelist(void)
 �����������������µİ�����
 �������ƣ���
 ����		   ��
				
 ��  ��  ֵ����
 
 ��      ��	��������
 ��      �ڣ�2013-06-2
 �޸���ʷ��
		����		�޸���		�޸�����
******************************************************************************/
uchar download_whiltelist(void);



#endif

