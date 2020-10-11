#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include "esp_log.h"
#include "WiFi.h"
#include "flipdot.h"
#include "fill.h"
#include "main.h"
#include "scroll.h"
#include "text.h"
#include <WiFiUdp.h>
#include "NTP.h"
#include <ESPmDNS.h>
#include <ArduinoOTA.h>

static const char *TAG = "Main";

const char *ssid = "network";
const char *password = "password";
const char *host = "flipdot";

WiFiUDP ntpUDP;
NTP ntp(ntpUDP);

dotboard_t clean_board;

void setup()
{
    ESP_LOGI(TAG, "Flipdot Display Controller System startup");

    // Initialise display
    flipdot_init();

    // Init wifi

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        vTaskDelay(500 / portTICK_RATE_MS);
    }

    if (!MDNS.begin(host))
    { //http://esp32.local
        ESP_LOGE("Error setting up MDNS responder!");
        while (1)
        {
            vTaskDelay(1000 / portTICK_RATE_MS);
        }
    }

    // Set up summer time rules
    ntp.ruleDST("BST", Last, Sun, Mar, 1, 60);
    ntp.ruleSTD("GMT", Last, Sun, Oct, 2, 0);
    ntp.begin();
}

void loop()
{
    ntp.update();
    fill_off(&clean_board);
    render_text_4x5(&clean_board, 0, 1, ntp.formattedTime("%a %d %b %H:%M"));
    write_dotboard(&clean_board, false);

    vTaskDelay(50 / portTICK_RATE_MS);
}
