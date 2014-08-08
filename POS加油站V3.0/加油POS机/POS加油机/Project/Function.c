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
extern INT8U const    VERIFY_PIN[11];     //验证口令
extern INT8U const    READ_15_FILE[5];    //读取15文件内容
extern INT8U const    WRITE_15_FILE[5];   //读取15文件内容

extern	DevHandle  hSam1Handle;
extern	DevHandle  hCpuHandle;
extern  DevHandle hMifsHandle;      //M1卡

extern const char parmFileName[];
extern const char blackFileName[];
extern const char currecFileName[];
extern const char conrecFileName[];
extern const char hisrecFileName[];
extern const char grayrecFileName[];

// *****************************************************************
// 功能：		参数设置界面
// *****************************************************************
INT8U  parm_set[10*20 +2] = {"1.油价调整          2.油品设置          3.记录查询          4.设备号设置        5.站点号设置        6.密码修改          7.重打上次小票      8.IP地址设置        9.修改加油员卡密码  10.油价生效时间设定 "};
INT8U  gas_set[4*20 +2]   = {"1.天然气            2.汽油93#           3.汽油97#           4.柴油              "}; 
INT8U  rec_query[2*20 +2] = {"1.日结单查询        2.单笔记录查询      "}; 
// *****************************************************************
// 功能：		信息查询界面
// *****************************************************************

