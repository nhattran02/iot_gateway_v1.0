#ifndef UI_MAIN_H
#define UI_MAIN_H

#include <stdbool.h>
#include "esp_err.h"
#include "lvgl.h"

typedef struct
{
    lv_style_t style;
    lv_style_t style_focus_no_outline;
    lv_style_t style_focus;
    lv_style_t style_pr;
} button_style_t;

void ui_main_start(void);
lv_obj_t *ui_main_get_status_bar(void);
button_style_t *ui_button_styles(void);


#endif // UI_MAIN_H
