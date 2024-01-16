/*
 * SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>

#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_log.h>
// #include "esp_bt.h"
#include <esp_netif.h>
#include <qrcode.h>
#include <nvs.h>
#include <nvs_flash.h>

#include <wifi_provisioning/manager.h>
#include <wifi_provisioning/scheme_ble.h>

#include "app_wifi.h"
// #include "app_sntp.h"
#include "ui_main.h"
#include "ui_net_config.h"

// static const int WIFI_STA_CONNECT_OK    = BIT0;
// static const int WIFI_PROV_EVENT_START  = BIT1;
static const int WIFI_PROV_EVENT_STOP   = BIT2;
// static const int WIFI_PROV_EVENT_EXIST  = BIT3;
static const int WIFI_PROV_EVENT_STOPED = BIT4;

static bool s_connected = false;
static char s_payload[150] = "";
static const char *TAG = "app_wifi";
static EventGroupHandle_t wifi_event_group;

#define PROV_QR_VERSION         "v1"
#define PROV_TRANSPORT_BLE      "ble"
#define QRCODE_BASE_URL         "https://rainmaker.espressif.com/qrcode.html"

#define CREDENTIALS_NAMESPACE   "rmaker_creds"
#define RANDOM_NVS_KEY          "random"


#if CONFIG_BSP_BOARD_ESP32_S3_BOX_3
/*set the ssid and password via "idf.py menuconfig"*/
#define DEFAULT_LISTEN_INTERVAL CONFIG_EXAMPLE_WIFI_LISTEN_INTERVAL
#define DEFAULT_BEACON_TIMEOUT  CONFIG_EXAMPLE_WIFI_BEACON_TIMEOUT
#endif

#if CONFIG_EXAMPLE_POWER_SAVE_MIN_MODEM
#define DEFAULT_PS_MODE WIFI_PS_MIN_MODEM
#elif CONFIG_EXAMPLE_POWER_SAVE_MAX_MODEM
#define DEFAULT_PS_MODE WIFI_PS_MAX_MODEM
#elif CONFIG_EXAMPLE_POWER_SAVE_NONE
#define DEFAULT_PS_MODE WIFI_PS_NONE
#else
#define DEFAULT_PS_MODE WIFI_PS_NONE
#endif /*CONFIG_POWER_SAVE_MODEM*/


char *app_wifi_get_prov_payload(void)
{
    return s_payload;
}

esp_err_t app_wifi_prov_start(void)
{
    bool provisioned = false;
    ESP_ERROR_CHECK(wifi_prov_mgr_is_provisioned(&provisioned));
    // if (!provisioned) {
    //     ESP_LOGI(TAG, "app_wifi_prov_start");
    //     return xEventGroupSetBits(wifi_event_group, WIFI_PROV_EVENT_START);
    // } else {
    //     return ESP_FAIL;
    // }
    return ESP_OK;
}

esp_err_t app_wifi_prov_stop(void)
{
    bool provisioned = false;
    ESP_ERROR_CHECK(wifi_prov_mgr_is_provisioned(&provisioned));
    if (!provisioned) {
        ESP_LOGI(TAG, "app_wifi_prov_stop");
        xEventGroupSetBits(wifi_event_group, WIFI_PROV_EVENT_STOP);
        xEventGroupClearBits(wifi_event_group, WIFI_PROV_EVENT_STOPED);
        xEventGroupWaitBits(wifi_event_group, WIFI_PROV_EVENT_STOPED, pdFALSE, pdFALSE, pdMS_TO_TICKS(1000));
        return ESP_OK;
    } else {
        return ESP_FAIL;
    }
}


esp_err_t app_wifi_get_wifi_ssid(char *ssid, size_t len)
{
    wifi_config_t wifi_cfg;
    if (esp_wifi_get_config(WIFI_IF_STA, &wifi_cfg) != ESP_OK) {
        return ESP_FAIL;
    }
    strncpy(ssid, (const char *)wifi_cfg.sta.ssid, len);
    return ESP_OK;
}


