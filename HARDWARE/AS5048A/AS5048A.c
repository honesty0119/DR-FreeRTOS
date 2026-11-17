/**
   ******************************************************************************
   * @file      AS5048A.c
   * @author    深圳市沁和智能科技有限公司
   * @version   V1.0
   * @date      2020.03.09
   * @brief     
   ******************************************************************************
   * @attention--SPI2
   * SCK  -- PB13
   * MISO -- PB14
   * MOSI -- PB15
   * CS   -- PB12
	 
	 * @attention--SPI1
   * SCK  -- PA5
   * MISO -- PA6
   * MOSI -- PA7
   * CS   -- PD0~3
	 
	 
   ******************************************************************************
   */
	 
#include "AS5048A.h"
#include "delay.h"


Angle_Type Angle_Struct;
uint8_t error_flag;
AS5048A_Type AS5048A_1;
AS5048A_Type AS5048A[4];

/**
 * @BRIF  AS5048a初始化 
*/
  
float float_abs(float x)			// 绝对值计算
{
	if(x < 0)
		x=-x;
	return x;
}

void SPI_Delay(uint32_t delay)
{
	while(delay--);
}

// Calculates even parity of 16it value, returns 1 (odd) or 0 (even)
uint8_t parity_even(uint16_t v)
{
      if(v == 0) return 0;


      v ^= v >> 8;
      v ^= v >> 4;
      v ^= v >> 2;
      v ^= v >> 1;
      return v & 1;
}

 
void D_AS5048A_QH_Init(void)
{

	
	GPIO_InitTypeDef  GPIO_InitStructure;
  SPI_InitTypeDef  SPI_InitStructure;
	
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能GPIOB时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);//使能GPIOB时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);//使能SPI2时钟
 
  //GPIOFB3,4,5初始化设置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化
	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOD, &GPIO_InitStructure);//初始化
	GPIO_SetBits(GPIOD, GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3);
		
		
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource5,GPIO_AF_SPI1); //PB13复用为 SPI2
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource6,GPIO_AF_SPI1); //PB14复用为 SPI2
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource7,GPIO_AF_SPI1); //PB15复用为 SPI2
  
	
	//这里只针对SPI口初始化

  SPI_Cmd(SPI1, DISABLE);
	
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//设置SPI工作模式:设置为主SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;		//设置SPI的数据大小:SPI发送接收8位帧结构
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;		//串行同步时钟的空闲状态为高电平
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	//串行同步时钟的第二个跳变沿（上升或下降）数据被采样
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;		//定义波特率预分频的值:波特率预分频值为256
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
	SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRC值计算的多项式
	SPI_Init(SPI1, &SPI_InitStructure);  //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器
 
	SPI_Cmd(SPI1, ENABLE); //使能SPI外设
	
	delay_us(500);
		 
}


/**
 * @param[in]   TxData    收发的数据
 * @retval      SPI接收的数据
 */
uint16_t SPI_WriteByte(uint16_t TxData,AS5048A_Type *p)
{
	GPIO_ResetBits(p->GPIOx, p->Pin_CS);

	SPI_Delay(12);

	SPI_I2S_SendData(p->SPIx, TxData); 
		
	while (SPI_I2S_GetFlagStatus(p->SPIx, SPI_I2S_FLAG_RXNE) == RESET);

	SPI_Delay(12);

	GPIO_SetBits(p->GPIOx, p->Pin_CS);

	SPI_Delay(12);
	return SPI_I2S_ReceiveData(p->SPIx); 
}


/**
 * @function    读取寄存器值
 * @param[in]   cmd    寄存器地址
 * @retval      该地址的值
 */
