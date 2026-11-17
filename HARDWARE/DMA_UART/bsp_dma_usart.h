#ifndef	__BSP_DMA_USART_H
#define	__BSP_DMA_USART_H

#include "stm32f4xx.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

// 串口发送对应的DMA请求通道
#define  USART_TX_DMA_STREAM     DMA2_Stream7
#define  USART_TX_DMA_CHANNEL	 DMA_Channel_4
//串口DMA发送完成中断通道
#define	 USART_TX_DMA_IRQN		 DMA2_Stream7_IRQn
//串口接收对应的DMA请求通道
#define  USART_RX_DMA_STREAM     DMA2_Stream5
#define  USART_RX_DMA_CHANNEL	 DMA_Channel_4     
// 外设寄存器地址
#define  USART_DR_ADDRESS        ((u32)&USART1->DR)
// 一次发送的数据量
#define  SENDBUFF_SIZE            128
// 一次接收的数据量
#define  RECBUFF_SIZE             128
//串口打印缓冲区的个数
#define	 PRINT_BUFF_NUM			  40

extern uint32_t SendBuff_Addr;;//发送数据缓冲数组地址
extern uint8_t RecBuff[];//接收数据缓冲数组

void UART_Tx_DMA_Config(void);
void UART_Rx_DMA_Config(void);
void UART_RxTx_DMA_EN(void);

void UART_Transmit_DMA(uint16_t Size);
void UART_Receive_DMA(uint16_t Size);

void DMA_Printf(const char *format,...);

void usart_Data_Process(uint8_t* temp);
#endif
