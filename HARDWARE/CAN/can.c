#include "main.h"

void CAN1_Mode_Init(void)
{

    GPIO_InitTypeDef       GPIO_InitStructure;
    CAN_InitTypeDef        CAN_InitStructure;
    CAN_FilterInitTypeDef  CAN_FilterInitStructure;
    NVIC_InitTypeDef       NVIC_InitStructure;

    //使能相关时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能PORTA时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);//使能CAN1时钟

    //初始化GPIO
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11| GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
    GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化PA11,PA12

    //引脚复用映射配置
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource11,GPIO_AF_CAN1); //GPIOA11复用为CAN1
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource12,GPIO_AF_CAN1); //GPIOA12复用为CAN1

    //CAN单元设置
    CAN_InitStructure.CAN_TTCM=DISABLE;	//非时间触发通信模式
    CAN_InitStructure.CAN_ABOM=DISABLE;	//软件自动离线管理
    CAN_InitStructure.CAN_AWUM=DISABLE;//睡眠模式通过软件唤醒(清除CAN->MCR的SLEEP位)
    CAN_InitStructure.CAN_NART=DISABLE;	//禁止报文自动传送
    CAN_InitStructure.CAN_RFLM=DISABLE;	//报文不锁定,新的覆盖旧的
    CAN_InitStructure.CAN_TXFP=DISABLE;	//优先级由报文标识符决定
    CAN_InitStructure.CAN_Mode=CAN_Mode_Normal;	 //模式设置
    CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;	//重新同步跳跃宽度(Tsjw)为tsjw+1个时间单位 CAN_SJW_1tq~CAN_SJW_4tq
    CAN_InitStructure.CAN_BS1=CAN_BS1_9tq; //Tbs1范围CAN_BS1_1tq ~CAN_BS1_16tq
    CAN_InitStructure.CAN_BS2=CAN_BS2_4tq;//Tbs2范围CAN_BS2_1tq ~	CAN_BS2_8tq
    CAN_InitStructure.CAN_Prescaler=3;  ////CAN BaudRate 42/(1+9+4)/3=1Mbps
    CAN_Init(CAN1, &CAN_InitStructure);   // 初始化CAN1

    //配置过滤器
    CAN_FilterInitStructure.CAN_FilterNumber=0;	  //指定了待初始化的过滤器0
    CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask; //指定了过滤器将被初始化到的模式为标识符屏蔽位模式
    CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; //给出了过滤器位宽1个32位过滤器
    CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;//用来设定过滤器标识符（32位位宽时为其高段位，16位位宽时为第一个）
    CAN_FilterInitStructure.CAN_FilterIdLow=0x0000; //用来设定过滤器标识符（32位位宽时为其高段位，16位位宽时为第一个）
    CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;//用来设定过滤器屏蔽标识符或者过滤器标识符（32位位宽时为其高段位，16位位宽时为第一个)
    CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000; //用来设定过滤器屏蔽标识符或者过滤器标识符（32位位宽时为其低段位，16位位宽时为第二个)
    CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;//过滤器0关联到FIFO0
    CAN_FilterInitStructure.CAN_FilterActivation=ENABLE; //使能过滤器0
    CAN_FilterInit(&CAN_FilterInitStructure);//滤波器初始化

    NVIC_InitStructure.NVIC_IRQChannel =CAN1_RX0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;     // 主优先级为1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;            // 次优先级为0
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);  //FIFO0消息挂号中断允许

}

void CAN2_Mode_Init(void)
{
    GPIO_InitTypeDef       GPIO_InitStructure;
    CAN_InitTypeDef        CAN_InitStructure;
    CAN_FilterInitTypeDef  CAN_FilterInitStructure;
    NVIC_InitTypeDef       NVIC_InitStructure;

    //使能相关时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//使能PORTB时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2, ENABLE);//使能CAN2时钟
    //初始化GPIO
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5| GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
    GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化PB5,PB6

    //引脚复用映射配置
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource5,GPIO_AF_CAN2); //GPIOB5复用为CAN2
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource6,GPIO_AF_CAN2); //GPIOB6复用为CAN2

    //CAN单元设置
    CAN_InitStructure.CAN_TTCM=DISABLE;	//非时间触发通信模式
    CAN_InitStructure.CAN_ABOM=DISABLE;	//软件自动离线管理
    CAN_InitStructure.CAN_AWUM=DISABLE;//睡眠模式通过软件唤醒(清除CAN->MCR的SLEEP位)
    CAN_InitStructure.CAN_NART=DISABLE;	//禁止报文自动传送
    CAN_InitStructure.CAN_RFLM=DISABLE;	//报文不锁定,新的覆盖旧的
    CAN_InitStructure.CAN_TXFP=DISABLE;	//优先级由报文标识符决定
    CAN_InitStructure.CAN_Mode= CAN_Mode_Normal;	 //模式设置
    CAN_InitStructure.CAN_SJW=CAN_SJW_1tq;	//重新同步跳跃宽度(Tsjw)为tsjw+1个时间单位 CAN_SJW_1tq~CAN_SJW_4tq
    CAN_InitStructure.CAN_BS1=CAN_BS1_9tq; //Tbs1范围CAN_BS1_1tq ~CAN_BS1_16tq
    CAN_InitStructure.CAN_BS2=CAN_BS2_4tq;//Tbs2范围CAN_BS2_1tq ~	CAN_BS2_8tq
    CAN_InitStructure.CAN_Prescaler=3;  ////CAN BaudRate 42/(1+9+4)/3=1Mbps
    CAN_Init(CAN2, &CAN_InitStructure);   // 初始化CAN1

    //配置过滤器
    CAN_FilterInitStructure.CAN_FilterNumber= 14;	  //过滤器0
    CAN_FilterInitStructure.CAN_FilterMode= CAN_FilterMode_IdMask;
    CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit; //32位
    CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;////32位ID
    CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
    CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;//32位MASK
    CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
    CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO1;//过滤器14关联到FIFO1
    CAN_FilterInitStructure.CAN_FilterActivation=ENABLE; //激活过滤器0
    CAN_FilterInit(&CAN_FilterInitStructure);//滤波器初始化

    NVIC_InitStructure.NVIC_IRQChannel =CAN2_RX1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;     // 主优先级为1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;            // 次优先级为0
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    CAN_ITConfig(CAN2,CAN_IT_FMP1,ENABLE);  //FIFO0消息挂号中断允许
}


