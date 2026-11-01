#include "anim_idle.h"
#include "face_state.h"
#include <math.h>

void AnimIdle::reset() {
    // Подхватываем текущую геометрию для бесшовного морфинга
    FaceState::loadGeometry(cx, cy, currentSpacing, lw, lh, lr, rw, rh, rr, mw, mh);

    // Задаем фирменную неоново-зеленую палитру Idle
    FaceState::setTargetPalette(
        50, 255, 100,  // Ядро (Яркий неон)
        10, 120, 30,   // Glow 1 (Средний ореол)
        0,  40,  10    // Glow 2 (Внешняя дымка)
    );

    // Базовые цели (Масштаб 160x80)
    targetX = 80.0f;
    targetY = 38.0f;
    targetSpacing = 50.0f;

    targetLW = 25.0f; targetLH = 40.0f; targetLR = 12.0f;
    targetRW = 25.0f; targetRH = 40.0f; targetRR = 12.0f;
    targetMW = 8.0f;  targetMH = 2.0f;

    unsigned long now = millis();
    lastSaccadeTime = now;
    nextSaccadeInterval = random(1800, 3500);

    lastBlinkTime = now;
    nextBlinkInterval = random(2500, 5000);
    isBlinking = false;
    pendingDoubleBlink = false;
}

void AnimIdle::drawGlowRoundRect(PocketSprite& sprite, int16_t x, int16_t y, int16_t w, int16_t h, int16_t r,
                                 uint16_t cEye, uint16_t cG1, uint16_t cG2) 
{
    if (w < 2) w = 2;
    if (h < 2) h = 2;
    int16_t maxR = (w < h ? w : h) / 2;
    if (r > maxR) r = maxR;

    // 1. Внешнее свечение Glow 2 (+3px во все стороны)
    sprite.fillRoundRect(x - 3, y - 3, w + 6, h + 6, r + 3, cG2);
    // 2. Среднее свечение Glow 1 (+1px во все стороны)
    sprite.fillRoundRect(x - 1, y - 1, w + 2, h + 2, r + 1, cG1);
    // 3. Яркое ядро
    sprite.fillRoundRect(x, y, w, h, r, cEye);
}

