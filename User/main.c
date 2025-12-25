#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./USMART/usmart.h"
#include "./BSP/LED/led.h"
#include "./BSP/DHT11/dht11.h"
#include "./BSP/KEY/key.h"
#include "./BSP/LOCK/lock.h"
#include "./BSP/WIRELESS/driver_wireless.h"
#include "./BSP/SR602/sr602.h"
#include "./BSP/SW-18010P/sw.h"
#include "./BSP/LED-WS2812/ws.h"
#include "./BSP/TIMER/gtim.h"
#include "./BSP/TIMER/btim.h"

#define WIRELESS_SEND_TIME 	3000	//上报数据时间间隔（单位：毫秒）
#define SENSOR_READ_TIME	3000    //获取传感器数据时间间隔（单位：毫秒）
#define KEY_SCAN_TIME		20    	//按键扫描（单位：毫秒）
#define UART4_RX_BUF_MAX    64      // UART4 接收缓冲区大小

uint8_t wireless_send_flag = 0;     //上报数据标志位
uint8_t sensor_read_flag = 0;		//读取传感器数据标志位
uint8_t dht11_available = 0;         //DHT11是否可用标志位
uint8_t wireless_available = 0;      //无线模块是否可用标志位

/* UART4 接收缓冲区（现在在usart.c中定义，这里只声明外部变量） */
extern uint8_t uart4_rx_buffer[UART4_RX_BUF_MAX];
extern volatile uint8_t uart4_rx_index;

/**
 * @brief   字符串比较函数（替代strcmp）
 * @param   str1: 字符串1
 * @param   str2: 字符串2
 * @retval  1-相等, 0-不相等
 */
uint8_t StringCompare(uint8_t *str1, uint8_t *str2)
{
    while (*str1 && *str2)
    {
        if (*str1 != *str2) return 0;
        str1++;
        str2++;
    }
    return (*str1 == *str2) ? 1 : 0;
}

