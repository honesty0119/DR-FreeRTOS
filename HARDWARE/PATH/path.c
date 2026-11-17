#include "path.h"

/********************************************直线路径参数*****************************/

Path_StraightType Straight1,Straight2,Straight3,Straight4,Straight5;  //测试直线路径

/**
  * @brief 直线路径参数初始化
  * @note  采用轮子转45度来辅助刹车，然后PID纠偏
  * @note  ODRIVE的加速及减速度有上限
  */
void Path_Straight_Init(void)
{
	Straight1.Velocity = 2000;
	Straight1.Target_X = 2000;
	Straight1.Target_Y = -4500; 
	Straight1.Posture = 0.0f;
	Straight1.Bias_Range = 10.0f;
	Straight1.PID_Range = 300; //800
	Straight1.KP = 15.0f;   //10.0f
	Straight1.KI = 0.0f;   //只用PD参数
	Straight1.KD = 40.0f;  //30
	Straight1.Integral_Max = 2000;
	Straight1.Output_Max = Straight1.Velocity;//根据期望速度值设定
	
	Straight2.Velocity = 2000; 
	Straight2.Target_X = 4994; //5058
	Straight2.Target_Y = -4236; //4238
	Straight2.Posture = 0.0f;
	Straight2.Bias_Range = 10.0f;
	Straight2.PID_Range = 200; //800
	Straight2.KP = 15.0f;   //10.0f
	Straight2.KI = 0.0f;   //只用PD参数
	Straight2.KD = 40.0f;  //30
	Straight2.Integral_Max = 2000;
	Straight2.Output_Max = Straight2.Velocity;//根据期望速度值设定
	
	Straight3.Velocity = 1000;
	Straight3.Target_X = 5028;
	Straight3.Target_Y = -2959; 
	Straight3.Posture = 0.0f;
	Straight3.Bias_Range = 10.0f;
	Straight3.PID_Range = 200; //800
	Straight3.KP = 15.0f;   //10.0f
	Straight3.KI = 0.0f;   //只用PD参数
	Straight3.KD = 40.0f;  //30
	Straight3.Integral_Max = 2000;
	Straight3.Output_Max = Straight3.Velocity;//根据期望速度值设定

	Straight4.Velocity = 1000;
	Straight4.Target_X = 3471;
	Straight4.Target_Y = -2078; 
	Straight4.Posture = 0.0f;
	Straight4.Bias_Range = 10.0f;
	Straight4.PID_Range = 200; //800
	Straight4.KP = 15.0f;   //10.0f
	Straight4.KI = 0.0f;   //只用PD参数
	Straight4.KD = 40.0f;  //30
	Straight4.Integral_Max = 2000;
	Straight4.Output_Max = Straight4.Velocity;//根据期望速度值设定
	
	Straight5.Velocity = 600;
	Straight5.Target_X = 4583;
	Straight5.Target_Y = -2078; 
	Straight5.Posture = 0.0f;
	Straight5.Bias_Range = 10.0f;
	Straight5.PID_Range = 300; //800
	Straight5.KP = 15.0f;   //10.0f
	Straight5.KI = 0.0f;   //只用PD参数
	Straight5.KD = 40.0f;  //30
	Straight5.Integral_Max = 2000;
	Straight5.Output_Max = Straight5.Velocity;//根据期望速度值设定
}

/*********************************************曲线路径参数********************************/

Path_CurveType	Curve_Start2Arrow;   //测试曲线路径
/**
  * @brief 曲线路径参数初始化
  */
void Path_Curve_Init(void)
{
	//启动区到箭架曲线参数
	Curve_Start2Arrow.Curve_Points_Num = CURVE_START2ARROW_POINTS_NUM;
	Curve_Start2Arrow.Points_X = Curve_Start2Arrow_X;
	Curve_Start2Arrow.Points_Y = Curve_Start2Arrow_Y;
	Curve_Start2Arrow.Bias_Range = 30;  
	Curve_Start2Arrow.Posture = 0.0f;
}

//曲线X坐标:启动区到箭架
int16_t Curve_Start2Arrow_X[CURVE_START2ARROW_POINTS_NUM]=
{
	-193,-234,-278,-325,-376,-429,-485,-544,-606,-671,
	-739,-809,-881,-957,-1034,-1114,-1197,-1282,-1369,-1458,
	-1549,-1642,-1738,-1835,-1934,-2035,-2138,-2243,-2349,-2456

};


//曲线Y坐标：启动区到箭架
int16_t Curve_Start2Arrow_Y[CURVE_START2ARROW_POINTS_NUM]=
{
	
	3008,2866,2729,2596,2467,2342,2221,2105,1992,1883,
	1778,1676,1579,1485,1395,1308,1225,1146,1070,997,
	928,861,799,739,682,629,578,531,486,445
	
};
