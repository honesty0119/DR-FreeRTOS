#include "chassis_control.h"
#include <math.h>
#include "ops9.h"
#include "movebase.h"
#include "motor_control.h"
#include "user_math.h"

#define DEBUG_USE 1  //启用调试信息

extern uint32_t cnt;

/*用于跑直线矫正姿态偏差的底盘方向PID参数*/
static ChasDir_CtrlType Str_CorPos = 
{
	3.0f,0.0f,20.0f,  //1.0f,0.03f,20.0f
	20.0f,0.0f,16.0f, //15.0f,0.00f,16.0f
	500,120,0.2   //输出限幅120
};

/*用于刹车后(X和Y坐标已达到误差范围)底盘方向PID纠偏时的底盘方向PID参数*/
static ChasDir_CtrlType Stp_CorPos = 
{
	10.0f,0.0f,250.0f,  
	100.0f,0.0f,400.0f, 
	500,720,0.2   //输出限幅720	
};
/**
  * @brief 底盘方向PID控制
  * @param Target_Angle:目标角度
  * @param Now_Angle :当前角度
  * @retval 底盘旋转角速度w：度/s
  * @note  输入为角度;输出为转速
  */
float Chas_DirCtrl(float Target_Angle,float Now_Angle,ChasDir_CtrlType* ChasDir_Pid)
{
	float omega = 0; //输出角速度 :度/s
	float Bias = 0;  //偏差
	static float Last_Bias = 0;//上次偏差
	static float Integral_Bias = 0; //积分值
	
	Bias = Target_Angle - Now_Angle ;
	Integral_Bias +=Bias;
	//积分限幅
	Integral_Bias = limitf(Integral_Bias,ChasDir_Pid->Integral_Max,-ChasDir_Pid->Integral_Max);
	//PID运算
	if(fabsf(Bias) > 15)  		//偏差角度较大时,用一种PID参数
		omega = ChasDir_Pid->kp1*Bias + ChasDir_Pid->ki1*Integral_Bias + ChasDir_Pid->kd1*(Bias - Last_Bias);
	else if(fabsf(Bias) > ChasDir_Pid->Bias_Range) //偏差角度较小时，用另一种PID参数
		omega = ChasDir_Pid->kp2*Bias + ChasDir_Pid->ki2*Integral_Bias + ChasDir_Pid->kd2*(Bias - Last_Bias);		
	else 
		omega = 0;      //偏差角很小时，直接输出0(故允许误差范围为±0.2)
	//输出限幅
	omega = limitf(omega,ChasDir_Pid->output_max,-ChasDir_Pid->output_max);
	
	Last_Bias = Bias;   

	return omega;
}



/**
  * @brief 底盘定点跑直线
  * @param Vel:期望速度；
  * @param Straight:直线结构体
  */
