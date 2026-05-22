#include "common.h"
#include "display.h"
#include "esp_log.h"

#define DISPLAY_QUEUE_SIZE 20

static QueueHandle_t xQueue = NULL;

BaseType_t initDisplay(void){
    xQueue = xQueueCreate(DISPLAY_QUEUE_SIZE, sizeof(SensorDatas_t));
    return (xQueue != NULL) ?  pdPASS :  pdFAIL;
}

BaseType_t sendDataToDisplay(const void* pvItemToQueue, TickType_t xTicksToWait){
    BaseType_t xStatus = xQueueSend(xQueue, pvItemToQueue, pdMS_TO_TICKS(xTicksToWait));
    return xStatus;
}

void vTaskDisplayInfoOnScreen(void* pvParameters)
{
    SensorDatas_t receivedDatasFromSensor;

    for (;;){
        UBaseType_t xStatus = xQueueReceive(xQueue, &receivedDatasFromSensor, pdMS_TO_TICKS(500));
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
    }
}