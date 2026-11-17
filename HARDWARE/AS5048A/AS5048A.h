#ifndef __AS5048A_H
#define __AS5048A_H


#include "sys.h"


#define CMD_ANGLE          0x3FFF  
#define CMD_READ_MAG       0x3FFE 
#define CMD_READ_DIAG      0x3FFD  
#define CMD_NOP            0x0000  
#define CMD_CLEAR_ERROR    0x0001  
#define CMD_ProgramControl 0x0003
#define CMD_OTPHigh 0x0016
#define CMD_OTPLow 0x0017

typedef struct{
	int Offset_Get_Check;
	float Offset_Angle;
	int16_t Raw_Angle;
	float Last_Angle;
	float Now_Angle;
	float All_Angle;
	float Motor_Angle;
	uint16_t Pin_CS; //片选引脚
	GPIO_TypeDef* GPIOx;
	SPI_TypeDef* SPIx;
}AS5048A_Type;


typedef struct{
	u8 Offset_Get_Check;   //获取初始角检查
	float Offset_Angle;   //初始角
	float Now_Angle;      //当前角
	float Last_Angle;     //上次测到的角
	float All_Angle;      //总角度
}Angle_Type;  //角度数据结构体
	  	
extern AS5048A_Type AS5048A_1;
extern AS5048A_Type AS5048A[4];




void Write_As5048A_Reg(uint16_t cmd,uint16_t value);
uint8_t Write_As5048A_ZeroPosition(void);//OTP Abandoned



void YT_AS5048A_QH_Init(void);
void D_AS5048A_QH_Init(void);
uint16_t SPI_WriteByte(uint16_t TxData,AS5048A_Type *p);
uint16_t Read_As5048A_Reg(uint16_t cmd,AS5048A_Type *p);
uint16_t Read_As5048A_QH_Value(uint16_t cmd,AS5048A_Type *p);
uint8_t parity_even(uint16_t v);
void Angle_Receive(AS5048A_Type *p);
void Angle_Struct_Init(AS5048A_Type *p);
void AS5048A_StructInit(void);
float Angle_Trans(AS5048A_Type *p);        //角度转换函数
float float_abs(float x);

#endif

