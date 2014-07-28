/*****************************************************************
模块名称：EH0218函数区
功能概要：本模块主要完成个种应用层面函数编写
作    者：大连金德姆电子有限公司/于福勇
日    期：2006－06－13
修改历史：
日期		修改人		修改描述
------		---------	-------------
*****************************************************************/

#include  "main.h"



//系统密码
INT8U const   SuperPass[6] =    "889015";	//超级密码
INT8U         UserPass[6]  =    "000000";	//用户可以修改的密码


extern INT8U const    SELECT_USER_ADF2[7];
extern INT8U const    VERIFY_PIN[11];//验证口令
extern INT8U const    READ_15_FILE[5];//读取15文件内容
extern INT8U const    WRITE_15_FILE[5];//读取15文件内容

extern	DevHandle  hSam1Handle;
extern	DevHandle  hCpuHandle;

extern const char parmFileName[];
extern const char blackFileName[];
extern const char currecFileName[];
extern const char conrecFileName[];
extern const char hisrecFileName[];
extern const char grayrecFileName[];


// *****************************************************************
// 功能：		功能设置界面
// *****************************************************************
// const char  func_set[7*16 +2] =  {"1.日期时间设置  2.按键音设置    3.自动背光设置  4.自动关机设置  5.密码设置      6.系统初始化    7.更新程序      "};
// const char  func_set[6*16 +2] =  {"1.日期时间设置  2.按键音设置    3.自动背光设置  4.自动关机设置  5.密码设置      6.系统初始化    "};
//INT8U  func_set[6*20 +2] =  {"1.按键音设置    2.自动背光设置  3.自动关机设置  4.帮助            "};

// *****************************************************************
// 功能：		参数设置界面
// *****************************************************************
//INT8U  parm_set[13*16 +2] =  {"1.设置日期时间  2.设置线路号    3.设置车号      4.快捷消费金额  5.消费模式设定  6.老年卡扣款设定7.免费卡扣款设定8.免费卡票价设定9.员工卡扣款设定10.密码设置     11.刷卡持续时间 12.最大票价设置 13.系统初始化   "}; 
//INT8U  parm_set[9*20 +2] =  {"1.油品设置          2.密码设置          3.油品单价设置      4.恢复出厂设置      5.APN接入点选择     6.白名单下载        7.加油记录上传      8.设备ID设置        9.站点号设置        "};
INT8U  parm_set[8*20 +2] =  {"1.白名单下载        2.油品设置          3.加油记录上传      4.设备号设置        5.站点号设置        6.密码修改          7.重打上次小票      8.VPN接入点选择     "};
INT8U  gas_set[3*20 +2] =   {"1.天然气            2.柴油              3.汽油               "}; 

// *****************************************************************
// 功能：		信息查询界面
// *****************************************************************
INT8U  Info_Query[3*20 +2] ={"1.系统参数查询      2.记录总数查询      3.交易记录查询      "};

// *****************************************************************
// 功能：		工作模式
// *****************************************************************
// const char  Data_Trans[3*16 +2]={"1.设置与消费    2.余额查询      3.数据上传      "};
//INT8U  Data_Trans[1*20 +2]={"1.记录上传          "};//2.记录采集      "};

// *****************************************************************
// 功能：		证件类型选择
// *****************************************************************
// const char  Data_Trans[3*16 +2]={"1.设置与消费    2.余额查询      3.数据上传      "};
INT8U  Id_Type[6*20 +2]={"1.身份证            2.老年证            3.离休证            4.军官证            5.户口证            6.学生证            "};
//1:身份证
//2:老年证
//3:离休证
//4:军官证
//5:户口证
//6:学生证

char ReceiveChar;//接收到的数据

//volatile INT8U SndBuf[COM_BUFFER_SIZE];								//串口发送缓冲区
//volatile INT8U RcvBuf[COM_BUFFER_SIZE];								//串口接收缓冲区

//系统超级变量
extern DEV_STAT     DevStat;					//设备状态
extern CARD_INFO    CardInfo;					//卡片信息，交易记录由此变量生成
extern ERROR_CARD_INFO ErrCardInfo;	//出错卡状态



// *****************************************************************
// 功能：		OperPasswordCheck
// 说明:		操作员卡密码检验
//				本系统设有超级密码和用户密码
// 入口参数：	
// 出口参数：	Null
// 作者：
// 返回值： -1 关机 						
// *****************************************************************
int OperPasswordCheck()//INT8U *strPassWord
{
	INT8U i = 0;
	INT8U input[50];
//	INT8U buf[50];

	EA_vCls();
	//EA_vTextOut(0, 0, EM_key_FONT8X16, 0, 1, 1, "   操作员登录   ");
	//  sprintf((void *)buf, "编号:%s", DevStat.pos_number);

//  sprintf((void *)buf, "%s%s%s%s%s%s", DevStat.card_password[0],DevStat.card_password[1],DevStat.card_password[2],
//  		DevStat.card_password[3], DevStat.card_password[4],DevStat.card_password[5]);
//  lcddisp(1, 1, (void *)buf);
//  (void)EA_uiInkey(0);
	lcddisp(1, 1, (void *)"     操作员登录     ");
	lcddisp(2, 1, (void *)"输入密码:");
	strcpy((void *)input, "");
	EA_ucClrKeyBuf();
	for (;;)
	{
		i = EA_ucGetInputStr(2, 10, 20, EM_BIG_FONT | EM_MODE_NUMBER | EM_ALIGN_LEFT | EM_SHOW_PASSWORD
							 , 6, 6, 0, (void *)input);
		if ( i == EM_SUCCESS )
			break;
		if ( i == EM_ABOLISH )
			return -6;
	}

	if ( memcmp((void *)input, &DevStat.card_password[0],6) == 0 )
	{
		return ok;
	}
	else
	{
		EA_ucSetInverse(EM_lcd_INVON);
		lcddisp(4, 1, (void *)"      密码错误      ");
		EA_ucSetInverse(EM_lcd_INVOFF);
		//(void)EA_uiInkey(1);
		return notok;
	}
}


// *****************************************************************
// 功能：		OperPasswordModify
// 说明:		操作员卡密码修改
//				本系统设有超级密码和用户密码
// 入口参数：	
// 出口参数：	Null
// 作者：
// 返回值： -1 关机 						
// *****************************************************************
int OperPasswordModify()
{

	int 		ret = 1;
	INT16U		k=0;
	INT8U       input[50];
	INT8U  		retstr[200]; 
	INT8U 		rcv_buf[100], temp_data[20];
	uint		rcv_length;
	uchar       i;
	INT8U            	oldpass[7],newpass[7],surepass[7]; 

	memset(oldpass, 0x00, sizeof(oldpass));
	memset(newpass, 0x00, sizeof(newpass));
	memset(surepass, 0x00, sizeof(surepass));

//  //请输入密码
	EA_vCls();
	//EA_vTextOut(0, 0, EM_key_FONT8X16, 0, 1, 1, "    操作员卡密码修改    ");
	EA_vDisplay(1, "   操作员密码修改   ");
	EA_vDisplay(2, "输入原密码:         ");
	strcpy((void *)input, (void *)"");
	(void)EA_ucClrKeyBuf();
	for (;;)
	{
		i = EA_ucGetInputStr(2, 12, 20, EM_BIG_FONT | EM_MODE_NUMBER | EM_ALIGN_LEFT | EM_SHOW_PASSWORD
							 , 6, 6, 0, (void *)input);
		if ( i == EM_SUCCESS )
			break;
		if ( i == EM_ABOLISH )
			return notok;
	}
	if ( memcmp((void *)input, &DevStat.card_password[0],6) == 0 )
	{
		EA_vDisplay(3,"输入新密码:         ");
		(void)EA_ucClrKeyBuf();
		strcpy((void *)newpass, (void *)"");
		for (;;)
		{
			i = EA_ucGetInputStr(3, 12, 20, EM_BIG_FONT | EM_MODE_NUMBER | EM_ALIGN_LEFT | EM_SHOW_PASSWORD
								 , 6, 6, 0, (void *)newpass);
			if ( i == EM_SUCCESS )
				break;
			if ( i == EM_ABOLISH )
				return notok;
		}
		EA_vDisplay(4,"再输入一次:         ");
		(void)EA_ucClrKeyBuf();
		strcpy((void *)surepass, (void *)"");
		for (;;)
		{
			i = EA_ucGetInputStr(4, 12, 20, EM_BIG_FONT | EM_MODE_NUMBER | EM_ALIGN_LEFT | EM_SHOW_PASSWORD
								 , 6, 6, 0, (void *)surepass);
			if ( i == EM_SUCCESS )
				break;
			if ( i == EM_ABOLISH )
				return notok;
		}
		if( memcmp(newpass, surepass, 6) == 0 )
		{
			lcddisperr("    密码修改成功    ");
			strcpy((void *)DevStat.card_password, (void *)newpass);
			WriteParam();
			EA_uiInkeyMs(2000);
			return ok;
		}
		else
		{
			lcddisperr("    密码修改失败    ");
			EA_uiInkeyMs(2000);
			return notok;
		}
		//return ok;
	}
	else
	{
		lcddisperr("      密码错误      ");
		return notok;
	}

	Cpucard_Init();	
	//cpu卡复位
	ret = Cpucard_power_on(rcv_buf, &rcv_length);
	if(ret)
	{
		Cpucard_power_off();
		return -1;//无卡
	}	
	
	memset(rcv_buf,0,sizeof(rcv_buf));
	rcv_length = 0;
	
	//选择1002目录
	k = CmdPsam(hCpuHandle, (void *)SELECT_USER_ADF2, sizeof(SELECT_USER_ADF2) ,rcv_buf, &rcv_length);
	if ( k != ok )
	{
		EA_vCls();
		lcddisperr("    选择目录错误    ");
		EA_uiInkeyMs(2000);
		Cpucard_power_off();
		return -2;
	}

	//验证口令
	k = CmdPsam(hCpuHandle, (void *)VERIFY_PIN, sizeof(VERIFY_PIN) ,rcv_buf, &rcv_length);
	if ( k != ok )
	{
		EA_vCls();
		EA_vDisplay(1,"    验证口令错误    ");
		EA_vDisplay(2,"    非本系统卡片    ");
		EA_vDisplay(3,"   按下任意键退出   ");
		EA_uiInkeyMs(2000);
		Cpucard_power_off();
		return -3;
	}


	//读取15文件内容－－判断是什么卡片类型
	//
	k = CmdPsam(hCpuHandle, (void *)READ_15_FILE, sizeof(READ_15_FILE) ,rcv_buf, &rcv_length);
	if ( k != ok )
	{
		EA_vCls();
		EA_vDisplay(1,"  读取信息文件错误  ");
		EA_vDisplay(2,"   按下任意键退出   ");
		Cpucard_power_off();
		return -3;
	}

	memcpy(temp_data, rcv_buf, 0x24);

	for(k=0;k<3;k++)
	{
		retstr[k] = ((a_to_h(newpass[2*k]))<<4)|((a_to_h(newpass[2*k+1]))&0x0f);
	}	

	memcpy(temp_data+11, retstr, 3);

	for ( k=0; k<8; k++ )			  //如果计算不成功，则最多循环计算8次
	{
		////////////////////////////////////////
		//更改卡片使用与否标识
		////////////////////////////////////////
		memcpy(retstr, WRITE_15_FILE, 5);		

		memcpy(&retstr[5], temp_data, 36);
		k = CmdPsam(hCpuHandle, retstr, 41, rcv_buf, &rcv_length);		//TAC计算初始化
		if ( k != ok )
			continue;
	}
	Cpucard_power_off();
	printf_debug((void *)"密码更改成功!");
	return ok;
}


// *****************************************************************
// 功能：		PasswordCheck
// 说明:		密码检验
//				本系统设有超级密码和用户密码
// 入口参数：	
// 出口参数：	Null
// 作者：
// 返回值： -1 关机 						
// *****************************************************************
int PasswordCheck()
{
	uchar i = 0;
	INT8U input[50];

	EA_vCls();
	lcddisp(2, 1, (void *)"输入密码:");
	lcddisp(4, 1, (void *)"请按<确认>键继续");
	strcpy((void *)input, "");

	for ( ;; )
	{
		i = EA_ucGetInputStr(2, 10, 20, EM_BIG_FONT | EM_MODE_NUMBER | EM_ALIGN_LEFT | EM_SHOW_PASSWORD
							 , 6, 6, 0, (void *)input);
		if ( i == EM_SUCCESS )
			break;
		if ( i == EM_ABOLISH )
			return 2;
	}
	if ( memcmp((void *)input, &DevStat.user_passwd[0],6) == 0 )
	{
		return ok;
	}
	else
	{
		EA_ucSetInverse(EM_lcd_INVON);
		lcddisp(4, 1, (void *)"      密码错误      ");
		EA_ucSetInverse(EM_lcd_INVOFF);
		(void)EA_uiInkey(1);
		return notok;
	}

}

