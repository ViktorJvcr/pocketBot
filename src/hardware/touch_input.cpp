#include "touch_input.h"
#include "pins.h"

namespace TouchInput {
    void init() {
        // У TTP223 активный выход (Push-Pull), подтяжка не обязательна, но INPUT безопасен
        pinMode(TOUCH_BTN_1, INPUT);
        pinMode(TOUCH_BTN_2, INPUT);
        pinMode(TOUCH_BTN_3, INPUT);
    }

    bool isPressed(uint8_t btnIndex) {
        switch (btnIndex) {
            case 1: return digitalRead(TOUCH_BTN_1) == HIGH;
            case 2: return digitalRead(TOUCH_BTN_2) == HIGH;
            case 3: return digitalRead(TOUCH_BTN_3) == HIGH;
            default: return false;
        }
    }
}
