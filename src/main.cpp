#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>

#include "pins.h"
#include "config.h"
#include "display/display.h"
#include "hardware/touch_input.h"
#include "sensors/prox_sensor.h"
#include "sensors/imu_sensor.h"
#include "animation/face_state.h"
#include "animation/anim_idle.h"

// Аппаратный SPI (без передачи MOSI/SCLK в конструктор!)
Adafruit_ST7735 tft = Adafruit_ST7735(&SPI, TFT_CS_PIN, TFT_DC_PIN, TFT_RST_PIN);
PocketSprite sprite(SCREEN_W, SCREEN_H);

AnimIdle animIdle;

uint32_t lastFrameMs = 0;

void setup() {
    Serial.begin(115200);

    // 1. Инициализация аппаратного SPI и дисплея (40 МГц для 50+ FPS)
    SPI.begin(TFT_SCLK_PIN, -1, TFT_MOSI_PIN, TFT_CS_PIN);
    initDisplay(tft, sprite);
    tft.setSPISpeed(40000000);

    // 2. Общая шина I2C (400 кГц, пин 9 не трогаем)
    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
    Wire.setClock(400000);

    // 3. Инициализация сенсоров и кнопок
    TouchInput::init();
    bool proxOk = ProxSensor::init();
    bool imuOk  = ImuSensor::init();

    // 4. Инициализация базового состояния лица и сброс анимации Idle
    FaceState::init();
    animIdle.reset();

    Serial.println("=== POCKET ROBOT C3 STARTED ===");
    Serial.printf("APDS-9930: %s | BMI160: %s (0x%02X)\n",
                  proxOk ? "OK" : "FAIL",
                  imuOk  ? "OK" : "FAIL",
                  ImuSensor::getAddress());
}

void loop() {
    uint32_t now = millis();

    // Неблокирующий опрос датчиков I2C (внутри свои таймеры на millis)
    ProxSensor::update();
    ImuSensor::update();

    // Проверка детекторов IMU в Serial Monitor
    if (ImuSensor::isSuddenLiftDetected()) {
        Serial.println("[IMU] Sudden Lift / Jolt detected! (-> SCARED)");
    }
    if (ImuSensor::isShakeDetected()) {
        Serial.println("[IMU] Strong Shake detected! (-> DIZZY)");
    }

    // Рендер-цикл строго 50 Гц (каждые 20 мс)
    if (now - lastFrameMs >= 20) {
        lastFrameMs = now;

        // Плавный переход цветов (Color Lerp)
        FaceState::updateColorLerp();

        // Шаг анимации Idle с живым параллаксом от BMI160
        animIdle.runStep(sprite, ImuSensor::getParallaxX(), ImuSensor::getParallaxY());

        // Вывод буфера на экран без мерцания
        pushSpriteToDisplay(tft, sprite);
    }
}