// *****************************************************************
// 功能：		PasswordSetting
// 说明:		用户密码设置
//				本系统设有超级密码和用户密码
// 入口参数：	
// 出口参数：	Null
// 作者：
// 返回值： -1 关机 						
// *****************************************************************
int PasswordSetting()
{
//	int             ret=0;
	char            oldpass[7],newpass[7],surepass[7]; 
	uchar i = 0;
	INT8U input[50];

	memset(oldpass, 0x00, sizeof(oldpass));
	memset(newpass, 0x00, sizeof(newpass));
	memset(surepass, 0x00, sizeof(surepass));

	//请输入密码
	EA_vCls();
//  EA_vTextOut(0, 0, EM_key_FONT8X16, 0, 1, 1, "     用户密码设置     ");
	EA_vDisplay(1, "    用户密码设置    ");
	EA_vDisplay(2, "输入原密码:         ");
	(void)EA_ucClrKeyBuf();
	strcpy((void *)input, "");
	for (;;)
	{
		i = EA_ucGetInputStr(2, 12, 20, EM_BIG_FONT | EM_MODE_NUMBER | EM_ALIGN_LEFT | EM_SHOW_PASSWORD
							 , 6, 6, 0, (void *)input);
		if ( i == EM_SUCCESS )
			break;
		if ( i == EM_ABOLISH )
			return notok;
	}
	if ( memcmp((void *)input, &DevStat.user_passwd[0],6) == 0 )
	{
		EA_vDisplay(3,"输入新密码:         ");
		(void)EA_ucClrKeyBuf();
		strcpy((void *)newpass, "");
		for (;;)
		{
			i = EA_ucGetInputStr(3, 12, 20, EM_BIG_FONT | EM_MODE_NUMBER | EM_ALIGN_LEFT | EM_SHOW_PASSWORD
								 , 6, 6, 0, (void *)newpass);
			if ( i == EM_SUCCESS )
				break;
			if ( i == EM_ABOLISH )
				return notok;
		}
		EA_vDisplay(4,"再输入一次:         ");
		(void)EA_ucClrKeyBuf();
		strcpy((void *)surepass, "");
		for (;;)
		{
			i = EA_ucGetInputStr(4, 12, 20, EM_BIG_FONT | EM_MODE_NUMBER | EM_ALIGN_LEFT | EM_SHOW_PASSWORD
								 , 6, 6, 0, (void *)surepass);
			if ( i == EM_SUCCESS )
				break;
			if ( i == EM_ABOLISH )
				return notok;
			if(EA_uiInkeyMs(10) == EM_key_CANCEL)
				return notok;
		}
		if( memcmp(newpass, surepass, 6) == 0 )
		{
			lcddisperr("    密码修改成功    ");
			strcpy((void *)DevStat.user_passwd, (void *)newpass);
			WriteParam();
//  		EA_uiInkeyMs(500);
			return ok;
		}
		else
		{
			lcddisperr("    密码修改失败    ");
//  		EA_uiInkeyMs(500);
			return notok;
		}
		return ok;
	}
	return notok;
}

// *****************************************************************
// 功能：		Func_Setting
// 说明:		功能设置菜单
//              此菜单需要密码保护;本系统设有超级密码和用户密码
// 入口参数：	
// 出口参数：	Null
// 作者：
// 返回值：	-1 关机							
// *****************************************************************
//int Func_Setting()
//{
//    uchar ret;
//    for ( ; ; )
//    {
//        ret = browse_menu(0, (void *)"", func_set, TRUE );
//        switch ( ret )
//        {
//            case 0://按键音设置
////  			ret = KeySound_Setting();
//                break;
//
//            case 1://自动背光设置
////  			ret = BackLight_Setting();
//                break;
//
//            case 2://自动休眠设置
////  			ret = PowerOff_Setting();
//                break;
//
//            case 3://帮助
//                EA_ucClrKeyBuf();
//                EA_vDisp(1, 1, "欢迎使用本设备");
//                EA_vDisp(2, 1, "大连易达通电子");
//                EA_vDisp(3, 1, "电话:         ");
//                EA_vDisp(4, 1, "              ");
//                (void)EA_uiInkey(5);
//                break;
//            case 4:
//                return -1;
//
//            case 5:
//                return -5;
//
//            default:
//                break;
//        }
//    }
//}
//
// *****************************************************************
// 功能：		Parm_Setting
// 说明:		参数设置菜单
// 入口参数：	
// 出口参数：	Null
// 作者：
// 返回值：	-1 关机						
// *****************************************************************
int Parm_Setting(void)
{
	int  ret;

	EA_ucClrKeyBuf();

	switch (PasswordCheck())
	{
		case notok:
			printf_debug((void *)"密码错误");
			return 1;
		case 2:
			return 1;
		case ok:
			EA_ucClrKeyBuf();
			break;
		default:
			return 1;
	}

	while(1)
	{

		ret = browse_menu(0, (void *)"", parm_set, TRUE );
		switch ( ret )
		{
		   case 0://白名单下载
				download_whiltelist();				
				break;	
		   case 1://油品设置
 			    select_gas_oil_Setting();
				break;
		   case 2://记录上传
			    Gprs_Upload_data();
				//GetTradeAmt();
				break;
		   case 3://设置设备ID号
				SetID();
				break;
		   case 4:  //设置站点号
				SetUserID(); 
				break;
		   case 5:  //密码修改
				PasswordSetting();
				break;
		   case 6:  //系统初始化 
			    Reprint_sale_record();
			    break;
		   case 7://APN设置
			   if(SAMCompSelect() == ok)
			   {
				   EA_vCls();
				   EA_vDisplay(3, "  APN接入点修改成功 ");
				   EA_vDisplay(4, "   系统将自动重启   ");
				   EA_uiInkeyMs(2000);
				   System_reset();
			   }
			   break;
//  	   case 7:
//  		   //系统初始化
//  			InitParam();
//  			break;
//  		case 8:
//   			DateTime_Setting();
//  			break;
			case -1:
//  			Power_Down();			//	关机
				return -1;

			case -5://返回上级菜单
				return -5;

			default:
				break;
		}
	}
}

// *****************************************************************
// 功能：		void Reprint_sale_record( void )
// 说明:		重新打印上次交易小票
// 入口参数：	无
// 出口参数：	Null
// 作者：  frank
// 返回值：无						
// *****************************************************************
void Reprint_sale_record( void )
{
	print_sale_record();
}
// *****************************************************************
// 功能：		SetID
// 说明:		手工输入ID号
// 入口参数：	
// 出口参数：	Null
// 作者：
// 返回值：	-1 关机							
// *****************************************************************
unsigned char SetID(void)
{
	char    buf[50];//输入字符存放数组
	int     i;   
//  char    str[50];//输入字符存放数组

//  sprintf((void *)str, "设备ID:%02x%02x%02x", DevStat.equ_id[0], DevStat.equ_id[1], DevStat.equ_id[2]);

	EA_vCls();
//  EA_vDisplay(1, (void *)str);
	EA_vDisplay(1, (void *)"设备ID:%02x%02x%02x",DevStat.equ_id[0], DevStat.equ_id[1], DevStat.equ_id[2]);
	EA_vDisplay(2, (void *)"输入设备ID:");
	EA_vDisplay(3, (void *)"请按<确认>键确认");
	strcpy((void *)buf, "");
	for ( ;; )
	{
		i = EA_ucGetInputStr(2, 12, 20, EM_BIG_FONT | EM_MODE_NUMBER | EM_ALIGN_LEFT | EM_SHOW_ORIGINALLY
							 , 6, 6, 0, (void *)buf);
		if ( i == EM_SUCCESS )
			break;
		if ( i == EM_ABOLISH )
			return notok;
	}
	
	DevStat.equ_id[0] = (ascii_to_hex(buf[0]) << 4) | ascii_to_hex(buf[1]);
	DevStat.equ_id[1] = (ascii_to_hex(buf[2]) << 4) | ascii_to_hex(buf[3]);
	DevStat.equ_id[2] = (ascii_to_hex(buf[4]) << 4) | ascii_to_hex(buf[5]);

	WriteParam();

	printf_debug((void *)"设备ID设置成功");

	return ok;
}

// *****************************************************************
// 功能：		SetUserID
// 说明:		手工输入商号
// 入口参数：	
// 出口参数：	Null
// 作者：
// 返回值：	-1 关机							
// *****************************************************************
unsigned char SetUserID(void)
{
	char    buf[50];//输入字符存放数组
	int     i;      
	char    str[50];//输入字符存放数组 

	sprintf((void *)str, "站点号:%02x%02x", DevStat.acnt_id[0], DevStat.acnt_id[1]);

	EA_vCls();
	EA_vDisplay(1, (void *)str);
	EA_vDisplay(2, (void *)"输入站点号:");
	EA_vDisplay(3, (void *)"请按<确认>键确认");
	strcpy((void *)buf, "");

	for ( ;; )
	{
		i = EA_ucGetInputStr(2, 12, 20, EM_BIG_FONT | EM_MODE_NUMBER | EM_ALIGN_LEFT | EM_SHOW_ORIGINALLY
							 , 4, 4, 0, (void *)buf);
		if ( i == EM_SUCCESS )
			break;
		if ( i == EM_ABOLISH )
			return notok;
	}
	
	DevStat.acnt_id[0] = (ascii_to_hex(buf[0]) << 4) | ascii_to_hex(buf[1]);
	DevStat.acnt_id[1] = (ascii_to_hex(buf[2]) << 4) | ascii_to_hex(buf[3]);

	WriteParam();

	printf_debug((void *)"站点号设置成功");

	return ok;
}
///******************************************************************************
// 函数名称：GetTradeAmt
// 功能描述：获得交易金额
// 参数描述：
// 参数名称：输入/输出？  类型        描述
//
// 返  回  值：    ok(0)-成功
//                 notok(1)-失败
//
// 作      者 ：于福勇
// 日      期：2007-01-30
// 修改历史：
//        日期        修改人      修改描述
//******************************************************************************/
//INT8U GetTradeAmt(void)
//{
//    INT8U   ret=0,flag=FALSE;
//    INT32U  LOCAL_V_TYPE  temp_int32u = 0;
//    INT8U   LOCAL_V_TYPE  temp_buf[50];
//    INT8U                 strbuf[50];
//    int                   key;
//////////////////////////////////////////
////输入充值金额
//////////////////////////////////////////
//
////  init_console(CONSOLE_CHINESE);
//
//    EA_vCls();
//
//    flag = FALSE;
//    EA_vDisplay(1, "请输入交易金额");
//    EA_vDisplay(2, "单位(分)");
//
//    temp_int32u = get_numeric(4,4,4,0);
//    if(temp_int32u<= 0)
//        temp_int32u = 0;
//
//    while (!flag)
//    {
//        //if(temp_int32u > DevStat.max_price_limit || temp_int32u==0 || temp_int32u<3000)//credit amt must >30 and <max_price_limit
//        if(temp_int32u>10000)//credit amt must >30 and <max_price_limit
//        {
//            //clr_scr();
//            //clr_scr();
//            DispStr_CE(0, 6, "交易金额输入无效", DISP_POSITION|DISP_CLRLINE);
//            Beep(1000);
//        }
//        else
//        {
//        //  clr_scr();
//        //  Clean_KEY_FIFO();
//            flag = TRUE;
//            DevStat.fare = temp_int32u;
//            WriteParam();
//            break;
//        //  sprintf(strbuf, "金额:%6.2f元", CardInfo.fare/100);
//        //  if(select_affirm(4, strbuf)==0)
//        //      break;
//
//        }
//
////判断有无退出键按下
//        ret = KEY_read();
//        if(ret ==EXIT_KEY_COMM)
//        {
//            Beep(2000);
//            delay_n_ms(200);
//            init_console(CONSOLE_CHINESE);
//            KEY_init(KEY_BEEP_KEY|KEY_AUTO_EL_ON|KEY_FIFO_MODE);//开启按键声音和背光
//            (void)sm_setting(MSK_KEY_UP|MSK_KEY_DOWN); //禁止键按下或抬起来时做响应
//            KEY_beep_mask_set(0xFFFfffff);
//            KEY_EL_mask_set(0xFFFfffff);
//            Clean_KEY_FIFO();
//            return notok;
//        }
//        if(ret == EXIT_KEY_POWER)
//        {
//            Halt_HD6305();
//        }
////		else
////		{
////			flag = FALSE;
////		}
//
////重新输入
////		clr_scr();
//        DispStr_CE(0, 6, "", DISP_POSITION|DISP_CLRLINE);
//        DispStr_CE(0, 0, "请输入交易金额", DISP_POSITION|DISP_CLRLINE);
//        temp_int32u = 0;
//        temp_int32u = get_numeric(4,4,4,0);
//        if(temp_int32u < 0)
//            temp_int32u = 0;
//
//    }
//
//    init_console(CONSOLE_CHINESE);
//    KEY_init(KEY_BEEP_KEY|KEY_AUTO_EL_ON|KEY_FIFO_MODE);//开启按键声音和背光
//    (void)sm_setting(MSK_KEY_UP|MSK_KEY_DOWN); //禁止键按下或抬起来时做响应
//    KEY_beep_mask_set(0xFFFfffff);
//    KEY_EL_mask_set(0xFFFfffff);
//    Clean_KEY_FIFO();
//
//    return ok;
//}
////////////////////////////////////////////////////////////
//APN接入点选择
////////////////////////////////////////////////////////////
INT8U SAMCompSelect()
{
	int ret;
	INT8U APN_menu[2*20 + 2] = {"1.移动公网          2.移动专网          "};
//	DevStat.samcompcode=1;
	ret = browse_menu(0, (unsigned char*)"", APN_menu, TRUE);
	switch ( ret )
	{
		case 0:	// 移动公网
			DevStat.samcompcode = 0;
			WriteParam();
			break;

		case 1:	// 移动专网
			DevStat.samcompcode = 1;
			WriteParam();
			break;

		default: 
			return notok;
		//	break;
	}
	return ok;
}
// *****************************************************************
// 功能：		Information_Query
// 说明:		参数设置菜单
// 入口参数：	
// 出口参数：	Null
// 作者：
// 返回值：	-1 关机							
// *****************************************************************
int Information_Query()
{

	int             ret;

	for ( ; ; )
	{
		ret = browse_menu(0, (void *)"", Info_Query, TRUE );
		switch ( ret )
		{
			case 0://参数查询
				ret = Parm_Query();
// 				return ret;
				break;

			case 1://记录总数查询
				ret = Record_Query();
// 				return ret;
				break;

			case 2://卡片查询
				ret = Card_Query();
// 				return ret;
				break;

			case -1://EXIT_KEY_POWER,EXIT_AUTO_QUIT
				//Halt_EH0218(POWER_DELAY);			//	关机
				return -1;
				break;

			case -5://返回上级菜单
				return -5;
				break;

			default:
				break;
		}
	}
}

