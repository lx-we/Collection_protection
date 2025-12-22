#include "./BSP/LOCK/lock.h"
uint8_t  Lock_status = 0;
/**
 * @brief       初始化电磁锁的IO口, 并使能时钟
 * @param       无
 * @retval      无
 */
void lock_init(void)
{
    LOCK_GPIO_CLK_ENABLE(); /* 电磁锁时钟使能 */

    sys_gpio_set(LOCK_GPIO_PORT, LOCK_GPIO_PIN,
                 SYS_GPIO_MODE_OUT, SYS_GPIO_OTYPE_OD, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PU);   /* PA3配置为开漏输出模式，上拉 */

    /* 立即设置为高电平（解锁状态），确保继电器断开 */
    LOCK_GPIO_PORT->BSRR = LOCK_GPIO_PIN;  /* 设置为高电平 */

    LOCK(LOCK_OFF);   /* 确认解锁状态(使用NC+低电平触发时:PA3=高电平,继电器断开,NC连通,电磁锁通电) */
}

/**
 * @brief       电磁锁上锁
 * @param       无
 * @retval      无
 */
void lock_lock(void)
{
    LOCK(LOCK_ON);
    Lock_status = 1;
}

/**
 * @brief       电磁锁解锁
 * @param       无
 * @retval      无
 */
void lock_unlock(void)
{
    LOCK(LOCK_OFF);
    Lock_status = 0;
}

/**
 * @brief       设置电磁锁状态
 * @param       status: 0-上锁, 1-解锁
 * @retval      无
 */
void lock_set(uint8_t status)
{
    if (status == 0)
    {
        lock_lock();
    }
    else
    {
        lock_unlock();
    }
}

/**
 * @brief       获取电磁锁当前状态
 * @param       无
 * @retval      0-上锁, 1-解锁
 */
uint8_t lock_get(void)
{
    return LOCK_GET();
}








