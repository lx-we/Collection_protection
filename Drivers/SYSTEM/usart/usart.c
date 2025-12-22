/**
 ****************************************************************************************************
 * @file        usmart_port.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V3.5
 * @date        2020-12-20
 * @brief       USMART 串口调试组件
 *
 *              USMART是由ALIENTEK开发的一个灵巧的串口调试互交组件,通过 它,你可以通过串口助手调用程
 *              序里面的任何函数,并执行.因此,你可以随意更改函数的输入参数(支持数字(10/16进制,支持负数)
 *              、字符串、函数入口地址等作为参数),单个函数最多支持10个输入参数,并支持函数返 回值显示.
 *              V2.1版本以后新增hex和dec两个指令.他们可以用于设置函数参数的显示格式.也可以用于数据的进
 *              制转换,例如:
 *              输入"hex 100"  会在串口调试助手上看到 HEX 0X64.
 *              输入"dec 0X64" 会在串口调试助手上看到 DEC 100.
 *   @note
 *              USMART资源占用情况@MDK 3.80A@2.0版本：
 *              FLASH:4K~K字节(通过USMART_USE_HELP和USMART_USE_WRFUNS设置)
 *              SRAM:72字节(最少的情况下)
 *              SRAM计算公式:   SRAM=PARM_LEN+72-4  其中PARM_LEN必须大于等于4.
 *              应该保证堆栈不小于100个字节.
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 * 
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 * 修改说明 
 * 
 * V3.4之前版本详细修改说明见USMART文件夹下的:readme.txt
 * 
 * V3.4 20200324
 * 1, 新增usmart_port.c和usmart_port.h,用于管理USMART的移植,方便修改
 * 2, 修改变量命名方式为: uint8_t, uint16_t, uint32_t
 * 3, 修改usmart_reset_runtime为usmart_timx_reset_time
 * 4, 修改usmart_get_runtime为usmart_timx_get_time
 * 5, 修改usmart_scan函数实现方式,改成由usmart_get_input_string获取数据流
 * 6, 修改printf函数为USMART_PRINTF宏定义
 * 7, 修改定时扫描相关函数,改用宏定义方式,方便移植
 *
 * V3.5 20201220
 * 1，修改部分代码以支持AC6编译器
 ****************************************************************************************************
 */
 
#ifndef __USMART_PORT_H
#define __USMART_PORT_H

#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"




/******************************************************************************************/
/* 用户配置参数 */


#define MAX_FNAME_LEN           30      /* 函数名最大长度，应该设置为不小于最长函数名的长度。 */
#define MAX_PARM                10      /* 最大为10个参数 ,修改此参数,必须修改usmart_exe与之对应. */
#define PARM_LEN                200     /* 所有参数之和的长度不超过PARM_LEN个字节,注意串口接收部分要与之对应(不小于PARM_LEN) */


#define USMART_ENTIMX_SCAN      1       /* 使用TIM的定时中断来扫描SCAN函数,如果设置为0,需要自己实现隔一段时间扫描一次scan函数.
                                         * 注意:如果要用runtime统计功能,必须设置USMART_ENTIMX_SCAN为1!!!!
                                         */

#define USMART_USE_HELP         1       /* 使用帮助，该值设为0，可以节省近700个字节，但是将导致无法显示帮助信息。 */
#define USMART_USE_WRFUNS       1       /* 使用读写函数,使能这里,可以读取任何地址的值,还可以写寄存器的值. */

#define USMART_PRINTF           printf  /* 定义printf输出 */

/******************************************************************************************/
/* USMART定时器 定义 */

# if USMART_ENTIMX_SCAN == 1    /* 开启了使能定时器扫描,则需要如下定义 */

/* TIMX 中断定义 
 * 用于定时调用usmart.scan函数扫描串口数据,并执行相关操作
 * 注意: 通过修改这4个宏定义,可以支持TIM1~TIM8任意一个定时器.
 */
#define USMART_TIMX                     TIM4
#define USMART_TIMX_IRQn                TIM4_IRQn
#define USMART_TIMX_IRQHandler          TIM4_IRQHandler
#define USMART_TIMX_CLK_ENABLE()        do{ RCC->APB1ENR |= 1 << 2; }while(0)   /* TIMX 时钟使能 */

#endif

/******************************************************************************************/


/* 如果没有定义uint32_t,则定义 */
#ifndef uint32_t
typedef unsigned           char uint8_t;
typedef unsigned short     int  uint16_t;
typedef unsigned           int  uint32_t;
#endif



char * usmart_get_input_string(void);	/* 获取输入数据流 */
void usmart_timx_reset_time(void);      /* 复位运行时间 */
uint32_t usmart_timx_get_time(void);    /* 获取运行时间 */
void usmart_timx_init(uint16_t tclk);   /* 初始化定时器 */

#endif



























                }
                else
                {
                    g_usart_rx_buf[g_usart_rx_sta & 0X3FFF] = rxdata;   /* 存储数据到 g_usart_rx_buf */
                    g_usart_rx_sta++;

                    if (g_usart_rx_sta > (USART_REC_LEN - 1))g_usart_rx_sta = 0;/* 接收数据溢出, 重新开始接收 */
                }
            }
        }
    }

#if SYS_SUPPORT_OS  /* 如果SYS_SUPPORT_OS为真，则需要支持OS. */
    OSIntExit();
#endif
}
#endif