// // *****************************************************************
// // 功能：		CardConsumeProcess
// // 说明:		卡片定额消费
// // 入口参数：
// // 出口参数：	Null
// // 作者：
// // 返回值：
// // *****************************************************************
// int CardConsumeProcess()
// {
// 	int     ret = 0;
// 	char    strbuf[20], str[20];
// 	float   fare = 0;
//
//
// 	EA_ucClrKeyBuf();
//
// 	CardInfo.fare = 100;//定额消费
//
// 	RCX_Init(TYPE_A_ISO14443);
//
// 	//提示请刷卡
// 	DispStr_CE(0, 3, "请在10秒内刷卡", DISP_CENTER);
//
// 	SPT_set(640);
//
// 	while ( SPT_read() )
// 	{
// 		ret = SPT_read()%64;
// 		if ( ret == 0 )
// 		{
// 			EA_ucClrKeyBuf();
// 			sprintf(strbuf, "请在%d秒内刷卡", SPT_read()/64);
// 			DispStr_CE(0, 3, strbuf, DISP_CENTER);
// 		}
// 		ret = ICCardProcess();
// 		if ( ret == 0 )
// 			break;
// 	}
//
// 	RCX_Close();
//
// 	ICC_deactivate_current_sock();
// 	ICC_interface_power(ICC_POWER_OFF);
// 	return 0;
// }

// *****************************************************************
// 功能：		Datetime_Setting
// 说明:		日期时间设置
// 入口参数：	
// 出口参数：	Null
// 作者：
// 返回值：							
// *****************************************************************
int DateTime_Setting(void)
{
//  modify_date();
	TimeModify();
	EA_uiInkeyMs(100);
	EA_ucClrKeyBuf();
	return 1;
}
// *****************************************************************
// 功能：		select_gas_oil_Setting(void)
// 说明:		油品选择  -- 汽油或者是气
// 入口参数：	
// 出口参数：	Null
// 作者：		刘及华
// 返回值：	    OK or NOTOK					
// *****************************************************************
int select_gas_oil_Setting(void)
{
	int uiRet;
	uiRet = browse_menu(0, (void *)"", gas_set, TRUE );
	switch ( uiRet )
	{
	   case 0:
		   DevStat.fill_mode = 0;
		   break;
	   case 1:
		   DevStat.fill_mode = 1;
		   break;
	   case 2:
		   DevStat.fill_mode = 2;
		   break;
	   default:
		   break;
	}
	WriteParam();
	return ok;
}

//// *****************************************************************
//// 功能：		Parm_Query
//// 说明:		参数查询
//// 入口参数：
//// 出口参数：	Null
//// 作者：
//// 返回值：
//// *****************************************************************
//int Parm_Query()
//{
//    char buf[50];
////	char temp_buf[4];
//    FP64            balance = 0.0;
//    EA_vCls();
//    strcpy((void *)buf, "设备编号: ");
////  temp_buf[0] = DevStat.equ_id[2];
////  temp_buf[1] = DevStat.equ_id[1];
////  temp_buf[2] = DevStat.equ_id[0];
//    htoa(&buf[10], DevStat.equ_id, sizeof(DevStat.equ_id)); //by liujihua
////  htoa(&buf[8], temp_buf, 3);
//
//    EA_vDisp(1,1,buf);
//
//    strcpy(buf, "PSAM卡号: ");
//    htoa(&buf[10], &(DevStat.pos_number[0]), 4);   //by liujihua
////  htoa(&buf[8], &DevStat.pos_number[3], 1);
////  htoa(&buf[10], &DevStat.pos_number[2], 1);
////  htoa(&buf[12], &DevStat.pos_number[1], 1);
////  htoa(&buf[14], &DevStat.pos_number[0], 1);
//
//    EA_vDisp(2,1,buf);
//
//    strcpy(buf, "操作员号: ");
//
//    htoa(&buf[10], &DevStat.cur_driver_number[1], 3);  //by liujihua
////  buf[8] = DevStat.cur_driver_number[2];
////  buf[9] = DevStat.cur_driver_number[1];
////  buf[10] = DevStat.cur_driver_number[0];
//
//
//
//    EA_vDisp(3,1,buf);
//    balance = (FP64)(DevStat.oper_card_amt / 100);
//    sprintf((void *)buf, "卡内余额: %7.2f元", balance);
//    EA_vDisp(4,1,buf);
//    EA_uiInkey(10);
//    return ok;
//}
//
// *****************************************************************
// 功能：		Record_Query
// 说明:		记录查询
// 入口参数：	
// 出口参数：	Null
// 作者：
// 返回值：							
// *****************************************************************
int Record_Query()
{
	INT16U key;
	INT16U Num_rec;
	INT16U Num_black;
	INT16U Num_gray;
	uint   uiRetNum;
	uchar  ucOpenID = 0;
	EA_vCls();
	EA_ucClrKeyBuf();
//////////////////////////////////////////////////////
	if ( EA_ucPFOpen((uchar *)currecFileName, &ucOpenID) != EM_ffs_SUCCESS )
	{
		lcddisperr("打开记录文件失败");
		(void)EA_ucPFClose(ucOpenID);
		return notok;
	}
	if ( EA_ucPFInfo(ucOpenID, EM_ffs_VALIDREC, &uiRetNum) != EM_ffs_SUCCESS )
	{
		lcddisperr("读取记录文件失败");
		(void)EA_ucPFClose(ucOpenID);
		return notok;
	}
	Num_rec = uiRetNum;
	(void)EA_ucPFClose(ucOpenID);
//////////////////////////////////////////////////////////
	if ( EA_ucPFOpen((uchar *)blackFileName, &ucOpenID) != EM_ffs_SUCCESS )
	{
		lcddisperr("打开记录文件失败");
		(void)EA_ucPFClose(ucOpenID);
		return notok;
	}
	if ( EA_ucPFInfo(ucOpenID, EM_ffs_VALIDREC, &uiRetNum) != EM_ffs_SUCCESS )
	{
		lcddisperr("读取记录文件失败");
		(void)EA_ucPFClose(ucOpenID);
		return notok;
	}
	Num_black = uiRetNum;
	(void)EA_ucPFClose(ucOpenID);
////////////////////////////////////////////////////////
	if ( EA_ucPFOpen((uchar *)conrecFileName, &ucOpenID) != EM_ffs_SUCCESS )
	{
		lcddisperr("打开记录文件失败");
		(void)EA_ucPFClose(ucOpenID);
		return notok;
	}
	if ( EA_ucPFInfo(ucOpenID, EM_ffs_VALIDREC, &uiRetNum) != EM_ffs_SUCCESS )
	{
		lcddisperr("读取记录文件失败");
		(void)EA_ucPFClose(ucOpenID);
		return notok;
	}
	Num_gray = uiRetNum;
	(void)EA_ucPFClose(ucOpenID);
//////////////////////////////////////////////////////////

	EA_vDisplay(1, "系统存储记录信息查询");
	EA_vDisplay(2, "交易记录: %6d   条", Num_rec);
	EA_vDisplay(3, "黑 名 单: %6d   条", Num_black);
	EA_vDisplay(4, "消费记录: %6d   条", Num_gray);
	key = EA_uiInkey(0);
	if( key == EM_key_CANCEL)
	{
		return ok;
	}
	return ok;
}
//
//// *****************************************************************
//// 功能：		AnalyAndDisplay
//// 说明:		查询记录表，并实现交易记录
//// 入口参数：
//// 出口参数：	Null
//// 作者：
//// 返回值：
//// *****************************************************************
//void AnalyAndDisplay(RECORD * strTradeInfo)
//{
//    char  strbuf[50];
//    float   fBuf = 0;
//    INT8U   LOCAL_V_TYPE  temp_buf[100];
//    RECORD * prec = (RECORD *)strTradeInfo;
//
//    EA_ucClrKeyBuf();
//
//    //显示卡片信息
//    EA_vCls();
//    htoa(temp_buf, &prec->card_serial_number[4], 4);
//    strcpy((void *)strbuf, (void *)"卡号:   ");
//    strcat((void *)strbuf, (void *)temp_buf);
//    EA_vDisp(1,1,strbuf);
///*
//    if (prec->card_purchase_type == 0x18)           //是否为司机卡上班追加记录
//    {
//        GetCardType(0xF0, temp);        //检索卡片类型信息
//    }
//    else
//    {
//        GetCardType(prec->card_type, temp);     //检索卡片类型信息
//    }
//    sprintf(strbuf, "类型: %s", temp);
//    DispStr_CE(0,2, strbuf, DISP_POSITION);
//*/
//
//    switch ( prec->card_purchase_type)
//    {
//        case TT_NORMAL_CONSUME_MONEY:
//            sprintf((void *)strbuf, "模式:冲帐   02%d02%d02%d", prec->card_in_time.hour,prec->card_in_time.minute,prec->card_in_time.second);//检索卡片类型信息
//            EA_vDisp(2,1, strbuf);
//
//            fBuf    = (float)(prec->fare[0] * 65536 + prec->fare[1]*256 + prec->fare[2])/100;
//            sprintf((void *)strbuf, "冲帐:     %5.2f元   ", fBuf);
//            EA_vDisp(3,1, strbuf);
//            fBuf    = (float)(prec->balance[0]*65536 + prec->balance[1]*256 + prec->balance[2])/100;
//            sprintf((void *)strbuf, "余额:     %5.2f元   ", fBuf);
//            EA_vDisp(4,1, strbuf);
//            break;
//        case TT_CARD_SALE:
//            //EA_vDisp(2, 1,          "模式:           售卡");
//            sprintf((void *)strbuf, "模式:售卡   02%d02%d02%d", prec->card_in_time.hour,prec->card_in_time.minute,prec->card_in_time.second);//检索卡片类型信息
//            EA_vDisp(2,1, strbuf);
//            fBuf    = (float)(prec->deposit_amt[0]*256 + prec->deposit_amt[1])/100;
//            sprintf((void *)strbuf, "押金:     %5.2f元   ", fBuf);
//            EA_vDisp(3,1, strbuf);
//            break;
//        case TT_CREDIT_MONEY:
//            //EA_vDisp(2,1,           "模式:           充值");
//            sprintf((void *)strbuf, "模式:充值   02%d02%d02%d", prec->card_in_time.hour,prec->card_in_time.minute,prec->card_in_time.second);//检索卡片类型信息
//            EA_vDisp(2,1, strbuf);
//            fBuf    = (float)(prec->fare[0]*65536 + prec->fare[1]*256 + prec->fare[2])/100;
//            sprintf((void *)strbuf, "充值:     %5.2f元   ", fBuf);
//            EA_vDisp(3,1, strbuf);
//            fBuf    = (float)(prec->balance[0]*65536 + prec->balance[1]*256 + prec->balance[2])/100;
//            sprintf((void *)strbuf, "余额:     %5.2f元   ", fBuf);
//            EA_vDisp(4,1, strbuf);
//            break;
//        default:
//            EA_vDisp(2,1,           "模式:           其他");
//            EA_vDisp(4,1,           "    按任意键退出    ");
//            break;
//
//    }
//}


