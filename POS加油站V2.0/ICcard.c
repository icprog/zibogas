/****************************************************************
Copyright 2012 	�����״�ͨ���Ӽ������޹�˾
All rights riserved.
�� �� ���� ICcard.c
ģ�����ƣ� E550 ˢ���Ϳ�����
���ܸ�Ҫ�� ������
*****************************************************************/
#include <ICCARD.H>


/******************************************************************************
 �������ƣ�ICCardProcess
 ��������������������
 ����������
 �������ƣ�	����/�����	����		����
 
 ��  ��  ֵ��ok(0)-�ɹ�
				 notok(0xFF)-ʧ��
				   
 ��      ��	������
 ��      �ڣ�2004-09-02
 �޸���ʷ��
		����		�޸���		�޸�����
		2005.2.18		myron	                   in function 
******************************************************************************/
//INT8U ICCardProcess(void)
//{
//    //  INT8U LOCAL_V_TYPE  j =0;
//    INT8U LOCAL_V_TYPE  i = 0;
//    INT8U temp_buf[50];
//    INT8U LOCAL_V_TYPE  kind_of_card;                           //�����࣬CPU or M1
//                                                                //  INT8U csn[4];
//
//    ToggleWD();
//
//    if ( DevStat.allow_upload_data == TRUE )                    //�ɼ�״̬������ˢ��
//        return notok;
//
//    i = Request(0, temp_buf);                //request idle
//    if ( i != MI_OK )
//        return notok;
//    kind_of_card = temp_buf[0];             //ȡ�ÿ�����
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
//    //S50 ����IC��
//    else if ( ((temp_buf[0] & 0x08) == 0x08) && (kind_of_card ==  0x04) )
//    {
//        i = M1CardProcess();
//    }
//    //S70 ����ͨ��
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
//    //  (void)ComCreateConnctn(115200, CHANNL_DATA_BOX_TR);         //�������ӣ�������115200��ͨ��Ϊ���ݺ��շ�ͨ��
//
//    if ( i != ok )
//        return notok;
//
//    DisplayPurchaseResult(&CardInfo);   //��ʾ���׽��
//
//    StoreRecord(&CardInfo);
//
//    SoundPurchaseResult(); //������ʾ���׽��
//
//    /////////////////////////////////////
//    //ˢ�����ÿ���ȴ����복��
//    /////////////////////////////////////
//    if ( CardInfo.card_type == CARDT_SET
//         && CardInfo.card_purchase_type == MANAGER_CONSUME_TYPE )
//    {
//        Set_Bus_Number_Process();
//        Beep(500);      //�����공�ź����������ʾ
//    }
//
//    if ( CardInfo.card_purchase_type == DRIVER_WORKING_TYPE || CardInfo.card_purchase_type == DRIVER_REST_TYPE) //˾�����ϰ���°࣬��ʱ�������ͬ��ʱ��
//    {
//        DevStat.update_time = TRUE;
//    }
//
//    //�ָ�״̬
//    memset(&CardInfo, 0x00, sizeof(CardInfo));          //�忨Ƭ������Ϣ
//    memset(&ErrCardInfo, 0x00, sizeof(ErrCardInfo));    //�������Ϣ
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
//    DevStat.purchase_again_req = FALSE;                 //������ˢ��������־
//    DevStat.voice_purchase_again_freq = FALSE;
//    halt_req = TRUE;
//
//    return ok;
//}

