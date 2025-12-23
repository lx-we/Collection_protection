/*
 ****************************************************************************************************
 * 实现原理:
 * 1. 配置TIM3_CH1工作在PWM模式,频率800kHz (72MHz/90)
 * 2. 配置DMA1_Channel6用于TIM3_CH1的CCR1寄存器传输
 * 3. 将24位GRB888颜色值编码为PWM比较值(0码=30, 1码=60)
 * 4. 通过DMA自动传输数据到TIM3->CCR1,实现WS2812B通信
 *
 ****************************************************************************************************
 */

#include "./BSP/LED-WS2812/ws.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/TIMER/gtim.h"

/* PWM发送缓冲区 (每个LED需要24个16位数据,总共支持WS2812_MAX_LEDS个LED) */
static uint16_t ws2812_pwm_buffer[WS2812_MAX_LEDS * 24];
uint8_t RBGLED_status = 0;

/**
 * @brief       将24位GRB888颜色值编码为PWM格式
 * @param       color: 24位GRB888颜色值 (格式: G[23:16] R[15:8] B[7:0])
 * @param       buffer: 输出缓冲区 (需要24个16位空间)
 * @retval      无
 */
static void ws2812_encode_color(uint32_t color, uint16_t *buffer)
{
    uint8_t i;
    /* 从最高位(bit23)开始,逐位检查并编码 */
    for (i = 0; i < 24; i++)
    {
        if (color & (0x800000 >> i))  /* 检查第(23-i)位 */
        {
            buffer[i] = HIGH_LEVEL;  /* 1码 */
        }
        else
        {
            buffer[i] = LOW_LEVEL;   /* 0码 */
        }
    }
}

/**
 * @brief       将RGB888转换为GRB888格式
 * @param       r: 红色分量 (0-255)
 * @param       g: 绿色分量 (0-255)
 * @param       b: 蓝色分量 (0-255)
 * @retval      24位GRB888颜色值
 */
static uint32_t ws2812_rgb_to_grb888(uint8_t r, uint8_t g, uint8_t b)
{
    /* GRB888格式: G[23:16] R[15:8] B[7:0] */
    return ((uint32_t)g << 16) | ((uint32_t)r << 8) | b;
}

/**
 * @brief       初始化WS2812B (PWM+DMA)
 * @param       无
 * @retval      无
 */
void ws2812_init(void)
{
    /* 使能DMA时钟 */
    WS2812_DMA_CLK_ENABLE();         /* DMA1时钟使能 */

    /* 使用gtim.c中的函数初始化TIM3_CH1 PWM模式 */
    /* PWM频率: 72MHz / 90 = 800kHz, 每个周期1.25us */
    gtim_ws2812_pwm_dma_init(WS2812_PWM_PERIOD - 1, 0);  /* ARR = 89, PSC = 0 */

    /* 配置DMA1_Channel6 (TIM3_CH1) */
    WS2812_DMA_CHANNEL->CCR = 0;     /* 清零CCR寄存器 */
    WS2812_DMA_CHANNEL->CCR |= 0 << 14;  /* MEM2MEM = 0, 内存到外设模式 */
    WS2812_DMA_CHANNEL->CCR |= 2 << 12;  /* PL[1:0] = 10, 高优先级 */
    WS2812_DMA_CHANNEL->CCR |= 0 << 13;  /* PL[1:0] = 10 */
    WS2812_DMA_CHANNEL->CCR |= 1 << 10;  /* MSIZE[1:0] = 01, 16位内存数据宽度 */
    WS2812_DMA_CHANNEL->CCR |= 0 << 11;  /* MSIZE[1:0] = 01 */
    WS2812_DMA_CHANNEL->CCR |= 1 << 8;   /* PSIZE[1:0] = 01, 16位外设数据宽度 */
    WS2812_DMA_CHANNEL->CCR |= 0 << 9;   /* PSIZE[1:0] = 01 */
    WS2812_DMA_CHANNEL->CCR |= 1 << 7;   /* MINC = 1, 内存地址递增 */
    WS2812_DMA_CHANNEL->CCR |= 0 << 6;   /* PINC = 0, 外设地址不递增 */
    WS2812_DMA_CHANNEL->CCR |= 0 << 5;   /* CIRC = 0, 非循环模式 */
    WS2812_DMA_CHANNEL->CCR |= 1 << 4;   /* DIR = 1, 从内存到外设 */
    WS2812_DMA_CHANNEL->CCR |= 0 << 3;   /* TEIE = 0, 传输错误中断禁用 */
    WS2812_DMA_CHANNEL->CCR |= 0 << 1;   /* TCIE = 0, 传输完成中断禁用 (使用轮询方式) */

    /* 设置DMA外设地址 (TIM3_CH1比较值寄存器) */
    WS2812_DMA_CHANNEL->CPAR = (uint32_t)&(GTIM_WS2812_TIM_CCRX);

    /* 先禁用DMA通道 */
    WS2812_DMA_CHANNEL->CCR &= ~(1 << 0);  /* EN = 0, 禁用DMA通道 */
    
    /* 初始化时清除所有LED */
    ws2812_show_black();
}