// *****************************************************************
// 功能：		Card_Query
// 说明:		卡片信息查询
// 入口参数：	
// 出口参数：	Null
// 作者：
// 返回值：							
// *****************************************************************
int Card_Query()
{
	int         count;
	long        lKeyRet = 255;  
	uint		uiRetCnd;
	uchar       ucOpenID = 0;
//	uchar   	ucRet;
	uint		uiReadCnt;
	uchar 		i = 0;	
	INT8U 		p[60];

	RECORD *prec = (RECORD *)p;
	count = 0 ;//最后一条

	(void)EA_ucClrKeyBuf();
	i = EA_ucPFOpen( (uchar *)currecFileName, &ucOpenID);
	if ( i != EM_ffs_SUCCESS )
	{
		(void)EA_ucPFClose(ucOpenID);
		lcddisperr("打开记录文件失败");
		return notok;
	}
	// 获取有效信息条数
	(void)EA_ucPFInfo(ucOpenID, EM_ffs_VALIDREC, &uiReadCnt);	
	if ( uiReadCnt == 0 )//记录为空，直接添加新记录即可
	{
		printf_debug((void *)"记录为空");
		(void)EA_ucPFClose(ucOpenID);
		return notok;
	}
	count = uiReadCnt;
	i = EA_ucPFReadRec(ucOpenID, uiReadCnt, 1, &uiRetCnd, p);
	if ( i != EM_ffs_SUCCESS )
	{
		(void)EA_ucPFClose(ucOpenID);
		lcddisperr("读记录文件失败");
		return notok;
	}
	(void)AnalyAndDisplay(prec);
	(void)EA_ucPFClose(ucOpenID);
	for ( ;; )
	{
		lKeyRet = EA_uiInkey(0);
		if ( (lKeyRet == EM_key_CANCEL) || (lKeyRet == EM_key_CLEAR) )			//退出
		{
			return 0;
		}
		switch ( lKeyRet )
		{
			case EM_key_INTERPUNCTION:		//	up
				count --;
				if ( count <= 0 )//记录超限
				{
					(void)EA_ucClrKeyBuf();
					EA_vCls();
					count = 0;
					EA_vDisp(4, 1, "     没有更早的记录     ");
					EA_uiInkey(1);
					return 0;
				}
				break;

			case EM_key_LETTER:		//	down
				count ++;
				if ( count > uiReadCnt )
				{
					(void)EA_ucClrKeyBuf();
					EA_vCls();
					count = 0;
					EA_vDisp(4, 1, "    最后一条记录    ");
					EA_uiInkey(1);
					return 0;
				}
				break;

			default:
				break;
		}//end switch
		i = EA_ucPFOpen( (uchar *)currecFileName, &ucOpenID);
		if ( i != EM_ffs_SUCCESS )
		{
			(void)EA_ucPFClose(ucOpenID);
			lcddisperr("打开记录文件失败");
			return notok;
		}
		i = EA_ucPFReadRec(ucOpenID, count, 1, &uiRetCnd, p);
		if ( i != EM_ffs_SUCCESS )
		{
			(void)EA_ucPFClose(ucOpenID);
			lcddisperr("读记录文件失败");
			return notok;
		}
		AnalyAndDisplay(prec);
		(void)EA_ucPFClose(ucOpenID);
	}//end while

}

// *****************************************************************
// 功能：		通讯处理
// 格式:        @START + COMMAND + LENGTH + DATA + CRC + END
// 说明:		完成数据采集、黑名单下载、时间更新等操作
// 				如果出现错误，错误返回为@START + E + REASON
// 入口参数：	
// 出口参数：	Null
// 作者：		
// 返回值：		-1:关机					
// *****************************************************************
//int CommunicationDeal()
//{
//
//    typ_msg_word        msg;
//    typ_UART_stat_word  Usw;
//    char                length;//接收数据长度
//
//    INT32U rec_num;
//    char        flag;
//
//    RcvdLength = 0;
//    //ClrUART();//清空串口缓冲区
//
//    while ( 1 )
//    {
//        //SetAutoPowerOffTime(9999);            //通讯禁止休眠
//        msg.s_word = sys_msg(SM_STAY_AWAKE);
//        if ( msg.bits.comm_data )
//        {
//            ReceiveChar = (char)UART_get_char();
//
//#ifdef DEBUG
//            TRACE(chtostr(ReceiveChar));
//#endif
//
//            if ( RcvdLength == 0 && !(ReceiveChar == 0x20))// || ReceiveChar == '@') )
//            {
//                continue;               //开头必须为0x02或'@'
//            }
//            else
//            {
//            }
//
//            RcvBuf[RcvdLength] = ReceiveChar;
//            RcvdLength++;
//
//            if ( RcvdLength >= COM_BUFFER_SIZE )
//            {
//                RcvdLength = 0;             //长度超了
//                return LENGTH_ERR;
//            }
//            if((RcvdLength==NeedRcvLength) && (NeedRcvLength!=0))
//            {
//                return ok;
//            }
//
//            if( (NeedRcvLength==0) && (RcvdLength == RcvBuf[2] + 4 ))
//            {
//                return ok;
//            }
//
//        }
//
//        if ( msg.bits.key_available )
//        {
//            switch ( KEY_read() )
//            {
//                case EXIT_KEY_COMM:
//                case EXIT_KEY_F1:
//                    CloseCom();
//                    //SetAutoPowerOffTime(DevStat.poweroff_time);       //恢复休眠
//                    return OPER_CANCLE;
//
//                case EXIT_KEY_POWER:
//                    CloseCom();
//                    //SetAutoPowerOffTime(DevStat.poweroff_time);       //恢复休眠
//                    return OPER_SLEEP;
//                default:
//                    break;
//            }
//        }
//    }
//
//}

// *****************************************************************
// 功能：		DataGather
// 说明:		数据采集
// 入口参数：	
// 出口参数：	Null
// 作者：		
// 返回值：		-1:关机					
// *****************************************************************
//int DataGather()
//{
//    typ_msg_word        msg;
//    typ_UART_stat_word  Usw;
//    char                length;//接收数据长度
//
//    INT32U rec_num;
//// 	INT8U *p;
//    char        flag;
//    RECORD *prec;
//    uchar ucResult;
//
//// 	//采集卡记录判断
//// 	rec_num = DB_count_records(DB_REC);
//// 	if(rec_num == 0)			// 没有记录
//// 	{
//// 		DispStr_CE(0,1, "未刷采集卡!", DISP_CENTER | DISP_CLRSCR);
//// 		DispStr_CE(0,3, "请返回至主画面", DISP_CENTER);
//// 		DispStr_CE(0,5, "刷采集卡", DISP_CENTER);
//// 		Beep(1000);
//// 		SleepMs(1000);
//// 		return notok;
//// 	}
//// 	else
//// 	{
//// 		//判断最后一条记录是否为采集卡记录
//// 		prec = (RECORD *)DB_jump_to_record(DB_REC, rec_num - 1, &flag);
//// 		if (prec->card_purchase_type != TT_GATHER)
//// 		{
//// 			DispStr_CE(0,1, "未刷采集卡!", DISP_CENTER | DISP_CLRSCR);
//// 			DispStr_CE(0,3, "请返回至主画面", DISP_CENTER);
//// 			DispStr_CE(0,5, "刷采集卡", DISP_CENTER);
//// 			Beep(1000);
//// 			SleepMs(1000);
//// 			return notok;
//// 		}
//// 	}
//    //(void)EA_ucClrKeyBuf();
//    //DispStr_CE(0,0, "通讯中...", DISP_CENTER);
//    EA_vCls();
//    EA_vDisplay(1, "通讯中...");
//    //InitCom();//初始化RS232
//    //  打开RS232设备
//    ucResult = EA_ucOpenDevice("COM1", EM_io_RDWR, &hRS232Handle);
//    if ( ucResult != EM_SUCCESS )
//    {
//        lcddisperr("打开串口失败");
//        return;
//    }
//    RcvdLength = 0;
//
//    while ( 1 )
//    {
//        //SetAutoPowerOffTime(9999);            //通讯禁止休眠
//        msg.s_word = sys_msg(SM_STAY_AWAKE);
//        if ( msg.bits.comm_data )
//        {
//            ReceiveChar = (char)armgetch();
//
//#ifdef DEBUG
//            TRACE(chtostr(ReceiveChar));
//#endif
//
//            if ( RcvdLength == 0 && !(ReceiveChar == 0x02 || ReceiveChar == '@') )
//            {
//                continue;       //开头必须为0x02或'@'
//            }
//            else
//            {
//            }
//
//            RcvBuf[RcvdLength] = ReceiveChar;
//            RcvdLength++;
//
//            if ( RcvdLength >= COM_BUFFER_SIZE )
//            {
//                RcvdLength = 0;             //长度超了
//                return;
//            }
//
//            if ( RcvdLength == RcvBuf[2] + 4 )
//            {
//                CmdProcess();
//            }
//
//// 			if ( RcvdLength==2 )//此时接收到的是数据域长度字节
//// 			{
//// 				RcvBuf[RcvdLength] = ReceiveChar;
//// 				length = ReceiveChar;
//// 			}
//// 			else
//// 			{
//// 				if(RcvdLength == length+14)//@START + COMMAD + LENGTH + DATA + CRC1 + CRC2 + \END
//// 				{
//// 					CommunicationDeal();//通讯处理
//// 					RcvdLength = 0;
//// 				}
//// 				else
//// 				{
//// 					RcvBuf[RcvdLength++] = ReceiveChar;
//// 				}
//// 			}
//        }
//
//        if ( msg.bits.key_available )
//        {
//            switch ( KEY_read() )
//            {
//                case EXIT_KEY_COMM:
//                case EXIT_KEY_F1:
//                    CloseCom();
//                    //SetAutoPowerOffTime(DevStat.poweroff_time);           //恢复休眠
//                    return 1;
//
//                case EXIT_KEY_POWER:
//                    CloseCom();
//                    //SetAutoPowerOffTime(DevStat.poweroff_time);           //恢复休眠
//                    return -1;
//
//                default:
//                    break;
//            }
//        }
//    }
//
//    CloseCom();
//
//
//    return 1;
//}

