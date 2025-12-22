/**
 ****************************************************************************************************
 * @file        usart.h
 * @author      閿熸枻鎷烽敓鏂ゆ嫹鍘熼敓鏂ゆ嫹閿熻剼璁规嫹(ALIENTEK)
 * @version     V1.0
 * @date        2020-04-17
 * @brief       閿熸枻鎷烽敓鑺傜鎷峰閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷�(涓€閿熸枻鎷烽敓瑙掕揪鎷烽敓鏂ゆ嫹1)閿熸枻鎷锋敮閿熸枻鎷穚rintf
 * @license     Copyright (c) 2020-2032, 閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸帴绉戠》鎷烽敓鏂ゆ嫹閿熺潾鐧告嫹鍙�
 ****************************************************************************************************
 * @attention
 *
 * 瀹為敓鏂ゆ嫹骞冲彴:閿熸枻鎷烽敓鏂ゆ嫹鍘熼敓鏂ゆ嫹 STM32F103閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷�
 * 閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽:www.yuanzige.com
 * 閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷峰潧:www.openedv.com
 * 閿熸枻鎷峰徃閿熸枻鎷峰潃:www.alientek.com
 * 閿熸枻鎷烽敓鏂ゆ嫹閿熻锟�:openedv.taobao.com
 *
 * 閿熺潾闈╂嫹璇撮敓鏂ゆ嫹
 * V1.0 20200417
 * 閿熸枻鎷蜂竴閿熻娇鍑ゆ嫹閿熸枻鎷�
 *
 ****************************************************************************************************
 */

#ifndef __USART_H
#define __USART_H

#include "stdio.h"
#include "./SYSTEM/sys/sys.h"


/******************************************************************************************/
/* 閿熸枻鎷烽敓鏂ゆ嫹 閿熸枻鎷� 閿熸枻鎷烽敓鏂ゆ嫹 閿熸枻鎷烽敓鏂ゆ嫹 
 * 榛橀敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熺祵SART1閿熸枻鎷�.
 * 娉ㄩ敓鏂ゆ嫹: 閫氶敓鏂ゆ嫹閿熺潾闈╂嫹閿熸枻鎷�12閿熸枻鎷烽敓鐤ュ畾閿熸枻鎷�,閿熸枻鎷烽敓鏂ゆ嫹鏀敓鏂ゆ嫹USART1~UART7閿熸枻鎷烽敓鏂ゆ嫹涓€閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷�.
 */
#define USART_TX_GPIO_PORT                  GPIOC
#define USART_TX_GPIO_PIN                   SYS_GPIO_PIN9
#define USART_TX_GPIO_CLK_ENABLE()          do{ RCC->APB2ENR |= 1 << 2; }while(0)   /* PA閿熸枻鎷锋椂閿熸枻鎷蜂娇閿熸枻鎷� */

#define USART_RX_GPIO_PORT                  GPIOA
#define USART_RX_GPIO_PIN                   SYS_GPIO_PIN10
#define USART_RX_GPIO_CLK_ENABLE()          do{ RCC->APB2ENR |= 1 << 2; }while(0)   /* PA閿熸枻鎷锋椂閿熸枻鎷蜂娇閿熸枻鎷� */

#define USART_UX                            USART1
#define USART_UX_IRQn                       USART1_IRQn
#define USART_UX_IRQHandler                 USART1_IRQHandler
#define USART_UX_CLK_ENABLE()               do{ RCC->APB2ENR |= 1 << 14; }while(0)  /* USART1 鏃堕敓鏂ゆ嫹浣块敓鏂ゆ嫹 */

/******************************************************************************************/

