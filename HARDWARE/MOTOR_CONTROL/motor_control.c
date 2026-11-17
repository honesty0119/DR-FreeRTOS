#include "motor_control.h"
#include "can.h"

///**
//  * @brief 将速度、位置参数发给电调
//  */
//void SendDriver(float lfVel , float lfDir , float rfVel , float rfDir,
//				      	float lrVel , float lrDir , float rrVel , float rrDir)
//{
//	CAN2_Trasnmit_TO_BOARD(lfVel,rfVel,lrVel,rrVel); //向电机板发送目标速度与角度		
//  CAN1_Send2M2006_D(PosCtrl_pid(&moto[0],lfDir),PosCtrl_pid(&moto[1],rfDir),
//	                  PosCtrl_pid(&moto[2],lrDir)，PosCtrl_pid(&moto[3],rrDir)); 	
//}
