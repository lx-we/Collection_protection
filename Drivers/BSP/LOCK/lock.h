#ifndef __LOCK_H
#define __LOCK_H

#include "./SYSTEM/sys/sys.h"

/******************************************************************************************/
/* 电磁锁 引脚定义 */

#define LOCK_GPIO_PORT                  GPIOA
#define LOCK_GPIO_PIN                   SYS_GPIO_PIN3
#define LOCK_GPIO_CLK_ENABLE()          do{ RCC->APB2ENR |= 1 << 2; }while(0)   /* PA时钟使能 */

/******************************************************************************************/

/* 电磁锁端口操作定义 */
/* 开漏模式下，直接操作ODR寄存器更可靠 */
#define LOCK(x)         do{ \
                            if(x) \
                                LOCK_GPIO_PORT->BSRR = LOCK_GPIO_PIN;      /* 设置为高电平(ODR=1) */ \
                            else \
                                LOCK_GPIO_PORT->BSRR = (uint32_t)LOCK_GPIO_PIN << 16;  /* 设置为低电平(ODR=0) */ \
                        }while(0)      /* 电磁锁控制 */
#define LOCK_GET()      ((LOCK_GPIO_PORT->ODR & LOCK_GPIO_PIN) ? 1 : 0)        /* 读取电磁锁输出状态(读取ODR寄存器，开漏模式下更准确) */

/* 电磁锁状态定义 */
/* 注意：使用NC（常闭）触点 + 低电平触发（跳线帽接LOW） */
/* 使用NC+低电平触发时：PA3=低电平→继电器吸合→NC断开→上锁，PA3=高电平→继电器断开→NC连通→解锁 */
#define LOCK_ON         0       /* 上锁(使用NC+低电平触发时:PA3=低电平,继电器吸合,NC断开) */
#define LOCK_OFF        1       /* 解锁(使用NC+低电平触发时:PA3=高电平,继电器断开,NC连通) */

/* 电磁锁操作宏定义 */
#define LOCK_LOCK()     LOCK(LOCK_ON)      /* 上锁 */
#define LOCK_UNLOCK()   LOCK(LOCK_OFF)     /* 解锁 */
#define LOCK_TOGGLE()   do{ LOCK_GPIO_PORT->ODR ^= LOCK_GPIO_PIN; }while(0)      /* 状态翻转 */


void lock_init(void);          /* 初始化电磁锁 */
void lock_set(uint8_t status); /* 设置电磁锁状态: 0-上锁, 1-解锁 */
uint8_t lock_get(void);        /* 获取电磁锁状态: 返回值 0-上锁, 1-解锁 */

#endif