void Chassis_MoveStraight(Path_StraightType* Straight)
{
	float Bias_X = 0,Bias_Y = 0;
	float Dir = 0.0f,omega = 0,OutVel = 0;
	float Bias_Sum = 0;
	float Integral_Bias = 0,Last_Bias = 0;
	Brake_Flag = 1;
	
	while(1)
	{
		//目标点与当前点的相对距离
		Bias_X = Straight->Target_X - GetPositionX();
		Bias_Y = Straight->Target_Y - GetPositionY();
		Bias_Sum = __sqrtf(Bias_X*Bias_X + Bias_Y*Bias_Y);

		//计算轮子目标朝向
		Dir = RAD2ANGLE(atan2f(Bias_Y,Bias_X));

		//根据到目标点的距离，计算OutVel及omega
		if(Bias_Sum > Straight->PID_Range)	//当未达到PID控制范围时
		{
			OutVel = Straight->Velocity;  //直接输出期望速度
			//计算底盘角速度
			omega = Chas_DirCtrl(Straight->Posture,GetAngle(),&Str_CorPos);
		}
		else if(Bias_Sum > Straight->Bias_Range)//未达到偏差允许范围，但达到PID控制范围时,先进行PID运算
		{
			if(Straight != &Straight5)
				Chassis_Brake();//到达PID范围先刹车
			
			//刹车完之后通过PID纠正偏差
			Integral_Bias += Bias_Sum;
			//积分限幅
			Integral_Bias = limitf(Integral_Bias,Straight->Integral_Max,-Straight->Integral_Max);
			OutVel = Straight->KP*Bias_Sum + Straight->KI*Integral_Bias + Straight->KD*(Bias_Sum - Last_Bias);		
			//输出限幅
			OutVel = limitf(OutVel,Straight->Output_Max,-Straight->Output_Max);
			Last_Bias = Bias_Sum;
			//计算底盘角速度
			omega = Chas_DirCtrl(Straight->Posture,GetAngle(),&Str_CorPos);
		}
		else//如果已经达到了目标范围，则输出速度0
		{ 
			if(fabsf(GetAngle()-Straight->Posture)<0.2f) //若姿态角也达到了目标范围
			{
				if(Straight != &Straight5)
					return;
				//底盘45度交叉锁定
				SendDriver(0.0f , -45.0f ,0.0f, -135.0f, 0.0f , 45.0f , 0.0f , 135.0f);
				vTaskDelay(10); //延时是为了保持10ms的周期
				cnt++;
				DMA_Printf("%d:%.2f %.2f %.2f\r\n",cnt,GetPositionX(),GetPositionY(),GetAngle());
				continue;
			}
			OutVel = 0;
			Dir = 0;
			//计算底盘角速度(调节范围小，用另一个参数,防止电机线速度过小，无法转动)
			omega = Chas_DirCtrl(Straight->Posture,GetAngle(),&Stp_CorPos);
		}

		OutputVel2Wheel(OutVel,Dir,omega);
		vTaskDelay(10);	//控制周期不能随意更改(11ms)
		
#if DEBUG_USE  //打印位置信息
		cnt++;
		DMA_Printf("%d:%.2f %.2f %.2f\r\n",cnt,GetPositionX(),GetPositionY(),GetAngle());
#endif
	}
}

/**
  * @brief 底盘跑曲线
  * @param Vel:期望速度
  * @param Curve:曲线结构体
  * @note  由于点数过多，点距较小，而速度较快，所以可能难以到达目标点
  * @note  曲线路径还有很大问题
  */
void Chassis_MoveCurve_Test(float Vel,Path_CurveType* Curve)
{
	uint8_t i = 0;
	uint16_t Point_Num = 0;  //当前目标点
	int16_t Target_X = 0,Target_Y = 0;
	int16_t Bias_X = 0,Bias_Y = 0,Bias_Sum = 0;
	float Dir = 0.0f,omega = 0.0f;

	while(Point_Num < Curve->Curve_Points_Num)//未跑完全部的点
	{
		//设定本次目标点
		Target_X = Curve->Points_X[Point_Num];
		Target_Y = Curve->Points_Y[Point_Num];
		//获取当前点距离目标点的偏差
		Bias_X = Target_X - GetPositionX();
		Bias_Y = Target_Y - GetPositionY();
		
		//计算轮子朝向
		Dir = RAD2ANGLE(atan2f(Bias_Y,Bias_X));
		//计算底盘角速度
		omega = Chas_DirCtrl(Curve->Posture,GetAngle(),&Str_CorPos);
		//输出到四个轮子
		OutputVel2Wheel(Vel,Dir,omega);
		
		for(i=0;i<10;i++)//控制周期10ms
		{	
			if(BIAS_NOW < Curve->Bias_Range) //若达到了当前目标点，则运算下一个目标点
			{
				Point_Num ++;
				break;  
			}
			delay_ms(1); 
		}
		
#if DEBUG_USE  //打印位置信息
		cnt++;
		DMA_Printf("%d:%.2f %.2f %.2f\r\n",cnt,GetPositionX(),GetPositionY(),GetAngle());
#endif
	}
	OutputVel2Wheel(0,0,0);  //达到目标点，输出0
}

/**
  * @brief 底盘刹车
  * @note  效果还行
  */
uint8_t Brake_Flag = 1;

