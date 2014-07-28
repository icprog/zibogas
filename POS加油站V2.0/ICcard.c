/****************************************************************
Copyright 2012 	大连易达通电子技术有限公司
All rights riserved.
文 件 名： ICcard.c
模块名称： E550 刷加油卡程序
功能概要： 主程序
*****************************************************************/
#include <ICCARD.H>


/******************************************************************************
 函数名称：ICCardProcess
 功能描述：卡交易流程
 参数描述：
 参数名称：	输入/输出？	类型		描述
 
 返  回  值：ok(0)-成功
				 notok(0xFF)-失败
				   
 作      者	：许岩
 日      期：2004-09-02
 修改历史：
		日期		修改人		修改描述
		2005.2.18		myron	                   in function 
******************************************************************************/
//INT8U ICCardProcess(void)
//{
//    //  INT8U LOCAL_V_TYPE  j =0;
//    INT8U LOCAL_V_TYPE  i = 0;
//    INT8U temp_buf[50];
//    INT8U LOCAL_V_TYPE  kind_of_card;                           //卡种类，CPU or M1
//                                                                //  INT8U csn[4];
//
//    ToggleWD();
//
//    if ( DevStat.allow_upload_data == TRUE )                    //采集状态不允许刷卡
//        return notok;
//
//    i = Request(0, temp_buf);                //request idle
//    if ( i != MI_OK )
//        return notok;
//    kind_of_card = temp_buf[0];             //取得卡种类
//
//    i = Anticoll(ANTICOLL1, 0, (void *)&CardInfo.csn[0]);
//    if ( i != MI_OK )
//        return notok;
//
//    i = Select(ANTICOLL1, (void *)&CardInfo.csn[0], temp_buf);
//    if ( i != MI_OK )
//        return notok;
//
//
//    //  (void)LoadTypeBParamtoRam(&DevStat.typea_para_position, TYPEA_PARA_POSITION, sizeof(DevStat.typea_para_position));
//    //  (void)LoadTypeAParamtoRam(&DevStat.record_number, DevStat.typea_para_position, sizeof(DevStat.record_number));
//    //  (void)LoadTypeBParamtoRam(&DevStat.record_base_ptr, RECORD_BASE_PTR, sizeof(DevStat.record_base_ptr));
//
//    if ( ((temp_buf[0] == 0x20) && (kind_of_card == CPU_CARD)) || temp_buf[0] == 0x28 )
//    {
//        i = CpuCardProcess();
//    }
//    //S50 公交IC卡
//    else if ( ((temp_buf[0] & 0x08) == 0x08) && (kind_of_card ==  0x04) )
//    {
//        i = M1CardProcess();
//    }
//    //S70 城市通卡
//    else if ( ((temp_buf[0] & 0x08) == 0x08) && (kind_of_card ==  0x02) )
//    {
////  	return notok;
////  	DevStat.cur_psam_channel=PSAM_SLV;
////  	i = M1Card_Fuzha_Process();
//        i = M1CardProcess();
//    }
//    else
//    {
////  	return notok;
//        i = CpuCardProcess();
//    }
//
//    //  (void)ComCreateConnctn(115200, CHANNL_DATA_BOX_TR);         //建立连接，波特率115200，通道为数据盒收发通道
//
//    if ( i != ok )
//        return notok;
//
//    DisplayPurchaseResult(&CardInfo);   //显示交易结果
//
//    StoreRecord(&CardInfo);
//
//    SoundPurchaseResult(); //语音提示交易结果
//
//    /////////////////////////////////////
//    //刷完设置卡后等待输入车号
//    /////////////////////////////////////
//    if ( CardInfo.card_type == CARDT_SET
//         && CardInfo.card_purchase_type == MANAGER_CONSUME_TYPE )
//    {
//        Set_Bus_Number_Process();
//        Beep(500);      //设置完车号后给蜂鸣器提示
//    }
//
//    if ( CardInfo.card_purchase_type == DRIVER_WORKING_TYPE || CardInfo.card_purchase_type == DRIVER_REST_TYPE) //司机卡上班或下班，此时与服务器同步时间
//    {
//        DevStat.update_time = TRUE;
//    }
//
//    //恢复状态
//    memset(&CardInfo, 0x00, sizeof(CardInfo));          //清卡片交易信息
//    memset(&ErrCardInfo, 0x00, sizeof(ErrCardInfo));    //清出错卡信息
//
//    if ( DevStat.long_disp_dly == TRUE )
//    {
//        DevStat.long_disp_dly = FALSE;
//        DispPurchaseResultDelay = DISP_DELAY * 4UL;
//    }
//    else
//    {
//        DispPurchaseResultDelay = DISP_DELAY;
//    }
//    DevStat.purchase_again_req = FALSE;                 //清重新刷卡语音标志
//    DevStat.voice_purchase_again_freq = FALSE;
//    halt_req = TRUE;
//
//    return ok;
//}