//系统超级变量
extern DEV_STAT     DevStat;					//设备状态
extern CARD_INFO    CardInfo;					//卡片信息，交易记录由此变量生成
extern ERROR_CARD_INFO ErrCardInfo;	//出错卡状态
extern INT8U sendbuf[1024];
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
// 功能：		PasswordCheck
// 说明:		密码检验
//				本系统设有超级密码和用户密码
// 入口参数：	
// 出口参数：	Null
// 作者：
// 返回值： -1 关机 						
// *****************************************************************
int PasswordCheck(void)
{
	uchar i = 0;
	INT8U input[22];

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
int PasswordSetting(void)
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
		for ( ;; )
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

	switch ( PasswordCheck() )
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
		   case 0://油价设置
 			    price_Setting();
				break;	
		   case 1://油品设置
 			    select_gas_oil_Setting();
				break;
		   case 2://交易记录查询
			    query_note_and_record();
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
		   case 6:  //重打上次小票
			    Reprint_sale_record();
			    break;
		   case 7://APN设置
			   if(GPRS_Input_IP() == ok)
			   {
				   EA_vCls();
				   EA_vDisplay(3, "   IP地址修改成功   ");
				   EA_uiInkeyMs(2000);
			   }
			   break;
		   case 8:
			   oper_code_setting();
			   break;
		   case 9:
//  		   DateTime_Setting(void);
			   break;

		   case -1://返回上级菜单
				return -1;

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
	char    buf[50];
	int     i;   

	EA_vCls();
	EA_vDisplay(1, (void *)"设备ID:%02X%02X%02X",DevStat.equ_id[0], DevStat.equ_id[1], DevStat.equ_id[2]);
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

	printf_debug((void *)"   设备ID设置成功   ");

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

	sprintf((void *)str, "站点号:%02X%02X", DevStat.acnt_id[0], DevStat.acnt_id[1]);

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

	printf_debug((void *)"   站点号设置成功   ");

	return ok;
}
////////////////////////////////////////////////////////////
//APN接入点选择
////////////////////////////////////////////////////////////
INT8U SAMCompSelect(void)
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
// 功能：		GPRS_Input_IP
// 说明:		手工输入IP地址
// 入口参数：	
// 出口参数：	Null
// 作者：
// 返回值：	-1 关机							
// *****************************************************************
//void GPRS_Input_IP(void)
//{
//    INT8U buf[16], IP_addr_1[16], i, j;
//    INT8U iTmp[4], iport;
//    INT8U IP_addr[16];
////	IP_addr[16] = "219.133.55.78", Comm_port[16] = "7455"
//
//IP_AGAIN:
//    memset(IP_addr, 0, sizeof(IP_addr));
//    memset(IP_addr_1, 0, sizeof(IP_addr));
//
//    EA_vCls();
//    (void)EA_ucClrKeyBuf();
//    j = 0;
//    EA_vDisplay(1, "原IP地址为:");
//    EA_vDisplay(2, "请输入新IP地址:");
//
//    sprintf(IP_addr, "%03d.%03d.%03d.%03d",DevStat.ServerIp[0], DevStat.ServerIp[1],
//            DevStat.ServerIp[2], DevStat.ServerIp[3]);
//    EA_vDisplay(3, IP_addr);
//
//    memcpy(IP_addr_1, "   .   .   .   ", 15);
//    IP_addr_1[15] = '\0';
////  if (ReadNumeric_(IP_addr_1, 15, 0, 2) < 0)
////  	return;
//
//    for ( ;; )
//    {
//        i = EA_ucGetInputStr(3, 1, 20, EM_BIG_FONT | EM_MODE_NUMBER | EM_ALIGN_LEFT | EM_SHOW_PASSWORD
//                             , 7, 15, 0, (void *)IP_addr_1);
//        if ( i == EM_SUCCESS )
//            break;
//        if ( i == EM_ABOLISH )
//            return ;
//    }
//
//    for(i = 0; i < 4; i++)
//    {
//        memcpy(buf, IP_addr_1 + i * 4, 3);
//        buf[3] = '\0';
//        iTmp[i] = atoi(buf);
//        if(atoi(buf) > 255)
//        {
//            lcddisperr("IP地址输入有误");
//            goto IP_AGAIN;
//        }
//    }
//
//    for(i = 0; i < 4; i++)
//    {
//        DevStat.ServerIp[i] = iTmp[i];
//    }
////  DevStat.ServerPort[0]=0x90;
////  DevStat.ServerPort[1]=0x00;
//
//    memset(IP_addr, '\0', 16);
//    sprintf(IP_addr, "%d.%d.%d.%d", iTmp[0], iTmp[1], iTmp[2], iTmp[3]);
//    EA_vDisplay(3, IP_addr);
//
//PORT_AGAIN:
//    EA_vDisplay(4, "请输入端口号:");
//    memcpy(IP_addr_1, "     ", 5);
//    IP_addr_1[5] = '\0';
////  if (ReadNumeric_(IP_addr_1, 5, 24, 2) < 0)
////  	return;
//    for ( ;; )
//    {
//        i = EA_ucGetInputStr(13, 1, 20, EM_BIG_FONT | EM_MODE_NUMBER | EM_ALIGN_LEFT | EM_SHOW_PASSWORD
//                             , 1, 5, 0, (void *)IP_addr_1);
//        if ( i == EM_SUCCESS )
//            break;
//        if ( i == EM_ABOLISH )
//            return ;
//    }
//
//    iport = atoi(IP_addr_1);
//    if( iport > 65535 )
//    {
//        lcddisperr("端口号输入有误");
//        goto PORT_AGAIN;
//    }
////	memset(Comm_port, '\0', 16);
////	sprintf(Comm_port, "%d", iport);
//    DevStat.ServerPort[0] = (INT8U)(iport>>8);
//    DevStat.ServerPort[1] = (INT8U)iport;
//
//    WriteParam();
//}