// *****************************************************************
// 功能：		RecordGather
// 说明:		采集其他手持数据
// 入口参数：	
// 出口参数：	Null
// 作者：		
// 返回值：		-1:关机					
// *****************************************************************
//int RecordGather()
//{
//
//    typ_msg_word        msg;
//    typ_UART_stat_word  Usw;
//    char                length;//接收数据长度
//
//    INT32U rec_num;
//    INT32U rec_no;//记录序号
//    INT16U crc;
//    INT8U record[83];
//    INT32U  black_num;//黑名单数量
//    INT32U  black_no;//黑名单序号
//// 	INT8U *p;
//    char        flag;
//    INT8U *p;
//    int    ret=0,i=0,j=0;
//
////  (void)EA_ucClrKeyBuf();
//    DispStr_CE(0,0, "通讯中...", DISP_CENTER);
//
//    InitCom();//初始化RS232
//
//    RcvdLength = 0;
//
////////////////////////////////////
////读取数据
/////////////////////////////////////
//
//
////读取记录总数
//
//    SndBuf[0]=0x02;
//    SndBuf[1]=0x00;
//    SndBuf[2]=10;
//    SndBuf[3]='Z';
//    SndBuf[4]='N';
//    SndBuf[5]='I';
//    SndBuf[6]=0x00;
//    for(i=0;i<6;i++)
//        SndBuf[7+i]=0xFF;
//    Rs232BCCWrite(SndBuf, 13);
//    NeedRcvLength = 35;
//    ret = CommunicationDeal();
//    if(ret != 0)
//    {
//        CloseCom();
//        return ret;
//    }
//
//    rec_no = 0;
////	err_num = 0;
//    NeedRcvLength = 0;
//
//    while(1)
//    {
//        SndBuf[0]=0x02;
//        SndBuf[1]=0x00;
//        SndBuf[2]=4;
//        SndBuf[3]='I';
//        SndBuf[4]=0x00;//暂时固定为0x01
//        SndBuf[5]=(INT8U)(rec_no>>8);
//        SndBuf[6]=(INT8U)(rec_no&0xff);
//        Rs232BCCWrite(SndBuf, 7);
//        ret = CommunicationDeal();
//        if(ret != 0)
//        {
//            CloseCom();
//            return ret;
//        }
//
//        if(RcvBuf[0]==0x20 && RcvBuf[1]==0x00 && RcvBuf[3] == 'E')//没有记录了
//            break;
//        if(RcvBuf[0]==0x20 && RcvBuf[1]==0x00 && RcvBuf[3] == 'I')//有记录
//        {
//
//            //保存记录到数据区
//            memcpy(record, (INT8U *)(RcvBuf+6), 57);
//            crc = Cal_CRC(record, 57);
//            record[57] = ((crc >> 8)& 0xff);//CRC1
//            record[58] = (crc & 0xff);//CRC2
//            ret = SaveCardRecord(record);
//            if ( ret != ok )
//            {
//#ifdef DEBUG
//                TRACE232("Store record fail.\n\r");
//#endif
//                (void)EA_ucClrKeyBuf();
//                DispStr_CE(0,0, "数据库保存失败!", DISP_CENTER);
//
//                CloseCom();
//                return notok;
//            }
//
//            rec_no++;//记录指针增加
//        }
//
//    }
//
////////////////////////////////////
////下载黑名单
/////////////////////////////////////
//    SndBuf[0]=0x02;
//    SndBuf[1]=0x00;
//    SndBuf[2]=0x01;
//    SndBuf[3]='J';
//    Rs232BCCWrite(SndBuf, 4);
//    ret = CommunicationDeal();
//    if(ret != 0)
//    {
//        CloseCom();
//        (void)EA_ucClrKeyBuf();
//        DispStr_CE(0,0, "数据下载成功!", DISP_CENTER);
//        DispStr_CE(0,4, "黑名单清空失败!", DISP_CENTER);
//        return ret;
//    }
//    black_num = DB_count_records(DB_BLACK);
//
//    black_no = 0;
//    while(1)
//    {
//        if(black_num == black_no)
//            break;
//        SndBuf[0]=0x02;
//        SndBuf[1]=0x00;
//        SndBuf[2]=0x05;
//        SndBuf[3]='G';
//        p = (INT8U *)DB_jump_to_record(DB_BLACK, black_no, &flag);  //取记录
//        memcpy((void *)&SndBuf[4], p,4);
//
//        Rs232BCCWrite(SndBuf, 4 + 5);
//
//        ret = CommunicationDeal();
//        if(ret != 0)
//        {
//            CloseCom();
//            return ret;
//        }
//
//        if(RcvBuf[0]==0x20 && RcvBuf[1]==0x00 && RcvBuf[3] == 'G')//黑名单更新成功
//        {
//
//            black_no++;//记录指针增加
//        }
//
//    }
//
////////////////////////////////////
////更新数据盒时间
/////////////////////////////////////
//
//    (void)EA_ucClrKeyBuf();
//    DispStr_CE(0,0, "采集成功!", DISP_CENTER);
//    delay_n_ms(200);
//    CloseCom();
//
//
//    return 1;
//}

/*****************************************************************
函数原型：chtostr
功能描述：把一个'1'转换为"1"
参数描述：
参数名称：	输入/输出？	类型		描述
-----------		-----------	------	   	-------
disp_buf		输入			INT8U		输入的参数

返  回  值：无
作      者	：许岩
日      期：2005-08-20
修改历史：
日期		修改人		修改描述
------		---------	-------------
*****************************************************************/
INT8U *chtostr(INT8U disp_buf)
{
	static INT8U buf[2];
	buf[0] = disp_buf;
	buf[1] = '\0';
	return buf;
}

