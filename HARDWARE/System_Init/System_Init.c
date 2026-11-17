#include "main.h"


void System_Init(void)
{
	Path_Straight_Init(); //直线路径初始化
	AS5048A_StructInit();
  MOTO_Slope_PID_Init();
	protocol_init();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组2
	delay_init(168);    //初始化延时函数
	GPIO_INIT();
	NRF24L01_Init();
	uart_init(115200);	//初始化串口波特率为115200
	usart2_init(115200);  //初始化定位器
	UART_RxTx_DMA_EN();
	//D_AS5048A_QH_Init();
	YT_AS5048A_QH_Init();
	CAN1_Mode_Init();
	CAN2_Mode_Init();
//	TIM3_Int_Init(999,83);//1ms
	TIM2_Int_Init();
	TIM5_Int_Init(999,16799); //100ms
//	TIM7_Int_Init(999,839);  //10ms
	
	while(NRF24L01_Check())
	{
		DMA_Printf("%s\r\n","NRF24L01 Error");//NRF24L01 Error
		delay_ms(200);
		LED0=1;		
	}
	LED0 = 0;
	
    DMA_Printf("%s\r\n","NRF24L01 OK");//NRF24L01 OK
	DMA_Printf("%s\r\n","NRF24L01 RX_Mode");//"NRF24L01 RX_Mode"
	DMA_Printf("%s\r\n","Received DATA:");	//"Received DATA"	
	while(isOps9Ready == 0)   //wait for ops9 ready
	{
		DMA_Printf("OPS9 Waiting...\r\n");
		delay_ms(10);
	}	
	delay_ms(1000);
	
}


