#include "esp_log.h"
#include "nvs_flash.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "services/gap/ble_svc_gap.h"

static void nimble_host_task(void *param);
static esp_err_t initNVSflash(void);
static void on_stack_sync(void);

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

    /* GAP Initialization */
    ble_svc_gap_init();
    ble_svc_gap_device_name_set("ESP32");

    ble_hs_cfg.sync_cb = on_stack_sync;

    // Create Host Bluetooth freeRTOS task
    xTaskCreate(nimble_host_task, "ble_host", 4096, NULL, 5, NULL);

    return pdPASS;
}


/* ----- PRIVATE FUNCTIONS ----- */

static void on_stack_sync(void) {
    // the informations the scanner device will see
    struct ble_hs_adv_fields publicAdvertisingFields = {0};
    // fields used to set the actual peripheral paramaters
    struct ble_gap_adv_params peripheralAdvertisingParameters = {0};

    /* GAP Advertising Config */
    const char* deviceName = ble_svc_gap_device_name();
    publicAdvertisingFields.name = (uint8_t *) deviceName;
    publicAdvertisingFields.name_len = strlen(deviceName);
    publicAdvertisingFields.name_is_complete = 1;

    /* Set advertising flags */
    // BLE_HS_ADV_F_DISC_GEN = everyone can see the peripheral
    // BLE_HS_ADV_F_BREDR_UNSUP = BR/EDR bluetooth not supported (so BLE is used)
    publicAdvertisingFields.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;

    /* Set device LE role */
    publicAdvertisingFields.le_role = 0x00; // = PERIPHERAL
    publicAdvertisingFields.le_role_is_present = 1;

    /* Set advertisement fields */
    ble_gap_adv_set_fields(&publicAdvertisingFields);

    /* Set non-connectable and general discoverable mode to be a beacon */
    peripheralAdvertisingParameters.conn_mode = BLE_GAP_CONN_MODE_NON;
    peripheralAdvertisingParameters.disc_mode = BLE_GAP_DISC_MODE_GEN;

    /* GAP Start advertising */
    ble_gap_adv_start(BLE_OWN_ADDR_PUBLIC, NULL, BLE_HS_FOREVER, &peripheralAdvertisingParameters, NULL, NULL);
}

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