/**
 * @brief       发送数据给WS2812B
 * @param       color_buf: 24位GRB888颜色值数组指针
 * @param       len: LED数量
 * @retval      无
 */
void ws2812_display(uint32_t *color_buf, uint16_t len)
{
    uint16_t i, j;
    uint16_t pwm_index = 0;
    uint16_t buffersize = len * 24;
    
    if (len > WS2812_MAX_LEDS)
    {
        len = WS2812_MAX_LEDS;
    }
    
    /* 清零PWM缓冲区（确保未使用的LED数据为0） */
    for (i = 0; i < buffersize; i++)
    {
        ws2812_pwm_buffer[i] = LOW_LEVEL;  /* 先全部设置为低电平 */
    }
    
    /* 将24位GRB888颜色值编码为PWM格式 */
    /* WS2812B数据格式：G[23:16] R[15:8] B[7:0] */
    /* 参考代码逻辑：每个字节从最高位(bit7)开始发送到最低位(bit0) */
    /* 对于24位数据：从bit23开始发送到bit0 */
    pwm_index = 0;
    for (i = 0; i < len; i++)
    {
        uint32_t color = color_buf[i];
        /* 对每个LED的24位颜色值进行编码，从最高位(bit23)开始发送 */
        /* 按照参考代码逻辑：将第(23-j)位左移到最高位，然后检查最高位 */
        for (j = 0; j < 24; j++)
        {
            /* j=0时：检查bit23，将其左移0位到bit23位置，然后检查bit23 */
            /* j=1时：检查bit22，将其左移1位到bit23位置，然后检查bit23 */
            /* ... */
            /* j=23时：检查bit0，将其左移23位到bit23位置，然后检查bit23 */
            ws2812_pwm_buffer[pwm_index] = 
                ((color << j) & 0x800000) ? HIGH_LEVEL : LOW_LEVEL;
            pwm_index++;
        }
    }
    
    /* 等待上一次DMA传输完成 */
    while (ws2812_is_busy());
    
    /* 确保DMA通道已禁用 */
    WS2812_DMA_CHANNEL->CCR &= ~(1 << 0);       /* EN = 0, 禁用DMA通道 */
    
    /* 等待DMA通道完全停止 */
    while (WS2812_DMA_CHANNEL->CCR & (1 << 0)); /* 等待EN位清零 */
    
    /* 清除所有DMA标志位 (Channel6的标志位) */
    /* Channel6: GIF6=bit20, TCIF6=bit21, HTIF6=bit22, TEIF6=bit23 */
    WS2812_DMA->IFCR = (1 << 20) | (1 << 21) | (1 << 22) | (1 << 23);
    
    /* 设置DMA传输参数（必须在使能DMA之前） */
    WS2812_DMA_CHANNEL->CNDTR = buffersize;        /* 传输数据数量 */
    WS2812_DMA_CHANNEL->CMAR = (uint32_t)ws2812_pwm_buffer;  /* 内存地址 */
    
    /* 先使能TIM3_CH1输出（必须在使能定时器之前） */
    GTIM_WS2812_TIM->CCER |= 1 << 0;                 /* CC1E = 1, 使能通道1输出 */
    
    /* 使能DMA通道（必须在使能定时器之前） */
    WS2812_DMA_CHANNEL->CCR |= 1 << 0;          /* EN = 1, 使能DMA通道 */
    
    /* 最后使能TIM3定时器（开始PWM输出和DMA传输） */
    GTIM_WS2812_TIM->CR1 |= 1 << 0;                  /* CEN = 1, 使能定时器 */
    
    /* 等待传输完成（添加超时保护） */
    uint32_t timeout = 0;
    while (ws2812_is_busy() && (timeout++ < 10000000));  /* 超时保护，防止死循环 */
    
    /* 传输完成后，先停止定时器 */
    GTIM_WS2812_TIM->CR1 &= ~(1 << 0);           /* CEN = 0, 禁用定时器 */
    
    /* 等待定时器完全停止 */
    while (GTIM_WS2812_TIM->CR1 & (1 << 0));     /* 等待CEN位清零 */
    
    /* 禁用DMA通道 */
    WS2812_DMA_CHANNEL->CCR &= ~(1 << 0);        /* EN = 0, 禁用DMA通道 */
    
    /* 等待DMA通道完全停止 */
    while (WS2812_DMA_CHANNEL->CCR & (1 << 0));  /* 等待EN位清零 */
    
    /* 清除所有DMA标志位 (Channel6的标志位) */
    WS2812_DMA->IFCR = (1 << 20) | (1 << 21) | (1 << 22) | (1 << 23);
    
    /* 传输完成后复位WS2812B */
    ws2812_reset();
}

