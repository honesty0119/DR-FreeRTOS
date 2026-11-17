#ifndef PATH_H
#define PATH_H

#include "stm32f4xx.h"

/*直线路径结构体*/
typedef struct
{
	float   Velocity;     //速度
	float   Target_X;     //目标点X坐标
	float   Target_Y;	  //目标点Y坐标
	float   Posture;   //目标点底盘姿态角
	float   PID_Range;    //直线路径PID运算范围(mm)
	float   Bias_Range;   //允许偏差范围(mm)
	float   Integral_Max; //PID积分限幅
	float   Output_Max;   //输出限幅
	float   KP;  //PID参数
	float   KI;
	float   KD;
}Path_StraightType; 

typedef struct 
{
	int16_t* Points_X;   		//微元法X坐标
	int16_t* Points_Y;   		//微元法Y坐标
	float Posture;       		//期望底盘姿态
	int16_t Bias_Range;  		//每个坐标点的允许偏差范围
	uint16_t Curve_Points_Num;  //曲线路径的点数
}Path_CurveType;

/***************************直线路径相关参数*****************/
extern Path_StraightType Straight1,Straight2,Straight3,Straight4,Straight5;

extern uint8_t Brake_Flag;

/****************************曲线路径相关参数****************/

/*启动区到箭架*/
#define CURVE_START2ARROW_POINTS_NUM	30

extern Path_CurveType Curve_Start2Arrow;
extern int16_t Curve_Start2Arrow_X[];
extern int16_t Curve_Start2Arrow_Y[];



void Path_Straight_Init(void);
void Path_Curve_Init(void);

#endif