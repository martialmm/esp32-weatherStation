#ifndef SENSOR_H
#define SENSOR_H

BaseType_t initSensors(void);
void vTaskSendTemperature(void* pvParameters);
void vTaskSendHumidity(void* pvParameters);


#endif // SENSOR_H