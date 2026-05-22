#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>

typedef enum{
    TEMPERATURE,
    HUMIDITY
}DataType_t;

typedef struct {
    DataType_t type;
    int16_t value;
} SensorDatas_t;

#endif