int main(void)
{
    uint8_t j = 0;
    uint32_t rgb_buf[LED_NUM];          /* 各灯珠RGB颜色缓冲区 */
    uint8_t led_state = 0;              /* LED状态：0=关闭，1=开启 */
    
    sys_stm32_clock_init(9);    /* 系统时钟初始化, 72Mhz */
    delay_init(72);             /* 延时初始化 */
    usart_init(72, 115200);     /* 串口初始化波特率115200 (USART2使用APB1时钟源,最大36MHz) */
    
    /* 无线模块初始化，失败则跳过继续运行 */
    wireless_init();                //无线模块初始化并连接onenet mqtt服务器
    if((WirelessStatus.error_code & (ERROR_WiFi_CONNECT | ERROR_MQTT_CONNECT)) == 0)
    {
        wireless_available = 1;
        printf("Wireless Module Init OK\r\n");
    }
    else
    {
        wireless_available = 0;
        printf("Wireless Module Not Available, Continue Without Wireless\r\n");
        printf("Error Code: 0x%04X\r\n", WirelessStatus.error_code);
    }
    
    led_init();
    uart4_init(115200);         /* UART4 初始化，波特率115200，仅接收功能 */
    key_init();
    lock_init();                 /* 初始化电磁锁 */
    sr602_init();                 /* 初始化SR602人体感应模块 */
    sw_init();                    /* 初始化SW-18010P震动传感器 */
    ws2812_init();                /* 初始化WS2812B RGB灯带 (PWM+DMA) */



    /* DHT11初始化，尝试3次，失败则跳过继续运行 */
    uint8_t dht11_init_retry = 0;
    for (dht11_init_retry = 0; dht11_init_retry < 3; dht11_init_retry++)
    {
        if (dht11_init() == 0)
        {
            dht11_available = 1;
            printf("DHT11 Init OK\r\n");
            break;
        }
        else
        {
            printf("DHT11 Error, retry %d/3\r\n", dht11_init_retry + 1);
            delay_ms(200);
        }
    }
    
    if (dht11_available == 0)
    {
        printf("DHT11 Not Available, Continue Without DHT11\r\n");
    }
    else
    {
        dht11_timer_init();                 /* 初始化TIM6定时器中断,用于定时读取DHT11数据 */
    }


    
    /* 初始化时关闭所有LED */
    for(j = 0; j < LED_NUM; j++)
    {
        rgb_buf[j] = GRB888_BLACK;  /* 黑色（关闭） */
    }
    ws2812_display(rgb_buf, LED_NUM);

    /* 先手动上报一次，确认 MQTT 上报流程通畅（仅当无线模块可用时） */
    if (wireless_available == 1)
    {
        wireless_onenet_data_handler();    /* 更新 OneNET 属性缓存 */
        wireless_publish_data();           /* 立即上报一次到 OneNET */
    }
    
    /* 所有初始化完成后再启动 TIM2 定时器（避免中断影响初始化过程中的 delay） */
    timer2_init(100,720);       /* TIM2 1ms 中断，用于定时任务 */
    
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
                lock_lock();
                printf("Lock: LOCKED (PA3=LOW, relay ON, NC disconnected)\r\n");
            }
            else
            {
                lock_unlock();
                printf("Lock: UNLOCKED (PA3=HIGH, relay OFF, NC connected)\r\n");
            }
            printf("After toggle, PA3 status: %d\r\n", lock_get());
        }
		if(sensor_read_flag == 1)			//读取传感器数据标志位
		{
			sensor_read_flag = 0;
			if (dht11_available == 1)        //只有DHT11可用时才读取
			{
				dht11_get_data(&dht11_data);	//获取温湿度数据
			}
		}
		
		if(wireless_send_flag == 1) 		//数据上报标志位
		{
			wireless_send_flag = 0;
			if (wireless_available == 1)  //只有无线模块可用时才处理
			{
				wireless_system_handler();		//执行无线模块相关事件
				wireless_onenet_data_handler();	//处理有关onenet的数据
				if((WirelessStatus.error_code & ERROR_MQTT_CONNECT) == 0)  wireless_publish_data(); 	//发布数据测试
			}
		}		
		if(wireless_get_receive_flag() == W_OK && wireless_available == 1)		//无线模块接收到数据（仅当无线模块可用时）
		{
			wireless_receive_data_handler();		//接收数据处理函数
		}
		
		/* UART4 接收数据处理 */
		if (uart4_get_rx_flag() == 1)
		{
			uint8_t rx_byte = uart4_get_rx_data();
			uart4_clear_rx_flag();
			
			/* 接收到换行符或回车符 = 指令完整 */
			if (rx_byte == '\n' || rx_byte == '\r')
			{
				if (uart4_rx_index > 0)
				{
					uart4_rx_buffer[uart4_rx_index - 1] = '\0';
					
					/* 处理命令 */
					if (StringCompare(uart4_rx_buffer, (uint8_t*)"1") == 1 || 
					    StringCompare(uart4_rx_buffer, (uint8_t*)"ON") == 1 ||
					    StringCompare(uart4_rx_buffer, (uint8_t*)"DETECTED") == 1)
					{
						LED0(0);  /* LED0 亮 */
					}
					else if (StringCompare(uart4_rx_buffer, (uint8_t*)"0") == 1 || 
					         StringCompare(uart4_rx_buffer, (uint8_t*)"OFF") == 1)
					{
						LED0(1);  /* LED0 灭 */
					}
				}
				
				/* 重置缓冲区 */
				uart4_rx_index = 0;
				for (j = 0; j < UART4_RX_BUF_MAX; j++)
				{
					uart4_rx_buffer[j] = 0;
				}
			}
		}
    }
}

/**  
  * @简要  定时器2中断服务函数
  * @参数  	1ms
  * @注意	定时设置标志位 ,
  * @返回值 无  
  */
 void TIM2_IRQHandler(void)
 {
     static uint16_t wireless_send_timer = 0;
     static uint16_t sensor_read_timer = 0;
     static uint16_t key_scan_timer = 0;
     if (TIM2->SR & (1 << 0))  /* 检查更新中断标志位 */
     {
         if(wireless_send_timer++ == WIRELESS_SEND_TIME) {wireless_send_timer = 0;wireless_send_flag = 1;}
         if(sensor_read_timer++ == SENSOR_READ_TIME) {sensor_read_timer = 0;sensor_read_flag = 1;}
         if(key_scan_timer++ == KEY_SCAN_TIME){key_scan_timer = 0; key_scan(1);}
         TIM2->SR &= ~(1 << 0);  /* 清除更新中断标志位 */
     }
 }
 
