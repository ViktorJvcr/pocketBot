#pragma once
#include <Arduino.h>
#include "../display/display.h"

class AnimIdle {
public:
    void reset();
    void runStep(PocketSprite& sprite, float parX = 0.0f, float parY = 0.0f);

private:
    // Текущая сглаженная геометрия
    float cx, cy, currentSpacing;
    float lw, lh, lr;
    float rw, rh, rr;
    float mw, mh;

    // Целевые значения для пружинной физики
    float targetX, targetY, targetSpacing;
    float targetLW, targetLH, targetLR;
    float targetRW, targetRH, targetRR;
    float targetMW, targetMH;

    // Таймеры поведения (на millis())
    unsigned long lastSaccadeTime = 0;
    unsigned long nextSaccadeInterval = 2500;

    unsigned long lastBlinkTime = 0;
    unsigned long nextBlinkInterval = 3200;
    bool isBlinking = false;
    bool pendingDoubleBlink = false;
    unsigned long blinkStartTime = 0;

    // Вспомогательный метод рендера скругленного прямоугольника с 3-Layer Glow
    void drawGlowRoundRect(PocketSprite& sprite, int16_t x, int16_t y, int16_t w, int16_t h, int16_t r,
                           uint16_t cEye, uint16_t cG1, uint16_t cG2);
};
