#include "imu_sensor.h"
#include <Wire.h>
#include <math.h>

namespace ImuSensor {
    namespace {
        uint8_t bmiAddr = 0x68;
        bool connected = false;
        uint32_t lastUpdateMs = 0;

        // Текущие физические величины
        float ax = 0.0f, ay = 0.0f, az = 1.0f;
        float gx = 0.0f, gy = 0.0f, gz = 0.0f;

        // Авто-калибровка нуля гироскопа (первые 40 выборок ~ 0.8 сек)
        bool calibrated = false;
        uint16_t calSamples = 0;
        float gxOffset = 0.0f, gyOffset = 0.0f, gzOffset = 0.0f;

        // Адаптивный горизонт (медленный LPF фильтр гравитации)
        float baseAx = 0.0f, baseAy = 0.0f, baseAz = 1.0f;

        // Итоговые координаты параллакса (для экрана 160x80)
        float parallaxX = 0.0f;
        float parallaxY = 0.0f;

        // Детекторы рывка (SCARED) и тряски (DIZZY)
        float shakeEnergy = 0.0f;
        bool liftFlag = false;
        bool shakeFlag = false;
        uint32_t lastLiftMs = 0;
        uint32_t lastShakeMs = 0;

        void writeReg(uint8_t reg, uint8_t val) {
            Wire.beginTransmission(bmiAddr);
            Wire.write(reg);
            Wire.write(val);
            Wire.endTransmission(true);
        }
    }

    bool init() {
        for (uint8_t addr : {0x68, 0x69}) {
            Wire.beginTransmission(addr);
            if (Wire.endTransmission(true) == 0) {
                bmiAddr = addr;
                connected = true;
                break;
            }
        }
        if (!connected) return false;

        // Пробуждение акселерометра (0x11) и гироскопа (0x15)
        writeReg(0x7E, 0x11);
        delay(10);
        writeReg(0x7E, 0x15);
        delay(60);

        calibrated = false;
        calSamples = 0;
        gxOffset = gyOffset = gzOffset = 0.0f;

        return true;
    }

    void update() {
        if (!connected) return;
        uint32_t now = millis();
        if (now - lastUpdateMs < 20) return; // Строго 50 Гц (20 мс)
        lastUpdateMs = now;

        Wire.beginTransmission(bmiAddr);
        Wire.write(0x0C); // Пакетное чтение 12 байт (Gyr + Acc)
        if (Wire.endTransmission(true) == 0) {
            if (Wire.requestFrom(bmiAddr, (uint8_t)12) == 12) {
                int16_t rawGx = Wire.read() | (Wire.read() << 8);
                int16_t rawGy = Wire.read() | (Wire.read() << 8);
                int16_t rawGz = Wire.read() | (Wire.read() << 8);
                int16_t rawAx = Wire.read() | (Wire.read() << 8);
                int16_t rawAy = Wire.read() | (Wire.read() << 8);
                int16_t rawAz = Wire.read() | (Wire.read() << 8);

                // Диапазон по умолчанию: ±2g (16384 LSB/g) и ±2000°/s (16.4 LSB/°/s)
                ax = rawAx / 16384.0f;
                ay = rawAy / 16384.0f;
                az = rawAz / 16384.0f;

                float curGx = rawGx / 16.4f;
                float curGy = rawGy / 16.4f;
                float curGz = rawGz / 16.4f;

                // 1. ФАЗА АВТО-КАЛИБРОВКИ ПРИ СТАРТЕ
                if (!calibrated) {
                    gxOffset += curGx;
                    gyOffset += curGy;
                    gzOffset += curGz;
                    baseAx += ax;
                    baseAy += ay;
                    baseAz += az;
                    calSamples++;

                    if (calSamples >= 40) {
                        gxOffset /= 40.0f;
                        gyOffset /= 40.0f;
                        gzOffset /= 40.0f;
                        baseAx   /= 40.0f;
                        baseAy   /= 40.0f;
                        baseAz   /= 40.0f;
                        calibrated = true;
                    }
                    return;
                }

                gx = curGx - gxOffset;
                gy = curGy - gyOffset;
                gz = curGz - gzOffset;

                // 2. АДАПТИВНЫЙ ГОРИЗОНТ (Медленный LPF подстраивается под новую позу покоя)
                baseAx += (ax - baseAx) * 0.025f;
                baseAy += (ay - baseAy) * 0.025f;
                baseAz += (az - baseAz) * 0.025f;

                // Отклонение от привычного горизонта
                float tiltX = ax - baseAx;
                float tiltY = ay - baseAy;

                // 3. РАСЧЕТ ПАРАЛЛАКСА (Масштаб 160x80: лимит ±14px по X, ±8px по Y)
                // Примечание: если оси датчика повернуты в корпусе, знаки/оси меняются здесь:
                float targetParX = (tiltY * 22.0f) - (gz * 0.08f);
                float targetParY = (tiltX * 16.0f) + (gy * 0.06f);

                targetParX = constrain(targetParX, -14.0f, 14.0f);
                targetParY = constrain(targetParY, -8.0f,  8.0f);

                // Плавный сглаживающий фильтр взгляда
                parallaxX += (targetParX - parallaxX) * 0.25f;
                parallaxY += (targetParY - parallaxY) * 0.25f;

                // 4. ДЕТЕКТОРЫ СОБЫТИЙ (Рывок / Свободное падение и Тряска)
                float accMag = sqrtf(ax * ax + ay * ay + az * az);
                float dynG   = fabsf(accMag - 1.0f); // Перегрузка относительно 1G
                float gyroMag = sqrtf(gx * gx + gy * gy + gz * gz);

                // Детектор резкого подъема / удара (кулдаун 2 сек)
                if (dynG > 0.65f && (now - lastLiftMs > 2000)) {
                    liftFlag = true;
                    lastLiftMs = now;
                }

                // Интегратор энергии тряски (для DIZZY)
                if (dynG > 0.35f || gyroMag > 180.0f) {
                    shakeEnergy += (dynG * 2.5f) + (gyroMag * 0.008f);
                } else {
                    shakeEnergy *= 0.88f; // Затухание в покое
                }

                if (shakeEnergy > 18.0f && (now - lastShakeMs > 4000)) {
                    shakeFlag = true;
                    shakeEnergy = 0.0f;
                    lastShakeMs = now;
                }
            }
        }
    }

    bool isConnected()   { return connected; }
    uint8_t getAddress() { return bmiAddr; }
    float getAccX()      { return ax; }
    float getAccY()      { return ay; }
    float getAccZ()      { return az; }
    float getGyrX()      { return gx; }
    float getGyrY()      { return gy; }
    float getGyrZ()      { return gz; }

    float getParallaxX() { return parallaxX; }
    float getParallaxY() { return parallaxY; }

    bool isSuddenLiftDetected() {
        if (liftFlag) {
            liftFlag = false;
            return true;
        }
        return false;
    }

    bool isShakeDetected() {
        if (shakeFlag) {
            shakeFlag = false;
            return true;
        }
        return false;
    }
}