/**
 * @brief       复位WS2812B (保持低电平280us以上)
 * @param       无
 * @retval      无
 */
void ws2812_reset(void)
{
    /* 禁用TIM3_CH1输出并设置为低电平 */
    GTIM_WS2812_TIM->CCER &= ~(1 << 0);              /* CC1E = 0, 禁用通道1输出 */
    GTIM_WS2812_TIM->CCR1 = 0;                       /* CCR1设置为0 */
    
    /* 直接控制GPIO输出低电平（确保复位时序） */
    GTIM_WS2812_TIM_GPIO_PORT->BRR = GTIM_WS2812_TIM_GPIO_PIN;  /* 输出低电平 */
    
    /* 保持低电平至少280us（WS2812B要求至少50us，使用280us更安全） */
    delay_us(280);
    
    /* 恢复GPIO为复用功能 */
    sys_gpio_set(GTIM_WS2812_TIM_GPIO_PORT, GTIM_WS2812_TIM_GPIO_PIN,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_NONE);
}

/**
 * @brief       发送缓冲区数据 (兼容旧接口)
 * @param       buffer: 数据缓冲区指针 (格式: GRB, GRB, ...)
 * @param       len: LED数量
 * @retval      无
 */
void ws2812_send_buffer(uint8_t *buffer, uint16_t len)
{
    uint16_t i;
    uint32_t color_buf[WS2812_MAX_LEDS];
    
    if (len > WS2812_MAX_LEDS)
    {
        len = WS2812_MAX_LEDS;
    }
    
    /* 将3字节GRB格式转换为24位GRB888格式 */
    for (i = 0; i < len; i++)
    {
        color_buf[i] = ws2812_rgb_to_grb888(buffer[i * 3 + 1],  /* 红色 */
                                             buffer[i * 3 + 0],  /* 绿色 */
                                             buffer[i * 3 + 2]); /* 蓝色 */
    }
    
    /* 发送数据 */
    ws2812_display(color_buf, len);
}

/**
 * @brief       清除所有LED (设置为黑色)
 * @param       num_leds: LED数量
 * @retval      无
 */
void ws2812_clear(uint16_t num_leds)
{
    uint16_t i;
    uint32_t color_buf[WS2812_MAX_LEDS];
    
    if (num_leds > WS2812_MAX_LEDS)
    {
        num_leds = WS2812_MAX_LEDS;
    }
    
    /* 填充黑色 (GRB888格式: 0x000000) */
    for (i = 0; i < num_leds; i++)
    {
        color_buf[i] = 0x000000;  /* 黑色 */
    }
    
    ws2812_display(color_buf, num_leds);
}

/**
 * @brief       设置所有LED为相同颜色
 * @param       r: 红色分量 (0-255)
 * @param       g: 绿色分量 (0-255)
 * @param       b: 蓝色分量 (0-255)
 * @param       num_leds: LED数量
 * @retval      无
 */
void ws2812_set_all(uint8_t r, uint8_t g, uint8_t b, uint16_t num_leds)
{
    uint16_t i;
    uint32_t color_buf[WS2812_MAX_LEDS];
    uint32_t color;
    
    if (num_leds > WS2812_MAX_LEDS)
    {
        num_leds = WS2812_MAX_LEDS;
    }
    
    /* 转换为GRB888格式 */
    color = ws2812_rgb_to_grb888(r, g, b);
    
    /* 填充缓冲区 */
    for (i = 0; i < num_leds; i++)
    {
        color_buf[i] = color;
    }
    
    /* 发送数据 */
    ws2812_display(color_buf, num_leds);
}