// *****************************************************************
// 功能：		Price_Setting
// 说明:		加油单价设置
// 入口参数：	无
// 出口参数：	Null
// 作者：    frank
// 返回值：	NULL							
// *****************************************************************
void price_Setting(void)
{
	int     ret;
	char    input[22], buf[22];
	uchar   i;
	double  temp_price;
	INT8U   price_menu[2*20 + 2] = {"1.自动              2.手动              "};
	INT16U  key = EM_key_NOHIT;
	INT32U  tmp_price[4] = {0, 0, 0, 0};
	//uint    sendbuf_len = 0;

	memset(buf, 0, sizeof(buf));
	memcpy(tmp_price, (INT8U *)&DevStat.price[0], 16);

	ret = browse_menu(0, (unsigned char*)"", price_menu, TRUE);
	switch ( ret )
	{
	    case 0:	// 自动更新

			if(download_price_table() == ok)
			{
				//lcddisperr("更新油价成功");
				GPRS_Close();
			}
			else
			{
				;
			}
			break;

		case 1:	// 手动设置
			EA_ucClrKeyBuf();
			switch ( PasswordCheck() )
			{
				case notok:
					printf_debug((void *)"密码错误");
					return ;
				case 2:
					return ;
				case ok:
					EA_ucClrKeyBuf();
					break;
				default:
					return ;
			}
			//当前油价显示
			EA_vCls();
			EA_ucClrKeyBuf();
			strcpy((void *)input, "");
reboot:
			EA_vDisplay(1, "天然气 :%2.2f元/立方", tmp_price[0]/100.0);
			EA_vDisplay(2, "汽油93#:%2.2f元/升 ",  tmp_price[1]/100.0);
			EA_vDisplay(3, "汽油97#:%2.2f元/升 ",  tmp_price[2]/100.0);
			EA_vDisplay(4, "柴  油 :%2.2f元/升 ",  tmp_price[3]/100.0);
			EA_vDisplay(5, "按数字键改相应的油价");

			key = EA_uiInkey(0);
			switch ( key )
			{
			    case EM_key_1:
					EA_vCls();
					EA_ucClrKeyBuf();
					EA_vDisplay(1, "天然气 :%2.2f元/立方", tmp_price[0]/100.0);
					EA_vDisplay(2, "调整价 :%2.2f元/立方", 00.00);
					for ( ;; )
					{
						i = get_input_str(2, 11, 1, 6, (void *)input);
						if ( i == EM_SUCCESS )
							break;
						if ( i == EM_ABOLISH )
							break ;
					}

					sscanf((void *)input, "%lf", &temp_price);
					tmp_price[0] = temp_price * 100 + 0.001;
				   	break;

				case EM_key_2:
					EA_vCls();
					EA_ucClrKeyBuf();
					EA_vDisplay(1, "汽油93#:%2.2f元/升 ", tmp_price[1]/100.0);
					EA_vDisplay(2, "调整价 :%2.2f元/升 ", 00.00);
					for ( ;; )
					{
						i = get_input_str(2, 11, 1, 6, (void *)input);
						if ( i == EM_SUCCESS )
							break;
						if ( i == EM_ABOLISH )
							break ;
					}
					sscanf((void *)input, "%lf", &temp_price);
					tmp_price[1] = temp_price * 100 + 0.001;
				    break;

				case EM_key_3:
					EA_vCls();
					EA_ucClrKeyBuf();
					EA_vDisplay(1, "汽油97#:%2.2f元/升 ", tmp_price[2]/100.0);
					EA_vDisplay(2, "调整价 :%2.2f元/升 ", 00.00);
					for ( ;; )
					{
						i = get_input_str(2, 11, 1, 6, (void *)input);
						if ( i == EM_SUCCESS )
							break;
						if ( i == EM_ABOLISH )
							break ;
					}

					sscanf((void *)input, "%lf", &temp_price);

					tmp_price[2] = temp_price * 100 + 0.001;
					
				   break;

				case EM_key_4:
					EA_vCls();
					EA_ucClrKeyBuf();
					EA_vDisplay(1, "柴  油 :%2.2f元/升 ", tmp_price[3]/100.0);
					EA_vDisplay(2, "调整价 :%2.2f元/升 ", 00.00);

					for ( ;; )
					{
						i = get_input_str(2, 11, 1, 6, (void *)input);
						if ( i == EM_SUCCESS )
							break;
						if ( i == EM_ABOLISH )
							break;
					}

					sscanf((void *)input, "%lf", &temp_price);
					tmp_price[3] = temp_price * 100 + 0.001;

				     break;

				case EM_key_ENTER:
					
					ret = upload_price_log(tmp_price);
					if(ret != ok)
					{
						lcddisperr("更新油价失败");
						GPRS_Close();
						break;
					}

					DevStat.price[0] = tmp_price[0];
					DevStat.price[1] = tmp_price[1];
					DevStat.price[2] = tmp_price[2];
					DevStat.price[3] = tmp_price[3];

					lcddisperr("更新油价成功");
					WriteParam();
					GPRS_Close();
					return;
				default: 
					break;
				case EM_key_EXIT:
					return;
				
			}
			goto reboot;
		default: 
			return ;
		//	break;
	}
	return ;
}
// *****************************************************************
// 功能：		Information_Query
// 说明:		参数设置菜单
// 入口参数：	
// 出口参数：	Null
// 作者：
// 返回值：	-1 关机							
// *****************************************************************
// *****************************************************************
// 功能：		Datetime_Setting
// 说明:		日期时间设置
// 入口参数：	
// 出口参数：	Null
// 作者：
// 返回值：							
// *****************************************************************
int oper_code_setting(void)
{
	uchar 	  i = 0;
	INT8U	  input1[22], input2[22];					
	INT8U     block_buffer1[16] = {0};			//M1卡一个block的数据缓冲
	INT8U     block_buffer2[16] = {0};			//M1卡一个block的数据缓冲
	INT16U    uckey = 0;
	uint      uikey = 0;
	INT8U     KeySector[2][6];	

	EA_vCls();

	while(1)
	{
		EA_vDisplay(2, (void *)"    请刷加油员卡    ");
		if( CardInit() == ok )
			break;

		uckey = EA_ucKBHit();
		if ( uckey ==  EM_key_HIT )		 //按下取消键
		{
			uikey = EA_uiInkey(0);
			if ( uikey == EM_key_CANCEL )		 //按下取消键
			{
				return ok;
			}
		}
	}
	
	////////////////////////////////////////
	//读Sector 1 中4 区信息
	////////////////////////////////////////
	memcpy(&KeySector[1][0], &CardInfo.csn[0], 4);
	KeySector[1][4] = ~KeySector[1][0];
	KeySector[1][5] = ~KeySector[1][1];

	i = Get_Sector_PermissionA(1, &KeySector[1][0]);
	if ( i != ok )
	{
		lcddisperr((char *)"请重新刷卡");
		return notok;
	}
		
	i = EA_ucMIFRead(hMifsHandle, 4, block_buffer1);
	if ( i != ok )
	{
		lcddisperr((char *)"请重新刷卡");
		return notok;
	}
		
	if ( block_buffer1[10] != 0x01 )	  //判断启用标志
	{
		//卡未启用处理
		lcddisperr((char *)"卡片未启用");
		Beeperr();
		return ok;
	}

	memcpy(&CardInfo.driver_number[0], &block_buffer1[1], 3);							 //加油员卡号																				 //
//  memcpy(&CardInfo.authentication_code[0], &block_buffer1[4], 4);						 //加油员密码
	CardInfo.card_type = block_buffer1[11];											     //卡类

	if (CardInfo.card_type != 0xBB)                 //只允许普通卡消费debug  by frank
	{
		lcddisperr((void *)"不是加油员卡!");
		return notok;
	}

	EA_vCls();
	EA_vDisplay(1, "加 油 员:%02X%02X%02X", CardInfo.driver_number[0],CardInfo.driver_number[1],CardInfo.driver_number[2]);
	EA_vDisplay(2, "输新入密码:");
	EA_vDisplay(3, "再输入一次:");
	EA_vDisplay(4, "   请勿移动卡片   ");
	strcpy((void *)input1, "");
	strcpy((void *)input2, "");

	for ( ;; )
	{
		i = EA_ucGetInputStr(2, 12, 20, EM_BIG_FONT | EM_MODE_NUMBER | EM_ALIGN_LEFT | EM_SHOW_PASSWORD
							 , 8, 8, 0, (void *)input1);
		if ( i == EM_SUCCESS )
			break;
		if ( i == EM_ABOLISH )
			return 2;
	}
	//再输入一次
	for ( ;; )
	{
		i = EA_ucGetInputStr(3, 12, 20, EM_BIG_FONT | EM_MODE_NUMBER | EM_ALIGN_LEFT | EM_SHOW_PASSWORD
							 , 8, 8, 0, (void *)input2);
		if ( i == EM_SUCCESS )
			break;
		if ( i == EM_ABOLISH )
			return 2;
	}

	if ( memcmp((void *)input1, input2, 8) == 0 )
	{
		//lcddisperr("密码修改成功");
		//return ok;
	}
	else
	{
		EA_ucSetInverse(EM_lcd_INVON);
		lcddisp(4, 1, (void *)"  两次密码输入不同  ");
		EA_ucSetInverse(EM_lcd_INVOFF);
		(void)EA_uiInkey(1);
		return notok;
	} 

//  i = Get_Sector_PermissionA(1, &KeySector[1][0]);
//  if ( i != ok )
//  {
//  	lcddisperr((char *)"写密钥验证失败");
//  	return notok;
//  }
	var_asc2bcd(&block_buffer1[4], &input1[0], 8);
//  block_buffer1[11] = 0xBB;
//  block_buffer1[5] = a_to_h(input1[1]);
//  block_buffer1[6] = a_to_h(input1[2]);
//  block_buffer1[7] = a_to_h(input1[3]);

	memcpy(block_buffer2, block_buffer1, 16);

	i = Sec0_Block_Is_Legal(block_buffer2);
	if ( (i == ok) )
	{
	    if(EA_ucMIFWrite (hMifsHandle, 4, block_buffer2) == ok)
		lcddisperr("密码修改成功");
		EA_ucMIFHalt(hMifsHandle);	//挂卡
		return ok;
	}
	lcddisperr("密码修改失败");
	EA_ucMIFHalt(hMifsHandle);	//挂卡
	return notok;
}


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
	   case 3:
		   DevStat.fill_mode = 3;
		   break;
	   default:
		   break;
	}

	WriteParam();

	return ok;
}

