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
#include "./BSP/LED/led.h"

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

    /* APB1 = 36MHz → USART2 时钟 = 36MHz */
    uint32_t brr = (36000000U + baud / 2) / baud;
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
    while ((USART2->SR & (1 << 7)) == 0);  /* 等待发送数据寄存器空 (TXE) */
    USART2->DR = byte;  /* 写入数据到数据寄存器 */
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

    /* APB1 = 36MHz → USART3 时钟 = 36MHz */
    uint32_t brr = (36000000U + baud / 2) / baud;
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
    while ((USART3->SR & (1 << 7)) == 0);  /* 等待发送数据寄存器空 (TXE) */
    USART3->DR = byte;  /* 写入数据到数据寄存器 */
}

void uart3_send_string(char *str)
{
    while (*str != '\0') { uart3_send_byte(*str++); }
}

/**
 * @brief   USART3 中断服务函数
 * @param   无
 * @retval  无
 */
void USART3_IRQHandler(void)
{
    uint8_t rxdata;
    
    if (USART3->SR & (1 << 5))  /* 接收到数据 (RXNE) */
    {
        rxdata = USART3->DR;  /* 读取接收到的数据 */
        
        /* 调用无线模块接收回调函数 */
        extern void wireless_receive_callback(uint8_t byte);
        wireless_receive_callback(rxdata);
    }
}

/* UART4 接收相关变量 */
#define UART4_RX_BUF_MAX    64      /* 接收缓冲区大小 */
static uint8_t uart4_rx_flag = 0;   /* 接收标志位 */
static uint8_t uart4_rx_data = 0;   /* 接收数据缓存 */

/**
 * @brief   UART4 初始化（仅接收功能）
 * @param   baud: 波特率
 * @retval  无
 */
void uart4_init(uint32_t baud)
{
    RCC->APB2ENR |= 1 << 4;   /* GPIOC 时钟使能 */
    RCC->APB1ENR |= 1 << 19;  /* UART4 时钟使能 */

    /* 配置 RX 引脚 PC11 为复用功能输入模式（AF），根据原理图PC11是U4_RX */
    /* 注意：UART的RX引脚必须配置为复用功能模式，才能将UART外设连接到GPIO引脚 */
    /* 对于STM32F103，复用功能输入：CNF[1:0]=10(复用推挽), MODE[1:0]=00(输入模式) = 0x08 */
    /* 直接配置GPIO寄存器，确保RX引脚正确配置为复用功能输入模式 */
    GPIOC->CRH &= ~(0x0F << ((11 - 8) * 4));  /* 清除PC11的配置位（PC11在CRH寄存器，位[15:12]） */
    GPIOC->CRH |= (0x08 << ((11 - 8) * 4));   /* CNF[1:0]=10(复用推挽), MODE[1:0]=00(输入) = 0x08 */
    GPIOC->ODR |= (1 << 11);                  /* 设置ODR位使能上拉（配合CNF=10实现上拉输入） */
    
    /* 验证配置 */
    uint32_t pcr11 = (GPIOC->CRH >> ((11 - 8) * 4)) & 0x0F;
    printf("UART4 PC11 GPIO Config: CRH[15:12]=0x%02lX (should be 0x08 for AF input)\r\n", pcr11);

    /* APB1 = 36MHz → UART4 时钟 = 36MHz */
    uint32_t brr = (36000000U + baud / 2) / baud;
    UART4->BRR = brr;
    
    printf("UART4 BRR: %lu (calculated for %lu baud, APB1=36MHz)\r\n", brr, baud);  /* 调试：打印BRR值 */

    UART4->CR1 = 0;
    /* 不设置 TE (发送使能)，只开启接收 */
    /* 默认配置：8数据位，无校验，1停止位（CR1默认值已满足） */
    /* CR1[12] = 0: 8数据位, CR1[10] = 0: 无校验, CR2[13:12] = 00: 1停止位 */
    UART4->CR1 |= 1 << 2;    /* RE (接收使能) */
    UART4->CR1 |= 1 << 5;    /* RXNEIE (接收中断使能) */
    UART4->CR1 |= 1 << 13;   /* UE (USART 使能) */
    
    printf("UART4 CR1: 0x%04X (RE=1, RXNEIE=1, UE=1)\r\n", (uint16_t)UART4->CR1);  /* 调试：打印CR1值 */

    /* 清除所有可能的错误标志（读取DR寄存器可以清除错误标志） */
    uint32_t temp_sr = UART4->SR;
    if (temp_sr & (1 << 1)) { uint8_t dummy = UART4->DR; (void)dummy; }  /* 清除FE */
    if (temp_sr & (1 << 2)) { uint8_t dummy = UART4->DR; (void)dummy; }  /* 清除NE */
    if (temp_sr & (1 << 3)) { uint8_t dummy = UART4->DR; (void)dummy; }  /* 清除ORE */

    sys_nvic_init(0, 0, UART4_IRQn, 2);
    
    /* 初始化接收标志位 */
    uart4_rx_flag = 0;
    uart4_rx_data = 0;
    
    printf("UART4 Init: PC11(RX), Baud=%lu, NVIC Priority=2\r\n", baud);  /* 调试信息 */
}

