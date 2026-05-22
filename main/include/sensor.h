#ifndef SENSOR_H
#define SENSOR_H

void vTaskSendTemperature(void* pvParameters);
void vTaskSendHumidity(void* pvParameters);


#endif // SENSOR_H