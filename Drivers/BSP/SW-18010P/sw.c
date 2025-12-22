/**
 ****************************************************************************************************
 * @file        sw.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2024-01-01
 * @brief       SW-18010P震动传感器 驱动代码
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
 * V1.0 20240101
 * 第一次发布
 *
 ****************************************************************************************************
 */

#include "./BSP/SW-18010P/sw.h"
#include "./SYSTEM/usart/usart.h"

/**
 * @brief       初始化SW-18010P的IO口,配置外部中断
 * @param       无
 * @retval      无
 */
void sw_init(void)
{
    SW_DO_GPIO_CLK_ENABLE();     /* 开启SW-18010P输出引脚时钟 */

    /* SW_DO引脚模式设置,浮空输入模式,用于读取模块数字输出 */
    sys_gpio_set(SW_DO_GPIO_PORT, SW_DO_GPIO_PIN,
                 SYS_GPIO_MODE_IN, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_NONE);

    /* 配置PC6为外部中断,双边沿触发(上升沿:检测到震动,下降沿:震动消失) */
    sys_nvic_ex_config(SW_DO_GPIO_PORT, SW_DO_GPIO_PIN, SYS_GPIO_BTIR);
    
    /* 配置NVIC中断,PC6对应EXTI6,使用EXTI9_5_IRQn中断 */
    sys_nvic_init(2, 2, EXTI9_5_IRQn, 2);  /* 抢占优先级2,子优先级2,中断通道EXTI9_5_IRQn,分组2 */
}

/**
 * @brief       获取SW-18010P状态
 * @param       无
 * @retval      0, 无震动(低电平)
 *              1, 检测到震动(高电平)
 */
uint8_t sw_get_status(void)
{
    return SW_DO_IN;  /* 读取输出引脚状态,高电平表示检测到震动 */
}

/**
 * @brief       EXTI9_5中断服务函数(处理PC6的外部中断)
 * @param       无
 * @retval      无
 */
void EXTI9_5_IRQHandler(void)
{
    if (EXTI->PR & (1 << 6))  /* 判断是否是EXTI6(PC6)触发的中断 */
    {
        EXTI->PR = 1 << 6;    /* 清除中断标志位 */
        
        /* 读取当前引脚状态判断是上升沿还是下降沿 */
        if (SW_DO_IN == 1)  /* 上升沿:检测到震动 */
        {
            printf("SW-18010P: Vibration detected! (PC6=HIGH, Rising Edge)\r\n");
        }
        else  /* 下降沿:震动消失 */
        {
            printf("SW-18010P: No vibration (PC6=LOW, Falling Edge)\r\n");
        }
    }
}

