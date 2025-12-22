/**
 ****************************************************************************************************
 * @file        gtim.h
 * @author      锟斤拷锟斤拷原锟斤拷锟脚讹拷(ALIENTEK)
 * @version     V1.1
 * @date        2020-05-30
 * @brief       通锟矫讹拷时锟斤拷 锟斤拷锟斤拷锟斤拷锟斤拷
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
 * V1.0 20200530
 * 锟斤拷一锟轿凤拷锟斤拷
 * V1.1 20200530
 * 锟斤拷锟斤拷gtim_timx_pwm_chy_init锟斤拷锟斤拷
 *
 ****************************************************************************************************
 */

#ifndef __GTIM_H
#define __GTIM_H

#include "./SYSTEM/sys/sys.h"

/******************************************************************************************/
/* 通锟矫讹拷时锟斤拷 锟斤拷锟斤拷 */

/* TIMX 锟叫断讹拷锟斤拷 
 * 默锟斤拷锟斤拷锟斤拷锟絋IM2~TIM5.
 * 注锟斤拷: 通锟斤拷锟睫革拷锟斤拷4锟斤拷锟疥定锟斤拷,锟斤拷锟斤拷支锟斤拷TIM1~TIM8锟斤拷锟斤拷一锟斤拷锟斤拷时锟斤拷.
 */
 
#define GTIM_TIMX_INT                       TIM3
#define GTIM_TIMX_INT_IRQn                  TIM3_IRQn
#define GTIM_TIMX_INT_IRQHandler            TIM3_IRQHandler
#define GTIM_TIMX_INT_CLK_ENABLE()          do{ RCC->APB1ENR |= 1 << 1; }while(0)   /* TIM3 时锟斤拷使锟斤拷 */


/* TIMX PWM锟斤拷锟斤拷锟斤拷锟� 
 * 锟斤拷锟斤拷锟斤拷锟斤拷锟絇WM锟斤拷锟斤拷LED0(RED)锟斤拷锟斤拷锟斤拷 (锟斤拷要一锟斤拷锟脚帮拷锟斤拷锟斤拷锟斤拷 PA0 锟斤拷 PA8 !!!)
 * 默锟斤拷锟斤拷锟斤拷锟絋IM2~TIM5
 * 注锟斤拷: 
 * 1, 通锟斤拷锟睫革拷锟解几锟斤拷锟疥定锟斤拷,锟斤拷锟斤拷支锟斤拷TIM1~TIM8锟斤拷锟斤拷一锟斤拷锟斤拷时锟斤拷,锟斤拷锟斤拷一锟斤拷IO锟斤拷锟斤拷锟絇WM
 * 1, TIM2_CH1通锟斤拷PA0锟斤拷锟絇WM, 锟斤拷锟斤拷锟斤拷锟角碉拷LED0锟斤拷锟斤拷PA8锟斤拷, 锟斤拷锟斤拷锟斤拷要锟矫杜帮拷锟斤拷锟斤拷锟斤拷 PA0 锟斤拷 PA8, 锟接讹拷锟斤拷锟斤拷LED0锟斤拷锟斤拷
 */
#define GTIM_TIMX_PWM_CHY_GPIO_PORT         GPIOA
#define GTIM_TIMX_PWM_CHY_GPIO_PIN          SYS_GPIO_PIN0
#define GTIM_TIMX_PWM_CHY_GPIO_CLK_ENABLE() do{ RCC->APB2ENR |= 1 << 2; }while(0)   /* PA锟斤拷时锟斤拷使锟斤拷 */

#define GTIM_TIMX_PWM                       TIM2 
#define GTIM_TIMX_PWM_CHY                   1                           /* 通锟斤拷Y,  1<= Y <=4 */
#define GTIM_TIMX_PWM_CHY_CCRX              GTIM_TIMX_PWM->CCR1         /* 通锟斤拷Y锟斤拷锟斤拷锟斤拷冉霞拇锟斤拷锟� */
#define GTIM_TIMX_PWM_CHY_CLK_ENABLE()      do{ RCC->APB1ENR |= 1 << 0; }while(0)   /* TIM2 时锟斤拷使锟斤拷 */

/******************************************************************************************/

void gtim_timx_int_init(uint16_t arr, uint16_t psc);        /* 通锟矫讹拷时锟斤拷 锟斤拷时锟叫断筹拷始锟斤拷锟斤拷锟斤拷 */
void gtim_timx_pwm_chy_init(uint16_t arr, uint16_t psc);    /* 通锟矫讹拷时锟斤拷 PWM锟斤拷始锟斤拷锟斤拷锟斤拷 */

/* WS2812B PWM瀹氭椂鍣ㄥ畾涔� (TIM3_CH1, PA6) */
#define GTIM_WS2812_TIM                     TIM3
#define GTIM_WS2812_TIM_CHANNEL            1                           /* 閫氶亾1 */
#define GTIM_WS2812_TIM_GPIO_PORT          GPIOA
#define GTIM_WS2812_TIM_GPIO_PIN           SYS_GPIO_PIN6
#define GTIM_WS2812_TIM_GPIO_CLK_ENABLE()  do{ RCC->APB2ENR |= 1 << 2; }while(0)   /* PA鍙ｆ椂閽熶娇鑳� */
#define GTIM_WS2812_TIM_CLK_ENABLE()       do{ RCC->APB1ENR |= 1 << 1; }while(0)   /* TIM3 鏃堕挓浣胯兘 */
#define GTIM_WS2812_TIM_CCRX               GTIM_WS2812_TIM->CCR1        /* 閫氶亾1姣旇緝瀵勫瓨鍣� */

void gtim_ws2812_pwm_dma_init(uint16_t arr, uint16_t psc);  /* WS2812B PWM+DMA鍒濆鍖栧嚱鏁� */

#endif

















