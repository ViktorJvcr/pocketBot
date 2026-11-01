#pragma once
#include <Arduino.h>

namespace TouchInput {
    void init();
    bool isPressed(uint8_t btnIndex); // 1, 2 или 3
}
