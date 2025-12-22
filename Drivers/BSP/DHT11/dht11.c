/**
 ****************************************************************************************************
 * @file        dht11.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2020-06-01
 * @brief       DHT11数字温湿度传感器 驱动代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子�?技有限�?��
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 STM32F103开发板
 * 在线视�?:www.yuanzige.com
 * 技�??�?:www.openedv.com
 * �?��网址:www.alientek.com
 * �?��地址:openedv.taobao.com
 *
 * �?��说明
 * V1.0 20200601
 * �?��次发�?
 *
 ****************************************************************************************************
 */

#include "./BSP/DHT11/dht11.h"
#include "./SYSTEM/delay/delay.h"
#include "./SYSTEM/usart/usart.h"


/**
 * @brief       复位DHT11
 * @param       data: 要写入的数据
 * @retval      �?
 */
static void dht11_reset(void)
{
    DHT11_DQ_OUT(0);    /* 拉低DQ */
    delay_ms(20);       /* 拉低至少18ms */
    DHT11_DQ_OUT(1);    /* DQ=1 */
    delay_us(30);       /* 主机拉高10~35us */
}

/**
 * @brief       等待DHT11的回�?
 * @param       �?
 * @retval      0, DHT11正常
 *              1, DHT11异常/不存�?
 */
uint8_t dht11_check(void)
{
    uint8_t retry = 0;
    uint8_t rval = 0;

    while (DHT11_DQ_IN && retry < 100)      /* DHT11会拉低约83us */
    {
        retry++;
        delay_us(1);
    }

    if (retry >= 100)
    {
        rval = 1;
    }
    else
    {
        retry = 0;

        while (!DHT11_DQ_IN && retry < 100) /* DHT11拉低后会再�?拉高�?87us */
        {
            retry++;
            delay_us(1);
        }
        if (retry >= 100) rval = 1;
    }
    
    return rval;
}

/**
 * @brief       从DHT11读取一�?��
 * @param       �?
 * @retval      读取到的位�?: 0 / 1
 */
uint8_t dht11_read_bit(void)
{
    uint8_t retry = 0;

    while (DHT11_DQ_IN && retry < 100)  /* 等待变为低电�? */
    {
        retry++;
        delay_us(1);
    }

    retry = 0;

    while (!DHT11_DQ_IN && retry < 100) /* 等待变高电平 */
    {
        retry++;
        delay_us(1);
    }

    delay_us(40);       /* 等待40us */

    if (DHT11_DQ_IN)    /* 根据引脚状态返�? bit */
    {
        return 1;
    }
    else 
    {
        return 0;
    }
}

/**
 * @brief       从DHT11读取一�?���?
 * @param       �?
 * @retval      读到的数�?
 */
static uint8_t dht11_read_byte(void)
{
    uint8_t i, data = 0;

    for (i = 0; i < 8; i++)         /* �?��读取8位数�? */
    {
        data <<= 1;                 /* 高位数据先输�?, 先左移一�? */
        data |= dht11_read_bit();   /* 读取1bit数据 */
    }

    return data;
}

/**
 * @brief       从DHT11读取一次数�?
 * @param       temp: 温度�?(范围:-20~60°)
 * @param       humi: 湿度�?(范围:5%~95%)
 * @retval      0, 正常.
 *              1, 失败
 */
uint8_t dht11_read_data(uint8_t *temp, uint8_t *humi)
{
    uint8_t buf[5];
    uint8_t i;
    dht11_reset();

    if (dht11_check() == 0)
    {
        for (i = 0; i < 5; i++)     /* 读取40位数�? */
        {
            buf[i] = dht11_read_byte();
        }

        if ((buf[0] + buf[1] + buf[2] + buf[3]) == buf[4])
        {
            *humi = buf[0];
            *temp = buf[2];
        }
    }
    else
    {
        return 1;
    }
    
    return 0;
}

/**
 * @brief       初�?化DHT11的IO�? DQ 同时检测DHT11的存�?
 * @param       �?
 * @retval      0, 正常
 *              1, 不存�?/不�?�?
 */
uint8_t dht11_init(void)
{
    DHT11_DQ_GPIO_CLK_ENABLE();     /* 开启DQ引脚时钟 */

    /* DHT11_DQ引脚模式设置,开漏输�?,上拉, 这样就不用再设置IO方向�?, 开漏输出的时�?(=1), 也可以�?取�?部信号的高低电平 */
    sys_gpio_set(DHT11_DQ_GPIO_PORT, DHT11_DQ_GPIO_PIN,
                 SYS_GPIO_MODE_OUT, SYS_GPIO_OTYPE_OD, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PU);

    dht11_reset();
    return dht11_check();
}

/**
 * @brief       初�?化TIM6定时器中�?,用于定时读取DHT11数据
 * @param       �?
 * @retval      �?
 */
void dht11_timer_init(void)
{
    /* 使能TIM6时钟 (TIM6在APB1总线�?) */
    RCC->APB1ENR |= 1 << 4;  /* TIM6时钟使能 */

    /* 配置TIM6定时�?: 2秒中�?���? */
    /* 系统时钟72MHz, APB1=36MHz, TIM6时钟=72MHz(倍�?) */
    /* 预分频器PSC = 7199, 定时器时�? = 72MHz / 7200 = 10kHz */
    /* �?��重�?载值ARR = 20000, �?��周期 = 20000 / 10kHz = 2�? */
    TIM6->PSC = 7199;          /* 预分频器, 7200分�? */
    TIM6->ARR = 20000;         /* �?��重�?载�?, 20000�??�? */
    TIM6->CNT = 0;             /* 计数器清�? */
    TIM6->DIER |= 1 << 0;      /* 使能更新�?�� */
    TIM6->CR1 |= 1 << 0;       /* 使能定时�? */

    /* 配置NVIC�?�� */
    sys_nvic_init(2, 2, TIM6_DAC_IRQn, 2);  /* 抢占优先�?2,子优先级2,�?��通道TIM6_DAC_IRQn,分组2 */
}

/**
 * @brief       TIM6更新�?��服务函数(定时读取DHT11数据)
 * @note        TIM6和DAC共享同一�?���?��量，�?��服务函数名为TIM6_DAC_IRQHandler
 *              注意：btim.c�?��BTIM_TIMX_INT_IRQHandler()已�?�?��弱�?�?(__weak)�?
 *              所以�?函数会�?盖btim.c�?��弱�?号函数，不会产生重�?定义错�?
 * @param       �?
 * @retval      �?
 */
void TIM6_DAC_IRQHandler(void)
{
    /* 检�?IM6更新�?��标志�? */
    if (TIM6->SR & 0x01)  /* 检查更新中�?��志位 */
    {
        TIM6->SR &= ~(1 << 0);  /* 清除更新�?��标志�? */
        
        /* 读取DHT11数据 */
        uint8_t temperature, humidity;
        if (dht11_read_data(&temperature, &humidity) == 0)
        {
            printf("Temperature: %d C, Humidity: %d %%\r\n", temperature, humidity);
        }
        else
        {
            printf("DHT11 Read Error\r\n");
        }
    }
}
























