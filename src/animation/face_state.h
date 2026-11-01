#pragma once
#include "../display/display.h"

namespace FaceState {
    struct State {
        float cx = 80.0f, cy = 38.0f, spacing = 48.0f;
        float lw = 25.0f, rw = 25.0f, lh = 40.0f, rh = 40.0f;
        float mw = 8.0f,  mh = 2.0f;

        float eyeR = 50.0f,  eyeG = 255.0f, eyeB = 100.0f;
        float gl1R = 10.0f,  gl1G = 120.0f, gl1B = 30.0f;
        float gl2R = 0.0f,   gl2G = 40.0f,  gl2B = 10.0f;

        float t_eyeR = 50.0f,  t_eyeG = 255.0f, t_eyeB = 100.0f;
        float t_gl1R = 10.0f,  t_gl1G = 120.0f, t_gl1B = 30.0f;
        float t_gl2R = 0.0f,   t_gl2G = 40.0f,  t_gl2B = 10.0f;

        uint16_t colorEye = 0, colorGlow1 = 0, colorGlow2 = 0;
    };

    extern State current;

    void init(Adafruit_ST7735 &tft);
    void updateColors(Adafruit_ST7735 &tft, float speed = 0.15f);
    void prepareTransition(bool blink);
    void setTargetPalette(float eR, float eG, float eB, float g1R, float g1G, float g1B, float g2R, float g2G, float g2B);
    void loadGeometry(float &cx, float &cy, float &spacing, float &lw, float &rw, float &lh, float &rh, float &mw, float &mh);
    void saveGeometry(float cx, float cy, float spacing, float lw, float rw, float lh, float rh, float mw, float mh);
}
