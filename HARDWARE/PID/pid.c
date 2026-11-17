#include "main.h"

#define F4_USE	1    //选择使用F4或者F1

///*******************************数值限制**************************************/

///**
//  * @brief  limit2
//  * @param  inDat:
//  * @retval outDat
//  */
int16_t limit2(int16_t x,int16_t max,int16_t min)
{
    if(x>max)
        x=max;
    else if(x<min)
        x=min;
    return x;
}


/********************************************PID**************************************/

MOTO_Control moto[6];

/**
	@brief 斜坡PID参数初始化
  */
void MOTO_Slope_PID_Init(void)
{

//底盘2006 pid
    for(int i=0; i<4; i++)
    {
        moto[i].velCtrl.kp 	       = 7.0f;
        moto[i].velCtrl.ki 	       = 1.02f;
        moto[i].velCtrl.maxOutput 	 = 13500.0f;//速度环限幅
        moto[i].velCtrl.integralmax = 8000.0f; //速度环积分限幅
        moto[i].posCtrl.kd		     	 = 15.0f;
        moto[i].posCtrl.kp 		     = 70.0f; //90.0f

        moto[i].velCtrl.acc 			       = 1000.0f;//可调参数
        moto[i].velCtrl.dec 			       = 1000.0f;
        moto[i].velCtrl.acc_small_large = 450.0f;	   //可调参数
        moto[i].velCtrl.dec_small_large = 450.0f;
        moto[i].velCtrl.desiredVel[CMD] = 1000.0f;
        moto[i].posCtrl.acc 			       = 200000.0f;//可调参数
        moto[i].posCtrl.posVel 		     = 15000.0f;//位置环限幅

    }

//射箭pid

    moto[4].velCtrl.kp 	       = 3.5f;
    moto[4].velCtrl.ki 	       = 0.82f;
    moto[4].velCtrl.maxOutput 	 = 12000.0f;//速度环限幅
    moto[4].velCtrl.integralmax = 8000.0f; //速度环积分限幅
    moto[4].posCtrl.kd		     	 = 5.0f;
    moto[4].posCtrl.kp 		     = 30.0f;

    moto[4].velCtrl.acc 			       = 1000.0f;//可调参数
    moto[4].velCtrl.dec 			       = 1000.0f;
    moto[4].velCtrl.acc_small_large = 450.0f;	   //可调参数
    moto[4].velCtrl.dec_small_large = 450.0f;
    moto[4].velCtrl.desiredVel[CMD] = 1000.0f;
    moto[4].posCtrl.acc 			       = 10000.0f;//可调参数
    moto[4].posCtrl.posVel 		     = 15000.0f;//位置环限幅



//云台射箭
    moto[5].velCtrl.kp 		     = 3.0f;
    moto[5].velCtrl.ki 		     = 0.82f;
    moto[5].velCtrl.maxOutput	 = 10000.0f;//速度环限幅
    moto[5].velCtrl.integralmax = 5000.0f; //速度环积分限幅
    moto[5].posCtrl.kd			    = 5.0f;
    moto[5].posCtrl.kp 		     = 45.0f;

    moto[5].velCtrl.acc 			       = 2000.0f;//可调参数
    moto[5].velCtrl.dec 			       = 8000.0f;
    moto[5].velCtrl.acc_small_large = 45.0f;	   //可调参数
    moto[5].velCtrl.dec_small_large = 25.0f;
    moto[5].velCtrl.desiredVel[CMD] = 1000.0f;
    moto[5].posCtrl.acc 			       = 8000.0f;//可调参数
    moto[5].posCtrl.posVel 		     = 12000.0f;//位置环限幅


}

/**
  * @brief 普通速度环PID初始化
  */
void MOTO_Speed_PID_Init(void)
{
    int i;
    for(i=0; i<6; i++)
    {
        moto[i].Increment_kp = 2.0f;
        moto[i].Increment_ki = 0.001f;
        moto[i].Increment_kd = 0.2f;
        moto[i].PrevError=moto[i].LastError=moto[i].NowError=moto[i].OutputValue=moto[i].Integral_speed =0;
    }
}

/**
  * @brief //普通速度PID
  */
