#include "moveBase.h"
#include "m3508_control.h"

#define DEBUG_USE 1  //启用调试信息

/**
  * @param  vel:平移速度大小（mm/s）
		    direction:平移速度方向（-180°到180°）
		    omega:绕底盘中心旋转角速度(°/s),顺时针为负逆时针为正
		    angleN:在机器人坐标系下旋转线速度的正方向(单位：°)   
			postureAngle:机器人返回的实时的姿态角(°)
  * @note   direction和vel是相关的，若vel=0,则direction也是0
  */
wheelVel_t CalcWheelSpeed(float vel , float direction , float omega , float angleN , float postureAngle)
{
	wheelVel_t sumVel = {0.0f};
	float velX , velY = 0.0f;
	float velN ,velNDirection= 0.0f;
	float sumVelX , sumVelY = 0.0f;
	
	//计算平移速度的X，Y分量
	velX = vel*cosf(ANGLE2RAD(direction));
	velY = vel*sinf(ANGLE2RAD(direction));
	
	//计算旋转的线速度
	velN = ANGLE2RAD(omega)*MOVEBASE_RADIUS;
	
	velNDirection = angleN ;//+ postureAngle;姿态角会限制旋转角度上限？待实际测试
	AngleLimit(&velNDirection);
	
	//计算和速度大小和方向
	sumVelX = velX + velN * cosf(ANGLE2RAD(velNDirection));
	sumVelY = velY + velN * sinf(ANGLE2RAD(velNDirection));
	
	sumVel.vel = __sqrtf(sumVelX * sumVelX + sumVelY * sumVelY);
	
	//计算合成速度方向时将0向量单独处理
	if(sumVel.vel>0.01f)
	{
		sumVel.direction = RAD2ANGLE(atan2f(sumVelY , sumVelX));
	}
	else
	{
		sumVel.direction = direction;
	}
	return sumVel;
}

/**
  * @brief  JudgeVelDirection判断轮子是否需要反转,以及确保旋转为劣弧
  * @note   轮子的最终朝向会被限制在±180°之内
  * @param  targetVel:目标速度大小和方向
  		    actualAngle：当前轮子正方向角度
  * @retval 
  */
void JudgeVelDirection(wheelVel_t *targetVel , float actualAngle)
{
	//如果当前角与目标角差值>90°
	if(fabsf(targetVel->direction - actualAngle)>90.0f)
	{
		//如果当前角在第三象限，目标角在第二象限；或当前角在第二象限，目标角在第一象限，则维持原值
		if(!(((actualAngle < -90.0f)&&(targetVel->direction < 0.0f))||((actualAngle > 90.0f)&&(targetVel->direction > 0.0f))))
		{
			//否则速度反向，角度±180°
			if(targetVel->direction > actualAngle)//如果目标角度更大，目标角度-180°
			{
				targetVel->direction -= 180.0f;
				targetVel->vel = -(targetVel->vel);
			}
			else
			{
				targetVel->direction += 180.0f;  //如果目标角度更小，目标角度+180°
				targetVel->vel = -(targetVel->vel);
			}		
		}
	}	
}

/**
  * @brief  WheelVelControl控制电机速度和位置
  * @note
  * @param  wheelVel:四个轮子速度大小和方向结构体
  * @retval 
  */

void WheelVelControl(wheel_t wheelVel)
{
	//判断是否需要将轮速反向(以3508当前角度做对照）
	JudgeVelDirection(&wheelVel.leftFront, moto[0].posCtrl.all_angle/GEAR_RATIO);
	JudgeVelDirection(&wheelVel.rightFront,moto[1].posCtrl.all_angle/GEAR_RATIO);
	JudgeVelDirection(&wheelVel.leftRear,  moto[2].posCtrl.all_angle/GEAR_RATIO);
	JudgeVelDirection(&wheelVel.rightRear, moto[3].posCtrl.all_angle/GEAR_RATIO);
	
	//向电机发送速度和位置命令
	SendDriver(wheelVel.leftFront.vel  , wheelVel.leftFront.direction ,
			   wheelVel.rightFront.vel , wheelVel.rightFront.direction,
			   wheelVel.leftRear.vel   , wheelVel.leftRear.direction  , 
			   wheelVel.rightRear.vel  , wheelVel.rightRear.direction);			
}

/**
* @brief  Transform2RobotCoodinate将世界坐标系下轮子朝向转换到机器人坐标系下
* @note
* @param  wheelVel:要转换的轮子速度结构体指针
* @retval
  */
void Transform2RobotCoodinate(float *Dir)
{
	//将定位系统坐标系下角度转换为机器人坐标系下角度
	*Dir -= GetAngle();
	//将角度限制在180度到-180度范围内
	AngleLimit(Dir);
}

/**
  * @brief  OutputVel2Wheel计算每个轮子的速度和朝向并输出到电机上
  * @param  vel:平移速度大小（mm/s）
		    direction:平移速度方向（-180°到180°）
		    omega:绕底盘中心旋转角速度(度/s),顺时针为负逆时针为正
  * @note 输入方向角direction在正负180°之间;输出角度控制在±180°之间
  * @note 巧妙之处在于根据目标角与当前角的差值，判断电机是否需要反转，以取最短路径（相对转动角度控制在±90°之间)
  */
void OutputVel2Wheel(float vel, float direction, float omega)
{
	
	wheel_t outputVel = {0.0f};
	
   	//将定位系统坐标系下角度转换为机器人坐标系下角度 direction-=GetAngle()
	Transform2RobotCoodinate(&direction);
	
	//计算每个轮子的和速度大小和方向
	outputVel.leftFront = CalcWheelSpeed(vel , direction , omega , LEFT_FRONT_VERTICAL_ANG , /*GetAngle()*/0);//未使用定位器，所以此处姿态角设为0
	
	outputVel.rightFront = CalcWheelSpeed(vel , direction , omega , RIGHT_FRONT_VERTICAL_ANG , /*GetAngle()*/0);
	
	outputVel.leftRear = CalcWheelSpeed(vel , direction , omega , LEFT_REAR_VERTICAL_ANG , /*GetAngle()*/0);
	
	outputVel.rightRear = CalcWheelSpeed(vel , direction , omega , RIGHT_REAR_VERTICAL_ANG , /*GetAngle()*/0);
		
	//将和速度输出
	WheelVelControl(outputVel);
	
}

/**
* @brief  AngleLimit角度限幅，将角度限制在-180°到180°
* @note 运用递归，限制大角度
* @param  angle:要限制的值
* @retval 
  */
void AngleLimit(float *angle)
{
	if(*angle>180.0f)
	{
		*angle-=360.0f;
		AngleLimit(angle);
	}
	else if(*angle<-180.0f)
	{
		*angle+=360.0f;
		AngleLimit(angle);
	}
}

/**
* @brief  角度限幅2，将角度限制在-180°到180°
* @note 运用递归，限制大角度
* @param  angle:要限制的值
* @retval angle
  */
float AngleLimit2(float angle)
{
	if(angle>180.0f)
	{
		angle-=360.0f;
		angle = AngleLimit2(angle);
	}
	else if(angle<-180.0f)
	{
		angle+=360.0f;
		angle = AngleLimit2(angle);
	}
	return angle;
}