/**
 * @brief   获取 UART4 接收标志位
 * @param   无
 * @retval  1-接收到数据, 0-未接收到数据
 */
uint8_t uart4_get_rx_flag(void)
{
    return uart4_rx_flag;
}

/**
 * @brief   获取 UART4 接收数据
 * @param   无
 * @retval  接收到的数据
 */
uint8_t uart4_get_rx_data(void)
{
    return uart4_rx_data;
}

/**
 * @brief   清除 UART4 接收标志位
 * @param   无
 * @retval  无
 */
void uart4_clear_rx_flag(void)
{
    uart4_rx_flag = 0;
}

/**
 * @brief   UART4 中断服务函数
 * @param   无
 * @retval  无
 */
void UART4_IRQHandler(void)
{
    uint32_t sr = UART4->SR;  /* 读取状态寄存器 */
    
    if (sr & (1 << 5))  /* 接收到数据 (RXNE) */
    {
        uart4_rx_data = UART4->DR;  /* 读取接收到的数据（读取 DR 会自动清除 RXNE 标志） */
        uart4_rx_flag = 1;           /* 设置接收标志位 */
        
        /* 检查是否有错误标志（在读取DR之前检查） */
        uint8_t has_error = 0;
        if (sr & (1 << 1)) { has_error = 1; printf("UART4: FE(帧错误) "); }  /* FE: 帧错误 */
        if (sr & (1 << 2)) { has_error = 1; printf("UART4: NE(噪声错误) "); }  /* NE: 噪声错误 */
        if (sr & (1 << 3)) { has_error = 1; printf("UART4: ORE(过载错误) "); }  /* ORE: 过载错误 */
        
        if (has_error == 0)
        {
            /* 正常接收，打印数据 */
            printf("UART4 ISR: RX=0x%02X", uart4_rx_data);
            if (uart4_rx_data >= 32 && uart4_rx_data < 127)
            {
                printf(" ('%c')\r\n", uart4_rx_data);
            }
            else
            {
                printf(" (非ASCII)\r\n");
            }
        }
        else
        {
            printf("UART4 ISR: RX=0x%02X (有错误)\r\n", uart4_rx_data);
        }
    }
    
    /* 清除错误标志（需要读取DR才能清除） */
    if (sr & (1 << 3))  /* ORE: 过载错误 */
    {
        uint8_t temp = UART4->DR;  /* 读取 DR 清除 ORE */
        (void)temp;
    }
    if (sr & (1 << 1))  /* FE: 帧错误 */
    {
        uint8_t temp = UART4->DR;  /* 读取 DR 清除 FE */
        (void)temp;
    }
    if (sr & (1 << 2))  /* NE: 噪声错误 */
    {
        uint8_t temp = UART4->DR;  /* 读取 DR 清除 NE */
        (void)temp;
    }
}

