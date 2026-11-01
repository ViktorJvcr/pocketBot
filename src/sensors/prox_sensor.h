#pragma once
#include <Arduino.h>

namespace ProxSensor {
    bool init();
    void update();
    uint16_t getProximity(); // 0..1023
    uint8_t getDeviceId();   // Должен вернуть 0x29 (или аналог)
    bool isConnected();
}
