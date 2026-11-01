#pragma once
#include <Arduino.h>

// Структура палитры (RGB 0..255 для плавного лерпа, плюс готовые RGB565 для рендера)
struct FacePalette {
    float rEye, gEye, bEye;
    float rGlow1, gGlow1, bGlow1;
    float rGlow2, gGlow2, bGlow2;

    uint16_t colorEye;
    uint16_t colorGlow1;
    uint16_t colorGlow2;
    uint16_t colorBg;
};

class FaceState {
public:
    // Глобальная текущая и целевая палитры
    static FacePalette current;
    static FacePalette target;

    // Сохраненная геометрия лица (масштаб 160x80: центр 80, 40)
    static float cx, cy;
    static float spacing;
    static float lw, lh, lr;
    static float rw, rh, rr;
    static float mw, mh;

    static void init();

    // Установка целевых цветов (RGB 0..255) для 3-Layer Glow
    static void setTargetPalette(
        uint8_t rE, uint8_t gE, uint8_t bE,
        uint8_t rG1, uint8_t gG1, uint8_t bG1,
        uint8_t rG2, uint8_t gG2, uint8_t bG2
    );

    // Плавный переход цветов (вызывается каждый кадр в AnimManager)
    static void updateColorLerp(float speed = 0.12f);

    // Подготовка к смене эмоции (squint = true делает рефлекторный прищур до 7px)
    static void prepareTransition(bool squint = true);

    // Загрузка и сохранение геометрии между эмоциями
    static void loadGeometry(float &outCx, float &outCy, float &outSp,
                             float &outLw, float &outLh, float &outLr,
                             float &outRw, float &outRh, float &outRr,
                             float &outMw, float &outMh);

    static void saveGeometry(float inCx, float inCy, float inSp,
                             float inLw, float inLh, float inLr,
                             float inRw, float inRh, float inRr,
                             float inMw, float inMh);

    // Утилита перевода RGB888 -> RGB565
    static uint16_t rgb565(uint8_t r, uint8_t g, uint8_t b);
};
