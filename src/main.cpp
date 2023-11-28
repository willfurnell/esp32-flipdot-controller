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
#include "graphics.h"

static const char *TAG = "Main";

const char *ssid = "NoConnection";
const char *password = "";
const char *host = "flipdot";

WiFiUDP ntpUDP;
NTP ntp(ntpUDP);

dotboard_t dots;

void setup()
{
    ESP_LOGI(TAG, "Flipdot Display Controller System startup");

    // Initialise display
    flipdot_init();

    fill_on(&dots);
    write_dotboard(&dots, true);
    fill_off(&dots);
    write_dotboard(&dots, true);
    render_text_4x5(&dots, 0, 1, "Connecting...");
    write_dotboard(&dots, true);

    // Init wifi

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        vTaskDelay(500 / portTICK_RATE_MS);
    }

    if (!MDNS.begin(host))
    {
        ESP_LOGE("Error setting up MDNS responder!");
        while (1)
        {
            vTaskDelay(1000 / portTICK_RATE_MS);
        }
    }
    fill_off(&dots);
    render_text_4x5(&dots, 0, 1, WiFi.localIP().toString().c_str());
    write_dotboard(&dots, true);
    vTaskDelay(2000 / portTICK_RATE_MS);

    //fill_off(&dots);
    //write_dotboard(&dots, true);

    // Set up summer time rules
    ntp.ruleDST("BST", Last, Sun, Mar, 1, 60);
    ntp.ruleSTD("GMT", Last, Sun, Oct, 2, 0);
    ntp.begin();

    fill_off(&dots);
    render_text_4x5(&dots, 0, 1, ntp.formattedTime("%a %d %b %H:%M"));
    write_dotboard(&dots, true);
}

void loop()
{
    ntp.update();
    //fill_on(&dots);
    //fill_off(&dots);
    //render_text_4x5(&dots, 0, 1, "PARTAYYYYYYYYY");
    //write_dotboard(&dots, false);
    //vTaskDelay(10000 / portTICK_RATE_MS);
    //fill_off(&dots);
    //render_text_4x5(&dots, 0, 1, "HAPPY BIRTHDAY!");
    //write_dotboard(&dots, false);
    //vTaskDelay(10000 / portTICK_RATE_MS);
    //fill_off(&dots);
    //render_text_4x5(&dots, 0, 1, "LAURA AND MEGAN");
    //write_dotboard(&dots, false);
    //vTaskDelay(10000 / portTICK_RATE_MS);
    fill_off(&dots);
    render_text_4x5(&dots, 0, 1, ntp.formattedTime("%a %d %b %H:%M"));
    write_dotboard(&dots, false);
    //scroll_update();
    vTaskDelay(20000 / portTICK_RATE_MS);
    fill_off(&dots);
    render_cyberman(&dots, 0);
    render_cyberman(&dots, 9);
    render_cyberman(&dots, 18);
    render_cyberman(&dots, 27);
    render_cyberman(&dots, 36);
    render_cyberman(&dots, 45);
    render_cyberman(&dots, 54);
    render_cyberman(&dots, 63);
    render_cyberman(&dots, 72);
    render_cyberman(&dots, 72);
    write_dotboard(&dots, false);
    vTaskDelay(20000 / portTICK_RATE_MS);
    //render_text_4x5(&dots, 0, 1, "HAPPY BIRTHDAY!");
    //write_dotboard(&dots, false);
    //vTaskDelay(10000 / portTICK_RATE_MS);
    //fill_off(&dots);
    //render_text_4x5(&dots, 0, 1, "MEGAN AND LAURA");
    //write_dotboard(&dots, false);
    //vTaskDelay(10000 / portTICK_RATE_MS);
    //fill_off(&dots);
    //render_text_4x5(&dots, 0, 1, ntp.formattedTime("%a %d %b %H:%M"));
    //write_dotboard(&dots, false);
    //scroll_update();
}