// *****************************************************************
// 功能：		Record_Query
// 说明:		记录查询
// 入口参数：	
// 出口参数：	Null
// 作者：
// 返回值：							
// *****************************************************************
int Record_Query(void)
{
	int         num;
	INT32U      RecordCount = 0;
	INT32U      Count = 0;
	uchar       uckey; 
	uint        uikey; 
	uchar 		ucOpenID=0;
	INT8U 		rec[57];

	EA_vCls();
	EA_ucClrKeyBuf();

	memset(rec, 0, sizeof(rec));
////////////////////////////////////////////////////////
//    if ( EA_ucPFOpen((uchar *)currecFileName, &ucOpenID) != EM_ffs_SUCCESS )
//    {
//        lcddisperr("打开记录文件失败");
//        (void)EA_ucPFClose(ucOpenID);
//        return notok;
//    }
//    if ( EA_ucPFInfo(ucOpenID, EM_ffs_VALIDREC, &uiRetNum) != EM_ffs_SUCCESS )
//    {
//        lcddisperr("读取记录文件失败");
//        (void)EA_ucPFClose(ucOpenID);
//        return notok;
//    }
//
//    Num_rec = uiRetNum;
//    (void)EA_ucPFClose(ucOpenID);
//
//    EA_vDisplay(1, "系统存储记录信息查询");
//    EA_vDisplay(2, "交易记录有:%6d条", Num_rec);

	if ( EA_ucPFOpen( (uchar *)hisrecFileName, &ucOpenID) != EM_ffs_SUCCESS )
	{
		lcddisperr("打开历史文件文件失败");
		return 0;
	}
	// 获取有效信息条数
	(void)EA_ucPFInfo(ucOpenID, EM_ffs_VALIDREC, &RecordCount);
	if( RecordCount == 0)
	{
		lcddisperr("没有交易信息");
		(void)EA_ucPFClose(ucOpenID);
		return 0;
	}

	num = RecordCount;//最后一条		

	if(EA_ucPFReadRec(ucOpenID, num, 1, &Count, rec) != EM_SUCCESS)
	{
		(void)EA_ucPFClose(ucOpenID);
		lcddisperr((void *)"读取不符合");
		return 0;
	}

	AnalyAndDisplay((RECORD *)rec);   //加油记录查询

	while (1)
	{
		uckey = EA_ucKBHit();
		if(uckey == EM_key_HIT)    //有按键
		{
			uikey = EA_uiInkey(0);    //读取按键键值
			if ( (uikey == EM_key_EXIT) || (uikey == EM_key_CANCEL) )			//退出
			{
				(void)EA_ucPFClose(ucOpenID);
				return 1;
			}

			switch ( uikey )
			{
				case EM_key_LETTER:	                //	上滚动一行
					num --;
					if ( num <= 0 )                 //记录超限
					{
						lcddisperr("已经是第一条");
						num = 1;
					}
					break;

				case EM_key_INTERPUNCTION:	        //	下滚动一行
					num ++;
					if ( num >= RecordCount+1 )
					{
						lcddisperr("已经是最后一条");
						num = RecordCount;
					}
					break;
				case EM_key_ENTER:
					break;
				default:
					break;
			}

			if ( (uikey == EM_key_INTERPUNCTION) || (uikey == EM_key_LETTER)  )
			{
				if(EA_ucPFReadRec(ucOpenID, num, 1, &Count, rec) != EM_SUCCESS)
				{
					(void)EA_ucPFClose(ucOpenID);
					lcddisperr((void *)"读取不符合");
					return 0;
				}
				AnalyAndDisplay((RECORD *)rec);
			}

			if(uikey == EM_key_ENTER)
			{
				print_record((RECORD *)rec);
				(void)EA_ucPFClose(ucOpenID);
				return 1;
			}			
		}
	}
}


