#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h" 

static const gpio_num_t green_led = GPIO_NUM_26;
static const gpio_num_t red_led = GPIO_NUM_12;
static const gpio_num_t push_button = GPIO_NUM_19;
static const uint32_t sleep_time_ms = 1000;

void greenLedFlashingTask(void *pvParameters)
{
    static uint8_t green_led_state = 0;;

    for( ;; )
    {
        green_led_state = !green_led_state;
        gpio_set_level(green_led, green_led_state);
        vTaskDelay(pdMS_TO_TICKS(sleep_time_ms));
    }
}

void redLedFlashingTask(void *pvParameters)
{
    for( ;; )
    {
        gpio_set_level(red_led, gpio_get_level(push_button));
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void app_main(void)
{
    gpio_config_t push_button_config = {
        .pin_bit_mask = (1UL << 19),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    gpio_config(&push_button_config);

    gpio_reset_pin(green_led);
    gpio_set_direction(green_led, GPIO_MODE_OUTPUT);

    gpio_reset_pin(red_led);
    gpio_set_direction(red_led, GPIO_MODE_OUTPUT);

    xTaskCreate(greenLedFlashingTask, "green Led", 2048, NULL, 1, NULL);
    xTaskCreate(redLedFlashingTask, "red Led", 2048, NULL, 2, NULL);
}
