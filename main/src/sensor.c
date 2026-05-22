#include "freertos/FreeRTOS.h"
#include "sensor.h"
#include "display.h"
#include "common.h"
#include "esp_log.h"

void vTaskSendTemperature(void* pvParameters)
{
    SensorDatas_t temperatureData;
    temperatureData.type = TEMPERATURE;
    temperatureData.value = -50;

    for(;;){
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if(temperatureData.value >= 50) temperatureData.value = -50;
        temperatureData.value++;
        BaseType_t xStatus = sendDataToDisplay(&temperatureData, 100); // BaseType_t xStatus = xQueueSend(xQueue, &temperatureData, pdMS_TO_TICKS(100));
        if(xStatus != pdPASS){
            ESP_LOGI("error", "temperature value could not be sent!");
        }
    }
}


void vTaskSendHumidity(void* pvParameters)
{

    SensorDatas_t humidityData;
    humidityData.type = HUMIDITY;
    humidityData.value = 0;

    for(;;){
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if(humidityData.value >= 100) humidityData.value = 0;
        humidityData.value++;
        BaseType_t xStatus = sendDataToDisplay(&humidityData, 100); //BaseType_t xStatus = xQueueSend(xQueue, &humidityData, pdMS_TO_TICKS(100));
        if(xStatus != pdPASS){
            ESP_LOGI("error", "humidity value could not be sent!");
        }
    }
}