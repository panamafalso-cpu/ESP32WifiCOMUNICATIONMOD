#include <string.h>
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "esp_http_server.h"
#include "nvs_flash.h"

static const char *TAG = "safe_c5_ap";
static httpd_handle_t server = NULL;

static esp_err_t root_get(httpd_req_t *req)
{
    static const char page[] =
        "<!doctype html><html lang='en'><head><meta charset='utf-8'>"
        "<meta name='viewport' content='width=device-width,initial-scale=1'>"
        "<title>ESP32-C5 Wi-Fi 6 Lab</title>"
        "<style>body{font:16px system-ui;max-width:42rem;margin:3rem auto;padding:0 1rem;"
        "background:#101827;color:#e7eefb}main{padding:1.5rem;border:1px solid #34445d;"
        "border-radius:1rem}h1{font-size:1.5rem}code{color:#8de0c0}</style></head><body>"
        "<main><h1>ESP32-C5 Wi-Fi 6 Lab AP</h1>"
        "<p>Standalone access point and local status page.</p>"
        "<p>Configured protocol: <code>802.11b/g/n/ax (2.4 GHz)</code></p>"
        "<p>This demo does not capture traffic, transmit raw management frames, "
        "or collect credentials.</p></main></body></html>";
    httpd_resp_set_type(req, "text/html; charset=utf-8");
    httpd_resp_set_hdr(req, "Cache-Control", "no-store");
    return httpd_resp_send(req, page, HTTPD_RESP_USE_STRLEN);
}

static void start_web_server(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.max_uri_handlers = 4;
    config.lru_purge_enable = true;
    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_uri_t root = {
            .uri = "/",
            .method = HTTP_GET,
            .handler = root_get,
            .user_ctx = NULL
        };
        ESP_ERROR_CHECK(httpd_register_uri_handler(server, &root));
        ESP_LOGI(TAG, "Web page ready at http://192.168.4.1/");
    } else {
        ESP_LOGE(TAG, "Could not start HTTP server");
    }
}

void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t init_cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&init_cfg));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));

    wifi_config_t ap_cfg = { 0 };
    memcpy(ap_cfg.ap.ssid, "C5-WiFi6-Lab", sizeof("C5-WiFi6-Lab") - 1);
    memcpy(ap_cfg.ap.password, "c5labwifi6", sizeof("c5labwifi6") - 1);
    ap_cfg.ap.ssid_len = sizeof("C5-WiFi6-Lab") - 1;
    ap_cfg.ap.channel = 1;
    ap_cfg.ap.max_connection = 4;
    ap_cfg.ap.authmode = WIFI_AUTH_WPA2_PSK;
    ap_cfg.ap.pmf_cfg.capable = true;
    ap_cfg.ap.pmf_cfg.required = false;
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_cfg));

    wifi_protocols_t protocols = {
        .ghz_2g = WIFI_PROTOCOL_11B | WIFI_PROTOCOL_11G |
                  WIFI_PROTOCOL_11N | WIFI_PROTOCOL_11AX,
        .ghz_5g = WIFI_PROTOCOL_11A | WIFI_PROTOCOL_11N |
                  WIFI_PROTOCOL_11AC | WIFI_PROTOCOL_11AX
    };
    ESP_ERROR_CHECK(esp_wifi_set_protocols(WIFI_IF_AP, &protocols));
    ESP_ERROR_CHECK(esp_wifi_start());
    start_web_server();

    ESP_LOGI(TAG, "SSID: C5-WiFi6-Lab");
    ESP_LOGI(TAG, "Development password: c5labwifi6");
    ESP_LOGI(TAG, "Connect to the AP and open http://192.168.4.1/");
}
