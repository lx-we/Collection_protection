#include "./BSP/SR602/sr602.h"
#include "./SYSTEM/usart/usart.h"
#include <stdint.h>

static uint8_t sr602_last_status = 0;  /* SR602上一次的状态 */
uint8_t human_detected_status = 0;     /* 人体检测状态：0-无感应，1-感应到人体 */
/**
 * @brief       初始化SR602的IO口和TIM1定时器中断
 * @param       无
 * @retval      无
 */
void sr602_init(void)
{
    SR602_OUT_GPIO_CLK_ENABLE();     /* 开启SR602输出引脚时钟 */

    /* SR602_OUT引脚模式设置,浮空输入模式,用于读取模块输出 */
    sys_gpio_set(SR602_OUT_GPIO_PORT, SR602_OUT_GPIO_PIN,
                 SYS_GPIO_MODE_IN, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_NONE);

    /* 初始化SR602状态 */
    sr602_last_status = SR602_OUT_IN;

    /* 使能TIM1时钟 */
    RCC->APB2ENR |= 1 << 11;  /* TIM1时钟使能 */

    /* 配置TIM1定时器: 100ms中断一次 */
    /* 系统时钟72MHz, APB2不分频, TIM1时钟 = 72MHz */
    /* 预分频器PSC = 7199, 定时器时钟 = 72MHz / 7200 = 10kHz */
    /* 自动重装载值ARR = 1000, 中断周期 = 1000 / 10kHz = 100ms */
    TIM1->PSC = 7199;          /* 预分频器, 7200分频 */
    TIM1->ARR = 1000;          /* 自动重装载值, 1000个计数 */
    TIM1->CNT = 0;             /* 计数器清零 */
    TIM1->DIER |= 1 << 0;      /* 使能更新中断 */
    TIM1->CR1 |= 1 << 0;       /* 使能定时器 */

    /* 配置NVIC中断 */
    sys_nvic_init(2, 2, TIM1_UP_IRQn, 2);  /* 抢占优先级2,子优先级2,中断通道TIM1_UP_IRQn,分组2 */
}

/**
 * @brief       获取SR602状态
 * @param       无
 * @retval      0, 无感应(低电平)
 *              1, 感应到人体(高电平)
 */
uint8_t sr602_get_status(void)
{
    return SR602_OUT_IN;  /* 读取输出引脚状态,高电平表示感应到人体 */
}

/**
 * @brief       TIM1更新中断服务函数(定时检测SR602状态)
 * @param       无
 * @retval      无
 */
void TIM1_UP_IRQHandler(void)
{
    if (TIM1->SR & 0x01)  /* 检查更新中断标志位 */
    {
        TIM1->SR &= ~(1 << 0);  /* 清除更新中断标志位 */
        
        /* 读取SR602当前状态 */
        uint8_t sr602_status = SR602_OUT_IN;
        
        /* 检测状态变化 */
        if (sr602_status != sr602_last_status)
        {
            if (sr602_status == 1)  /* 检测到人体 */
            {
                printf("SR602: Human detected! (PC7=HIGH)\r\n");
                human_detected_status = 1;
            }
            else  /* 人体离开 */
            {
                printf("SR602: No human detected (PC7=LOW)\r\n");
                human_detected_status = 0;
            }
            sr602_last_status = sr602_status;  /* 更新状态 */
        }
    }
}

