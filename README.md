# Weather Station

A connected and distributed weather station built using an **ESP32** and an **STM32F401**. 

The main purpose of this project is to demonstrate and learn embedded systems concepts:
- STM32 Low-Layer (LL) Libraries: peripheral configuration (UART, I2C, DMA, ADC, GPIO).
- ESP32 with FreeRTOS & esp-idf: Multitasking environment using the nimBLE stack for Bluetooth Low Energy (BLE).
- Inter-MCU Communication (UART).

## Data Flow

[Sensor and potentiometers reads via STM32] ---- UART ----> [ESP32] ---- BLE ----> [Smartphone]

## Features

- Sensors: Temperature and humidity reading via DHT22.
- Alarms: Local thresholds configured via potentiometers to trigger temperature/humidity alarms.
- Display: local LCD screen managed via I2C to display sensor informations.
- Wireless: Server running on FreeRTOS to broadcast data.

## Hardware

- Microcontrollers: - ESP32-WROOM-32D & STM32F401
- Sensors & Displays:
  - DHT22 Temperature/Humidity sensor
  - LCD Screen 2004A (with I2C communication)
  - MUX
  - Potentiometers
