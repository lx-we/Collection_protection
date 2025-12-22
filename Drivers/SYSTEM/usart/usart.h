/**
 ****************************************************************************************************
 * @file        usart.h
 * @author      锟斤拷锟斤拷原锟斤拷锟脚讹拷(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-17
 * @brief       锟斤拷锟节筹拷始锟斤拷锟斤拷锟斤拷(一锟斤拷锟角达拷锟斤拷1)锟斤拷支锟斤拷printf
 * @license     Copyright (c) 2020-2032, 锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟接科硷拷锟斤拷锟睫癸拷司
 ****************************************************************************************************
 * @attention
 *
 * 实锟斤拷平台:锟斤拷锟斤拷原锟斤拷 STM32F103锟斤拷锟斤拷锟斤拷
 * 锟斤拷锟斤拷锟斤拷频:www.yuanzige.com
 * 锟斤拷锟斤拷锟斤拷坛:www.openedv.com
 * 锟斤拷司锟斤拷址:www.alientek.com
 * 锟斤拷锟斤拷锟街�:openedv.taobao.com
 *
 * 锟睫革拷说锟斤拷
 * V1.0 20200417
 * 锟斤拷一锟轿凤拷锟斤拷
 *
 ****************************************************************************************************
 */

#ifndef __USART_H
#define __USART_H

#include "stdio.h"
#include "./SYSTEM/sys/sys.h"


/******************************************************************************************/
/* 锟斤拷锟斤拷 锟斤拷 锟斤拷锟斤拷 锟斤拷锟斤拷 
 * 默锟斤拷锟斤拷锟斤拷锟経SART1锟斤拷.
 * 注锟斤拷: 通锟斤拷锟睫革拷锟斤拷12锟斤拷锟疥定锟斤拷,锟斤拷锟斤拷支锟斤拷USART1~UART7锟斤拷锟斤拷一锟斤拷锟斤拷锟斤拷.
 */
#define USART_TX_GPIO_PORT                  GPIOA
#define USART_TX_GPIO_PIN                   SYS_GPIO_PIN2
#define USART_TX_GPIO_CLK_ENABLE()          do{ RCC->APB2ENR |= 1 << 2; }while(0)   /* PA锟斤拷时锟斤拷使锟斤拷 */

#define USART_RX_GPIO_PORT                  GPIOA
#define USART_RX_GPIO_PIN                   SYS_GPIO_PIN3
#define USART_RX_GPIO_CLK_ENABLE()          do{ RCC->APB2ENR |= 1 << 2; }while(0)   /* PA锟斤拷时锟斤拷使锟斤拷 */

#define USART_UX                            USART2
#define USART_UX_IRQn                       USART2_IRQn
#define USART_UX_IRQHandler                 USART2_IRQHandler
#define USART_UX_CLK_ENABLE()               do{ RCC->APB1ENR |= 1 << 17; }while(0)  /* USART1 时锟斤拷使锟斤拷 */

/******************************************************************************************/

/******************************************************************************************/
/* UART 时锟斤拷源锟斤拷锟斤拷
 * 说锟斤拷:
 * - APB1时锟斤拷源: 锟斤拷锟斤拷USART2, USART3, UART4, UART5, 锟斤拷锟狡碉拷锟轿�36MHz (PCLK1)
 * - APB2时锟斤拷源: 锟斤拷锟斤拷USART1, 锟斤拷锟狡碉拷锟轿�72MHz (PCLK2)
 * 
 * 锟斤拷锟绞癸拷锟紸PB1时锟斤拷源锟斤拷UART (锟斤拷USART2/3/4/5), 锟斤拷使锟斤拷锟斤拷锟铰宏定锟斤拷:
 * #define USART_UX_CLK_ENABLE()  do{ RCC->APB1ENR |= 1 << 17; }while(0)  /* USART2 时锟斤拷使锟斤拷(APB1) */
 * 锟斤拷确锟斤拷锟斤拷锟斤拷usart_init锟斤拷锟斤拷锟斤拷时锟斤拷频锟绞诧拷锟斤拷锟斤拷36MHz
 */
#define USART_CLK_SOURCE_APB1_MAX_FREQ      36          /* APB1时锟斤拷源锟斤拷锟狡碉拷锟�: 36MHz */
#define USART_CLK_SOURCE_APB2_MAX_FREQ      72          /* APB2时锟斤拷源锟斤拷锟狡碉拷锟�: 72MHz */

/******************************************************************************************/


#define USART_REC_LEN               200         /* 锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟街斤拷锟斤拷 200 */
#define USART_EN_RX                 1           /* 使锟杰ｏ拷1锟斤拷/锟斤拷止锟斤拷0锟斤拷锟斤拷锟斤拷1锟斤拷锟斤拷 */


extern uint8_t  g_usart_rx_buf[USART_REC_LEN];  /* 锟斤拷锟秸伙拷锟斤拷,锟斤拷锟経SART_REC_LEN锟斤拷锟街斤拷.末锟街斤拷为锟斤拷锟叫凤拷 */
extern uint16_t g_usart_rx_sta;                 /* 锟斤拷锟斤拷状态锟斤拷锟� */

void usart_init(uint32_t pclk2, uint32_t bound);

void uart2_send_byte(uint8_t byte);
void uart2_send_string(char *str);

/* USART1 涓插彛鍑芥暟澹版槑 */
void Serial_Init(uint32_t Baud);          /* 涓插彛鍒濆鍖� */
void Serial_SendByte(uint8_t Byte);       /* 涓插彛鍙戦€佷竴涓瓧鑺� */
void Serial_SendString(char *String);     /* 涓插彛鍙戦€佷竴涓瓧绗︿覆 *//* 锟斤拷锟节筹拷始锟斤拷锟斤拷锟斤拷 */

#endif  
















