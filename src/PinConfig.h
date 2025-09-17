#pragma once

// Định nghĩa các chân GPIO sử dụng cho toàn bộ dự án
// Sửa đổi tại đây sẽ đồng bộ cho tất cả các module

// LED Outputs
#define GPIO_LED_OUT_1   5
#define GPIO_LED_OUT_2   18
#define GPIO_LED_OUT_3   19
#define GPIO_LED_OUT_4   21

// LCD I2C
#define GPIO_LCD_SDA     21
#define GPIO_LCD_SCL     22

// Rotary Encoder
#define GPIO_ROTARY_A    32
#define GPIO_ROTARY_B    33
#define GPIO_ROTARY_BTN  25

// Status LED
#define GPIO_STATUS_LED  2


// W5500 Ethernet (SPI)
#define GPIO_W5500_MOSI   23
#define GPIO_W5500_MISO   19
#define GPIO_W5500_SCK    18
#define GPIO_W5500_CS     15
#define GPIO_W5500_RST    12

// SD Card (nếu dùng SPI)
#define GPIO_SD_MOSI      23
#define GPIO_SD_MISO      19
#define GPIO_SD_SCK       18
#define GPIO_SD_CS        4

// Các module khác có thể bổ sung tại đây