void AnimIdle::runStep(PocketSprite& sprite, float parX, float parY) {
    unsigned long now = millis();

    // ---------------------------------------------------------
    // 1. ГЕНЕРАТОР САККАД (Живые взгляды по сторонам)
    // ---------------------------------------------------------
    if (now - lastSaccadeTime >= nextSaccadeInterval) {
        lastSaccadeTime = now;
        nextSaccadeInterval = random(1400, 4200);

        int roll = random(0, 100);
        if (roll < 45) {
            // 45% времени: Спокойный взгляд прямо
            targetX = 80.0f;
            targetY = 38.0f;
            targetSpacing = 50.0f;
            targetLW = 25.0f; targetLH = 40.0f; targetLR = 12.0f;
            targetRW = 25.0f; targetRH = 40.0f; targetRR = 12.0f;
            targetMW = 8.0f;  targetMH = 2.0f;
        } 
        else if (roll < 80) {
            // 35% времени: Взгляд в сторону с 3D-перспективой
            float lookX = (float)random(-14, 15);
            float lookY = (float)random(-6, 7);
            targetX = 80.0f + lookX;
            targetY = 38.0f + lookY;
            targetSpacing = 48.0f;

            // Тот глаз, в сторону которого смотрим, кажется чуть больше
            if (lookX > 3.0f) {
                targetLW = 23.0f; targetLH = 36.0f; targetLR = 11.0f;
                targetRW = 26.0f; targetRH = 42.0f; targetRR = 12.0f;
            } else if (lookX < -3.0f) {
                targetLW = 26.0f; targetLH = 42.0f; targetLR = 12.0f;
                targetRW = 23.0f; targetRH = 36.0f; targetRR = 11.0f;
            } else {
                targetLW = 25.0f; targetLH = 40.0f; targetLR = 12.0f;
                targetRW = 25.0f; targetRH = 40.0f; targetRR = 12.0f;
            }
            targetMW = 7.0f; targetMH = 2.0f;
        } 
        else {
            // 20% времени: Любопытство (один глаз чуть прищурен, ротик "о")
            targetX = 80.0f + (float)random(-8, 9);
            targetY = 37.0f;
            targetSpacing = 51.0f;
            if (random(0, 2) == 0) {
                targetLW = 25.0f; targetLH = 32.0f; targetLR = 10.0f;
                targetRW = 26.0f; targetRH = 42.0f; targetRR = 12.0f;
            } else {
                targetLW = 26.0f; targetLH = 42.0f; targetLR = 12.0f;
                targetRW = 25.0f; targetRH = 32.0f; targetRR = 10.0f;
            }
            targetMW = 5.0f; targetMH = 4.0f; // Удивленный маленький ротик
        }
    }

    // ---------------------------------------------------------
    // 2. СИСТЕМА МОРГАНИЯ (Одиночное и двойное)
    // ---------------------------------------------------------
    if (!isBlinking && (now - lastBlinkTime >= nextBlinkInterval)) {
        isBlinking = true;
        blinkStartTime = now;
        // 25% шанс сделать двойное моргание
        if (!pendingDoubleBlink && random(0, 100) < 25) {
            pendingDoubleBlink = true;
        } else {
            pendingDoubleBlink = false;
        }
    }

    float curTargetLH = targetLH;
    float curTargetRH = targetRH;
    float curTargetLR = targetLR;
    float curTargetRR = targetRR;

    if (isBlinking) {
        unsigned long blinkElapsed = now - blinkStartTime;
        if (blinkElapsed < 90) {
            // Фаза быстрого закрытия век
            curTargetLH = 3.0f;
            curTargetRH = 3.0f;
            curTargetLR = 1.0f;
            curTargetRR = 1.0f;
        } else {
            // Открываем глаза обратно
            isBlinking = false;
            lastBlinkTime = now;
            if (pendingDoubleBlink) {
                nextBlinkInterval = 140; // Короткая пауза перед вторым взмахом ресниц
            } else {
                nextBlinkInterval = random(2200, 5500);
            }
        }
    }

    // ---------------------------------------------------------
    // 3. ПРОЦЕДУРНОЕ ДЫХАНИЕ + ПАРАЛЛАКС IMU + ПРУЖИННАЯ ФИЗИКА
    // ---------------------------------------------------------
    float breathY = sinf(now * 0.0025f) * 1.5f;   // Плавный вдох-выдох (±1.5px)
    float swayX   = cosf(now * 0.0012f) * 0.8f;   // Едва заметное покачивание

    float finalTargetX = targetX + swayX + parX;
    float finalTargetY = targetY + breathY + parY;

    // Ограничиваем координаты, чтобы глаза с ореолом не вылезали за экран 160x80
    if (finalTargetX < 44.0f)  finalTargetX = 44.0f;
    if (finalTargetX > 116.0f) finalTargetX = 116.0f;
    if (finalTargetY < 24.0f)  finalTargetY = 24.0f;
    if (finalTargetY > 52.0f)  finalTargetY = 52.0f;

    // Пружинная интерполяция (в czasie моргания веки двигаются быстрее: 0.45f)
    float eyeHSpeed = isBlinking ? 0.48f : 0.22f;

    cx             += (finalTargetX  - cx)             * 0.16f;
    cy             += (finalTargetY  - cy)             * 0.16f;
    currentSpacing += (targetSpacing - currentSpacing) * 0.15f;

    lw += (targetLW    - lw) * 0.20f;
    lh += (curTargetLH - lh) * eyeHSpeed;
    lr += (curTargetLR - lr) * 0.25f;

    rw += (targetRW    - rw) * 0.20f;
    rh += (curTargetRH - rh) * eyeHSpeed;
    rr += (curTargetRR - rr) * 0.25f;

    mw += (targetMW    - mw) * 0.18f;
    mh += (targetMH    - mh) * 0.18f;

    // Сохраняем геометрию в FaceState для будущих бесшовных переходов
    FaceState::saveGeometry(cx, cy, currentSpacing, lw, lh, lr, rw, rh, rr, mw, mh);

    // ---------------------------------------------------------
    // 4. РЕНДЕР КАДРА (160x80, 3-Layer Glow)
    // ---------------------------------------------------------
    uint16_t cEye = FaceState::current.colorEye;
    uint16_t cG1  = FaceState::current.colorGlow1;
    uint16_t cG2  = FaceState::current.colorGlow2;
    uint16_t cBg  = FaceState::current.colorBg;

    sprite.fillSprite(cBg);

    // Координаты центров левого и правого глаза
    int16_t leftCenterX  = (int16_t)(cx - currentSpacing * 0.5f);
    int16_t rightCenterX = (int16_t)(cx + currentSpacing * 0.5f);
    int16_t eyeCenterY   = (int16_t)cy;

    // Левый глаз
    int16_t lx = leftCenterX - (int16_t)(lw * 0.5f);
    int16_t ly = eyeCenterY  - (int16_t)(lh * 0.5f);
    drawGlowRoundRect(sprite, lx, ly, (int16_t)lw, (int16_t)lh, (int16_t)lr, cEye, cG1, cG2);

    // Правый глаз
    int16_t rx = rightCenterX - (int16_t)(rw * 0.5f);
    int16_t ry = eyeCenterY   - (int16_t)(rh * 0.5f);
    drawGlowRoundRect(sprite, rx, ry, (int16_t)rw, (int16_t)rh, (int16_t)rr, cEye, cG1, cG2);

    // Рот (находится ниже линии глаз на 23px, чуть отстает по горизонтали для эффекта глубины)
    int16_t mouthCenterX = (int16_t)(cx + (cx - 80.0f) * 0.15f);
    int16_t mouthCenterY = (int16_t)(cy + 23.0f);
    int16_t mx = mouthCenterX - (int16_t)(mw * 0.5f);
    int16_t my = mouthCenterY - (int16_t)(mh * 0.5f);
    int16_t mr = (int16_t)(mh * 0.5f);

    drawGlowRoundRect(sprite, mx, my, (int16_t)mw, (int16_t)mh, mr, cEye, cG1, cG2);
}