uint16_t Read_As5048A_Reg(uint16_t cmd,AS5048A_Type *p)
{
	uint16_t data = 0;
	uint16_t res;
	uint16_t command = 0x4000;// PAR=0 R/W=R
	command = command | cmd;
	command |= ((uint16_t)parity_even(command)<<15);//Add a parity bit on the the MSB
				SPI_WriteByte(command,p);

	command = 0x4000;// PAR=0 R/W=R
	command = command | CMD_NOP;
	command |= ((uint16_t)parity_even(command)<<15);//Add a parity bit on the the MSB
				res = SPI_WriteByte(command,p);
	error_flag = 1;

	if ((res & (1 << 14)) == 0){
		data = (res & 0x3FFF);
		error_flag = (parity_even(data) ^ (res >> 15));
	}
	else{
		command = 0x4000;
		command = command | CMD_CLEAR_ERROR;
		command |= ((uint16_t)parity_even(command)<<15);//Add a parity bit on the the MSB
		SPI_WriteByte(command,p);
	}
  return data;
}

/**
 * @function    读取AS5048A数据
 * @param[in]   cmd    寄存器地址
 * @retval      角度信息
 */
uint16_t Read_As5048A_QH_Value(uint16_t cmd,AS5048A_Type *p)
{
	uint16_t val;
	val = Read_As5048A_Reg(cmd,p);
	if(error_flag)
	{
	val = 0;
	}
	return val;
}



void YT_AS5048A_QH_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef SPI_InitStructure;

	RCC_APB1PeriphClockCmd( RCC_APB1Periph_SPI2, ENABLE );
	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOB, ENABLE) ;

	//PB13:SCK;PB15:MOSI 	//PB14:MISO 	//PB12:CS
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;//PB3~5复用功能输出	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; //Set CS pin high to disable device
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_SetBits(GPIOB, GPIO_Pin_12);

  GPIO_PinAFConfig(GPIOB,GPIO_PinSource13,GPIO_AF_SPI2); //PB13复用为 SPI2
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource14,GPIO_AF_SPI2); //PB14复用为 SPI2
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource15,GPIO_AF_SPI2); //PB15复用为 SPI2
  
	SPI_Cmd(SPI2, DISABLE);
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI2, &SPI_InitStructure);

	SPI_Cmd(SPI2, ENABLE);

	delay_us(500);
}


///************************************
//OTP Write Zero Position: 0 for No error
//1. Read angle information
//2. Set the Programming Enable bit in the OTP control register
//3. Write previous read angle position into OTP zero position register
//4. Read back for verification the zero position register data
//5. Set the Burn bit to start the automatic programming procedure
//6. Read angle information (equals to 0)
//7. Set the Verify bit to load the OTP data again into the internal registers with modified threshold comparator levels
//8. Read angle information (equals to 0)
//******************************************/
//uint8_t Write_As5048A_ZeroPosition()
//{
//	uint16_t Angle_val;
//	uint8_t Angle_High,Angle_Low;
//	uint16_t cmd;
//	Angle_val = Read_As5048A_QH_Value(CMD_ANGLE);
//	cmd=Read_As5048A_QH_Value(CMD_ProgramControl);

//	Write_As5048A_Reg(CMD_ProgramControl,0x01);
//	delay_us(10);
//	cmd=Read_As5048A_QH_Value(CMD_ProgramControl);
//	delay_us(10);
//	Write_As5048A_Reg(CMD_OTPHigh,Angle_val>>8);
//	delay_us(10);
//	Write_As5048A_Reg(CMD_OTPLow,Angle_val&0xFF);
//	delay_us(10);
//	Angle_High=Read_As5048A_QH_Value(CMD_OTPHigh);
//	delay_us(10);
//	Angle_Low=Read_As5048A_QH_Value(CMD_OTPLow);
//	delay_us(10);
//	if(Angle_High != (uint8_t)(Angle_val>>8))
//	return 1;
//	if(Angle_Low != (uint8_t)(Angle_val&0xFF))
//	return 2;

