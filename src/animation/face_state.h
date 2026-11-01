#pragma once
#include <TFT_eSPI.h>

namespace FaceState {
    struct State {
        // Геометрия (масштаб под 160x80)
        float cx = 80.0f;
        float cy = 38.0f;
        float spacing = 48.0f;
        float lw = 25.0f, rw = 25.0f;
        float lh = 40.0f, rh = 40.0f;
        float mw = 8.0f,  mh = 2.0f;

        // Текущие плавающие RGB компоненты
        float eyeR = 50.0f,  eyeG = 255.0f, eyeB = 100.0f;
        float gl1R = 10.0f,  gl1G = 120.0f, gl1B = 30.0f;
        float gl2R = 0.0f,   gl2G = 40.0f,  gl2B = 10.0f;

        // Целевые RGB компоненты
        float t_eyeR = 50.0f,  t_eyeG = 255.0f, t_eyeB = 100.0f;
        float t_gl1R = 10.0f,  t_gl1G = 120.0f, t_gl1B = 30.0f;
        float t_gl2R = 0.0f,   t_gl2G = 40.0f,  t_gl2B = 10.0f;

        // Готовые 16-битные цвета для рендера
        uint16_t colorEye = 0;
        uint16_t colorGlow1 = 0;
        uint16_t colorGlow2 = 0;
    };

    extern State current;

    void init(TFT_eSPI &tft);
    void updateColors(TFT_eSPI &tft, float speed = 0.15f);
    void prepareTransition(bool blink);

    void setTargetPalette(float eR, float eG, float eB,
                          float g1R, float g1G, float g1B,
                          float g2R, float g2G, float g2B);

    void loadGeometry(float &cx, float &cy, float &spacing,
                      float &lw, float &rw, float &lh, float &rh,
                      float &mw, float &mh);

    void saveGeometry(float cx, float cy, float spacing,
                      float lw, float rw, float lh, float rh,
                      float mw, float mh);
}