/******************************************************************************
 函数名称：CmdProcess
 功能描述：与数据盒的通讯命令解析和处理
 参数描述：
 参数名称：	输入/输出？	类型		描述
				
 返  回  值：无
 作      者	：许岩
 日      期：2004-09-02
 修改历史：
		日期		修改人		修改描述
		------		---------	-------------
******************************************************************************/
//void CmdProcess(void)
//{
//
////	INT8U LOCAL_V_TYPE i = 0;
////     INT8U j = 0;
////     INT8U LOCAL_V_TYPE m = 0;
//    static INT16U LOCAL_V_TYPE cur_trans_rec_num = 0;               //当前传送记录的指针
//    static INT8U LOCAL_V_TYPE rec_have_snd = 0;                         //当发送出一条记录时，＝TRUE，当收到确认时＝FLASE，保证了记录的不丢条
////	INT16U LOCAL_V_TYPE data_len = 0;										//数据域长度
//    BUS_TIME time;
////	typ_RTC_date_rec    date;
////	typ_RTC_time_rec    time2;
//    INT32U temp_int32u = 0;
//    INT32U black_ptr = 0;
////	INT16U temp_int16u = 0;
//    static INT32U rec_num = 0;
//    static INT32U db_rec_num = 0;
//    static INT32U db_rec_bak_num = 0;
//    static INT32U db_rec_history_num = 0;
//    INT32U cur_trans_rec_num = 0;               //当前传送记录的指针
//    INT8U *p;
//    char        flag=1;
////	INT32U rec_ptr;
//    INT8U buf[40];
//
//
//    switch ( RcvBuf[3] )
//    {
//        ////////////////////////////////////////
//        //检测数据盒是否存在或删除数据盒
//        ////////////////////////////////////////
//        case  'O':      //欧
//        case  'Y':      // 删除数据盒(Del Box)
//            SetBell(ON);    //开蜂鸣器
//
//            SndBuf[0] = 0x20;
//            SndBuf[1] = 0x00;
//            SndBuf[2] = 0x01;
//            SndBuf[3] = 'O';        //欧
//
//            Rs232BCCWrite(SndBuf, 4);
//
//            SetBell(OFF);   //关蜂鸣器
//            break;
//
//            ////////////////////////////////////////
//            // 读数据盒类型
//            ////////////////////////////////////////
//        case 'Q':
//            SetBell(ON);    //开蜂鸣器
//
//            memcpy((void *)SndBuf, "\x20\x00\x05Q\x35\x00\x00\x00", 8);
//            SndBuf[8] = DevStat.POS_type;
//
//            Rs232BCCWrite(SndBuf, 9);
//
//            SetBell(OFF);   //关蜂鸣器
//            break;
//
//            ////////////////////////////////////////
//            // 读数据盒时间
//            ////////////////////////////////////////
//        case 'A':
//            SetBell(ON);    //开蜂鸣器
//            (void)Get_Time(&time);
//
//            memset((void *)SndBuf, 0x00, sizeof(SndBuf));
//            memcpy((void *)SndBuf, "\x20\x00", 2);
//            SndBuf[2] = DevStat.POS_type;
//            memcpy((void *)&SndBuf[19], &time, sizeof(time));
//            //memcpy((void *)&SndBuf[26], &DevStat.bus_number[1], 2);
//            //memcpy((void *)&SndBuf[28], &DevStat.line_number[0], 2);
//            memcpy((void *)&SndBuf[26], &DevStat.equ_id[1], 2);
//            memcpy((void *)&SndBuf[28], &DevStat.acnt_id[0], 2);
//            Rs232BCCWrite(SndBuf, 30);
//
//            SetBell(OFF);   //关蜂鸣器
//            break;
//
//            ////////////////////////////////////////
//            // 更改数据盒类型(此项更改关机后自动清除)
//            ////////////////////////////////////////
//        case  'D':
//            SetBell(ON);    //开蜂鸣器
//            DevStat.POS_type = POS_T_ABNORMAL;
//
//            memcpy((void *)SndBuf, (void *)RcvBuf, 7);
//            SndBuf[0] = 0x20;
//            SndBuf[1] = 0x00;
//            Rs232BCCWrite(SndBuf, 7);
//            SetBell(OFF);   //关蜂鸣器
//            break;
//
//        case  'K':
//            SetBell(ON);    //开蜂鸣器
//
//            if ( RcvBuf[4] == POS_T_NORMAL )              //其他类型不理会
//            {
//                DevStat.POS_type = POS_T_NORMAL;
//
//                memcpy((void *)SndBuf, (void *)RcvBuf, 7);
//                SndBuf[0] = 0x20;
//                SndBuf[1] = 0x00;
//                Rs232BCCWrite(SndBuf, 7);
//            }
//
//            SetBell(OFF);   //关蜂鸣器
//            break;
//
//            ////////////////////////////////////////
//            // 数据盒时间更新(change time from computer)
//            ////////////////////////////////////////
//        case  'T':
//            SetBell(ON);    //开蜂鸣器
//
//            date.fields.year = bcdhex(RcvBuf[4]) * 100 + bcdhex(RcvBuf[5]);
//            date.fields.month = bcdhex(RcvBuf[6]);
//            date.fields.day = bcdhex(RcvBuf[7]);
//
//            time2.fields.hour = bcdhex(RcvBuf[8]);
//            time2.fields.minute = bcdhex(RcvBuf[9]);
//            time2.fields.second = bcdhex(RcvBuf[10]);
//
//            // 修改当前系统时间
//            (void)RTC_set_date(date.l_word);
//            (void)RTC_set_time(time2.l_word);
//
//            if(time_okay_3511() == 0)
//            init_3511();
//
//            if(time_okay_3511() != 0)
//            {
//                date.l_word = RTC_read_date();
//                time2.l_word    = RTC_read_time();
//                set_all_time_for_3511(date, time2);
//            }
//
//            memcpy((void *)SndBuf, (void *)RcvBuf, 12);
//            SndBuf[0] = 0x20;
//            SndBuf[1] = 0x00;
//            Rs232BCCWrite(SndBuf, 12);
//
//            SetBell(OFF);   //关蜂鸣器
//            break;
//
//            ////////////////////////////////////////
//            // 查询数据盒中有文件个数
//            ////////////////////////////////////////
//        case 'R':
//            SetBell(ON);    //开蜂鸣器
//
//            memset((void *)SndBuf, 0x00, sizeof(SndBuf));
//            memcpy((void *)SndBuf, "\x20\x00", 2);
//
//            if ( DevStat.POS_type == POS_T_NORMAL && DB_count_records(DB_REC) == 0 )          //正常盒，没有记录
//            {
//                SndBuf[2] = 0x00;           //文件数量
//                SndBuf[3] = 0x35;       //???
//                SndBuf[5] = DevStat.POS_type;
//                SndBuf[15] = DevStat.POS_type;
//                SndBuf[16] = 0x00;          //文件数量
//                Rs232BCCWrite(SndBuf, 17);
//                SetBell(OFF);   //关蜂鸣器
//                break;
//            }
//
//            SndBuf[2] = 0x01;           //文件数量
//            SndBuf[3] = 0x35;       //???
//            SndBuf[5] = DevStat.POS_type;
//            SndBuf[15] = DevStat.POS_type;
//            SndBuf[16] = 0x01;          //文件数量
//
//            // 读文件名到通信数据缓冲区
//            //读当前数据文件个数及文件中记录个数
//            switch ( DevStat.POS_type )
//            {
//                case POS_T_NORMAL:          //正常采集
//                    temp_int32u = DB_count_records(DB_REC);
//                    SndBuf[17 + 15] = (INT8U)(temp_int32u >> 8);            //记录数量
//                    SndBuf[17 + 16] = (INT8U)temp_int32u;
//                    break;
//
//                case POS_T_ABNORMAL:            //异常采集
//                    temp_int32u = DB_count_records(DB_REC) + DB_count_records(DB_REC_BAK)
//                                  + DB_count_records(DB_REC_HISTORY);
//                    SndBuf[17 + 15] = (INT8U)(temp_int32u >> 8);            //记录数量
//                    SndBuf[17 + 16] = (INT8U)temp_int32u;
//                    break;
//
//            }
//
//            (void)Get_Time(&time);
//
//            SndBuf[17] = 0x01;                              // POS中文件序号
//            SndBuf[18] = DevStat.POS_type;          // 数据盒类型
//            //SndBuf[19] = DevStat.line_number[0];  //线路号
//            //SndBuf[20] = DevStat.line_number[1];
//            //SndBuf[21] = DevStat.bus_number[0];       //车号
//            //SndBuf[22] = DevStat.bus_number[1];
//            SndBuf[19] = DevStat.acnt_id[0];    //线路号
//            SndBuf[20] = DevStat.acnt_id[1];
//            SndBuf[21] = DevStat.equ_id[0];     //车号
//            SndBuf[22] = DevStat.equ_id[1];
//            SndBuf[23] = 0x00;                                  // 下载员号
//            SndBuf[24] = 0x00;                                  // 下载员号
//
//            memcpy((void *)&SndBuf[25], (void *)&time, 7);          //下载时间
//
//            memcpy((void *)&SndBuf[34], (void *)&DevStat.pos_number[0], 6);         //pos号
////			SndBuf[40] = DevStat.bus_number[2];		// 车号最高位
//            SndBuf[40] = DevStat.equ_id[2];     // 车号最高位
//
//
//            Rs232BCCWrite(SndBuf, 41);
//
//            SetBell(OFF);   //关蜂鸣器
//            break;
//
//
//            ////////////////////////////////////////
//            // 读取数据盒中指定序号的黑名单信息
//            ////////////////////////////////////////
//        case 'X':
//            SetBell(ON);    //开蜂鸣器
//            temp_int32u = DB_count_records(DB_BLACK);
//            black_ptr = ((INT32U)RcvBuf[4] << 8) + (INT32U)RcvBuf[5];           //要读取的黑名单条号
//
//            if ( black_ptr >= temp_int32u )         //要取的黑名单条数大于黑名单容量
//            {
//                memset((void *)SndBuf, 0x00, sizeof(SndBuf));
//                memcpy((void *)SndBuf, "\x02\x00", 2);
//            }
//            else
//            {
//                memset((void *)SndBuf, 0x00, sizeof(SndBuf));
//                memcpy((void *)SndBuf, "\x20\x00", 2);
//                GetBlackName((void *)&SndBuf[5], (INT16U)black_ptr);
//            }
//
//            SndBuf[2] = 'X';
//            SndBuf[3] = (INT8U)temp_int32u;
//            SndBuf[4] = (INT8U)(temp_int32u >> 8);
//
//            Rs232BCCWrite(SndBuf, 9);
//
//            SetBell(OFF);   //关蜂鸣器
//            break;
//
//            ////////////////////////////////////////
//            // 清空黑名单(Clear BlackName)
//            ////////////////////////////////////////
//        case  'J':
//            SetBell(ON);    //开蜂鸣器
//
//            DispStr_CE(0,2,"下载黑名单",DISP_CENTER|DISP_CLRSCR);
//
//            // 清黑名单总数
//            (void)DB_erase_filesys(DB_BLACK);
//
//            memcpy((void *)SndBuf, (void *)RcvBuf, 7);
//            SndBuf[0] = 0x20;
//            SndBuf[1] = 0x00;
//            Rs232BCCWrite(SndBuf, 4);
//
//            SetBell(OFF);   //关蜂鸣器
//            break;
//
//            ////////////////////////////////////////
//            // 下载黑名单(Add blackname)
//            ////////////////////////////////////////
//        case 'G':
//            DB_add_record(DB_BLACK, &RcvBuf[4]);
//
//            memcpy((void *)SndBuf, (void *)RcvBuf, 9);
//            SndBuf[0] = 0x20;
//            SndBuf[1] = 0x00;
//            Rs232BCCWrite(SndBuf, 9);
//
//            break;
//
//            ////////////////////////////////////////
//            // 启动采集指定文件
//            ////////////////////////////////////////
//        case 'Z':
//            DispStr_CE(0,2,"记录传输中",DISP_CENTER|DISP_CLRSCR);
//
//            memcpy((void *)SndBuf, (void *)RcvBuf, 9);
//            SndBuf[0] = 0x20;
//            SndBuf[1] = 0x00;
//
//            SndBuf[13] = RcvBuf[6] + 1;
//
//            //SndBuf[15] = DevStat.line_number[0];  //线路号
//            //SndBuf[16] = DevStat.line_number[1];
//            //SndBuf[17] = DevStat.bus_number[0];       //车号
//            //SndBuf[18] = DevStat.bus_number[1];
//            SndBuf[15] = DevStat.acnt_id[0];    //线路号
//            SndBuf[16] = DevStat.acnt_id[1];
//            SndBuf[17] = DevStat.equ_id[0];     //车号
//            SndBuf[18] = DevStat.equ_id[1];
//            SndBuf[19] = 0x00;                                  // 下载员号
//            SndBuf[20] = 0x00;                                  // 下载员号
//
//            (void)Get_Time(&time);
//            memcpy((void *)&SndBuf[21], (void *)&time, 7);          //下载时间
//
//            memcpy((void *)&SndBuf[28], (void *)&DevStat.pos_number[0], 6);         //pos号
//            //SndBuf[34] = DevStat.bus_number[2];       // 车号最高位
//            SndBuf[18] = DevStat.equ_id[2];
//
//// 			rec_num = DB_count_records(DB_REC);
//
//            switch ( DevStat.POS_type )
//            {
//                case POS_T_NORMAL:          //正常采集
//                    rec_num = DB_count_records(DB_REC);
//                    break;
//
//                case POS_T_ABNORMAL:            //异常采集
//                    db_rec_num = DB_count_records(DB_REC);
//                    db_rec_bak_num = DB_count_records(DB_REC_BAK);
//                    db_rec_history_num = DB_count_records(DB_REC_HISTORY);
//                    rec_num = db_rec_num + db_rec_bak_num + db_rec_history_num;
//                    break;
//
//            }
//
//            Rs232BCCWrite(SndBuf, 35);
//            break;
//
//            ////////////////////////////////////////
//            // 记录上传
//            ////////////////////////////////////////
//        case    'I':
//            cur_trans_rec_num = ((INT32U)RcvBuf[5] << 8) + (INT32U)RcvBuf[6];
//
//            sprintf((void *)buf,"进度%5.2f%%", (FP32)cur_trans_rec_num / (FP32)rec_num * 100.0);
//            EA_vDisp(4,1,buf);
//
//            if ( cur_trans_rec_num >= rec_num )           //记录全部传输完毕
//            {
//                EA_vDisp(2,1,"记录传输完毕");
//
//                SndBuf[0] = 0x20;
//                SndBuf[1] = 0x00;
//                SndBuf[2] = 0x03;           //数据包长度
//                SndBuf[3] = 'E';
//                SndBuf[4] = RcvBuf[5];      //记录条号
//                SndBuf[5] = RcvBuf[6];
//
//                //Rs232BCCWrite(SndBuf, 7);
//                break;
//            }
//
//            SndBuf[0] = 0x20;
//            SndBuf[1] = 0x00;
//            SndBuf[2] = 57 + 3;         //数据包长度
//            SndBuf[3] = 'I';
//            SndBuf[4] = RcvBuf[5];      //记录条号
//            SndBuf[5] = RcvBuf[6];
//
//            switch ( DevStat.POS_type )
//            {
//                case POS_T_NORMAL:
//                default:
//                    p = (INT8U *)DB_jump_to_record(DB_REC, cur_trans_rec_num, &flag);   //取记录
//                    //memcpy((void *)&SndBuf[6], p, 32);
//
//                    //Rs232BCCWrite(SndBuf, 32 + 7);
//                    memcpy((void *)&SndBuf[6], p, 57);
//
//                    Rs232BCCWrite(SndBuf, 57 + 7);
//                    break;
//
//                case POS_T_ABNORMAL:        //异常补采
//                    if (cur_trans_rec_num < db_rec_num)
//                    {
//                        p = (INT8U *)DB_jump_to_record(DB_REC, cur_trans_rec_num, &flag);   //取记录
//                    }
//                    else if (cur_trans_rec_num < db_rec_bak_num)
//                    {
//                        p = (INT8U *)DB_jump_to_record(DB_REC, cur_trans_rec_num - db_rec_num, &flag);  //取记录
//                    }
//                    else
//                    {
//                        p = (INT8U *)DB_jump_to_record(DB_REC, cur_trans_rec_num - db_rec_num - db_rec_bak_num, &flag); //取记录
//                    }
//
//                    //memcpy((void *)&SndBuf[6], p, 32);
//
//                    //Rs232BCCWrite(SndBuf, 32 + 7);
//                    memcpy((void *)&SndBuf[6], p, 57);
//
//                    Rs232BCCWrite(SndBuf, 57 + 7);
//                    break;
//            }
//            break;
//
//            ////////////////////////////////////////
//            // 清空数据盒，在此做为采集结束用
//            ////////////////////////////////////////
//        case  'F':
//// 			SetBell(ON);	//开蜂鸣器
//
//            memcpy((void *)SndBuf, (void *)RcvBuf, 4);
//            SndBuf[0] = 0x20;
//            SndBuf[1] = 0x00;
//
//            Rs232BCCWrite(SndBuf, 4);
//
//            if ( DB_count_records(DB_REC_BAK) >
//                 DB_capacity(DB_REC_HISTORY) - DB_count_records(DB_REC_HISTORY) )
//            {   //历史记录数据库容量不够了，清空
//                DispStr_CE(0,2,"清空历史数据库",DISP_CENTER|DISP_CLRSCR);
//                (void)DB_erase_filesys(DB_REC_HISTORY);
//            }
//
//            //db_rec_bak -> db_rec_history
//            DispStr_CE(0,2,"写历史数据库",DISP_CENTER|DISP_CLRSCR);
//            rec_num = DB_count_records(DB_REC_BAK);
//            for ( temp_int32u = 0; temp_int32u < rec_num; temp_int32u++ )
//            {
//                DB_add_record( DB_REC_HISTORY, DB_jump_to_record(DB_REC_BAK, temp_int32u, &flag) );
//                sprintf(buf,"进度%5.2f%%", ((FP32)temp_int32u + 1.0) / (FP32)rec_num * 100.0);
//                DispStr_CE(0,4,buf,DISP_CENTER|DISP_CLRSCR);
//            }
//
//            //db_rec -> db_rec_bak
//            DispStr_CE(0,2,"写正常补采数据库",DISP_CENTER|DISP_CLRSCR);
//            (void)DB_erase_filesys(DB_REC_BAK);
//            rec_num = DB_count_records(DB_REC);
//            for ( temp_int32u = 0; temp_int32u < rec_num; temp_int32u++ )
//            {
//                DB_add_record( DB_REC_BAK, DB_jump_to_record(DB_REC, temp_int32u, &flag) );
//                sprintf(buf,"进度%5.2f%%", ((FP32)temp_int32u + 1.0) / (FP32)rec_num * 100.0);
//                DispStr_CE(0,4,buf,DISP_CENTER|DISP_CLRSCR);
//            }
//
//            DispStr_CE(0,2,"清空正常采集数据库",DISP_CENTER|DISP_CLRSCR);
//            (void)DB_erase_filesys(DB_REC);
//
//            DispStr_CE(0,2,"完毕",DISP_CENTER|DISP_CLRSCR);
//            Beep(100);
//            SleepMs(100);
//            Beep(100);
//
//            break;
//
//
//
//        default:    //do nothing
//            break;
//    }
//
//// 	DevStat.databox_cmd_rcvd = FALSE;
//    RcvdLength = 0;                         //清接收缓冲区
//    ClrUART();
//// 	i = SBUF;			//清接收缓冲区
//// 	RI = 0;
//// 	EnableSerialINT();
//
//
//}
//
//INT8U InitParam(void)
//{
//
////  SELE_INLINE KS_Setting;
////	int ret;
//    INT16U key;
////  EA_ucClrKeyBuf();
//    EA_vDisp(1, 1, "   系统将恢复到出   ");
//    EA_vDisp(2, 1, "  厂状态, 所有记录  ");
//    EA_vDisp(3, 1, "  和参数全部被清除  ");
//    EA_vDisp(4, 1, "确定            取消");
//
//
////    KS_Setting.count = 2;   //菜单条数
////    KS_Setting.line = 6;    //菜单行号
//////	KS_Setting.enable46 = 1; //使能4，6键。这里可以使用4，6键代替滑轮
//////	KS_Setting.flagch = 0;	 //当前选择项的标志字符‘◆’
////    KS_Setting.defsele = 0; // 默认选择“关闭”
////    KS_Setting.autoexit = 30; // 30秒无按键自动退出
////
////    KS_Setting.selepstr[0].xx = 1;  //第一菜单位置
////    strcpy(KS_Setting.selepstr[0].pstr, "否" );
////    KS_Setting.selepstr[1].xx = 8;  //第二菜单位置
////    strcpy( KS_Setting.selepstr[1].pstr, "是" );
////
////    KS_Setting.qEvent = EXIT_KEY_F1 | EXIT_KEY_COMM|EXIT_KEY_POWER; // 按[C]键电源键或无按键自动退出
//
////ret = select_in_line( &KS_Setting );
//    key = EA_uiInkey(0);
//    switch ( key )
//    {
////  	case EM_key_ENTER:	// 否
////  		return notok;
//        case EM_key_ENTER:  // 是
////  		EA_ucClrKeyBuf();
//            EA_vDisp(1, 1, "系统正在初始化");
//            EA_vDisp(2, 1, "请等待....");
//
////  		init_db();
////  		//初始化参数
////		   InitImportantParameter();
//
////  		(void)EA_vCls();
////			DispStr_CE(0, 2, "系统初始化完毕", DISP_CENTER);
//// 			DispStr_CE(0, 4, "将会自动关机", DISP_CENTER);
////			DispStr_CE(0, 4, "将会自动重启", DISP_CENTER);
////			Beep(1000);
//// 			(void)delay_and_wait_key(5,0,0);
////			SleepMs(1000);
////			resetpos();
////  		_start();
//            param_factory_default(1);
//            return ok;
//        default:
//            return notok;
//    }
//
//}

