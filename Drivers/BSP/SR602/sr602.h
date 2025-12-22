#ifndef __SR602_H
#define __SR602_H

#include "./SYSTEM/sys/sys.h"

extern uint8_t  human_detected_status;

/******************************************************************************************/
/* SR602 引脚 定义 */

#define SR602_OUT_GPIO_PORT                  GPIOC
#define SR602_OUT_GPIO_PIN                   SYS_GPIO_PIN7
#define SR602_OUT_GPIO_CLK_ENABLE()          do{ RCC->APB2ENR |= 1 << 4; }while(0)   /* PC口时钟使能 */

/******************************************************************************************/
/* IO操作函数 */
#define SR602_OUT_IN         sys_gpio_pin_get(SR602_OUT_GPIO_PORT, SR602_OUT_GPIO_PIN)     /* 读取SR602输出引脚 */

/******************************************************************************************/
/* 函数声明 */
void sr602_init(void);                      /* 初始化SR602 */
uint8_t sr602_get_status(void);             /* 获取SR602状态: 0-无感应, 1-感应到人体 */

#endif