/******************************************************************************************/
/* UART 鏃堕敓鏂ゆ嫹婧愰敓鏂ゆ嫹閿熸枻鎷�
 * 璇撮敓鏂ゆ嫹:
 * - APB1鏃堕敓鏂ゆ嫹婧�: 閿熸枻鎷烽敓鏂ゆ嫹USART2, USART3, UART4, UART5, 閿熸枻鎷烽敓鐙＄鎷烽敓杞匡拷36MHz (PCLK1)
 * - APB2鏃堕敓鏂ゆ嫹婧�: 閿熸枻鎷烽敓鏂ゆ嫹USART1, 閿熸枻鎷烽敓鐙＄鎷烽敓杞匡拷72MHz (PCLK2)
 * 
 * 閿熸枻鎷烽敓缁炵櫢鎷烽敓绱窹B1鏃堕敓鏂ゆ嫹婧愰敓鏂ゆ嫹UART (閿熸枻鎷稶SART2/3/4/5), 閿熸枻鎷蜂娇閿熸枻鎷烽敓鏂ゆ嫹閿熼摪瀹忓畾閿熸枻鎷�:
 * #define USART_UX_CLK_ENABLE()  do{ RCC->APB1ENR |= 1 << 17; }while(0)  /* USART2 鏃堕敓鏂ゆ嫹浣块敓鏂ゆ嫹(APB1) */
 * 閿熸枻鎷风‘閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷穟sart_init閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷锋椂閿熸枻鎷烽閿熺粸璇ф嫹閿熸枻鎷烽敓鏂ゆ嫹36MHz
 */
#define USART_CLK_SOURCE_APB1_MAX_FREQ      36          /* APB1鏃堕敓鏂ゆ嫹婧愰敓鏂ゆ嫹閿熺嫛纰夋嫹閿燂拷: 36MHz */
#define USART_CLK_SOURCE_APB2_MAX_FREQ      72          /* APB2鏃堕敓鏂ゆ嫹婧愰敓鏂ゆ嫹閿熺嫛纰夋嫹閿燂拷: 72MHz */

/******************************************************************************************/


#define USART_REC_LEN               200         /* 閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓琛楁枻鎷烽敓鏂ゆ嫹 200 */
#define USART_EN_RX                 1           /* 浣块敓鏉帮綇鎷�1閿熸枻鎷�/閿熸枻鎷锋閿熸枻鎷�0閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷�1閿熸枻鎷烽敓鏂ゆ嫹 */


extern uint8_t  g_usart_rx_buf[USART_REC_LEN];  /* 閿熸枻鎷烽敓绉镐紮鎷烽敓鏂ゆ嫹,閿熸枻鎷烽敓绲孲ART_REC_LEN閿熸枻鎷烽敓琛楁枻鎷�.鏈敓琛楁枻鎷蜂负閿熸枻鎷烽敓鍙嚖鎷� */
extern uint16_t g_usart_rx_sta;                 /* 閿熸枻鎷烽敓鏂ゆ嫹鐘舵€侀敓鏂ゆ嫹閿燂拷 */

void usart_init(uint32_t pclk2, uint32_t bound);/* 閿熸枻鎷烽敓鑺傜鎷峰閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷� */

#endif  


/* 濡傛灉娌℃湁瀹氫箟uint32_t,鍒欏畾涔� */
#ifndef uint32_t
typedef unsigned           char uint8_t;
typedef unsigned short     int  uint16_t;
typedef unsigned           int  uint32_t;
#endif



char * usmart_get_input_string(void);	/* 鑾峰彇杈撳叆鏁版嵁娴� */
void usmart_timx_reset_time(void);      /* 澶嶄綅杩愯鏃堕棿 */
uint32_t usmart_timx_get_time(void);    /* 鑾峰彇杩愯鏃堕棿 */
void usmart_timx_init(uint16_t tclk);   /* 鍒濆鍖栧畾鏃跺櫒 */

#endif



























            {
                if (rxdata != 0x0a)             /* ???????0x0a? (????????????0x0d,????0x0a) */
                {
                    g_usart_rx_sta = 0;         /* ???????, ?????? */
                }
                else
                {
                    g_usart_rx_sta |= 0x8000;   /* ?????0x0a,??????????? */
                }
            }
            else      /* ??????0x0d */
            {
                if (rxdata == 0x0d)
                {
                    g_usart_rx_sta |= 0x4000;   /* ????????? 0x0d */
                }
                else
                {
                    g_usart_rx_buf[g_usart_rx_sta & 0X3FFF] = rxdata;   /* ?锟斤拷????? g_usart_rx_buf */
                    g_usart_rx_sta++;

                    if (g_usart_rx_sta > (USART_REC_LEN - 1))g_usart_rx_sta = 0;/* ???????????, ?????????? */
                }
            }
        }
    }

