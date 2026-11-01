#pragma once
#include <Arduino.h>

// --- Шина I2C ---
constexpr uint8_t I2C_SDA_PIN = 8;
constexpr uint8_t I2C_SCL_PIN = 10;

// --- Экран ST7735 (80x160, в альбоме 160x80) ---
constexpr uint8_t TFT_BL_PIN  = 21;
constexpr int16_t SCREEN_W    = 160;
constexpr int16_t SCREEN_H    = 80;

// --- Сенсорные кнопки TTP223 (Активный HIGH) ---
constexpr uint8_t TOUCH_BTN_1 = 0;  // Например: Голова / Поглаживание
constexpr uint8_t TOUCH_BTN_2 = 1;  // Например: Левый бок
constexpr uint8_t TOUCH_BTN_3 = 20; // Например: Правый бок (Безопасный пин вместо GPIO 2)
