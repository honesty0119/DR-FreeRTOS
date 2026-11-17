#ifndef __PID_H
#define __PID_H
#include "pid.h"
#include "m3508_control.h"

/**************************斜坡算法PID参数*************************/
#define  CMD   0
#define  SOFT  1
#define  VEL_KP_3508   			3.5f	 //斜坡速度PID的KP参数
#define  VEL_KI_3508			  1.0f	 //斜坡速度PID的KI参数0.82

#define  POS_KP_3508          40.0f	 //斜坡位置PID的KP参数  //75
#define  POS_KD_3508        	5.0f	 //斜坡位置PID的KD参数
#define  CURRENT_MAX_3508   	12000.0f //斜坡PID输出限幅
#define  VEL_INTEG_MAX		 	8000.0f  //斜坡速度PID积分限幅

/**************************普通PID参数*****************************/
#define Max_Speed 			10000   //速度PID输出限幅
#define Max_Integral_speed  5000	//速度PID积分限幅

/**
  * @brief  VelCtrl type structure definition
			斜坡算法速度环结构体
  * @note
**/
typedef struct
{
    float speed;

    float desiredVel[2];

    float velErr;

    float acc;

    float dec;

    float acc_small_large;

    float dec_small_large;

    float kp;

    float ki;

    float iOut;

    int16_t output;

    float integralmax;

    float maxOutput;

} VelCtrlType;

/**
  * @brief  PosCtrl type structure definition
			斜坡算法位置环结构体
  * @note
**/
typedef struct
{

    float desiredPos;

    float posErr,posErrLast;

    float posVel,acc;

    float kp;

    float kd;

    float output;

    int16_t NowAngle;

    int16_t offset_angle;

    int16_t LastAngle;

    float all_angle;

    int16_t round_cnt;
} PosCtrlType;

/**
  * @brief  电机结构体
  * @note
**/
typedef struct
{
    float Increment_kp,Increment_ki,Increment_kd;

    int16_t ActualSpeed;
    int16_t last_speed;
    int16_t OutputValue;
    int16_t NowError;
    int16_t LastError;
    int16_t PrevError;
    int16_t Offset_Get_Check;
    int16_t IncrementSpeed;
    int16_t Integral_speed;
    int16_t Integral_position;

    VelCtrlType velCtrl;
    PosCtrlType posCtrl;
    int total_angle ;
    float all_angle;

} MOTO_Control;

extern MOTO_Control moto[6];

void 	MOTO_Slope_PID_Init(void);
int16_t VelPidCtrl(VelCtrlType *velPid,float desire_speed);
float 	PosCtrl(PosCtrlType *posPid,float desire_pos);
int16_t PosCtrl_pid(MOTO_Control *moto,float desire_pos);
int16_t Speed_Pid(MOTO_Control *speed,int16_t Input_Value);
void	MOTO_Speed_PID_Init(void);

#endif