#if SYS_SUPPORT_OS  /* ???SYS_SUPPORT_OS??锟斤拷????????OS. */
    OSIntExit();
#endif
}
#endif

/**
 * @brief       ????X?????????
 * @param       sclk: ????X?????????(??锟斤拷: MHz)
 *              ????1 ??????????: APB2 (PCLK2), ??????? 72MHz
 *              ????2/3/4/5 ??????????: APB1 (PCLK1), ??????? 36MHz
 * @note        ???: 
 *              1. ?????????????sclk, ??????????????????.
 *              2. ???UART???APB1????, ???sclk??????36MHz (USART_CLK_SOURCE_APB1_MAX_FREQ)
 *              3. ???UART???APB2????(??USART1), sclk?????72MHz (USART_CLK_SOURCE_APB2_MAX_FREQ)
 * @param       baudrate: ??????, ?????????????锟斤拷??????
 * @retval      ??
 */
void usart_init(uint32_t sclk, uint32_t baudrate)
{
    uint32_t temp;
    /* IO ?? ??????? */
    USART_TX_GPIO_CLK_ENABLE(); /* ??????TX????? */
    USART_RX_GPIO_CLK_ENABLE(); /* ??????RX????? */
    USART_UX_CLK_ENABLE();      /* ????????? */

    sys_gpio_set(USART_TX_GPIO_PORT, USART_TX_GPIO_PIN,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_PU);   /* ????TX?? ?????? */

    sys_gpio_set(USART_RX_GPIO_PORT, USART_RX_GPIO_PIN,
                 SYS_GPIO_MODE_IN, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_PU);   /* ????RX?? ???????锟斤拷??????? */

    temp = (sclk * 1000000 + baudrate / 2) / baudrate;  /* ???BRR, ??????????????? */
    /* ?????????? */
    USART_UX->BRR = temp;       /* ?????????? */
    USART_UX->CR1 = 0;          /* ????CR1????? */
    USART_UX->CR1 |= 0 << 12;   /* M = 0, 1?????锟斤拷, 8??????锟斤拷, n????锟斤拷(??USART_CR2 STOP[1:0]???, ?????0, ???1????锟斤拷) */
    USART_UX->CR1 |= 1 << 3;    /* TE = 1, ?????????? */
#if USART_EN_RX  /* ??????????? */
    /* ???????锟斤拷? */
    USART_UX->CR1 |= 1 << 2;    /* RE = 1, ?????????? */
    USART_UX->CR1 |= 1 << 5;    /* RXNEIE = 1, ?????????????锟斤拷???? */
    sys_nvic_init(3, 3, USART_UX_IRQn, 2); /* ??2?????????? */
#endif
    USART_UX->CR1 |= 1 << 13;   /* UE = 1, ??????? */
}

/**
 * @brief       USART2鍙戦€佸崟瀛楄妭
 * @param       byte: 瑕佸彂閫佺殑瀛楄妭鏁版嵁
 * @retval      鏃�
 * @note        灏嗗瓧鑺傛暟鎹啓鍏ユ暟鎹瘎瀛樺櫒锛屽啓鍏ュ悗USART鑷姩鐢熸垚鏃跺簭娉㈠舰
 *              绛夊緟鍙戦€佸畬鎴愭爣蹇椾綅锛屼笅娆″啓鍏ユ暟鎹瘎瀛樺櫒浼氳嚜鍔ㄦ竻闄ゅ彂閫佸畬鎴愭爣蹇椾綅
 */
