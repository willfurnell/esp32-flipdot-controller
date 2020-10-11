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

static const char* TAG = "Main";

// Mode state

const char *ssid     = "network";
const char *password = "password";
const char* host = "flipdot";

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

    while ( WiFi.status() != WL_CONNECTED ) {
        vTaskDelay(500 / portTICK_RATE_MS);
    }

    if (!MDNS.begin(host)) { //http://esp32.local
        ESP_LOGE("Error setting up MDNS responder!");
        while (1) {
            vTaskDelay(1000 / portTICK_RATE_MS);
        }
    }

    // Set up summer time rules
    ntp.ruleDST("BST", Last, Sun, Mar, 1, 60); 
    ntp.ruleSTD("GMT", Last, Sun, Oct, 2, 0); 
    ntp.begin();

    ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      ESP_LOGI("Start updating " + type);
    })
    .onEnd([]() {
      ESP_LOGI("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) ESP_LOGE("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) ESP_LOGE("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) ESP_LOGE("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) ESP_LOGE("Receive Failed");
      else if (error == OTA_END_ERROR) ESP_LOGE("End Failed");
    });

ArduinoOTA.begin();

}

void loop() {
    ArduinoOTA.handle();
    ntp.update();
    fill_off(&clean_board);
    render_text_4x5(&clean_board, 0, 1, ntp.formattedTime("%a %d %b %H:%M"));
    write_dotboard(&clean_board, false);

    vTaskDelay(50 / portTICK_RATE_MS);
}
