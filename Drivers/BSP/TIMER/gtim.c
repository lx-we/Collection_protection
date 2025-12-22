/**
 ****************************************************************************************************
 * @file        gtim.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.1
 * @date        2020-05-30
 * @brief       通用定时器 驱动代码
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
 * V1.0 20200530
 * 第一次发布
 * V1.1 20200530
 * 新增gtim_timx_pwm_chy_init函数
 *
 ****************************************************************************************************
 */

#include "./BSP/TIMER/gtim.h"
#include "./BSP/LED/led.h"

/**
 * @brief       通用定时器TIMX中断服务函数
 * @param       无
 * @retval      无
 */
void GTIM_TIMX_INT_IRQHandler(void)
{ 
    if (GTIM_TIMX_INT->SR & 0X0001)   /* 溢出中断 */
    {
        LED1_TOGGLE();
    }

    GTIM_TIMX_INT->SR &= ~(1 << 0); /* 清除中断标志位 */
} 

/**
 * @brief       通用定时器TIMX定时中断初始化函数
 * @note
 *              通用定时器的时钟来自APB1,当PPRE1 ≥ 2分频的时候
 *              通用定时器的时钟为APB1时钟的2倍, 而APB1为36M, 所以定时器时钟 = 72Mhz
 *              定时器溢出时间计算方法: Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *              Ft=定时器工作频率,单位:Mhz
 *
 * @param       arr: 自动重装值。
 * @param       psc: 时钟预分频数
 * @retval      无
 */
void gtim_timx_int_init(uint16_t arr, uint16_t psc)
{
    GTIM_TIMX_INT_CLK_ENABLE();
    GTIM_TIMX_INT->ARR = arr;           /* 设定计数器自动重装值 */
    GTIM_TIMX_INT->PSC = psc;           /* 设置预分频器  */
    GTIM_TIMX_INT->DIER |= 1 << 0;      /* 允许更新中断 */
    GTIM_TIMX_INT->CR1 |= 1 << 0;       /* 使能定时器TIMX */
    sys_nvic_init(1, 3, GTIM_TIMX_INT_IRQn, 2); /* 抢占1，子优先级3，组2 */
}

/**
 * @brief       通用定时器TIMX 通道Y PWM输出 初始化函数（使用PWM模式1）
 * @note
 *              通用定时器的时钟来自APB1,当PPRE1 ≥ 2分频的时候
 *              通用定时器的时钟为APB1时钟的2倍, 而APB1为36M, 所以定时器时钟 = 72Mhz
 *              定时器溢出时间计算方法: Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *              Ft=定时器工作频率,单位:Mhz
 *
 * @param       arr: 自动重装值。
 * @param       psc: 时钟预分频数
 * @retval      无
 */
void gtim_timx_pwm_chy_init(uint16_t arr, uint16_t psc)
{
    uint8_t chy = GTIM_TIMX_PWM_CHY;
    GTIM_TIMX_PWM_CHY_GPIO_CLK_ENABLE();    /* TIMX 通道 IO口时钟使能 */
    GTIM_TIMX_PWM_CHY_CLK_ENABLE();         /* TIMX 时钟使能 */

    sys_gpio_set(GTIM_TIMX_PWM_CHY_GPIO_PORT, GTIM_TIMX_PWM_CHY_GPIO_PIN,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_PU);    /* TIMX PWM CHY 引脚模式设置 */

    GTIM_TIMX_PWM->ARR = arr;       /* 设定计数器自动重装值 */
    GTIM_TIMX_PWM->PSC = psc;       /* 设置预分频器  */
    GTIM_TIMX_PWM->BDTR |= 1 << 15; /* 使能MOE位(仅TIM1/8 有此寄存器,必须设置MOE才能输出PWM), 其他通用定时器, 这个
                                     * 寄存器是无效的, 所以设置/不设置并不影响结果, 为了兼容这里统一改成设置MOE位
                                     */

    if (chy <= 2)
    {
        GTIM_TIMX_PWM->CCMR1 |= 6 << (4 + 8 * (chy - 1));   /* CH1/2 PWM模式1 */
        GTIM_TIMX_PWM->CCMR1 |= 1 << (3 + 8 * (chy - 1));   /* CH1/2 预装载使能 */
    }
    else if (chy <= 4)
    {
        GTIM_TIMX_PWM->CCMR2 |= 6 << (4 + 8 * (chy - 3));   /* CH3/4 PWM模式1 */
        GTIM_TIMX_PWM->CCMR2 |= 1 << (3 + 8 * (chy - 3));   /* CH3/4 预装载使能 */
    }

    GTIM_TIMX_PWM->CCER |= 1 << (4 * (chy - 1));        /* OCy 输出使能 */
    GTIM_TIMX_PWM->CCER |= 1 << (1 + 4 * (chy - 1));    /* OCy 低电平有效 */
    GTIM_TIMX_PWM->CR1 |= 1 << 7;   /* ARPE使能 */
    GTIM_TIMX_PWM->CR1 |= 1 << 0;   /* 使能定时器TIMX */
}

