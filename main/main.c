#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "sensor.h"
#include "display.h"
#include "bluetooth.h"


void app_main(void)
{
    if(initBluetooth() != pdPASS){
        ESP_LOGE("Bluetooth", "Bluetooth Error!");
        return;
    }

    if(initDisplay() != pdPASS){
        ESP_LOGE("display", "Display Error!");
        return;
    }

    if(initSensors() != pdPASS){
        ESP_LOGE("sensors", "Sensors Error!");
        return;
    }
}