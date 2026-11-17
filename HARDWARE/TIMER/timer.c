#include "main.h"

uint8_t RX_BUF[32]={0};
extern uint16_t flag;
//float Gatling_Angle[3] = {45.0f,333.0f,621.0f};
//float Pan_tilt_Angle[3] = {32.9f,56.1f,34.6f};
//extern uint16_t Shoot_flag;
//extern uint16_t Rotate_flag;


void TIM2_Int_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);  ///使能TIM3时钟

    TIM_TimeBaseInitStructure.TIM_Period = 0xffffffff; 	//自动重装载值
    TIM_TimeBaseInitStructure.TIM_Prescaler=83;  //定时器分频
    TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
    TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;

    TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);//初始化TIM3

    TIM_Cmd(TIM2,ENABLE); //使能定时器3
}

//void TIM3_Int_Init(u16 arr,u16 psc)
//{
//    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
//    NVIC_InitTypeDef NVIC_InitStructure;

//    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  ///使能TIM3时钟

//    TIM_TimeBaseInitStructure.TIM_Period = arr; 	//自动重装载值
//    TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  //定时器分频
//    TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
//    TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;

//    TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);//初始化TIM3

//    TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //允许定时器3更新中断
//    TIM_Cmd(TIM3,ENABLE); //使能定时器3

//    NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; //定时器3中断
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x04; //抢占优先级4
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x00; //子优先级0
//    NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
//    NVIC_Init(&NVIC_InitStructure);
//}

//通用定时器5中断初始化
//arr：自动重装值。
//psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz
//这里使用的是定时器5!
void TIM5_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);	//使能TIM5时钟

    TIM_TimeBaseInitStructure.TIM_Period = arr; 		//自动重装载值
    TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  		//定时器分频
    TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
    TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;

    TIM_TimeBaseInit(TIM5,&TIM_TimeBaseInitStructure);	//初始化TIM5

    TIM_ITConfig(TIM5,TIM_IT_Update,ENABLE); 			//允许定时器5更新中断
    TIM_Cmd(TIM5,ENABLE); 								//使能定时器5

    NVIC_InitStructure.NVIC_IRQChannel=TIM5_IRQn; 		//定时器5中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x05; 	//抢占优先级5
    NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x00; 		//子优先级0
    NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

//void TIM7_Int_Init(u16 arr,u16 psc)
//{
//    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
//    NVIC_InitTypeDef NVIC_InitStructure;

//    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7,ENABLE);	//使能TIM5时钟

//    TIM_TimeBaseInitStructure.TIM_Period = arr; 		//自动重装载值
//    TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  		//定时器分频
//    TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
//    TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1;

//    TIM_TimeBaseInit(TIM7,&TIM_TimeBaseInitStructure);	//初始化TIM5

//    TIM_ITConfig(TIM7,TIM_IT_Update,ENABLE); 			//允许定时器5更新中断
//    TIM_Cmd(TIM7,ENABLE); 								//使能定时器5

//    NVIC_InitStructure.NVIC_IRQChannel=TIM7_IRQn; 		//定时器5中断
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x05; 	//抢占优先级5
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x00; 		//子优先级0
//    NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
//    NVIC_Init(&NVIC_InitStructure);
//}

////定时器3中断服务函数
//void TIM3_IRQHandler(void)
//{
//    if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //溢出中断
//    {
//        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  //清除TIMx更新中断标志
//        Angle_Receive(&AS5048A_1);
//    }
//}


////定时器7中断服务函数
//void TIM7_IRQHandler(void)
//{
//    if(TIM_GetITStatus(TIM7,TIM_IT_Update)==SET) //溢出中断
//    {
//        TIM_ClearITPendingBit(TIM7, TIM_IT_Update);  //清除TIMx更新中断标志
//        CAN2_Send2M2006_U(PosCtrl_pid(&moto[4],Gatling_Angle[Rotate_flag]),PosCtrl_pid(&moto[5],Pan_tilt_Angle[Rotate_flag]));
//    }
//}


//定时器5中断服务函数
void TIM5_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM5,TIM_IT_Update)==SET) //溢出中断
    {
        TIM_ClearITPendingBit(TIM5,TIM_IT_Update);
        NRF24L01_RX_Mode();
        if(NRF24L01_RxPacket(RX_BUF)==0)
        {
            switch(RX_BUF[0])
            {

            case('1'):
							flag = 1;
            break;

            case('2'):
              flag = 2;
            break;

            case('3'):
              flag = 3;
            break;

            case('4'):
              flag = 4;
            break;
						
						case('5'):
							flag = 5;
						break;
						
            }
        }

    }
}