/**
 * @brief       TIM2 基础定时初始化（时基 + 更新中断）
 * @param       period: 自动重装值 (ARR = period - 1)
 * @param       prescaler: 预分频值 (PSC = prescaler - 1)
 * @retval      无
 */
void timer2_init(uint16_t period, uint16_t prescaler)
{
    /* 时钟使能，内部时钟默认即可 */
    RCC->APB1ENR |= 1 << 0;     /* TIM2 时钟 */

    TIM2->CR1 = 0;              /* 复位控制寄存器，向上计数，CKD=DIV1 */
    TIM2->PSC = prescaler - 1;  /* 预分频 */
    TIM2->ARR = period - 1;     /* 自动重装 */
    TIM2->EGR |= 1 << 0;        /* 产生更新事件，装载预分频和ARR */

    TIM2->SR &= ~(1 << 0);      /* 清除更新中断标志 */
    TIM2->DIER |= 1 << 0;       /* 允许更新中断 */

    /* NVIC: 组2，抢占/响应优先级 3/3 */
    sys_nvic_init(3, 3, TIM2_IRQn, 2);

    TIM2->CR1 |= 1 << 0;        /* 使能计数器 */
}

/**
 * @brief       WS2812B PWM+DMA初始化函数 (TIM3_CH1, PA6)
 * @note        基于gtim_timx_pwm_chy_init()，但针对WS2812B的特殊需求进行调整：
 *              1. 禁用预装载 (OC1PE = 0)，使DMA更新CCR1时立即生效
 *              2. 禁用ARPE，使ARR更新立即生效
 *              3. 高电平有效 (CC1P = 0)
 *              4. 先禁用输出和定时器，等DMA配置好后再使能
 *              5. 配置DMA请求使能
 * 
 * @param       arr: 自动重装值 (PWM周期 = arr + 1)
 * @param       psc: 时钟预分频数
 * @retval      无
 */
void gtim_ws2812_pwm_dma_init(uint16_t arr, uint16_t psc)
{
    uint8_t chy = GTIM_WS2812_TIM_CHANNEL;  /* 通道1 */
    
    /* 使能时钟 */
    GTIM_WS2812_TIM_GPIO_CLK_ENABLE();    /* PA口时钟使能 */
    GTIM_WS2812_TIM_CLK_ENABLE();         /* TIM3时钟使能 */
    
    /* 配置PA6为复用推挽输出 (TIM3_CH1) */
    sys_gpio_set(GTIM_WS2812_TIM_GPIO_PORT, GTIM_WS2812_TIM_GPIO_PIN,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_NONE);
    
    /* 配置TIM3预分频器和自动重装载值 */
    GTIM_WS2812_TIM->ARR = arr;           /* 设定计数器自动重装值 */
    GTIM_WS2812_TIM->PSC = psc;           /* 设置预分频器 */
    GTIM_WS2812_TIM->CNT = 0;             /* 计数器清零 */
    
    /* 配置TIM3_CH1为PWM模式1 (复用gtim_timx_pwm_chy_init的逻辑) */
    if (chy <= 2)
    {
        GTIM_WS2812_TIM->CCMR1 |= 6 << (4 + 8 * (chy - 1));   /* CH1/2 PWM模式1 */
        GTIM_WS2812_TIM->CCMR1 &= ~(1 << (3 + 8 * (chy - 1))); /* CH1/2 预装载禁用 (WS2812B特殊需求) */
    }
    else if (chy <= 4)
    {
        GTIM_WS2812_TIM->CCMR2 |= 6 << (4 + 8 * (chy - 3));   /* CH3/4 PWM模式1 */
        GTIM_WS2812_TIM->CCMR2 &= ~(1 << (3 + 8 * (chy - 3))); /* CH3/4 预装载禁用 */
    }
    
    /* WS2812B特殊配置：先禁用输出，高电平有效 */
    GTIM_WS2812_TIM->CCER &= ~(1 << (4 * (chy - 1)));        /* OCy 输出禁用 (WS2812B特殊需求) */
    GTIM_WS2812_TIM->CCER &= ~(1 << (1 + 4 * (chy - 1)));    /* OCy 高电平有效 (CC1P = 0) */
    
    /* WS2812B特殊配置：禁用ARPE，使ARR更新立即生效 */
    GTIM_WS2812_TIM->CR1 &= ~(1 << 7);   /* ARPE禁用 (WS2812B特殊需求) */
    
    GTIM_WS2812_TIM->CCR1 = 0;              /* 初始比较值 = 0 */
    
    /* 配置TIM3 DMA请求 - TIM3_CH1使用CC1DE触发DMA1_Channel6 */
    GTIM_WS2812_TIM->DIER |= 1 << 9;      /* CC1DE = 1, 通道1 DMA请求使能 (位9) */
    
    /* WS2812B特殊配置：先禁用定时器，等DMA配置好后再使能 */
    GTIM_WS2812_TIM->CR1 &= ~(1 << 0);    /* CEN = 0, 禁用定时器 */
}


