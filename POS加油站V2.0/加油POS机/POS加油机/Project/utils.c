
//utils.cpp

#include "main.h"
//////////////////////////////////////////////////////////////////////////
unsigned int bcd2int(unsigned int val)
{
	unsigned int     ret;
	unsigned char    upper, lower;

	upper = val >> 8;
	lower = val;
	ret = (((upper >> 4) & 0x0f)*1000) + ((upper & 0x0f)*100) +
		  (((lower >> 4) & 0x0f)*10)   + (lower & 0x0f);
	return ret;
}

unsigned int bcd2int_2(unsigned char val)
{
	unsigned int     ret;
	unsigned char    upper, lower;

	upper = (val >> 4)& 0x0f;
	lower = val & 0x0f;
	ret   = upper*10+lower;
	return ret;
}

//hw, 2007-01-22, step 2
void bcd2asc(unsigned char *dest, unsigned char val)
{
	unsigned char  tmp, i;

	for (i=0; i<2; i++) {
		if (i == 0) {
			tmp = (val & 0xf0) >> 4 ;
		} else {
			tmp = val & 0x0f;
		}
		if (tmp <= 0x09) {
			dest[i] = tmp + '0';
		} else {
			tmp -= 0x0A;
			dest[i] = tmp + 'A';
		}
	}
}

/*****************************************************************
函数原型：over_turn(unsigned char length,unsigned char *ptr)
功能描述：本函数为字符串高低字节转换功能
参数描述：lengrth = 字符串长度 
		  *ptr    = 输入的字符串 
参数名称：	输入/输出？	类型		描述
-----------		-----------	------	   	-------
返 回 值：无
*****************************************************************/
void over_turn(unsigned char length,unsigned char *ptr)
{
	unsigned char i;
	unsigned char temp[16];
	memcpy(temp,ptr,length);
	for(i=0;i<length;i++)
	{
		ptr[i]=temp[length-i-1];
	}
}
//hw, 2007-01-22, step 2
void var_bcd2asc(unsigned char   *dest, unsigned char   *src, unsigned int len) 
{
	unsigned int  i;

 	for (i=0; i<len; i++) {
		bcd2asc(&dest[(unsigned int)(i*2)], src[i]);
	}
}

//hw, 2007-01-22, step 2
void var_hex2asc(char *dest, unsigned int *src, unsigned int len)
{
	unsigned int  i;

	for ( i=0; i<len; i++ )
		sprintf(dest + i * 2, (void *)"%02X", src[i]);
	dest[len * 2] = '\0';
}

//hw, 2007-01-22, step 2
unsigned char asc2bcd(unsigned char   *src) 
{
	unsigned char  tmp, i, ret=0;

	for (i=0; i<2; i++) {
		if (src[i] >= '0' && src[i] <= '9') {
			tmp = src[i] - '0';
		} else {
			tmp = src[i] - 'A' + 0x0A;
		}

		if (i==0) {
			ret = tmp << 4;
		} else {
			ret |= tmp;
		}
	}

	return (ret);
}

void var_asc2bcd(unsigned char   *dest, unsigned char   *src, unsigned int len) 
{
	unsigned int  i;

	for (i=0; i<(len/2); i++) {
		dest[i] = asc2bcd(&src[(unsigned int)(i*2)]);
	}
}

//\x00\x00\x00\x64 -> 100   把HEX转换为整型数据
unsigned int unpackInt(unsigned char *msg)
{
	union 
	{
		int i;
		struct 
		{
			unsigned char ch1;
			unsigned char ch2;
			unsigned char ch3;
			unsigned char ch4;
		} 
		ch;
	} 
	ui;
	
	ui.ch.ch4=msg[0];
	ui.ch.ch3=msg[1];
	ui.ch.ch2=msg[2];
	ui.ch.ch1=msg[3];
	//典型的小端模式
	return ui.i;
}