// *****************************************************************
// 功能：		OldManSetting
// 说明:		老年卡扣款设置
// 入口参数：	
// 出口参数：	Null
// 作者：
// 返回值：							
// *****************************************************************
INT8U OldManSetting(void)
{
	(void)EA_ucClrKeyBuf();
	EA_vDisp(1,1,"老年卡在本线路");
	EA_vDisp(2,1,"不享受免费待遇");
	EA_vDisp(3,1,"无须设置本参数");
	EA_vDisp(4,1,"按任意键将退出");

//  delay_and_wait_key(5, 0, 0);
	EA_uiInkey(5);
	
	return 1;


	
/*	SELE_INLINE KS_Setting;
	int ret;
	

	DispStr_CE(0,0,"老年卡扣款设定",DISP_CENTER|DISP_CLRSCR);

	KS_Setting.count = 2;	//菜单条数
	KS_Setting.line = 2;	//菜单行号
	KS_Setting.enable46 = 1; //使能4，6键。这里可以使用4，6键代替滑轮
	KS_Setting.flagch = 0;	 //当前选择项的标志字符‘◆’
	if (DevStat.oldman_free == TRUE)
		KS_Setting.defsele = 1;	// 默认选择“不扣款” 
	else
		KS_Setting.defsele = 0;	// 默认选择“扣款” 

	KS_Setting.autoexit = 30; // 30秒无按键自动退出

	KS_Setting.selepstr[0].xx = 25;	//第一菜单位置
	strcpy(KS_Setting.selepstr[0].pstr, "扣款" ); 
	KS_Setting.selepstr[1].xx = 72;	//第二菜单位置
	strcpy( KS_Setting.selepstr[1].pstr, "不扣款" ); 

	KS_Setting.qEvent = EXIT_KEY_F1 | EXIT_KEY_CLR|EXIT_KEY_POWER|EXIT_AUTO_QUIT; // 按[C]键电源键或无按键自动退出 
	ret = select_in_line( &KS_Setting ); 
	switch ( ret )
	{
		case 0:	// 选择扣款处理 
			DevStat.oldman_free = FALSE;
			WriteParam();
			break; 

		case 1:	// 选择不扣款处理
			DevStat.oldman_free = TRUE;
			WriteParam();
			break; 

		default: 
			if ( KS_Setting.qEvent = EXIT_KEY_F1 ) // 按[F1]键退出 
				return 1;
			else if ( KS_Setting.qEvent = EXIT_KEY_POWER ) // 按电源键退出 
				return -1;
			else if ( KS_Setting.qEvent = EXIT_AUTO_QUIT ) // 30秒无按键自动退出
				return 1;
			break;
	}

	return 1;
*/
}


// *****************************************************************
// 功能：		EmployeeASetting
// 说明:		家属卡扣款设置
// 入口参数：	
// 出口参数：	Null
// 作者：
// 返回值：							
// *****************************************************************
//INT8U EmployeeASetting(void)
//{
/*
	SELE_INLINE KS_Setting;
	int ret;

	DispStr_CE(0,0,"免费卡扣款设定",DISP_CENTER|DISP_CLRSCR);

	KS_Setting.count = 2;	//菜单条数
	KS_Setting.line = 2;	//菜单行号
	KS_Setting.enable46 = 1; //使能4，6键。这里可以使用4，6键代替滑轮
	KS_Setting.flagch = 0;	 //当前选择项的标志字符‘◆’
	if (DevStat.employee_a_free == TRUE)
		KS_Setting.defsele = 1;	// 默认选择“不扣款” 
	else
		KS_Setting.defsele = 0;	// 默认选择“扣款” 

	KS_Setting.autoexit = 30; // 30秒无按键自动退出

	KS_Setting.selepstr[0].xx = 25;	//第一菜单位置
	strcpy(KS_Setting.selepstr[0].pstr, "扣款" ); 
	KS_Setting.selepstr[1].xx = 72;	//第二菜单位置
	strcpy( KS_Setting.selepstr[1].pstr, "不扣款" ); 

	KS_Setting.qEvent = EXIT_KEY_F1 | EXIT_KEY_CLR|EXIT_KEY_POWER|EXIT_AUTO_QUIT; // 按[C]键电源键或无按键自动退出 
	ret = select_in_line( &KS_Setting ); 
	switch ( ret )
	{
		case 0:	// 选择扣款处理 
			DevStat.employee_a_free = FALSE;
			WriteParam();
			break; 

		case 1:	// 选择不扣款处理
			DevStat.employee_a_free = TRUE;
			WriteParam();
			break; 

		default: 
			if ( KS_Setting.qEvent = EXIT_KEY_F1 ) // 按[F1]键退出 
				return 1;
			else if ( KS_Setting.qEvent = EXIT_KEY_POWER ) // 按电源键退出 
				return -1;
			else if ( KS_Setting.qEvent = EXIT_AUTO_QUIT ) // 30秒无按键自动退出
				return 1;
			break;
	}
*/
//    return 1;
//}

// *****************************************************************
// 功能：		FreePriceSetting
// 说明:		免费卡票价设置
// 入口参数：	
// 出口参数：	Null
// 作者：
// 返回值：							
// *****************************************************************
//INT8U FreePriceSetting(void)
//{
/*	
	INT8U i = 0;
//	INT8U  PriceSettingMenu[10*16 +2] =  {0};
	INT8U buf[100];
	int 			ret;
//	int 			cPtr=0;
//	int 			lPtr=0;
	INT8U disp_buf[10];
	INT8U key = NO_KEY;
	const INT8U position[6][2] =								   //各位显示的位置
	{
		{4 * 8, 3},
		{5 * 8, 3},
		{6 * 8, 3},
		{7 * 8, 3},
		{9 * 8, 3},
		{10 * 8, 3},
	};
	INT8S cur_ptr = 0;

	ToggleWD();

	DispStr_CE(0, 1, "免费卡票价设置", DISP_CLRSCR | DISP_CENTER);

	sprintf(buf, "    %07.2f元   ", (FP64)DevStat.free_price/ 100.0);
	DispStr_CE(0, 3, buf, DISP_POSITION);


	disp_buf[0] = ascii(DevStat.free_price / 100000);						//千位
	disp_buf[1] = ascii((DevStat.free_price % 100000) / 10000);	//百位
	disp_buf[2] = ascii((DevStat.free_price % 10000) / 1000);		//十
	disp_buf[3] = ascii((DevStat.free_price % 1000) / 100);			//个
	disp_buf[4] = ascii((DevStat.free_price % 100) / 10);				//毛
	disp_buf[5] = ascii(DevStat.free_price % 10);							//分

	cur_ptr = 0;
	DispStr_CE(position[cur_ptr][0], position[cur_ptr][1], chtostr(disp_buf[0]), DISP_POSITION | DISP_REVERSE);


	for ( ;; )
	{
		do
		{				  //only accept 0-9,KEY_CANCEL, KEY_ENTER
			key = get_key();
		}while ( !( (key>='0'&&key<='9') || (key==EXIT_KEY_F1) || (key==EXIT_KEY_COMM) || (key==EXIT_KEY_ENT) ) );

		switch ( key )
		{
			case EXIT_KEY_COMM:
				break;

			case EXIT_KEY_F1:
				return notok;			//不修改时间，返回

			case EXIT_KEY_ENT:
					DevStat.free_price = (INT32U)ascii_to_hex(disp_buf[0]) * 100000 + (INT32U)ascii_to_hex(disp_buf[1]) * 10000 
							+ (INT32U)ascii_to_hex(disp_buf[2]) * 1000 + (INT32U)ascii_to_hex(disp_buf[3]) * 100 
							+ (INT32U)ascii_to_hex(disp_buf[4]) * 10 + (INT32U)ascii_to_hex(disp_buf[5]);
					WriteParam();
					return ok;

			default:			//数字键处理
				disp_buf[cur_ptr] = key;
				DispStr_CE(position[cur_ptr][0], position[cur_ptr][1], chtostr(disp_buf[cur_ptr]), DISP_POSITION);
				cur_ptr++;
				if ( cur_ptr >= 6 )
				{
					cur_ptr = 0;
				}
				DispStr_CE(position[cur_ptr][0], position[cur_ptr][1], chtostr(disp_buf[cur_ptr]), DISP_POSITION | DISP_REVERSE);
//				DispStr_CE(position[cur_ptr][0], position[cur_ptr][1], "_", DISP_POSITION);
				break;
		}
	}

*/
//    return ok;
//}

// *****************************************************************
// 功能：		DriverSetting
// 说明:		员工卡扣款设置
// 入口参数：	
// 出口参数：	Null
// 作者：
// 返回值：							
// *****************************************************************
INT8U DriverSetting(void)
{
	(void)EA_ucClrKeyBuf();
	EA_vDisp(1,1,"员工卡在本线路");
	EA_vDisp(2,1,"不享受免费待遇");
	EA_vDisp(3,1,"无须设置本参数");
	EA_vDisp(4,1,"按任意键将退出");

//  delay_and_wait_key(5, 0, 0);
	EA_uiInkey(5);
	
	return 1;

/*	
	SELE_INLINE KS_Setting;
	int ret;

	DispStr_CE(0,0,"员工卡扣款设定",DISP_CENTER|DISP_CLRSCR);

	KS_Setting.count = 2;	//菜单条数
	KS_Setting.line = 2;	//菜单行号
	KS_Setting.enable46 = 1; //使能4，6键。这里可以使用4，6键代替滑轮
	KS_Setting.flagch = 0;	 //当前选择项的标志字符‘◆’
	if (DevStat.driver_free == TRUE)
		KS_Setting.defsele = 1;	// 默认选择“不扣款” 
	else
		KS_Setting.defsele = 0;	// 默认选择“扣款” 

	KS_Setting.autoexit = 30; // 30秒无按键自动退出

	KS_Setting.selepstr[0].xx = 25;	//第一菜单位置
	strcpy(KS_Setting.selepstr[0].pstr, "扣款" ); 
	KS_Setting.selepstr[1].xx = 72;	//第二菜单位置
	strcpy( KS_Setting.selepstr[1].pstr, "不扣款" ); 

	KS_Setting.qEvent = EXIT_KEY_F1 | EXIT_KEY_CLR|EXIT_KEY_POWER|EXIT_AUTO_QUIT; // 按[C]键电源键或无按键自动退出 
	ret = select_in_line( &KS_Setting ); 
	switch ( ret )
	{
		case 0:	// 选择扣款处理 
			DevStat.driver_free = FALSE;
			WriteParam();
			break; 

		case 1:	// 选择不扣款处理
			DevStat.driver_free = TRUE;
			WriteParam();
			break; 

		default: 
			if ( KS_Setting.qEvent = EXIT_KEY_F1 ) // 按[F1]键退出 
				return 1;
			else if ( KS_Setting.qEvent = EXIT_KEY_POWER ) // 按电源键退出 
				return -1;
			else if ( KS_Setting.qEvent = EXIT_AUTO_QUIT ) // 30秒无按键自动退出
				return 1;
			break;
	}

	return 1;
*/
}


// *****************************************************************
// 功能：		ModeSetting
// 说明:		消费模式设置
// 入口参数：	
// 出口参数：	Null
// 作者：
// 返回值：							
// *****************************************************************

//INT8U ModeSetting(void)
//{
/*	SELE_INLINE KS_Setting;
	int ret;

	DispStr_CE(0,0,"消费模式设定",DISP_CENTER|DISP_CLRSCR);

	KS_Setting.count = 2;	//菜单条数
	KS_Setting.line = 2;	//菜单行号
	KS_Setting.enable46 = 1; //使能4，6键。这里可以使用4，6键代替滑轮
	KS_Setting.flagch = 0;	 //当前选择项的标志字符‘◆’
	if (DevStat.mode == NORMAL)
		KS_Setting.defsele = 0;	// 选择“正常” 
	else
		KS_Setting.defsele = 1;	// 选择“快捷” 

	KS_Setting.autoexit = 30; // 30秒无按键自动退出

	KS_Setting.selepstr[0].xx = 25;	//第一菜单位置
	strcpy(KS_Setting.selepstr[0].pstr, "正常" ); 
	KS_Setting.selepstr[1].xx = 72;	//第二菜单位置
	strcpy( KS_Setting.selepstr[1].pstr, "快捷" ); 

	KS_Setting.qEvent = EXIT_KEY_F1 | EXIT_KEY_CLR|EXIT_KEY_POWER|EXIT_AUTO_QUIT; // 按[C]键电源键或无按键自动退出 
	ret = select_in_line( &KS_Setting ); 
	switch ( ret )
	{
		case 0:	// 选择正常处理 
			DevStat.mode = NORMAL;
			WriteParam();
			break; 

		case 1:	// 选择快捷处理
			DevStat.mode = SHORTCUT;
			WriteParam();
			break; 

		default: 
			if ( KS_Setting.qEvent = EXIT_KEY_F1 ) // 按[F1]键退出 
				return 1;
			else if ( KS_Setting.qEvent = EXIT_KEY_POWER ) // 按电源键退出 
				return -1;
			//else if ( KS_Setting.qEvent = AUTO_QUIT ) // 30秒无按键自动退出
			//	return 1;
			break;
	}
*/
//    return 1;
//}

