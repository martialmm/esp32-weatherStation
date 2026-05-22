#include "freertos/FreeRTOS.h"
#include "sensor.h"
#include "display.h"
#include "common.h"
#include "esp_log.h"

static void vTaskSendTemperature(void* pvParameters);
static void vTaskSendHumidity(void* pvParameters);
static void xTaskTemperatureCallback(TimerHandle_t xTimer);
static void xTaskHumidityCallback(TimerHandle_t xTimer);

static TaskHandle_t xTaskTemperatureHandle = NULL;
static TaskHandle_t xTaskHumidityHandle = NULL;

BaseType_t initSensors(void){

    TimerHandle_t xTimerForTemperature = xTimerCreate("Timer temp sensor", pdMS_TO_TICKS(1000), pdTRUE, NULL, xTaskTemperatureCallback);
    TimerHandle_t xTimerForHumidity = xTimerCreate("Timer humidity sensor", pdMS_TO_TICKS(1000), pdTRUE, NULL, xTaskHumidityCallback);

    if (xTimerStart(xTimerForTemperature, pdMS_TO_TICKS(100)) != pdPASS) {
        ESP_LOGE("temperature sensor", "Failed to start timer");
        return pdFAIL;
    }

    if (xTimerStart(xTimerForHumidity, pdMS_TO_TICKS(100)) != pdPASS) {
        ESP_LOGE("humidity sensor", "Failed to start timer");
        return pdFAIL;
    }

    if( (xTaskCreate(vTaskSendTemperature, "Temp task", 2048, NULL, 1, &xTaskTemperatureHandle) ) != pdPASS){
        ESP_LOGE("temperature sensor", "Failed to start task");
        return pdFAIL;
    }

    if( (xTaskCreate(vTaskSendHumidity, "Hum task", 2048, NULL, 1, &xTaskHumidityHandle) ) != pdPASS){
        ESP_LOGE("humidity sensor", "Failed to start task");
        return pdFAIL;
    }
    return pdPASS;
}

static void xTaskTemperatureCallback(TimerHandle_t xTimer){
    if(xTaskTemperatureHandle != NULL){
        xTaskNotifyGive(xTaskTemperatureHandle);
    }
}

static void xTaskHumidityCallback(TimerHandle_t xTimer){
    if(xTaskHumidityHandle != NULL){
        xTaskNotifyGive(xTaskHumidityHandle);
    }
}

static void vTaskSendTemperature(void* pvParameters)
{
    SensorDatas_t temperatureData;
    temperatureData.type = TEMPERATURE;
    temperatureData.value = -50;

    for(;;){
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if(temperatureData.value >= 50) temperatureData.value = -50;
        temperatureData.value++;
        BaseType_t xStatus = sendDataToDisplay(&temperatureData, 100);
        if(xStatus != pdPASS){
            ESP_LOGE("error", "temperature value could not be sent!");
        }
    }
}


static void vTaskSendHumidity(void* pvParameters)
{
    SensorDatas_t humidityData;
    humidityData.type = HUMIDITY;
    humidityData.value = 0;

    for(;;){
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if(humidityData.value >= 100) humidityData.value = 0;
        humidityData.value++;
        BaseType_t xStatus = sendDataToDisplay(&humidityData, 100);
        if(xStatus != pdPASS){
            ESP_LOGE("error", "humidity value could not be sent!");
        }
    }
}