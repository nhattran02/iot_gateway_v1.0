#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ui.h"
#include "gui_setting.h"
#include "ui_main.h"

static const char *TAG = "main";

void app_main(void)
{
    gui_init();
    ESP_LOGI(TAG, "Start UI");

    ui_main_start();
    // ui_init();

    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(10));
        lv_timer_handler();
    }
}