//接收中断服务函数
void CAN1_RX0_IRQHandler(void)
{
    CanRxMsg RxMessage;
    if (CAN_GetITStatus(CAN1,CAN_IT_FMP0)!= RESET)
    {
        CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);  //接收信息
        CanRec_Dir_M2006_Process(&RxMessage);
    }
}

//接收中断服务函数
void CAN2_RX1_IRQHandler(void)
{
    CanRxMsg RxMessage;
    if (CAN_GetITStatus(CAN2,CAN_IT_FMP1)!= RESET)
    {
        CAN_Receive(CAN2, CAN_FIFO1, &RxMessage);  //接收信息
        CanRec_Up_M2006_Process(&RxMessage);
    }
}

/**
  * @brief 3508的CAN报文发送函数
  */
void CAN1_Send2M2006_D(int16_t ESC_201,int16_t ESC_202,int16_t ESC_203,int16_t ESC_204)
{
    u8 mbox;
    u16 i=0;
    CanTxMsg TxMessage;     //定义一个发送信息的结构体
    TxMessage.StdId = 0x200;     //根据820r设置标识符
    TxMessage.IDE = CAN_ID_STD;     //指定将要传输的消息的标识符的类型
    TxMessage.RTR = CAN_RTR_DATA;    //指定的帧将被传输的消息的类型   数据帧或远程帧
    TxMessage.DLC =0x08;      //指定数据的长度
    TxMessage.Data[0] = (ESC_201>>8);
    TxMessage.Data[1] = ESC_201;
    TxMessage.Data[2] = (ESC_202>>8);
    TxMessage.Data[3] = ESC_202;
    TxMessage.Data[4] = (ESC_203>>8);
    TxMessage.Data[5] = ESC_203;
    TxMessage.Data[6] = (ESC_204>>8);
    TxMessage.Data[7] = ESC_204;
    i=0;
    mbox=CAN_Transmit(CAN1,&TxMessage);   //发送信息
    while((CAN_TransmitStatus(CAN1, mbox)==CAN_TxStatus_Failed)&&(i<0XFFF))//等待发送结束
        i++;
}

void CAN2_Send2M2006_U(int16_t ESC_201,int16_t ESC_202)
{
    u8 mbox;
    u16 i=0;
    CanTxMsg TxMessage;     //定义一个发送信息的结构体
    TxMessage.StdId = 0x200;     //根据820r设置标识符
    TxMessage.IDE = CAN_ID_STD;     //指定将要传输的消息的标识符的类型
    TxMessage.RTR = CAN_RTR_DATA;    //指定的帧将被传输的消息的类型   数据帧或远程帧
    TxMessage.DLC =0x08;      //指定数据的长度
    TxMessage.Data[0] = (ESC_201>>8);
    TxMessage.Data[1] = ESC_201;
    TxMessage.Data[2] = (ESC_202>>8);
    TxMessage.Data[3] = ESC_202;
    i=0;
    mbox=CAN_Transmit(CAN2,&TxMessage);   //发送信息
    while((CAN_TransmitStatus(CAN2, mbox)==CAN_TxStatus_Failed)&&(i<0XFFF))//等待发送结束
        i++;
}

/**
  * @brief 向电机板传送信息
  * @param VEL1~4;ANGLE1~4
  */
void CAN2_Trasnmit_TO_BOARD(float lfVel,float rfVel,float lrVel,float rrVel)
{
    CanTxMsg TxMessage;
    uint8_t mbox;
    uint8_t Flag = 0;
    uint16_t i,j=0;
    static union
    {
        int16_t vel[4];
        int8_t data[8];
    } Target_Vel;

    Target_Vel.vel[0] = (int16_t)lfVel;
    Target_Vel.vel[1] = (int16_t)rfVel;
    Target_Vel.vel[2] = (int16_t)lrVel;
    Target_Vel.vel[3] = (int16_t)rrVel;


    TxMessage.StdId = VEL_ID;
    TxMessage.IDE = CAN_ID_STD;
    TxMessage.RTR = CAN_RTR_DATA;
    TxMessage.DLC = 0x08;

    for(i=0; i<8; i++)
    {
        TxMessage.Data[i] = Target_Vel.data[i];
        //DMA_Printf("%x\r\n",TxMessage.Data[i]);
    }

    mbox=CAN_Transmit(CAN2,&TxMessage);   //发送信息
    Flag++;
    while((CAN_TransmitStatus(CAN2, mbox)==CAN_TxStatus_Failed)&&(j<0XFFF))//等待发送结束
        j++;


}

