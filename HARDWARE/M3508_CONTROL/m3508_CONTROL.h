#ifndef __M3508_H
#define __M3508_H
#include "sys.h"

//设置电调ID编号
#define	D_CAN_2006Moto1_ID  0x201
#define	D_CAN_2006Moto2_ID  0x202
#define	D_CAN_2006Moto3_ID  0x203
#define	D_CAN_2006Moto4_ID  0x204

#define U_CAN_2006Moto1_ID  0x201
#define U_CAN_2006Moto2_ID  0x202

#define M3508_DIVISOR 157312.68
#define M2006_DIVISOR 294912

void CanRec_Dir_M2006_Process(CanRxMsg* msg);
void CanRec_Up_M2006_Process(CanRxMsg* msg);
void SendDriver(float lfVel , float lfDir , float rfVel , float rfDir,
                float lrVel , float lrDir , float rrVel , float rrDir);

#endif


