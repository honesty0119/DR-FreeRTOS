#include "main.h"

uint32_t cnt = 0;  //串口打印计数
uint16_t flag;
uint16_t Shoot_flag = 0;
uint16_t Rotate_flag = 0;
extern MOTO_Control moto[6];
float Gatling_Angle[3] = {45.0f,333.0f,621.0f};
float Pan_tilt_Angle[3] = {32.9f,56.4f,35.2f};


//任务优先级
#define START_TASK_PRIO		1
//任务堆栈大小	
#define START_STK_SIZE 		128  
//任务句柄
TaskHandle_t StartTask_Handler;
//任务函数
void start_task(void *pvParameters);

//任务优先级
#define TASK1_TASK_PRIO		4
//任务堆栈大小	
#define TASK1_STK_SIZE 		128  
//任务句柄
TaskHandle_t Task1Task_Handler;
//任务函数
void task1_task(void *pvParameters);

//任务优先级
#define TASK2_TASK_PRIO		4
//任务堆栈大小	
#define TASK2_STK_SIZE 	 512  
//任务句柄
TaskHandle_t Task2Task_Handler;
//任务函数
void task2_task(void *pvParameters);

//任务优先级
#define TASK3_TASK_PRIO		 3
//任务堆栈大小	
#define TASK3_STK_SIZE 		128  
//任务句柄
TaskHandle_t Task3Task_Handler;
//任务函数
void task3_task(void *pvParameters);

//任务优先级
#define TASK4_TASK_PRIO		 2
//任务堆栈大小	
#define TASK4_STK_SIZE 		128  
//任务句柄
TaskHandle_t Task4Task_Handler;
//任务函数
void task4_task(void *pvParameters);

int main(void)
{ 
   System_Init();

	//创建开始任务
    xTaskCreate((TaskFunction_t )start_task,            //任务函数
                (const char*    )"start_task",          //任务名称
                (uint16_t       )START_STK_SIZE,        //任务堆栈大小
                (void*          )NULL,                  //传递给任务函数的参数
                (UBaseType_t    )START_TASK_PRIO,       //任务优先级
                (TaskHandle_t*  )&StartTask_Handler);   //任务句柄              
    vTaskStartScheduler();          //开启任务调度
}

//开始任务任务函数
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //进入临界区
    //创建TASK1任务
    xTaskCreate((TaskFunction_t )task1_task,             
                (const char*    )"task1_task",           
                (uint16_t       )TASK1_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )TASK1_TASK_PRIO,        
                (TaskHandle_t*  )&Task1Task_Handler);   
    //创建TASK2任务
    xTaskCreate((TaskFunction_t )task2_task,     
                (const char*    )"task2_task",   
                (uint16_t       )TASK2_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASK2_TASK_PRIO,
                (TaskHandle_t*  )&Task2Task_Handler); 
								
		xTaskCreate((TaskFunction_t )task3_task,     
                (const char*    )"task3_task",   
                (uint16_t       )TASK3_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASK3_TASK_PRIO,
                (TaskHandle_t*  )&Task3Task_Handler); 
								
    xTaskCreate((TaskFunction_t )task4_task,     
                (const char*    )"task4_task",   
                (uint16_t       )TASK4_STK_SIZE,
                (void*          )NULL,
                (UBaseType_t    )TASK4_TASK_PRIO,
                (TaskHandle_t*  )&Task4Task_Handler); 
								
    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}

//task1任务函数
void task1_task(void *pvParameters)
{
	    while(1)
    {
       
        switch(flag)
        {

        case(1):
        {
          if(Shoot_flag<3)
          shoot(Shoot_flag);
          Shoot_flag++;
            flag = 0;
        }
        break;

        case(2):
        {
					DMA_Printf("rotate\r\n");
					Rotate_flag = 1;
          flag = 0;
        }
        break;

        case(3):
        {
					DMA_Printf("rotate2\r\n");
					Rotate_flag = 2;
          flag = 0;
        }
        break;

        case(4):
        {
					DMA_Printf("shake\r\n");
          shake();
          flag = 0;
        }
				break;
				
				case(5):
				{
					DMA_Printf("solenoid\r\n");
					Solenoid(3) = 1;
					flag = 0;
        }
				break;
				
			}
				//DMA_Printf("task1\r\n");
				vTaskDelay(20);
				
		}
}

//task2任务函数
void task2_task(void *pvParameters)
{
	Chassis_MoveStraight(&Straight1);
	Chassis_MoveStraight(&Straight2);	
	Chassis_MoveStraight(&Straight3);	
	Chassis_MoveStraight(&Straight4);
	Chassis_MoveStraight(&Straight5);		
	while(1)
	{
		SendDriver(0,0,0,0,0,0,0,0);
		vTaskDelay(10);
	}
}

void task3_task(void *pvParameters)
{
	while(1)
	{
		//DMA_Printf("task3\r\n");
		Angle_Receive(&AS5048A_1);
		//DMA_Printf("%.2f\r\n",AS5048A_1.All_Angle);
		vTaskDelay(3);
	}
}

void task4_task(void *parameters)
{
	while(1)
	{
		//DMA_Printf("task4\r\n");
		CAN2_Send2M2006_U(PosCtrl_pid(&moto[4],Gatling_Angle[Rotate_flag]),PosCtrl_pid(&moto[5],Pan_tilt_Angle[Rotate_flag]));
		vTaskDelay(15);
	}
}	

