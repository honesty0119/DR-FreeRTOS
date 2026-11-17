#include "main.h"

MOTO_Control *p;

/**
	@brief
	@note
	*/
void CanRec_Dir_M2006_Process(CanRxMsg *msg)//判断电调
{
    int i;
    switch(msg->StdId)
    {
    case D_CAN_2006Moto1_ID:
        i=0;
        break;
    case D_CAN_2006Moto2_ID:
        i=1;
        break;
    case D_CAN_2006Moto3_ID:
        i=2;
        break;
    case D_CAN_2006Moto4_ID:
        i=3;
        break;
    }
		
        p=&moto[i];
        p->posCtrl.LastAngle =p->posCtrl.NowAngle;
        p->posCtrl.NowAngle = (uint16_t)(msg->Data[0]<<8 | msg->Data[1]);
        if(p->Offset_Get_Check==0)   //是否获取初始偏移角标志位
        {
            p->posCtrl.offset_angle=p->posCtrl.NowAngle;
            p->posCtrl.LastAngle =p->posCtrl.NowAngle;
            p->Offset_Get_Check=1;
        }
        p->ActualSpeed=p->velCtrl.speed  = (int16_t)(msg->Data[2]<<8 | msg->Data[3]);
        if(p->posCtrl.NowAngle - p->posCtrl.LastAngle > 4096)
            p->posCtrl.round_cnt--;
        else if (p->posCtrl.NowAngle -  p->posCtrl.LastAngle < -4096)
            p->posCtrl.round_cnt ++;                     //对此段代码的理解同get_total_angle
        p->posCtrl.all_angle = (float)(p->posCtrl.round_cnt  * 8192 + p->posCtrl.NowAngle - p->posCtrl.offset_angle)*360/M2006_DIVISOR;
}


void CanRec_Up_M2006_Process(CanRxMsg *msg)//判断电调
{
    switch(msg->StdId)
    {

    case(U_CAN_2006Moto1_ID):
    {
        p=&moto[4];
        p->posCtrl.LastAngle =p->posCtrl.NowAngle;
        p->posCtrl.NowAngle = (uint16_t)(msg->Data[0]<<8 | msg->Data[1]);
        if(p->Offset_Get_Check==0)   //是否获取初始偏移角标志位
        {
            p->posCtrl.offset_angle=p->posCtrl.NowAngle;
            p->posCtrl.LastAngle =p->posCtrl.NowAngle;
            p->Offset_Get_Check=1;
        }
        p->ActualSpeed=p->velCtrl.speed  = (int16_t)(msg->Data[2]<<8 | msg->Data[3]);
        if(p->posCtrl.NowAngle - p->posCtrl.LastAngle > 4096)
            p->posCtrl.round_cnt--;
        else if (p->posCtrl.NowAngle -  p->posCtrl.LastAngle < -4096)
            p->posCtrl.round_cnt ++;                     //对此段代码的理解同get_total_angle
        p->posCtrl.all_angle = (float)(p->posCtrl.round_cnt  * 8192 + p->posCtrl.NowAngle - p->posCtrl.offset_angle)*360/M2006_DIVISOR;
    }
    break;

    case(U_CAN_2006Moto2_ID):
    {
        p=&moto[5];
        p->ActualSpeed=p->velCtrl.speed  = (int16_t)(msg->Data[2]<<8 | msg->Data[3]);
        p->posCtrl.all_angle = AS5048A_1.All_Angle;//由绝对值磁编获得角度_AS5048A
    }
    break;

    }
}

/**
  * @brief 将速度、位置参数发给电调
  */
void SendDriver(float lfVel , float lfDir , float rfVel , float rfDir,
				float lrVel , float lrDir , float rrVel , float rrDir)
{
	CAN2_Trasnmit_TO_BOARD(VEL_LIN2ROT(lfVel),VEL_LIN2ROT(rfVel),
						   VEL_LIN2ROT(lrVel),VEL_LIN2ROT(rrVel)); //向电机板发送目标速度与角度	
	
	CAN1_Send2M2006_D(PosCtrl_pid(&moto[0],lfDir*GEAR_RATIO),PosCtrl_pid(&moto[1],rfDir*GEAR_RATIO),
					  PosCtrl_pid(&moto[2],lrDir*GEAR_RATIO),PosCtrl_pid(&moto[3],rrDir*GEAR_RATIO)); 	
}


