/**
 ****************************************************************************************************
 * @file        usart.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-17
 * @brief       串口初始化代码(一般是串口1)，支持printf
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
 * V1.0 20200417
 * 第一次发布
 *
 ****************************************************************************************************
 */

#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"

/* 如果使用os,则包括下面的头文件即可. */
#if SYS_SUPPORT_OS
#include "os.h"   /* os 使用 */
#endif

/******************************************************************************************/
/* 支持printf函数，无需选择 MicroLIB */
#if 1

#if (__ARMCC_VERSION >= 6010050)            /* 使用AC6编译器时 */
__asm(".global __use_no_semihosting\n\t");
__asm(".global __ARM_use_no_argv \n\t");
#else
#pragma import(__use_no_semihosting)
struct __FILE { int handle; };
#endif

int _ttywrch(int ch) { return ch; }
void _sys_exit(int x) { (void)x; }
char *_sys_command_string(char *cmd, int len) { (void)cmd; (void)len; return NULL; }

FILE __stdout;

int fputc(int ch, FILE *f)
{
    (void)f;
    while ((USART_UX->SR & 0X40) == 0);
    USART_UX->DR = (uint8_t)ch;
    return ch;
}
#endif
/******************************************************************************************/

#if USART_EN_RX
uint8_t  g_usart_rx_buf[USART_REC_LEN];
uint16_t g_usart_rx_sta = 0;

void USART_UX_IRQHandler(void)
{
    uint8_t rxdata;
#if SYS_SUPPORT_OS
    OSIntEnter();
#endif

    if (USART_UX->SR & (1 << 5))
    {
        rxdata = USART_UX->DR;
        if ((g_usart_rx_sta & 0x8000) == 0)
        {
            if (g_usart_rx_sta & 0x4000)
            {
                if (rxdata != 0x0a)
                {
                    g_usart_rx_sta = 0;
                }
                else
                {
                    g_usart_rx_sta |= 0x8000;
                }
            }
            else
            {
                if (rxdata == 0x0d)
                {
                    g_usart_rx_sta |= 0x4000;
                }
                else
                {
                    g_usart_rx_buf[g_usart_rx_sta & 0X3FFF] = rxdata;
                    g_usart_rx_sta++;
                    if (g_usart_rx_sta > (USART_REC_LEN - 1)) g_usart_rx_sta = 0;
                }
            }
        }
    }

#if SYS_SUPPORT_OS
    OSIntExit();
#endif
}
#endif /* USART_EN_RX */

/**
 * @brief       串口X初始化函数
 * @param       sclk: 串口X的时钟源频率(单位: MHz)
 *              串口1 的时钟源来自: PCLK2 = 72Mhz
 *              串口2 - 5 的时钟源来自: PCLK1 = 36Mhz
 * @param       baudrate: 波特率
 */
void usart_init(uint32_t sclk, uint32_t baudrate)
{
    uint32_t temp = (sclk * 1000000U + baudrate / 2) / baudrate;  /* BRR */

    USART_TX_GPIO_CLK_ENABLE();
    USART_RX_GPIO_CLK_ENABLE();
    USART_UX_CLK_ENABLE();

    sys_gpio_set(USART_TX_GPIO_PORT, USART_TX_GPIO_PIN,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_PU);
    sys_gpio_set(USART_RX_GPIO_PORT, USART_RX_GPIO_PIN,
                 SYS_GPIO_MODE_IN, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_PU);

    USART_UX->BRR = temp;
    USART_UX->CR1 = 0;
    USART_UX->CR1 |= 1 << 3;    /* TE */
#if USART_EN_RX
    USART_UX->CR1 |= 1 << 2;    /* RE */
    USART_UX->CR1 |= 1 << 5;    /* RXNEIE */
    sys_nvic_init(3, 3, USART_UX_IRQn, 2);
#endif
    USART_UX->CR1 |= 1 << 13;   /* UE */
}

/**
 * @brief   USART2 初始化（Serial 用）
 */
void Serial_Init(uint32_t baud)
{
    RCC->APB2ENR |= 1 << 2;   /* GPIOA */
    RCC->APB1ENR |= 1 << 17;  /* USART2 */

    sys_gpio_set(GPIOA, SYS_GPIO_PIN2, SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_PU);
    sys_gpio_set(GPIOA, SYS_GPIO_PIN3, SYS_GPIO_MODE_IN, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_PU);

    uint32_t brr = (72000000U + baud / 2) / baud; /* 假设APB1定时器时钟72MHz */
    USART2->BRR = brr;

    USART2->CR1 = 0;
    USART2->CR1 |= 1 << 3;    /* TE */
    USART2->CR1 |= 1 << 2;    /* RE */
    USART2->CR1 |= 1 << 5;    /* RXNEIE */
    USART2->CR1 |= 1 << 13;   /* UE */

    sys_nvic_init(0, 0, USART2_IRQn, 2);
}

void Serial_SendByte(uint8_t byte)
{
    USART_SendData(USART2, byte);
    while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
}

void Serial_SendString(char *str)
{
    while (*str != '\0') { Serial_SendByte(*str++); }
}

/**
 * @brief   USART3 初始化
 */
void uart3_init(uint32_t baud)
{
    RCC->APB2ENR |= 1 << 3;   /* GPIOB */
    RCC->APB1ENR |= 1 << 18;  /* USART3 */

    sys_gpio_set(GPIOB, SYS_GPIO_PIN10, SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_PU);
    sys_gpio_set(GPIOB, SYS_GPIO_PIN11, SYS_GPIO_MODE_IN, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_PU);

    uint32_t brr = (72000000U + baud / 2) / baud; /* 假设APB1定时器时钟72MHz */
    USART3->BRR = brr;

    USART3->CR1 = 0;
    USART3->CR1 |= 1 << 3;    /* TE */
    USART3->CR1 |= 1 << 2;    /* RE */
    USART3->CR1 |= 1 << 5;    /* RXNEIE */
    USART3->CR1 |= 1 << 13;   /* UE */

    sys_nvic_init(0, 0, USART3_IRQn, 2);
}

void uart3_send_byte(uint8_t byte)
{
    USART_SendData(USART3, byte);
    while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
}

void uart3_send_string(char *str)
{
    while (*str != '\0') { uart3_send_byte(*str++); }
}

