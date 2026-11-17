#ifndef __CAN_H
#define __CAN_H
#include "sys.h"


#define VEL_ID 0x01
#define DIR_ID1 0x03
#define DIR_ID2 0x04


void CAN2_Trasnmit_TO_BOARD(float lfVel,float rfVel,float lrVel,float rrVel);
void CAN1_Mode_Init(void);//CAN初始化
void CAN2_Mode_Init(void);
void CAN1_Send2M2006_D(int16_t ESC_201,int16_t ESC_202,int16_t ESC_203,int16_t ESC_204);
void CAN2_Send2M2006_U(int16_t ESC_201,int16_t ESC_202);
#endif

