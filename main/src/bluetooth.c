#include "esp_log.h"
#include "nvs_flash.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"

static void nimble_host_task(void *param);
static esp_err_t initNVSflash(void);

/* ----- PUBLIC FUNCTIONS ----- */

BaseType_t initBluetooth(void){
    esp_err_t objectInitStatus = ESP_OK;

    /* NVS flash initialization */
    if(initNVSflash() != ESP_OK){
        ESP_LOGE("BLE_INIT", "Failed to initialize NVS Flash");
        return pdFAIL;
    }

    /* NimBLE host stack initialization */
    objectInitStatus = nimble_port_init();
    if (objectInitStatus != ESP_OK) {
        ESP_LOGE("NimBLE", "failed to initialize nimble stack, error code: %d ", objectInitStatus);
        return pdFAIL;
    }

    // Create Host Bluetooth freeRTOS task
    xTaskCreate(nimble_host_task, "ble_host", 4096, NULL, 5, NULL);

    return pdPASS;
}


/* ----- PRIVATE FUNCTIONS ----- */

static void nimble_host_task(void *param) {
    ESP_LOGI("BLE", "NimBLE Running...");

    /* Loops until nimble_port_stop() is executed */
    nimble_port_run();
    
    /* Clean up at exit */
    vTaskDelete(NULL);
}


static esp_err_t initNVSflash(void){
    esp_err_t nvsInitStatus = ESP_OK;

    /* NVS flash initialization */
    /* --> initialize a part of the flash memory for the bluetooth stack 
       --> if this init is not successfull, we erase the NVS part and we redo an init*/

    nvsInitStatus = nvs_flash_init();
    if (nvsInitStatus == ESP_ERR_NVS_NO_FREE_PAGES || nvsInitStatus == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        nvsInitStatus = nvs_flash_init();
    }

    // if the init still fails, we stop the program:
    if (nvsInitStatus != ESP_OK) {
        ESP_LOGE("NVS", "failed to initialize nvs flash, error code: %d ", nvsInitStatus);
    }

    return nvsInitStatus;
}