void Chassis_Brake(void)
{
	static uint32_t brake_delay = 0;
	while(Brake_Flag)
	{	
		//向电机发送速度(0)和位置命令(45度交叉，这样减速并不是最好的方案，但这种轮子朝向各方面都对称)
		SendDriver(0.0f , -45.0f ,0.0f, -135.0f, 0.0f , 45.0f , 0.0f , 135.0f);	

		vTaskDelay(10);
		
		if(fabsf(fabsf(moto[1].posCtrl.all_angle/GEAR_RATIO)-130)<5.0f)
		{
			brake_delay++;
			if(brake_delay>25) //留足够的刹车时间
			{
				brake_delay = 0;
				Brake_Flag = 0;
			}
		}
		
#if DEBUG_USE  //打印位置信息
		cnt++;
		DMA_Printf("%d:%.2f %.2f %.2f\r\n",cnt,GetPositionX(),GetPositionY(),GetAngle());
#endif
	}
}


/**
  * @brief 底盘锁定，用于固定坐标点时底盘旋转到目标姿态角并锁定
  * @param Straight: 锁定时的坐标点相关信息
  * @note  可以接在跑路径之后
  */
void Chassis_Rotate(Path_StraightType* Straight)
{
	float Bias_X = 0,Bias_Y = 0;
	float Dir = 0.0f,omega = 0,OutVel = 0;
	float Bias_Sum = 0;
	float Integral_Bias = 0,Last_Bias = 0;
	
	while(1)
	{
		//目标点与当前点的相对距离
		Bias_X = Straight->Target_X - GetPositionX();
		Bias_Y = Straight->Target_Y - GetPositionY();
		Bias_Sum = __sqrtf(Bias_X*Bias_X + Bias_Y*Bias_Y);

		//计算轮子目标朝向
		Dir = RAD2ANGLE(atan2f(Bias_Y,Bias_X));	
		
		if(Bias_Sum > Straight->Bias_Range)//未达到偏差允许范围先进行PID运算
		{
			//刹车完之后通过PID纠正偏差
			Integral_Bias += Bias_Sum;
			//积分限幅
			Integral_Bias = limitf(Integral_Bias,Straight->Integral_Max,-Straight->Integral_Max);
			OutVel = Straight->KP*Bias_Sum + Straight->KI*Integral_Bias + Straight->KD*(Bias_Sum - Last_Bias);		
			//输出限幅
			OutVel = limitf(OutVel,Straight->Output_Max,-Straight->Output_Max);
			Last_Bias = Bias_Sum;
			//计算底盘角速度
			omega = Chas_DirCtrl(Straight->Posture,GetAngle(),&Str_CorPos);
		}
		else//如果已经达到了目标范围，则输出速度0
		{
			if(fabsf(GetAngle()-Straight->Posture)<0.2f) //若姿态角也达到了目标范围
			{
				//底盘45度交叉锁定
				SendDriver(0.0f , -45.0f ,0.0f, -135.0f, 0.0f , 45.0f , 0.0f , 135.0f);
				delay_ms(10); //延时是为了保持10ms的周期
				continue;
			}
			OutVel = 0;
			Dir = 0;
			//计算底盘角速度(调节范围小，用另一个参数,防止电机线速度过小，无法转动)
			omega = Chas_DirCtrl(Straight->Posture,GetAngle(),&Str_CorPos);
		}

		OutputVel2Wheel(OutVel,Dir,omega);
		delay_ms(10);	//控制周期不能随意更改(11ms)
		
#if DEBUG_USE  //打印位置信息
		cnt++;
		DMA_Printf("%d:%.2f %.2f %.2f\r\n",cnt,GetPositionX(),GetPositionY(),GetAngle());
#endif	
	}
	
}

/*******************************仅作测试使用**************************/
/**
  * @brief 底盘自旋方向控制
  * @param Target_Direction:目标朝向,范围：±160°
  */
void Chassis_RotationControl(float Target_Direction)
{
	//方向PID运算
	float omega = Chas_DirCtrl(Target_Direction,GetAngle(),&Str_CorPos);
	//速度分解及输出,整体速度及方向设为0即可
	OutputVel2Wheel(0,0,omega);
	
#if DEBUG_USE  //打印角度信息
	DMA_Printf("A:%.2f\r\n",GetAngle());
#endif
}