void uart2_send_byte(uint8_t byte)
{
    USART2->DR = byte;                                    /* 灏嗗瓧鑺傛暟鎹啓鍏ユ暟鎹瘎瀛樺櫒 */
    while ((USART2->SR & 0X40) == 0);                     /* 绛夊緟鍙戦€佸畬鎴�(TXE鏍囧織浣�) */
    /* 涓嬫鍐欏叆鏁版嵁瀵勫瓨鍣ㄤ細鑷姩娓呴櫎鍙戦€佸畬鎴愭爣蹇椾綅锛屾晠姝ゅ惊鐜悗锛屾棤闇€娓呴櫎鏍囧織浣� */
}

/**
 * @brief       USART2鍙戦€佸瓧绗︿覆
 * @param       str: 瀛楃鏁扮粍鎸囬拡
 * @retval      鏃�
 * @note        寰幆鍙戦€佸瓧绗︿覆涓殑姣忎釜瀛楃锛岀洿鍒伴亣鍒扮粨鏉熺'\0'
 */
void uart2_send_string(char *str)
{
    while(*str != '\0')
    {
        uart2_send_byte(*str++);
    }
}

/* 鍓嶅悜澹版槑 wireless_receive_callback 鍑芥暟 */
extern void wireless_receive_callback(uint8_t byte);

/**
 * @brief       USART2涓插彛鎺ユ敹涓柇鏈嶅姟鍑芥暟
 * @param       鏃�
 * @retval      鏃�
 * @note        灏嗘棤绾挎ā鍧楁帴鏀舵暟鎹嚱鏁版斁鍦ㄤ覆鍙ｆ帴鏀朵腑鏂腑锛屾帴鏀舵暟鎹�
 */
void USART2_IRQHandler(void)
{
    uint8_t data = 0;
    
    /* 妫€鏌ユ槸鍚﹀彂鐢熸帴鏀朵腑鏂紙RXNE: 鎺ユ敹鏁版嵁瀵勫瓨鍣ㄩ潪绌猴級 */
    if ((USART2->SR & (1 << 5)) != 0)      /* RXNE鏍囧織浣� (bit 5) */
    {
        data = (uint8_t)(USART2->DR & 0xFF);   /* 璇诲彇鎺ユ敹鍒扮殑鏁版嵁 */
        
        /* 灏嗘棤绾挎ā鍧楁帴鏀舵暟鎹嚱鏁版斁鍦ㄤ覆鍙ｆ帴鏀朵腑鏂腑锛屾帴鏀舵暟鎹� */
        wireless_receive_callback(data);
        
        /* 璇诲彇DR瀵勫瓨鍣ㄤ細鑷姩娓呴櫎RXNE鏍囧織浣嶏紝鏃犻渶鎵嬪姩娓呴櫎 */
    }
}

/**
  * @brief  涓插彛鍒濆鍖�
  * @param  Baud: 娉㈢壒鐜囧ぇ灏�
  * @retval 鏃�
  */