//尼娟 SHENGH:   户籍：   和谁 我的于2014登记结婚初婚，   户口2012-1244在河南，  u户口再次期间没有是生养孩子，，村和镇，乡
//// *****************************************************************日期： 地址：  电话：
//// 功能：		AnalyAndDisplay
//// 说明:		查询记录表，并实现交易记录
//// 入口参数：
//// 出口参数：	Null
//// 作者：
//// 返回值：
//// *****************************************************************
void AnalyAndDisplay(RECORD * strTradeInfo)
{
	char     strbuf[22];
	double    fBuf = 0;
	INT8U    temp_buf[22];
	RECORD * prec = (RECORD *)strTradeInfo;

	memset(strbuf, 0, sizeof(strbuf));

	EA_ucClrKeyBuf();

	//显示卡片信息
	EA_vCls();
	memcpy(temp_buf, &prec->bus_number[0], 5);
	strcpy((void *)strbuf, (void *)"鲁C-");
	strcat((void *)strbuf, (void *)temp_buf);
	EA_vDisp(1, 1, strbuf);

	switch ( prec->fuel_type )
	{
		case 0:
			sprintf((void *)strbuf, "天燃气");//检索卡片类型信息
			EA_vDisp(1,12, strbuf);

			fBuf    = (double)(prec->fare)/100.0;
			sprintf((void *)strbuf, "明细:%4.2lf元", fBuf);
			EA_vDisp(2,1, strbuf);
			fBuf    = (double)(prec->capacity)/100.0;
			sprintf((void *)strbuf, "%3.2lf方", fBuf);
			EA_vDisp(2,14, strbuf);
			break;
		case 1:
			sprintf((void *)strbuf, "汽油93#");//检索卡片类型信息
			EA_vDisp(1,12, strbuf);

			fBuf    = (double)(prec->fare)/100.0;
			sprintf((void *)strbuf, "明细:%4.2lf元", fBuf);
			EA_vDisp(2,1, strbuf);
			fBuf    = (double)(prec->capacity)/100.0;
			sprintf((void *)strbuf, "%3.2lf升", fBuf);
			EA_vDisp(2,14, strbuf);
			break;
		case 2:
			sprintf((void *)strbuf, "汽油97#");//检索卡片类型信息
			EA_vDisp(1,12, strbuf);

			fBuf    = (double)(prec->fare)/100.0;
			sprintf((void *)strbuf, "明细:%4.2lf元", fBuf);
			EA_vDisp(2,1, strbuf);
			fBuf    = (double)(prec->capacity)/100.0;
			sprintf((void *)strbuf, "%3.2lf升", fBuf);
			EA_vDisp(2,14, strbuf);
			break;
		case 3:
			sprintf((void *)strbuf, "柴油");//检索卡片类型信息
			EA_vDisp(1,12, strbuf);

			fBuf    = (double)(prec->fare)/100.0;
			sprintf((void *)strbuf, "明细:%4.2lf元", fBuf);
			EA_vDisp(2,1, strbuf);
			fBuf    = (double)(prec->capacity)/100.0;
			sprintf((void *)strbuf, "%3.2lf升", fBuf);
			EA_vDisp(2,14, strbuf);
			break;

		default:
			break;

	}
	htoa(temp_buf, (void *)&prec->card_in_time, 6);
	strcpy((void *)strbuf, (void *)"日期:");
	strcat((void *)strbuf, (void *)temp_buf);
	EA_vDisp(3, 1, strbuf);

	htoa2(temp_buf, (void *)&prec->pos_purchase_serial_num[0], 3);

	strcpy((void *)strbuf, (void *)"交易序号:");
	strcat((void *)strbuf, (void *)temp_buf);
	EA_vDisp(4, 1, strbuf);

}


