#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// Pin map (must match wiring exactly)
static const uint8_t PIN_DHT11_DATA = 2;   // D2
static const uint8_t PIN_MQ2_DOUT   = 3;   // D3 (active LOW)
static const uint8_t PIN_BUZZER     = 4;   // D4
static const uint8_t PIN_MQ2_AOUT   = A0;  // A0

// OLED (SSD1306 I2C)
static const uint8_t OLED_I2C_ADDR_7BIT = 0x3C;

// Timing (non-blocking)
static const unsigned long DHT_READ_INTERVAL_MS = 2000;
static const unsigned long MQ_READ_INTERVAL_MS  = 250;
static const unsigned long UI_UPDATE_INTERVAL_MS = 500;

// MQ-2 calibration endpoints for Arduino Uno (10-bit ADC)
static const int MQ2_CLEAN_AIR_RAW = 61;   // 0%
static const int MQ2_SMOKE_RAW     = 921;  // 100%

// Alarm threshold
static const int GAS_PERCENT_ALARM_THRESHOLD = 40;

#endif
