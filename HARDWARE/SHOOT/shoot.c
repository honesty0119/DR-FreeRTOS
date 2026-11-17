#include "main.h"
#include <math.h>

extern MOTO_Control moto[6];
extern float Gatling_Angle[3];
extern float Pan_tilt_Angle[3];

void shoot(uint16_t shoot_flag)
{
			Solenoid(shoot_flag) = 1;
	    DMA_Printf("shoot\r\n");
			vTaskDelay(700);
			Solenoid(shoot_flag) = 0;
	    LED1 = 1;
}

//void rotate(uint16_t rotate_flag)
//{
//	while((fabsf(moto[4].posCtrl.all_angle - Gatling_Angle[rotate_flag])>=0.5f)&&(fabsf(moto[5].posCtrl.all_angle - Pan_tilt_Angle[rotate_flag])>=0.5f))
//		{
//			CAN2_Send2M2006_U(PosCtrl_pid(&moto[4],Gatling_Angle[rotate_flag]),PosCtrl_pid(&moto[5],Pan_tilt_Angle[rotate_flag]));		
//			delay_ms(5);
//		}
//		LED1 = 0;
//}

void shake(void)
{
		Solenoid(4) = 1;
		vTaskDelay(700);
		Solenoid(4) = 0;
}

