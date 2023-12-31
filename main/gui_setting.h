#ifndef GUI_SETTING_H
#define GUI_SETTING_H

#include "esp_lcd_ili9341.h"
#include "esp_err.h"
#include "esp_log.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_lcd_touch_xpt2046.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "lvgl.h"
#include "esp_timer.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"

#define LCD_HOST  SPI2_HOST
#define LCD_PIXEL_CLOCK_HZ     (20 * 1000 * 1000)
#define LCD_BK_LIGHT_ON_LEVEL  1
#define LCD_BK_LIGHT_OFF_LEVEL !LCD_BK_LIGHT_ON_LEVEL
#define PIN_NUM_SCLK           18
#define PIN_NUM_MOSI           19
#define PIN_NUM_MISO           21
#define PIN_NUM_LCD_DC         5
#define PIN_NUM_LCD_RST        3
#define PIN_NUM_LCD_CS         4
#define PIN_NUM_BK_LIGHT       2
#define PIN_NUM_TOUCH_CS       15
#define LCD_H_RES              240
#define LCD_V_RES              320
#define LCD_CMD_BITS           8
#define LCD_PARAM_BITS         8
#define LVGL_TICK_PERIOD_MS    2

void gui_init(void);

#endif // GUI_SETTING_H