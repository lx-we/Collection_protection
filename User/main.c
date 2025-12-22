#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./USMART/usmart.h"
#include "./BSP/LED/led.h"
#include "./BSP/DHT11/dht11.h"
#include "./BSP/KEY/key.h"
#include "./BSP/LOCK/lock.h"
#include "./BSP/SR602/sr602.h"
#include "./BSP/SW-18010P/sw.h"
#include "./BSP/LED-WS2812/ws.h"
#include "./BSP/TIMER/gtim.h"
#include "./BSP/TIMER/btim.h"

int main(void)
{
    uint8_t j = 0;
    uint32_t rgb_buf[LED_NUM];          /* 各灯珠RGB颜色缓冲区 */
    uint8_t led_state = 0;              /* LED状态：0=关闭，1=开启 */

    sys_stm32_clock_init(9);    /* 系统时钟初始化, 72Mhz */
    delay_init(72);             /* 延时初始化 */
    usart_init(72, 115200);     /* 串口初始化波特率115200 (USART2使用APB1时钟源,最大36MHz) */
    led_init();
    key_init();
    lock_init();                 /* 初始化电磁锁 */
    sr602_init();                 /* 初始化SR602人体感应模块 */
    sw_init();                    /* 初始化SW-18010P震动传感器 */
    ws2812_init();                /* 初始化WS2812B RGB灯带 (PWM+DMA) */
    printf("WS2812B Init OK\r\n");

    printf("STM32 DHT11 TEST\r\n");

    while (dht11_init())        /* DHT11初始化 */
    {
        printf("DHT11 Error\r\n");
        delay_ms(200);
    }

    printf("DHT11 OK\r\n");
    
    dht11_timer_init();                 /* 初始化TIM6定时器中断,用于定时读取DHT11数据 */

    printf("Press KEY1 to turn on/off warm yellow LED\r\n");
    
    /* 初始化时关闭所有LED */
    for(j = 0; j < LED_NUM; j++)
    {
        rgb_buf[j] = GRB888_BLACK;  /* 黑色（关闭） */
    }
    ws2812_display(rgb_buf, LED_NUM);
    
    while (1)
    {
        /* 按键扫描 */
        if (key_scan(0) == KEY1_PRES)
        {
            if (led_state == 0)
            {
                /* 打开LED，显示暖黄色 */
                for(j = 0; j < LED_NUM; j++)
                {
                    rgb_buf[j] = GRB888_WARM_YELLOW;  /* 暖黄色 */
                }
                printf("LED ON: Warm Yellow (0x%06X), LED_NUM=%d\r\n", GRB888_WARM_YELLOW, LED_NUM);
                ws2812_display(rgb_buf, LED_NUM);
                led_state = 1;
            }
            else
            {
                /* 关闭LED */
                for(j = 0; j < LED_NUM; j++)
                {
                    rgb_buf[j] = GRB888_BLACK;  /* 黑色（关闭） */
                }
                ws2812_display(rgb_buf, LED_NUM);
                led_state = 0;
                printf("LED OFF\r\n");
            }
        }
        
        /* 按键扫描 */
        if (key_scan(0) == KEY0_PRES)
        {
            uint8_t current_status = lock_get();
            if (current_status == LOCK_OFF)
            {
                LOCK_LOCK();
                printf("Lock: LOCKED (PA3=LOW, relay ON, NC disconnected)\r\n");
            }
            else
            {
                LOCK_UNLOCK();
                printf("Lock: UNLOCKED (PA3=HIGH, relay OFF, NC connected)\r\n");
            }
            printf("After toggle, PA3 status: %d\r\n", lock_get());
        }
    }
}
