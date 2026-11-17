#ifndef CHASSIS_CONTROL_H
#define CHASSIS_CONTROL_H

#include <stdint.h>
#include <math.h>
#include "path.h"

//当前位置距离目标的直线偏差
#define  BIAS_NOW  __sqrtf((Target_X-GetPositionX())*(Target_X-GetPositionX())+(Target_Y-GetPositionY())*(Target_Y-GetPositionY()))

typedef struct 
{
	float kp1;	
	float ki1;
	float kd1;
	float kp2;	
	float ki2;
	float kd2;	
	float Integral_Max;
	float output_max;
	float Bias_Range;  //偏差允许范围（±)
} ChasDir_CtrlType;  //底盘方向PID参数


float Chas_DirCtrl(float Target_Angle,float Now_Angle,ChasDir_CtrlType* ChasDir_Pid);
void Chassis_MoveStraight(Path_StraightType* Straight);
void Chassis_MoveCurve_Test(float Vel,Path_CurveType* Curve);
void Chassis_Brake(void);
void Chassis_Rotate(Path_StraightType* Straight);

#endif