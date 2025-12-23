# Collection_protection
收藏品防护系统


设备/传感器概览（STM32F103，72 MHz）

- DHT11 温湿度传感器  
  - 作用：采集环境温度、湿度，上报到 OneNET  
  - 引脚：PA1（DQ 数据线）  
  - 定时器/中断：TIM6 定时更新中断，周期性读取并更新 `dht11_data`

- SR602 人体感应模块  
  - 作用：检测有人/无人，上报到 OneNET 的 `human_detected`  
  - 引脚：PC7（数字输出）  
  - 定时器/中断：TIM1 定时更新中断（100 ms 轮询状态）

- SW-18010P 震动传感器  
  - 作用：检测震动，上报到 OneNET 的 `Vibration_detected`  
  - 引脚：PC6（数字输出）  
  - 中断：EXTI6（EXTI9_5_IRQn，双边沿触发）

- 电磁锁（继电器）  
  - 作用：上锁/解锁，状态上报 `Lock_Control`  
  - 引脚：PA3（开漏输出，低电平吸合上锁，NC 触点）

- WS2812B 灯带  
  - 作用：指示灯带，可调亮度/颜色，属性 `RBGLED`  
  - 引脚：PA6（TIM3_CH1 PWM 输出）  
  - 定时器/DMA：TIM3 + DMA1_Channel6 发送 PWM 数据帧

- ESP8266 无线模块（OneNET MQTT）  
  - 作用：联网，上报/订阅 OneNET 属性  
  - 引脚：USART3 — PB10 (TX), PB11 (RX)  
  - 中断：USART3 RXNE 中断接收 AT 响应

- TIM2 周期定时  
  - 作用：1 ms 周期；置位无线上报标志、传感器读取标志等（`timer2_init(100,720)` 在时钟初始化后调用）

上报属性（OneNET）对应关系：
- `Lock_Control`（bool）：PA3 继电器，上锁/解锁
- `Vibration_detected`（bool）：PC6 震动
- `human_detected`（bool）：PC7 人体感应
- `RBGLED`（int 0–100）：WS2812 亮度百分比
- `temp`（float）：DHT11 温度
- `humidity`（float）：DHT11 湿度