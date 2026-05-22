#include <stdio.h>
#include "esp_log.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h" 
#include "sensor.h"
#include "display.h"
#include "common.h"

// BLUETOOTH
#include "nvs_flash.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"


void nimble_host_task(void *param) {
    ESP_LOGI("BLE", "NimBLE Running...");

    /* Loops until nimble_port_stop() is executed */
    nimble_port_run();
    
    /* Clean up at exit */
    vTaskDelete(NULL);
}


void app_main(void)
{
    // BLUETOOTH variables
    esp_err_t objectInitStatus = ESP_OK;

    /* NVS flash initialization */
    /* --> initialize a part of the flash memory for the bluetooth stack 
       --> if this init is not successfull, we erase the NVS part and we redo an init*/
    objectInitStatus = nvs_flash_init();
    if (objectInitStatus == ESP_ERR_NVS_NO_FREE_PAGES || objectInitStatus == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        objectInitStatus = nvs_flash_init();
    }

    // if the init still fails, we stop the program:
    if (objectInitStatus != ESP_OK) {
        ESP_LOGE("NVS", "failed to initialize nvs flash, error code: %d ", objectInitStatus);
        return;
    }

    /* NimBLE host stack initialization */
    objectInitStatus = nimble_port_init();
    if (objectInitStatus != ESP_OK) {
        ESP_LOGE("NimBLE", "failed to initialize nimble stack, error code: %d ", objectInitStatus);
        return;
    }

    // Create Host Bluetooth freeRTOS task
    xTaskCreate(nimble_host_task, "ble_host", 4096, NULL, 5, NULL);

    BaseType_t displayInitStatus = initDisplay();
    BaseType_t sensorsInitStatus = initSensors();

    if(displayInitStatus != pdFAIL){
        ESP_LOGE("display", "Display Error!");
        return;
    }

    if(sensorsInitStatus != pdFAIL){
        ESP_LOGE("sensors", "Sensors Error!");
        return;
    }
}