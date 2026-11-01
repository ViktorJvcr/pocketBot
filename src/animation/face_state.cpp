#include "face_state.h"

FacePalette FaceState::current;
FacePalette FaceState::target;

// Стартовые координаты под экран 160x80 (коэфф. 0.5 от старшего робота)
float FaceState::cx = 80.0f;
float FaceState::cy = 39.0f;
float FaceState::spacing = 50.0f;
float FaceState::lw = 25.0f;
float FaceState::lh = 40.0f;
float FaceState::lr = 12.0f;
float FaceState::rw = 25.0f;
float FaceState::rh = 40.0f;
float FaceState::rr = 12.0f;
float FaceState::mw = 8.0f;
float FaceState::mh = 2.0f;

uint16_t FaceState::rgb565(uint8_t r, uint8_t g, uint8_t b) {
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

void FaceState::init() {
    // Базовый неоново-зеленый (Idle)
    setTargetPalette(50, 255, 100,   10, 120, 30,   0, 40, 10);
    current = target;
    current.colorBg = 0x0000;
    updateColorLerp(1.0f); // Мгновенно применить на старте
}

void FaceState::setTargetPalette(
    uint8_t rE, uint8_t gE, uint8_t bE,
    uint8_t rG1, uint8_t gG1, uint8_t bG1,
    uint8_t rG2, uint8_t gG2, uint8_t bG2) 
{
    target.rEye = rE;     target.gEye = gE;     target.bEye = bE;
    target.rGlow1 = rG1;  target.gGlow1 = gG1;  target.bGlow1 = bG1;
    target.rGlow2 = rG2;  target.gGlow2 = gG2;  target.bGlow2 = bG2;
}

void FaceState::updateColorLerp(float speed) {
    current.rEye   += (target.rEye   - current.rEye)   * speed;
    current.gEye   += (target.gEye   - current.gEye)   * speed;
    current.bEye   += (target.bEye   - current.bEye)   * speed;

    current.rGlow1 += (target.rGlow1 - current.rGlow1) * speed;
    current.gGlow1 += (target.gGlow1 - current.gGlow1) * speed;
    current.bGlow1 += (target.bGlow1 - current.bGlow1) * speed;

    current.rGlow2 += (target.rGlow2 - current.rGlow2) * speed;
    current.gGlow2 += (target.gGlow2 - current.gGlow2) * speed;
    current.bGlow2 += (target.bGlow2 - current.bGlow2) * speed;

    current.colorEye   = rgb565((uint8_t)current.rEye,   (uint8_t)current.gEye,   (uint8_t)current.bEye);
    current.colorGlow1 = rgb565((uint8_t)current.rGlow1, (uint8_t)current.gGlow1, (uint8_t)current.bGlow1);
    current.colorGlow2 = rgb565((uint8_t)current.rGlow2, (uint8_t)current.gGlow2, (uint8_t)current.bGlow2);
    current.colorBg    = 0x0000;
}

void FaceState::prepareTransition(bool squint) {
    if (squint) {
        lh = 7.0f;
        rh = 7.0f;
        lr = 3.0f;
        rr = 3.0f;
    }
}

void FaceState::loadGeometry(float &outCx, float &outCy, float &outSp,
                             float &outLw, float &outLh, float &outLr,
                             float &outRw, float &outRh, float &outRr,
                             float &outMw, float &outMh) {
    outCx = cx; outCy = cy; outSp = spacing;
    outLw = lw; outLh = lh; outLr = lr;
    outRw = rw; outRh = rh; outRr = rr;
    outMw = mw; outMh = mh;
}

void FaceState::saveGeometry(float inCx, float inCy, float inSp,
                             float inLw, float inLh, float inLr,
                             float inRw, float inRh, float inRr,
                             float inMw, float inMh) {
    cx = inCx; cy = inCy; spacing = inSp;
    lw = inLw; lh = inLh; lr = inLr;
    rw = inRw; rh = inRh; rr = inRr;
    mw = inMw; mh = inMh;
}
