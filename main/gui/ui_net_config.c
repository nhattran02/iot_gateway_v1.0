#include "esp_log.h"
#include "lvgl.h"
#include "ui_net_config.h"
#include "ui_main.h"
#include "app_wifi.h"

static const char *TAG = "ui_net_config";

LV_FONT_DECLARE(font_en_16);

LV_IMG_DECLARE(wifi_icon)
LV_IMG_DECLARE(i4G);
LV_IMG_DECLARE(bluetooth_icon);

typedef struct
{
    char *name;
    void *img_src;
    void (*start_fn)(void (*fn)(void));
    void (*end_fn)(void);
} item_desc_t;

static void ui_wifi_start(void (*fn)(void));
static void ui_4G_start(void (*fn)(void));
static void ui_BLE_start(void (*fn)(void));

static void ui_wifi_end(void);
static void ui_BLE_end(void);
static void ui_4G_end(void);

static item_desc_t net_item[] = {
    {"WiFi",       (void *)&wifi_icon,          ui_wifi_start,  ui_wifi_end},
    {"4G",         (void *)&i4G,                NULL/*ui_4G_start*/,    NULL/*ui_4G_end*/},
    {"BLE",        (void *)&bluetooth_icon,     NULL/*ui_BLE_start*/,   NULL/*ui_BLE_end*/},
};

static ui_net_type_t g_active_net_type = UI_NET_WIFI;
static lv_obj_t *g_btn_app_hint = NULL;
static lv_obj_t *g_hint_lab = NULL;
static lv_obj_t *g_qr = NULL;
static lv_obj_t *g_img = NULL;
static lv_obj_t *g_page_wifi = NULL;
static void (*g_net_wifi_end_cb)(void) = NULL;

static ui_net_state_t g_net_state = UI_NET_EVT_LOARDING;
static lv_obj_t *g_btn_return = NULL;
static lv_obj_t *g_func_btn[3] = {NULL};

static void (*g_net_config_end_cb)(void) = NULL;

static void btn_return_down_cb(void *handle, void *arg);

static void ui_net_config_page_return_click_cb(lv_event_t *e)
{
    lv_obj_t *obj = lv_event_get_user_data(e);
    lv_obj_del(obj);
    ESP_LOGI(TAG, "net config end");
    // g_page = NULL;
    g_func_btn[0] = NULL;
    g_func_btn[1] = NULL;
    g_func_btn[2] = NULL;
    if (g_net_config_end_cb)
    {
        g_net_config_end_cb();
    }
}

typedef struct
{
    ui_net_type_t type;
    const char *name;
    lv_img_dsc_t const *img_on;
} btn_img_src_t;

static const btn_img_src_t img_src_list[] = {
    {.type = UI_NET_WIFI, .name = "WiFi", .img_on = &wifi_icon},
    {.type = UI_NET_4G, .name = "4G", .img_on = &i4G},
    {.type = UI_NET_BLE, .name = "BLE", .img_on = &bluetooth_icon},
};

