#include "imu_sensor.h"
#include <Wire.h>

namespace ImuSensor {
    namespace {
        uint8_t bmiAddr = 0x68;
        bool connected = false;
        uint32_t lastUpdateMs = 0;

        float ax = 0, ay = 0, az = 0;
        float gx = 0, gy = 0, gz = 0;

        void writeReg(uint8_t reg, uint8_t val) {
            Wire.beginTransmission(bmiAddr);
            Wire.write(reg);
            Wire.write(val);
            Wire.endTransmission(true);
        }
    }

    bool init() {
        // Автопоиск адреса 0x68 или 0x69
        for (uint8_t addr : {0x68, 0x69}) {
            Wire.beginTransmission(addr);
            if (Wire.endTransmission(true) == 0) {
                bmiAddr = addr;
                connected = true;
                break;
            }
        }
        if (!connected) return false;

        // Пробуждение акселерометра (0x11 в регистр CMD 0x7E)
        writeReg(0x7E, 0x11);
        delay(10); // Небольшая пауза при старте питания допустима в setup()
        // Пробуждение гироскопа (0x15 в регистр CMD 0x7E)
        writeReg(0x7E, 0x15);
        delay(60);

        return true;
    }

    void update() {
        if (!connected) return;
        uint32_t now = millis();
        if (now - lastUpdateMs < 20) return; // 50 Гц, как на большом роботе
        lastUpdateMs = now;

        Wire.beginTransmission(bmiAddr);
        Wire.write(0x0C); // Начало пакета Gyr + Acc (12 байт)
        if (Wire.endTransmission(true) == 0) {
            if (Wire.requestFrom(bmiAddr, (uint8_t)12) == 12) {
                int16_t rawGx = Wire.read() | (Wire.read() << 8);
                int16_t rawGy = Wire.read() | (Wire.read() << 8);
                int16_t rawGz = Wire.read() | (Wire.read() << 8);
                int16_t rawAx = Wire.read() | (Wire.read() << 8);
                int16_t rawAy = Wire.read() | (Wire.read() << 8);
                int16_t rawAz = Wire.read() | (Wire.read() << 8);

                // По умолчанию BMI160 после старта в режиме ±2g (16384 LSB/g)
                ax = rawAx / 16384.0f;
                ay = rawAy / 16384.0f;
                az = rawAz / 16384.0f;
                gx = rawGx / 16.4f;
                gy = rawGy / 16.4f;
                gz = rawGz / 16.4f;
            }
        }
    }

    bool isConnected()  { return connected; }
    uint8_t getAddress(){ return bmiAddr; }
    float getAccX()     { return ax; }
    float getAccY()     { return ay; }
    float getAccZ()     { return az; }
}