void Serial_Init(uint32_t Baud)
{
    uint32_t temp;
    
    /* 寮€鍚椂閽� */
    RCC->APB2ENR |= 1 << 14;    /* 寮€鍚疷SART1鐨勬椂閽� */
    RCC->APB2ENR |= 1 << 2;     /* 寮€鍚疓PIOA鐨勬椂閽� */
    
    /* GPIO鍒濆鍖� - PA9(TX): 澶嶇敤鎺ㄦ尳杈撳嚭, 50MHz */
    sys_gpio_set(GPIOA, SYS_GPIO_PIN9,
                 SYS_GPIO_MODE_AF, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_NONE);
    
    /* GPIO鍒濆鍖� - PA10(RX): 涓婃媺杈撳叆 */
    sys_gpio_set(GPIOA, SYS_GPIO_PIN10,
                 SYS_GPIO_MODE_IN, SYS_GPIO_OTYPE_PP, SYS_GPIO_SPEED_HIGH, SYS_GPIO_PUPD_PU);
    
    /* USART鍒濆鍖� - 璁＄畻BRR鍊� (绯荤粺鏃堕挓72MHz, USART1浣跨敤APB2) */
    temp = (72000000 + Baud / 2) / Baud;  /* 寰楀埌BRR, 閲囩敤鍥涜垗浜斿叆璁＄畻 */
    USART1->BRR = temp;                   /* 娉㈢壒鐜囪缃� */
    
    /* USART閰嶇疆: 8浣嶆暟鎹綅, 1浣嶅仠姝綅, 鏃犲鍋舵牎楠�, 鏃犵‖浠舵祦鎺у埗 */
    USART1->CR1 = 0;                      /* 娓呴浂CR1瀵勫瓨鍣� */
    USART1->CR1 |= 0 << 12;               /* M = 0, 8浣嶆暟鎹綅 */
    USART1->CR1 |= 1 << 3;                /* TE = 1, 鍙戦€佷娇鑳� */
    USART1->CR1 |= 1 << 2;                /* RE = 1, 鎺ユ敹浣胯兘 */
    
    /* 涓柇杈撳嚭閰嶇疆 - 寮€鍚帴鏀朵腑鏂� */
    USART1->CR1 |= 1 << 5;                /* RXNEIE = 1, 鎺ユ敹涓柇浣胯兘 */
    
    /* NVIC涓柇鍒嗙粍鍜岄厤缃� - 鎶㈠崰浼樺厛绾�1, 鍝嶅簲浼樺厛绾�1 */
    sys_nvic_init(1, 1, USART1_IRQn, 2);  /* 缁�2锛屾姠鍗犱紭鍏堢骇1锛屽搷搴斾紭鍏堢骇1 */
    
    /* USART浣胯兘 */
    USART1->CR1 |= 1 << 13;               /* UE = 1, 涓插彛浣胯兘 */
}

/**
  * @brief  涓插彛鍙戦€佷竴涓瓧鑺�
  * @param  Byte: 瑕佸彂閫佺殑涓€涓瓧鑺�
  * @retval 鏃�
  */
void Serial_SendByte(uint8_t Byte)
{
    USART1->DR = Byte;                        /* 灏嗗瓧鑺傛暟鎹啓鍏ユ暟鎹瘎瀛樺櫒 */
    while ((USART1->SR & 0X40) == 0);        /* 绛夊緟鍙戦€佸畬鎴�(TXE鏍囧織浣�) */
    /* 涓嬫鍐欏叆鏁版嵁瀵勫瓨鍣ㄤ細鑷姩娓呴櫎鍙戦€佸畬鎴愭爣蹇椾綅锛屾晠姝ゅ惊鐜悗锛屾棤闇€娓呴櫎鏍囧織浣� */
}

/**
  * @brief  涓插彛鍙戦€佷竴涓瓧绗︿覆
  * @param  String: 瑕佸彂閫佸瓧绗︿覆鐨勯鍦板潃
  * @retval 鏃�
  */
void Serial_SendString(char *String)
{
    uint8_t i;
    for (i = 0; String[i] != '\0'; i++)      /* 閬嶅巻瀛楃鏁扮粍锛堝瓧绗︿覆锛夛紝閬囧埌瀛楃涓茬粨鏉熸爣蹇椾綅鍚庡仠姝� */
    {
        Serial_SendByte(String[i]);          /* 渚濇璋冪敤Serial_SendByte鍙戦€佹瘡涓瓧鑺傛暟鎹� */
    }
}

/* FILE 鍦� stdio.h閲岄潰瀹氫箟 */
FILE __stdout;

/**
  * @brief  浣跨敤printf闇€瑕侀噸瀹氬悜鐨勫簳灞傚嚱鏁�
  * @param  ch: 瑕佸彂閫佺殑瀛楃
  * @param  f: 鏂囦欢鎸囬拡锛堜繚鎸佸師濮嬫牸寮忥級
  * @retval 杩斿洖鍙戦€佺殑瀛楃
  * @note   灏唒rintf鐨勫簳灞傞噸瀹氬悜鍒癠SART1鐨勫彂閫佸瓧鑺傚嚱鏁�
  */
int fputc(int ch, FILE *f)
{
    Serial_SendByte(ch);                     /* 灏唒rintf鐨勫簳灞傞噸瀹氬悜鍒拌嚜宸辩殑鍙戦€佸瓧鑺傚嚱鏁� */
    return ch;
}




