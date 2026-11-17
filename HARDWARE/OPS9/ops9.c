/**
19               29cm  -5cm
39.25 first: 6 times   Ybais:28cm Xbais:0.5cm
89                           30cm   -12cm
139.25                      21cm       Xbais:6cm
239.25                       24cm        3.5cm
339                           50cm       3cm

  ******************************************************************************
  * @file    ops9.c 
  * @author  Billy, Action Team
  * @version V1.0.0
  * @date    30-October-2017
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, ACTION ROBOT SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2017 Action Robot</center></h2>
  ******************************************************************************  
  */ 
#include "main.h"	    

//distance between robot's geometric center and ops9' center
#define CENTER_COMPENSATE (154.5f)  //仅适用于当前2006+3508舵轮

_Bool isOps9Ready = 0; //用来判断ops9是否初始化成功
_Bool OPS9ErrorFlag=0;
	
static float gAngle = 0.0f;   	//陀螺仪z方向角度 范围-180~180
static float gPostionX = 0.0f;	 //定位系统返回的X坐标
static float gPostionY = 0.0f;	 //定位系统返回的Y坐标

/*OPS9角度值结构体*/
struct
{
	float Last_Angle;  //上次角度值
	float Now_Angle;   //当前角度值
	float All_Angle;   //累计角度值
}OPS9_Angle={0.0f,0.0f,0.0f};

/**
  * @brief  对OPS9进行多圈计数，角度值累加
  * @param  none
  * @retval void
  */
static void Angle_Sum(void)
{
	float res1, res2, delta;
	if(OPS9_Angle.Now_Angle < OPS9_Angle.Last_Angle)
	{			
		res1 =OPS9_Angle.Now_Angle + 360 -OPS9_Angle.Last_Angle;	
		res2 =OPS9_Angle.Now_Angle -OPS9_Angle.Last_Angle;			
	}
	else
	{	
		res1 =OPS9_Angle.Now_Angle - 360 -OPS9_Angle.Last_Angle ;
		res2 =OPS9_Angle.Now_Angle -OPS9_Angle.Last_Angle;				
	}

	if(fabsf(res1)<fabsf(res2))
		delta = res1;
	else
		delta = res2;

 	OPS9_Angle.All_Angle += delta;	
}
/**
  * @brief  对OPS9获取的角度值进行处理
  * @param  angle: 当前获取实际角度值
  * @retval void
  */
static void Angle_Process(float angle)
{
	OPS9_Angle.Last_Angle = OPS9_Angle.Now_Angle;
	OPS9_Angle.Now_Angle = angle;
	Angle_Sum();
}
/**
  * @brief  Set the robot's angle from ops9 
  * @param  none 
  * @retval void
  */
void SetAngle(float angle)
{
	Angle_Process(angle);
	gAngle = OPS9_Angle.All_Angle;
}

/**
  * @brief  Get the robot's angle from ops9 
  * @param  none 
  * @retval void
  */
float GetAngle(void)
{
	return gAngle;
}

/**
  * @brief  Set the robot's position x from ops9 
  * @param  none 
  * @retval void
  */
void SetPositionX(float x)
{
	gPostionX = x;
}

/**
  * @brief  Get the robot's position x from ops9 
  * @param  none 
  * @retval void
  * @note   去掉了负号,由原本的+CENTER_COMPENSATE改为-
  */
float GetPositionX(void)
{
	return (-gPostionY - CENTER_COMPENSATE*(1-cosf(GetAngle()*3.1415926f/180.0f)));
}

/**
  * @brief  Set the robot's position y from ops9 
  * @param  none 
  * @retval void
  */
void SetPositionY(float y)
{
	gPostionY = y;
}

/**
  * @brief  Get the robot's position Y from ops9 
  * @param  none 
  * @retval void
  * @note   去掉了负号,且由原本的cos改为1-cos
  */

float GetPositionY(void)
{
	return (gPostionX + CENTER_COMPENSATE*sinf(GetAngle()*3.1415926f/180.0f));
}

/****串口1 接收OPS9数据中断服务函数*********/
extern _Bool isOps9Ready;
#ifdef OPS9toUSART1
void USART1_IRQHandler(void)
{
	static uint8_t ch = 0;
	static uint8_t count = 0;
	static uint8_t i = 0;
	
	static union
	{
		uint8_t data[24];
		float ActVal[6];
	}posture;
		
	 if(USART_GetITStatus(USART1, USART_IT_RXNE)==SET)   
	 {
		USART_ClearITPendingBit( USART1,USART_IT_RXNE);
		ch=USART_ReceiveData(USART1);
		switch(count)
		{
		case 0:
			if(ch == 0x0d)
				count++;
			else
				count = 0;
			break;
		 
		case 1:
			if(ch == 0x0a)
			{
				i = 0;
				count++;
			}
			else if(ch == 0x0d);
			else
				count = 0;
			break;
		 
		case 2:
			posture.data[i] = ch;
			i++;
			if(i >= 24)
			{
				i = 0;
				count++;
			}
			break;
		
		case 3:
			if(ch == 0x0a)
				count++;
			else
				count=0;
			break;
		 
		case 4:
			if(ch == 0x0d)
			{				 
				SetPositionX(posture.ActVal[3]);
				SetPositionY(posture.ActVal[4]);
				SetAngle(posture.ActVal[0]);
				isOps9Ready = 1;
			}
			count = 0;
			break;

		default:
			count = 0;
			break;		 
		}	 	 
	 }
}
#else 
void USART2_IRQHandler(void)//暂时不用
{
	static uint8_t ch = 0;
	static uint8_t count = 0;
	static uint8_t i = 0;
	
	static union
	{
		uint8_t data[24];
		float ActVal[6];
	}posture;
		
	 if(USART_GetITStatus(USART2, USART_IT_RXNE)==SET)   
	 {
		USART_ClearITPendingBit( USART2,USART_IT_RXNE);
		ch=USART_ReceiveData(USART2);
		switch(count)
		{
		case 0:
			if(ch == 0x0d)
				count++;
			else
				count = 0;
			break;
		 
		case 1:
			if(ch == 0x0a)
			{
				i = 0;
				count++;
			}
			else if(ch == 0x0d);
			else
				count = 0;
			break;
		 
		case 2:
			posture.data[i] = ch;
			i++;
			if(i >= 24)
			{
				i = 0;
				count++;
			}
			break;
		
		case 3:
			if(ch == 0x0a)
				count++;
			else
				count=0;
			break;
		 
		case 4:
			if(ch == 0x0d)
			{				 
				SetPositionX(posture.ActVal[3]);
				SetPositionY(posture.ActVal[4]);
				SetAngle(posture.ActVal[0]);
				isOps9Ready = 1;
			}
			count = 0;
			break;

		default:
			count = 0;
			break;		 
		}	 	
	 }
}
#endif
/**
  * @brief  Send robot's angle/position x/position y out 
  * @param  none 
  * @retval void
  */
void USART_OUT_XYAngle(void)
{
#ifdef OPS9toUSART1
	USART_OUT(DEBUG_USART, (uint8_t *)"%d\t%d\t%d\r\n", (int)GetPositionX(), (int)GetPositionY(), (int)GetAngle());
#else
	//printf("%d\t%d\t%d\r\n", (int)GetPositionX(), (int)GetPositionY(), (int)GetAngle());
//	printf("%d\t%d\t%d\r\n", (int)GetZuobiao(1), (int)GetZuobiao(2), (int)GetZuobiao(3));
#endif
}
