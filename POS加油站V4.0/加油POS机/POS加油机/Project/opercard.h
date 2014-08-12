/*****************************************************************
Copyright 2003 �����״�ͨ���޹�˾
All rights riserved.

�ļ���	��bus.h
ģ�����ƣ������˹��õ�һЩdefine

��ǰ�汾��0.0.1
�޸���	������
������ڣ�2004.09.02
 *****************************************************************/
#ifndef __OPERCARD_H__
#define __OPERCARD_H__
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

#define MAX_REC_NUM						2000
#define CPU_CARD 						0x08
//#define M1_CARD 						0x04
#define M1_CARD 						0x04
#define DISP_DELAY						600					//���׳ɹ������ͣ������Ļ�ϵ�ʱ��
#define SINGLE_KEY_QUERY_DELAY			500					//������ѯʱ


//������
//���ѿ�
#define CARDT_NORMAL        0x01    //��ͨ��
#define CARDT_STUDENT		0x02    //���꿨(���)
#define CARDT_OLDMAN   		0x03    //���곤�ٿ�
#define CARDT_FREE			0x04    //ְ����
#define CARDT_MEMORY        0x05    //���
#define CARDT_DRIVER        0x06    //Ա����


 //����
#define CARDT_SET           0x10    //��·Ʊ�۳������ÿ�
#define CARDT_GATHER        0x11    //���ݲɼ���
#define CARDT_SIGNED        0x12    //ǩ�㿨
#define CARDT_CHECKED       0x13    //��⿨
#define CARDT_INSPECTOR      0x18    //���鿨


//֤������
#define  IDENTIFY_TYPE 		0x01//1:���֤
#define  OLD_TYPE			0x02//2:����֤
#define  RETIRE_TYPE		0x03//3:����֤
#define  OFFICER_TYPE		0x04//4:����֤
#define	 RESIDENCE_TYPE		0x05//5:����֤
#define	 STUDENT_TYPE		0x06//6:ѧ��֤


//���ݺ�����
#define POS_T_NORMAL	0x34			//�����ɼ�����
#define POS_T_ABNORMAL	0x36			//�쳣�ɼ�����


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
															����struct���岿��
******************************************************************************/

//���㵥
typedef struct _BANLANCE_IN_HAND_
{
	INT8U	acnt_id[2];        //�̻�ID
	INT8U	equ_id[3];         //�豸ID
	INT8U	oper_id[3];		   //����������Ա

	BUS_TIME work_datetime;	   //�ϰ�ʱ��
	BUS_TIME down_datetime;	   //�°�ʱ��
//����Ӧ�÷�Ϊ����ȡͳ�ơ���¼�ս���
	INT8U   consume_kind[4];      //��������
	INT16U  consume_cnt[4];    //���ʹ���
	INT32U	consume_cap[4];    //�������
	INT32U	consume_amt[4];    //���ͽ��
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
	INT16U record_base_ptr;				//δ�ϴ���¼ָ����ʼָ��(ԭp0+2) 

	INT32U record_number;				//δ�ϴ��ļ�¼����(ԭp0+4)
	INT8U POS_type;						//������
	INT8U card_password[6];				//����Ա����
	INT8U Gprs_On_Flag;                 //gprsͨ���Ƿ���ñ�־
	INT8U black_flag;
	INT16U current_record_index;

	//��������Ҫ�洢��flash�е�����
	INT8U   start;	
	INT8U   ServerIp[4];                  //��������ַ
	INT8U   ServerPort[2];                //�������˿ں�
	INT8U   dev_trade_no[3];              //�豸�������
	INT8U   oper_cardno[3];				//����Ա����

	INT16U  send_total_num;				//��ǰ���ͼ�¼���
	INT8U   cur_driver_number[4];		//����˾���ţ��°���0------>�ĳɵ������Ա��
	INT8U   driver_is_working;          //����Ա��¼��־
	INT16U  black_name_number;          //����������
	INT8U	black_version[7];           //�汾��
	INT8U	bus_number[5];              //���ƺ�
	INT8S   user_passwd[7];			    //�������ò˵�������
	INT8U   samcompcode;			    //SAM�����̴���
	_BANLANCE_IN_HAND  binhand;         //���㵥
										//
	INT8U   fill_mode;				    //��Ʒ��ʾ0������ 1������93#��2.����97#��3.���� 
	INT32U  price[4];                   //����

/*--------------------20070131�����ӵ�--------------------*/
	INT8U   acnt_id[2];                 //�̻��ű���
	INT8U   equ_id[3];                  //�豸����
	BUS_TIME effect_time;               //�ͼ۸�����Чʱ��
	INT32U  price_backup[4];            //�����µ�ȼ�ϵ���
//  INT8U   update_price_done;			//1,��ʾ�Ѿ����¹��ˣ�0����ʾû�и���
	INT8U   request_all_rec;
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
		INT8U card_number[8];  //����
		CARD_NUM detail_card_number;
	}_CARD_NUMBER;

	BUS_TIME card_in_time;			//����ʱ��
	INT8U   card_begin_dt[4];       //��Ƭ��������yyyymmdd
	INT8U   card_useful_dt[4];      //��Ƭ��Ч��yyyymmdd
	INT8U   card_add_dt[4];         //��Ƭ�ӿ�ʱ��yymmddhh
	
	INT8U  card_type;				//������
	INT8U  fuel_type;				//������0����Ȼ����1������ 2������
	INT32U capacity;				//���ͣ�������λL
	INT32U price;					//���͵���
	INT32U fare;					//���ͽ��
	INT32U cur_fare;				//��ǰӦ�۽�CPU������ʹ��

	INT8U init_purchase_serial_num[2];		//��ʼ�Ŀ������ѽ������
	INT8U purchase_serial_num[3];			//��ʼ�Ŀ��ڽ������+1

	INT32U init_balance;					//ԭ���
	INT32U balance;							//���׺����
	INT8U card_purchase_type;				//��������
	INT8U tac[4];							//TAC��
	INT8U mac2[4];							//CPU�����ص�MAC2

	INT8U bus_number[6];                    //���ƺ���
	INT8U csn[4];							//Ӳ������
	INT8U authentication_code[4];			//����֤��

	INT8U card_random_num[4];				//CPU�������
	INT8U cpu_card_key_version;				//CPU����Կ�汾��
	INT8U arithmetic_sign;					//�㷨��ʶ

	INT32U psam_purchase_serial_num;	    //PSAM���������
	INT8U mac1[4];							//PSAM���������MAC1

	//����˾���������°������Ա����
	INT8U drv_card_type;					//˾��������

	/////////////////////////////////////////////
	//�Żݿ���Ӧ��Ϣ
	/////////////////////////////////////////////
	INT16U init_time_balance;               //ԭ�������
	INT16U time_balance;                    //���׺�������

	INT8U station_number[2];                //վ����
	INT8U checked_flag;                     //����־


	INT8U driver_number[3];			        //˾����

/*--------------20070131�����ӵ��û���������Ϣ--------------------*/
	INT8U acnt_id[3];                       //�������ı���
	INT8U spot_id[3];                       //�ۿ������
	INT8U equ_id[3];                        //�豸����
	INT8U key_type;                         //��Կ����
	INT8U pay_type;                         //���ʽ
	INT8U deposit_amt[2];                   //Ѻ��
	INT8U isam_no[6];                       //ISAM������
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
	INT8U equ_id[3];                 //�豸����
	INT8U oper_id[3];                //����Ա��
	INT32U price;                    //��Ʒ����
	INT8U card_serial_number[8];     //���ķ�����ˮ��	8�ֽ�
	INT8U card_type;                 //��Ƭ����  1�ֽ�
	INT8U card_purchase_type;        //��������	1�ֽ�
                                     

	INT8U pos_purchase_serial_num[3];//�ն˽������3�ֽ�
	INT8U bus_number[5];             //�����ƺ�	5�ֽ�
	INT8U driver_no[3];              //Ա��������
    INT32U fare;                     //���ѽ��   4�ֽ�
	INT32U capacity;                 //�������  4�ֽ�
	INT8U fuel_type;                 //ȼ��Ʒ��:0.��Ȼ����1.����93��2.����97��3.����
	BUS_TIME card_in_time;           //����ʱ��	7�ֽ�
									 

	INT8U purchase_serial_num[2];    //��ʼ�Ŀ��ڽ������+1	2�ֽ�
	INT8U tac[4];                    //TAC��	4�ֽ�
	INT16U crc;
}   RECORD;

typedef struct _GPRS
{
	INT16U record_base_ptr;                 //δ�ϴ���¼ָ����ʼָ��(ԭp0+2)
	INT16U record_number;                   //δ�ϴ��ļ�¼����(ԭp0+4)

	INT8U have_driver_rec;                  //�Ƿ���˾�����°��¼
	INT16U snd_len;                         //���ͳ���
	INT16U rcv_len;                         //���ճ���

	INT8U have_drec;						//�Ƿ���δ�ϴ���drec����=TRUE ��=FALSE
	INT8U update_time;						//�Ƿ�׼���������ͬ��ʱ�䣬��=TRUE ��=FALSE
	INT8U driver_is_working;				//���°��־

	INT16U package_num;						//����
	INT16U last_packgae_rec_num;			//���һ���ļ�¼��

}GPRS;

//GPRS gprs;

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

// *****************************************************************
// ���ܣ�		����
// ��ڲ�����	i=0:����"��"һ��
//				i=1:����"������"����
// ���ڲ�����	Null
// ���ߣ�
// ����������EH0218---COMMON.C���ӳ���
// *****************************************************************
void Beep(int i);
/*****************************************************************
����ԭ�ͣ�TextHeadDisplayTimer(void)
����������������Ϊ�ڿ�ͷ��λ����ʾʱ��
������������
�������ƣ�	����/�����	����		����
-----------		-----------	------	   	-------
�� �� ֵ��ok  ?  notok
*****************************************************************/
void TextHeadDisplayTimer(void);

// *****************************************************************
// ���ܣ�		����
// ��ڲ�����	i=0:����"��"һ��
//				i=1:����"������"����
// ���ڲ�����	Null
// ���ߣ�
// ����������EH0218---COMMON.C���ӳ���
// *****************************************************************
void Beeperr(void);

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

void printf_debug(char * buf);


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


// *****************************************************************
// ���ܣ�����Ļ������ַ�
// ��ڲ�����	lb->��ʾ����ַ�
// ���ڲ�����	Null
// ���ߣ�
// ����������EH0218---COMMON.C���ӳ���
// *****************************************************************
char HextoBcd(char sour);


// *****************************************************************
// ���ܣ�������Ƭ������Ϣ
// ��ڲ�����	lkeyvalue->����ֵ
// ���ڲ�����	Null
// ���ߣ�
// ����������EH0218---COMMON.C���ӳ���
// *****************************************************************
void GetCardType(INT8U  cardtype, INT8U * temp);

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


INT8U Logout(void);

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
INT8U download_whiltelist(void);
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
INT8U download_price_table(void);

INT8U Is_timeup(void);

#endif