// *****************************************************************
// 功能：		Note_Query
// 说明:		日结单查询
// 入口参数：	
// 出口参数：	Null
// 作者：
// 返回值：							
// *****************************************************************
int Note_Query(void)
{
	int         num;
	INT32U      RecordCount = 0;
	INT32U      Count = 0;
	uchar       uckey; 
	uint        uikey; 
	uchar 		ucOpenID=0;
	INT8U 		rec[100];

	EA_vCls();
	EA_ucClrKeyBuf();

	memset(rec, 0, sizeof(rec));

	if ( EA_ucPFOpen( (uchar *)conrecFileName, &ucOpenID) != EM_ffs_SUCCESS )
	{
		lcddisperr("打开历史文件文件失败");
		return 0;
	}
	// 获取有效信息条数
	(void)EA_ucPFInfo(ucOpenID, EM_ffs_VALIDREC, &RecordCount);
	if( RecordCount == 0)
	{
		lcddisperr("没有日结信息");
		(void)EA_ucPFClose(ucOpenID);
		return 0;
	}

	num = RecordCount;//最后一条		

	if(EA_ucPFReadRec(ucOpenID, num, 1, &Count, rec) != EM_SUCCESS)
	{
		(void)EA_ucPFClose(ucOpenID);
		lcddisperr((void *)"读取不符合");
		return 0;
	}

	AnalyAndDisplayNote((INT8U *)rec);   //加油记录查询

	while ( 1 )
	{
		uckey = EA_ucKBHit();
		if(uckey == EM_key_HIT)    //有按键
		{
			uikey = EA_uiInkey(0);    //读取按键键值
			if ( (uikey == EM_key_EXIT) || (uikey == EM_key_CANCEL) )			//退出
			{
				(void)EA_ucPFClose(ucOpenID);
				return 1;
			}

			switch ( uikey )
			{
				case EM_key_LETTER:	                //	上滚动一行
					num --;
					if ( num <= 0 )                 //记录超限
					{
						lcddisperr("已经是第一条");
						num = 1;
					}
					break;

				case EM_key_INTERPUNCTION:	        //	下滚动一行
					num ++;
					if ( num >= RecordCount+1 )
					{
						lcddisperr("已经是最后一条");
						num = RecordCount;
					}
					break;
				case EM_key_ENTER:
					break;
				default:
					break;
			}

			if ( (uikey == EM_key_INTERPUNCTION) || (uikey == EM_key_LETTER)  )
			{
				if(EA_ucPFReadRec(ucOpenID, num, 1, &Count, rec) != EM_SUCCESS)
				{
					(void)EA_ucPFClose(ucOpenID);
					lcddisperr((void *)"读取不符合");
					return 0;
				}
				AnalyAndDisplayNote((INT8U *)rec);
			}

			if(uikey == EM_key_ENTER)
			{
				print_work_note_his(rec);
				(void)EA_ucPFClose(ucOpenID);
				return 1;
			}			
		}
	}
}