int16_t Speed_Pid(MOTO_Control *M,int16_t Input_Value)
{

    M->NowError  = Input_Value - M->ActualSpeed;
    M->Integral_speed += M->NowError;
    M->Integral_speed=limit2(M->Integral_speed,Max_Integral_speed,-Max_Integral_speed);
    M->OutputValue = M->Increment_kp *M->NowError  + M->Increment_ki * M->Integral_speed +M->Increment_kd*(M->NowError  - 2*M->LastError+M->PrevError );
    M->OutputValue = limit2(M->OutputValue,Max_Speed,-Max_Speed);
    M->PrevError=M->LastError;
    M->LastError = M->NowError;

    return M->OutputValue;
}

/**
  * @brief  斜坡加速度处理函数
  * @param  None
  * @retval ??????
  */

float VelSlope(VelCtrlType *velPid)
{
    if(fabsf(velPid->desiredVel[CMD])>=fabsf(velPid->acc))
    {
        if(velPid->speed < (velPid->desiredVel[CMD] - velPid->acc))
            velPid->desiredVel[SOFT] +=velPid->acc;
        else if(velPid->speed> (velPid->desiredVel[CMD] + velPid->dec))
            velPid->desiredVel[SOFT] -=velPid->dec;
        else
            velPid->desiredVel[SOFT] = velPid->desiredVel[CMD];
    }
    else
    {
        if(velPid->speed< (velPid->desiredVel[CMD] - velPid->acc_small_large))
            velPid->desiredVel[SOFT] +=velPid->acc_small_large;
        else if(velPid->speed > (velPid->desiredVel[CMD] + velPid->dec_small_large))
            velPid->desiredVel[SOFT] -=velPid->dec_small_large;
        else
            velPid->desiredVel[SOFT] = velPid->desiredVel[CMD];
    }
    return velPid->desiredVel[SOFT];
}
/**
  * @brief  斜坡算法速度环
  * @param  None
  * @retval ??PID???
  */
int16_t VelPidCtrl(VelCtrlType *velPid,float desire_Speed)
{
    velPid->desiredVel[CMD]=desire_Speed;

    VelSlope(velPid);

    velPid->velErr = velPid->desiredVel[SOFT] - velPid->speed;

    velPid->iOut += velPid->ki * velPid->velErr;

    velPid->iOut = MaxMinLimit(velPid->iOut,velPid->integralmax);

    velPid->output = velPid->kp * velPid->velErr + velPid->iOut ;

    velPid->output = MaxMinLimit(velPid->output,velPid->maxOutput);

    return velPid->output;
}

/**
  * @brief  斜坡算法位置环
  * @param  None
  * @retval ???PID????
  */
float PosCtrl(PosCtrlType *posPid,float desire_pos)
{
    float posPidOut = 0.0f;
    float desiredVel = 0.0f,signVel = 1.0f;

    /******************************???????**************************************/
    posPid->desiredPos=desire_pos;

    posPid->posErr = posPid->desiredPos - posPid->all_angle;

    posPidOut = posPid->posErr*posPid->kp + posPid->kd*(posPid->posErr-posPid->posErrLast);

    posPid->posErrLast = posPid->posErr;

    if(posPid->posErr < 0.0f) signVel = -1.0f;

    /******************************计算位置环输出**************************************/
#if	F4_USE
    desiredVel = signVel*__sqrtf(2.0f*5.0f*posPid->acc*signVel*posPid->posErr);   //注意F4可用__sqrtf函数，F1只能用sqrt函数
#else
    desiredVel = signVel*sqrt(2.0f*5.0f*posPid->acc*signVel*posPid->posErr);
#endif

    if(fabsf(desiredVel) < fabsf(posPidOut))
        posPidOut = desiredVel;
    //给一定大小的死区
    if(fabsf(posPid->posErr) <= 0.1f)	   //此处可以设定误差角度范围
        posPidOut = 0.0f;

    posPid->output = MaxMinLimit(posPidOut,posPid->posVel);

    return posPid->output;
}
/**
  * @brief 斜坡算法
**/
int16_t PosCtrl_pid(MOTO_Control *moto,float desire_pos)
{
    PosCtrl(&moto->posCtrl,desire_pos);
    return VelPidCtrl(&moto->velCtrl,moto->posCtrl.output);
}


