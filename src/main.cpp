#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

#include "config.h"
#include "pins.h"
#include "display/display.h"
#include "sensors/prox_sensor.h"
#include "sensors/imu_sensor.h"
#include "hardware/touch_input.h"
#include "animation/face_state.h"
#include "animation/anim_idle.h"

// ВАЖНО: Передаем только 3 пина (CS, DC, RST) — это включает АППАРАТНЫЙ SPI!
Adafruit_ST7735 tft = Adafruit_ST7735(&SPI, TFT_CS_PIN, TFT_DC_PIN, TFT_RST_PIN);
PocketSprite sprite(160, 80);

// Экземпляр базовой анимации покоя
AnimIdle animIdle;

// Таймер кадров (50 FPS = 20 мс)
unsigned long lastFrameTime = 0;

void setup() {
    Serial.begin(115200);

    // 1. Запускаем АППАРАТНЫЙ SPI на наших пинах (SCLK=6, MISO=-1, MOSI=7, CS=5)
    SPI.begin(TFT_SCLK_PIN, -1, TFT_MOSI_PIN, TFT_CS_PIN);

    // 2. Инициализация общей шины I2C (SDA = 8, SCL = 10, 400 кГц)
    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
    Wire.setClock(400000);

    // 3. Инициализация экрана 0.96" ST7735 (160x80) и буфера PocketSprite
    initDisplay(tft, sprite);
    
    // Разгоняем аппаратный SPI дисплея до 40 МГц (кадр 160x80 улетает за ~5 мс!)
    tft.setSPISpeed(40000000);

    // 4. Инициализация генератора случайных чисел для живых саккад и морганий
    randomSeed(micros());

    // 5. Инициализация базовой геометрии лица (160x80) и запуск AnimIdle
    FaceState::init();
    animIdle.reset();

    Serial.println("[PocketRobot] Hardware SPI 40MHz & AnimIdle initialized!");
}

void loop() {
    unsigned long now = millis();

    // Рендер-цикл анимации: строго на millis(), ~50 Гц (каждые 20 мс)
    if (now - lastFrameTime >= 20) {
        lastFrameTime = now;

        // Плавная интерполяция палитры (Color Lerp)
        FaceState::updateColorLerp();

        // Расчет пружинной физики, дыхания, моргания и отрисовка в буфер
        animIdle.runStep(sprite, 0.0f, 0.0f);

        // Вывод готового кадра 160x80 из ОЗУ на экран без мерцания
        pushSpriteToDisplay(tft, sprite);
    }
}
