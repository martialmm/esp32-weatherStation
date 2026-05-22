#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "sensor.h"
#include "display.h"
#include "common.h"
#include "bluetooth.h"


void app_main(void)
{
    BaseType_t bluetoothInitStatus = initBluetooth();
    BaseType_t displayInitStatus = initDisplay();
    BaseType_t sensorsInitStatus = initSensors();

    if(bluetoothInitStatus != pdPASS){
        ESP_LOGE("Bluetooth", "Bluetooth Error!");
        return;
    }

    if(displayInitStatus != pdPASS){
        ESP_LOGE("display", "Display Error!");
        return;
    }

    if(sensorsInitStatus != pdPASS){
        ESP_LOGE("sensors", "Sensors Error!");
        return;
    }
}