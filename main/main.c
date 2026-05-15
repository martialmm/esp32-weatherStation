#include <stdio.h>
#include "esp_log.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h" 

QueueHandle_t xQueue;

typedef enum{
    TEMPERATURE,
    HUMIDITY
}DataType_t;

typedef struct {
    DataType_t type;
    int16_t value;
} SensorDatas_t;

void vTaskSendTemperature(void* pvParameters)
{
    SensorDatas_t temperatureData;
    temperatureData.type = TEMPERATURE;
    temperatureData.value = -50;

    for(;;){
        if(temperatureData.value >= 50) temperatureData.value = -50;
        temperatureData.value++;
        BaseType_t xStatus = xQueueSend(xQueue, &temperatureData, pdMS_TO_TICKS(100));
        if(xStatus != pdPASS){
            ESP_LOGI("error", "temperature value could not be sent!");
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void vTaskSendHumidity(void* pvParameters)
{

    SensorDatas_t humidityData;
    humidityData.type = HUMIDITY;
    humidityData.value = 0;

    for(;;){
        if(humidityData.value >= 100) humidityData.value = 0;
        humidityData.value++;
        BaseType_t xStatus = xQueueSend(xQueue, &humidityData, pdMS_TO_TICKS(100));
        if(xStatus != pdPASS){
            ESP_LOGI("error", "humidity value could not be sent!");
        }
        vTaskDelay(pdMS_TO_TICKS(600));
    }
}

void vTaskDisplayInfoOnScreen(void* pvParameters)
{
    SensorDatas_t receivedDatasFromSensor;

    for (;;){
        UBaseType_t xStatus = xQueueReceive(xQueue, &receivedDatasFromSensor, pdMS_TO_TICKS(200));
        if(xStatus == pdPASS){
            if(receivedDatasFromSensor.type == HUMIDITY){
                ESP_LOGI("screen", "humidity sensor value: %d", receivedDatasFromSensor.value);
            }
            else if(receivedDatasFromSensor.type == TEMPERATURE){
                ESP_LOGI("screen", "temperature sensor value: %d", receivedDatasFromSensor.value);
            }
            else{
                ESP_LOGI("screen", "data error");
            }
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void app_main(void)
{
    xQueue = xQueueCreate(20, sizeof(SensorDatas_t));
    if(xQueue != NULL){
        xTaskCreate(vTaskSendTemperature, "Temp task", 4096, NULL, 1, NULL);
        xTaskCreate(vTaskSendHumidity, "Hum task", 4096, NULL, 1, NULL);
        xTaskCreate(vTaskDisplayInfoOnScreen, "Screen display task", 4096, NULL, 2, NULL);
    }
    else{}
}