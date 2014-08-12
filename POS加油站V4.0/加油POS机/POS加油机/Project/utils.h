

#ifndef   __UTILS_H__
#define   __UTILS_H__

char *fmtAmtStr( char *destbuf, char *srcbuf, unsigned char srclen);
char *fmtDateStr(char *destbuf, char *srcbuf, unsigned char srclen);
char *fmtTimeStr(char *destbuf, char *srcbuf, unsigned char srclen);
void *fmtDTStr(char *destbuf, char *srcbuf, unsigned char srclen);
void var_asc2bcd(unsigned char *dest, unsigned char  *src, unsigned int len);
unsigned char asc2bcd(unsigned char   *src) ;
unsigned char CheckDateTimeValidity(unsigned char *buf);
unsigned int unpackInt(unsigned char *msg);
void var_bcd2asc(unsigned char   *dest, unsigned char   *src, unsigned int len) ;
void var_hex2asc(char *dest, unsigned int *src, unsigned int len);
unsigned int bcd2int(unsigned int val) ;
unsigned int bcd2int_2(unsigned char val) ;
void bcd2asc(unsigned char   *dest, unsigned char val);
void scrShowMsgInfo(char *msgname, unsigned char *msg, int msg_len, int orgmode);
unsigned char * packInt(unsigned char *dest, int num);
/*****************************************************************
����ԭ�ͣ�over_turn(unsigned char length,unsigned char *ptr)
����������������Ϊ�ַ����ߵ��ֽ�ת������
����������lengrth = �ַ������� 
		  *ptr    = ������ַ��� 
�������ƣ�	����/�����	����		����
-----------		-----------	------	   	-------
�� �� ֵ����
*****************************************************************/
void over_turn(unsigned char length,unsigned char *ptr);
#endif
