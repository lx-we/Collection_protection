/**
 ****************************************************************************************************
 * @file        gtim.h
 * @author      閿熸枻鎷烽敓鏂ゆ嫹鍘熼敓鏂ゆ嫹閿熻剼璁规嫹(ALIENTEK)
 * @version     V1.1
 * @date        2020-05-30
 * @brief       閫氶敓鐭鎷锋椂閿熸枻鎷� 閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹
 * @license     Copyright (c) 2020-2032, 閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸帴绉戠》鎷烽敓鏂ゆ嫹閿熺潾鐧告嫹鍙�
 ****************************************************************************************************
 * @attention
 *
 * 瀹為敓鏂ゆ嫹骞冲彴:閿熸枻鎷烽敓鏂ゆ嫹鍘熼敓鏂ゆ嫹 STM32F103閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷�
 * 閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽:www.yuanzige.com
 * 閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷峰潧:www.openedv.com
 * 閿熸枻鎷峰徃閿熸枻鎷峰潃:www.alientek.com
 * 閿熸枻鎷烽敓鏂ゆ嫹閿熻锟�:openedv.taobao.com
 *
 * 閿熺潾闈╂嫹璇撮敓鏂ゆ嫹
 * V1.0 20200530
 * 閿熸枻鎷蜂竴閿熻娇鍑ゆ嫹閿熸枻鎷�
 * V1.1 20200530
 * 閿熸枻鎷烽敓鏂ゆ嫹gtim_timx_pwm_chy_init閿熸枻鎷烽敓鏂ゆ嫹
 *
 ****************************************************************************************************
 */

#ifndef __GTIM_H
#define __GTIM_H

#include "./SYSTEM/sys/sys.h"

/******************************************************************************************/
/* 閫氶敓鐭鎷锋椂閿熸枻鎷� 閿熸枻鎷烽敓鏂ゆ嫹 */

/* TIMX 閿熷彨鏂鎷烽敓鏂ゆ嫹 
 * 榛橀敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熺祴IM2~TIM5.
 * 娉ㄩ敓鏂ゆ嫹: 閫氶敓鏂ゆ嫹閿熺潾闈╂嫹閿熸枻鎷�4閿熸枻鎷烽敓鐤ュ畾閿熸枻鎷�,閿熸枻鎷烽敓鏂ゆ嫹鏀敓鏂ゆ嫹TIM1~TIM8閿熸枻鎷烽敓鏂ゆ嫹涓€閿熸枻鎷烽敓鏂ゆ嫹鏃堕敓鏂ゆ嫹.
 */
 
#define GTIM_TIMX_INT                       TIM3
#define GTIM_TIMX_INT_IRQn                  TIM3_IRQn
#define GTIM_TIMX_INT_IRQHandler            TIM3_IRQHandler
#define GTIM_TIMX_INT_CLK_ENABLE()          do{ RCC->APB1ENR |= 1 << 1; }while(0)   /* TIM3 鏃堕敓鏂ゆ嫹浣块敓鏂ゆ嫹 */


/* TIMX PWM閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓锟� 
 * 閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熺祰WM閿熸枻鎷烽敓鏂ゆ嫹LED0(RED)閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷� (閿熸枻鎷疯涓€閿熸枻鎷烽敓鑴氬府鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹 PA0 閿熸枻鎷� PA8 !!!)
 * 榛橀敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熺祴IM2~TIM5
 * 娉ㄩ敓鏂ゆ嫹: 
 * 1, 閫氶敓鏂ゆ嫹閿熺潾闈╂嫹閿熻В鍑犻敓鏂ゆ嫹閿熺枼瀹氶敓鏂ゆ嫹,閿熸枻鎷烽敓鏂ゆ嫹鏀敓鏂ゆ嫹TIM1~TIM8閿熸枻鎷烽敓鏂ゆ嫹涓€閿熸枻鎷烽敓鏂ゆ嫹鏃堕敓鏂ゆ嫹,閿熸枻鎷烽敓鏂ゆ嫹涓€閿熸枻鎷稩O閿熸枻鎷烽敓鏂ゆ嫹閿熺祰WM
 * 1, TIM2_CH1閫氶敓鏂ゆ嫹PA0閿熸枻鎷烽敓绲嘩M, 閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓瑙掔鎷稬ED0閿熸枻鎷烽敓鏂ゆ嫹PA8閿熸枻鎷�, 閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷疯閿熺煫鏉滃府鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹 PA0 閿熸枻鎷� PA8, 閿熸帴璁规嫹閿熸枻鎷烽敓鏂ゆ嫹LED0閿熸枻鎷烽敓鏂ゆ嫹
 */
#define GTIM_TIMX_PWM_CHY_GPIO_PORT         GPIOA
#define GTIM_TIMX_PWM_CHY_GPIO_PIN          SYS_GPIO_PIN0
#define GTIM_TIMX_PWM_CHY_GPIO_CLK_ENABLE() do{ RCC->APB2ENR |= 1 << 2; }while(0)   /* PA閿熸枻鎷锋椂閿熸枻鎷蜂娇閿熸枻鎷� */

#define GTIM_TIMX_PWM                       TIM2 
#define GTIM_TIMX_PWM_CHY                   1                           /* 閫氶敓鏂ゆ嫹Y,  1<= Y <=4 */
#define GTIM_TIMX_PWM_CHY_CCRX              GTIM_TIMX_PWM->CCR1         /* 閫氶敓鏂ゆ嫹Y閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷峰唹闇炴媷閿熸枻鎷烽敓锟� */
#define GTIM_TIMX_PWM_CHY_CLK_ENABLE()      do{ RCC->APB1ENR |= 1 << 0; }while(0)   /* TIM2 鏃堕敓鏂ゆ嫹浣块敓鏂ゆ嫹 */

/******************************************************************************************/

void gtim_timx_int_init(uint16_t arr, uint16_t psc);        /* 通用定时器 定时中断初始化函数 */
void gtim_timx_pwm_chy_init(uint16_t arr, uint16_t psc);    /* 通用定时器 PWM初始化函数 */
void timer2_init(uint16_t period, uint16_t prescaler);      /* TIM2 基础定时初始化 */

/* WS2812B PWM鐎规碍妞傞崳銊ョ暰娑旓拷 (TIM3_CH1, PA6) */
#define GTIM_WS2812_TIM                     TIM3
#define GTIM_WS2812_TIM_CHANNEL            1                           /* 闁岸浜�1 */
#define GTIM_WS2812_TIM_GPIO_PORT          GPIOA
#define GTIM_WS2812_TIM_GPIO_PIN           SYS_GPIO_PIN6
#define GTIM_WS2812_TIM_GPIO_CLK_ENABLE()  do{ RCC->APB2ENR |= 1 << 2; }while(0)   /* PA閸欙絾妞傞柦鐔跺▏閼筹拷 */
#define GTIM_WS2812_TIM_CLK_ENABLE()       do{ RCC->APB1ENR |= 1 << 1; }while(0)   /* TIM3 閺冨爼鎸撴担鑳厴 */
#define GTIM_WS2812_TIM_CCRX               GTIM_WS2812_TIM->CCR1        /* 闁岸浜�1濮ｆ棁绶濈€靛嫬鐡ㄩ崳锟� */

void gtim_ws2812_pwm_dma_init(uint16_t arr, uint16_t psc);  /* WS2812B PWM+DMA閸掓繂顫愰崠鏍у毐閺侊拷 */

#endif

















