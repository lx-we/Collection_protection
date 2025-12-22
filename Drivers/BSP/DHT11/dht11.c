/**
 ****************************************************************************************************
 * @file        dht11.c
 * @author      姝ｇ偣鍘熷瓙鍥㈤槦(ALIENTEK)
 * @version     V1.0
 * @date        2020-06-01
 * @brief       DHT11鏁板瓧娓╂箍搴︿紶鎰熷櫒 椹卞姩浠ｇ爜
 * @license     Copyright (c) 2020-2032, 骞垮窞甯傛槦缈肩數瀛愮?鎶€鏈夐檺鍏?徃
 ****************************************************************************************************
 * @attention
 *
 * 瀹為獙骞冲彴:姝ｇ偣鍘熷瓙 STM32F103寮€鍙戞澘
 * 鍦ㄧ嚎瑙嗛?:www.yuanzige.com
 * 鎶€鏈??鍧?:www.openedv.com
 * 鍏?徃缃戝潃:www.alientek.com
 * 璐?拱鍦板潃:openedv.taobao.com
 *
 * 淇?敼璇存槑
 * V1.0 20200601
 * 绗?竴娆″彂甯?
 *
 ****************************************************************************************************
 */

#include "./BSP/DHT11/dht11.h"
#include "./SYSTEM/delay/delay.h"
#include "./SYSTEM/usart/usart.h"

/**
  * @简要：dht11数据结构体
  * @注意：无
  */
 DHT11_TypeDef dht11_data = {0, 0};


/**
 * @brief       澶嶄綅DHT11
 * @param       data: 瑕佸啓鍏ョ殑鏁版嵁
 * @retval      鏃?
 */
static void dht11_reset(void)
{
    DHT11_DQ_OUT(0);    /* 鎷変綆DQ */
    delay_ms(20);       /* 鎷変綆鑷冲皯18ms */
    DHT11_DQ_OUT(1);    /* DQ=1 */
    delay_us(30);       /* 涓绘満鎷夐珮10~35us */
}

/**
 * @brief       绛夊緟DHT11鐨勫洖搴?
 * @param       鏃?
 * @retval      0, DHT11姝ｅ父
 *              1, DHT11寮傚父/涓嶅瓨鍦?
 */
uint8_t dht11_check(void)
{
    uint8_t retry = 0;
    uint8_t rval = 0;

    while (DHT11_DQ_IN && retry < 100)      /* DHT11浼氭媺浣庣害83us */
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

        while (!DHT11_DQ_IN && retry < 100) /* DHT11鎷変綆鍚庝細鍐嶆?鎷夐珮绾?87us */
        {
            retry++;
            delay_us(1);
        }
        if (retry >= 100) rval = 1;
    }
    
    return rval;
}

/**
 * @brief       浠嶥HT11璇诲彇涓€涓?綅
 * @param       鏃?
 * @retval      璇诲彇鍒扮殑浣嶅€?: 0 / 1
 */
uint8_t dht11_read_bit(void)
{
    uint8_t retry = 0;

    while (DHT11_DQ_IN && retry < 100)  /* 绛夊緟鍙樹负浣庣數骞? */
    {
        retry++;
        delay_us(1);
    }

    retry = 0;

    while (!DHT11_DQ_IN && retry < 100) /* 绛夊緟鍙橀珮鐢靛钩 */
    {
        retry++;
        delay_us(1);
    }

    delay_us(40);       /* 绛夊緟40us */

    if (DHT11_DQ_IN)    /* 鏍规嵁寮曡剼鐘舵€佽繑鍥? bit */
    {
        return 1;
    }
    else 
    {
        return 0;
    }
}

/**
 * @brief       浠嶥HT11璇诲彇涓€涓?瓧鑺?
 * @param       鏃?
 * @retval      璇诲埌鐨勬暟鎹?
 */
static uint8_t dht11_read_byte(void)
{
    uint8_t i, data = 0;

    for (i = 0; i < 8; i++)         /* 寰?幆璇诲彇8浣嶆暟鎹? */
    {
        data <<= 1;                 /* 楂樹綅鏁版嵁鍏堣緭鍑?, 鍏堝乏绉讳竴浣? */
        data |= dht11_read_bit();   /* 璇诲彇1bit鏁版嵁 */
    }

    return data;
}

/**
 * @brief       浠嶥HT11璇诲彇涓€娆℃暟鎹?
 * @param       temp: 娓╁害鍊?(鑼冨洿:-20~60掳)
 * @param       humi: 婀垮害鍊?(鑼冨洿:5%~95%)
 * @retval      0, 姝ｅ父.
 *              1, 澶辫触
 */
