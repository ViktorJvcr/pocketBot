#include "prox_sensor.h"
#include <Wire.h>

namespace ProxSensor {
    namespace {
        constexpr uint8_t APDS_ADDR   = 0x39;
        constexpr uint8_t CMD_BIT     = 0xA0; // Auto-increment protocol
        
        constexpr uint8_t REG_ENABLE  = 0x00;
        constexpr uint8_t REG_ATIME   = 0x01;
        constexpr uint8_t REG_PTIME   = 0x02;
        constexpr uint8_t REG_PPULSE  = 0x0E;
        constexpr uint8_t REG_CONTROL = 0x0F;
        constexpr uint8_t REG_ID      = 0x12;
        constexpr uint8_t REG_PDATAL  = 0x18;

        bool connected = false;
        uint8_t devId = 0;
        uint16_t proxValue = 0;
        uint32_t lastUpdateMs = 0;

        void writeReg(uint8_t reg, uint8_t val) {
            Wire.beginTransmission(APDS_ADDR);
            Wire.write(CMD_BIT | reg);
            Wire.write(val);
            Wire.endTransmission(true);
        }

        uint8_t readReg(uint8_t reg) {
            Wire.beginTransmission(APDS_ADDR);
            Wire.write(CMD_BIT | reg);
            if (Wire.endTransmission(true) != 0) return 0xFF;
            Wire.requestFrom(APDS_ADDR, (uint8_t)1);
            return Wire.available() ? Wire.read() : 0xFF;
        }
    }

    bool init() {
        Wire.beginTransmission(APDS_ADDR);
        if (Wire.endTransmission(true) != 0) {
            connected = false;
            return false;
        }

        devId = readReg(REG_ID);
        writeReg(REG_ENABLE, 0x00);  // Выключаем перед настройкой
        writeReg(REG_ATIME, 0xFF);   // 2.7 ms
        writeReg(REG_PTIME, 0xFF);   // 2.7 ms
        writeReg(REG_PPULSE, 0x08);  // 8 импульсов ИК-диода (критично для клона!)
        writeReg(REG_CONTROL, 0x20); // 100mA LED drive, Proximity Gain 1x
        writeReg(REG_ENABLE, 0x07);  // Power ON (0x01) + ALS (0x02) + Proximity (0x04)

        connected = true;
        return true;
    }

    void update() {
        if (!connected) return;
        uint32_t now = millis();
        if (now - lastUpdateMs < 50) return; // Опрос 20 Гц
        lastUpdateMs = now;

        Wire.beginTransmission(APDS_ADDR);
        Wire.write(CMD_BIT | REG_PDATAL);
        if (Wire.endTransmission(true) == 0) {
            if (Wire.requestFrom(APDS_ADDR, (uint8_t)2) == 2) {
                uint8_t low = Wire.read();
                uint8_t high = Wire.read();
                proxValue = (uint16_t(high) << 8) | low;
            }
        }
    }

    uint16_t getProximity() { return proxValue; }
    uint8_t getDeviceId()   { return devId; }
    bool isConnected()      { return connected; }
}
