/**
 ****************************************************************************************************
 * @file        usart.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-17
 * @brief       串口初始化代码(一般是串口1)，支持printf
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 STM32F103开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 * 修改说明
 * V1.0 20200417
 * 第一次发布
 *
 ****************************************************************************************************
 */

#ifndef __USART_H
#define __USART_H

#include "stdio.h"
#include "./SYSTEM/sys/sys.h"


/******************************************************************************************/
/* 引脚 和 串口 定义 
 * 默认是针对USART1的.
 * 注意: 通过修改这12个宏定义,可以支持USART1~UART7任意一个串口.
 */
#define USART_TX_GPIO_PORT                  GPIOA
#define USART_TX_GPIO_PIN                   SYS_GPIO_PIN9
#define USART_TX_GPIO_CLK_ENABLE()          do{ RCC->APB2ENR |= 1 << 2; }while(0)   /* PA口时钟使能 */

#define USART_RX_GPIO_PORT                  GPIOA
#define USART_RX_GPIO_PIN                   SYS_GPIO_PIN10
#define USART_RX_GPIO_CLK_ENABLE()          do{ RCC->APB2ENR |= 1 << 2; }while(0)   /* PA口时钟使能 */

#define USART_UX                            USART1
#define USART_UX_IRQn                       USART1_IRQn
#define USART_UX_IRQHandler                 USART1_IRQHandler
#define USART_UX_CLK_ENABLE()               do{ RCC->APB2ENR |= 1 << 14; }while(0)  /* USART1 时钟使能 */

/******************************************************************************************/


#define USART_REC_LEN               200         /* 定义最大接收字节数 200 */
#define USART_EN_RX                 1           /* 使能（1）/禁止（0）串口1接收 */


extern uint8_t  g_usart_rx_buf[USART_REC_LEN];  /* 接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 */
extern uint16_t g_usart_rx_sta;                 /* 接收状态标记 */

void usart_init(uint32_t pclk2, uint32_t bound);/* 串口初始化函数 */
void Serial_Init(uint32_t baud);               /* USART2 初始化，作为 Serial 使用 */
void Serial_SendByte(uint8_t byte);            /* USART2 发送 1 字节 */
void Serial_SendString(char *str);             /* USART2 发送字符串 */
void uart3_init(uint32_t baud);                /* USART3 初始化，替代示例中的 uart2 */
void uart3_send_byte(uint8_t byte);            /* USART3 发送 1 字节 */
void uart3_send_string(char *str);             /* USART3 发送字符串 */
void uart4_init(uint32_t baud);                /* UART4 初始化（仅接收功能） */
uint8_t uart4_get_rx_flag(void);               /* 获取 UART4 接收标志位 */
uint8_t uart4_get_rx_data(void);               /* 获取 UART4 接收数据 */
void uart4_clear_rx_flag(void);                /* 清除 UART4 接收标志位 */
extern uint8_t uart4_rx_buffer[64];            /* UART4 接收缓冲区（在中断中直接保存） */
extern volatile uint8_t uart4_rx_index;        /* UART4 接收缓冲区索引 */

#endif  
