/**
 * @brief       检查DMA传输是否完成
 * @param       无
 * @retval      0-传输完成, 1-正在传输
 */
uint8_t ws2812_is_busy(void)
{
    /* 检查DMA通道是否使能 */
    if ((WS2812_DMA_CHANNEL->CCR & (1 << 0)) == 0)
    {
        return 0;  /* DMA未使能,传输完成 */
    }
    
    /* 检查传输完成标志 (Channel6的TCIF6标志位，位21) */
    /* DMA1_Channel6的TCIF6标志位在ISR寄存器的位21 */
    if (WS2812_DMA->ISR & (1 << 21))  /* TCIF6 = bit 21 */
    {
        return 0;  /* 传输完成 */
    }
    
    /* 检查传输错误标志 (Channel6的TEIF6标志位，位23) */
    if (WS2812_DMA->ISR & (1 << 23))  /* TEIF6 = bit 23 */
    {
        /* 传输错误，清除标志并返回完成（避免死循环） */
        WS2812_DMA->IFCR = (1 << 23);  /* 清除TEIF6标志 */
        WS2812_DMA_CHANNEL->CCR &= ~(1 << 0);  /* 禁用DMA通道 */
        return 0;  /* 返回完成，避免死循环 */
    }
    
    return 1;  /* 正在传输 */
}

/**
 * @brief       改变颜色的亮度值
 * @param       rgb: 24位GRB888颜色值
 * @param       k: 亮度系数 (0.0-1.0, 0.0为最暗, 1.0为最亮)
 * @retval      调整后的24位GRB888颜色值
 */
uint32_t ws2812_color_change_bright(uint32_t rgb, float k)
{
    uint8_t r, g, b;
    uint32_t result;
    
    /* 限制亮度系数范围 */
    if (k < 0.0f) k = 0.0f;
    if (k > 1.0f) k = 1.0f;
    
    /* 提取GRB分量 */
    g = (rgb >> 16) & 0xFF;  /* 绿色 */
    r = (rgb >> 8) & 0xFF;   /* 红色 */
    b = rgb & 0xFF;          /* 蓝色 */
    
    /* 调整亮度 */
    r = (uint8_t)(r * k);
    g = (uint8_t)(g * k);
    b = (uint8_t)(b * k);
    
    /* 重新组合为GRB888格式 */
    result = ((uint32_t)g << 16) | ((uint32_t)r << 8) | b;
    
    return result;
}

/**
 * @brief       设置所有LED的亮度并显示
 * @param       color_buf: 颜色缓冲区指针
 * @param       len: LED数量
 * @param       brightness: 亮度系数 (0.0-1.0)
 * @retval      无
 */
void ws2812_set_brightness(uint32_t *color_buf, uint16_t len, float brightness)
{
    uint16_t i;
    uint32_t adjusted_buf[WS2812_MAX_LEDS];
    
    /* 限制亮度范围 */
    if (brightness < 0.0f) brightness = 0.0f;
    if (brightness > 1.0f) brightness = 1.0f;
    
    /* 对每个LED应用亮度调整 */
    for (i = 0; i < len && i < WS2812_MAX_LEDS; i++)
    {
        adjusted_buf[i] = ws2812_color_change_bright(color_buf[i], brightness);
    }
    
    /* 显示调整后的颜色 */
    ws2812_display(adjusted_buf, len);
}

/**
 * @brief       WS2812B不显示颜色，复位状态（关灯）
 * @param       无
 * @retval      无
 */
void ws2812_show_black(void)
{
    ws2812_clear(LED_NUM);
}

/**
 * @brief       设置所有LED为暖黄色，亮度可调
 * @param       percent: 亮度百分比 (0-100, 0为关闭, 100为最亮)
 * @retval      无
 * @note        颜色固定为暖黄色，只需输入亮度百分比即可
 */
void ws2812_set_warm_yellow(uint8_t percent)
{
    uint16_t i;
    uint32_t color_buf[WS2812_MAX_LEDS];
    float brightness;
    
    /* 限制百分比范围 */
    if (percent > 100)
    {
        percent = 100;
    }
    
    /* 将百分比转换为亮度系数 (0.0-1.0) */
    brightness = (float)percent / 100.0f;
    
    /* 对所有LED填充暖黄色 */
    for (i = 0; i < LED_NUM; i++)
    {
        color_buf[i] = GRB888_WARM_YELLOW;
    }
    RBGLED_status = percent;
    
    /* 应用亮度调整并显示 */
    ws2812_set_brightness(color_buf, LED_NUM, brightness);
}
