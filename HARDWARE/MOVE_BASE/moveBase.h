/*  
part:.h
data:2020.11.23

*/
#ifndef __MOVEBASE_H
#define __MOVEBASE_H

#include "main.h"

typedef struct
{
	//轮子速度大小
	float vel;
	//轮子速度方向
	float direction;
}wheelVel_t;

typedef struct
{
	float lf;
	float rf;
	float lr;
	float rr;
}turnVel_t;


//轮子圈数偏移结构体
typedef struct
{
	int32_t lf;
	int32_t rf;
	int32_t lr;
	int32_t rr;
}steerLoopShift_t;

//轮子结构体
typedef struct
{
	//左前轮
	wheelVel_t leftFront;
	//右前轮
	wheelVel_t rightFront;
	//左后轮
	wheelVel_t leftRear;
	//右后轮
	wheelVel_t rightRear;
}wheel_t;

#define M3508_REDUC_RATIO	(6.40107f) //M3508电机减速比:19.2 / 3

#define GEAR_RATIO (4.0f)   //转向轮齿轮比

#define WHL_RADIUS	 (36.0f) //速度轮半径/mm

//轮子线速度(mm/s)转化到3508转子转速RPM
#define VEL_LIN2ROT(x) (x*1.697936f) // 公式：x/PI/2/WHL_RADIUS*M3508_REDUC_RATIO*60)

//底盘旋转半径
#define MOVEBASE_RADIUS (342.24f)
//弧度制转换为角度制
#define RAD2ANGLE(x) (x/PI*180.0f)
//角度制转化为弧度制
#define ANGLE2RAD(x) (x/180.0f*PI)


//左前轮与中心连线切线方向
#define LEFT_FRONT_VERTICAL_ANG (-135.0f)
//右前轮与中心连线切线方向
#define RIGHT_FRONT_VERTICAL_ANG (135.0f)
//左后轮与中心连线切线方向
#define LEFT_REAR_VERTICAL_ANG (-45.0f)
//右后轮与中心连线切线方向
#define RIGHT_REAR_VERTICAL_ANG (45.0f)


void WheelVelControl(wheel_t wheelVel);
void AngleLimit(float *angle);
float AngleLimit2(float angle);
void OutputVel2Wheel(float vel, float direction, float omega);
wheelVel_t CalcWheelSpeed(float vel , float direction , float omega , float angleN , float postureAngle);
void Transform2RobotCoodinate(float *Dir);
void JudgeVelDirection(wheelVel_t *targetVel , float actualAngle);
#endif