//////////////////////////////////////////////////////////////////////////
//把整型数,如100转化为HEX;如100 -> 00 00 00 64 
unsigned char * packInt(unsigned char *dest, int num)
{  	
	union {
		int i;
		struct {
			unsigned char ch1;
			unsigned char ch2;
			unsigned char ch3;
			unsigned char ch4;
		} ch;
	} ui;
	
	ui.i = num;
	dest[0]=ui.ch.ch4;
	dest[1]=ui.ch.ch3;
	dest[2]=ui.ch.ch2;
	dest[3]=ui.ch.ch1;
	
	return dest;
}


//////////////////////////////////////////////////////////////////////////
//
//123456 -> 1234.56
char *fmtAmtStr(char *destbuf, char *srcbuf, unsigned char srclen)
{
	strcpy((char *)destbuf, "0.00");

	if (srclen==1) {
		destbuf[3]=srcbuf[0];
	}
	else if (srclen==2) {
		destbuf[2]=srcbuf[0];
		destbuf[3]=srcbuf[1];
	}
	else if (srclen>=3) {
		memcpy(destbuf, srcbuf, srclen-2);
		destbuf[srclen-2]='.';
		memcpy(destbuf+srclen-1, srcbuf+srclen-2, 2);
		//destbuf[srclen+2]=0;
	}
	
	return (destbuf);
}

//////////////////////////////////////////////////////////////////////////
//20080101 -> 2008/01/01
//当不足8位时,左对齐;即先填充年,然后再填充月,最后是日,空位填空格;
char *fmtDateStr(char *destbuf, char *srcbuf, unsigned char srclen)
{
	union{
		char buf[8];
		struct {
			char Y[4];
			char M[2];
			char D[2];
		} fmtbuf;
	}fmt;
		
	memset(fmt.buf, ' ', 8);
	memcpy(fmt.buf, srcbuf, srclen);
	memcpy(destbuf+0,fmt.fmtbuf.Y,4);
	destbuf[4]='/';
	memcpy(destbuf+5,fmt.fmtbuf.M,2);
	destbuf[7]='/';
	memcpy(destbuf+8,fmt.fmtbuf.D,2);
	destbuf[10]=0;

	return (destbuf);
}

//////////////////////////////////////////////////////////////////////////
char *fmtTimeStr(char *destbuf, char *srcbuf, unsigned char srclen)
{
	union{
		char buf[6];
		struct {
			char H[2];
			char M[2];
			char S[2];
		} fmtbuf;
	}fmt;
		
	memset(fmt.buf, ' ', 6);
	memcpy(fmt.buf, srcbuf, srclen);
	memcpy(destbuf+0,fmt.fmtbuf.H,2);
	destbuf[2]=':';
	memcpy(destbuf+3,fmt.fmtbuf.M,2);
	destbuf[5]=':';
	memcpy(destbuf+6,fmt.fmtbuf.S,2);
	destbuf[8]=0;

	return (destbuf);
}

/************************************************************************
* 名称：	fmtDTStr()                                                                      
* 功能：	转化无格式的14个ASCII字符串为日期时间格式的19个ASCII字符串;
			如: 20090102093001 -> 2009/01/02 09:30:01
* 参数：	
* 返回：	char *destbuf
************************************************************************/
//void *fmtDTStr(char *destbuf, char *srcbuf, unsigned char srclen)
//{
//    int  ilen1, ilen2;
//    char buf[21];
//
//    memset(buf, 0, sizeof(buf));
//    ilen1=8;
//    if (srclen<8) {
//        ilen1=srclen;
//    }
//    fmtDateStr(buf, srcbuf, ilen1);
//    strcat(buf, " ");
//
//    ilen2=6;
//    if (srclen<14) {
//        if (srclen>8) {
//            ilen2=srclen-8;
//        } else {
//            ilen2=0;
//        }
//    }
//    fmtTimeStr(buf+11, srcbuf+ilen1, ilen2);
//    strcpy(destbuf, buf);
//
//}