//	Write_As5048A_Reg(CMD_ProgramControl,0x09);
//	delay_us(10);
//	cmd=Read_As5048A_QH_Value(CMD_ProgramControl);
//	delay_us(10);
//	Angle_val = Read_As5048A_QH_Value(CMD_ANGLE);
//	delay_us(10);
//	if(Angle_val != 0 )
//	return 3;

//	Write_As5048A_Reg(CMD_ProgramControl,0x49);
//	delay_us(10);
//	cmd=Read_As5048A_QH_Value(CMD_ProgramControl);
//	delay_us(10);
//	Angle_val = Read_As5048A_QH_Value(CMD_ANGLE);
//	delay_us(10);
//	if(Angle_val != 0 )
//	return 4;

//	return 0;
//}


/*************************我的代码*************************
*****角度转化函数******************************************/
float Angle_Trans(AS5048A_Type *p)
{
	float True_Angle;
	True_Angle=(float)(p->Raw_Angle*360)/16384;
	return True_Angle;
}

//总的角度值累积
void Get_AllAngle(AS5048A_Type *p)
{

	float c1, c2, delta;
	if(p->Now_Angle > p->Last_Angle)
	{
		c1 = p->Now_Angle - p->Last_Angle;
		c2 = (p->Now_Angle - p->Last_Angle) - 360;
	}
	else
	{	
		c1 = p->Now_Angle - p->Last_Angle;
		c2 = 360 - ( p->Last_Angle - 	p->Now_Angle);	
	}

	if(float_abs(c1) > float_abs(c2))
		delta = c2;
	else
		delta = c1;

 	p->All_Angle += delta;
}

//角度读取函数
void Angle_Receive(AS5048A_Type *p)
{
	p->Raw_Angle=Read_As5048A_QH_Value(CMD_ANGLE,p);  //获取原始角度值
	p->Last_Angle=p->Now_Angle; 
	p->Now_Angle=360-Angle_Trans(p);  //若电机方向与编码器方向相同，则直接用Angle_Trans(),若相反，则用360-Angle_Trans()
	if(p->Offset_Get_Check==0) //若未获取了初始角（即此次为首次进入Angle_Receive函数)
	{	
//		p->Offset_Angle=p->Now_Angle;   //获取偏移角
		p->All_Angle = p->Now_Angle;
		p->Offset_Get_Check = 1;   //标记已经获取了初始角
	}
	else  //若已经获取了初始角(非首次进入Angle_Receive函数)
		Get_AllAngle(p);  //首次进入Angle_Receive函数，Last_Angle仍为初始值，故不能调用Get_AllAngle,运算NowAngle-LastAngle.
	  //p->All_Angle = p->All_Angle/36;
	//p->Motor_Angle=p->All_Angle/36;   //M2006减速比为36：1;舵轮底座传动比4：1
}

void Angle_Struct_Init(AS5048A_Type *p)
{
	p->Offset_Get_Check=0;
	p->All_Angle=0;
	p->Last_Angle=0;
	p->Now_Angle=0;
}

void AS5048A_StructInit(void)
{
//	AS5048A[0].Pin_CS = GPIO_Pin_0;
//	AS5048A[1].Pin_CS = GPIO_Pin_1;
//	AS5048A[2].Pin_CS = GPIO_Pin_2;
//	AS5048A[3].Pin_CS = GPIO_Pin_3;	
	AS5048A_1.Pin_CS  = GPIO_Pin_12;
//	AS5048A[0].GPIOx  = GPIOD;
//	AS5048A[1].GPIOx  = GPIOD;
//	AS5048A[2].GPIOx  = GPIOD;
//	AS5048A[3].GPIOx  = GPIOD;
	AS5048A_1.GPIOx   = GPIOB;
//	AS5048A[0].SPIx   = SPI1;
//	AS5048A[1].SPIx   = SPI1;
//	AS5048A[2].SPIx   = SPI1;
//	AS5048A[3].SPIx   = SPI1;
	AS5048A_1.SPIx    = SPI2;
}