uint8_t dht11_read_data(uint8_t *temp, uint8_t *humi)
{
    uint8_t buf[5];
    uint8_t i;
    dht11_reset();

    if (dht11_check() == 0)
    {
        for (i = 0; i < 5; i++)     /* 璇诲彇40浣嶆暟鎹? */
        {
            buf[i] = dht11_read_byte();
        }

        if ((buf[0] + buf[1] + buf[2] + buf[3]) == buf[4])
        {
            *humi = buf[0];
            *temp = buf[2];

            /* 更新全局温湿度结构体 */
            dht11_data.humi = buf[0];
            dht11_data.temp = (float)buf[2];
        }
    }
    else
    {
        return 1;
    }
    
    return 0;
}

/**
 * @brief       鍒濆?鍖朌HT11鐨処O鍙? DQ 鍚屾椂妫€娴婦HT11鐨勫瓨鍦?
 * @param       鏃?
 * @retval      0, 姝ｅ父
 *              1, 涓嶅瓨鍦?/涓嶆?甯?
 */
uint8_t dht11_init(void)
{
    DHT11_DQ_GPIO_CLK_ENABLE();     /* 寮€鍚疍Q寮曡剼鏃堕挓 */

    /* DHT11_DQ寮曡剼妯″紡璁剧疆,寮€婕忚緭鍑?,涓婃媺, 杩欐牱灏变笉鐢ㄥ啀璁剧疆IO鏂瑰悜浜?, 寮€婕忚緭鍑虹殑鏃跺€?(=1), 涔熷彲浠ヨ?鍙栧?閮ㄤ俊鍙风殑楂樹綆鐢靛钩 */
    sys_gpio_set(DHT11_DQ_GPIO_PORT, DHT11_DQ_GPIO_PIN,
                 SYS_GPIO_MODE_OUT, SYS_GPIO_OTYPE_OD, SYS_GPIO_SPEED_MID, SYS_GPIO_PUPD_PU);

    dht11_reset();
    return dht11_check();
}

/**
 * @brief       鍒濆?鍖朤IM6瀹氭椂鍣ㄤ腑鏂?,鐢ㄤ簬瀹氭椂璇诲彇DHT11鏁版嵁
 * @param       鏃?
 * @retval      鏃?
 */
void dht11_timer_init(void)
{
    /* 浣胯兘TIM6鏃堕挓 (TIM6鍦ˋPB1鎬荤嚎涓?) */
    RCC->APB1ENR |= 1 << 4;  /* TIM6鏃堕挓浣胯兘 */

    /* 閰嶇疆TIM6瀹氭椂鍣?: 2绉掍腑鏂?竴娆? */
    /* 绯荤粺鏃堕挓72MHz, APB1=36MHz, TIM6鏃堕挓=72MHz(鍊嶉?) */
    /* 棰勫垎棰戝櫒PSC = 7199, 瀹氭椂鍣ㄦ椂閽? = 72MHz / 7200 = 10kHz */
    /* 鑷?姩閲嶈?杞藉€糀RR = 20000, 涓?柇鍛ㄦ湡 = 20000 / 10kHz = 2绉? */
    TIM6->PSC = 7199;          /* 棰勫垎棰戝櫒, 7200鍒嗛? */
    TIM6->ARR = 20000;         /* 鑷?姩閲嶈?杞藉€?, 20000涓??鏁? */
    TIM6->CNT = 0;             /* 璁℃暟鍣ㄦ竻闆? */
    TIM6->DIER |= 1 << 0;      /* 浣胯兘鏇存柊涓?柇 */
    TIM6->CR1 |= 1 << 0;       /* 浣胯兘瀹氭椂鍣? */

    /* 閰嶇疆NVIC涓?柇 */
    sys_nvic_init(2, 2, TIM6_DAC_IRQn, 2);  /* 鎶㈠崰浼樺厛绾?2,瀛愪紭鍏堢骇2,涓?柇閫氶亾TIM6_DAC_IRQn,鍒嗙粍2 */
}

/**
 * @brief       TIM6鏇存柊涓?柇鏈嶅姟鍑芥暟(瀹氭椂璇诲彇DHT11鏁版嵁)
 * @note        TIM6鍜孌AC鍏变韩鍚屼竴涓?腑鏂?悜閲忥紝涓?柇鏈嶅姟鍑芥暟鍚嶄负TIM6_DAC_IRQHandler
 *              娉ㄦ剰锛歜tim.c涓?殑BTIM_TIMX_INT_IRQHandler()宸茶?缃?负寮辩?鍙?(__weak)锛?
 *              鎵€浠ユ?鍑芥暟浼氳?鐩朾tim.c涓?殑寮辩?鍙峰嚱鏁帮紝涓嶄細浜х敓閲嶅?瀹氫箟閿欒?
 * @param       鏃?
 * @retval      鏃?
 */
void TIM6_DAC_IRQHandler(void)
{
    /* 妫€鏌?IM6鏇存柊涓?柇鏍囧織浣? */
    if (TIM6->SR & 0x01)  /* 妫€鏌ユ洿鏂颁腑鏂?爣蹇椾綅 */
    {
        TIM6->SR &= ~(1 << 0);  /* 娓呴櫎鏇存柊涓?柇鏍囧織浣? */
        
        /* 璇诲彇DHT11鏁版嵁 */
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
























