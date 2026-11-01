#include "face_state.h"

namespace FaceState {
    State current;

    void init(TFT_eSPI &tft) {
        current.colorEye   = tft.color565((uint8_t)current.eyeR, (uint8_t)current.eyeG, (uint8_t)current.eyeB);
        current.colorGlow1 = tft.color565((uint8_t)current.gl1R, (uint8_t)current.gl1G, (uint8_t)current.gl1B);
        current.colorGlow2 = tft.color565((uint8_t)current.gl2R, (uint8_t)current.gl2G, (uint8_t)current.gl2B);
    }

    void updateColors(TFT_eSPI &tft, float speed) {
        current.eyeR += (current.t_eyeR - current.eyeR) * speed;
        current.eyeG += (current.t_eyeG - current.eyeG) * speed;
        current.eyeB += (current.t_eyeB - current.eyeB) * speed;

        current.gl1R += (current.t_gl1R - current.gl1R) * speed;
        current.gl1G += (current.t_gl1G - current.gl1G) * speed;
        current.gl1B += (current.t_gl1B - current.gl1B) * speed;

        current.gl2R += (current.t_gl2R - current.gl2R) * speed;
        current.gl2G += (current.t_gl2G - current.gl2G) * speed;
        current.gl2B += (current.t_gl2B - current.gl2B) * speed;

        current.colorEye   = tft.color565((uint8_t)current.eyeR, (uint8_t)current.eyeG, (uint8_t)current.eyeB);
        current.colorGlow1 = tft.color565((uint8_t)current.gl1R, (uint8_t)current.gl1G, (uint8_t)current.gl1B);
        current.colorGlow2 = tft.color565((uint8_t)current.gl2R, (uint8_t)current.gl2G, (uint8_t)current.gl2B);
    }

    void prepareTransition(bool blink) {
        if (blink) {
            // Рефлекторный прищур для экрана 160x80 (вдвое меньше 14.0f)
            current.lh = 7.0f;
            current.rh = 7.0f;
        }
    }

    void setTargetPalette(float eR, float eG, float eB,
                          float g1R, float g1G, float g1B,
                          float g2R, float g2G, float g2B) {
        current.t_eyeR = eR; current.t_eyeG = eG; current.t_eyeB = eB;
        current.t_gl1R = g1R; current.t_gl1G = g1G; current.t_gl1B = g1B;
        current.t_gl2R = g2R; current.t_gl2G = g2G; current.t_gl2B = g2B;
    }

    void loadGeometry(float &cx, float &cy, float &spacing,
                      float &lw, float &rw, float &lh, float &rh,
                      float &mw, float &mh) {
        cx = current.cx; cy = current.cy; spacing = current.spacing;
        lw = current.lw; rw = current.rw;
        lh = current.lh; rh = current.rh;
        mw = current.mw; mh = current.mh;
    }

    void saveGeometry(float cx, float cy, float spacing,
                      float lw, float rw, float lh, float rh,
                      float mw, float mh) {
        current.cx = cx; current.cy = cy; current.spacing = spacing;
        current.lw = lw; current.rw = rw;
        current.lh = lh; current.rh = rh;
        current.mw = mw; current.mh = mh;
    }
}
