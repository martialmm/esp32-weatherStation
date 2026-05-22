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

QueueHandle_t xQueue = NULL;
TaskHandle_t xTaskTemperatureHandle = NULL;
TaskHandle_t xTaskHumidityHandle = NULL;
 

void xTaskTemperatureCallback(TimerHandle_t xTimer){
    if(xTaskTemperatureHandle != NULL){
        xTaskNotifyGive(xTaskTemperatureHandle);
    }
}

void xTaskHumidityCallback(TimerHandle_t xTimer){
    if(xTaskHumidityHandle != NULL){
        xTaskNotifyGive(xTaskHumidityHandle);
    }
}

void nimble_host_task(void *param) {
    ESP_LOGI("BLE", "NimBLE Running...");

    /* Loops until nimble_port_stop() is executed */
    nimble_port_run();
    
    /* Clean up at exit */
    vTaskDelete(NULL);
}


void app_main(void)
{
    TimerHandle_t xTimerForTemperature = xTimerCreate("Timer temp sensor", pdMS_TO_TICKS(1000), pdTRUE, NULL, xTaskTemperatureCallback);
    TimerHandle_t xTimerForHumidity = xTimerCreate("Timer humidity sensor", pdMS_TO_TICKS(1000), pdTRUE, NULL, xTaskHumidityCallback);

    // BLUETOOTH variables
    esp_err_t objectInitStatus = ESP_OK;

    if (xTimerStart(xTimerForTemperature, pdMS_TO_TICKS(100)) != pdPASS) {
        ESP_LOGE("temperature sensor", "Failed to start timer");
        return;
    }

    if (xTimerStart(xTimerForHumidity, pdMS_TO_TICKS(100)) != pdPASS) {
        ESP_LOGE("humidity sensor", "Failed to start timer");
        return;
    }

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

    if(displayInitStatus != pdFAIL){
        xTaskCreate(vTaskSendTemperature, "Temp task", 2048, NULL, 1, &xTaskTemperatureHandle);
        xTaskCreate(vTaskSendHumidity, "Hum task", 2048, NULL, 1, &xTaskHumidityHandle);
        xTaskCreate(vTaskDisplayInfoOnScreen, "Screen display task", 4096, NULL, 2, NULL);
    }
}