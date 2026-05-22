#ifndef DISPLAY_H
#define DISPLAY_H

#include "freertos/FreeRTOS.h"

BaseType_t initDisplay(void);
BaseType_t sendDataToDisplay(const void* pvItemToQueue, TickType_t xTicksToWait);

#endif // DISPLAY_H