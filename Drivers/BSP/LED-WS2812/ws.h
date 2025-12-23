/**
 ****************************************************************************************************
 * @file        ws.h
 * @author      User
 * @version     V1.0
 * @date        2024
 * @brief       WS2812B RGB LED灯带驱动 (PWM+DMA方式)
 * @license     Copyright (c) 2024
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 STM32F103开发板
 * 引脚: PA6 (TIM3_CH1) - 用于输出WS2812B数据
 * 系统时钟: 72MHz
 *
 * WS2812B时序要求:
 * - 0码: 高电平0.3us, 低电平0.9us (总共1.2us)
 * - 1码: 高电平0.6us, 低电平0.6us (总共1.2us)
 * - 复位码: 低电平至少50us
 *
 * PWM配置:
 * - PWM频率: 72MHz / 90 = 800kHz, 每个周期1.25us
 * - 0码: CCR = 30 (高电平30个周期, 约0.375us)
 * - 1码: CCR = 60 (高电平60个周期, 约0.75us)
 *
 ****************************************************************************************************
 */

#ifndef __WS_H
#define __WS_H

#include "./SYSTEM/sys/sys.h"


/******************************************************************************************/
/* WS2812B PWM配置定义 */
/* 注意：定时器相关定义在 gtim.h 中，这里只保留DMA相关定义 */

#include "./BSP/TIMER/gtim.h"  /* 包含定时器驱动头文件 */

/* 使用gtim.h中定义的定时器宏 */
#define WS2812_TIM                    GTIM_WS2812_TIM
#define WS2812_TIM_GPIO_PORT         GTIM_WS2812_TIM_GPIO_PORT
#define WS2812_TIM_GPIO_PIN          GTIM_WS2812_TIM_GPIO_PIN

/* DMA配置定义 */
#define WS2812_DMA                    DMA1
#define WS2812_DMA_CLK_ENABLE()      do{ RCC->AHBENR |= 1 << 0; }while(0)       /* DMA1时钟使能 */
#define WS2812_DMA_CHANNEL            DMA1_Channel6                              /* TIM3_CH1使用DMA1_Channel6 */


extern uint8_t RBGLED_status;

/******************************************************************************************/
/* RGBLED的数据高低电平逻辑定义 */
#define HIGH_LEVEL      (uint16_t)60     /* CCR为占空比为三分之二代表逻辑1 */
#define LOW_LEVEL       (uint16_t)30     /* CCR为占空比为三分之一代表逻辑0 */

#define LED_NUM         30               /* 灯珠的个数 */
#define DATA_SIZE       24               /* 一个RGB数据的位数，3*8 */

/******************************************************************************************/
/* WS2812B PWM编码定义 */
/* PWM频率: 72MHz / 90 = 800kHz, 每个周期1.25us */
/* 0码: CCR = 30 (高电平30个周期, 约0.375us) */
/* 1码: CCR = 60 (高电平60个周期, 约0.75us) */
#define WS2812_CODE_0                 LOW_LEVEL    /* 0码 */
#define WS2812_CODE_1                 HIGH_LEVEL   /* 1码 */

/* PWM周期值 */
#define WS2812_PWM_PERIOD             90    /* ARR = 90-1 = 89 */

/* 最大LED数量 */
#define WS2812_MAX_LEDS               256

/* 实际使用的LED数量 (根据实际灯带修改) */
#define WS2812_NUM_LEDS                LED_NUM

/******************************************************************************************/
/* RGB888颜色值 (GRB格式) */
#define GRB888_RED          0x00FF00        /* 红色 */
#define GRB888_GREEN        0xFF0000        /* 绿色 */
#define GRB888_BLUE         0x0000FF        /* 蓝色 */
#define GRB888_BLACK        0x000000        /* 黑色 */
#define GRB888_WHITE        0xFFFFFF        /* 白色 */
#define GRB888_YELLOW       0xFFFF00        /* 黄色 */
#define GRB888_IRED         0x5CCD5C        /* 浅红色 */
#define GRB888_ORANGE       0xA5FF00        /* 橙色 */
#define GRB888_PURPLE       0x008080        /* 紫色 */
#define GRB888_PING         0xB6FFC1        /* 浅粉色 */
#define GRB888_CYAN         0xFF00FF        /* 青色 */
#define GRB888_PBLUE        0x80008C        /* 孔雀蓝 */
#define GRB888_VIOLET       0x008BFF        /* 紫罗兰 */
#define GRB888_WARM_YELLOW  0xC8FF64        /* 暖黄色 (G=200, R=255, B=100) */

/******************************************************************************************/
/* 函数声明 */
void ws2812_init(void);                          /* 初始化WS2812B (PWM+DMA) */
void ws2812_display(uint32_t *color_buf, uint16_t len);  /* 发送24位GRB888颜色值数组 */
void ws2812_reset(void);                         /* 复位WS2812B (保持低电平280us) */
void ws2812_send_buffer(uint8_t *buffer, uint16_t len);   /* 发送缓冲区数据 (GRB格式,兼容旧接口) */
void ws2812_clear(uint16_t num_leds);            /* 清除所有LED (设置为黑色) */
void ws2812_set_all(uint8_t r, uint8_t g, uint8_t b, uint16_t num_leds);  /* 设置所有LED为相同颜色 */
uint8_t ws2812_is_busy(void);                   /* 检查DMA传输是否完成 */
uint32_t ws2812_color_change_bright(uint32_t rgb, float k);  /* 改变颜色的亮度值 */
void ws2812_set_brightness(uint32_t *color_buf, uint16_t len, float brightness);  /* 设置所有LED的亮度并显示 */
void ws2812_set_warm_yellow(uint8_t percent);  /* 设置所有LED为暖黄色，亮度可调 (0-100%) */
void ws2812_show_black(void);                   /* 不显示颜色，关灯 */

#endif
