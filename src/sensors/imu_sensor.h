#pragma once
#include <Arduino.h>

namespace ImuSensor {
    bool init();
    void update();
    bool isConnected();
    uint8_t getAddress();
    // Для теста выведем сырые G по осям
    float getAccX();
    float getAccY();
    float getAccZ();
}