static void ui_net_config_page_app_click_cb(lv_event_t *e)
{
    /* **************** FRAMWORK **************** */
    lv_obj_t *page = lv_obj_create(lv_scr_act());
    lv_obj_set_size(page, lv_obj_get_width(lv_obj_get_parent(page)), 185);
    lv_obj_clear_flag(page, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(page, 15, LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(page, 1, LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(page, 20, LV_PART_MAIN);
    lv_obj_set_style_shadow_opa(page, LV_OPA_30, LV_PART_MAIN);
    lv_obj_align(page, LV_ALIGN_CENTER, 0, 0);
}

static void ui_net_wifi_page_return_click_cb(lv_event_t *e)
{
    lv_obj_t *obj = lv_event_get_user_data(e);
    lv_obj_del(obj);
    ESP_LOGI(TAG, "net wifi end");

    if(g_net_wifi_end_cb){
        g_net_wifi_end_cb();
    }
}

static void ui_wifi_start(void (*fn)(void))
{
    ESP_LOGI(TAG, "net wifi start");
    g_net_wifi_end_cb = fn;

    lv_obj_t *g_page_wifi = lv_obj_create(lv_scr_act());
    lv_obj_set_size(g_page_wifi, lv_obj_get_width(lv_obj_get_parent(g_page_wifi)), lv_obj_get_height(lv_obj_get_parent(g_page_wifi)) - lv_obj_get_height(ui_main_get_status_bar()));
    lv_obj_set_style_border_width(g_page_wifi, 0, LV_PART_MAIN);
    lv_obj_set_style_bg_color(g_page_wifi, lv_obj_get_style_bg_color(lv_scr_act(), LV_STATE_DEFAULT), LV_PART_MAIN);
    lv_obj_clear_flag(g_page_wifi, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_align_to(g_page_wifi, ui_main_get_status_bar(), LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);        
    
        
    lv_obj_t *btn_return = lv_btn_create(g_page_wifi);
    lv_obj_set_size(btn_return, 40, 40);
    lv_obj_add_style(btn_return, &ui_button_styles()->style, 0);
    lv_obj_add_style(btn_return, &ui_button_styles()->style_pr, LV_STATE_PRESSED);
    lv_obj_add_style(btn_return, &ui_button_styles()->style_focus, LV_STATE_FOCUS_KEY);
    lv_obj_add_style(btn_return, &ui_button_styles()->style_focus, LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_color(btn_return, lv_color_make(0, 0, 0), LV_PART_MAIN);
    lv_obj_set_style_shadow_width(btn_return, 10, LV_PART_MAIN);
    lv_obj_set_style_shadow_opa(btn_return, LV_OPA_80, LV_PART_MAIN);
    lv_obj_align(btn_return, LV_ALIGN_TOP_LEFT, 0, -8);
    lv_obj_t *lab_btn_text = lv_label_create(btn_return);
    lv_label_set_text_static(lab_btn_text, LV_SYMBOL_LEFT);
    lv_obj_set_style_text_color(lab_btn_text, lv_color_make(0, 0, 0), LV_STATE_DEFAULT);
    lv_obj_center(lab_btn_text);
    lv_obj_add_event_cb(btn_return, ui_net_wifi_page_return_click_cb, LV_EVENT_CLICKED, g_page_wifi);    
}


static void ui_wifi_end(void)
{
    ESP_LOGI(TAG, "ui_wifi_end");

    ui_net_config_start(net_end_cb);
}

static void ui_net_page_func_click_cb(lv_event_t *e)
{
    uint32_t i = (uint32_t)lv_event_get_user_data(e);

    lv_obj_t *menu_btn_parent = lv_obj_get_parent(g_func_btn[i]);
    lv_obj_del(menu_btn_parent);
    net_item[i].start_fn(net_item[i].end_fn);
}

void ui_net_config_start(void (*fn)(void))
{
    ESP_LOGI(TAG, "net config start");
    g_net_config_end_cb = fn;

    lv_obj_t *g_page = lv_obj_create(lv_scr_act());
    lv_obj_set_size(g_page, lv_obj_get_width(lv_obj_get_parent(g_page)), lv_obj_get_height(lv_obj_get_parent(g_page)) - lv_obj_get_height(ui_main_get_status_bar()));
    lv_obj_set_style_border_width(g_page, 0, LV_PART_MAIN);
    lv_obj_set_style_bg_color(g_page, lv_obj_get_style_bg_color(lv_scr_act(), LV_STATE_DEFAULT), LV_PART_MAIN);
    lv_obj_clear_flag(g_page, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_align_to(g_page, ui_main_get_status_bar(), LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);        
    

    lv_obj_t *btn_return = lv_btn_create(g_page);
    lv_obj_set_size(btn_return, 40, 40);
    lv_obj_add_style(btn_return, &ui_button_styles()->style, 0);
    lv_obj_add_style(btn_return, &ui_button_styles()->style_pr, LV_STATE_PRESSED);
    lv_obj_add_style(btn_return, &ui_button_styles()->style_focus, LV_STATE_FOCUS_KEY);
    lv_obj_add_style(btn_return, &ui_button_styles()->style_focus, LV_STATE_FOCUSED);
    lv_obj_set_style_shadow_color(btn_return, lv_color_make(0, 0, 0), LV_PART_MAIN);
    lv_obj_set_style_shadow_width(btn_return, 10, LV_PART_MAIN);
    lv_obj_set_style_shadow_opa(btn_return, LV_OPA_80, LV_PART_MAIN);
    lv_obj_align(btn_return, LV_ALIGN_TOP_LEFT, 0, -8);
    lv_obj_t *lab_btn_text = lv_label_create(btn_return);
    lv_label_set_text_static(lab_btn_text, LV_SYMBOL_LEFT);
    lv_obj_set_style_text_color(lab_btn_text, lv_color_make(0, 0, 0), LV_STATE_DEFAULT);
    lv_obj_center(lab_btn_text);
    lv_obj_add_event_cb(btn_return, ui_net_config_page_return_click_cb, LV_EVENT_CLICKED, g_page);

    for (size_t i = 0; i < 3; i++)
    {
        g_func_btn[i] = lv_btn_create(g_page);
        lv_obj_set_size(g_func_btn[i], 85, 85);
        lv_obj_add_style(g_func_btn[i], &ui_button_styles()->style_focus, LV_STATE_FOCUS_KEY);
        lv_obj_add_style(g_func_btn[i], &ui_button_styles()->style_focus, LV_STATE_FOCUSED);
        lv_obj_set_style_bg_color(g_func_btn[i], lv_color_white(), LV_STATE_DEFAULT);
        lv_obj_set_style_bg_color(g_func_btn[i], lv_color_white(), LV_STATE_CHECKED);
        lv_obj_set_style_shadow_color(g_func_btn[i], lv_color_make(0, 0, 0), LV_PART_MAIN);
        lv_obj_set_style_shadow_width(g_func_btn[i], 10, LV_PART_MAIN);
        lv_obj_set_style_shadow_opa(g_func_btn[i], LV_OPA_40, LV_PART_MAIN);

        lv_obj_set_style_border_width(g_func_btn[i], 1, LV_PART_MAIN);
        lv_obj_set_style_border_color(g_func_btn[i], lv_palette_main(LV_PALETTE_GREY), LV_PART_MAIN);

        lv_obj_set_style_radius(g_func_btn[i], 10, LV_STATE_DEFAULT);
        lv_obj_align(g_func_btn[i], LV_ALIGN_CENTER, 100 * i - 100 , 0);

        lv_obj_t *img = lv_img_create(g_func_btn[i]);
        lv_img_set_src(img, img_src_list[i].img_on);
        lv_obj_align(img, LV_ALIGN_CENTER, 0, -10);
        lv_obj_set_user_data(img, (void *)&img_src_list[i]);

        lv_obj_t *label = lv_label_create(g_func_btn[i]);
        lv_label_set_text_static(label, img_src_list[i].name);
        lv_obj_set_style_text_color(label, lv_color_make(40, 40, 40), LV_STATE_DEFAULT);
        lv_obj_set_style_text_font(label, &font_en_16, LV_STATE_DEFAULT);
        lv_obj_align(label, LV_ALIGN_CENTER, 0, 20);

        lv_obj_set_user_data(g_func_btn[i], (void *)img);
        lv_obj_add_event_cb(g_func_btn[i], ui_net_page_func_click_cb, LV_EVENT_CLICKED, (void *)i);
    }
    

    // /* **************** APP NOT INSTALLED **************** */
    // g_btn_app_hint = lv_btn_create(ui_main_get_status_bar());
    // lv_obj_set_size(g_btn_app_hint, LV_SIZE_CONTENT, 25);
    // lv_obj_add_style(g_btn_app_hint, &ui_button_styles()->style, 0);
    // lv_obj_add_style(g_btn_app_hint, &ui_button_styles()->style_pr, LV_STATE_PRESSED);
    // lv_obj_add_style(g_btn_app_hint, &ui_button_styles()->style_focus, LV_STATE_FOCUS_KEY);
    // lv_obj_add_style(g_btn_app_hint, &ui_button_styles()->style_focus, LV_STATE_FOCUSED);
    // lv_obj_set_style_shadow_color(g_btn_app_hint, lv_color_make(0, 0, 0), LV_PART_MAIN);
    // lv_obj_set_style_shadow_width(g_btn_app_hint, 7, LV_PART_MAIN);
    // lv_obj_set_style_shadow_opa(g_btn_app_hint, LV_OPA_80, LV_PART_MAIN);
    // lv_obj_align(g_btn_app_hint, LV_ALIGN_RIGHT_MID, 0, 0);
    // lv_obj_t *lab_app_inst = lv_label_create(g_btn_app_hint);
    // lv_label_set_text_static(lab_app_inst, "Help");
    // lv_obj_set_style_text_color(lab_app_inst, lv_color_make(0, 0, 0), LV_STATE_DEFAULT);
    // lv_obj_align(lab_app_inst, LV_ALIGN_CENTER, 0, 0);
    // lv_obj_add_event_cb(g_btn_app_hint, ui_net_config_page_app_click_cb, LV_EVENT_CLICKED, NULL);

    // g_hint_lab = lv_label_create(g_page);
    // lv_label_set_recolor(g_hint_lab, true);
    // lv_label_set_text_static(g_hint_lab, "...");
    // lv_obj_align(g_hint_lab, LV_ALIGN_CENTER, 0, 0);
    // ui_net_config_update_cb(g_net_state, NULL);

}