// *****************************************************************
// 功能：		PHoldTime_Setting
// 说明:		刷卡持续时间设置
// 入口参数：	
// 出口参数：	Null
// 作者：
// 返回值：		-1:关机					
// *****************************************************************
//int PHoldTime_Setting()
//{
/*
	char strbuf[50];
	int ret;
	long ll_input=0;
	int cPtr = 0;
	int lPtr = 0;
	GETSTR_PARAM gs;
// 	char    buf[50];//输入字符存放数组

	EA_ucClrKeyBuf();

// 	DispStr_CE(0,0,"推荐休眠时间为60",DISP_CENTER|DISP_CLRSCR);
	DispStr_CE(0,2,"刷卡持续时间(秒)",DISP_CENTER);
	DispStr_CE(0,4,"范围:1 - 255秒",DISP_CENTER);
//     sprintf(strbuf,"%d",30);

	sprintf(strbuf, "%d", DevStat.purchase_hold_time);

	gs.qx = 0;		//提示语句显示坐标
	gs.qy = 6;

	gs.alpha = 0;	//输入模式标志是否允许输入字母、符号，为0不允许输入字母、符号，不为0则允许输入字母、符号
	gs.sFont = 0;	//标志显示字体大小，为 0 表示使用7x9的大字体， 不为 0 则表示使用5x7的小字体
	gs.pPrompt = "持续时间:";
	gs.csChar = '_';
	gs.pwChar = 0;
	gs.nullEnable = 0;
// 	gs.spChar = ',';
	gs.spChar = 0;
	gs.pStr = strbuf;			//	所输入的字符存储在缓冲区strbuf中
	gs.maxNum = 3;
	gs.minNum = 1;
	gs.minValue = 1;
	gs.maxValue = 255;
	if (DevStat.purchase_hold_time < 10)
	{
		gs.retNum = 1;			//初始显示1位
	}
	else if (DevStat.purchase_hold_time < 100)
	{
		gs.retNum = 2;			//初始显示2位
	}
	else if (DevStat.purchase_hold_time < 1000)
	{
		gs.retNum = 3;			//初始显示3位
	}
	else if (DevStat.purchase_hold_time < 10000)
	{
		gs.retNum = 4;			//初始显示4位
	}
	else
	{
		gs.retNum = 5;			//初始显示5位
	}
	gs.qEvent = KEY_F1 | KEY_POWER; // 默认F1退出
// 	sprintf(strbuf,"%d",30);

	ret = huGetStr_E(&gs);
	switch ( ret )
	{
		case 0:	//正确输入。 
			DevStat.purchase_hold_time = atol(strbuf);
			WriteParam();
			return 1;

		case 1:// 输入空字符串
			DispStr_CE(0,3,"输入有误",DISP_CENTER|DISP_CLRSCR);
			Beep(500);
			return -1;

		case -1://事件退出（字符串未正确输入）。
			break;

		default:
			break;          
	}
*/
//    return 1;
//}

// *****************************************************************
// 功能：		MaxPriceLimitSet
// 说明:		设置最大消费金额
// 入口参数：	
// 出口参数：	Null
// 作者：
// 返回值：							
// *****************************************************************
//INT8U MaxPriceLimitSet(void)
//{
//    INT8U i = 0;
//// 	INT8U  PriceSettingMenu[10*16 +2] =  {0};
//    INT8U buf[100];
//    int             ret;
//// 	int             cPtr=0;
//// 	int             lPtr=0;
//    INT8U disp_buf[10];
//    INT8U key = NO_KEY;
//    const INT8U position[6][2] =                                   //各位显示的位置
//    {
//        {4 * 8, 3},
//        {5 * 8, 3},
//        {6 * 8, 3},
//        {7 * 8, 3},
//        {9 * 8, 3},
//        {10 * 8, 3},
//    };
//    INT8S cur_ptr = 0;
//
//    ToggleWD();
//
//    DispStr_CE(0, 1, "最大消费金额设置", DISP_CLRSCR | DISP_CENTER);
//
//    sprintf(buf, "    %07.2f元   ", (FP64)DevStat.max_price_limit /100.0);
//    DispStr_CE(0, 3, buf, DISP_POSITION);
//
//
//    disp_buf[0] = ascii(DevStat.max_price_limit / 100000);                      //千位
//    disp_buf[1] = ascii((DevStat.max_price_limit % 100000) / 10000);    //百位
//    disp_buf[2] = ascii((DevStat.max_price_limit % 10000) / 1000);      //十
//    disp_buf[3] = ascii((DevStat.max_price_limit % 1000) / 100);            //个
//    disp_buf[4] = ascii((DevStat.max_price_limit % 100) / 10);              //毛
//    disp_buf[5] = ascii(DevStat.max_price_limit % 10);                          //分
//
//    cur_ptr = 0;
//    DispStr_CE(position[cur_ptr][0], position[cur_ptr][1], chtostr(disp_buf[0]), DISP_POSITION | DISP_REVERSE);
//
//
//    for ( ;; )
//    {
//        do
//        {                 //only accept 0-9,KEY_CANCEL, KEY_ENTER
//            key = get_key();
//        }while ( !( (key>='0'&&key<='9') || (key==EXIT_KEY_F1) || (key==EXIT_KEY_COMM) || (key==EXIT_KEY_ENT) ) );
//
//        switch ( key )
//        {
//            case EXIT_KEY_COMM:
//                break;
//
//            case EXIT_KEY_F1:
//                return notok;           //不修改时间，返回
//
//            case EXIT_KEY_ENT:
//                    DevStat.max_price_limit = (INT32U)ascii_to_hex(disp_buf[0]) * 100000 + (INT32U)ascii_to_hex(disp_buf[1]) * 10000
//                            + (INT32U)ascii_to_hex(disp_buf[2]) * 1000 + (INT32U)ascii_to_hex(disp_buf[3]) * 100
//                            + (INT32U)ascii_to_hex(disp_buf[4]) * 10 + (INT32U)ascii_to_hex(disp_buf[5]);
//                    WriteParam();
//                    return ok;
//
//            default:            //数字键处理
//                disp_buf[cur_ptr] = key;
//                DispStr_CE(position[cur_ptr][0], position[cur_ptr][1], chtostr(disp_buf[cur_ptr]), DISP_POSITION);
//                cur_ptr++;
//                if ( cur_ptr >= 6 )
//                {
//                    cur_ptr = 0;
//                }
//                DispStr_CE(position[cur_ptr][0], position[cur_ptr][1], chtostr(disp_buf[cur_ptr]), DISP_POSITION | DISP_REVERSE);
//// 				DispStr_CE(position[cur_ptr][0], position[cur_ptr][1], "_", DISP_POSITION);
//                break;
//        }
//    }
//
//
//    return ok;
//}


// *****************************************************************
// 功能：		PrintModeSelect
// 说明:		打印模式选择
// 入口参数：	
// 出口参数：	Null
// 作者：
// 返回值：							
// *****************************************************************
int PrintModeSelect()
{

//	SELE_INLINE KS_Setting;
	int ret;
	INT8U Print_menus[2*20 + 2] = {"1.自动              2.手动              "};
	

//  EA_vCls();
//  EA_vDisplay(1,"打印模式选择");
	ret = browse_menu(0, (unsigned char*)"", Print_menus, TRUE);
	switch ( ret )
	{
		case 0: // 选择自动
			DevStat.mode = 0x00;
			WriteParam();
			break;

		case 1: // 选择手动
			DevStat.mode = 0x01;
			WriteParam();
			break;

		default:
			return notok;
			break;
	}
	return ok;

}


// *****************************************************************
// 功能：		InitParam
// 说明:		恢复出厂设置
// 入口参数：	
// 出口参数：	Null
// 作者：
// 返回值：							
// *****************************************************************
INT8U InitParam()
{
//	int ret;
	INT16U  key;  
		 
	EA_ucClrKeyBuf();
	EA_vCls();
	EA_vDisplay(1, "   系统将恢复到出   ");
	EA_vDisplay(2, "   厂状态,所有记录  ");
	EA_vDisplay(3, "  和参数全部被清除  ");
	EA_vDisplay(4, "F1-确定    其他-取消");
	key = EA_uiInkey(0);

	switch ( key )
	{
		case EM_key_F1:	// 是
			EA_ucClrKeyBuf();
			EA_vCls();
			EA_vDisplay(1, "  系统正在初始化   ");
			EA_vDisplay(2, "     请等待....    ");
			EA_uiInkeyMs(2000);
			//初始化参数
//  		format_process();
			param_factory_default(1);

			(void)EA_vCls();
			EA_vDisplay(1, "   系统初始化完毕   ");
			EA_vDisplay(2, "    将会自动关机    ");
			EA_vDisplay(3, "    将会自动重启    ");
			EA_uiInkeyMs(2000);
			Beep(1000);
			System_reset();
//  		Power_Down();
//  		return ok;
		default: 
			return notok;
	}



}



// *****************************************************************
// 功能：		Sex_Select
// 说明:		性别选择
// 入口参数：	
// 出口参数：	Null
// 作者：
// 返回值：							
// *****************************************************************
//int Sex_Select()
//{
//
//    SELE_INLINE KS_Setting;
//    int ret;
//
//    EA_vCls();
//
//    DispStr_CE(0,0,"性别选择",DISP_CENTER|DISP_CLRSCR);
//
//    KS_Setting.count = 2;   //菜单条数
//    KS_Setting.line = 4;    //菜单行号
//    //KS_Setting.enable46 = 1; //使能4，6键。这里可以使用4，6键代替滑轮
//    //KS_Setting.flagch = 0;     //当前选择项的标志字符‘◆’
//
//    KS_Setting.defsele = 0; // 默认选择“开启”
//    KS_Setting.autoexit = DevStat.poweroff_time; // 30秒无按键自动退出
//
//    KS_Setting.selepstr[0].xx = 2;  //第一菜单位置
//    strcpy(KS_Setting.selepstr[0].pstr, "男" );
//    KS_Setting.selepstr[1].xx = 10; //第二菜单位置
//    strcpy( KS_Setting.selepstr[1].pstr, "女" );
//
//    CardInfo.sex = 0;
//
//    KS_Setting.qEvent = EXIT_KEY_F1 | EXIT_KEY_COMM|EXIT_KEY_POWER; // 按[C]键电源键或无按键自动退出
//    ret = select_in_line( &KS_Setting );
//    switch ( ret )
//    {
//        case 0: // 选择男
//            CardInfo.sex = 0x00;
//            break;
//
//        case 1: // 选择女
//            CardInfo.sex = 0x01;
//            break;
//
//        default:
//            return ret;
//            break;
//    }
//
//    return 1;
//
//}


// *****************************************************************
// 功能：		Id_Select
// 说明:		证件类型选择
// 入口参数：	
// 出口参数：	Null
// 作者：
// 返回值：	-1 关机							
// *****************************************************************
int Id_Select()
{
	int             ret;
	CardInfo.id_type = 1;

	//1:身份证
	//2:老年证
	//3:离休证
	//4:军官证
	//5:户口证
	//6:学生证
	for ( ; ; )
	{
		ret = browse_menu(0, (void *)"    证件类型    ", Id_Type, TRUE);
		switch ( ret )
		{
			case 0://身份证
			case EM_key_1:
				CardInfo.id_type = 1;
				return 0;
				break;

			case 1://老年证
			case EM_key_2:
				CardInfo.id_type = 2;
				return 1;
				break;

			case 2://离休证
			case EM_key_3:			
				CardInfo.id_type = 3;
				return 2;
				break;
			case 3://军官证
			case EM_key_4:			
				CardInfo.id_type = 4;
				return 4;
				break;
			case 4://户口证
			case EM_key_5:			
				CardInfo.id_type = 5;
				return 5;
				break;
			case 5://学生证
			case EM_key_6:			
				CardInfo.id_type = 6;
				return 6;
				break;
			case -1://EXIT_KEY_POWER,EXIT_AUTO_QUIT
//  			Halt_HD6305();
				return -1;

			case -5://返回上级菜单
				return -5;

			default:
				break;
		}
	}
}





