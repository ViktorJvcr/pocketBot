#pragma once
#include <Arduino.h>

namespace ImuSensor {
    bool init();
    void update();
    bool isConnected();
    uint8_t getAddress();

    // Сырые ускорения (в единицах g) и угловые скорости (град/с)
    float getAccX();
    float getAccY();
    float getAccZ();
    float getGyrX();
    float getGyrY();
    float getGyrZ();

    // Смещение взгляда для эффекта живого параллакса (масштаб 160x80)
    float getParallaxX();
    float getParallaxY();

    // Детекторы физических воздействий (сбрасывают флаг при чтении)
    bool isSuddenLiftDetected();
    bool isShakeDetected();
}