//////////////////////////////////////////////////////////////////////////

//
/////////////////////////////////////////////////////////////////////////////////////
////YYMMDDHHMM
////检查日期时间的值是否合法.
//unsigned char CheckDateTimeValidity(unsigned char *buf)
//{
//    //if (memcmp(buf,"00",2)==0) return FALSE;
//
//    //MM
//    if (buf[2]>'1') return (2);         //FALSE;月
//    if ((memcmp(&buf[2], "00", 2)==0) || ((memcmp(&buf[2], "12", 2)>0)))
//    {
//        return (3);                     //FALSE;
//    }
//
//    //DD
//    if (buf[4]>'3') return (4);         //FALSE;日
//    if ((memcmp(&buf[4], "00", 2)==0) || ((memcmp(&buf[4], "31", 2)>0)))
//    {
//        return (5);                     //FALSE;
//    }
//
//    //01-24
//    if (buf[6]>'3') return (6);         //FALSE;时
//    if (memcmp(&buf[6], "23", 2)>0) return (7);     //FALSE;
//
//    if (buf[8]>'6') return (8);         //FALSE;分
//    if (memcmp(&buf[8], "59", 2)>0) return (9);     //FALSE;秒
//
//    return TRUE;
//}


///////////////////////////////////////////////////////////////////////////////////
//YYMMDDHHMMSS 
//检查日期时间的值是否合法.
//合数则返回 ST_OK 
unsigned char CheckDateTimeValidity(unsigned char *buf1)
{
	unsigned char buf[32]; 
 
	memcpy(buf, buf1+2, 12);
	//if (memcmp(buf,"00",2)==0) return (2);
	
	//MM  01-12
	if (buf[2]>'1') return (2);			//FALSE;
	if ((memcmp(buf+2, "00", 2)==0) || ((memcmp(buf+2, "12", 2)>0)))
	{
		return (3);						//FALSE;
	}
	
	//DD  01-31
	if (buf[4]>'3') return (4);			//FALSE;
	if ((memcmp(buf+4, "00", 2)==0) || ((memcmp(buf+4, "31", 2)>0)))
	{
		return (5);						//FALSE;
	}

	//hour  01-24 
	if (buf[6]>'3') return (6);			//FALSE;
	if (memcmp(buf+6, "23", 2)>0) return (7);		//FALSE;

	//mm  分 01-60
	if (buf[8]>'6') return (8);			//FALSE;
	if (memcmp(buf+8, "59", 2)>0) return (9);		//FALSE;

	//ss 秒  01-60
	if (buf[10]>'6') return (8);		//FALSE;
	if (memcmp(buf+10, "59", 2)>0) return (9);		//FALSE;

	return ok;
}

void scrShowMsgInfo(char *msgname, unsigned char *msg, int msg_len, int orgmode)
{
	int  i=0;
	int  msgname_max;
	unsigned char buf[1024];
	unsigned char tmp[1024];

	memset(buf, 0, sizeof(buf));
	memset(tmp, 0, sizeof(tmp));
	
	memcpy(tmp, msg, msg_len%1024);
	sprintf((char *)buf, "[%10s] = ", (unsigned char *)msgname);
	msgname_max = 10;  //!
	
	switch(orgmode) {
	default:
	case ORG_NORMAL:
	case ORG_ASCII:
		sprintf((char *)buf+msgname_max+5, "%s", tmp); 
		break;
	case ORG_BCD:
	case ORG_HEX:
		i=0;
		for (i=0; i<msg_len; i++)
		{
			sprintf((char *)buf+msgname_max+5+i*3, "%02X ", (unsigned char)tmp[i]);
		}
		break; 
	}
	//scrShowLine((char *)buf);
	Debugprintf((char *)buf);
	Debugprintf("\n");
}