/**
 * @brief       串口X初始化函数
 * @param       sclk: 串口X的时钟源频率(单位: MHz)
 *              串口1 的时钟源来自: PCLK2 = 72Mhz
 *              串口2 - 5 的时钟源来自: PCLK1 = 36Mhz
 * @note        注意: 必须设置正确的sclk, 否则串口波特率就会设置异常.
 * @param       baudrate: 波特率, 根据自己需要设置波特率值
 * @retval      无
 */
void usart_init(uint32_t sclk, uint32_t baudrate)
{
    uint32_t temp;
    /* IO 及 时钟配置 */
    USART_TX_GPIO_CLK_ENABLE(); /* 使能串口TX脚时钟 */
    USART_RX_GPIO_CLK_ENABLE(); /* 使能串口RX脚时钟 */
    USART_UX_CLK_ENABLE();      /* 使能串口时钟 */

    sys_gpio_set(USART_TX_GPIO_PORT, USART_TX_GPIO_PIN,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_PU);   /* 串口TX脚 模式设置 */

    sys_gpio_set(USART_RX_GPIO_PORT, USART_RX_GPIO_PIN,
                 SYS_GPIO_MODE_IN, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_PU);   /* 串口RX脚 必须设置成输入模式 */

    temp = (sclk * 1000000 + baudrate / 2) / baudrate;  /* 得到BRR, 采用四舍五入计算 */
    /* 波特率设置 */
    USART_UX->BRR = temp;       /* 波特率设置 */
    USART_UX->CR1 = 0;          /* 清零CR1寄存器 */
    USART_UX->CR1 |= 0 << 12;   /* M = 0, 1个起始位, 8个数据位, n个停止位(由USART_CR2 STOP[1:0]指定, 默认是0, 表示1个停止位) */
    USART_UX->CR1 |= 1 << 3;    /* TE = 1, 串口发送使能 */
#if USART_EN_RX  /* 如果使能了接收 */
    /* 使能接收中断 */
    USART_UX->CR1 |= 1 << 2;    /* RE = 1, 串口接收使能 */
    USART_UX->CR1 |= 1 << 5;    /* RXNEIE = 1, 接收缓冲区非空中断使能 */
    sys_nvic_init(3, 3, USART_UX_IRQn, 2); /* 组2，最低优先级 */
#endif
    USART_UX->CR1 |= 1 << 13;   /* UE = 1, 串口使能 */
}

/**
 * @brief   USART2 初始化（Serial 用）
 * @param   baud: 波特率
 */
void Serial_Init(uint32_t baud)
{
    GPIO_InitTypeDef  gpio_init;
    USART_InitTypeDef usart_init_struct;
    NVIC_InitTypeDef  nvic_init;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    /* PA2 -> TX , PA3 -> RX */
    gpio_init.GPIO_Pin   = GPIO_Pin_2;
    gpio_init.GPIO_Mode  = GPIO_Mode_AF_PP;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio_init);

    gpio_init.GPIO_Pin  = GPIO_Pin_3;
    gpio_init.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &gpio_init);

    usart_init_struct.USART_BaudRate            = baud;
    usart_init_struct.USART_WordLength          = USART_WordLength_8b;
    usart_init_struct.USART_StopBits            = USART_StopBits_1;
    usart_init_struct.USART_Parity              = USART_Parity_No;
    usart_init_struct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    usart_init_struct.USART_Mode                = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(USART2, &usart_init_struct);

    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    nvic_init.NVIC_IRQChannel = USART2_IRQn;
    nvic_init.NVIC_IRQChannelPreemptionPriority = 0;
    nvic_init.NVIC_IRQChannelSubPriority        = 0;
    nvic_init.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&nvic_init);

    USART_Cmd(USART2, ENABLE);
}

/**
 * @brief   USART2 发送 1 字节
 */
void Serial_SendByte(uint8_t byte)
{
    USART_SendData(USART2, byte);
    while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
}

/**
 * @brief   USART2 发送字符串
 */
void Serial_SendString(char *str)
{
    while (*str != '\0')
    {
        Serial_SendByte(*str++);
    }
}

/**
 * @brief   USART3 初始化（示例中的 uart2 改为使用 USART3）
 * @param   baud: 波特率
 */
void uart3_init(uint32_t baud)
{
    GPIO_InitTypeDef  gpio_init;
    USART_InitTypeDef usart_init_struct;
    NVIC_InitTypeDef  nvic_init;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    /* PB10 -> TX , PB11 -> RX */
    gpio_init.GPIO_Pin   = GPIO_Pin_10;
    gpio_init.GPIO_Mode  = GPIO_Mode_AF_PP;
    gpio_init.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &gpio_init);

    gpio_init.GPIO_Pin  = GPIO_Pin_11;
    gpio_init.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &gpio_init);

    usart_init_struct.USART_BaudRate            = baud;
    usart_init_struct.USART_WordLength          = USART_WordLength_8b;
    usart_init_struct.USART_StopBits            = USART_StopBits_1;
    usart_init_struct.USART_Parity              = USART_Parity_No;
    usart_init_struct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    usart_init_struct.USART_Mode                = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(USART3, &usart_init_struct);

    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    nvic_init.NVIC_IRQChannel = USART3_IRQn;
    nvic_init.NVIC_IRQChannelPreemptionPriority = 0;
    nvic_init.NVIC_IRQChannelSubPriority        = 0;
    nvic_init.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&nvic_init);

    USART_Cmd(USART3, ENABLE);
}

/**
 * @brief   USART3 发送 1 字节
 */
void uart3_send_byte(uint8_t byte)
{
    USART_SendData(USART3, byte);
    while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
}

/**
 * @brief   USART3 发送字符串
 */
void uart3_send_string(char *str)
{
    while (*str != '\0')
    {
        uart3_send_byte(*str++);
    }
}













